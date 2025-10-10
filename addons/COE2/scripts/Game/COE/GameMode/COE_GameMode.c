//------------------------------------------------------------------------------------------------
//! The different phases for the game mode
enum COE_EGameModeState
{
	INTERMISSION, // Phase between missions where commander sets up the next objective
	BRIEFING, // Phase after AO has been generated. Players can discuss the execution and commander can select insertion points
	EXECUTION, // Phase when an AO is active
	EXFILTRATION, // Phase when all tasks have been completed and the players have to leave the AO
	DEBRIEFING // Unused; Might become a specific phase where player stats and achievements are listed
}

//------------------------------------------------------------------------------------------------
//! Types of requests that can be done by the commander via COE_EditorModeCommanderEntity.Request
enum COE_ECommanderRequest
{
	GENERATE_AO, // Request to generate an AO
	CANCEL_AO, // Request to cancel current AO
	END_BRIEFING, // Request to end the briefing and start execution
	INSERTION_POINT // Request to set an insertion point at the specified position
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(EMapDescriptorType, "m_iType")]
class COE_MapDescriptorType
{
	[Attribute(uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(EMapDescriptorType))]
	protected EMapDescriptorType m_iType;
	
	bool IsType(IEntity entity)
	{
		MapDescriptorComponent mapDescriptor = MapDescriptorComponent.Cast(entity.FindComponent(MapDescriptorComponent));
		if (!mapDescriptor)
			return false;
		
		return mapDescriptor.Item().GetBaseType() == m_iType;
	}
}

//------------------------------------------------------------------------------------------------
class COE_GameModeClass : SCR_BaseGameModeClass
{
}

//------------------------------------------------------------------------------------------------
class COE_GameMode : SCR_BaseGameMode
{
	[Attribute(desc: "Location types to be excluded", category: "Locations")]
	protected ref array<ref COE_MapDescriptorType> m_aBlacklistedDescriptorTypes;
	
	[Attribute(desc: "Available task builders", category: "Tasks")]
	protected ref array<ref COE_BaseTaskBuilder> m_aAvailableTaskBuilders;
	
	[Attribute(defvalue: "", desc: "Default faction key for the players", category: "Default Scenario Properties")]
	protected FactionKey m_sDefaultPlayerFactionKey;
	
	[Attribute(defvalue: "", desc: "Default faction key of the enemy AI", category: "Default Scenario Properties")]
	protected FactionKey m_sDefaultEnemyFactionKey;
	
	[Attribute(defvalue: "CIV", desc: "Default faction key of the civilian AI", category: "Default Scenario Properties")]
	protected FactionKey m_sDefaultCivilianFactionKey;
	
	[RplProp(), Attribute(defvalue: "250", desc: "Radius of AO", category: "Default Scenario Properties")]
	protected float m_fAORadius;
	
	[Attribute(defvalue: "50", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(EAISkill), desc: "Enemy AI Skill Level", category: "Default Scenario Properties")]
	protected EAISkill m_eEnemyAISkill;
	
	[Attribute(defvalue: "30", desc: "Minimal enemy AI count for an AO", category: "Default Scenario Properties")]
	protected int m_iMinEnemyAICount;
	
	[Attribute(defvalue: "2.5", desc: "Total enemy AI count for an AO will be this multiplier times the total player count (ignored when below m_iMinEnemyAICount)", category: "Default Scenario Properties")]
	protected float m_fEnemyAICountMultiplier;
	
	[Attribute(defvalue: "2", desc: "Default number of enemy armed vehicles for an AO.", category: "Default Scenario Properties")]
	protected int m_iEnemyArmedVehicleCount;
	
	[Attribute(defvalue: "true", desc: "Whether enemy can support their AOs.", category: "Default Support Settings")]
	bool m_bEnemySupportEnabled;
	
	[Attribute(defvalue: "0", desc: "Number of enemy mortars for support.", category: "Default Support Settings")]
	int m_iEnemyMortarCount;
	
	[Attribute(defvalue: "5", desc: "Minimum time in minutes required for the enemy to send reinforcements.", category: "Default Support Settings")]
	float m_fMinEnemyReinforcementTime;
	
	[Attribute(defvalue: "15", desc: "Maximum time in minutes required for the enemy to send reinforcements.", category: "Default Support Settings")]
	float m_fMaxEnemyReinforcementTime;
	
	[Attribute(defvalue: "true", desc: "Whether civilians spawn on the AOs", category: "Default Scenario Properties", category: "Default Scenario Properties")]
	protected bool m_bCiviliansEnabled;
	
	[Attribute(defvalue: "false", desc: "Whether a voted in commander also becomes GM", category: "Default Scenario Properties")]
	protected bool m_bCommanderBecomesGM;
	
	[RplProp()]
	protected vector m_vMainBasePos;
	protected COE_MainBaseEntity m_pMainBase;
	
	[RplProp()]
	protected ref COE_AOParams m_pCurrentAOParams = new COE_AOParams();
	protected ref COE_AOParams m_pNextAOParams = new COE_AOParams();
	protected COE_AO m_pCurrentAO;
	
	[RplProp(onRplName: "OnInsertionPointUpdated")]
	protected RplId m_iInsertionPointId;
	protected IEntity m_pInsertionPoint;
	
	[RplProp(onRplName: "COE_OnStateChanged")]
	protected COE_EGameModeState m_eCOE_CurrentState = COE_EGameModeState.INTERMISSION;
	protected ref ScriptInvoker m_pCOE_OnStateChanged = new ScriptInvoker();
	
	protected COE_FactionManager m_pFactionManager;
	protected ref array<IEntity> m_aEntitiesToDelete = {};
	
	protected const float MAIN_BASE_RANGE = 25;
		
	//------------------------------------------------------------------------------------------------
	void COE_GameMode(IEntitySource src, IEntity parent)
	{
		SCR_RespawnComponent.SGetOnLocalPlayerSpawned().Insert(ShowDocumentation);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		// Read mission header
		COE_MissionHeader header = COE_MissionHeader.Cast(GetGame().GetMissionHeader());
		if (header)
		{
			if (!header.m_sCOE_DefaultPlayerFactionKey.IsEmpty())
				m_sDefaultPlayerFactionKey = header.m_sCOE_DefaultPlayerFactionKey;
			
			if (!header.m_sCOE_DefaultEnemyFactionKey.IsEmpty())
				m_sDefaultEnemyFactionKey = header.m_sCOE_DefaultEnemyFactionKey;
			
			if (!header.m_sCOE_DefaultCivilianFactionKey.IsEmpty())
				m_sDefaultCivilianFactionKey = header.m_sCOE_DefaultCivilianFactionKey;
			
			if (header.m_eCOE_DefaultEnemyAiSkill != EAISkill.NONE)
				m_eEnemyAISkill = header.m_eCOE_DefaultEnemyAiSkill;
			
			if (header.m_fCOE_DefaultAORadius > 0)
				m_fAORadius = header.m_fCOE_DefaultAORadius;
			
			if (header.m_iCOE_DefaultMinEnemyAICount >= 0)
				m_iMinEnemyAICount = header.m_iCOE_DefaultMinEnemyAICount;
			
			if (header.m_fCOE_DefaultEnemyAICountMultiplier >= 0)
				m_fEnemyAICountMultiplier = header.m_fCOE_DefaultEnemyAICountMultiplier;
			
			m_iEnemyArmedVehicleCount = header.m_iCOE_EnemyArmedVehicleCount;
			m_bEnemySupportEnabled = header.m_bCOE_EnemySupportEnabled;
			m_iEnemyMortarCount = header.m_iCOE_EnemyMortarCount;
			m_fMinEnemyReinforcementTime = header.m_fCOE_MinEnemyReinforcementTime;
			m_fMaxEnemyReinforcementTime = header.m_fCOE_MaxEnemyReinforcementTime;
			m_bCiviliansEnabled = header.m_bCOE_CiviliansEnabled;
			m_bCommanderBecomesGM = header.m_bCOE_CommanderBecomesGM;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static COE_GameMode GetInstance()
	{
		return COE_GameMode.Cast(GetGame().GetGameMode());
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnGameStart()
	{
		super.OnGameStart();
		
		array<IEntity> bases = {};
		KSC_WorldTools.GetEntitiesByType(bases, COE_MainBaseEntity);
		if (bases.IsEmpty())
			return;
		
		m_pMainBase = COE_MainBaseEntity.Cast(bases[Math.RandomInt(0, bases.Count())]);
		m_vMainBasePos = m_pMainBase.GetOrigin();
		Replication.BumpMe();
		
		m_pFactionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		if (!m_pFactionManager)
			return;
		
		if (!m_sDefaultPlayerFactionKey.IsEmpty())
			m_pFactionManager.SetPlayerFaction(SCR_Faction.Cast(m_pFactionManager.GetFactionByKey(m_sDefaultPlayerFactionKey)), true);
		
		if (!m_sDefaultEnemyFactionKey.IsEmpty())
			m_pFactionManager.SetEnemyFaction(SCR_Faction.Cast(m_pFactionManager.GetFactionByKey(m_sDefaultEnemyFactionKey)));
		
		if (!m_sDefaultCivilianFactionKey.IsEmpty())
			m_pFactionManager.SetCivilianFaction(SCR_Faction.Cast(m_pFactionManager.GetFactionByKey(m_sDefaultCivilianFactionKey)));
		
		KSC_GameTools.SetAISkill(m_eEnemyAISkill);
		
		COE_SetState(COE_EGameModeState.INTERMISSION);
		
		// Make server host commander
		int playerID = SCR_PlayerController.GetLocalPlayerId();
		if (playerID > 0)
			GetGame().GetPlayerManager().GivePlayerRole(playerID, EPlayerRole.COE_COMMANDER);
	}
	
	//------------------------------------------------------------------------------------------------
	COE_MainBaseEntity GetMainBase()
	{
		return m_pMainBase;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Synced property
	vector GetMainBasePos()
	{
		return m_vMainBasePos;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref COE_BaseTaskBuilder> GetAvailableTaskBuilders()
	{
		return m_aAvailableTaskBuilders;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref KSC_Location> GetAvailableLocations()
	{
		return m_pKSC_WorldSlotsConfig.m_aLocations;
	}
	
	//------------------------------------------------------------------------------------------------
	COE_AOParams GetNextAOParams()
	{
		return m_pNextAOParams;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetNextAOParams(COE_AOParams params)
	{
		m_pNextAOParams.Copy(params);
	}
	
	//------------------------------------------------------------------------------------------------
	KSC_Location GetCurrentLocation()
	{
		if (m_pCurrentAOParams.m_iLocationIdx < 0)
			return null;
		
		return GetAvailableLocations()[m_pCurrentAOParams.m_iLocationIdx];
	}
	
	//------------------------------------------------------------------------------------------------
	COE_AOParams GetCurrentAOParams()
	{
		return m_pCurrentAOParams;
	}
	
	//------------------------------------------------------------------------------------------------
	void ExecuteCommanderRequest(COE_ECommanderRequest request, vector pos = vector.Zero)
	{
		switch (request)
		{
			case COE_ECommanderRequest.GENERATE_AO: { GenerateAO(); break; };
			case COE_ECommanderRequest.CANCEL_AO: { DeleteAO(); break; };
			case COE_ECommanderRequest.END_BRIEFING: { StartAO(); break; };
			case COE_ECommanderRequest.INSERTION_POINT: { CreateInsertionPoint(pos); break; };
		};
	}
	
	//------------------------------------------------------------------------------------------------
	COE_EGameModeState COE_GetState()
	{
		return m_eCOE_CurrentState;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void COE_SetState(COE_EGameModeState state)
	{
		m_eCOE_CurrentState = state;
				
		switch (state)
		{
			case COE_EGameModeState.INTERMISSION:
			{
				// Select random mission and objective
				m_pNextAOParams.m_eTaskTypes = 1 << Math.RandomInt(0, m_aAvailableTaskBuilders.Count());
				array<ref KSC_Location> locations = GetAvailableLocations();
				m_pNextAOParams.m_iLocationIdx = Math.RandomInt(0, locations.Count());
				
				// Clear old AO;
				DeleteInsertionPoint();
				m_pCurrentAOParams.Clear();
				break;
			}
			//case COE_EGameModeState.BRIEFING:
			case COE_EGameModeState.EXECUTION:
			{
				// Copy AO params
				m_pCurrentAOParams.Copy(m_pNextAOParams);
				break;
			}
		};
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void GenerateAO()
	{
		//COE_SetState(COE_EGameModeState.BRIEFING);
		COE_SetState(COE_EGameModeState.EXECUTION);
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = GetCurrentLocation().m_vCenter;
		SCR_TerrainHelper.SnapToTerrain(params.Transform);
		m_pCurrentAO = COE_AO.Cast(GetGame().SpawnEntity(COE_AO, GetWorld(), params));
		
		if (m_bEnemySupportEnabled)
			COE_EnemySupportSystem.GetInstance().Register(m_pCurrentAO);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void StartAO()
	{
		COE_SetState(COE_EGameModeState.EXECUTION);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DeleteAO()
	{
		COE_SetState(COE_EGameModeState.INTERMISSION);
		
		if (m_bEnemySupportEnabled)
			COE_EnemySupportSystem.GetInstance().Unregister(m_pCurrentAO);
		
		// Heal and teleport all players to main base
		array<int> playerIds = {};
		GetGame().GetPlayerManager().GetPlayers(playerIds);
		
		// Generate a grid for players to be teleported to
		array<vector> targetPositions = {};
		targetPositions.Reserve(playerIds.Count());
		int nGrid = Math.Ceil(Math.Sqrt(playerIds.Count()));
		float offset = 0.3 * Math.Floor(nGrid / 2);
		vector startPos = m_vMainBasePos - Vector(offset, 0, offset);
		
        for (int i = 0; i < nGrid; ++i)
		{
			for (int j = 0; j < nGrid; ++j)
			{
				targetPositions.Insert(startPos + 0.3 * Vector(i, 0, j));
			}
		}
		

		foreach (int i, int playerId : playerIds)
		{
			SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
			if (!player)
				continue;
			
			CharacterControllerComponent charCtrl = player.GetCharacterController();
			if (charCtrl.GetLifeState() == ECharacterLifeState.DEAD)
				continue;
			
			COE_PlayerController playerCtrl = COE_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
			playerCtrl.RequestFastTravel(targetPositions[i], 0, 5);
			
			SCR_DamageManagerComponent damageManager = player.GetDamageManager();
			damageManager.FullHeal();
		}
		
		CollectBuiltEntitiesForCleanUp();
		GetGame().GetCallqueue().CallLater(SCR_EntityHelper.DeleteEntityAndChildren, 3000, false, m_pCurrentAO);
		GetGame().GetCallqueue().CallLater(CleanUpBuiltEntities, 4000);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Delete all AI outside main base
	void CollectBuiltEntitiesForCleanUp()
	{
		SCR_EditableEntityCore core = SCR_EditableEntityCore.Cast(SCR_EditableEntityCore.GetInstance(SCR_EditableEntityCore));
		set<SCR_EditableEntityComponent> entities = new set<SCR_EditableEntityComponent>();
		core.GetAllEntities(entities, true, true);
		m_aEntitiesToDelete.Resize(entities.Count());
		
		foreach (SCR_EditableEntityComponent entity : entities)
		{
			if (SCR_EditableVehicleComponent.Cast(entity) || SCR_EditableGroupComponent.Cast(entity))
				m_aEntitiesToDelete.Insert(entity.GetOwner());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Delete all AI outside main base
	void CleanUpBuiltEntities()
	{
		foreach (IEntity entity : m_aEntitiesToDelete)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(entity);
		}
		
		m_aEntitiesToDelete.Clear();
	}
		
	//------------------------------------------------------------------------------------------------
	protected void CreateInsertionPoint(vector pos)
	{
		if (KSC_TerrainHelper.SurfaceIsWater(pos))
		{
			EWaterSurfaceType surfaceType;
			float area;
			pos[1] =  SCR_WorldTools.GetWaterSurfaceY(GetWorld(), pos, surfaceType, area);
		}
		else
		{
			pos[1] = SCR_TerrainHelper.GetTerrainY(pos);
		}
		
		vector dir = (GetCurrentLocation().m_vCenter - pos);
		
		if (!m_pInsertionPoint)
		{
			m_pInsertionPoint = KSC_GameTools.SpawnSpawnPointPrefab("{A99028D5E6D1B04D}PrefabsEditable/SpawnPoints/E_COE_InsertionPoint.et", pos, dir.ToYaw());
			RplComponent rpl = RplComponent.Cast(m_pInsertionPoint.FindComponent(RplComponent));
			m_iInsertionPointId = rpl.Id();
			Replication.BumpMe();	
		}
		else
		{
			// Reuse the existing point by moving it to the new position
			SCR_EditableSpawnPointComponent editable = SCR_EditableSpawnPointComponent.Cast(m_pInsertionPoint.FindComponent(SCR_EditableSpawnPointComponent));
			vector transform[4];
			KSC_GameTools.GetTransformFromPosAndRot(transform, pos, dir.ToYaw());
			editable.SetTransform(transform);
			editable.UpdateNearestLocation(pos);
		};
		
		SCR_FactionAffiliationComponent factionAffiliation = SCR_FactionAffiliationComponent.Cast(m_pInsertionPoint.FindComponent(SCR_FactionAffiliationComponent));
		factionAffiliation.SetAffiliatedFaction(m_pFactionManager.GetPlayerFaction());
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DeleteInsertionPoint()
	{
		SCR_EntityHelper.DeleteEntityAndChildren(m_pInsertionPoint);
		m_pInsertionPoint = null;
		m_iInsertionPointId = Replication.INVALID_ID;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetInsertionPoint()
	{
		return m_pInsertionPoint;
	}
	
	//------------------------------------------------------------------------------------------------
	COE_AO GetCurrentAO()
	{
		return m_pCurrentAO;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetAORadius(float radius)
	{
		m_fAORadius = radius;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetAORadius()
	{
		return m_fAORadius;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetMinEnemyAiCount(int count)
	{
		m_iMinEnemyAICount = count;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetMinEnemyAiCount()
	{
		return m_iMinEnemyAICount;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetEnemyAiCountMultiplier(float multiplier)
	{
		m_fEnemyAICountMultiplier = multiplier;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetEnemyAiCountMultiplier()
	{
		return m_fEnemyAICountMultiplier;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetTargetEnemyAICount()
	{
		return Math.Max(m_fEnemyAICountMultiplier * GetGame().GetPlayerManager().GetPlayerCount(), m_iMinEnemyAICount);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetEnemyArmedVehicleCount(int count)
	{
		m_iEnemyArmedVehicleCount = count;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetEnemyArmedVehicleCount()
	{
		return m_iEnemyArmedVehicleCount;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetEnemySupportEnabled(bool enable)
	{
		m_bEnemySupportEnabled = enable;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsEnemySupportEnabled()
	{
		return m_bEnemySupportEnabled;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetEnemyMortarCount(int count)
	{
		m_iEnemyMortarCount = count;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetEnemyMortarCount()
	{
		return m_iEnemyMortarCount;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetMinEnemyReinforcementTime(float time)
	{
		m_fMinEnemyReinforcementTime = time;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetMinEnemyReinforcementTime()
	{
		return m_fMinEnemyReinforcementTime;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetMaxEnemyReinforcementTime(float time)
	{
		m_fMaxEnemyReinforcementTime = time;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetMaxEnemyReinforcementTime()
	{
		return m_fMaxEnemyReinforcementTime;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCiviliansEnabled(bool enabled)
	{
		m_bCiviliansEnabled = enabled;
	}	
	//------------------------------------------------------------------------------------------------
	bool CiviliansEnabled()
	{
		return m_bCiviliansEnabled;
	}

	//------------------------------------------------------------------------------------------------
	bool CommanderBecomesGM()
	{
		return m_bCommanderBecomesGM;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnInsertionPointUpdated()
	{
		if (Replication.IsServer())
			return;
		
		GetGame().GetCallqueue().CallLater(OnInsertionPointUpdatedDelayed, 1000);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnInsertionPointUpdatedDelayed()
	{
		RplComponent rpl = RplComponent.Cast(Replication.FindItem(m_iInsertionPointId));
		if (!rpl)
		{
			m_pInsertionPoint = null;
			return;
		}
		
		m_pInsertionPoint = rpl.GetEntity();
	}	
	
	//------------------------------------------------------------------------------------------------
	protected void COE_OnStateChanged()
	{
		m_pCOE_OnStateChanged.Invoke(m_eCOE_CurrentState);
	}
	
	//------------------------------------------------------------------------------------------------
	ScriptInvoker COE_GetOnStateChanged()
	{
		return m_pCOE_OnStateChanged;
	}
	
	//------------------------------------------------------------------------------------------------
	//! If player becomes admin or commander, they should get access to the commander editor
	protected override void OnPlayerRoleChange(int playerId, EPlayerRole roleFlags)
	{
		super.OnPlayerRoleChange(playerId, roleFlags);
		
		if (Replication.IsClient())
			return;
					
		
		COE_PlayerController playerController = COE_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return;
		
		bool wasCommanderOrAdmin = (playerController.GetPlayerRoles() & (EPlayerRole.COE_COMMANDER | EPlayerRole.ADMINISTRATOR));
		bool becomesCommanderOrAdmin = (roleFlags & (EPlayerRole.COE_COMMANDER | EPlayerRole.ADMINISTRATOR));
		// Check if commander flag was changed
		if (wasCommanderOrAdmin != becomesCommanderOrAdmin)
		{
			SCR_EditorManagerCore core = SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));
			SCR_EditorManagerEntity editorManager = core.GetEditorManager(playerId);
			
			EEditorMode modes = EEditorMode.COE_COMMANDER;
			if (m_bCommanderBecomesGM)
				modes |= EEditorMode.EDIT;
			
			if (becomesCommanderOrAdmin)
				editorManager.AddEditorModes(EEditorModeAccess.BASE, modes);
			else
				editorManager.RemoveEditorModes(EEditorModeAccess.BASE, modes);
		}
		
		playerController.OnPlayerRoleChangeServer(roleFlags);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ShowDocumentation()
	{
		SCR_FieldManualUI.Open(EFieldManualEntryId.COE_COMMANDER);
		SCR_RespawnComponent.SGetOnLocalPlayerSpawned().Remove(ShowDocumentation);
	}
}

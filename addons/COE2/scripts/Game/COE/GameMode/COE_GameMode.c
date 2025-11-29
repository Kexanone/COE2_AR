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
	INSERTION_POINT, // Request to set an insertion point at the specified position
	EXFIL_POINT, // Request to set an exfil point at the specified position
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
	[Attribute(desc: "Available task builders", category: "Tasks")]
	protected ref array<ref COE_BaseTaskBuilder> m_aAvailableTaskBuilders;
	
	[Attribute(defvalue: "{73F871FEDE53CD1F}Prefabs/Tasks/KSC_ExfilTask.et", desc: "Prefab of the exfil task", params: "et", category: "Tasks")]
	protected ResourceName m_sExfilTaskPrefabName;
	
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
	
	protected ref array<ref KSC_Location> m_aAvailableLocations;
	
	[RplProp()]
	protected vector m_vMainBasePos;
	protected COE_MainBaseEntity m_pMainBase;
	
	[RplProp()]
	protected ref array<ref COE_AOParams> m_aNextAOParams = {};
	
	protected ref array<COE_AO> m_aCurrentAOs = {};
	
	[RplProp(onRplName: "OnInsertionPointUpdated")]
	protected RplId m_iInsertionPointId;
	protected SCR_SpawnPoint m_pInsertionPoint;
	
	[RplProp()]
	protected bool m_bHasExfilPoint = false;
	
	[RplProp(onRplName: "COE_OnStateChanged")]
	protected COE_EGameModeState m_eCOE_CurrentState = COE_EGameModeState.INTERMISSION;
	protected ref ScriptInvoker m_pCOE_OnStateChanged = new ScriptInvoker();
	
	protected COE_FactionManager m_pFactionManager;
	protected ref array<IEntity> m_aEntitiesToDelete = {};
	protected ref array<ref SCR_MapMarkerBase> m_aExfilMarkers;
	protected KSC_CountdownAreaTriggerTask m_ExfilTask;
	
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
		if (!m_aAvailableLocations)
		{
			array<KSC_CustomLocationEntity> customLocations = KSC_CustomLocationEntity.GetInstances();
			m_aAvailableLocations = {};
			m_aAvailableLocations.Reserve(m_pKSC_WorldSlotsConfig.m_aLocations.Count() + customLocations.Count());
			
			foreach (KSC_Location location : m_pKSC_WorldSlotsConfig.m_aLocations)
			{
				m_aAvailableLocations.Insert(location);
			}
			
			foreach (KSC_CustomLocationEntity customLocation : customLocations)
			{
				m_aAvailableLocations.Insert(customLocation.GetLocationData());
			}
		}
		
		return m_aAvailableLocations;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref COE_AOParams> GetNextAOParams()
	{
		return m_aNextAOParams;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetNextAOParams(array<ref COE_AOParams> params)
	{
		m_aNextAOParams.Clear();
		m_aNextAOParams.Reserve(params.Count());
		
		foreach (COE_AOParams entry : params)
		{
			COE_AOParams copy = new COE_AOParams();
			copy.Copy(entry);
			m_aNextAOParams.Insert(copy);
		}
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetClosestAOPos(vector refPos, out vector closestPos)
	{
		bool found = false;
		float closestLocationDistance = float.INFINITY;
		
		foreach (COE_AOParams params : m_aNextAOParams)
		{
			if (!params)
				continue;
			
			KSC_Location location = params.GetLocation();
			if (!location)
				continue;
			
			float distance = vector.DistanceSqXZ(location.m_vCenter, refPos);
			
			if (distance < closestLocationDistance)
			{
				closestPos = location.m_vCenter;
				closestLocationDistance = distance;
				found = true;
			}
		}
		
		return found;
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
			case COE_ECommanderRequest.EXFIL_POINT: { CreateExfilPoint(pos); break; };
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
				m_aNextAOParams.Clear();
				break;
			}
			//case COE_EGameModeState.BRIEFING:
			case COE_EGameModeState.EXECUTION:
			{
				if (m_pInsertionPoint)
				{
					m_pInsertionPoint.SetSpawnPointEnabled_S(true);
					// Update rotation with respect to new AOs
					UpdateInsertionPointPosAndDir(m_pInsertionPoint.GetOrigin());
				}
				
				break;
			}
		}
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void GenerateAO()
	{
		//COE_SetState(COE_EGameModeState.BRIEFING);
		COE_SetState(COE_EGameModeState.EXECUTION);
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		
		foreach (COE_AOParams entry : m_aNextAOParams)
		{
			spawnParams.Transform[3] = entry.GetLocation().m_vCenter;
			SCR_TerrainHelper.SnapToTerrain(spawnParams.Transform);
			COE_AO ao = COE_AO.Cast(GetGame().SpawnEntity(COE_AO, GetWorld(), spawnParams));
			ao.SetParams(entry);
			ao.GetOnAllTasksFinished().Insert(OnAOFinished);
			m_aCurrentAOs.Insert(ao);
			
			if (m_bEnemySupportEnabled)
				COE_EnemySupportSystem.GetInstance().Register(ao);
		}
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
		{
			foreach (COE_AO ao : m_aCurrentAOs)
			{
				COE_EnemySupportSystem.GetInstance().Unregister(ao);
			}
		}
		
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
		GetGame().GetCallqueue().CallLater(CleanUpAOs, 3000);
		GetGame().GetCallqueue().CallLater(CleanUpBuiltEntities, 4000);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Delete all AI outside main base
	void CollectBuiltEntitiesForCleanUp()
	{
		SCR_EditableEntityCore core = SCR_EditableEntityCore.Cast(SCR_EditableEntityCore.GetInstance(SCR_EditableEntityCore));
		set<SCR_EditableEntityComponent> entities = new set<SCR_EditableEntityComponent>();
		core.GetAllEntities(entities, true, true);
		m_aEntitiesToDelete.Reserve(entities.Count());
		
		foreach (SCR_EditableEntityComponent entity : entities)
		{
			if (SCR_EditableVehicleComponent.Cast(entity) || SCR_EditableGroupComponent.Cast(entity))
				m_aEntitiesToDelete.Insert(entity.GetOwner());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CleanUpAOs()
	{
		DeleteInsertionPoint();
		DeleteExfilPoint();
		
		foreach (COE_AO ao : m_aCurrentAOs)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(ao);
		}
		
		m_aCurrentAOs.Clear();
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
		
		float dir;
		
		vector closestAOPos;
		if (GetClosestAOPos(pos, closestAOPos))
			dir = (closestAOPos - pos).ToYaw();
		
		if (!m_pInsertionPoint)
		{
			m_pInsertionPoint = KSC_GameTools.SpawnSpawnPointPrefab("{A99028D5E6D1B04D}PrefabsEditable/SpawnPoints/E_COE_InsertionPoint.et", pos, dir);
			
			if (m_eCOE_CurrentState == COE_EGameModeState.INTERMISSION || m_eCOE_CurrentState == COE_EGameModeState.BRIEFING)
				m_pInsertionPoint.SetSpawnPointEnabled_S(false);
			
			RplComponent rpl = RplComponent.Cast(m_pInsertionPoint.FindComponent(RplComponent));
			m_iInsertionPointId = rpl.Id();
			Replication.BumpMe();	
		}
		else
		{
			// Reuse the existing point by moving it to the new position
			UpdateInsertionPointPosAndDir(pos);
		}
		
		SCR_FactionAffiliationComponent factionAffiliation = SCR_FactionAffiliationComponent.Cast(m_pInsertionPoint.FindComponent(SCR_FactionAffiliationComponent));
		factionAffiliation.SetAffiliatedFaction(m_pFactionManager.GetPlayerFaction());
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateInsertionPointPosAndDir(vector newPos)
	{
		if (!m_pInsertionPoint)
			return;
		
		float newDir;
		
		// Rotate in direction of closest AO if one exists
		vector closestAOPos;
		if (GetClosestAOPos(newPos, closestAOPos))
			newDir = (closestAOPos - newPos).ToYaw();
		else
			newDir = m_pInsertionPoint.GetTransformAxis(2).ToYaw();
		
		SCR_EditableSpawnPointComponent editable = SCR_EditableSpawnPointComponent.Cast(SCR_EditableSpawnPointComponent.GetEditableEntity(m_pInsertionPoint));
		if (!editable)
			return;
		
		vector transform[4];
		KSC_GameTools.GetTransformFromPosAndRot(transform, newPos, newDir);
		editable.SetTransform(transform);
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
	SCR_SpawnPoint GetInsertionPoint()
	{
		return m_pInsertionPoint;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CreateExfilPoint(vector pos)
	{
		bool hasExfiltask = (m_ExfilTask != null);
		DeleteExfilPoint();
		
		if (hasExfiltask)
			CreateExfilTask(pos);
		
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
		if (!markerManager)
			return;
				
		m_aExfilMarkers = {new SCR_MapMarkerBase(), new SCR_MapMarkerBase()};
		
		foreach (SCR_MapMarkerBase marker : m_aExfilMarkers)
		{
			marker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
			marker.SetColorEntry(m_pFactionManager.GetMarkerCustomColorEntry(m_pFactionManager.GetPlayerFaction()));
			marker.SetWorldPos(pos[0], pos[2]);
			marker.SetCanBeRemovedByOwner(false);
		}
		
		m_aExfilMarkers[0].SetIconEntry(SCR_EScenarioFrameworkMarkerCustom.CIRCLE);
		m_aExfilMarkers[1].SetIconEntry(SCR_EScenarioFrameworkMarkerCustom.ARROW_SMALL3);
		m_aExfilMarkers[1].SetRotation(-90);
		
		foreach (SCR_MapMarkerBase marker : m_aExfilMarkers)
		{
			markerManager.InsertStaticMarker(marker, false, true);
		}
		
		m_bHasExfilPoint = true;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CreateExfilTask(vector pos)
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
		
		m_ExfilTask = KSC_CountdownAreaTriggerTask.Cast(KSC_GameTools.SpawnPrefab(m_sExfilTaskPrefabName, pos));
		m_ExfilTask.SetParams(m_pFactionManager.GetPlayerFaction(), 30);
		SCR_Task.GetOnTaskStateChanged().Insert(OnExfilStateChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasExfilPoint()
	{
		return m_bHasExfilPoint;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DeleteExfilPoint()
	{
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.GetInstance();
		if (!markerManager)
			return;
		
		if (m_aExfilMarkers)
		{
			foreach (SCR_MapMarkerBase marker : m_aExfilMarkers)
			{
				markerManager.RemoveStaticMarker(marker);
			}
		}
		
		if (m_ExfilTask)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(m_ExfilTask);
			SCR_Task.GetOnTaskStateChanged().Remove(OnExfilStateChanged);
		}
		
		m_aExfilMarkers = null;
		m_bHasExfilPoint = false;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	array<COE_AO> GetCurrentAOs()
	{
		return m_aCurrentAOs;
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
		
		m_pInsertionPoint = SCR_SpawnPoint.Cast(rpl.GetEntity());
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
	[Friend(COE_AO)]
	protected void OnAOFinished(COE_AO completedAO)
	{
		foreach (COE_AO ao : m_aCurrentAOs)
		{
			if (ao && !ao.AreAllTasksFinished())
				return;
		}
		
		int markerPos[2];
		m_aExfilMarkers[0].GetWorldPos(markerPos);
		vector pos = Vector(markerPos[0], 0, markerPos[1]);
		CreateExfilTask(pos);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnExfilStateChanged(SCR_Task task, SCR_ETaskState newState)
	{
		if (task != m_ExfilTask)
			return;
		
		if (newState != SCR_ETaskState.COMPLETED)
			return;
				
		ExecuteCommanderRequest(COE_ECommanderRequest.CANCEL_AO);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ShowDocumentation()
	{
		SCR_FieldManualUI.Open(EFieldManualEntryId.COE_COMMANDER);
		SCR_RespawnComponent.SGetOnLocalPlayerSpawned().Remove(ShowDocumentation);
	}
}

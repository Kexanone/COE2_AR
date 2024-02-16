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
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(KSC_ETaskType, "m_eType")]
class COE_TaskType
{
	[Attribute(uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(KSC_ETaskType))]
	protected KSC_ETaskType m_eType;
	
	KSC_ETaskType GetID()
	{
		return m_eType;
	}
	
	LocalizedString GetName()
	{
		return SCR_Enum.GetEnumName(KSC_ETaskType, m_eType);
	}
}

//------------------------------------------------------------------------------------------------
class COE_GameModeClass : SCR_BaseGameModeClass
{
};

//------------------------------------------------------------------------------------------------
class COE_GameMode : SCR_BaseGameMode
{
	[Attribute(desc: "Location types to be excluded", category: "Locations")]
	protected ref array<ref COE_MapDescriptorType> m_aBlacklistedDescriptorTypes;
	
	[Attribute(desc: "Available task types", category: "Tasks")]
	protected ref array<ref COE_TaskType> m_aAvailableTaskTypes;
	
	[Attribute(desc: "Default faction key for the players", category: "Default Scenario Properties")]
	protected FactionKey m_sDefaultPlayerFactionKey;
	
	[Attribute(defvalue: "FIA", desc: "Default faction key of the enemy AI", category: "Default Scenario Properties")]
	protected FactionKey m_sDefaultEnemyFactionKey;
	
	[Attribute(defvalue: "3", desc: "Number of objectives before session ends", category: "Default Scenario Properties")]
	protected int m_iNumberOfRemainingObjectives;
	
	[Attribute(defvalue: "-1", uiwidget: UIWidgets.Slider, params: "-1 3 1", desc: "Number of tasks per objective. Random if -1.", category: "Default Scenario Properties")]
	protected int m_iTasksPerObjective;
	protected int m_iMaxTasksPerObjective = 3;
	
	[Attribute(defvalue: "30", desc: "Minimal enemy AI count for an AO", category: "Default Scenario Properties")]
	protected int m_iMinEnemyAiCount;
	
	[Attribute(defvalue: "2.5", desc: "Total enemy AI count for an AO will be this multiplier times the total player count (ignored when below m_iMinEnemyAiCount)", category: "Default Scenario Properties")]
	protected float m_fEnemyAiCountMultiplier;
	
	[RplProp()]
	protected vector m_vMainBasePos;
	protected COE_MainBaseEntity m_pMainBase;
	
	[RplProp()]
	protected ref COE_AOParams m_pCurrentAOParams = new COE_AOParams();
	protected ref COE_AOParams m_pNextAOParams = new COE_AOParams();
	
	[RplProp()]
	protected vector m_vInsertionPos;
	protected SCR_SpawnPoint m_pInsertionPoint;
	
	[RplProp(onRplName: "COE_OnStateChanged")]
	protected COE_EGameModeState m_eCOE_CurrentState = COE_EGameModeState.INTERMISSION;
	protected ref ScriptInvoker m_pCOE_OnStateChanged = new ScriptInvoker();
	
	protected ref array<IEntity> m_aAvailableLocations;
	protected COE_FactionManager m_pFactionManager;
		
	//------------------------------------------------------------------------------------------------
	void COE_GameMode(IEntitySource src, IEntity parent)
	{
		if (!GetGame().InPlayMode() || Replication.IsClient())
			return;
		
		// Read mission header
		COE_MissionHeader header = COE_MissionHeader.Cast(GetGame().GetMissionHeader());
		if (header)
		{
		};
	}
	
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
		
		m_pFactionManager = COE_FactionManager.GetInstance();
		if (!m_pFactionManager)
			return;
		
		if (!m_sDefaultPlayerFactionKey.IsEmpty())
			m_pFactionManager.SetPlayerFaction(SCR_Faction.Cast(m_pFactionManager.GetFactionByKey(m_sDefaultPlayerFactionKey)), true);
		
		if (!m_sDefaultEnemyFactionKey.IsEmpty())
			m_pFactionManager.SetEnemyFaction(SCR_Faction.Cast(m_pFactionManager.GetFactionByKey(m_sDefaultEnemyFactionKey)));
		
		COE_SetState(COE_EGameModeState.INTERMISSION);
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
	array<ref COE_TaskType> GetAvailableTaskTypes()
	{
		return m_aAvailableTaskTypes;
	}
	
	//------------------------------------------------------------------------------------------------
	array<IEntity> GetAvailableLocations()
	{
		if (!m_aAvailableLocations)
		{
			vector mins, maxs;
			GetGame().GetWorld().GetBoundBox(mins, maxs);
			m_aAvailableLocations = {};
			GetGame().GetWorld().QueryEntitiesByAABB(mins, maxs, QueryLocationEntitiesCallback, null, EQueryEntitiesFlags.STATIC);
		}
		
		return m_aAvailableLocations;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool QueryLocationEntitiesCallback(IEntity entity)
	{
		if (!SCR_EditableCommentComponent.Cast(entity.FindComponent(SCR_EditableCommentComponent)))
			return true;
		
		if (!MapDescriptorComponent.Cast(entity.FindComponent(MapDescriptorComponent)))
			return true;
		
		foreach (COE_MapDescriptorType type : m_aBlacklistedDescriptorTypes)
		{
			if (type.IsType(entity))
				return true;
		};
		
		m_aAvailableLocations.Insert(entity);
		return true;
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
				m_pNextAOParams.m_eTaskTypes = 1 << Math.RandomInt(0, m_aAvailableTaskTypes.Count());
				array<IEntity> locations = GetAvailableLocations();
				m_pNextAOParams.m_pLocation = locations[Math.RandomInt(0, locations.Count())];
				
				// Clear old AO;
				DeleteInsertionPoint();
				m_pCurrentAOParams.Clear();
				break;
			}
			case COE_EGameModeState.BRIEFING:
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
		COE_SetState(COE_EGameModeState.BRIEFING);
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
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CreateInsertionPoint(vector pos)
	{
		vector dir = (m_pCurrentAOParams.m_pLocation.GetOrigin() - pos);
		
		if (!m_pInsertionPoint)
		{
			m_pInsertionPoint = KSC_GameTools.SpawnSpawnPointPrefab("{987991DCED3DC197}PrefabsEditable/SpawnPoints/E_SpawnPoint.et", pos, dir.ToYaw());
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
		m_vInsertionPos = pos;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DeleteInsertionPoint()
	{
		SCR_EntityHelper.DeleteEntityAndChildren(m_pInsertionPoint);
		m_vInsertionPos = vector.Zero;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	vector GetInsertionPos()
	{
		return m_vInsertionPos;
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetCurrentAO()
	{
		if (!m_pCurrentAOParams)
			return null;
		
		return m_pCurrentAOParams.m_pLocation;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetNumRemainingObjectives()
	{
		return m_iNumberOfRemainingObjectives;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetNumRemainingObjectives(int numObjectives)
	{
		m_iNumberOfRemainingObjectives = numObjectives;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetMinEnemyAiCount(int count)
	{
		m_iMinEnemyAiCount = count;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetMinEnemyAiCount()
	{
		return m_iMinEnemyAiCount;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetEnemyAiCountMultiplier(float multiplier)
	{
		m_fEnemyAiCountMultiplier = multiplier;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetEnemyAiCountMultiplier()
	{
		return m_fEnemyAiCountMultiplier;
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
			
			if (becomesCommanderOrAdmin)
			{
				editorManager.AddEditorModes(EEditorModeAccess.BASE, EEditorMode.COE_COMMANDER);
			}
			else
			{
				editorManager.RemoveEditorModes(EEditorModeAccess.BASE, EEditorMode.COE_COMMANDER);
			};
		}
		
		playerController.OnPlayerRoleChangeServer(roleFlags);
	}
}

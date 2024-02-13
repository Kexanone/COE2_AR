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
	END_BRIEFING // Request to end the briefing and start execution
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
};

//------------------------------------------------------------------------------------------------
class COE_GameMode : SCR_BaseGameMode
{
	[Attribute(desc: "Names entities to pick from for main base", category: "Main Base")]
	protected ref array<string> m_aMainBaseNames;
	
	[Attribute(desc: "Location types to be excluded", category: "Locations")]
	protected ref array<ref COE_MapDescriptorType> m_aBlacklistedDescriptorTypes;
	
	[Attribute(desc: "Default faction key for the players", category: "Default Scenario Properties")]
	protected FactionKey m_sDefaultPlayerFactionKey;
	
	[Attribute(defvalue: "FIA", desc: "Default faction key of the enemy AI", category: "Default Scenario Properties")]
	protected FactionKey m_sDefaultEnemyFactionKey;
	
	[Attribute(defvalue: "3", desc: "Number of objectives before session ends", category: "Default Scenario Properties")]
	protected int m_iNumberOfRemainingObjectives;
	
	[RplProp()]
	protected vector m_vMainBasePos;
	
	[RplProp(onRplName: "COE_OnStateChanged")]
	protected COE_EGameModeState m_eCOE_CurrentState = COE_EGameModeState.INTERMISSION;
	protected ref ScriptInvoker m_pCOE_OnStateChanged = new ScriptInvoker();
	
	protected IEntity m_pMainBase;
	protected ref array<IEntity> m_aAvailableLocations;
	protected COE_FactionManager m_pFactionManager;
	protected IEntity m_pNextLocation;
		
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
	
		if (m_aMainBaseNames.IsEmpty())
			return;
		
		m_pMainBase = GetGame().GetWorld().FindEntityByName(m_aMainBaseNames[Math.RandomInt(0, m_aMainBaseNames.Count())]);
		m_vMainBasePos = m_pMainBase.GetOrigin();
		Replication.BumpMe();
		
		m_pFactionManager = COE_FactionManager.GetInstance();
		if (!m_pFactionManager)
			return;
		
		if (!m_sDefaultPlayerFactionKey.IsEmpty())
			m_pFactionManager.SetPlayerFaction(SCR_Faction.Cast(m_pFactionManager.GetFactionByKey(m_sDefaultPlayerFactionKey)), true);
		
		if (!m_sDefaultEnemyFactionKey.IsEmpty())
			m_pFactionManager.SetEnemyFaction(SCR_Faction.Cast(m_pFactionManager.GetFactionByKey(m_sDefaultEnemyFactionKey)));
		
		// Compile possible locations and select one randomly
		array<IEntity> locations = GetAvailableLocations();
		m_pNextLocation = locations[Math.RandomInt(0, locations.Count())];
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetMainBase()
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
		
		foreach (COE_MapDescriptorType type : m_aBlacklistedDescriptorTypes)
		{
			if (type.IsType(entity))
				return true;
		};
		
		m_aAvailableLocations.Insert(entity);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetNextLocation()
	{
		return m_pNextLocation;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetNextLocation(IEntity location)
	{
		m_pNextLocation = location;
	}
	
	//------------------------------------------------------------------------------------------------
	void ExecuteCommanderRequest(COE_ECommanderRequest request)
	{
		switch (request)
		{
			case COE_ECommanderRequest.GENERATE_AO: { GenerateAO(); break; };
			case COE_ECommanderRequest.CANCEL_AO: { DeleteAO(); break; };
			case COE_ECommanderRequest.END_BRIEFING: { StartAO(); break; };
		}
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

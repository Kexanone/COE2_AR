//------------------------------------------------------------------------------------------------
enum COE_EGameState
{
	BRIEFING,
	OBJECTIVE
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
	protected COE_Faction m_pPlayerFaction;
	
	[Attribute(defvalue: "FIA", desc: "Default faction key of the enemy AI", category: "Default Scenario Properties")]
	protected FactionKey m_sDefaultEnemyFactionKey;
	protected COE_Faction m_pEnemyFaction;
	
	[Attribute(defvalue: "3", desc: "Number of objectives before session ends", category: "Default Scenario Properties")]
	protected int m_iNumberOfRemainingObjectives;
	
	[RplProp()]
	protected vector m_vMainBasePos;
	
	protected IEntity m_pMainBase;
	protected ref array<IEntity> m_aPossibleLocations;
	protected COE_FactionManager m_pFactionManagers;
	protected Managed m_pCurrentEditedSettingsEntity;
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
		
		m_pFactionManagers = COE_FactionManager.Cast(GetGame().GetFactionManager());
		if (!m_sDefaultPlayerFactionKey.IsEmpty())
			SetPlayerFaction(COE_Faction.Cast(m_pFactionManagers.GetFactionByKey(m_sDefaultPlayerFactionKey)), true);
		
		if (!m_sDefaultEnemyFactionKey.IsEmpty())
			SetEnemyFaction(COE_Faction.Cast(m_pFactionManagers.GetFactionByKey(m_sDefaultEnemyFactionKey)));
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
		
		// Compile possible locations and select one randomly
		array<IEntity> locations = GetPossibleLocations();
		m_pNextLocation = locations[Math.RandomInt(0, locations.Count())];
	}
	
	//------------------------------------------------------------------------------------------------
	vector GetMainBasePos()
	{
		return m_vMainBasePos;
	}
	
	//------------------------------------------------------------------------------------------------
	array<IEntity> GetPossibleLocations()
	{
		if (!m_aPossibleLocations)
		{
			vector mins, maxs;
			GetGame().GetWorld().GetBoundBox(mins, maxs);
			m_aPossibleLocations = {};
			GetGame().GetWorld().QueryEntitiesByAABB(mins, maxs, QueryLocationEntitiesCallback, null, EQueryEntitiesFlags.STATIC);
		}
		
		return m_aPossibleLocations;
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
		
		m_aPossibleLocations.Insert(entity);
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
	COE_Faction GetPlayerFaction()
	{
		return m_pPlayerFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPlayerFaction(COE_Faction faction, bool isInit = false)
	{
		// Directly setting IsPlayable only works during init
		if (isInit)
		{
			faction.SetIsPlayable(true);
		}
		else
		{
			SCR_DelegateFactionManagerComponent delegateFactionManager = SCR_DelegateFactionManagerComponent.GetInstance();
			if (!delegateFactionManager)
				return;
			
			SCR_EditableFactionComponent prevFactionDelegate = delegateFactionManager.GetFactionDelegate(m_pPlayerFaction);
			if (prevFactionDelegate)
				prevFactionDelegate.SetFactionPlayableServer(false);
			
			SCR_EditableFactionComponent newFactionDelegate = delegateFactionManager.GetFactionDelegate(faction);
			if (newFactionDelegate)
				newFactionDelegate.SetFactionPlayableServer(true);
		};

		m_pPlayerFaction = faction;
	}
	
	//------------------------------------------------------------------------------------------------
	COE_Faction GetEnemyFaction()
	{
		return m_pEnemyFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetEnemyFaction(COE_Faction faction)
	{
		m_pEnemyFaction = faction;		
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
	//! Opens attribute window for the given entity
	//! Makes sure to switch to an editor with full support for attributes if possible
	void COE_StartEditingAttributes(Managed entity = null)
	{
		SCR_EditorManagerEntity editorManagerEntity = SCR_EditorManagerEntity.Cast(SCR_EditorManagerEntity.GetInstance());
		if (!editorManagerEntity)
			return;
		
		m_pCurrentEditedSettingsEntity = entity;
		EEditorMode mode = editorManagerEntity.GetCurrentMode();
		if (mode == EEditorMode.EDIT || mode == EEditorMode.COE_SQUAD_LEADER)
		{
			COE_StartEditingOnModeChange();
			return;
		};
		
		if (editorManagerEntity.HasMode(EEditorMode.EDIT))
		{
			editorManagerEntity.SetCurrentMode(EEditorMode.EDIT);
		}
		else if (editorManagerEntity.HasMode(EEditorMode.COE_SQUAD_LEADER))
		{
			editorManagerEntity.SetCurrentMode(EEditorMode.COE_SQUAD_LEADER);
		};
		
		if (Replication.IsClient())
		{
			editorManagerEntity.GetOnModeChange().Insert(COE_StartEditingOnModeChange);
		}
		else
		{
			COE_StartEditingOnModeChange();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! For scheduling opening of attribute window when a mode change was required
	void COE_StartEditingOnModeChange(SCR_EditorModeEntity curMode = null, SCR_EditorModeEntity prevMode = null)
	{
		SCR_EditorManagerEntity editorManagerEntity = SCR_EditorManagerEntity.Cast(SCR_EditorManagerEntity.GetInstance());
		if (editorManagerEntity)
			editorManagerEntity.GetOnModeChange().Remove(COE_StartEditingOnModeChange);
		
		SCR_AttributesManagerEditorComponent attributesManager = SCR_AttributesManagerEditorComponent.Cast(SCR_AttributesManagerEditorComponent.GetInstance(SCR_AttributesManagerEditorComponent));
		if (!attributesManager)
			return;
		
		if (!m_pCurrentEditedSettingsEntity)
		{
			attributesManager.StartEditing(this);
		}
		else
		{
			attributesManager.StartEditing(m_pCurrentEditedSettingsEntity);
		}
	}
	
	protected override void OnPlayerRoleChange(int playerId, EPlayerRole roleFlags)
	{
		super.OnPlayerRoleChange(playerId, roleFlags);
		
		if (Replication.IsClient())
			return;
					
		
		COE_PlayerController playerController = COE_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return;
		
		bool wasSquadLeader = playerController.HasPlayerRole(EPlayerRole.COE_SQUAD_LEADER);
		bool becomesSquadLeader = (roleFlags & EPlayerRole.COE_SQUAD_LEADER);
		// Check if squad leader flag was changed
		if (wasSquadLeader != becomesSquadLeader)
		{
			SCR_EditorManagerCore core = SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));
			SCR_EditorManagerEntity editorManager = core.GetEditorManager(playerId);
			
			if (becomesSquadLeader)
			{
				editorManager.AddEditorModes(EEditorModeAccess.BASE, EEditorMode.COE_SQUAD_LEADER, true);
			}
			else
			{
				editorManager.RemoveEditorModes(EEditorModeAccess.BASE, EEditorMode.COE_SQUAD_LEADER);
			};
		}
		
		playerController.OnPlayerRoleChangeServer(roleFlags);
	}
}

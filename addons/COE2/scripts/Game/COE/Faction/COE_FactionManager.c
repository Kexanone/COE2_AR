//------------------------------------------------------------------------------------------------
class COE_FactionManagerClass : KSC_FactionManagerClass
{
}

//------------------------------------------------------------------------------------------------
class COE_FactionManager : KSC_FactionManager
{
	// -1 = not set, so that a replicated index 0 counts as a change on proxies
	[RplProp(onRplName: "OnPlayerFactionChanged")]
	protected int m_iPlayerFactionId = -1;
	protected Faction m_pPlayerFaction;

	[RplProp(onRplName: "OnEnemyFactionChanged")]
	protected int m_iEnemyFactionId = -1;
	protected Faction m_pEnemyFaction;

	[RplProp(onRplName: "OnCivilianFactionChanged")]
	protected int m_iCivilianFactionId = -1;
	protected Faction m_pCivilianFaction;
	
	protected SCR_DelegateFactionManagerComponent m_pDelegateFactionManager;
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		m_pDelegateFactionManager = SCR_DelegateFactionManagerComponent.Cast(FindComponent(SCR_DelegateFactionManagerComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	Faction GetPlayerFaction()
	{
		// Proxies may not have run the onRpl handler for the initial state
		if (!m_pPlayerFaction && m_iPlayerFactionId >= 0)
			m_pPlayerFaction = GetFactionByIndex(m_iPlayerFactionId);

		return m_pPlayerFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPlayerFaction(SCR_Faction faction, bool isInit = false)
	{
		// Directly setting IsPlayable only works during init
		if (isInit)
		{
			faction.SetIsPlayable(true);
		}
		else
		{
			SCR_EditableFactionComponent prevFactionDelegate = m_pDelegateFactionManager.GetFactionDelegate(m_pPlayerFaction);
			if (prevFactionDelegate)
				prevFactionDelegate.SetFactionPlayableServer(false);
			
			SCR_EditableFactionComponent newFactionDelegate = m_pDelegateFactionManager.GetFactionDelegate(faction);
			if (newFactionDelegate)
				newFactionDelegate.SetFactionPlayableServer(true);
		};

		m_pPlayerFaction = faction;
		m_iPlayerFactionId = GetFactionIndex(faction);
		Replication.BumpMe();
		
		// Delete all players such that they have to change to the new faction
		array<int> playerIds = {};
		GetGame().GetPlayerManager().GetPlayers(playerIds);
		foreach (int playerId : playerIds)
		{			
			IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if (player)
				SCR_EntityHelper.DeleteEntityAndChildren(player);
		}
		
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return;
		
		// Delete ongoing objectives
		if (gameMode.COE_GetState() == COE_EGameModeState.INTERMISSION)
			gameMode.ExecuteCommanderRequest(COE_ECommanderRequest.CANCEL_AO);
		
		// Delete all AI immediately
		gameMode.CollectBuiltEntitiesForCleanUp();
		gameMode.CleanUpBuiltEntities();
		
		// Change affiliation of all objects at the player base
		COE_MainBaseEntity mainBase = gameMode.GetMainBase();
		if (mainBase)
			mainBase.SetFaction(faction);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Called on clients when player faction has changed
	protected void OnPlayerFactionChanged()
	{
		m_pPlayerFaction = SCR_Faction.Cast(GetFactionByIndex(m_iPlayerFactionId));
	}
	
	//------------------------------------------------------------------------------------------------
	void SetEnemyFaction(SCR_Faction faction)
	{
		m_pEnemyFaction = faction;	
		m_iEnemyFactionId = GetFactionIndex(faction);
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	Faction GetEnemyFaction()
	{
		// Proxies may not have run the onRpl handler for the initial state
		if (!m_pEnemyFaction && m_iEnemyFactionId >= 0)
			m_pEnemyFaction = GetFactionByIndex(m_iEnemyFactionId);

		return m_pEnemyFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Called on clients when enemy faction has changed
	protected void OnEnemyFactionChanged()
	{
		m_pEnemyFaction = GetFactionByIndex(m_iEnemyFactionId);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCivilianFaction(SCR_Faction faction)
	{
		m_pCivilianFaction = faction;
		m_iCivilianFactionId = GetFactionIndex(faction);
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	Faction GetCivilianFaction()
	{
		// Proxies may not have run the onRpl handler for the initial state
		if (!m_pCivilianFaction && m_iCivilianFactionId >= 0)
			m_pCivilianFaction = GetFactionByIndex(m_iCivilianFactionId);

		return m_pCivilianFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Called on clients when civilian faction has changed
	protected void OnCivilianFactionChanged()
	{
		m_pCivilianFaction = GetFactionByIndex(m_iCivilianFactionId);
	}
}

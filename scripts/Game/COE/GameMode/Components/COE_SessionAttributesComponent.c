class COE_SessionAttributesComponentClass : SCR_BaseGameModeComponentClass
{
}

class COE_SessionAttributesComponent : SCR_BaseGameModeComponent
{
	[Attribute(defvalue: "3", desc: "Number of objectives before session ends")]
	protected int m_iNumberOfObjectives;

	protected COE_Faction m_pPlayerFaction;
	protected COE_Faction m_pEnemyFaction;

	COE_Faction GetPlayerFaction()
	{
		return m_pPlayerFaction;
	}
	
	void SetPlayerFaction(COE_Faction faction)
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
		
		m_pPlayerFaction = faction;
	}
	
	COE_Faction GetEnemyFaction()
	{
		if (!m_pEnemyFaction)
			m_pEnemyFaction = COE_Faction.Cast(GetGame().GetFactionManager().GetFactionByIndex(0));
		
		return m_pEnemyFaction;
	}
	
	void SetEnemyFaction(COE_Faction faction)
	{
		m_pEnemyFaction = faction;		
	}
	
	int GetNumObjectives()
	{
		return m_iNumberOfObjectives;
	}
	
	void SetNumObjectives(int numObjectives)
	{
		m_iNumberOfObjectives = numObjectives;
	}
	
	override bool RplSave(ScriptBitWriter writer) { return true; }
	override bool RplLoad(ScriptBitReader reader) { return true; }
}
//------------------------------------------------------------------------------------------------
class COE_FactionManagerClass: SCR_FactionManagerClass
{
};

//------------------------------------------------------------------------------------------------
class COE_FactionManager : SCR_FactionManager
{
	protected ref COE_Faction m_PlayerFaction;
	protected ref COE_Faction m_EnemyFaction;
	
	COE_Faction GetPlayerFaction()
	{
		return m_PlayerFaction;
	}
	
	COE_Faction GetEnemyFaction()
	{
		return m_EnemyFaction;
	}
}

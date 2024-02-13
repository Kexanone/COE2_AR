[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_EnemyFactionEditorAttribute: COE_BaseFactionEditorAttribute
{		
	override SCR_Faction GetFaction(COE_GameMode gameMode)
	{
		return m_pFactionManager.GetEnemyFaction();
	}
	
	override void SetFaction(COE_GameMode gameMode, SCR_Faction faction, int playerID)
	{
		m_pFactionManager.SetEnemyFaction(faction);
	}
	
	override typename GetTypeLinkedAttribute()
	{
		return COE_PlayerFactionEditorAttribute;
	}
}
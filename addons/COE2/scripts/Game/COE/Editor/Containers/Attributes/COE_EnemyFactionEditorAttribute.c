//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_EnemyFactionEditorAttribute : COE_BaseFactionEditorAttribute
{
	//------------------------------------------------------------------------------------------------
	override Faction GetFaction(COE_FactionManager factionManager)
	{
		return factionManager.GetEnemyFaction();
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetFaction(COE_FactionManager factionManager, SCR_Faction faction, int playerID)
	{
		factionManager.SetEnemyFaction(faction);
	}
	
	//------------------------------------------------------------------------------------------------
	override typename GetLinkedAttributeType()
	{
		return COE_PlayerFactionEditorAttribute;
	}
}
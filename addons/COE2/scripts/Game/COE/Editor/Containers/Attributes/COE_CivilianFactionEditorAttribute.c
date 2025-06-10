//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_CivilianFactionEditorAttribute : COE_BaseFactionEditorAttribute
{
	//------------------------------------------------------------------------------------------------
	override Faction GetFaction(COE_FactionManager factionManager)
	{
		return factionManager.GetCivilianFaction();
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetFaction(COE_FactionManager factionManager, SCR_Faction faction, int playerID)
	{
		factionManager.SetCivilianFaction(faction);
	}
}
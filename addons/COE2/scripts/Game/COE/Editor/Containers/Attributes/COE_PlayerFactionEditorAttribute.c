//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_PlayerFactionEditorAttribute : COE_BaseFactionEditorAttribute
{
	//------------------------------------------------------------------------------------------------
	override SCR_Faction GetFaction(COE_FactionManager factionManager)
	{
		return factionManager.GetPlayerFaction();
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetFaction(COE_FactionManager factionManager, SCR_Faction faction, int playerID)
	{
		factionManager.SetPlayerFaction(faction);
		SCR_NotificationsComponent.SendToEveryone(ENotification.EDITOR_ATTRIBUTES_FACTION_CHANGED, playerID);
	}
	
	//------------------------------------------------------------------------------------------------
	override typename GetLinkedAttributeType()
	{
		return COE_EnemyFactionEditorAttribute;
	}
}
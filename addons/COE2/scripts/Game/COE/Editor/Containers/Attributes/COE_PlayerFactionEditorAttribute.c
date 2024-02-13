[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_PlayerFactionEditorAttribute: COE_BaseFactionEditorAttribute
{		
	override SCR_Faction GetFaction(COE_GameMode gameMode)
	{
		return m_pFactionManager.GetPlayerFaction();
	}
	
	override void SetFaction(COE_GameMode gameMode, SCR_Faction faction, int playerID)
	{
		m_pFactionManager.SetPlayerFaction(faction);
		SCR_NotificationsComponent.SendToEveryone(ENotification.EDITOR_ATTRIBUTES_FACTION_CHANGED, playerID);
	}
	
	override typename GetTypeLinkedAttribute()
	{
		return COE_EnemyFactionEditorAttribute;
	}
}
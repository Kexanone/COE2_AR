//------------------------------------------------------------------------------------------------
modded class SCR_PlayerListMenu : SCR_SuperMenuBase
{
	//------------------------------------------------------------------------------------------------
	//! Add Zeus icon to players with commander role
	//! --- TO DO: Use conflict commander indicator once they add it
	override protected void UpdateGameMasterIndicator(notnull SCR_PlayerListEntry entry, bool editorIslimited)
	{
		super.UpdateGameMasterIndicator(entry, editorIslimited);
		
		if (!editorIslimited)
			return;
				
		COE_PlayerController playerController = COE_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(entry.m_iID));
		if (!playerController)
			return;
		
		Widget gameMasterIndicator = entry.m_wRow.FindAnyWidget(m_sGameMasterIndicatorName);
		if (gameMasterIndicator)
			gameMasterIndicator.SetVisible(playerController.HasPlayerRole(EPlayerRole.COE_COMMANDER));
	}
}
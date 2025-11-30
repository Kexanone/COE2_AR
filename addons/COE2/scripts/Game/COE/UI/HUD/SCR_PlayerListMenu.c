//------------------------------------------------------------------------------------------------
modded class SCR_PlayerListMenu : SCR_SuperMenuBase
{
	//------------------------------------------------------------------------------------------------
	//! Add Zeus icon to players with commander role
	//! --- TO DO: Use conflict commander indicator once they add it
	override protected void UpdateGameMasterIndicator(notnull SCR_PlayerListEntry entry, bool editorIslimited)
	{
		super.UpdateGameMasterIndicator(entry, editorIslimited);
		
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return;
				
		ToggleCommanderIndicator(entry, gameMode.IsCommander(entry.m_iID));
	}
}
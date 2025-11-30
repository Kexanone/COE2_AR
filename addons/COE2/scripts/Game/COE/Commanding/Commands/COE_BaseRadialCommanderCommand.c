[BaseContainerProps("", "Entry of one command"), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class COE_BaseRadialCommanderCommand : SCR_BaseRadialCommand
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShown()
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		return gameMode.IsCommander(SCR_PlayerController.GetLocalPlayerId());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanShowOnMap()
	{
		return CanBeShown();
	}
}

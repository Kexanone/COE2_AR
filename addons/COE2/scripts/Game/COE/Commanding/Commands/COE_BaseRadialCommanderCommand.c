[BaseContainerProps("", "Entry of one command"), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class COE_BaseRadialCommanderCommand : SCR_BaseRadialCommand
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShown()
	{
		COE_PlayerController playerController = COE_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return false;
		
		return (playerController.HasPlayerRole(EPlayerRole.COE_COMMANDER) || playerController.HasPlayerRole(EPlayerRole.ADMINISTRATOR));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanShowOnMap()
	{
		return CanBeShown();
	}
}

//------------------------------------------------------------------------------------------------
//! Board actions reserved to commanders
class COE_BaseCommanderBoardUserAction : COE_BaseBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		COE_PlayerController playerController = COE_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return false;
		
		return (playerController.HasPlayerRole(EPlayerRole.COE_COMMANDER) || playerController.HasPlayerRole(EPlayerRole.ADMINISTRATOR));
	}
}

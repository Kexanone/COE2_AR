//------------------------------------------------------------------------------------------------
//! Board actions reserved to commanders
class COE_BaseCommanderBoardUserAction : COE_BaseBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		return gameMode.IsCommander(SCR_PlayerController.GetLocalPlayerId());
	}
}

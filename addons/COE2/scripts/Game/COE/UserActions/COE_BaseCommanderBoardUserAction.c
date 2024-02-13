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
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user) 
	{
		if (COE_GameMode.GetInstance().COE_GetState() == COE_EGameModeState.BRIEFING)
		{
			m_sCannotPerformReason = "#COE-Reason_OngoingBriefing";
			return false;
		};
		
		if (COE_GameMode.GetInstance().COE_GetState() != COE_EGameModeState.INTERMISSION)
		{
			m_sCannotPerformReason = "#COE-Reason_OngoingAO";
			return false;
		};
		
		return true;
	}
}

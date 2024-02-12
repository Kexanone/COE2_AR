//------------------------------------------------------------------------------------------------
class COE_BaseSquadLeaderBoardUserAction : COE_BaseBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		COE_PlayerController playerController = COE_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return false;
		
		if (!playerController.HasPlayerRole(EPlayerRole.COE_SQUAD_LEADER))
		{
			m_sCannotPerformReason = "Squad leader only";
			return false;
		};
		
		return true;
	}
}

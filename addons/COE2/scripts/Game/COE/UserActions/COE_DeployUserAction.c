//------------------------------------------------------------------------------------------------
class COE_DeployUserAction : COE_BaseBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{		
		GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.COE_DeploymentMenu, DialogPriority.INFORMATIVE, 0, true);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user) 
	{
		if (COE_GameMode.GetInstance().COE_GetState() == COE_EGameModeState.INTERMISSION)
		{
			m_sCannotPerformReason = "#COE-Reason_NoAO";
			return false;
		};
		
		if (COE_GameMode.GetInstance().COE_GetState() == COE_EGameModeState.BRIEFING)
		{
			m_sCannotPerformReason = "#COE-Reason_OngoingBriefing";
			return false;
		};
		
		return true;
	}
}

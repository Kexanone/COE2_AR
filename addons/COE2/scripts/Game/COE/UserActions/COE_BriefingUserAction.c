//------------------------------------------------------------------------------------------------
class COE_BriefingUserAction : COE_BaseBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		COE_EditorModeCommanderEntity.Request(COE_ECommanderRequest.END_BRIEFING);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user) 
	{
		if (COE_GameMode.GetInstance().COE_GetState() == COE_EGameModeState.INTERMISSION)
		{
			m_sCannotPerformReason = "#COE-Reason_NoAOGenerated";
			return false;
		};
		
		if (COE_GameMode.GetInstance().COE_GetState() != COE_EGameModeState.BRIEFING)
		{
			m_sCannotPerformReason = "#COE-Reason_OngoingAO";
			return false;
		};
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user) 
	{
		return false;
	}
}

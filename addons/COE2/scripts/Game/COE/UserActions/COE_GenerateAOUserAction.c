//------------------------------------------------------------------------------------------------
class COE_GenerateAOUserAction : COE_BaseCommanderBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{		
		COE_EditorModeCommanderEntity.Request(COE_ECommanderRequest.GENERATE_AO);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user) 
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		if (gameMode.GetNextAOParams().IsEmpty())
		{
			m_sCannotPerformReason = "#COE-Reason_NoAOSelected";
			return false;
		}
		
		if (!gameMode.GetInsertionPoint())
		{
			m_sCannotPerformReason = "#COE-Reason_NoInsertionPoint";
			return false;
		}
		
		if (!gameMode.HasExfilPoint())
		{
			m_sCannotPerformReason = "#COE-Reason_NoExfilPoint";
			return false;
		}
		
		if (gameMode.COE_GetState() != COE_EGameModeState.INTERMISSION)
		{
			m_sCannotPerformReason = "#COE-Reason_OngoingAO";
			return false;
		}
		
		return true;
	}
}

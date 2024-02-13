//------------------------------------------------------------------------------------------------
class COE_CancelAOUserAction : COE_BaseCommanderBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{		
		COE_EditorModeCommanderEntity.Request(COE_ECommanderRequest.CANCEL_AO);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user) 
	{
		if (COE_GameMode.GetInstance().COE_GetState() == COE_EGameModeState.INTERMISSION)
		{
			m_sCannotPerformReason = "#COE-Reason_NoAO";
			return false;
		};
		
		return true;
	}
}

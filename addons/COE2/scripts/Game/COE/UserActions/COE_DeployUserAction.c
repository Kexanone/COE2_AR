//------------------------------------------------------------------------------------------------
class COE_DeployUserAction : COE_BaseBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return;
		
		IEntity location = gameMode.GetCurrentAO();
		if (!location)
			return;
		
		vector locationPos = location.GetOrigin();
		vector spawnPos = gameMode.GetInsertionPos();
		COE_PlayerController.GetInstance().RequestFastTravel(spawnPos, (locationPos - spawnPos).ToYaw());
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

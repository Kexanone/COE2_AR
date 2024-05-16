//------------------------------------------------------------------------------------------------
class COE_DeployUserAction : COE_BaseBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return;
		
		KSC_Location location = gameMode.GetCurrentLocation();
		if (!location)
			return;
		
		vector spawnPos = gameMode.GetInsertionPos();		
		COE_PlayerController.GetInstance().RequestFastTravel(spawnPos, (location.m_vCenter - spawnPos).ToYaw());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user) 
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		if (gameMode.COE_GetState() == COE_EGameModeState.INTERMISSION)
		{
			m_sCannotPerformReason = "#COE-Reason_NoAO";
			return false;
		}
		
		if (gameMode.GetInsertionPos() == vector.Zero)
		{
			m_sCannotPerformReason = "#COE-Reason_NoInsertionPoint";
			return false;
		}
		
		return true;
	}
}

//------------------------------------------------------------------------------------------------
class COE_DeployUserAction : COE_BaseBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return;
		
		vector spawnPos = gameMode.GetInsertionPoint().GetOrigin();
		
		COE_AO closestAO = gameMode.GetClosestAO(spawnPos);
		if (!closestAO)
			return;
		
		COE_PlayerController.GetInstance().RequestFastTravel(spawnPos, (closestAO.GetOrigin() - spawnPos).ToYaw());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user) 
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		if (gameMode.COE_GetState() == COE_EGameModeState.INTERMISSION)
		{
			m_sCannotPerformReason = "#COE-Reason_NoAOGenerated";
			return false;
		}
		
		SCR_SpawnPoint insertionPoint = gameMode.GetInsertionPoint();
		if (!insertionPoint || !insertionPoint.IsSpawnPointEnabled())
		{
			m_sCannotPerformReason = "#COE-Reason_NoInsertionPoint";
			return false;
		}
		
		return true;
	}
}

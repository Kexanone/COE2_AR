//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_DeploySelectionMenuEntry : COE_SelectionMenuEntry
{
	//------------------------------------------------------------------------------------------------
	override protected void OnPerform()
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return;
		
		KSC_Location location = gameMode.GetCurrentLocation();
		if (!location)
			return;
		
		vector locationPos = location.m_vCenter;
		vector spawnPos = gameMode.GetInsertionPos();
		COE_PlayerController.GetInstance().RequestFastTravel(spawnPos, (locationPos - spawnPos).ToYaw());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed()
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		if (gameMode.COE_GetState() == COE_EGameModeState.INTERMISSION)
			return false;
		
		if (!gameMode.GetInsertionPos())
			return false;
		
		if (vector.DistanceXZ(gameMode.GetMainBasePos(), SCR_PlayerController.GetLocalControlledEntity().GetOrigin()) > 25)
			return false;
		
		return true;
	}
}

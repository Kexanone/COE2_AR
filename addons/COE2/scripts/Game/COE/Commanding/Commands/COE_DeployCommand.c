//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class COE_DeployCommand : SCR_BaseRadialCommand
{
	//------------------------------------------------------------------------------------------------
	override bool Execute(IEntity cursorTarget, IEntity target, vector targetPosition, int playerID, bool isClient)
	{
		if (playerID != SCR_PlayerController.GetLocalPlayerId())
			return true;
		
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		KSC_Location location = gameMode.GetCurrentLocation();
		if (!location)
			return false;
		
		vector locationPos = location.m_vCenter;
		vector spawnPos = gameMode.GetInsertionPoint().GetOrigin();
		COE_PlayerController.GetInstance().RequestFastTravel(spawnPos, (locationPos - spawnPos).ToYaw());
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed(notnull SCR_ChimeraCharacter user)
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		if (gameMode.COE_GetState() == COE_EGameModeState.INTERMISSION)
		{
			m_sCannotPerformReason = "#COE-Reason_NoAO";
			return false;
		}
		
		if (!gameMode.GetInsertionPoint())
		{
			m_sCannotPerformReason = "#COE-Reason_NoInsertionPoint";
			return false;
		}
		
		if (vector.DistanceXZ(gameMode.GetMainBasePos(), SCR_PlayerController.GetLocalControlledEntity().GetOrigin()) > 25)
			return false;
		
		return true;
	}
}

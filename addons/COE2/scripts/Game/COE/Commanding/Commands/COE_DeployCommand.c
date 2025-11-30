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
		
		vector spawnPos = gameMode.GetInsertionPoint().GetOrigin();
		
		vector closestAOPos;
		if (!gameMode.GetClosestAOPos(spawnPos, closestAOPos))
			return false;
		
		COE_PlayerController.GetInstance().RequestFastTravel(spawnPos, (closestAOPos - spawnPos).ToYaw());
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
			m_sCannotPerformReason = "#COE-Reason_NoAOGenerated";
			return false;
		}
		
		SCR_SpawnPoint insertionPoint = gameMode.GetInsertionPoint();
		if (!insertionPoint || !insertionPoint.IsSpawnPointEnabled())
		{
			m_sCannotPerformReason = "#COE-Reason_NoInsertionPoint";
			return false;
		}
		
		if (vector.DistanceXZ(gameMode.GetMainBasePos(), SCR_PlayerController.GetLocalControlledEntity().GetOrigin()) > 25)
			return false;
		
		return true;
	}
}

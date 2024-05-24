//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class COE_SetInsertionPointCommand : SCR_BaseRadialCommand
{
	override bool Execute(IEntity cursorTarget, IEntity target, vector targetPosition, int playerID, bool isClient)
	{
		if (playerID != SCR_PlayerController.GetLocalPlayerId())
			return true;

		targetPosition[1] = SCR_TerrainHelper.GetTerrainY(targetPosition);
		COE_EditorModeCommanderEntity.Request(COE_ECommanderRequest.INSERTION_POINT, targetPosition);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanShowOnMap()
	{
		return (COE_GameMode.GetInstance().COE_GetState() != COE_EGameModeState.INTERMISSION);
	}
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class COE_SetInsertionPointCommand : COE_BaseRadialCommanderCommand
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
	override bool CanBePerformed(notnull SCR_ChimeraCharacter user)
	{
		if (COE_GameMode.GetInstance().COE_GetState() == COE_EGameModeState.INTERMISSION)
		{
			m_sCannotPerformReason = "#COE-Reason_NoAO";
			return false;
		}

		return true;
	}
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class COE_SelectAOCommand : COE_BaseRadialCommanderCommand
{
	//------------------------------------------------------------------------------------------------
	override bool Execute(IEntity cursorTarget, IEntity target, vector targetPosition, int playerID, bool isClient)
	{
		if (playerID != SCR_PlayerController.GetLocalPlayerId())
			return true;
		
		COE_EditorModeCommanderEntity.StartSelectingAO();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed(notnull SCR_ChimeraCharacter user)
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		if (gameMode.COE_GetState() != COE_EGameModeState.INTERMISSION)
		{
			m_sCannotPerformReason = "#COE-Reason_OngoingAO";
			return false;
		}
		
		return true;
	}
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class COE_CancelAOCommand : COE_BaseRadialCommanderCommand
{
	//------------------------------------------------------------------------------------------------
	override bool Execute(IEntity cursorTarget, IEntity target, vector targetPosition, int playerID, bool isClient)
	{
		if (playerID != SCR_PlayerController.GetLocalPlayerId())
			return true;
		
		COE_EditorModeCommanderEntity.Request(COE_ECommanderRequest.CANCEL_AO);
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

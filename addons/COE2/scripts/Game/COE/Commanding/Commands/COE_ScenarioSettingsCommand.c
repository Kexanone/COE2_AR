//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class COE_ScenarioSettingsCommand : COE_BaseRadialCommanderCommand
{
	override bool Execute(IEntity cursorTarget, IEntity target, vector targetPosition, int playerID, bool isClient)
	{
		if (playerID != SCR_PlayerController.GetLocalPlayerId())
			return true;
		
		COE_EditorModeCommanderEntity.StartEditingAttributes(GetGame().GetGameMode());
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed()
	{
		return (COE_GameMode.GetInstance().COE_GetState() == COE_EGameModeState.INTERMISSION);
	}
}

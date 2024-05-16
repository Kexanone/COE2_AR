//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_ScenarioSettingsSelectionMenuEntry : COE_SelectionMenuEntry
{
	//------------------------------------------------------------------------------------------------
	override protected void OnPerform()
	{
		COE_EditorModeCommanderEntity.StartEditingAttributes(GetGame().GetGameMode());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed()
	{
		return (COE_GameMode.GetInstance().COE_GetState() == COE_EGameModeState.INTERMISSION);
	}
}

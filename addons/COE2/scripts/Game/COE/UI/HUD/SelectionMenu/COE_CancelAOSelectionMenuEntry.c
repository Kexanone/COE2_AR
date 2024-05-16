//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_CancelAOSelectionMenuEntry : COE_SelectionMenuEntry
{
	//------------------------------------------------------------------------------------------------
	override protected void OnPerform()
	{
		COE_EditorModeCommanderEntity.Request(COE_ECommanderRequest.CANCEL_AO);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed()
	{
		return (COE_GameMode.GetInstance().COE_GetState() != COE_EGameModeState.INTERMISSION);
	}
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_CancelAOSelectionMenuEntry : COE_SelectionMenuEntry
{
//------------------------------------------------------------------------------------------------
	//! Public method to make entry update itself
	override void Update()
	{
		Enable(COE_GameMode.GetInstance().COE_GetState() != COE_EGameModeState.INTERMISSION);
	}

	//------------------------------------------------------------------------------------------------
	//! Empty methods for deifinition of perform behavior
	override protected void OnPerform()
	{
		COE_EditorModeCommanderEntity.Request(COE_ECommanderRequest.CANCEL_AO);
	}
}

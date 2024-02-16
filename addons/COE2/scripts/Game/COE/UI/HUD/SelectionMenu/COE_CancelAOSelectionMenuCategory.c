//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_CancelAOSelectionMenuCategory : COE_SelectionMenuCategory
{
//------------------------------------------------------------------------------------------------
	//! Public method to make entry update itself
	override void Update()
	{
		Enable(COE_GameMode.GetInstance().COE_GetState() != COE_EGameModeState.INTERMISSION);
	}
}

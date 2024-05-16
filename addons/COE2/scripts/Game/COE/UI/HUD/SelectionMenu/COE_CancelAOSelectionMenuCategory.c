//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_CancelAOSelectionMenuCategory : COE_SelectionMenuCategory
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed()
	{
		return (COE_GameMode.GetInstance().COE_GetState() != COE_EGameModeState.INTERMISSION);
	}
}

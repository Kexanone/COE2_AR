/*!
Scripted selection menu entry based on UI info for storing data into SCR_SelectionMenu
*/

//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_SelectionMenuEntry : SCR_SelectionMenuEntry
{
	//------------------------------------------------------------------------------------------------
	// Custom methods
	//------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------
	//! Public method to make entry update itself
	override void Update()
	{
		Print(7777);
	}

	//------------------------------------------------------------------------------------------------
	//! Empty methods for deifinition of perform behavior
	override protected void OnPerform()
	{
		Print(1111);
	}
}

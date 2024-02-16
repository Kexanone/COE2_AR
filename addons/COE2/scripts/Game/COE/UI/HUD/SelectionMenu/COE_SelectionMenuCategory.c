//------------------------------------------------------------------------------------------------
/*!
Scripted selection menu category, that can contain multiple entries
*/
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_SelectionMenuCategory : SCR_SelectionMenuCategoryEntry
{
	//------------------------------------------------------------------------------------------------
	override protected void Update()
	{
		foreach (SCR_SelectionMenuEntry entry : m_aEntries)
			entry.Update();
	}
}
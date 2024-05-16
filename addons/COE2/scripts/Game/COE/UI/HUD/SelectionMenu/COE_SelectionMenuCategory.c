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
		bool enabled = CanBePerformed();
		
		if (enabled)
		{
			foreach (SCR_SelectionMenuEntry entry : m_aEntries)
				entry.Update();
		};
	
		Enable(enabled);	
	}
	
	bool CanBePerformed()
	{
		return true;
	}
	
	bool CanBeShown()
	{
		return true;
	}
}
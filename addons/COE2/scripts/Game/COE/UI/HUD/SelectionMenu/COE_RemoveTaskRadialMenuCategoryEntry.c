//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_RemoveTaskRadialMenuCategoryEntry : COE_SelectionMenuCategoryEntry
{
	//------------------------------------------------------------------------------------------------
	//! Show available tasks to be removed
	override void OnPerformOnMap(vector position, COE_MapUIElement selectedMapElement)
	{
		m_aEntries.Clear();
		
		COE_MapUILocation selectedLocation = COE_MapUILocation.Cast(selectedMapElement);
		if (!selectedLocation)
			return;
		
		foreach (int i, COE_BaseTaskBuilder builder : selectedLocation.GetTaskBuilders())
		{
			m_aEntries.Insert(new COE_RemoveTaskRadialMenuEntry(selectedLocation, i));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Can only be performed on locations
	override bool CanBeShownOnMap(vector position, COE_MapUIElement selectedMapElement)
	{
		return COE_MapUILocation.Cast(selectedMapElement);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check if there are tasks to be removed
	override bool CanBePerformedOnMap(vector position, COE_MapUIElement selectedMapElement)
	{
		COE_MapUILocation selectedLocation = COE_MapUILocation.Cast(selectedMapElement);
		if (!selectedLocation)
			return false;
		
		return !selectedLocation.GetTaskBuilders().IsEmpty();
	}
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_AddTaskRadialMenuCateogryEntry : COE_SelectionMenuCategoryEntry
{
	//------------------------------------------------------------------------------------------------
	//! Show available tasks to be added
	override void OnPerformOnMap(vector position, COE_MapUIElement selectedMapElement)
	{
		m_aEntries.Clear();
		
		COE_MapUILocation selectedLocation = COE_MapUILocation.Cast(selectedMapElement);
		if (!selectedLocation)
			return;
		
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return;
		
		array<ref COE_BaseTaskBuilder> builders = gameMode.GetAvailableTaskBuilders();
		foreach (COE_BaseTaskBuilder builder : builders)
		{
			m_aEntries.Insert(new COE_AddTaskRadialMenuEntry(builder));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Can only be performed on locations
	override bool CanBeShownOnMap(vector position, COE_MapUIElement selectedMapElement)
	{
		return COE_MapUILocation.Cast(selectedMapElement);
	}
}

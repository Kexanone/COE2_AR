//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_AddTaskRadialMenuEntry : COE_TaskRadialMenuEntryBase
{
	protected COE_BaseTaskBuilder m_TaskBuilder;

	//------------------------------------------------------------------------------------------------
	void COE_AddTaskRadialMenuEntry(COE_BaseTaskBuilder builder)
	{
		m_TaskBuilder = builder;
		InitFromBuilder(builder);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPerformOnMap(vector position, COE_MapUIElement selectedMapElement)
	{
		COE_MapUILocation selectedLocation = COE_MapUILocation.Cast(selectedMapElement);
		if (selectedLocation)
			selectedLocation.AddTaskBuilder(m_TaskBuilder);
	}
}

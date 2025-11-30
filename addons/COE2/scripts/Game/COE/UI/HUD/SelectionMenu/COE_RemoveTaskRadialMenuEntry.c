//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_RemoveTaskRadialMenuEntry : COE_TaskRadialMenuEntryBase
{
	protected int m_iIdx;

	//------------------------------------------------------------------------------------------------
	void COE_RemoveTaskRadialMenuEntry(COE_MapUILocation location, int idx)
	{
		m_iIdx = idx;
		COE_BaseTaskBuilder builder = location.GetTaskBuilders()[idx];
		InitFromBuilder(builder);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPerformOnMap(vector position, COE_MapUIElement selectedMapElement)
	{
		COE_MapUILocation selectedLocation = COE_MapUILocation.Cast(selectedMapElement);
		if (selectedLocation)
			selectedLocation.RemoveTaskBuilder(m_iIdx);
	}
}

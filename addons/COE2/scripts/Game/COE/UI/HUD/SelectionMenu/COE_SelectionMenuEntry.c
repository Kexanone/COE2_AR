//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_SelectionMenuEntry : SCR_SelectionMenuEntry
{
	//------------------------------------------------------------------------------------------------
	void OnPerformOnMap(vector position, COE_MapUIElement selectedMapElement);
	
	//------------------------------------------------------------------------------------------------
	bool CanBeShownOnMap(vector position, COE_MapUIElement selectedMapElement)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanBePerformedOnMap(vector position, COE_MapUIElement selectedMapElement)
	{
		return true;
	}
}

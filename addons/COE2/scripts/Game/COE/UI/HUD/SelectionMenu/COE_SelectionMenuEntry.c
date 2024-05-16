//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_SelectionMenuEntry : SCR_SelectionMenuEntry
{
	//------------------------------------------------------------------------------------------------
	override void Update()
	{
		Enable(CanBePerformed());
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnPerform()
	{
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

//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_TaskRadialMenuEntryBase : COE_SelectionMenuEntry
{
	//------------------------------------------------------------------------------------------------
	protected void InitFromBuilder(COE_BaseTaskBuilder builder)
	{
		SetName(builder.GetTaskName());
		
		Resource taskRes = Resource.Load(builder.GetTaskResourceName());
		if (!taskRes.IsValid())
			return;
		
		IEntitySource taskSrc = SCR_BaseContainerTools.FindEntitySource(taskRes);
		if (!taskSrc)
			return;
		
		BaseContainer infoSrc = taskSrc.GetObject("m_TaskUIInfo");
		string iconImageset, iconName;
		infoSrc.Get("Icon", iconImageset);
		infoSrc.Get("IconSetName", iconName);
		SetIcon(iconImageset, string.Format("%1_UI", iconName));
	}
}

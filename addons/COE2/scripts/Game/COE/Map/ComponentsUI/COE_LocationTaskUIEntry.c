//------------------------------------------------------------------------------
class COE_LocationTaskUIEntry : SCR_ScriptedWidgetComponent
{
	protected ImageWidget m_wIcon;
	protected TextWidget m_wName;
	
	//------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

		m_wIcon = ImageWidget.Cast(w.FindAnyWidget("TaskIcon"));
		m_wName = TextWidget.Cast(w.FindAnyWidget("TaskName"));
	}
	
	//------------------------------------------------------------------------------------------------
	void SetTaskBuilder(COE_BaseTaskBuilder builder)
	{
		m_wName.SetText(builder.GetTaskName());
		
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
		m_wIcon.LoadImageFromSet(0, iconImageset, string.Format("%1_UI", iconName));
	}
}

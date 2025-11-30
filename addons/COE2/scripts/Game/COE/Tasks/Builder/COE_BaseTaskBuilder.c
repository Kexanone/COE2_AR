//------------------------------------------------------------------------------------------------
[SCR_BaseContainerLocalizedTitleField(propertyName: "m_sTaskName")]
class COE_BaseTaskBuilder : KSC_BaseTaskBuilder
{
	[Attribute(desc: "Name of the task in the AO selection menu")]
	protected LocalizedString m_sTaskName;
	
	//------------------------------------------------------------------------------------------------
	KSC_BaseTask Build(COE_AO ao) { return null; }
	
	//------------------------------------------------------------------------------------------------
	LocalizedString GetTaskName()
	{
		return m_sTaskName;
	}
}

//------------------------------------------------------------------------------------------------
class COE_BaseTaskBuilder : KSC_BaseTaskBuilder
{
	protected static const int MAX_ATTEMPTS = 10000;
	
	//------------------------------------------------------------------------------------------------
	KSC_BaseTask Build(COE_AO ao) { return null; }
	
	//------------------------------------------------------------------------------------------------
	LocalizedString GetTaskName()
	{
		return "";
	}
}

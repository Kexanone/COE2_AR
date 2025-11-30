//------------------------------------------------------------------------------------------------
[SCR_BaseContainerLocalizedTitleField(propertyName: "m_sTaskName")]
class COE_ClearAreaTaskBuilder : COE_BaseTaskBuilder
{
	[Attribute(defvalue: "0.666", uiwidget: UIWidgets.Slider, desc: "Prefab name of the intel object", params: "0 1")]
	protected float m_fFriendlyRatioThreshold;
	
	//------------------------------------------------------------------------------------------------
	override KSC_BaseTask Build(COE_AO ao)
	{
		KSC_ClearAreaTask task = KSC_ClearAreaTask.Cast(SpawnTaskEntity(ao.GetOrigin()));
		if (!task)
			return null;
		
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return null;
		
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return null;
		
		task.SetParams(factionManager.GetPlayerFaction(), gameMode.GetAORadius(), m_fFriendlyRatioThreshold);
		return task;
	}
}

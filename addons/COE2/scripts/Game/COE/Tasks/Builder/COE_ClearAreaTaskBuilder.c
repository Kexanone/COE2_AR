//------------------------------------------------------------------------------------------------
class COE_ClearAreaTaskBuilder : COE_BaseTaskBuilder
{
	[Attribute(defvalue: "0.666", uiwidget: UIWidgets.Slider, desc: "Prefab name of the intel object", params: "0 1")]
	protected float m_fFriendlyRatioThreshold;
	
	//------------------------------------------------------------------------------------------------
	override KSC_BaseTask Build(COE_AO ao)
	{
		KSC_ClearAreaTaskSupportEntity supportEntity = KSC_ClearAreaTaskSupportEntity.Cast(GetTaskManager().FindSupportEntity(KSC_ClearAreaTaskSupportEntity));
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		
		return KSC_BaseTask.Cast(supportEntity.CreateTask(factionManager.GetPlayerFaction(), gameMode.GetCurrentLocation().m_vCenter, gameMode.GetAORadius(), m_fFriendlyRatioThreshold));
	}
	
	//------------------------------------------------------------------------------------------------
	override LocalizedString GetTaskName()
	{
		return "Clear Area";
	}
}

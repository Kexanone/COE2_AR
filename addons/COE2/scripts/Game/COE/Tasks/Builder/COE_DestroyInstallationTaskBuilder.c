//------------------------------------------------------------------------------------------------
class COE_DestroyInstallationTaskBuilder : COE_BaseTaskBuilder
{
	//------------------------------------------------------------------------------------------------
	override KSC_BaseTask Build(COE_AO ao)
	{
		KSC_DestroyObjectTaskSupportEntity supportEntity = KSC_DestroyObjectTaskSupportEntity.Cast(GetTaskManager().FindSupportEntity(KSC_DestroyObjectTaskSupportEntity));
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		
		array<ResourceName> entries = {};
		factionManager.GetFactionEntityListWithLabel(factionManager.GetEnemyFaction(), EEntityCatalogType.COMPOSITION, EEditableEntityLabel.KSC_TRAIT_HVT, entries);
		if (entries.IsEmpty())
			return null;
				
		IEntity hvt = KSC_GameTools.SpawnStructurePrefab(entries.GetRandomElement(), ao.GetOrigin(), Math.RandomFloat(0, 360));
		ao.AddEntity(hvt);
				
		return KSC_BaseTask.Cast(supportEntity.CreateTask(factionManager.GetPlayerFaction(), hvt));
	}
	
	//------------------------------------------------------------------------------------------------
	override LocalizedString GetTaskName()
	{
		return "Destroy Installation";
	}
}

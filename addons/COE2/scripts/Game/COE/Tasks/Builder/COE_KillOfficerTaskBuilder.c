//------------------------------------------------------------------------------------------------
class COE_KillOfficerTaskBuilder : COE_BaseTaskBuilder
{
	//------------------------------------------------------------------------------------------------
	override KSC_BaseTask Build(COE_AO ao)
	{
		KSC_KillTaskSupportEntity supportEntity = KSC_KillTaskSupportEntity.Cast(GetTaskManager().FindSupportEntity(KSC_KillTaskSupportEntity));
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		
		array<ResourceName> entries = {};
		factionManager.GetFactionEntityListWithLabel(factionManager.GetEnemyFaction(), EEntityCatalogType.COMPOSITION, EEditableEntityLabel.KSC_TRAIT_TENT, entries);
		if (entries.IsEmpty())
			return null;
		
		ao.AddEntity(KSC_GameTools.SpawnStructurePrefab(entries.GetRandomElement(), ao.GetOrigin(), Math.RandomFloat(0, 360)));
		
		entries.Clear();
		factionManager.GetFactionEntityListWithLabel(factionManager.GetEnemyFaction(), EEntityCatalogType.CHARACTER, EEditableEntityLabel.KSC_TRAIT_HVT, entries);
		if (entries.IsEmpty())
			return null;
		
		SCR_ChimeraCharacter hvt = KSC_GameTools.SpawnCharacterPrefab(entries.GetRandomElement(), ao.GetOrigin(), Math.RandomFloat(0, 360));
		ao.AddEntity(hvt);
		
		return KSC_BaseTask.Cast(supportEntity.CreateTask(factionManager.GetPlayerFaction(), hvt));
	}
	
	//------------------------------------------------------------------------------------------------
	override LocalizedString GetTaskName()
	{
		return "Kill Officer";
	}
}

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
		array<Faction> factionsToConsider = {factionManager.GetEnemyFaction(), factionManager.GetFactionByKey("FIA")}; // If not available for selection faction, get the ones of FIA
		foreach (Faction faction : factionsToConsider)
		{
			factionManager.GetFactionEntityListWithLabels(faction, EEntityCatalogType.COMPOSITION, {EEditableEntityLabel.SERVICE_ANTENNA, EEditableEntityLabel.TRAIT_DESTRUCTABLE}, {}, entries);
			
			if (!entries.IsEmpty())
				break;
		}
		
		if (entries.IsEmpty())
			return null;
				
		IEntity hvt = ao.SpawnInRandomFlatSlot(entries.GetRandomElement(), EEditableEntityLabel.SLOT_FLAT_SMALL, false);
		if (!hvt)
			return null;
		
		ao.AddPositionToDefend(hvt.GetOrigin());	
		return KSC_BaseTask.Cast(supportEntity.CreateTask(factionManager.GetPlayerFaction(), hvt));
	}
	
	//------------------------------------------------------------------------------------------------
	override LocalizedString GetTaskName()
	{
		return "Destroy Installation";
	}
}

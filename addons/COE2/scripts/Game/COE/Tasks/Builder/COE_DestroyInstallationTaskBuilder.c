//------------------------------------------------------------------------------------------------
[SCR_BaseContainerLocalizedTitleField(propertyName: "m_sTaskName")]
class COE_DestroyInstallationTaskBuilder : COE_BaseTaskBuilder
{
	//------------------------------------------------------------------------------------------------
	override KSC_BaseTask Build(COE_AO ao)
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return null;
		
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return null;
		
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
		
		IEntity hvt;
		
		while (!entries.IsEmpty())
		{
			ResourceName entry = entries.GetRandomElement();
			EEditableEntityLabel slotLabel;
			
			if (KSC_TerrainSlotTools.GetSlotLabel(entry, slotLabel))
			{
				hvt = ao.SpawnInRandomFlatSlot(entry, slotLabel, false);
				if (hvt)
					break;
			}
			
			entries.RemoveItem(entry);
		}
			
		if (!hvt)
			return null;
		
		ao.AddPositionToDefend(hvt.GetOrigin());
		
		KSC_DestroyObjectTask task = KSC_DestroyObjectTask.Cast(SpawnTaskEntity(hvt.GetOrigin()));
		if (!task)
			return null;
		
		task.SetParams(factionManager.GetPlayerFaction(), hvt);
		return task;
	}
}

//------------------------------------------------------------------------------------------------
[SCR_BaseContainerLocalizedTitleField(propertyName: "m_sTaskName")]
class COE_DestroyCacheTaskBuilder : COE_BaseTaskBuilder
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
		
		IEntity structure; vector pos;
		Tuple2<IEntity, array<ref PointInfo>> slot = ao.GetRandomBuildingWithSlots(EEditableEntityLabel.ROLE_SCOUT);
		if (slot)
		{
			structure = slot.param1;
			PointInfo point = slot.param2.GetRandomElement();
			point.Init(structure);
			vector transform[4];
			point.GetWorldTransform(transform);
			pos = transform[3];
		}
		else
		{
			array<Faction> factionsToConsider = {factionManager.GetEnemyFaction(), factionManager.GetFactionByKey("FIA")}; // If not available for selection faction, get the ones of FIA
			foreach (Faction faction : factionsToConsider)
			{
				factionManager.GetFactionEntityListWithLabel(faction, EEntityCatalogType.COMPOSITION, EEditableEntityLabel.KSC_TRAIT_TENT, entries);
				
				if (!entries.IsEmpty())
					break;
			}
			
			if (entries.IsEmpty())
				return null;
			
			structure = ao.SpawnInRandomFlatSlot(entries.GetRandomElement(), EEditableEntityLabel.SLOT_FLAT_SMALL, false);
			pos = structure.GetOrigin();
			ao.AddPositionToDefend(pos);
		}
		
		if (!structure)
			return null;
		
		entries.Clear();
		array<Faction> factionsToConsider = {factionManager.GetEnemyFaction(), factionManager.GetFactionByKey("FIA")}; // If not available for selection faction, get the ones of FIA
		foreach (Faction faction : factionsToConsider)
		{
			factionManager.GetFactionEntityListWithLabels(faction, EEntityCatalogType.COMPOSITION, {EEditableEntityLabel.TRAIT_ARSENAL, EEditableEntityLabel.TRAIT_DESTRUCTABLE}, {}, entries);
			
			if (!entries.IsEmpty())
				break;
		}
		
		if (entries.IsEmpty())
			return null;
		
		float y = pos[1];
		KSC_CircleArea area = KSC_CircleArea(pos, 5);
		pos = area.SamplePointInArea();
		pos[1] = y;
		SCR_WorldTools.FindEmptyTerrainPosition(pos, pos, 5);
		IEntity cache = KSC_GameTools.SpawnPrefab(entries.GetRandomElement(), pos, Math.RandomFloat(0, 360));
		ao.AddEntity(cache);
		
		KSC_DestroyObjectTask task = KSC_DestroyObjectTask.Cast(SpawnTaskEntity(cache.GetOrigin()));
		if (!task)
			return null;
		
		task.SetParams(factionManager.GetPlayerFaction(), cache);
		return task;
	}
}

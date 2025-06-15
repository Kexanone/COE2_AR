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
		factionManager.GetFactionEntityListWithLabel(factionManager.GetEnemyFaction(), EEntityCatalogType.CHARACTER, EEditableEntityLabel.ROLE_LEADER, entries);
		if (entries.IsEmpty())
			return null;
		
		array<ResourceName> filteredEntities = {};
		filteredEntities.Reserve(entries.Count());
		foreach (ResourceName entry : entries)
		{
			if (entry.Contains("Officer"))
				filteredEntities.Insert(entry);
		}
		
		if (!filteredEntities.IsEmpty())
			entries = filteredEntities;
		
		SCR_ChimeraCharacter hvt = KSC_GameTools.SpawnCharacterPrefab(entries.GetRandomElement(), pos, Math.RandomFloat(0, 360));
		ao.AddEntity(hvt);
		AIGroup group = KSC_AIHelper.GetGroup(hvt);
		KSC_AITasks.Defend(group, pos, 5);
		ao.AddGroup(group);
		return KSC_BaseTask.Cast(supportEntity.CreateTask(factionManager.GetPlayerFaction(), hvt));
	}
	
	//------------------------------------------------------------------------------------------------
	override LocalizedString GetTaskName()
	{
		return "Kill Officer";
	}
}

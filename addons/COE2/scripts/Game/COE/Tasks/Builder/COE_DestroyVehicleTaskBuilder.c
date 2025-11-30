//------------------------------------------------------------------------------------------------
[SCR_BaseContainerLocalizedTitleField(propertyName: "m_sTaskName")]
class COE_DestroyVehicleTaskBuilder : COE_BaseTaskBuilder
{
	[Attribute(defvalue: "0.5", desc: "Chance for spawning a helo if a helipad is available", params: "0 1")]
	protected float m_fHeloSpawnChance;
	
	//------------------------------------------------------------------------------------------------
	override KSC_BaseTask Build(COE_AO ao)
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return null;
		
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return null;
		
		if (Math.RandomFloat(0, 1) < m_fHeloSpawnChance)
		{
			Tuple2<IEntity, array<ref PointInfo>> slot = ao.GetRandomBuildingWithSlots(EEditableEntityLabel.VEHICLE_HELICOPTER, addGarrison: true, blockSlot: true);
			if (slot)
			{
				array<ResourceName> entries = {};
				factionManager.GetFactionEntityListWithLabel(factionManager.GetEnemyFaction(), EEntityCatalogType.VEHICLE, EEditableEntityLabel.VEHICLE_HELICOPTER, entries);
				if (!entries.IsEmpty())
					return SpawnHeloDestroyTaskInSlot(ao, factionManager, slot, entries.GetRandomElement());
			}
		}
		
		array<EEditableEntityLabel> labels = {
			EEditableEntityLabel.TRAIT_MANAGEMENT_BASE,
			EEditableEntityLabel.TRAIT_REPAIRING,
			EEditableEntityLabel.TRAIT_REFUELING,
			EEditableEntityLabel.TRAIT_REARMING,
			EEditableEntityLabel.TRAIT_RADIO,
			EEditableEntityLabel.VEHICLE_APC
		};
		
		array<ResourceName> entries = {};
		
		while (!labels.IsEmpty())
		{
			EEditableEntityLabel label = labels.GetRandomElement();
			
			factionManager.GetFactionEntityListWithLabels(factionManager.GetEnemyFaction(), EEntityCatalogType.VEHICLE,
				includedLabels: {label},
				excludedLabels: {EEditableEntityLabel.VEHICLE_HELICOPTER},
			entries);
			
			if (!entries.IsEmpty())
				break;
			
			labels.RemoveItem(label);
		}
		
		// Consider light armed vehicles if no others were found
		if (entries.IsEmpty())
			factionManager.GetFactionEntityListWithLabels(factionManager.GetEnemyFaction(), EEntityCatalogType.VEHICLE,
				includedLabels: {EEditableEntityLabel.TRAIT_ARMED},
				excludedLabels: {EEditableEntityLabel.VEHICLE_HELICOPTER},
			entries);
		
		if (entries.IsEmpty())
			return null;
		
		vector pos;
		float rotation;
		if (!ao.GetRandomParkingSlot(pos, rotation))
			return null;
				
		IEntity hvt = KSC_GameTools.SpawnVehiclePrefab(entries.GetRandomElement(), pos, rotation);
		if (!hvt)
			return null;
			
		ao.AddEntity(hvt);
		ao.AddPositionToDefend(hvt.GetOrigin());
		ao.SpawnTurretOccupants(hvt);
				
		KSC_DestroyObjectTask task = KSC_DestroyObjectTask.Cast(SpawnTaskEntity(hvt.GetOrigin()));
		if (!task)
			return null;
		
		task.SetParams(factionManager.GetPlayerFaction(), hvt);
		return task;
	}
	
	//------------------------------------------------------------------------------------------------
	protected KSC_BaseTask SpawnHeloDestroyTaskInSlot(COE_AO ao, COE_FactionManager factionManager, Tuple2<IEntity, array<ref PointInfo>> slot, ResourceName heloPrefab)
	{
		PointInfo point = slot.param2.GetRandomElement();
		point.Init(slot.param1);
		vector transform[4];
		point.GetWorldTransform(transform);
		
		IEntity hvt = KSC_GameTools.SpawnPrefab(heloPrefab, transform);
		ao.AddEntity(hvt);
		
		KSC_DestroyObjectTask task = KSC_DestroyObjectTask.Cast(SpawnTaskEntity(hvt.GetOrigin()));
		if (!task)
			return null;
		
		task.SetParams(factionManager.GetPlayerFaction(), hvt);
		return task;
	}
}

//------------------------------------------------------------------------------------------------
class COE_DestroyVehicleTaskBuilder : COE_BaseTaskBuilder
{
	//------------------------------------------------------------------------------------------------
	override KSC_BaseTask Build(COE_AO ao)
	{
		KSC_DestroyObjectTaskSupportEntity supportEntity = KSC_DestroyObjectTaskSupportEntity.Cast(GetTaskManager().FindSupportEntity(KSC_DestroyObjectTaskSupportEntity));
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		
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
		return KSC_BaseTask.Cast(supportEntity.CreateTask(factionManager.GetPlayerFaction(), hvt));
	}
	
	//------------------------------------------------------------------------------------------------
	override LocalizedString GetTaskName()
	{
		return "Destroy Vehicle";
	}
}

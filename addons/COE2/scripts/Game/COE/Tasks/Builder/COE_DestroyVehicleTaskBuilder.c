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
		
		int attempt = 0;
		while (attempt < MAX_ATTEMPTS)
		{
			if (labels.IsEmpty())
				return null;
			
			EEditableEntityLabel label = labels.GetRandomElement();
			
			factionManager.GetFactionEntityListWithLabel(factionManager.GetEnemyFaction(), EEntityCatalogType.VEHICLE, label, entries);
			if (entries.IsEmpty())
			{
				labels.RemoveItem(label);
				continue;
			}
			
			attempt++;
		}
		
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

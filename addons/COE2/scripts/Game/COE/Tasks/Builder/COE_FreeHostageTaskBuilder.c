//------------------------------------------------------------------------------------------------
class COE_FreeHostageTaskBuilder : COE_BaseTaskBuilder
{
	//------------------------------------------------------------------------------------------------
	override KSC_BaseTask Build(COE_AO ao)
	{
		KSC_FreeHostageTaskSupportEntity supportEntity = KSC_FreeHostageTaskSupportEntity.Cast(GetTaskManager().FindSupportEntity(KSC_FreeHostageTaskSupportEntity));
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
		factionManager.GetFactionEntityListWithLabel(factionManager.GetPlayerFaction(), EEntityCatalogType.CHARACTER, EEditableEntityLabel.TRAIT_UNARMED, entries);
		if (entries.IsEmpty())
			return null;
		
		float y = pos[1];
		KSC_CircleArea area = KSC_CircleArea(pos, 5);
		pos = area.SamplePointInArea();
		pos[1] = y;
		SCR_WorldTools.FindEmptyTerrainPosition(pos, pos, 5);
		SCR_ChimeraCharacter hostage = KSC_GameTools.SpawnCharacterPrefab(entries.GetRandomElement(), pos, Math.RandomFloat(0, 360));
		ao.AddEntity(hostage);
		SCR_CharacterControllerComponent hostageCharController = SCR_CharacterControllerComponent.Cast(hostage.GetCharacterController());
		hostageCharController.ACE_Captives_SetCaptive(true);
		return KSC_BaseTask.Cast(supportEntity.CreateTask(factionManager.GetPlayerFaction(), hostage));
	}
	
	//------------------------------------------------------------------------------------------------
	override LocalizedString GetTaskName()
	{
		return "Free Hostage";
	}
}

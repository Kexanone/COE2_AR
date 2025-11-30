//------------------------------------------------------------------------------------------------
[SCR_BaseContainerLocalizedTitleField(propertyName: "m_sTaskName")]
class COE_FindIntelTaskBuilder : COE_BaseTaskBuilder
{
	[Attribute(defvalue: "{6BDCF13C3C65DC1F}Prefabs/Items/Misc/IntelligenceFolder_E_01/KSC_IntelligenceFolder_E_01.et", desc: "Prefab name of the intel object")]
	protected ResourceName m_sIntelPrefabName;
	
	//------------------------------------------------------------------------------------------------
	override KSC_BaseTask Build(COE_AO ao)
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return null;
		
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return null;
		
		IEntity structure; array<IEntity> tables = {};
		Tuple2<IEntity, array<ref PointInfo>> entry = ao.GetRandomBuildingWithSlots(EEditableEntityLabel.ROLE_SCOUT);
		if (entry)
		{
			structure = entry.param1;
			FindTables(structure, tables);
		}
		else
		{
			array<ResourceName> entries = {};
			
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
			KSC_CompositionHelper.GetChildrenByXobSubstring(structure, "Table", tables);
			KSC_CompositionHelper.GetChildrenByXobSubstring(structure, "Desk", tables);
			ao.AddPositionToDefend(structure.GetOrigin());
		}			
		
		if (!structure || tables.IsEmpty())
			return null;
		
		IEntity table = tables.GetRandomElement();
		vector mins, maxs;
		table.GetWorldBounds(mins, maxs);
				
		IEntity intel = KSC_GameTools.SpawnPrefab(m_sIntelPrefabName, Vector((maxs[0] + mins[0]) / 2, maxs[1] + 0.5, (maxs[2] + mins[2]) / 2), Math.RandomFloat(0, 360));
		intel.GetPhysics().SetActive(ActiveState.ACTIVE);
		ao.AddEntity(intel);
		
		KSC_PickUpItemTask task = KSC_PickUpItemTask.Cast(SpawnTaskEntity(intel.GetOrigin()));
		if (!task)
			return null;
		
		task.SetParams(factionManager.GetPlayerFaction(), intel);
		return task;
	}
	
	protected ref array<IEntity> m_aTables;
	
	//------------------------------------------------------------------------------------------------
	protected void FindTables(IEntity structure, inout notnull array<IEntity> tables)
	{
		m_aTables = {};
		vector mins, maxs;
		structure.GetBounds(mins, maxs);
		vector transform[4];
		structure.GetWorldTransform(transform);
		GetGame().GetWorld().QueryEntitiesByOBB(mins, maxs, transform, QueryTablesCallback);
		tables.InsertAll(m_aTables);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool QueryTablesCallback(IEntity entity)
	{
		VObject vObject = entity.GetVObject();
		if (!vObject)
			return true;
		
		ResourceName xobName = vObject.GetResourceName();
		xobName.ToLower();
		if (xobName.IndexOf("table") >= 0 || xobName.IndexOf("desk") >= 0)
			m_aTables.Insert(entity);
		
		return true;
	}
}

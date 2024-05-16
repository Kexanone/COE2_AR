//------------------------------------------------------------------------------------------------
class COE_FindIntelTaskBuilder : COE_BaseTaskBuilder
{
	[Attribute(defvalue: "{6BDCF13C3C65DC1F}Prefabs/Items/Misc/IntelligenceFolder_E_01/KSC_IntelligenceFolder_E_01.et", desc: "Prefab name of the intel object")]
	protected ResourceName m_sIntelPrefabName;
	
	//------------------------------------------------------------------------------------------------
	override KSC_BaseTask Build(COE_AO ao)
	{
		KSC_FindIntelTaskSupportEntity supportEntity = KSC_FindIntelTaskSupportEntity.Cast(GetTaskManager().FindSupportEntity(KSC_FindIntelTaskSupportEntity));
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		
		array<ResourceName> entries = {};
		factionManager.GetFactionEntityListWithLabel(factionManager.GetEnemyFaction(), EEntityCatalogType.COMPOSITION, EEditableEntityLabel.KSC_TRAIT_TENT, entries);
		if (entries.IsEmpty())
			return null;
				
		IEntity tent = KSC_GameTools.SpawnStructurePrefab(entries.GetRandomElement(), ao.GetOrigin(), Math.RandomFloat(0, 360));
		ao.AddEntity(tent);
				
		array<IEntity> tables = KSC_CompositionHelper.GetChildrenByXobSubstring(tent, "Table");
		if (tables.IsEmpty())
			return null;
				
		IEntity table = tables.GetRandomElement();
		vector mins, maxs;
		table.GetWorldBounds(mins, maxs);
				
		IEntity intel = KSC_GameTools.SpawnPrefab(m_sIntelPrefabName, Vector((maxs[0] + mins[0]) / 2, maxs[1] + 0.5, (maxs[2] + mins[2]) / 2), Math.RandomFloat(0, 360));
		intel.GetPhysics().SetActive(ActiveState.ACTIVE);
		ao.AddEntity(intel);
		
		return KSC_BaseTask.Cast(supportEntity.CreateTask(factionManager.GetPlayerFaction(), intel));
	}
	
	//------------------------------------------------------------------------------------------------
	override LocalizedString GetTaskName()
	{
		return "Find Intel";
	}
}

//------------------------------------------------------------------------------------------------
[SCR_BaseContainerLocalizedTitleField(propertyName: "m_sTaskName")]
class COE_DemineEffectModuleTaskBuilder : COE_BaseTaskBuilder
{
	[Attribute(defvalue: "{EBEF621BD8DFFC60}PrefabsEditable/EffectsModules/Mine/KSC_DemineEffectModule_MineField_AP_Small_US.et")]
	protected ResourceName m_sAPMineFieldPrefabName;
	
	[Attribute(defvalue: "{7B05535D7A24B141}PrefabsEditable/EffectsModules/Mine/KSC_DemineEffectModule_MineField_AT_Small_USSR.et")]
	protected ResourceName m_sATMineFieldPrefabName;
	
	//------------------------------------------------------------------------------------------------
	override KSC_BaseTask Build(COE_AO ao)
	{
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return null;
		
		IEntity mineField;
		
		if (Math.RandomFloat(0, 1) > 0.5)
		{
			mineField = TrySpawnAPMineField(ao);
			if (!mineField)
				mineField = TrySpawnATMineField(ao);
		}
		else
		{
			mineField = TrySpawnATMineField(ao);
			if (!mineField)
				mineField = TrySpawnAPMineField(ao);
		}
		
		if (!mineField)
			return null;	

		ao.AddEntity(mineField);
		
		KSC_DemineEffectModuleTask task = KSC_DemineEffectModuleTask.Cast(SpawnTaskEntity(mineField.GetOrigin()));
		if (!task)
			return null;
		
		task.SetParams(factionManager.GetPlayerFaction(), mineField);
		return task;
	}
	
	//------------------------------------------------------------------------------------------------
	protected IEntity TrySpawnAPMineField(COE_AO ao)
	{
		return ao.SpawnInRandomRoadSlot(m_sAPMineFieldPrefabName, EEditableEntityLabel.SLOT_FLAT_SMALL, false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected IEntity TrySpawnATMineField(COE_AO ao)
	{
		array<EEditableEntityLabel> labels = {EEditableEntityLabel.SLOT_ROAD_SMALL, EEditableEntityLabel.SLOT_ROAD_MEDIUM, EEditableEntityLabel.SLOT_ROAD_LARGE};
		
		while (!labels.IsEmpty())
		{
			EEditableEntityLabel label = labels.GetRandomElement();
			
			IEntity mineField = ao.SpawnInRandomRoadSlot(m_sATMineFieldPrefabName, label);
			if (mineField)
				return mineField;
			
			labels.RemoveItem(label);
		}
		
		return null;
	}
}

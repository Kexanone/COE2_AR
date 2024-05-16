//------------------------------------------------------------------------------------------------
class COE_MainBaseEntityClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
class COE_MainBaseEntity : GenericEntity
{
	void SetFaction(SCR_Faction faction)
	{
		IEntity entity = GetChildren();
		while (entity)
		{
			SCR_FactionAffiliationComponent.SetFaction(entity, faction);
			
			SCR_EditableSpawnPointComponent editableSpawn = SCR_EditableSpawnPointComponent.Cast(entity.FindComponent(SCR_EditableSpawnPointComponent));
			if (editableSpawn)
				editableSpawn.UpdateNearestLocation(entity.GetOrigin());
			
			entity = entity.GetSibling();
		};
	}
}
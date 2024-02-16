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
			SCR_FactionAffiliationComponent factionAffiliation = SCR_FactionAffiliationComponent.Cast(entity.FindComponent(SCR_FactionAffiliationComponent));
			if (factionAffiliation)
				factionAffiliation.SetAffiliatedFaction(faction);
			
			SCR_EditableSpawnPointComponent editableSpawn = SCR_EditableSpawnPointComponent.Cast(entity.FindComponent(SCR_EditableSpawnPointComponent));
			if (editableSpawn)
				editableSpawn.UpdateNearestLocation(entity.GetOrigin());
			
			entity = entity.GetSibling();
		};
	}
}
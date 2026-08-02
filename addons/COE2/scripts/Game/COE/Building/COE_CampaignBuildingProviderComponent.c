//------------------------------------------------------------------------------------------------
class COE_CampaignBuildingProviderComponentClass : SCR_CampaignBuildingProviderComponentClass
{
}

//------------------------------------------------------------------------------------------------
class COE_CampaignBuildingProviderComponent : SCR_CampaignBuildingProviderComponent
{
	protected bool m_bIsBlockedByEnemy;
	
	//------------------------------------------------------------------------------------------------
	override bool IsEnemyFaction(notnull SCR_ChimeraCharacter char)
	{
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return false;
		
		FactionAffiliationComponent factionComponent = FactionAffiliationComponent.Cast(char.FindComponent(FactionAffiliationComponent));
		if (!factionComponent)
			return false;
		
		return factionManager.GetEnemyFaction() == factionComponent.GetAffiliatedFaction();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsBlockedByEnemy()
	{
		m_bIsBlockedByEnemy = false;
		GetGame().GetWorld().QueryEntitiesBySphere(GetOwner().GetOrigin(), m_fBuildingRadius, CheckForEnemyCallback, null, EQueryEntitiesFlags.DYNAMIC);
		return m_bIsBlockedByEnemy;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool CheckForEnemyCallback(IEntity entity)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(entity);
		if (!char)
			return true;
		
		if (IsEnemyFaction(char))
			m_bIsBlockedByEnemy = true;
		
		return !m_bIsBlockedByEnemy;
	}
}

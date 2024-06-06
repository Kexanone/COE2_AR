//------------------------------------------------------------------------------------------------
modded class SCR_CampaignBuildingProviderComponent : SCR_MilitaryBaseLogicComponent
{
	//------------------------------------------------------------------------------------------------
	override bool IsEnemyFaction(notnull ChimeraCharacter char)
	{
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return super.IsEnemyFaction(char);
		
		FactionAffiliationComponent factionComponent = FactionAffiliationComponent.Cast(char.FindComponent(FactionAffiliationComponent));
		if (!factionComponent)
			return false;
		
		return factionManager.GetEnemyFaction() == factionComponent.GetAffiliatedFaction();
	}
	
	protected bool m_bCOE_IsBlockedByEnemy;
	
	//------------------------------------------------------------------------------------------------
	bool COE_IsBlockedByEnemy()
	{
		m_bCOE_IsBlockedByEnemy = false;
		GetGame().GetWorld().QueryEntitiesBySphere(GetOwner().GetOrigin(), m_fBuildingRadius, COE_EvaluateEntityCallback, null, EQueryEntitiesFlags.DYNAMIC);
		return m_bCOE_IsBlockedByEnemy;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool COE_EvaluateEntityCallback(IEntity entity)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(entity);
		if (!char)
			return true;
		
		if (IsEnemyFaction(char))
			m_bCOE_IsBlockedByEnemy = true;
		
		return !m_bCOE_IsBlockedByEnemy;
	}
}

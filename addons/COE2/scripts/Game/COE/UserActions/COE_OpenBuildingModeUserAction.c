//------------------------------------------------------------------------------------------------
class COE_OpenBuildingModeUserAction : COE_BaseCommanderBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{		
		COE_CampaignBuildingProviderComponent provider = COE_CampaignBuildingProviderComponent.Cast(COE_GameMode.GetInstance().GetInsertionPoint().FindComponent(COE_CampaignBuildingProviderComponent));
		provider.RequestEnterBuildingMode(SCR_PlayerController.GetLocalPlayerId(), true);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user) 
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		IEntity insertionPoint = gameMode.GetInsertionPoint();
		if (!insertionPoint)
		{
			m_sCannotPerformReason = "#COE-Reason_NoInsertionPoint";
			return false;
		}
		
		COE_CampaignBuildingProviderComponent provider = COE_CampaignBuildingProviderComponent.Cast(insertionPoint.FindComponent(COE_CampaignBuildingProviderComponent));
		if (provider.IsBlockedByEnemy())
		{
			m_sCannotPerformReason = "#AR-Campaign_Action_ShowBuildPreviewEnemyPresence";
			return false;
		}
		
		return true;
	}
}

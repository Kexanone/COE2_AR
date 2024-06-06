//------------------------------------------------------------------------------------------------
class COE_OpenBuildingModeUserAction : COE_BaseCommanderBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{		
		SCR_CampaignBuildingProviderComponent provider = SCR_CampaignBuildingProviderComponent.Cast(COE_GameMode.GetInstance().GetInsertionPoint().FindComponent(SCR_CampaignBuildingProviderComponent));
		provider.RequestBuildingMode(SCR_PlayerController.GetLocalPlayerId(), true);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user) 
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		if (gameMode.COE_GetState() == COE_EGameModeState.INTERMISSION)
		{
			m_sCannotPerformReason = "#COE-Reason_NoAO";
			return false;
		}
		
		IEntity insertionPoint = gameMode.GetInsertionPoint();
		if (!insertionPoint)
		{
			m_sCannotPerformReason = "#COE-Reason_NoInsertionPoint";
			return false;
		}
		
		SCR_CampaignBuildingProviderComponent provider = SCR_CampaignBuildingProviderComponent.Cast(insertionPoint.FindComponent(SCR_CampaignBuildingProviderComponent));
		if (provider.COE_IsBlockedByEnemy())
		{
			m_sCannotPerformReason = "#AR-Campaign_Action_ShowBuildPreviewEnemyPresence";
			return false;
		}
		
		return true;
	}
}

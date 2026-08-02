//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class COE_OpenBuildingModeCommand : COE_BaseRadialCommanderCommand
{
	//------------------------------------------------------------------------------------------------
	override bool Execute(IEntity cursorTarget, IEntity groupEnt, vector targetPosition, int playerID, bool isClient)
	{
		if (playerID != SCR_PlayerController.GetLocalPlayerId())
			return true;
		
		SCR_CampaignBuildingProviderComponent provider = SCR_CampaignBuildingProviderComponent.Cast(COE_GameMode.GetInstance().GetInsertionPoint().FindComponent(SCR_CampaignBuildingProviderComponent));
		provider.RequestEnterBuildingMode(SCR_PlayerController.GetLocalPlayerId(), true);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed(notnull SCR_ChimeraCharacter user)
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

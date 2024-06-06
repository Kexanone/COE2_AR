//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class COE_OpenBuildingModeCommand : SCR_BaseRadialCommand
{
	//------------------------------------------------------------------------------------------------
	override bool Execute(IEntity cursorTarget, IEntity target, vector targetPosition, int playerID, bool isClient)
	{
		if (playerID != SCR_PlayerController.GetLocalPlayerId())
			return true;
		
		SCR_CampaignBuildingProviderComponent provider = SCR_CampaignBuildingProviderComponent.Cast(COE_GameMode.GetInstance().GetInsertionPoint().FindComponent(SCR_CampaignBuildingProviderComponent));
		provider.RequestBuildingMode(SCR_PlayerController.GetLocalPlayerId(), true);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed()
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		if (gameMode.COE_GetState() == COE_EGameModeState.INTERMISSION)
			return false;
		
		IEntity insertionPoint = gameMode.GetInsertionPoint();
		if (!insertionPoint)
			return false;
		
		SCR_CampaignBuildingProviderComponent provider = SCR_CampaignBuildingProviderComponent.Cast(insertionPoint.FindComponent(SCR_CampaignBuildingProviderComponent));
		if (provider.COE_IsBlockedByEnemy())
			return false;
		
		return true;
	}
}

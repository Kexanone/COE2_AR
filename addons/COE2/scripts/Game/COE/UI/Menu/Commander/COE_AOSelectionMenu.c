class COE_AOSelectionMenu : SCR_CommandPostMapMenuUI
{
	//------------------------------------------------------------------------------------------------
	override protected void OpenMap()
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return;

		COE_MapConfigComponent configComp = COE_MapConfigComponent.Cast(gameMode.FindComponent(COE_MapConfigComponent));
		if (!configComp)
			return;

		MapConfiguration mapConfigFullscreen = m_MapEntity.SetupMapConfig(EMapEntityMode.COE_LOCATION_SELECTION, configComp.GetLocationSelectionMapConfig(), GetRootWidget());
		m_MapEntity.OpenMap(mapConfigFullscreen);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		SCR_InfoDisplay actionMenuDisplay = GetGame().GetHUDManager().FindInfoDisplay(SCR_ActionMenuInteractionDisplay);
		if (actionMenuDisplay)
			actionMenuDisplay.GetRootWidget().SetVisible(false);
	}

	//------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
		super.OnMenuClose();
		
		SCR_InfoDisplay actionMenuDisplay = GetGame().GetHUDManager().FindInfoDisplay(SCR_ActionMenuInteractionDisplay);
		if (actionMenuDisplay)
			actionMenuDisplay.GetRootWidget().SetVisible(true);
	}
}

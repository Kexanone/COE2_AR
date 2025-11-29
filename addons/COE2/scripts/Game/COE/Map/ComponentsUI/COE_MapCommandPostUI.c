//------------------------------------------------------------------------------------------------
class COE_MapCommandPostUI : SCR_MapCommandPostUI
{
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
	
		if (m_InputButton)
		{
			// Replace m_OnClicked with m_OnActivated to get keybinds working
			m_InputButton.m_OnClicked.Remove(OnCloseButtonClicked);
			m_InputButton.m_OnActivated.Insert(OnCloseButtonClicked);
		}
	}

	//------------------------------------------------------------------------------------------------
	override void HandlerDeattached(Widget w)
	{
		if (m_InputButton)
			m_InputButton.m_OnActivated.Remove(OnCloseButtonClicked);
		
		super.HandlerDeattached(w);
	}
	
	
	//------------------------------------------------------------------------------------------------
	override protected void SetModeType(Widget w)
	{
		m_sModeName = "#COE-Action_SelectAO";
		m_ModeColor = UIColors.EDITOR_MODE_ADMIN_MODE;
		m_sIconName = "scenarios";
		
		m_wModeText = TextWidget.Cast(w.FindAnyWidget("ModeText"));
		m_wModeTopBar = ImageWidget.Cast(w.FindAnyWidget("TopBadgeLine"));
		m_wModeIcon = ImageWidget.Cast(w.FindAnyWidget("ModeIcon"));
		
		m_wModeText.SetText(m_sModeName);
		m_wModeTopBar.SetColor(m_ModeColor);
		m_wModeIcon.SetColor(m_ModeColor);
		m_wModeIcon.LoadImageFromSet(0, m_sWrapperImageset, m_sIconName);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void CloseMenu()
	{
		MenuManager menuManager = GetGame().GetMenuManager();
		menuManager.CloseMenuByPreset(ChimeraMenuPreset.COE_AOSelectionMenuV2);
	}
}

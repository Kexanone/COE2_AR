//------------------------------------------------------------------------------------------------
//! --- TO DO: Handle case where player loses commander role while the menu is open or the scenario has been created
class COE_AOSelectionMenu : ChimeraMenuBase
{
	protected COE_GameMode m_GameMode;
	protected SCR_MapEntity m_MapEntity;
	protected ref MapConfiguration m_MapConfigDeploy = new MapConfiguration();
	protected SCR_MapUIElementContainer m_UIElementContainer;
	
	protected SCR_InputButtonComponent m_ConfirmButton;
	protected SCR_InputButtonComponent m_CloseButton;
	protected TextWidget m_LocationText;
	protected SCR_InputButtonComponent m_ChatButton;
	protected SCR_InputButtonComponent m_DeployButton;
	protected SCR_ChatPanel m_ChatPanel;
	
	protected ref ScriptInvoker m_OnLocationConfirmed = new ScriptInvoker();
	
	protected IEntity m_pSelectedLocation;
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		m_GameMode = COE_GameMode.GetInstance();
		if (!m_GameMode)
			return;
		
		m_MapEntity = SCR_MapEntity.GetMapInstance();
		if (!m_MapEntity)
			return;
		
		SCR_MapConfigComponent configComp = SCR_MapConfigComponent.Cast(m_GameMode.FindComponent(SCR_MapConfigComponent));
		if (!configComp)
			return;
		
		m_MapConfigDeploy = m_MapEntity.SetupMapConfig(EMapEntityMode.COE_LOCATION_SELECTION, configComp.COE_GetLocationSelectionMapConfig(), GetRootWidget());
		m_MapEntity.OpenMap(m_MapConfigDeploy);
		m_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
		
		SCR_HUDManagerComponent hud = GetGame().GetHUDManager();
		
		SCR_InfoDisplay actionMenuDisplay = GetGame().GetHUDManager().FindInfoDisplay(SCR_ActionMenuInteractionDisplay);
		if (actionMenuDisplay)
			actionMenuDisplay.GetRootWidget().SetVisible(false);
		
		Widget chat = GetRootWidget().FindAnyWidget("ChatPanel");
		if (chat)
			m_ChatPanel = SCR_ChatPanel.Cast(chat.FindHandler(SCR_ChatPanel));
		
		m_ChatButton = SCR_InputButtonComponent.GetInputButtonComponent("ChatButton", GetRootWidget());
		if (m_ChatButton)
			m_ChatButton.m_OnActivated.Insert(OnChatToggle);
		
		m_LocationText = TextWidget.Cast(GetRootWidget().FindAnyWidget("LocationText"));

		m_ConfirmButton = SCR_InputButtonComponent.GetInputButtonComponent("ConfirmButton", GetRootWidget());
		if (m_ConfirmButton)
			m_ConfirmButton.m_OnActivated.Insert(ConfirmAndClose);
		
		m_CloseButton = SCR_InputButtonComponent.GetInputButtonComponent("CloseButton", GetRootWidget());
		if (m_CloseButton)
			m_CloseButton.m_OnActivated.Insert(Close);
		
		SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_HUD_MAP_OPEN);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnMapOpen(MapConfiguration config)
	{
		m_MapEntity.SetZoom(1);

		// note@lk: temporary hotfix for duplicite journal entries, better solution is on the way
		Widget toolMenu = GetRootWidget().FindAnyWidget("ToolMenuVert");
		Widget child = toolMenu.GetChildren();
		while (child)
		{
			Widget sibling = child.GetSibling();
			child.RemoveFromHierarchy();
			child = sibling;
		}		
		
		m_UIElementContainer = SCR_MapUIElementContainer.Cast(m_MapEntity.GetMapUIComponent(SCR_MapUIElementContainer));
		if (m_UIElementContainer)
			m_UIElementContainer.COE_GetOnLocationSelected().Insert(SetLocationExt);
		
		SetLocationExt(m_pSelectedLocation);
	}
	
	void SetInitialLocation(IEntity entity)
	{
		m_pSelectedLocation = entity;
	}
	
	protected void SetLocationExt(IEntity entity)
	{
		m_pSelectedLocation = entity;
		FocusOnPoint(entity);
		
		MapDescriptorComponent mapDescriptor = MapDescriptorComponent.Cast(entity.FindComponent(MapDescriptorComponent));
		if (!mapDescriptor)
			return;
		
		m_LocationText.SetText(mapDescriptor.Item().GetDisplayName());
	}
	
	//! Centers map to a specific spawn point.
	protected void FocusOnPoint(notnull IEntity entity, bool smooth = true)
	{
		if (!m_MapEntity || !m_MapEntity.IsOpen())
			return;

		vector o = entity.GetOrigin();

		float x, y;
		m_MapEntity.WorldToScreen(o[0], o[2], x, y);

		float xScaled = GetGame().GetWorkspace().DPIUnscale(x);
		float yScaled = GetGame().GetWorkspace().DPIUnscale(y);

		if (smooth)
			m_MapEntity.PanSmooth(x, y);
		else
			m_MapEntity.SetPan(xScaled, yScaled);
	}

	//------------------------------------------------------------------------------------------------
	override void OnMenuUpdate(float tDelta)
	{
		if (m_MapEntity && m_MapEntity.IsOpen())
			GetGame().GetInputManager().ActivateContext("MapContext");
		
		if (m_ChatPanel)
			m_ChatPanel.OnUpdateChat(tDelta);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuHide()
	{
		if (m_MapEntity && m_MapEntity.IsOpen())
			m_MapEntity.CloseMap();		
	}
	
	void ConfirmAndClose()
	{
		m_OnLocationConfirmed.Invoke(m_pSelectedLocation);
		Close();
	}

	//------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
		super.OnMenuClose();
		
		if (m_MapEntity && m_MapEntity.IsOpen())
			m_MapEntity.CloseMap();
		
		SCR_InfoDisplay actionMenuDisplay = GetGame().GetHUDManager().FindInfoDisplay(SCR_ActionMenuInteractionDisplay);
		if (actionMenuDisplay)
			actionMenuDisplay.GetRootWidget().SetVisible(true);
				
		SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_HUD_MAP_CLOSE);

	}
	
	//------------------------------------------------------------------------------------------------
	//! Toggle chat.
	protected void OnChatToggle()
	{
		if (!m_ChatPanel)
			return;

		SCR_ChatPanelManager.GetInstance().ToggleChatPanel(m_ChatPanel);
	}
	
	ScriptInvoker GetOnLocationConfirmed()
	{
		return m_OnLocationConfirmed;
	}
}
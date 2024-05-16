//------------------------------------------------------------------------------------------------
//! --- TO DO: Handle case where player loses commander role while the menu is open or the scenario has been created
class COE_AOSelectionMenu : ChimeraMenuBase
{
	protected COE_GameMode m_GameMode;
	protected SCR_MapEntity m_MapEntity;
	protected ref MapConfiguration m_MapConfigDeploy = new MapConfiguration();
	protected COE_MapUIElementContainer m_UIElementContainer;
	
	protected SCR_InputButtonComponent m_CloseButton;
	protected SCR_InputButtonComponent m_ChatButton;
	protected SCR_ChatPanel m_ChatPanel;
		
	protected SCR_ToolboxComponent m_ObjectiveToolbox;
	protected SCR_SpinBoxComponent m_LocationSelector;
	protected SCR_InputButtonComponent m_ConfirmButton;

	protected ref ScriptInvoker m_OnConfirm = new ScriptInvoker();
	
	protected ref COE_AOParams m_SelectedAOParams = new COE_AOParams();
	protected ref array<ref COE_BaseTaskBuilder> m_aTaskBuilders;
	protected ref array<ref KSC_Location> m_aLocations;
	
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
		
		COE_MapConfigComponent configComp = COE_MapConfigComponent.Cast(m_GameMode.FindComponent(SCR_MapConfigComponent));
		if (!configComp)
			return;
		
		m_MapConfigDeploy = m_MapEntity.SetupMapConfig(EMapEntityMode.COE_LOCATION_SELECTION, configComp.GetLocationSelectionMapConfig(), GetRootWidget());
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
		
		m_ConfirmButton = SCR_InputButtonComponent.GetInputButtonComponent("ConfirmButton", GetRootWidget());
		if (m_ConfirmButton)
			m_ConfirmButton.m_OnActivated.Insert(OnConfirm);
		
		m_CloseButton = SCR_InputButtonComponent.GetInputButtonComponent("CloseButton", GetRootWidget());
		if (m_CloseButton)
			m_CloseButton.m_OnActivated.Insert(Close);
		
		m_ObjectiveToolbox = SCR_ToolboxComponent.GetToolboxComponent("ObjectiveToolbox", GetRootWidget());
		if (m_ObjectiveToolbox)
		{
			m_aTaskBuilders = COE_GameMode.GetInstance().GetAvailableTaskBuilders();
			
			foreach(COE_BaseTaskBuilder builder : m_aTaskBuilders)
			{
				m_ObjectiveToolbox.AddItem(builder.GetTaskName());
			}
			
			m_ObjectiveToolbox.m_OnChanged.Insert(OnObjectiveChanged);
		};
		
		m_LocationSelector = SCR_SpinBoxComponent.GetSpinBoxComponent("LocationSelector", GetRootWidget());
		if (m_LocationSelector)
		{
			m_aLocations = COE_GameMode.GetInstance().GetAvailableLocations();
			
			foreach(KSC_Location location : m_aLocations)
			{
				m_LocationSelector.AddItem(location.m_sName);
			}
			
			m_LocationSelector.m_OnChanged.Insert(OnLocationSelectorChanged);
			GetGame().GetWorkspace().SetFocusedWidget(m_LocationSelector.GetRootWidget());
		};
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
		
		m_UIElementContainer = COE_MapUIElementContainer.Cast(m_MapEntity.GetMapUIComponent(COE_MapUIElementContainer));
		if (m_UIElementContainer)
			m_UIElementContainer.GetOnLocationSelected().Insert(SetLocationExt);
		
		FocusOnPoint(m_aLocations[m_SelectedAOParams.m_iLocationIdx]);
		
		SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_HUD_MAP_OPEN);
	}
	
	void SetParams(COE_AOParams params)
	{
		m_SelectedAOParams.Copy(params);
		SetLocationExt(m_aLocations[m_SelectedAOParams.m_iLocationIdx]);
		
		for (int i = 0; i < m_aTaskBuilders.Count(); i++)
		{
			if (KSC_BitTools.IsBitSet(m_SelectedAOParams.m_eTaskTypes, i))
				m_ObjectiveToolbox.SetCurrentItem(i);
		};
	}
	
	protected void SetLocationExt(KSC_Location location)
	{
		int idx = m_aLocations.Find(location);
		m_SelectedAOParams.m_iLocationIdx = idx;
		FocusOnPoint(location);
		m_LocationSelector.SetCurrentItem(idx);
	}
	
	//! Centers map to a specific spawn point.
	protected void FocusOnPoint(notnull KSC_Location location, bool smooth = true)
	{
		if (!m_MapEntity || !m_MapEntity.IsOpen())
			return;

		vector o = location.m_vCenter;

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
	protected void OnObjectiveChanged(SCR_SelectionWidgetComponent comp, int i, bool state)
	{
		if (state)
		{
			KSC_BitTools.SetBit(m_SelectedAOParams.m_eTaskTypes, i);
		}
		else
		{
			KSC_BitTools.ClearBit(m_SelectedAOParams.m_eTaskTypes, i);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnLocationSelectorChanged(SCR_SelectionWidgetComponent comp, int i)
	{
		SetLocationExt(m_aLocations[i]);
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
	
	void OnConfirm()
	{
		m_OnConfirm.Invoke(m_SelectedAOParams);
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
	}
	
	//------------------------------------------------------------------------------------------------
	//! Toggle chat.
	protected void OnChatToggle()
	{
		if (!m_ChatPanel)
			return;

		SCR_ChatPanelManager.GetInstance().ToggleChatPanel(m_ChatPanel);
	}
	
	ScriptInvoker GetOnConfirm()
	{
		return m_OnConfirm;
	}
}
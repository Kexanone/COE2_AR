//------------------------------------------------------------------------------
//! Map UI component that can be added via map config
//! Extends SCR_MapUIElementContainer for drawing and handling available objective
//! locations that can be picked in COE_AOSelectionMenu
class COE_MapUIElementContainerV2 : SCR_MapUIElementContainer
{
	[Attribute("{764EA71E6CAA60AF}UI/layouts/Map/COE_LocationElement.layout", params: "layout")]
	protected ResourceName m_sCOE_LocationElement;
	
	[Attribute("0")]
	protected bool m_bShowAvailableLocations;
	
	protected COE_MapUIElement m_HoveredElement;
	protected ref ScriptInvoker<COE_MapUIElement> m_OnElementHovered;
		
	//------------------------------------------------------------------------------------------------
	override void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);
		
		if (m_bShowAvailableLocations)
			InitLocations();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMapClose(MapConfiguration config)
	{
		if (m_bShowAvailableLocations)
			UpdateAOParams();
		
		super.OnMapClose(config);
		m_HoveredElement = null;
	}

	//------------------------------------------------------------------------------
	protected void InitLocations()
	{
		COE_GameMode gameMode = COE_GameMode.Cast(m_GameMode);
		if (!gameMode)
			return;
		
		if (m_bShowAvailableLocations)
		{
			foreach (KSC_Location location : gameMode.GetAvailableLocations())
			{
				array<COE_BaseTaskBuilder> taskBuilders;
				
				foreach (COE_AOParams entry : gameMode.GetNextAOParams())
				{
					if (entry.GetLocation() == location)
					{
						taskBuilders = entry.GetTaskBuilders();
						break;
					}
				}
				
				ShowLocation(location, taskBuilders);
			}
		}

		UpdateIcons();
	}

	//------------------------------------------------------------------------------
	protected void ShowLocation(notnull KSC_Location location, array<COE_BaseTaskBuilder> taskBuilders = null)
	{
		Widget w = GetGame().GetWorkspace().CreateWidgets(m_sCOE_LocationElement, m_wIconsContainer);
		COE_MapUILocation handler = COE_MapUILocation.Cast(w.FindHandler(COE_MapUILocation));
		if (!handler)
			return;

		handler.Init(location);
		handler.SetParent(this);
		m_mIcons.Set(w, handler);

		FrameSlot.SetSizeToContent(w, true);
		FrameSlot.SetAlignment(w, 0.5, 0.5);
		
		if (taskBuilders)
		{
			foreach (COE_BaseTaskBuilder taskBuilder : taskBuilders)
			{
				handler.AddTaskBuilder(taskBuilder, isInit: true);
			}
		}
	}
	
	//------------------------------------------------------------------------------
	[Friend(COE_MapUILocation)]
	protected void UpdateAOParams()
	{
		array<ref COE_AOParams> params = {};

		foreach (Widget w, SCR_MapUIElement e : m_mIcons)
		{
			COE_MapUILocation locationHandler = COE_MapUILocation.Cast(e);
			if (!locationHandler)
				continue;
			
			array<COE_BaseTaskBuilder> taskBuilders = locationHandler.GetTaskBuilders();
			if (taskBuilders.IsEmpty())
				continue;
			
			COE_AOParams entry = new COE_AOParams();
			entry.SetLocation(locationHandler.GetLocation());
			entry.SetTaskBuilders(taskBuilders);
			params.Insert(entry);
		}
		
		COE_EditorModeCommanderEntity.RequestUpdateAOParams(params);
	}
	
	//------------------------------------------------------------------------------
	void SetHoveredElement(COE_MapUIElement element)
	{
		m_HoveredElement = element;
		
		if (m_OnElementHovered)
			m_OnElementHovered.Invoke(element);
	}
	
	//------------------------------------------------------------------------------
	COE_MapUILocation FindLocationIcon(KSC_Location location)
	{
		foreach (Widget w, SCR_MapUIElement e : m_mIcons)
		{
			COE_MapUILocation locIcon = COE_MapUILocation.Cast(e);
			if (!locIcon)
				continue;

			if (locIcon.GetLocation() == location)
				return locIcon;
		}

		return null;
	}
	
	//------------------------------------------------------------------------------
	ScriptInvoker<COE_MapUIElement> GetOnElementHovered()
	{
		if (!m_OnElementHovered)
			m_OnElementHovered = new ScriptInvoker<COE_MapUIElement>();
		
		return m_OnElementHovered;
	}
}
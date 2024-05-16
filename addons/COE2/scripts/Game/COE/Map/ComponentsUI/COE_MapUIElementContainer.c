//------------------------------------------------------------------------------
//! Map UI component that can be added via map config
//! Extends SCR_MapUIElementContainer for drawing and handling available objective
//! locations that can be picked in COE_AOSelectionMenu
class COE_MapUIElementContainer : SCR_MapUIElementContainer
{
	[Attribute("{764EA71E6CAA60AF}UI/layouts/Map/COE_LocationElement.layout", params: "layout")]
	protected ResourceName m_sCOE_LocationElement;
	
	[Attribute("0")]
	protected bool m_bShowAvailableLocations;
	
	[Attribute("0")]
	protected bool m_bShowCurrentLocation;
	
	protected ref ScriptInvoker m_OnLocationSelected;

	protected void AddLocation(KSC_Location location)
	{
		ShowLocation(location);
		UpdateIcons();
	}

	//------------------------------------------------------------------------------------------------
	override void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);
		
		if (m_bShowAvailableLocations || m_bShowCurrentLocation)
			InitLocations();
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
				ShowLocation(location);
			}
		}
		
		if (m_bShowCurrentLocation)
		{
			KSC_Location location = gameMode.GetCurrentLocation();
			if (location)
				ShowLocation(location);
		}

		UpdateIcons();
	}

	//------------------------------------------------------------------------------
	protected void ShowLocation(notnull KSC_Location location)
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
	}

	//------------------------------------------------------------------------------
	void OnLocationSelectedExt(KSC_Location location) // called when selected via deploy menu spinbox
	{
		SCR_MapUIElement icon = FindLocationIcon(location);
		if (icon)
			icon.SelectIcon(false);
	}
	
	//------------------------------------------------------------------------------
	void OnLocationSelected(KSC_Location location = null)
	{
		GetOnLocationSelected().Invoke(location);
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

	ScriptInvoker GetOnLocationSelected()
	{
		if (!m_OnLocationSelected)
			m_OnLocationSelected = new ScriptInvoker();

		return m_OnLocationSelected;
	}
}
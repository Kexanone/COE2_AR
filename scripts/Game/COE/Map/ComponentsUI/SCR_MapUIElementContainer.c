modded class SCR_MapUIElementContainer : SCR_MapUIBaseComponent
{
	[Attribute("{764EA71E6CAA60AF}UI/layouts/Map/COE_LocationElement.layout", params: "layout")]
	protected ResourceName m_sCOE_LocationElement;
	
	[Attribute("0")]
	protected bool m_bCOE_ShowLocations;
	
	protected ref ScriptInvoker m_COE_OnLocationSelected;

	protected void COE_AddLocation(IEntity location)
	{
		if (SCR_EditableCommentComponent.Cast(location.FindComponent(SCR_EditableCommentComponent)))
		{
			COE_ShowLocation(location);
			UpdateIcons();
		}
	}

	//------------------------------------------------------------------------------------------------
	override void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);
		
		if (m_bCOE_ShowLocations)
			COE_InitLocations();
	}

	protected void COE_InitLocations()
	{
		COE_GameMode gameMode = COE_GameMode.Cast(m_GameMode);
		if (!gameMode)
			return;
		
		foreach (IEntity location : gameMode.GetPossibleLocations())
		{
			COE_ShowLocation(location);
		}

		UpdateIcons();
	}

	protected void COE_ShowLocation(notnull IEntity location)
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

	void COE_OnLocationSelectedExt(IEntity location) // called when selected via deploy menu spinbox
	{
		SCR_MapUIElement icon = FindLocationIcon(location);
		if (icon)
			icon.SelectIcon(false);
	}
	void COE_OnLocationSelected(IEntity location = null)
	{
		COE_GetOnLocationSelected().Invoke(location);
	}
	
	COE_MapUILocation FindLocationIcon(IEntity location)
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

	ScriptInvoker COE_GetOnLocationSelected()
	{
		if (!m_COE_OnLocationSelected)
			m_COE_OnLocationSelected = new ScriptInvoker();

		return m_COE_OnLocationSelected;
	}
}
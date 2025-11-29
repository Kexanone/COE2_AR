//------------------------------------------------------------------------------
class COE_MapCommanderRadialMenu : SCR_MapUIBaseComponent
{
	[Attribute()]
	protected ref array<ref SCR_SelectionMenuEntry> m_aEntries;
	
	protected SCR_MapRadialUI m_MapContextMenu;
	protected SCR_RadialMenu m_RadialMenu;
	protected COE_MapUIElement m_HoveredMapElement;
	protected COE_MapUIElement m_SelectedMapElement;
	
	//------------------------------------------------------------------------------------------------
	override void OnMapOpen(MapConfiguration config)
	{
		COE_MapUIElementContainerV2 mapElementContainer = COE_MapUIElementContainerV2.Cast(m_MapEntity.GetMapUIComponent(COE_MapUIElementContainerV2));
		if (mapElementContainer)
			mapElementContainer.GetOnElementHovered().Insert(OnMapElementHovered);
		
		m_MapContextMenu = SCR_MapRadialUI.GetInstance();
		if (!m_MapContextMenu)
			return;

		m_MapContextMenu.GetOnMenuInitInvoker().Insert(OnRadialMenuInit);
	}

	//------------------------------------------------------------------------------------------------
	override void OnMapClose(MapConfiguration config)
	{
		m_HoveredMapElement = null;
		
		COE_MapUIElementContainerV2 mapElementContainer = COE_MapUIElementContainerV2.Cast(m_MapEntity.GetMapUIComponent(COE_MapUIElementContainerV2));
		if (mapElementContainer)
			mapElementContainer.GetOnElementHovered().Remove(OnMapElementHovered);
		
		if (!m_MapContextMenu)
			return;

		m_MapContextMenu.GetOnMenuInitInvoker().Remove(OnRadialMenuInit);
		
		if (!m_RadialMenu)
			return;
		
		m_RadialMenu.GetOnOpen().Remove(OnRadialMenuOpen);
		m_RadialMenu.GetOnBeforeOpen().Remove(OnRadialMenuBeforeOpen);
		m_RadialMenu.GetOnClose().Remove(OnRadialMenuClose);
		m_RadialMenu.GetOnPerform().Remove(OnPerformEntry);
		m_RadialMenu.COE_GetOnPerformCategory().Remove(OnPerformCategory);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnMapElementHovered(COE_MapUIElement mapElement)
	{
		m_HoveredMapElement = mapElement;
	}

	//------------------------------------------------------------------------------------------------
	protected void OnRadialMenuInit()
	{
		if (!m_MapContextMenu || !m_aEntries)
			return;
		
		m_RadialMenu = m_MapContextMenu.GetRadialController().GetRadialMenu();
		m_RadialMenu.GetOnOpen().Insert(OnRadialMenuOpen);
		m_RadialMenu.GetOnBeforeOpen().Insert(OnRadialMenuBeforeOpen);
		m_RadialMenu.GetOnClose().Insert(OnRadialMenuClose);
		m_RadialMenu.GetOnPerform().Insert(OnPerformEntry);
		m_RadialMenu.COE_GetOnPerformCategory().Insert(OnPerformCategory);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPerformEntry(SCR_SelectionMenu menu, SCR_SelectionMenuEntry entry)
	{
		COE_SelectionMenuEntry coeEntry = COE_SelectionMenuEntry.Cast(entry);
		if (coeEntry)
			return coeEntry.OnPerformOnMap(m_MapContextMenu.GetMenuWorldPosition(), m_SelectedMapElement);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPerformCategory(SCR_SelectionMenu menu, SCR_SelectionMenuCategoryEntry entry)
	{
		COE_SelectionMenuCategoryEntry coeCategoryEntry = COE_SelectionMenuCategoryEntry.Cast(entry);
		if (coeCategoryEntry)
			return coeCategoryEntry.OnPerformOnMap(m_MapContextMenu.GetMenuWorldPosition(), m_SelectedMapElement);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Add hover entries
	protected void OnRadialMenuOpen()
	{
		vector pos = m_MapContextMenu.GetMenuWorldPosition();
		
		foreach (SCR_SelectionMenuEntry entry : m_aEntries)
		{
			COE_SelectionMenuEntry coeEntry = COE_SelectionMenuEntry.Cast(entry);
			if (coeEntry)
			{
				if (!coeEntry.CanBeShownOnMap(pos, m_SelectedMapElement))
					continue;
				
				coeEntry.Enable(coeEntry.CanBePerformedOnMap(pos, m_SelectedMapElement));
			}
			
			COE_SelectionMenuCategoryEntry coeCategoryEntry = COE_SelectionMenuCategoryEntry.Cast(entry);
			if (coeCategoryEntry)
			{
				if (!coeCategoryEntry.CanBeShownOnMap(pos, m_SelectedMapElement))
					continue;
				
				coeCategoryEntry.Enable(coeCategoryEntry.CanBePerformedOnMap(pos, m_SelectedMapElement));
			}
			
			m_MapContextMenu.InsertCustomRadialEntry(entry);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnRadialMenuBeforeOpen()
	{
		m_SelectedMapElement = m_HoveredMapElement;
	}

	//------------------------------------------------------------------------------------------------
	protected void OnRadialMenuClose()
	{
		// Remove entries the next frame so callbacks still work properly
		GetGame().GetCallqueue().CallLater(ClearRadialMenu);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Remove hover entries
	protected void ClearRadialMenu()
	{
		m_SelectedMapElement = null;
		
		foreach (SCR_SelectionMenuEntry entry : m_aEntries)
		{
			m_MapContextMenu.RemoveRadialEntry(entry);
		}
	}
}

//------------------------------------------------------------------------------
class COE_MapUIElement : SCR_MapUIElement
{
	//------------------------------------------------------------------------------
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		COE_MapUIElementContainer container = COE_MapUIElementContainer.Cast(m_Parent);
		if (container)
			container.SetHoveredElement(this);

		return super.OnMouseEnter(w, x, y);
	}

	//------------------------------------------------------------------------------
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		COE_MapUIElementContainer container = COE_MapUIElementContainer.Cast(m_Parent);
		if (container)
			container.SetHoveredElement(null);

		return super.OnMouseLeave(w, enterW, x, y);
	}
}

class COE_MapUILocation : SCR_MapUIElement
{
	protected IEntity m_Location;
	protected string m_sFactionKey;
	protected TextWidget m_wLocationName;
	protected Widget m_wImageOverlay;
	protected OverlayWidget m_wSymbolOverlay;
	protected SCR_MilitarySymbolUIComponent m_MilitarySymbolComponent;
	protected ButtonWidget m_wButton;
	protected SizeLayoutWidget m_wSizeLayout;
	
	//------------------------------------------------------------------------------
	void Init(notnull IEntity location)
	{
		m_Location = location;
		MapDescriptorComponent mapDescriptor = MapDescriptorComponent.Cast(location.FindComponent(MapDescriptorComponent));
		if (!mapDescriptor)
			return;
		
		MapItem mapItem = mapDescriptor.Item();
		if (!mapItem)
			return;
		
		m_wLocationName.SetText(mapItem.GetDisplayName());
		
		COE_SessionAttributesComponent sessionAttributes = COE_SessionAttributesComponent.Cast(GetGame().GetGameMode().FindComponent(COE_SessionAttributesComponent));
		if (!sessionAttributes)
			return;
		
		m_sFactionKey = sessionAttributes.GetEnemyFaction().GetFactionKey();
		UpdateIcon();
	}

	//------------------------------------------------------------------------------------------------
	override vector GetPos()
	{
		return m_Location.GetOrigin();
	}

	//------------------------------------------------------------------------------
	void UpdateIcon()
	{
		SCR_MilitarySymbol symbol = new SCR_MilitarySymbol();
		
		SCR_GroupIdentityCore core = SCR_GroupIdentityCore.Cast(SCR_GroupIdentityCore.GetInstance(SCR_GroupIdentityCore));
		if (!core)
			return;
		
		SCR_Faction faction = SCR_Faction.Cast(GetGame().GetFactionManager().GetFactionByKey(m_sFactionKey));
		if (faction)
		{
			SCR_MilitarySymbolRuleSet ruleSet = core.GetSymbolRuleSet();
			ruleSet.UpdateSymbol(symbol, faction);
		}
		else
		{
			symbol.SetIdentity(EMilitarySymbolIdentity.UNKNOWN);
		}
		
		//Selection visuals 
		string selection;
		string highlight;
		switch (symbol.GetIdentity())
		{
			case EMilitarySymbolIdentity.INDFOR:
			{
				selection = "Neutral_Select";
				highlight = "Neutral_Focus";
				break;
			}
			case EMilitarySymbolIdentity.OPFOR:
			{
				selection = "Hostile_Select";
				highlight = "Hostile_Focus";
				break;
			}
			case EMilitarySymbolIdentity.BLUFOR:
			{
				selection = "Friend_Select";
				highlight = "Friend_Focus";
				break;
			}
			case EMilitarySymbolIdentity.UNKNOWN:
			{
				selection = "Unknown_Select";
				highlight = "Unknown_Focus";
				break;
			}
		}
		m_bVisible = true;
		
		m_wHighlightImg.LoadImageFromSet(0, m_sImageSetARO, highlight);
		m_wSelectImg.LoadImageFromSet(0, m_sImageSetARO, selection);
		symbol.SetIcons(0);
		
		m_wSymbolOverlay.SetColor(GetColorForFaction(m_sFactionKey));
		if (m_wGradient)
			m_wGradient.SetColor(GetColorForFaction(m_sFactionKey));

		if (faction)
			m_MilitarySymbolComponent.Update(symbol);
	}

	//------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

		m_wImageOverlay = w.FindAnyWidget("IconOverlay");
		m_wSizeLayout = SizeLayoutWidget.Cast(w.FindAnyWidget("SizeLayout"));
		m_wLocationName = TextWidget.Cast(w.FindAnyWidget("Name"));
		m_wSymbolOverlay = OverlayWidget.Cast(m_wImageOverlay.FindWidget("Symbol"));

		if (!m_wSymbolOverlay)
			return;
		
		m_MilitarySymbolComponent = SCR_MilitarySymbolUIComponent.Cast(m_wSymbolOverlay.FindHandler(SCR_MilitarySymbolUIComponent));
		
		m_wButton = ButtonWidget.Cast(w.FindAnyWidget("Button"));

		if (m_wButton)
			m_wButton.SetEnabled(false);
	}

	//------------------------------------------------------------------------------------------------
	override void HandlerDeattached(Widget w)
	{
		super.HandlerDeattached(w);
	}

	//------------------------------------------------------------------------------
	override void SelectIcon(bool invoke = true)
	{
		if (!m_wSelectImg)
			return;

		if (s_SelectedElement && s_SelectedElement != this)
			s_SelectedElement.Select(false);

		Select();

		m_wSelectImg.SetVisible(true);
		if (m_wGradient)
			m_wGradient.SetVisible(true);

		if (m_bIsSelected && invoke)
			m_Parent.COE_OnLocationSelected(m_Location);
	}

	//------------------------------------------------------------------------------
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (m_wSizeLayout && w == m_wSizeLayout && m_wButton)
			m_wButton.SetEnabled(true);
		
		GetGame().GetWorkspace().SetFocusedWidget(w);
		super.OnMouseEnter(w, x, y);
		m_wRoot.SetZOrder(1);
		m_wHighlightImg.SetVisible(true);

		if (m_wGradient)
			m_wGradient.SetVisible(true);

		if (!m_bIsSelected)
			AnimExpand();

		return false;
	}

	//------------------------------------------------------------------------------
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		super.OnMouseLeave(w, enterW, x, y);
		m_wRoot.SetZOrder(0);
		m_wHighlightImg.SetVisible(false);
		if (!m_bIsSelected && m_wGradient)
			m_wGradient.SetVisible(false);
		if (!m_bIsSelected)
		{
			AnimCollapse();
		}
		
		if (RenderTargetWidget.Cast(enterW) && m_wButton.IsEnabled())
			m_wButton.SetEnabled(false);

		return false;
	}

	IEntity GetLocation()
	{
		return m_Location;
	}
};
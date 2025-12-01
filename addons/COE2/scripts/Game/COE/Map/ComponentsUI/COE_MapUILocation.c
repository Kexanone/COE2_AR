//------------------------------------------------------------------------------
//! Handler for an objective location element used in COE_MapUIElementContainer
class COE_MapUILocation : COE_MapUIElement
{
	[Attribute("{3FAF784F537B325A}UI/layouts/Map/COE_LocationTaskEntry.layout.layout", params: "layout")]
	protected ResourceName m_sTaskElementLayoutName;
	
	protected KSC_Location m_Location;
	protected TextWidget m_wLocationName;
	protected FactionKey m_sOwnerFactionKey;
	protected ref array<COE_BaseTaskBuilder> m_aTaskBuilders = {};
	protected ref array<Widget> m_aTaskWidgets = {};
	protected Widget m_wImageOverlay;
	protected OverlayWidget m_wSymbolOverlay;
	protected SCR_MilitarySymbolUIComponent m_MilitarySymbolComponent;
	protected Widget m_wTaskFrame;
	protected Widget m_wTaskList;
	
	//------------------------------------------------------------------------------
	void Init(notnull KSC_Location location)
	{
		m_Location = location;		
		m_wLocationName.SetText(location.m_sName);
		UpdateIcon();
	}

	//------------------------------------------------------------------------------------------------
	override vector GetPos()
	{
		return m_Location.m_vCenter;
	}

	//------------------------------------------------------------------------------
	void UpdateIcon()
	{
		SCR_Faction faction = SCR_Faction.Cast(GetGame().GetFactionManager().GetFactionByKey(m_sOwnerFactionKey));
		SCR_MilitarySymbol symbol = new SCR_MilitarySymbol();
		
		SCR_GroupIdentityCore core = SCR_GroupIdentityCore.Cast(SCR_GroupIdentityCore.GetInstance(SCR_GroupIdentityCore));
		if (!core)
			return;
		
		SCR_MilitarySymbolRuleSet ruleSet = core.GetSymbolRuleSet();
		if (ruleSet)
			ruleSet.UpdateSymbol(symbol, faction);
		
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
		
		Color color = GetColorForFaction(m_sOwnerFactionKey);
		
		if (m_wSymbolOverlay)
			m_wSymbolOverlay.SetColor(color);
		
		if (m_wGradient)
			m_wGradient.SetColor(color);

		m_MilitarySymbolComponent.Update(symbol);
	}

	//------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

		m_wImageOverlay = w.FindAnyWidget("IconOverlay");
		m_wLocationName = TextWidget.Cast(w.FindAnyWidget("Name"));
		m_wSymbolOverlay = OverlayWidget.Cast(m_wImageOverlay.FindWidget("Symbol"));
		m_wTaskFrame = w.FindAnyWidget("TaskFrame");
		m_wTaskList = m_wTaskFrame.FindAnyWidget("TaskList");

		if (!m_wSymbolOverlay)
			return;
		
		m_MilitarySymbolComponent = SCR_MilitarySymbolUIComponent.Cast(m_wSymbolOverlay.FindHandler(SCR_MilitarySymbolUIComponent));
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
	}

	//------------------------------------------------------------------------------
	override bool OnMouseEnter(Widget w, int x, int y)
	{
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

		return false;
	}

	//------------------------------------------------------------------------------
	KSC_Location GetLocation()
	{
		return m_Location;
	}
	
	//------------------------------------------------------------------------------
	void SetOwnerFactionKey(FactionKey factionKey)
	{
		m_sOwnerFactionKey = factionKey;
	}
	
	//------------------------------------------------------------------------------
	FactionKey GetOwnerFactionKey()
	{
		return m_sOwnerFactionKey;
	}
	
	//------------------------------------------------------------------------------
	int AddTaskBuilder(COE_BaseTaskBuilder builder, bool isInit = false)
	{
		if (m_aTaskBuilders.IsEmpty())
		{
			COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
			if (factionManager)
				m_sOwnerFactionKey = factionManager.GetEnemyFaction().GetFactionKey();
			
			UpdateIcon();
			m_wTaskFrame.SetVisible(true);
		}
		
		Widget taskWidget = GetGame().GetWorkspace().CreateWidgets(m_sTaskElementLayoutName, m_wTaskList);
		COE_LocationTaskUIEntry handler = COE_LocationTaskUIEntry.Cast(taskWidget.FindHandler(COE_LocationTaskUIEntry));
		handler.SetTaskBuilder(builder);
		m_aTaskWidgets.Insert(taskWidget);
		
		int idx = m_aTaskBuilders.Insert(builder);
		
		COE_MapUIElementContainer container = COE_MapUIElementContainer.Cast(m_Parent);
		if (!isInit && container)
			container.UpdateAOParams();
		
		return idx;
	}
	
	//------------------------------------------------------------------------------
	void RemoveTaskBuilder(int idx)
	{
		m_aTaskBuilders.Remove(idx);
		
		if (m_aTaskBuilders.IsEmpty())
		{
			m_sOwnerFactionKey = "";
			UpdateIcon();
			m_wTaskFrame.SetVisible(false);
		}
		
		m_wTaskList.RemoveChild(m_aTaskWidgets[idx]);
		m_aTaskWidgets.Remove(idx);
		
		COE_MapUIElementContainer container = COE_MapUIElementContainer.Cast(m_Parent);
		if (container)
			container.UpdateAOParams();
	}
	
	//------------------------------------------------------------------------------
	array<COE_BaseTaskBuilder> GetTaskBuilders()
	{
		return m_aTaskBuilders;
	}
}

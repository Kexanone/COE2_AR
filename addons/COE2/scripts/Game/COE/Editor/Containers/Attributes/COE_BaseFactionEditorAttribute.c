//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_BaseFactionEditorAttribute: SCR_BaseFactionEditableAttribute
{
	[Attribute("true", desc: "Whether the factions to select are military or civilian")]
	protected bool m_bIsMilitary;
	
	//------------------------------------------------------------------------------------------------
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(item);
		if (!gameMode)
			return null;
		
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return null;
		
		int factionIndex = factionManager.GetFactionIndex(GetFaction(factionManager));
		
		if (m_aValues.IsEmpty())
			CreatePresets();
		
		foreach (int i, SCR_EditorAttributeFloatStringValueHolder value : m_aValues)
		{
			if ((int)value.GetFloatValue() == factionIndex)
				return SCR_BaseEditorAttributeVar.CreateInt(i);
		}
		
		return SCR_BaseEditorAttributeVar.CreateInt(-1);
	}
	
	//------------------------------------------------------------------------------------------------
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(item);
		if (!gameMode)
			return;
		
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return;
		
		int index = var.GetInt();
		if (index < 0 || index >= m_aValues.Count())
			return;
		
		SCR_EditorAttributeFloatStringValueHolder value = m_aValues[index];
		if (!value)
			return;
		
		SCR_Faction faction = SCR_Faction.Cast(factionManager.GetFactionByIndex(value.GetFloatValue()));
		if (!faction)
			return;

		SetFaction(factionManager, faction, playerID);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Same as SCR_BaseFactionEditableAttribute::CreatePresets, but filtering non-military factions
	protected override void CreatePresets()
	{
		SCR_DelegateFactionManagerComponent delegateFactionManager = SCR_DelegateFactionManagerComponent.GetInstance();
		if (!delegateFactionManager)
			return;

		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
			return;

		m_aValues.Clear();

		SCR_SortedArray<SCR_EditableFactionComponent> factionDelegates = new SCR_SortedArray<SCR_EditableFactionComponent>;
		int count = delegateFactionManager.GetSortedFactionDelegates(factionDelegates);
		m_aValues.Reserve(count);

		SCR_Faction scrFaction;

		for(int i = 0; i < count; ++i)
		{
			scrFaction = SCR_Faction.Cast(factionDelegates.Get(i).GetFaction());

			if (!scrFaction || scrFaction.IsMilitary() != m_bIsMilitary)
				continue;

			SCR_EditorAttributeFloatStringValueHolder value = new SCR_EditorAttributeFloatStringValueHolder();
			value.SetWithUIInfo(scrFaction.GetUIInfo(), factionDelegates.GetValue(i).GetFactionIndex());
			m_aValues.Insert(value);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void UpdateInterlinkedVariables(SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, bool isInit = false)
	{
		if (!var)
			return;
		
		typename linkedAttributeType = GetLinkedAttributeType();
		if (!linkedAttributeType)
			return;
		
		SCR_BaseEditorAttributeVar varOther;
		manager.GetAttributeVariable(linkedAttributeType, varOther);
		if (var.GetInt() != varOther.GetInt())
			return;
		
		if (var.GetInt())
			manager.SetAttributeVariable(linkedAttributeType, SCR_BaseEditorAttributeVar.CreateInt(0));
		else
			manager.SetAttributeVariable(linkedAttributeType, SCR_BaseEditorAttributeVar.CreateInt(1));
	}
	
	//------------------------------------------------------------------------------------------------
	Faction GetFaction(COE_FactionManager factionManager);
	
	//------------------------------------------------------------------------------------------------
	void SetFaction(COE_FactionManager factionManager, SCR_Faction faction, int playerID);
	
	//------------------------------------------------------------------------------------------------
	typename GetLinkedAttributeType()
	{
		return typename.Empty;
	}
}
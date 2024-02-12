[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_BaseFactionEditorAttribute: SCR_BaseFactionEditableAttribute
{		
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(item);
		if (!gameMode)
			return null;		
		
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
			return null;
		
		int factionIndex = factionManager.GetFactionIndex(GetFaction(gameMode));
		
		
		SCR_DelegateFactionManagerComponent delegateFactionManager = SCR_DelegateFactionManagerComponent.GetInstance();
		if (!delegateFactionManager)
			return null;
		
		SCR_SortedArray<SCR_EditableFactionComponent> factionDelegates = new SCR_SortedArray<SCR_EditableFactionComponent>;
		int count = delegateFactionManager.GetSortedFactionDelegates(factionDelegates);
		
		if (factionDelegates.IsEmpty())
			return null;
				
		for (int i = 0; i < count; i++)
		{
			if (factionDelegates.GetValue(i).GetFactionIndex() == factionIndex)
				return SCR_BaseEditorAttributeVar.CreateInt(i);
		};
		
		return SCR_BaseEditorAttributeVar.CreateInt(-1);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(item);
		if (!gameMode)
			return;
		
		SCR_DelegateFactionManagerComponent delegateFactionManager = SCR_DelegateFactionManagerComponent.GetInstance();
		if (!delegateFactionManager)
			return;
		
		SCR_SortedArray<SCR_EditableFactionComponent> factionDelegates = new SCR_SortedArray<SCR_EditableFactionComponent>;
		delegateFactionManager.GetSortedFactionDelegates(factionDelegates);
		
		if (factionDelegates.IsEmpty())
			return;
		
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
			return;
		
		COE_Faction faction = COE_Faction.Cast(factionManager.GetFactionByIndex(factionDelegates.GetValue(var.GetInt()).GetFactionIndex()));
		if (!faction)
			return;

		SetFaction(gameMode, faction);
	}
	
	override void UpdateInterlinkedVariables(SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, bool isInit = false)
	{
		if (!var)
			return;
		
		SCR_BaseEditorAttributeVar varOther;
		manager.GetAttributeVariable(GetTypeLinkedAttribute(), varOther);
		if (var.GetInt() != varOther.GetInt())
			return;
		
		if (var.GetInt())
			manager.SetAttributeVariable(GetTypeLinkedAttribute(), SCR_BaseEditorAttributeVar.CreateInt(0));
		else
			manager.SetAttributeVariable(GetTypeLinkedAttribute(), SCR_BaseEditorAttributeVar.CreateInt(1));
	};
	
	COE_Faction GetFaction(COE_GameMode gameMode)
	{
		return null;
	}
	
	void SetFaction(COE_GameMode gameMode, COE_Faction faction)
	{
	}
	
	typename GetTypeLinkedAttribute()
	{
		return typename.Empty;
	}
}
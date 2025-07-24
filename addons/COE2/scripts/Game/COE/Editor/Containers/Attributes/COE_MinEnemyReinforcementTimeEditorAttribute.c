//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_MinEnemyReinforcementTimeEditorAttribute : SCR_BaseValueListEditorAttribute
{
	//------------------------------------------------------------------------------------------------
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(item);
		if (!gameMode)
			return null;
		
		return SCR_BaseEditorAttributeVar.CreateFloat(gameMode.GetMinEnemyReinforcementTime());
	}
	
	//------------------------------------------------------------------------------------------------
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(item);
		if (!gameMode)
			return;
		
		if (!var)
			return;
		
		gameMode.SetMinEnemyReinforcementTime(Math.Round(var.GetFloat()));
	}
	
	//------------------------------------------------------------------------------------------------
	override void UpdateInterlinkedVariables(SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, bool isInit = false)
	{
		if (!var)
			return;
		
		SCR_BaseEditorAttributeVar varOther;
		manager.GetAttributeVariable(COE_MaxEnemyReinforcementTimeEditorAttribute, varOther);
		if (varOther.GetFloat() >= var.GetFloat())
			return;
		
		manager.SetAttributeVariable(COE_MaxEnemyReinforcementTimeEditorAttribute, SCR_BaseEditorAttributeVar.CreateFloat(var.GetFloat()));
	}
}

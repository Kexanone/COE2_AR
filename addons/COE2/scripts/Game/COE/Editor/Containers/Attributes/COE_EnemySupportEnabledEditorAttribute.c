//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_EnemySupportEnabledEditorAttribute : SCR_BaseEditorAttribute
{
	//------------------------------------------------------------------------------------------------
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(item);
		if (!gameMode)
			return null;
		
		return SCR_BaseEditorAttributeVar.CreateBool(gameMode.IsEnemySupportEnabled());
	}
	
	//------------------------------------------------------------------------------------------------
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(item);
		if (!gameMode)
			return;
		
		if (!var)
			return;
		
		gameMode.SetEnemySupportEnabled(var.GetBool());
	}
	
	//------------------------------------------------------------------------------------------------
	override void UpdateInterlinkedVariables(SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, bool isInit = false)
	{
		if (isInit)
		{
			manager.SetAttributeAsSubAttribute(COE_EnemyMortarCountEditorAttribute);
			manager.SetAttributeAsSubAttribute(COE_MinEnemyReinforcementTimeEditorAttribute);
			manager.SetAttributeAsSubAttribute(COE_MaxEnemyReinforcementTimeEditorAttribute);
		}
		
		bool enabled = var.GetBool();
		manager.SetAttributeEnabled(COE_EnemyMortarCountEditorAttribute, enabled);
		manager.SetAttributeEnabled(COE_MinEnemyReinforcementTimeEditorAttribute, enabled);
		manager.SetAttributeEnabled(COE_MaxEnemyReinforcementTimeEditorAttribute, enabled);
	}
}

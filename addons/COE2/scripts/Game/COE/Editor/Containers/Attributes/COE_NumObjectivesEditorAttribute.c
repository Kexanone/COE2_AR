[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_NumObjectivesEditorAttribute : SCR_BaseValueListEditorAttribute
{		
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(item);
		if (!gameMode)
			return null;			
		
		return SCR_BaseEditorAttributeVar.CreateFloat(gameMode.GetNumRemainingObjectives());
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(item);
		if (!gameMode)
			return;	
		
		if (!var)
			return;
		
		gameMode.SetNumRemainingObjectives(Math.Round(var.GetFloat()));
	}
}
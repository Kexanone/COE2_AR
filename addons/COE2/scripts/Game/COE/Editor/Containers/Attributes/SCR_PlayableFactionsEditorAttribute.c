//------------------------------------------------------------------------------------------------
//! Ensure that GM factions properties are disabled
[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
modded class SCR_PlayableFactionsEditorAttribute : SCR_BaseMultiSelectPresetsEditorAttribute
{
	//------------------------------------------------------------------------------------------------
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		if (COE_GameMode.Cast(item))
			return null;
		
		return super.ReadVariable(item, manager);
	}
}
class KSC_Location : Managed {};

class COE_ObjectiveAttributesComponentClass : SCR_BaseGameModeComponentClass
{
}

class COE_ObjectiveAttributesComponent : SCR_BaseGameModeComponent
{
	[Attribute(desc: "Default task type", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(KSC_ETaskType))]
	protected KSC_ETaskType m_iTaskType;
	
	protected ref KSC_Location m_pLocation;
	protected float m_fReinforcements;
	
	override bool RplSave(ScriptBitWriter writer) { return true; }
	override bool RplLoad(ScriptBitReader reader) { return true; }
}
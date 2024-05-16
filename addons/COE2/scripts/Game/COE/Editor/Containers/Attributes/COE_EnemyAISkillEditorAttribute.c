[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_EnemyAISkillEditorAttribute : SCR_BaseFloatValueHolderEditorAttribute
{
	protected static ref array<EAISkill> s_aSkills = {};
	
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(item);
		if (!gameMode)
			return null;
		
		if (s_aSkills.IsEmpty())
			SCR_Enum.GetEnumValues(EAISkill, s_aSkills);
				
		foreach (int i, EAISkill skill : s_aSkills)
		{
			if (skill == KSC_GameTools.GetAISkill())
				return SCR_BaseEditorAttributeVar.CreateInt(i);
		}
		
		return null;
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(item);
		if (!gameMode)
			return;	
		
		if (!var)
			return;
		
		KSC_GameTools.SetAISkill(s_aSkills[var.GetInt()]);
	}
	
	override int GetEntries(notnull array<ref SCR_BaseEditorAttributeEntry> outEntries)
	{
		FillValues();
		outEntries.Insert(new SCR_BaseEditorAttributeFloatStringValues(m_aValues));
		return outEntries.Count();
	}
	
	protected void FillValues()
	{
		m_aValues.Clear();	
		array<EAISkill> skillLevels = {};
		SCR_Enum.GetEnumValues(EAISkill, skillLevels);
		
		foreach (EAISkill skillLevel : skillLevels)
		{
			SCR_EditorAttributeFloatStringValueHolder value = new SCR_EditorAttributeFloatStringValueHolder();
			value.SetName(SCR_Enum.GetEnumName(EAISkill, skillLevel));
			value.SetFloatValue(skillLevel);
			m_aValues.Insert(value);
		};
	}
}

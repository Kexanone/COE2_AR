//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(EHint, "m_eHintId")]
class COE_HintEntry : Managed
{
	[Attribute(uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(EHint))]
	EHint m_eHintId;

	[Attribute()]
	ref SCR_HintUIInfo m_HintInfo;
}

//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "GameScripted/GameMode/Components", description: "")]
class COE_HintManagerComponentClass : SCR_HintManagerComponentClass
{
}

//------------------------------------------------------------------------------------------------
class COE_HintManagerComponent: SCR_HintManagerComponent
{
	[Attribute()]
	protected ref array<ref COE_HintEntry> m_aCOE_HintEntries;
	
	void COE_ShowHint(EHint id, bool isSilent = false, bool ignoreShown = false)
	{
		foreach (COE_HintEntry entry : m_aCOE_HintEntries)
		{
			if (entry.m_eHintId == id)
			{
				ShowHint(entry.m_HintInfo, isSilent, ignoreShown);
				break;
			};
		};
			
	}
}

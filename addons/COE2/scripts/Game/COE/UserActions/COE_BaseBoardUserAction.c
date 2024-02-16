//------------------------------------------------------------------------------------------------
//! Board action base visible to anyone
class COE_BaseBoardUserAction : ScriptedUserAction
{
	[Attribute(defvalue: "1", desc: "Whether it can be shown in the map radial menu", category: "Radial Menu")]
	protected bool m_bShownOnMapRadialMenu;
	
	[Attribute(defvalue: "1", desc: "Whether it can be shown in the commanding radial menu", category: "Radial Menu")]
	protected bool m_bShownOnCommandingRadialMenu;
	
	[Attribute(desc: "Image set name for the icon shown in radial menu", category: "Radial Menu")]
	protected string m_sImageSetName;
	
	//------------------------------------------------------------------------------------------------
	bool CanBeShownOnMapRadialMenu()
	{
		return m_bShownOnMapRadialMenu;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanBeShownOnCommandingRadialMenu()
	{
		return m_bShownOnCommandingRadialMenu;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetImageSetName()
	{
		return m_sImageSetName;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
modded class SCR_PlayerCommandingMenuCategoryElement : SCR_PlayerCommandingMenuBaseElement
{
	[Attribute("{2EFEA2AF1F38E7F0}UI/Textures/Icons/icons_wrapperUI-64.imageset", UIWidgets.ResourceNamePicker, "Imageset source for display icon" )]
	protected ResourceName m_sImageset;
	
	[Attribute("search", UIWidgets.EditBox, "Name of the icon associated to the command, taken from appropriate imageset set in the radial menu" )]
	protected string m_sIconName;
	
	//------------------------------------------------------------------------------------------------
	ResourceName GetIconImageset()
	{
		return m_sImageset;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetIconName()
	{
		return m_sIconName;
	}
}
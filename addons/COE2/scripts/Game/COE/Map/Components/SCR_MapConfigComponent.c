//------------------------------------------------------------------------------------------------
//! Attached to BaseGameMode, used for map config selection
modded class SCR_MapConfigComponent : ScriptComponent
{
	[Attribute("{2E2B64DBB9C9E518}Configs/Map/COE_MapLocationSelectionMenu.conf", UIWidgets.ResourceNamePicker, desc: "Location selection map config", "conf class=SCR_MapConfig")]
	protected ResourceName m_sCOE_LocationSelectionMapConfigPath;
	
	//------------------------------------------------------------------------------------------------
	//! Get config of the game master map
	ResourceName COE_GetLocationSelectionMapConfig()
	{
		return m_sCOE_LocationSelectionMapConfigPath;
	}
};

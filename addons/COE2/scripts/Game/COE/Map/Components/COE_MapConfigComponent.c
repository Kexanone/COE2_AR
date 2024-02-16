//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "GameScripted/GameMode/Components", description: "Component for map config selection")]
class COE_MapConfigComponentClass : SCR_MapConfigComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Attached to BaseGameMode, used for map config selection
class COE_MapConfigComponent : SCR_MapConfigComponent
{
	[Attribute("{2E2B64DBB9C9E518}Configs/Map/COE_MapLocationSelectionMenu.conf", UIWidgets.ResourceNamePicker, desc: "Location selection map config", "conf class=SCR_MapConfig")]
	protected ResourceName m_sLocationSelectionMapConfigPath;
	
	//------------------------------------------------------------------------------------------------
	//! Get config of the game master map
	ResourceName GetLocationSelectionMapConfig()
	{
		return m_sLocationSelectionMapConfigPath;
	}
}

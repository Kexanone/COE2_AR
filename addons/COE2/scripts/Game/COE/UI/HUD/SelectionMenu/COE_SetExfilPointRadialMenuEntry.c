//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_SetExfilPointRadialMenuEntry : COE_SelectionMenuEntry
{
	//------------------------------------------------------------------------------------------------
	override void OnPerformOnMap(vector position, COE_MapUIElement selectedMapElement)
	{
		position[1] = SCR_TerrainHelper.GetTerrainY(position);
		COE_EditorModeCommanderEntity.Request(COE_ECommanderRequest.EXFIL_POINT, position);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Restricted to commander
	override bool CanBeShownOnMap(vector position, COE_MapUIElement selectedMapElement)
	{
		if (COE_MapUILocation.Cast(selectedMapElement))
			return false;
		
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return false;
		
		return gameMode.IsCommander(SCR_PlayerController.GetLocalPlayerId());
	}
}

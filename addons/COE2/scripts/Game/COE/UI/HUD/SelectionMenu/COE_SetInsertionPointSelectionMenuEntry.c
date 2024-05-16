//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class COE_SetInsertionPointSelectionMenuEntry : COE_SelectionMenuEntry
{
	//------------------------------------------------------------------------------------------------
	override protected void OnPerform()
	{
		SCR_MapEntity mapEntity = SCR_MapEntity.Cast(GetGame().GetMapManager());
		if (!mapEntity)
			return;
		
		float x, z;
		mapEntity.GetMapCenterWorldPosition(x, z);
		vector pos = Vector(x, 0, z);
		pos[1] = SCR_TerrainHelper.GetTerrainY(pos);
		COE_EditorModeCommanderEntity.Request(COE_ECommanderRequest.INSERTION_POINT, pos);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed()
	{
		return (COE_GameMode.GetInstance().COE_GetState() != COE_EGameModeState.INTERMISSION);
	}
}

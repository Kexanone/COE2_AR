//------------------------------------------------------------------------------------------------
class COE_SelectAOUserAction : COE_BaseCommanderBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		COE_EditorModeCommanderEntity.StartEditingNextLocation();
	}
}

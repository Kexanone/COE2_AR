//------------------------------------------------------------------------------------------------
class COE_ScenarioSettingsUserAction : COE_BaseCommanderBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{		
		COE_EditorModeCommanderEntity.StartEditingAttributes(GetGame().GetGameMode());
	}
}

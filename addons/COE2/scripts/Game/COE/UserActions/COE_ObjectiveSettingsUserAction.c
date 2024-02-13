//------------------------------------------------------------------------------------------------
class COE_ObjectiveSettingsUserAction : COE_BaseCommanderBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{		
		COE_EditorModeCommanderEntity.StartEditingAttributes(GetOwner().FindComponent(COE_EditableObjectiveBoardComponent));
	}
}

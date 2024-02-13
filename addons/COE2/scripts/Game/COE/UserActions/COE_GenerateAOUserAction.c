//------------------------------------------------------------------------------------------------
class COE_GenerateAOUserAction : COE_BaseCommanderBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{		
		COE_EditorModeCommanderEntity.Request(COE_ECommanderRequest.GENERATE_AO);
	}
}

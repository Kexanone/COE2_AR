//------------------------------------------------------------------------------------------------
class COE_ObjectiveSettingsUserAction : COE_BaseSquadLeaderBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{		
		COE_GameMode gameMode = COE_GameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return;
		
		gameMode.COE_StartEditingAttributes(GetOwner().FindComponent(COE_EditableObjectiveBoardComponent));
	}
}

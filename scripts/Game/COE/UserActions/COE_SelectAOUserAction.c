//------------------------------------------------------------------------------------------------
class COE_SelectAOUserAction : COE_BaseSquadLeaderBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		SCR_EditorManagerCore core = SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));
		if (!core)
			return;

		SCR_EditorManagerEntity editorManager = core.GetEditorManager();
		if (!editorManager)
			return;

		SCR_EditorModeEntity editorEntity = editorManager.FindModeEntity(EEditorMode.COE_SQUAD_LEADER);
		if (!editorEntity)
			return;
		
		editorEntity.COE_StartEditingNextLocation();
	}
}

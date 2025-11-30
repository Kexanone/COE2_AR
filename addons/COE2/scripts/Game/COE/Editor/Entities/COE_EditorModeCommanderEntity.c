//------------------------------------------------------------------------------------------------
class COE_EditorModeCommanderEntityClass : SCR_EditorModeEntityClass
{
}

//------------------------------------------------------------------------------------------------
class COE_EditorModeCommanderEntity : SCR_EditorModeEntity
{
	protected COE_GameMode m_pGameMode;
	protected static SCR_EditorManagerEntity m_pEditorManagerEntity;
	protected static Managed m_pCurrentEditedEntity;

	//------------------------------------------------------------------------------------------------
	override void InitServer(SCR_EditorManagerEntity editorManager)
	{
		super.InitServer(editorManager);
		m_pGameMode = COE_GameMode.GetInstance();
	}
	
	//------------------------------------------------------------------------------------------------
	static void Request(COE_ECommanderRequest request, vector pos = vector.Zero)
	{
		COE_EditorModeCommanderEntity editorEntity = COE_EditorModeCommanderEntity.Cast(COE_EditorModeCommanderEntity.GetInstance());
		if (!editorEntity)
			return;
		
		editorEntity.Rpc(editorEntity.RequestServer, request, pos);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RequestServer(COE_ECommanderRequest request, vector pos)
	{
		m_pGameMode.ExecuteCommanderRequest(request, pos);
	}
	
	//------------------------------------------------------------------------------------------------
	override static SCR_EditorModeEntity GetInstance()
	{
		SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
		if (!editorManager)
			return null;
		
		return editorManager.FindModeEntity(EEditorMode.COE_COMMANDER);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Opens attribute window for the given entity
	//! Makes sure to switch to an editor with full support for attributes if possible
	static void StartEditingAttributes(Managed entity)
	{
		m_pEditorManagerEntity = SCR_EditorManagerEntity.GetInstance();
		if (!m_pEditorManagerEntity)
			return;
		
		m_pCurrentEditedEntity = entity;
		EEditorMode mode = m_pEditorManagerEntity.GetCurrentMode();
		if (mode == EEditorMode.EDIT || mode == EEditorMode.COE_COMMANDER)
		{
			StartEditingOnModeChange();
			return;
		};
		
		if (m_pEditorManagerEntity.HasMode(EEditorMode.EDIT))
		{
			m_pEditorManagerEntity.SetCurrentMode(EEditorMode.EDIT);
		}
		else if (m_pEditorManagerEntity.HasMode(EEditorMode.COE_COMMANDER))
		{
			m_pEditorManagerEntity.SetCurrentMode(EEditorMode.COE_COMMANDER);
		};
		
		if (Replication.IsClient())
		{
			m_pEditorManagerEntity.GetOnModeChange().Insert(StartEditingOnModeChange);
		}
		else
		{
			StartEditingOnModeChange();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! For scheduling opening of attribute window when a mode change was requested
	protected static void StartEditingOnModeChange(SCR_EditorModeEntity curMode = null, SCR_EditorModeEntity prevMode = null)
	{

		m_pEditorManagerEntity.GetOnModeChange().Remove(StartEditingOnModeChange);
		
		SCR_AttributesManagerEditorComponent attributesManager = SCR_AttributesManagerEditorComponent.Cast(SCR_AttributesManagerEditorComponent.GetInstance(SCR_AttributesManagerEditorComponent));
		if (!attributesManager)
			return;
		
		attributesManager.StartEditing(m_pCurrentEditedEntity);
	}

	//------------------------------------------------------------------------------------------------
	static void StartSelectingAO()
	{
		GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.COE_AOSelectionMenu, DialogPriority.INFORMATIVE, 0, true);
	}
	
	//------------------------------------------------------------------------------------------------
	static void RequestUpdateAOParams(array<ref COE_AOParams> params)
	{
		COE_EditorModeCommanderEntity editorEntity = COE_EditorModeCommanderEntity.Cast(COE_EditorModeCommanderEntity.GetInstance());
		if (!editorEntity)
			return;
		
		editorEntity.Rpc(editorEntity.RpcDo_UpdateAOParamsServer, params);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_UpdateAOParamsServer(array<ref COE_AOParams> params)
	{
		m_pGameMode.SetNextAOParams(params);
	}
}

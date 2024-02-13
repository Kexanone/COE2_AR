class COE_ScenarioSettingsButtonUIComponent: ScriptedWidgetComponent
{
	protected Widget m_Widget;
	protected COE_GameMode m_pGameMode;
	protected COE_PlayerController m_pPlayerController;

	//------------------------------------------------------------------------------------------------
	protected void OnGameStateChange()
	{
		OnPlayerRoleChange(m_pPlayerController.GetPlayerRoles());
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPlayerRoleChange(EPlayerRole roles)
	{
		// Only available during intermission
		if (m_pGameMode.COE_GetState() != COE_EGameModeState.INTERMISSION)
		{
			m_Widget.SetVisible(false);
			return;
		};
		
		// Only available to commanders and admins
		m_Widget.SetVisible(roles & (EPlayerRole.COE_COMMANDER | EPlayerRole.ADMINISTRATOR));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnClick(Widget w, int x, int y, int button)
	{
		COE_EditorModeCommanderEntity.StartEditingAttributes(GetGame().GetGameMode());
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_Widget = w;
		
		m_pGameMode = COE_GameMode.GetInstance();
		if (!m_pGameMode)
			return;
		
		m_pPlayerController = COE_PlayerController.Cast(GetGame().GetPlayerController());
		if (!m_pPlayerController)
			return;
		
		OnGameStateChange();
		m_pGameMode.COE_GetOnStateChanged().Insert(OnGameStateChange);
		m_pPlayerController.GetOnLocalPlayerRoleChange().Insert(OnPlayerRoleChange);
		
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandlerDeattached(Widget w)
	{
		m_pGameMode.COE_GetOnStateChanged().Remove(OnGameStateChange);
		m_pPlayerController.GetOnLocalPlayerRoleChange().Remove(OnPlayerRoleChange);
	}
};
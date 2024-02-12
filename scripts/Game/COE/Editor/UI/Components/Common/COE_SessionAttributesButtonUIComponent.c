class COE_SessionAttributesButtonUIComponent: ScriptedWidgetComponent
{
	protected Widget m_Widget;
	
	protected void OnPlayerRoleChange(EPlayerRole roles)
	{
		m_Widget.SetVisible(roles & EPlayerRole.COE_SQUAD_LEADER);
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		COE_GameMode gameMode = COE_GameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return false;
		
		gameMode.COE_StartEditingAttributes();
		return false;
	}
	
	override void HandlerAttached(Widget w)
	{
		m_Widget = w;
		
		COE_PlayerController playerController = COE_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return;
		
		OnPlayerRoleChange(playerController.GetPlayerRoles());
		playerController.GetOnLocalPlayerRoleChange().Insert(OnPlayerRoleChange);
	}
	override void HandlerDeattached(Widget w)
	{
		COE_PlayerController playerController = COE_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return;
		
		playerController.GetOnLocalPlayerRoleChange().Remove(OnPlayerRoleChange);
	}
};
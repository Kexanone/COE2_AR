//------------------------------------------------------------------------------------------------
class COE_PlayerControllerClass : SCR_PlayerControllerClass
{
}

//------------------------------------------------------------------------------------------------
class COE_PlayerController : SCR_PlayerController
{
	[RplProp(onRplName: "OnLocalPlayerRoleChange")]
	protected EPlayerRole m_iPlayerRoles = EPlayerRole.NONE;
	
	protected ref ScriptInvoker m_pOnLocalPlayerRoleChange = new ScriptInvoker();
	
	protected const float FADE_DURATION = 1.5;
	protected const float BLACKSCREEN_DURATION = 1;
	
	//------------------------------------------------------------------------------------------------
	static COE_PlayerController GetInstance()
	{
		return COE_PlayerController.Cast(GetGame().GetPlayerController());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Teleports local player at an emtpy position at the given location and search radius
	void RequestFastTravel(vector pos, float rotation = 0, float searchRadius = 10)
	{
		SCR_FadeInOutEffect fade = SCR_FadeInOutEffect.Cast(GetGame().GetHUDManager().FindInfoDisplay(SCR_FadeInOutEffect));
		fade.FadeOutEffect(true, FADE_DURATION);
		
		vector transform[4];
		SCR_WorldTools.FindEmptyTerrainPosition(pos, pos, searchRadius);
		KSC_GameTools.GetTransformFromPosAndRot(transform, pos, rotation);
		GetGame().GetCallqueue().CallLater(FastTravelTeleport, (FADE_DURATION + BLACKSCREEN_DURATION) * 1000, false, transform);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Helper method for RequestFastTravel; Does the teleport, notifications and fade in effect
	protected void FastTravelTeleport(vector transform[4])
	{
		BaseGameEntity.Cast(m_MainEntity).Teleport(transform);
		ChimeraWorld world = GetGame().GetWorld();
		world.GetMusicManager().Play("SOUND_COE_EXECUTION");
		SCR_FadeInOutEffect fade = SCR_FadeInOutEffect.Cast(GetGame().GetHUDManager().FindInfoDisplay(SCR_FadeInOutEffect));
		fade.FadeOutEffect(false, FADE_DURATION);
		
		SCR_PlayerTeleportedFeedbackComponent teleportFeedback = SCR_PlayerTeleportedFeedbackComponent.Cast(FindComponent(SCR_PlayerTeleportedFeedbackComponent));
		if (teleportFeedback)
			teleportFeedback.PlayerTeleported(m_MainEntity, false, SCR_EPlayerTeleportedReason.FAST_TRAVEL);
		
		SCR_NotificationsComponent.SendLocal(ENotification.FASTTRAVEL_DONE);
	}
		
	//------------------------------------------------------------------------------------------------
	void OnPlayerRoleChangeServer(EPlayerRole roles)
	{
		m_iPlayerRoles = roles;

		// Only act to change locally if not dedicated
		if (RplSession.Mode() != RplMode.Dedicated)
			OnLocalPlayerRoleChange();
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void OnLocalPlayerRoleChange()
	{
		RplComponent rpl = RplComponent.Cast(FindComponent(RplComponent));
		if (!rpl || !rpl.IsOwner())
			return;
		
		m_pOnLocalPlayerRoleChange.Invoke(m_iPlayerRoles);
	}
	
	//------------------------------------------------------------------------------------------------
	EPlayerRole GetPlayerRoles()
	{
		return m_iPlayerRoles;
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasPlayerRole(EPlayerRole role)
	{
		return m_iPlayerRoles & role;
	}
	
	//------------------------------------------------------------------------------------------------
	ScriptInvoker GetOnLocalPlayerRoleChange()
	{
		return m_pOnLocalPlayerRoleChange;
	}
}
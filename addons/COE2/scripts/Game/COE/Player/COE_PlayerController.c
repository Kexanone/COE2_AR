//------------------------------------------------------------------------------------------------
class COE_PlayerControllerClass : SCR_PlayerControllerClass
{
}

//------------------------------------------------------------------------------------------------
class COE_PlayerController : SCR_PlayerController
{
	[RplProp(onRplName: "OnLocalPlayerRoleChange")]
	protected EPlayerRole m_iPlayerRoles = EPlayerRole.NONE;
	
	protected ref ScriptInvoker m_OnLocalPlayerRoleChange = new ScriptInvoker();
	static ref ScriptInvoker s_OnLocalPlayerFastTraveled = new ScriptInvoker();
	
	protected const float FADE_DURATION = 1.5;
	protected const float BLACKSCREEN_DURATION = 1;
	
	//------------------------------------------------------------------------------------------------
	static COE_PlayerController GetInstance()
	{
		return COE_PlayerController.Cast(GetGame().GetPlayerController());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Teleports player at an emtpy position at the given location and search radius
	void RequestFastTravel(vector pos, float rotation = 0, float searchRadius = 10)
	{
		if (vector.DistanceXZ(m_MainEntity.GetOrigin(), pos) < 25)
			return;
		
		if (Replication.IsServer())
		{
			Rpc(RpcDo_Owner_FastTravel, pos, rotation, searchRadius);
		}
		else
		{
			RpcDo_Owner_FastTravel(pos, rotation, searchRadius);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_Owner_FastTravel(vector pos, float rotation, float searchRadius)
	{
		SCR_FadeInOutEffect fade = SCR_FadeInOutEffect.Cast(GetGame().GetHUDManager().FindInfoDisplay(SCR_FadeInOutEffect));
		fade.FadeOutEffect(true, FADE_DURATION);
		SCR_WorldTools.FindEmptyTerrainPosition(pos, pos, searchRadius);
		GetGame().GetCallqueue().CallLater(FastTravelTeleport, (FADE_DURATION + BLACKSCREEN_DURATION) * 1000, false, pos, rotation);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Helper method for RequestFastTravel; Does the teleport, notifications and fade in effect
	protected void FastTravelTeleport(vector pos, float rotation)
	{
		vector transform[4];
		KSC_GameTools.GetTransformFromPosAndRot(transform, pos, rotation);
		Rpc(RpcAsk_Server_FastTravelTeleport, transform);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Helper method for RequestFastTravel; Does the teleport, notifications and fade in effect
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_Server_FastTravelTeleport(vector transform[4])
	{
		if (m_MainEntity)
		{
			SCR_EditableCharacterComponent editChar = SCR_EditableCharacterComponent.Cast(m_MainEntity.FindComponent(SCR_EditableCharacterComponent));
			if (editChar)
				editChar.SetTransform(transform);
		}
		
		Rpc(RpcDo_Owner_FastTravelTeleport, transform);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Helper method for RequestFastTravel; Does the teleport, notifications and fade in effect
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_Owner_FastTravelTeleport(vector transform[4])
	{

		s_OnLocalPlayerFastTraveled.Invoke(m_MainEntity, transform);
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
		
		m_OnLocalPlayerRoleChange.Invoke(m_iPlayerRoles);
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
		return m_OnLocalPlayerRoleChange;
	}
}
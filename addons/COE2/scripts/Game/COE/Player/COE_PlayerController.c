//------------------------------------------------------------------------------------------------
class COE_PlayerControllerClass : SCR_PlayerControllerClass
{
}

//------------------------------------------------------------------------------------------------
class COE_PlayerController : SCR_PlayerController
{
	protected ref ScriptInvoker<bool> m_OnLocalCommanderRoleChanged = new ScriptInvoker<bool>();
	static ref ScriptInvoker s_OnLocalPlayerFastTraveled = new ScriptInvoker();
	
	protected const float FAST_TRAVEL_FADE_DURATION = 1.5;
	protected const float FAST_TRAVEL_BLACKSCREEN_DURATION = 1;
	protected const float FAST_TRAVEL_MIN_DISTANCE = 25;
	protected const float FAST_TRAVEL_AI_MEMBER_RANGE = 25;
	
	//------------------------------------------------------------------------------------------------
	static COE_PlayerController GetInstance()
	{
		return COE_PlayerController.Cast(GetGame().GetPlayerController());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Teleports player at an emtpy position at the given location and search radius
	void RequestFastTravel(vector pos, float rotation = 0, float searchRadius = 10)
	{
		if (vector.DistanceXZ(m_MainEntity.GetOrigin(), pos) < FAST_TRAVEL_MIN_DISTANCE)
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
		SCR_ScreenEffectsManager manager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay();
		if (manager)
		{
			SCR_FadeInOutEffect fade = SCR_FadeInOutEffect.Cast(manager.GetEffect(SCR_FadeInOutEffect));
			fade.FadeOutEffect(true, FAST_TRAVEL_FADE_DURATION);
		}
		
		SCR_WorldTools.FindEmptyTerrainPosition(pos, pos, searchRadius);
		GetGame().GetCallqueue().CallLater(FastTravelTeleport, (FAST_TRAVEL_FADE_DURATION + FAST_TRAVEL_BLACKSCREEN_DURATION) * 1000, false, pos, rotation);
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
		
		SCR_ScreenEffectsManager manager = SCR_ScreenEffectsManager.GetScreenEffectsDisplay();
		if (manager)
		{
			SCR_FadeInOutEffect fade = SCR_FadeInOutEffect.Cast(manager.GetEffect(SCR_FadeInOutEffect));
			fade.FadeOutEffect(false, FAST_TRAVEL_FADE_DURATION);
		}
		
		SCR_PlayerTeleportedFeedbackComponent teleportFeedback = SCR_PlayerTeleportedFeedbackComponent.Cast(FindComponent(SCR_PlayerTeleportedFeedbackComponent));
		if (teleportFeedback)
			teleportFeedback.PlayerTeleported(m_MainEntity, false, SCR_EPlayerTeleportedReason.FAST_TRAVEL);
		
		SCR_NotificationsComponent.SendLocal(ENotification.FASTTRAVEL_DONE);
	}

	//------------------------------------------------------------------------------------------------
	[Friend(COE_GameMode)]
	protected void OnCommanderRoleChangedServer(bool isCommander)
	{
		Rpc(RpcDo_OnCommanderRoleChangedOwner, isCommander);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_OnCommanderRoleChangedOwner(bool isCommander)
	{
		if (m_OnLocalCommanderRoleChanged)
			m_OnLocalCommanderRoleChanged.Invoke(isCommander);
	}
	
	//------------------------------------------------------------------------------------------------
	ScriptInvoker<bool> GetOnLocalCommanderRoleChanged()
	{
		if (!m_OnLocalCommanderRoleChanged)
			m_OnLocalCommanderRoleChanged = new ScriptInvoker<bool>();
		
		return m_OnLocalCommanderRoleChanged;
	}
}

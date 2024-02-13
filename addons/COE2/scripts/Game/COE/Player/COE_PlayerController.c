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
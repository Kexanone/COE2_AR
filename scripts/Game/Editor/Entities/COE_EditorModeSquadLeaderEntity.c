modded class SCR_EditorModeEntity : SCR_EditorModeEntity
{
	COE_GameMode m_pCOE_GameMode;
	
	override void InitServer(SCR_EditorManagerEntity editorManager)
	{
		super.InitServer(editorManager);
		m_pCOE_GameMode = COE_GameMode.Cast(GetGame().GetGameMode());
	}
	
	
	void COE_StartEditingNextLocation()
	{
		Rpc(COE_StartEditingNextLocationServer)
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void COE_StartEditingNextLocationServer()
	{
		Rpc(COE_StartEditingNextLocationOwner, m_pCOE_GameMode.GetNextLocation().GetOrigin());
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void COE_StartEditingNextLocationOwner(vector nextLocationPos)
	{
		IEntity location = KSC_WorldTools.GetNearestEntityByType(nextLocationPos);
		if (!location)
			return;
		
		COE_AOSelectionMenu dialog = COE_AOSelectionMenu.Cast(GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.COE_AOSelectionMenu, DialogPriority.INFORMATIVE, 0, true));
		if (!dialog)
			return;
		
		dialog.SetInitialLocation(location);
		dialog.GetOnLocationConfirmed().Insert(COE_ConfirmEditingNextLocation);
	}
	
	void COE_ConfirmEditingNextLocation(IEntity newLocation)
	{
		Rpc(COE_ConfirmEditingNextLocationServer, newLocation.GetOrigin());
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void COE_ConfirmEditingNextLocationServer(vector newLocationPos)
	{
		IEntity location = KSC_WorldTools.GetNearestEntityByType(newLocationPos);
		m_pCOE_GameMode.SetNextLocation(location);
	}
}
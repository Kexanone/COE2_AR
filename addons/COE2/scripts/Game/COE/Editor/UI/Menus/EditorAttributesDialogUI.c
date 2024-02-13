//------------------------------------------------------------------------------------------------
//! Since we want to use the attributes dialog outside of the editors, we have to ensure that
//! no HUD interfers with it
modded class EditorAttributesDialogUI: EditorMenuBase
{
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{		
		super.OnMenuOpen();
		
		SCR_HUDManagerComponent hud = GetGame().GetHUDManager();
		if (hud)
			hud.SetVisible(false);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
		super.OnMenuClose();
		
		SCR_HUDManagerComponent hud = GetGame().GetHUDManager();
		if (hud)
			hud.SetVisible(true);
	}
};
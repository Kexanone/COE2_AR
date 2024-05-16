[EntityEditorProps(category: "GameScripted/Commanding", description: "This component should be attached to player controller and is used by commanding to send requests to server.")]
class COE_PlayerControllerCommandingComponentClass : SCR_PlayerControllerCommandingComponentClass
{
};

//------------------------------------------------------------------------------------------------
class COE_PlayerControllerCommandingComponent : SCR_PlayerControllerCommandingComponent
{
	[Attribute(desc: "Entries for radial menu actions", category: "Combat Ops")]
	protected ref array<ref SCR_SelectionMenuEntry> m_aCOE_RadialMenuEntries;
	
	[Attribute(desc: "Entries for map radial menu actions", category: "Combat Ops")]
	protected ref array<ref SCR_SelectionMenuEntry> m_aCOE_MapRadialMenuEntries;
	
	//------------------------------------------------------------------------------------------------
	override void SetupPlayerRadialMenu(IEntity owner)
	{
		super.SetupPlayerRadialMenu(owner);
		m_RadialMenu.GetOnClose().Insert(COE_OnRadialMenuClose);
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetupMapRadialMenu()
	{
		// Commands restricted to commander and admins
		if (!(COE_PlayerController.Cast(GetOwner()).GetPlayerRoles() & (EPlayerRole.COE_COMMANDER | EPlayerRole.ADMINISTRATOR)))
			return;
				
		foreach (SCR_SelectionMenuEntry entry : m_aCOE_MapRadialMenuEntries)
		{
			SCR_SelectionMenuCategoryEntry category = SCR_SelectionMenuCategoryEntry.Cast(entry);
			if (category)
			{
				m_MapContextualMenu.InsertCustomRadialCategory(category);
			}
			else
			{
				m_MapContextualMenu.InsertCustomRadialEntry(entry);
			};
		};
		
		COE_UpdateRadialMenuEntries(m_aCOE_MapRadialMenuEntries);
		GetGame().GetCallqueue().CallLater(COE_UpdateRadialMenuEntries, SCR_GameModeCampaign.DEFAULT_DELAY, true, m_aCOE_MapRadialMenuEntries);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerRadialMenuOpen()
	{	
		super.OnPlayerRadialMenuOpen();
			
		// Commands restricted to commander and admins
		if (!(COE_PlayerController.Cast(GetOwner()).GetPlayerRoles() & (EPlayerRole.COE_COMMANDER | EPlayerRole.ADMINISTRATOR)))
			return;
				
		foreach (SCR_SelectionMenuEntry entry : m_aCOE_RadialMenuEntries)
		{
			SCR_SelectionMenuCategoryEntry category = SCR_SelectionMenuCategoryEntry.Cast(entry);
			if (category)
			{
				m_RadialMenu.AddCategoryEntry(category);
			}
			else
			{
				m_RadialMenu.AddEntry(entry);
			};
		};
		
		COE_UpdateRadialMenuEntries(m_aCOE_RadialMenuEntries);
		GetGame().GetCallqueue().CallLater(COE_UpdateRadialMenuEntries, SCR_GameModeCampaign.DEFAULT_DELAY, true, m_aCOE_RadialMenuEntries);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void COE_UpdateRadialMenuEntries(array<ref SCR_SelectionMenuEntry> entries)
	{
		foreach (SCR_SelectionMenuEntry entry : entries)
			entry.Update();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void COE_OnRadialMenuClose()
	{
		GetGame().GetCallqueue().Remove(COE_UpdateRadialMenuEntries);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMapClose(MapConfiguration config)
	{
		super.OnMapClose(config);
		GetGame().GetCallqueue().Remove(COE_UpdateRadialMenuEntries);
	}
}
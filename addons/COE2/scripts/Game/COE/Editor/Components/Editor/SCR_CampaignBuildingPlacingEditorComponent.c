//------------------------------------------------------------------------------------------------
modded class SCR_CampaignBuildingPlacingEditorComponent : SCR_PlacingEditorComponent
{
	//------------------------------------------------------------------------------------------------
	//! Handle autorecruiting of placed AI agents
	override protected void OnEntityCreatedServer(array<SCR_EditableEntityComponent> entities)
	{
		super.OnEntityCreatedServer(entities);
		
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode || !gameMode.ShouldAutorecruitAI())
			return;
		
		foreach (SCR_EditableEntityComponent editableEntity : entities)
		{
			SCR_EditableGroupComponent editableGroup = SCR_EditableGroupComponent.Cast(editableEntity);
			if (!editableGroup)
				continue;
			
			SCR_AIGroup group = editableGroup.GetAIGroupComponent();
			if (group)
				group.GetOnInit().Insert(COE_RecruitAIAgents);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Recruit all AI agents of the group
	protected void COE_RecruitAIAgents(SCR_AIGroup group)
	{
		if (!group)
			return;
		
		int playerId = GetManager().GetPlayerID();
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!playerController)
			return;
		
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.Cast(playerController.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!groupController)
			return;
		
		foreach (SCR_ChimeraCharacter char : KSC_GroupHelper.GetUnits(group))
		{
			groupController.RequestAddAIAgent(char);
		}
		
		group.GetOnInit().Remove(COE_RecruitAIAgents);
	}
}

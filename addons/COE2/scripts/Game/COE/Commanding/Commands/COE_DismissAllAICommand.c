//------------------------------------------------------------------------------------------------
//! Delete all AI members command
[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class COE_DismissAllAICommand : SCR_BaseGroupCommand
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeExecuted(IEntity target)
	{
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.GetLocalPlayerControllerGroupComponent();
		if (!groupController)
			return false;
		
		SCR_AIGroup group = groupController.GetPlayersGroup();
		if (!group)
			return false;
		
		SCR_AIGroup slaveGroup = group.GetSlave();
		if (!slaveGroup || slaveGroup.GetServerAgentsCount() <= 0)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool Execute(IEntity cursorTarget, IEntity groupEnt, vector targetPosition, int playerID, bool isClient)
	{
		if (isClient)
			return true;
				
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerID));
		if (!playerController)
			return false;
		
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.Cast(playerController.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!groupController)
			return false;
		
		SCR_AIGroup group = groupController.GetPlayersGroup();
		if (!group)
			return false;
		
		SCR_AIGroup slaveGroup = group.GetSlave();
		if (!slaveGroup)
			return false;
		
		foreach (SCR_ChimeraCharacter char : KSC_GroupHelper.GetUnits(slaveGroup))
		{
			SCR_EntityHelper.DeleteEntityAndChildren(char);
		}
		
		return true;
	}
}

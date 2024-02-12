//------------------------------------------------------------------------------------------------
class COE_DeployUserAction : COE_BaseBoardUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{		
		GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.COE_DeploymentMenu, DialogPriority.INFORMATIVE, 0, true);
	}
}

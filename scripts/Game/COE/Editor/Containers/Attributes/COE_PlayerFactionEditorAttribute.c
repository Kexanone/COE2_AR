[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_PlayerFactionEditorAttribute: COE_BaseFactionEditorAttribute
{		
	override COE_Faction GetFaction(COE_GameMode gameMode)
	{
		return gameMode.GetPlayerFaction();
	}
	
	override void SetFaction(COE_GameMode gameMode, COE_Faction faction)
	{
		gameMode.SetPlayerFaction(faction);
	}
	
	override typename GetTypeLinkedAttribute()
	{
		return COE_EnemyFactionEditorAttribute;
	}
}
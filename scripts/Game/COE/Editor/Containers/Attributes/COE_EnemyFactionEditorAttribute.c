[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class COE_EnemyFactionEditorAttribute: COE_BaseFactionEditorAttribute
{		
	override COE_Faction GetFaction(COE_GameMode gameMode)
	{
		return gameMode.GetEnemyFaction();
	}
	
	override void SetFaction(COE_GameMode gameMode, COE_Faction faction)
	{
		gameMode.SetEnemyFaction(faction);
	}
	
	override typename GetTypeLinkedAttribute()
	{
		return COE_PlayerFactionEditorAttribute;
	}
}
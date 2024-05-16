/*
modded class ArmaReforgerScripted : ChimeraGame
{
	ref KSC_BuildingPosQuery KSC_TMP;
	
	override bool OnGameStart()
	{
		KSC_TMP = new KSC_BuildingPosQuery(GetGame().GetWorld().FindEntityByName("KEK"));
		return super.OnGameStart();
	}
}
*/

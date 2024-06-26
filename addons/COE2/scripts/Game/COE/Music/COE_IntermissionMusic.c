class COE_IntermissionMusic : ScriptedMusic
{
	MusicManager m_MusicManager;
	
	//------------------------------------------------------------------------------------------------
	// On local player spawned
	protected void OnPlayerSpawned()
	{
		// Location check has to be done a frame later to get the correct player position
		GetGame().GetCallqueue().CallLater(CheckLocationAndPlay, 100, false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CheckLocationAndPlay()
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return;
		
		// Play briefing music when spawned at main base
		if (vector.DistanceXZ(gameMode.GetMainBasePos(), SCR_PlayerController.GetLocalMainEntity().GetOrigin()) > 25)
			return;
		
		m_MusicManager.Play("SOUND_COE_INTERMISSION");	
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init() 
	{
		ChimeraWorld world = GetGame().GetWorld();

		if (!world)
			return;
		
		m_MusicManager = world.GetMusicManager();
		if (!m_MusicManager)
			return;
		
		SCR_RespawnComponent.SGetOnLocalPlayerSpawned().Insert(OnPlayerSpawned);
		COE_PlayerController.s_OnLocalPlayerFastTraveled.Insert(OnPlayerSpawned);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDelete()
	{
		if (!m_MusicManager)
			return;
		
		SCR_RespawnComponent.SGetOnLocalPlayerSpawned().Remove(OnPlayerSpawned);
		COE_PlayerController.s_OnLocalPlayerFastTraveled.Remove(OnPlayerSpawned);
	}
}
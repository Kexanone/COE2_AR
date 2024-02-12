class COE_BriefingMusic : ScriptedMusic
{
	MusicManager m_MusicManager;
	
	//------------------------------------------------------------------------------------------------
	// On local player spawned
	protected void OnPlayerSpawned()
	{
		// Location check has to be done a frame later to get the correct player position
		GetGame().GetCallqueue().Call(CheckLocationAndPlay);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CheckLocationAndPlay()
	{
		COE_GameMode gameMode = COE_GameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return;
		
		// Play briefing music when spawned at main base
		if (vector.Distance(gameMode.GetMainBasePos(), SCR_PlayerController.GetLocalMainEntity().GetOrigin()) > 100)
			return;
		
		m_MusicManager.Play("SOUND_COE_BRIEFING");	
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
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDelete()
	{
		if (!m_MusicManager)
			return;
		
		SCR_RespawnComponent.SGetOnLocalPlayerSpawned().Remove(OnPlayerSpawned);
	}
}
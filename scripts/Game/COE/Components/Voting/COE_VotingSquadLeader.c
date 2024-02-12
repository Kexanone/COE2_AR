[BaseContainerProps(configRoot:true), SCR_BaseContainerCustomTitleEnum(EVotingType, "m_Type")]
class COE_VotingSquadLeaderIn: SCR_VotingReferendum
{
	protected SCR_EditorManagerEntity m_EditorManager;
	
	override bool IsAvailable(int value, bool isOngoing)
	{
		if (isOngoing)
			return true;
		
		//--- Players can only nominate themselves
		COE_PlayerController playerController = COE_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController || playerController.GetPlayerId() != value)
			return false;

		//--- Can only vote for players that are not yet squad leaders
		return !playerController.HasPlayerRole(EPlayerRole.COE_SQUAD_LEADER);
	}
	
	override bool CanSendNotification(int value)
	{
		if (!Replication.IsRunning())
			return false;
		
		SCR_VotingManagerComponent votingManager = SCR_VotingManagerComponent.GetInstance();
		SCR_GameModeEditor gameMode = SCR_GameModeEditor.Cast(GetGame().GetGameMode());
		return (votingManager && votingManager.GetHostPlayerID() != value) || (gameMode && gameMode.GetGameMasterTarget() != EGameModeEditorTarget.EVERYBODY);
	}
	
	override void OnVotingEnd(int value = DEFAULT_VALUE, int winner = DEFAULT_VALUE)
	{
		if (winner == DEFAULT_VALUE)
			return;
		
		GetGame().GetPlayerManager().GivePlayerRole(winner, EPlayerRole.COE_SQUAD_LEADER);
	}
};
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(EVotingType, "m_Type")]
class COE_VotingSquadLeaderOut: COE_VotingSquadLeaderIn
{
	override protected int GetPlayerCount()
	{
		return Math.Max(super.GetPlayerCount() - 1, 2); //--- Ignore target player. 2 is a limit to prevent instant completion in a session with just 2 people.
	}
	
	override bool IsAvailable(int value, bool isOngoing)
	{
		//--- Cannot vote for self
		if (SCR_PlayerController.GetLocalPlayerId() == value)
			return false;
		
		//--- Cannot vote host or admin out
		SCR_VotingManagerComponent votingManager = SCR_VotingManagerComponent.GetInstance();
		if (!votingManager || votingManager.GetHostPlayerID() == value || GetGame().GetPlayerManager().HasPlayerRole(value, EPlayerRole.ADMINISTRATOR))
			return false;
		
		COE_PlayerController subjectPlayerController = COE_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(value));
		if (!subjectPlayerController)
			return false;
		
		//--- Can only vote for players that are squad leaders
		return subjectPlayerController.HasPlayerRole(EPlayerRole.COE_SQUAD_LEADER);
	}
	
	override bool CanSendNotification(int value)
	{
		SCR_GameModeEditor gameMode = SCR_GameModeEditor.Cast(GetGame().GetGameMode());
		return gameMode && gameMode.GetGameMasterTarget() != EGameModeEditorTarget.EVERYBODY;
	}
	
	override void OnVotingEnd(int value = DEFAULT_VALUE, int winner = DEFAULT_VALUE)
	{
		if (winner == DEFAULT_VALUE)
			return;
		
		GetGame().GetPlayerManager().ClearPlayerRole(winner, EPlayerRole.COE_SQUAD_LEADER);
	}
};

[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(EVotingType, "m_Type")]
class COE_VotingSquadLeaderWithdraw: COE_VotingSquadLeaderIn
{
	override bool Evaluate(out EVotingOutcome outcome)
	{
		return true;
	}
	
	override bool IsAvailable(int value, bool isOngoing)
	{
		//--- Players can only vote themselves
		COE_PlayerController playerController = COE_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController || playerController.GetPlayerId() != value)
			return false;
		
		//--- Cannot apply when a host or an admin
		SCR_VotingManagerComponent votingManager = SCR_VotingManagerComponent.GetInstance();
		if (!votingManager || votingManager.GetHostPlayerID() == value || GetGame().GetPlayerManager().HasPlayerRole(value, EPlayerRole.ADMINISTRATOR))
			return false;
		
		//--- Can only vote for players that are not yet squad leaders
		return playerController.HasPlayerRole(EPlayerRole.COE_SQUAD_LEADER);
	}
	
	override void OnVotingEnd(int value = DEFAULT_VALUE, int winner = DEFAULT_VALUE)
	{
		if (winner == DEFAULT_VALUE)
			return;
		
		GetGame().GetPlayerManager().ClearPlayerRole(winner, EPlayerRole.COE_SQUAD_LEADER);

	}
}
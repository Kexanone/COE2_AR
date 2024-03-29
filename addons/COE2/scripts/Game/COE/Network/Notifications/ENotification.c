modded enum ENotification
{
	// Voting
	COE_VOTING_COMMANDER_IN_START, ///< Voting started to become commander (param1 = player id)
	COE_VOTING_COMMANDER_IN_SUCCEED, ///< Voting succeeded to become commander (param1 = player id)
	COE_VOTING_COMMANDER_IN_FAIL, ///< Voting failed to become commander (param1 = player id)
	COE_VOTING_COMMANDER_IN_VOTE_CASTED, ///< Voting failed to become commander (param1 = player id)
	COE_VOTING_COMMANDER_IN_VOTE_ABSTAINED, ///< Voting cast locally by player to make other commander (param1 = player id)
	COE_VOTING_COMMANDER_OUT_START,///< Voting abstained locally by player to make other commander (param1 = player id)
	COE_VOTING_COMMANDER_OUT_SUCCEED, ///< Voting succeeded to remove commander (param1 = player id)
	COE_VOTING_COMMANDER_OUT_FAIL, ///< Voting failed to remove commander (param1 = player id)
	COE_VOTING_COMMANDER_OUT_VOTE_CASTED, ///< Voting cast locally by player to remove other commander (param1 = player id)
	COE_VOTING_COMMANDER_OUT_VOTE_ABSTAINED, ///< Voting abstained locally by player to remove other commander (param1 = player id)
	// Rights
	COE_COMMANDER_ROLE_ASSIGNED, ///< Player got rights assigned - (param1 = GameMasterID, param2 = TargetPlayerID))
	COE_COMMANDER_ROLE_REMOVED ///< Player got rights revoked - (param1 = GameMasterID, param2 = TargetPlayerID)
}


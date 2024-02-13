enum COE_EEntityLabel
{
	PLAYER_MAIN_BASE,
	PLAYER_MAIN_BASE_CARRIER,
	PLAYER_SPAWN,
	ARSENAL_BOX,
	SMALL_VEHICLE_REQUEST_BOARD,
	HELICOPTER_REQUEST_BOARD,
	INSTALLATION_TO_DESTROY,
	SUPPLIES_TO_DESTROY,
	HVT,
	INTEL,
	SMALL_ROAD_SLOT,
	MEDIUM_ROAD_SLOT,
	LARGE_ROAD_SLOT,
	SMALL_FLAT_SLOT,
	MEDIUM_FLAT_SLOT,
	LARGE_FLAT_SLOT,
	MG_NEST,
	BUNKER,
	ARMED_VEHICLE,
	MARKSMAN,
	RIFLEMAN_MG,
	RIFLEMAN_GL,
	RIFLEMAN_AT,
	RIFLEMAN,
	PATROL_GROUP,
	CHECKPOINT_GROUP
}

//------------------------------------------------------------------------------------------------
class COE_Faction : SCR_Faction
{
	[Attribute(desc: "Prefabs for player's main base")]
	protected ref array<ResourceName> m_aPlayerMainBasePrefabs;
	
	[Attribute(desc: "Prefabs for player's main base carrier")]
	protected ref array<ResourceName> m_aPlayerMainBaseCarrierPrefabs;
	
	[Attribute(desc: "Prefabs for player spawn")]
	protected ref array<ResourceName> m_aPlayerSpawnPrefabs;
	
	[Attribute(desc: "Prefabs for arsenal box for players")]
	protected ref array<ResourceName> m_aArsenalBoxPrefabs;
	
	[Attribute(desc: "Prefabs for requesting small vehicles")]
	protected ref array<ResourceName> m_aSmallVehicleRequestBoardPrefabs;
	
	[Attribute(desc: "Prefabs for requesting helicopters vehicles")]
	protected ref array<ResourceName> m_aHelicopterRequestBoardPrefabs;
	
	[Attribute(desc: "Prefabs for installations to be destroyed")]
	protected ref array<ResourceName> m_aInstallationToDestroyPrefabs;
	
	[Attribute(desc: "Prefabs for supplies to be destroyed")]
	protected ref array<ResourceName> m_aSuppliesToDestroyPrefabs;
	
	[Attribute(desc: "Prefabs for high value targets")]
	protected ref array<ResourceName> m_aHVTPrefabs;
	
	[Attribute(desc: "Prefabs for pickable intels")]
	protected ref array<ResourceName> m_aIntelPrefabs;
	
	[Attribute(desc: "Prefabs for small road slots")]
	protected ref array<ResourceName> m_aSmallRoadSlotPrefabs;
	
	[Attribute(desc: "Prefabs for medium road slots")]
	protected ref array<ResourceName> m_aMediumRoadSlotPrefabs;
	
	[Attribute(desc: "Prefabs for large road slots")]
	protected ref array<ResourceName> m_aLargeRoadSlotPrefabs;
	
	[Attribute(desc: "Prefabs for large flat slots")]
	protected ref array<ResourceName> m_aLargeFlatSlotPrefabs;
	
	[Attribute(desc: "Prefabs for MG nests")]
	protected ref array<ResourceName> m_aMGNestPrefabs;
	
	[Attribute(desc: "Prefabs for armed vehicles")]
	protected ref array<ResourceName> m_aArmedVehiclePrefabs;
	
	[Attribute(desc: "Prefabs for marksman")]
	protected ref array<ResourceName> m_aMarksmanPrefabs;
	
	[Attribute(desc: "Prefabs for rifleman MG")]
	protected ref array<ResourceName> m_aRiflemanMGPrefabs;
	
	[Attribute(desc: "Prefabs for rifleman GL")]
	protected ref array<ResourceName> m_aRiflemanGLPrefabs;

	[Attribute(desc: "Prefabs for rifleman AT")]
	protected ref array<ResourceName> m_aRiflemanATPrefabs;
	
	[Attribute(desc: "Prefabs for rifleman")]
	protected ref array<ResourceName> m_aRiflemanPrefabs;
	
	[Attribute(desc: "Prefabs for patrol groups")]
	protected ref array<ResourceName> m_aPatrolGroupPrefabs;
	
	[Attribute(desc: "Prefabs for groups that occupy checkpoints")]
	protected ref array<ResourceName> m_aCheckpointGroupPrefabs;
		
	protected IEntity m_pMainBase;
	
	//------------------------------------------------------------------------------------------------
	void SetPlayerMainBase(IEntity mainBase)
	{
		m_pMainBase = mainBase;
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetPlayerMainBase()
	{
		return m_pMainBase;
	}
	
	//------------------------------------------------------------------------------------------------
	ResourceName GetRandomPrefabByLabel(COE_EEntityLabel label)
	{
		array<ResourceName> prefabs = GetPrefabsByLabel(label);
		if (prefabs.IsEmpty())
			return string.Empty;
		
		Math.Randomize(-1);
		return prefabs[Math.RandomInt(0, prefabs.Count())];
	}
	
	
	//------------------------------------------------------------------------------------------------
	array<ResourceName> GetPrefabsByLabel(COE_EEntityLabel label)
	{
		switch (label)
		{
			case COE_EEntityLabel.PLAYER_MAIN_BASE: {return m_aPlayerMainBasePrefabs;};
			case COE_EEntityLabel.PLAYER_MAIN_BASE_CARRIER: {return m_aPlayerMainBaseCarrierPrefabs;};
			case COE_EEntityLabel.PLAYER_SPAWN: {return m_aPlayerSpawnPrefabs;};
			case COE_EEntityLabel.ARSENAL_BOX: {return m_aArsenalBoxPrefabs;};
			case COE_EEntityLabel.SMALL_VEHICLE_REQUEST_BOARD: {return m_aSmallVehicleRequestBoardPrefabs;};
			case COE_EEntityLabel.HELICOPTER_REQUEST_BOARD: {return m_aHelicopterRequestBoardPrefabs;};
			case COE_EEntityLabel.INSTALLATION_TO_DESTROY: {return m_aInstallationToDestroyPrefabs;};
			case COE_EEntityLabel.SUPPLIES_TO_DESTROY: {return m_aSuppliesToDestroyPrefabs;};
			case COE_EEntityLabel.HVT: {return m_aHVTPrefabs;};
			case COE_EEntityLabel.INTEL: {return m_aIntelPrefabs;};
			case COE_EEntityLabel.SMALL_ROAD_SLOT: {return m_aSmallRoadSlotPrefabs;};
			case COE_EEntityLabel.MEDIUM_ROAD_SLOT: {return m_aMediumRoadSlotPrefabs;};
			case COE_EEntityLabel.LARGE_ROAD_SLOT: {return m_aLargeRoadSlotPrefabs;};
			case COE_EEntityLabel.LARGE_FLAT_SLOT: {return m_aLargeFlatSlotPrefabs;};
			case COE_EEntityLabel.MG_NEST: {return m_aMGNestPrefabs;};
			case COE_EEntityLabel.ARMED_VEHICLE: {return m_aArmedVehiclePrefabs;};
			case COE_EEntityLabel.MARKSMAN: {return m_aMarksmanPrefabs;};
			case COE_EEntityLabel.RIFLEMAN: {return m_aRiflemanPrefabs;};
			case COE_EEntityLabel.PATROL_GROUP: {return m_aPatrolGroupPrefabs;};
			case COE_EEntityLabel.CHECKPOINT_GROUP: {return m_aCheckpointGroupPrefabs;};
		};
		
		return {};
	}
};

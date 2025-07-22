//------------------------------------------------------------------------------------------------
class COE_AOClass : KSC_AOClass
{
}

//------------------------------------------------------------------------------------------------
class COE_AO : KSC_AO
{
	protected ref array<IEntity> m_aEntities = {};
	protected ref array<KSC_BaseTask> m_aTasks = {};
	protected ref array<SCR_MapMarkerBase> m_aMarkers = {};
	protected COE_GameMode m_pGameMode;
	protected COE_FactionManager m_pFactionManager;
	protected COE_AOParams m_Params;
	protected ref KSC_AreaBase m_Area;
	protected int m_iNumTasks = 0;
	protected int m_iNumCompletedTasks = 0;
	
	protected int m_iAiCount = 0;
	protected int m_iMaxAICount;
	
	protected ref map<EEditableEntityLabel, ref array<vector>> m_mTerrainSlots = new map<EEditableEntityLabel, ref array<vector>>();
	protected ref map<EEditableEntityLabel, ref array<ref Tuple2<IEntity, array<ref PointInfo>>>> m_mBuildingSlots = new map<EEditableEntityLabel, ref array<ref Tuple2<IEntity, array<ref PointInfo>>>>();
	protected static ref map<EEditableEntityLabel, float> s_mSlotRadii;
	
	protected ref array<vector> m_aPositionsToDefend = {};
	protected ref array<vector> m_aEnemyPositionsToReveal = {};
	
	protected static const int TERRAIN_DATA_BUFFER_SIZE = 1000000;
	protected static const int MAX_ATTEMPTS = 10000;
			
	//------------------------------------------------------------------------------------------------
	void COE_AO(IEntitySource src, IEntity parent)
	{
		if (!Replication.IsServer())
			return;
		
		GetGame().GetCallqueue().CallLater(DelayedInit, 100);
	}
	
	//------------------------------------------------------------------------------------------------
	void DelayedInit()
	{
		Math.Randomize(-1);
		m_pGameMode = COE_GameMode.GetInstance();
		m_pFactionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		m_Params = m_pGameMode.GetCurrentAOParams();
		m_Area = m_pGameMode.GetCurrentLocation().m_Area;
		m_iMaxAICount = m_pGameMode.GetTargetEnemyAICount();
		ScanTerrainSlots();
		ScanBuildingSlots();
		SetUpTasks();
		
		if (m_aTasks.IsEmpty())
		{
			SCR_EntityHelper.DeleteEntityAndChildren(this);
			return;
		}
		
		SetUpFortifications();
		SetUpRoadBlocks();
		SetUpSnipers();
		SetUpDefenders();
		SetUpPatrols();
		SetUpArmedVehicles();
		
		SetUpCivlianVehicles();
		
		if (m_pGameMode.CiviliansEnabled())
			SetUpCivilians();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ScanTerrainSlots()
	{
		KSC_WorldSlotsConfig config = COE_GameMode.GetInstance().KSC_GetWorldSlotsConfig();
		vector mins, maxs;
		m_Area.GetBoundBox(mins, maxs);
		
		foreach (KSC_TerrainSlotConfig configEntry : config.m_mTerrainSlots)
		{
			array<vector> data = {};
			data.Reserve(TERRAIN_DATA_BUFFER_SIZE);
						
			for (int i = 0; i < configEntry.m_mMatrix.GetRowIcs().Count(); i++)
			{
				float x = configEntry.m_mMatrix.GetRowIcs()[i];
				if (x < mins[0] || x > maxs[0])
					continue;
				
				float z = configEntry.m_mMatrix.GetColIcs()[i];
				if (z < mins[2] || z > maxs[2])
					continue;
				
				vector v =  Vector(x, configEntry.m_mMatrix.GetData()[i], z);
				if (!m_Area.IsPointInArea(v))
					continue;
				
				data.Insert(v);
			}
			
			if (data.IsEmpty())
				continue;
			
			m_mTerrainSlots.Insert(configEntry.m_eLabel, data);
		}
		
		if (!s_mSlotRadii)
		{
			s_mSlotRadii =  new map<EEditableEntityLabel, float>();
			s_mSlotRadii[EEditableEntityLabel.SLOT_FLAT_SMALL] = 7;
			s_mSlotRadii[EEditableEntityLabel.SLOT_FLAT_MEDIUM] = 14;
			s_mSlotRadii[EEditableEntityLabel.SLOT_FLAT_LARGE] = 21;
			s_mSlotRadii[EEditableEntityLabel.SLOT_ROAD_SMALL] = 7;
			s_mSlotRadii[EEditableEntityLabel.SLOT_ROAD_MEDIUM] = 14;
			s_mSlotRadii[EEditableEntityLabel.SLOT_ROAD_LARGE] = 21;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ScanBuildingSlots()
	{
		vector mins, maxs;
		m_Area.GetBoundBox(mins, maxs);
		GetGame().GetWorld().QueryEntitiesByAABB(mins, maxs, QueryBuildingCallback);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool QueryBuildingCallback(IEntity entity)
	{
		if (entity.GetParent())
			return true;
		
		VObject vObject = entity.GetVObject();
		if (!vObject)
			return true;
		
		ResourceName xobName = vObject.GetResourceName();
		
		foreach (KSC_BuildingSlotConfigEntry entry : m_pGameMode.KSC_GetWorldSlotsConfig().m_pBuildingSlotConfig.m_aBuildingSlots)
		{
			if (entry.m_sBuildingXobName != xobName)
				continue;
			
			array<ref Tuple2<IEntity, array<ref PointInfo>>> slots;
			if (m_mBuildingSlots.Contains(entry.m_eLabel))
			{
				slots = m_mBuildingSlots[entry.m_eLabel];
			}
			else
			{
				slots = {};
				m_mBuildingSlots[entry.m_eLabel] = slots;
			}
			
			slots.Insert(new Tuple2<IEntity, array<ref PointInfo>>(entity, entry.m_aUnitPositions));
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	Tuple2<IEntity, array<ref PointInfo>> GetRandomBuildingWithSlots(EEditableEntityLabel label, bool addGarrison = true)
	{
		array<ref Tuple2<IEntity, array<ref PointInfo>>> buildingEntries;
		if (!m_mBuildingSlots.Find(label, buildingEntries))
			return null;
		
		if (buildingEntries.IsEmpty())
			return null;
		
		Tuple2<IEntity, array<ref PointInfo>> entry = buildingEntries.GetRandomElement();
		
		if (addGarrison)
		{			
			array<ResourceName> entries = {};
			m_pFactionManager.GetFactionEntityListWithLabel(m_pFactionManager.GetEnemyFaction(), EEntityCatalogType.GROUP, EEditableEntityLabel.GROUPSIZE_SMALL, entries);
			if (entries.IsEmpty())
				return entry;
			
			foreach (PointInfo point : entry.param2)
			{
				point.Init(entry.param1);
				vector transform[4];
				point.GetWorldTransform(transform);
				
				SCR_AIGroup group = SpawnGroupPrefab(entries.GetRandomElement(), transform[3]);
				if (!group)
					return entry;
				
				PrintFormat("%1|%2", transform[3], group.GetOrigin());
				
				KSC_AITasks.Defend(group, transform[3], 10);
				AddGroup(group);
			}
		}
		
		return entry;
	}
	
	protected ref array<ref Tuple2<vector, float>> m_aSniperPoints;
	protected ref set<IEntity> m_aSniperStructures;
	
	//------------------------------------------------------------------------------------------------
	void SetUpSnipers()
	{
		m_aSniperPoints = {};
		m_aSniperStructures = new set<IEntity>();
		vector mins, maxs;
		m_Area.GetBoundBox(mins, maxs);
		GetGame().GetWorld().QueryEntitiesByAABB(mins, maxs, SpawnSnipersOnBuildingsCallback);
		if (m_aSniperPoints.IsEmpty())
			return;
		
		array<ResourceName> entries = {};
		m_pFactionManager.GetFactionEntityListWithLabel(m_pFactionManager.GetEnemyFaction(), EEntityCatalogType.CHARACTER, EEditableEntityLabel.ROLE_SHARPSHOOTER, entries);
		if (entries.IsEmpty())
			return;
		
		int minCount = m_iMaxAICount / 10;
		int count = Math.RandomIntInclusive(minCount, minCount+1);
		for (int i = 0; i < count; i++)
		{
			int idx = m_aSniperPoints.GetRandomIndex();
			Tuple2<vector, float> point = m_aSniperPoints[idx];
			SCR_ChimeraCharacter char = KSC_GameTools.SpawnCharacterPrefab(entries.GetRandomElement(), point.param1, point.param2);
			AddEntity(char);
			AIWaypoint wp = KSC_GameTools.SpawnWaypointPrefab("{84FB298EA1F2C7CE}Prefabs/AI/Waypoints/KSC_AIWaypoint_Defend_0m.et", point.param1);
			KSC_AIHelper.GetGroup(char).AddWaypoint(wp);
			AddEntity(wp);
			m_aSniperPoints.Remove(idx);
			
			if (m_aSniperPoints.IsEmpty())
				return;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool SpawnSnipersOnBuildingsCallback(IEntity entity)
	{
		if (entity.GetParent())
			return true;
		
		VObject vObject = entity.GetVObject();
		if (!vObject)
			return true;
		
		// Workaround: Same structure can be queried multiple times
		if (m_aSniperStructures.Contains(entity))
			return true;
		
		ResourceName xobName = vObject.GetResourceName();
		
		foreach (KSC_BuildingSlotConfigEntry entry : m_pGameMode.KSC_GetWorldSlotsConfig().m_pBuildingSlotConfig.m_aBuildingSlots)
		{
			if (entry.m_eLabel != EEditableEntityLabel.ROLE_SHARPSHOOTER)
				continue;
			
			if (entry.m_sBuildingXobName != xobName)
				continue;
			
			PointInfo point = entry.m_aUnitPositions.GetRandomElement();
			point.Init(entity);
			vector transform[4];
			point.GetWorldTransform(transform);
			m_aSniperPoints.Insert(new Tuple2<vector, float>(transform[3], transform[2].ToYaw()));
			m_aSniperStructures.Insert(entity);
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetUpTasks()
	{
		foreach (int i, COE_BaseTaskBuilder builder : m_pGameMode.GetAvailableTaskBuilders())
		{
			if (!KSC_BitTools.IsBitSet(m_Params.m_eTaskTypes, i))
				continue;
			
			KSC_BaseTask task = builder.Build(this);
			if (!task)
				continue;
			
			task.GetOnStateChanged().Insert(OnTaskStateChanged);
			m_aTasks.Insert(task);
			m_iNumTasks++;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! --- To Do: Refactor duplicate code from SetUpRoadBlocks
	void SetUpFortifications()
	{
		array<EEditableEntityLabel> labels = {EEditableEntityLabel.SLOT_FLAT_SMALL, EEditableEntityLabel.SLOT_FLAT_MEDIUM, EEditableEntityLabel.SLOT_FLAT_LARGE};
		
		int count = Math.RandomIntInclusive(4, 6);
		int counter = 0;
		while (counter < count)
		{
			if (labels.IsEmpty())
				break;
			
			EEditableEntityLabel label = labels.GetRandomElement();
			array<ResourceName> entries = {};
			m_pFactionManager.GetFactionEntityListWithLabels(m_pFactionManager.GetEnemyFaction(), EEntityCatalogType.COMPOSITION,
				includedLabels: {label, EEditableEntityLabel.TRAIT_FORTIFICATION},
				excludedLabels: {EEditableEntityLabel.TRAIT_MORTAR},
			entries);
			
			if (entries.IsEmpty())
			{
				labels.RemoveItem(label);
				continue;
			}
			
			IEntity structure = SpawnInRandomFlatSlot(entries.GetRandomElement(), label);
			if (!structure)
			{
				labels.RemoveItem(label);
				continue;
			}
			
			m_aEnemyPositionsToReveal.Insert(structure.GetOrigin());
			AddEntity(structure);
						
			if (!SpawnTurretOccupants(structure))
				SpawnSmartInteractionOccupants(structure);
			
			counter++;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SetUpRoadBlocks()
	{
		array<EEditableEntityLabel> labels = {EEditableEntityLabel.SLOT_ROAD_SMALL, EEditableEntityLabel.SLOT_ROAD_MEDIUM, EEditableEntityLabel.SLOT_ROAD_LARGE};
		
		int count = Math.RandomIntInclusive(3, 5);
		int counter = 0;
		while (counter < count)
		{
			if (labels.IsEmpty())
				break;
			
			EEditableEntityLabel label = labels.GetRandomElement();
			array<ResourceName> entries = {};
			m_pFactionManager.GetFactionEntityListWithLabel(m_pFactionManager.GetEnemyFaction(), EEntityCatalogType.COMPOSITION, label, entries);
			
			if (entries.IsEmpty())
			{
				labels.RemoveItem(label);
				continue;
			}
			
			IEntity structure = SpawnInRandomRoadSlot(entries.GetRandomElement(), label);
			if (!structure)
			{
				labels.RemoveItem(label);
				continue;
			}
			
			m_aEnemyPositionsToReveal.Insert(structure.GetOrigin());
			AddEntity(structure);
			SpawnTurretOccupants(structure);
			counter++;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool SpawnTurretOccupants(IEntity entity)
	{
		array<IEntity> turrets = {};
		KSC_CompositionHelperT<Turret>.GetChildrenByType(entity, turrets);
		if (turrets.IsEmpty())
			return false;
		
		foreach (IEntity turret : turrets)
		{
			SCR_BaseCompartmentManagerComponent compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(turret.FindComponent(SCR_BaseCompartmentManagerComponent));
			if (!compartmentManager)
				continue;
			
			array<BaseCompartmentSlot> slots = {};
			compartmentManager.GetCompartmentsOfType(slots, ECompartmentType.TURRET);
			if (slots.IsEmpty())
				continue;
			
			array<ResourceName> entries = {};
			m_pFactionManager.GetFactionEntityListWithLabel(m_pFactionManager.GetEnemyFaction(), EEntityCatalogType.CHARACTER, EEditableEntityLabel.ROLE_RIFLEMAN, entries);
			if (entries.IsEmpty())
				continue;
			
			foreach (BaseCompartmentSlot slot : slots)
			{
				SCR_ChimeraCharacter char = KSC_GameTools.SpawnCharacterPrefab(entries.GetRandomElement(), turret.GetOrigin());
				AddEntity(char);
				AIWaypoint wp = KSC_GameTools.SpawnWaypointPrefab("{84FB298EA1F2C7CE}Prefabs/AI/Waypoints/KSC_AIWaypoint_Defend_0m.et", turret.GetOrigin());
				KSC_AIHelper.GetGroup(char).AddWaypoint(wp);
				AddEntity(wp);
				CompartmentAccessComponent compartmentAccess = char.GetCompartmentAccessComponent();
				GetGame().GetCallqueue().CallLater(compartmentAccess.GetInVehicle, 1000, false, turret, slot, true, -1, ECloseDoorAfterActions.CLOSE_DOOR, true);
			}
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool SpawnSmartInteractionOccupants(IEntity entity)
	{
		array<Managed> sentinels = {};
		KSC_CompositionHelper.GetChildComponentsByType(entity, SCR_AISmartActionSentinelComponent, sentinels);
		if (sentinels.IsEmpty())
			return false;

		array<ResourceName> entries = {};
		m_pFactionManager.GetFactionEntityListWithLabel(m_pFactionManager.GetEnemyFaction(), EEntityCatalogType.GROUP, EEditableEntityLabel.GROUPSIZE_SMALL, entries);
		if (entries.IsEmpty())
			return false;
		
		SCR_AIGroup group = SpawnGroupPrefab(entries.GetRandomElement(), entity.GetOrigin());
		if (!group)
			return false;
		
		KSC_AITasks.Defend(group, entity.GetOrigin(), 5);
		AddGroup(group);
		return true;	
	}
	
	//------------------------------------------------------------------------------------------------
	void SetUpDefenders()
	{
		array<ResourceName> entries = {};
		m_pFactionManager.GetFactionEntityListWithLabel(m_pFactionManager.GetEnemyFaction(), EEntityCatalogType.GROUP, EEditableEntityLabel.GROUPSIZE_MEDIUM, entries);
		m_pFactionManager.GetFactionEntityListWithLabel(m_pFactionManager.GetEnemyFaction(), EEntityCatalogType.GROUP, EEditableEntityLabel.GROUPSIZE_LARGE, entries);
		
		int aiCount = 0;
		int maxAICount = m_pGameMode.GetTargetEnemyAICount();
		
		foreach (vector posToDefend : m_aPositionsToDefend)
		{
			vector pos = posToDefend;
			SCR_WorldTools.FindEmptyTerrainPosition(pos, posToDefend, 10);
			SCR_AIGroup group = SpawnGroupPrefab(entries.GetRandomElement(), pos);
			if (!group)
				break;
			
			KSC_AITasks.Defend(group, posToDefend, 15); 
			AddGroup(group);
		}
		
		entries.Clear();
		m_pFactionManager.GetFactionEntityListWithLabel(m_pFactionManager.GetEnemyFaction(), EEntityCatalogType.GROUP, EEditableEntityLabel.ENTITYTYPE_GROUP, entries);
		
		// Put half of AI inside inner area
		while (m_iAiCount < m_iMaxAICount / 2)
		{
			SCR_AIGroup group = SpawnGroupPrefab(entries.GetRandomElement(), GetOrigin());
			if (!group)
				break;
			
			KSC_AITasks.Defend(group, m_Area);
			AddGroup(group);
			
			array<AIWaypoint> waypoints = {};
			group.GetWaypoints(waypoints);
			
			if (!waypoints.IsEmpty())
			{
				KSC_SpawnGroupHelperComponent spawnGroupHelper = KSC_SpawnGroupHelperComponent.Cast(group.FindComponent(KSC_SpawnGroupHelperComponent));
				spawnGroupHelper.SetCenter(waypoints[0].GetOrigin());
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SetUpPatrols()
	{
		array<ResourceName> entries = {};
		m_pFactionManager.GetFactionEntityListWithLabel(m_pFactionManager.GetEnemyFaction(), EEntityCatalogType.GROUP, EEditableEntityLabel.ENTITYTYPE_GROUP, entries);
		
		KSC_CircleArea patrolArea = KSC_CircleArea(GetOrigin(), m_pGameMode.GetAORadius());
		
		while (m_iAiCount < m_iMaxAICount)
		{
			SCR_AIGroup group = SpawnGroupPrefab(entries.GetRandomElement(), GetOrigin());
			if (!group)
				break;
			
			KSC_AITasks.Patrol(group, patrolArea);
			AddGroup(group);
			
			array<AIWaypoint> waypoints = {};
			group.GetWaypoints(waypoints);
			
			// Move group to position of last waypoint
			if (!waypoints.IsEmpty())
			{
				AIWaypointCycle wpCycle = AIWaypointCycle.Cast(waypoints[0]);
				if (wpCycle)
					wpCycle.GetWaypoints(waypoints);
				
				KSC_SpawnGroupHelperComponent spawnGroupHelper = KSC_SpawnGroupHelperComponent.Cast(group.FindComponent(KSC_SpawnGroupHelperComponent));
				spawnGroupHelper.SetCenter(waypoints[waypoints.Count() - 1].GetOrigin());
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SetUpArmedVehicles()
	{
		// Only include cars if no tanks or APCs are available
		array<ResourceName> entries = {};
		m_pFactionManager.GetFactionEntityListWithLabels(m_pFactionManager.GetEnemyFaction(), EEntityCatalogType.VEHICLE,
			includedLabels: {EEditableEntityLabel.TRAIT_ARMED},
			excludedLabels: {EEditableEntityLabel.VEHICLE_HELICOPTER, EEditableEntityLabel.VEHICLE_CAR},
		entries);
		
		if (entries.IsEmpty())
		{
			m_pFactionManager.GetFactionEntityListWithLabels(m_pFactionManager.GetEnemyFaction(), EEntityCatalogType.VEHICLE,
				includedLabels: {EEditableEntityLabel.TRAIT_ARMED},
				excludedLabels: {EEditableEntityLabel.VEHICLE_HELICOPTER},
			entries);
		}
		
		if (entries.IsEmpty())
			return;
		
		for (int i = 0; i < m_pGameMode.GetEnemyArmedVehicleCount(); i++)
		{
			vector pos;
			float rotation;
			if (!GetRandomParkingSlot(pos, rotation))
				return;
			
			IEntity vehicle = KSC_GameTools.SpawnVehiclePrefab(entries.GetRandomElement(), pos, rotation);
			if (!vehicle)
				return;
			
			AddEntity(vehicle);
			SpawnTurretOccupants(vehicle);
			m_aEnemyPositionsToReveal.Insert(pos);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SetUpCivilians()
	{
		array<ResourceName> entries = {};
		m_pFactionManager.GetFactionEntityListWithLabel(m_pFactionManager.GetCivilianFaction(), EEntityCatalogType.CHARACTER, EEditableEntityLabel.ENTITYTYPE_CHARACTER, entries);
		int count = Math.RandomIntInclusive(8, 12);
		int counter = 0;
		
		while (counter < count)
		{
			SCR_ChimeraCharacter char = KSC_GameTools.SpawnCharacterPrefab(entries.GetRandomElement(), GetOrigin());
			if (!char)
				break;
			
			AIGroup group = KSC_AIHelper.GetGroup(char);
			if (!group)
				break;
			
			KSC_AITasks.Patrol(group, m_Area);
			AddGroup(group);
			
			array<AIWaypoint> waypoints = {};
			group.GetWaypoints(waypoints);
			
			// Move character to position of last waypoint
			if (!waypoints.IsEmpty())
			{
				AIWaypointCycle wpCycle = AIWaypointCycle.Cast(waypoints[0]);
				if (wpCycle)
					wpCycle.GetWaypoints(waypoints);
				
				char.SetOrigin(waypoints[waypoints.Count() - 1].GetOrigin());
			}
			
			counter++;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SetUpCivlianVehicles()
	{
		array<ResourceName> entries = {};
		m_pFactionManager.GetFactionEntityListWithLabels(m_pFactionManager.GetCivilianFaction(), EEntityCatalogType.VEHICLE,
			includedLabels: {EEditableEntityLabel.ENTITYTYPE_VEHICLE},
			excludedLabels: {EEditableEntityLabel.VEHICLE_HELICOPTER},
		entries);
		
		if (entries.IsEmpty())
			return;
		
		int count = Math.RandomIntInclusive(3, 5);
		
		for (int i = 0; i < count; i++)
		{
			vector pos;
			float rotation;
			if (!GetRandomParkingSlot(pos, rotation))
				return;
			
			IEntity vehicle = KSC_GameTools.SpawnVehiclePrefab(entries.GetRandomElement(), pos, rotation);
			if (!vehicle)
				return;
			
			AddEntity(vehicle);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetRandomParkingSlot(out vector pos, out float rotation, bool blockSlots = true)
	{
		array<EEditableEntityLabel> labels = {EEditableEntityLabel.SLOT_ROAD_SMALL, EEditableEntityLabel.SLOT_ROAD_MEDIUM, EEditableEntityLabel.SLOT_ROAD_LARGE};
		
		int attempt = 0;
		while (attempt < MAX_ATTEMPTS)
		{
			if (labels.IsEmpty())
				return false;
			
			EEditableEntityLabel label = labels.GetRandomElement();
			if (!GetRandomTerrainSlot(pos, label, blockSlots))
			{
				labels.RemoveItem(label);
				continue;
			}
			
			rotation = pos[1] + 180 * Math.RandomIntInclusive(0, 1);
			pos[1] = SCR_TerrainHelper.GetTerrainY(pos);
			
			float radius = 1.5;
			
			if (label == EEditableEntityLabel.SLOT_ROAD_LARGE)
			{
				radius = 3.5;
			}
			else if (label == EEditableEntityLabel.SLOT_ROAD_MEDIUM)
			{
				radius = 2.5;
			}
			
			pos += radius * Vector(Math.Cos(Math.DEG2RAD * rotation), 0, -Math.Sin(Math.DEG2RAD * rotation));
			return true;
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_AIGroup SpawnGroupPrefab(ResourceName prefabName, vector pos)
	{
		if (m_iAiCount > m_iMaxAICount)
			return null;
		
		SCR_AIGroup group = SCR_AIGroup.Cast(KSC_GameTools.SpawnGroupPrefab(prefabName, pos));
		BaseContainer container = group.GetPrefabData().GetPrefab();
		array<ResourceName> units;
		container.Get("m_aUnitPrefabSlots", units);			
		m_iAiCount += units.Count();
		return group;
	}
	
	//------------------------------------------------------------------------------------------------
	KSC_BaseTask GetNearestTask(vector pos)
	{
		float minDist = float.INFINITY;
		KSC_BaseTask nearestTask;
		
		foreach (KSC_BaseTask task : m_aTasks)
		{
			float dist = vector.DistanceXZ(pos, task.GetOrigin());
			
			if (dist < minDist)
			{
				nearestTask = task;
				minDist = dist;
			}
		}
		
		return nearestTask;
	}
	
	//------------------------------------------------------------------------------------------------
	void AddPositionToDefend(vector pos)
	{
		m_aPositionsToDefend.Insert(pos);
	}
	
	//------------------------------------------------------------------------------------------------
	void AddGroup(AIGroup group)
	{
		array<AIWaypoint> waypoints = {};
		group.GetWaypoints(waypoints);
				
		foreach (AIWaypoint waypoint : waypoints)
		{
			AddEntity(waypoint);
		}
		
		m_aEntities.Insert(group);
	}
	
	//------------------------------------------------------------------------------------------------
	void AddEntity(IEntity entity)
	{
		m_aEntities.Insert(entity);
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity SpawnInRandomFlatSlot(ResourceName prefabName, EEditableEntityLabel slotLabel = EEditableEntityLabel.SLOT_FLAT_SMALL, bool useNearestTaskDir = true)
	{
		vector pos;
		if (!GetRandomTerrainSlot(pos, slotLabel))
			return null;
		
		float dir;
		
		if (useNearestTaskDir)
		{
			KSC_BaseTask nearestTask = GetNearestTask(pos);
			if (!nearestTask)
				return null;
			
			float refDir = KSC_NumTools.WrapAngle((pos - nearestTask.GetOrigin()).ToYaw());
			float minDiff = float.INFINITY;
			
			for (int i = 0; i < 4; i++)
			{
				float otherDir = KSC_NumTools.WrapAngle(pos[1] + 90 * i);
				float diff = Math.AbsFloat(otherDir - refDir);
				
				if (diff < minDiff)
				{
					dir = otherDir;
					minDiff = diff;
				}
			}
		}
		else
		{
			dir = pos[1] + 90 * Math.RandomInt(0, 4);
		}
		 
		pos[1] = SCR_TerrainHelper.GetTerrainY(pos);
		IEntity structure = KSC_GameTools.SpawnStructurePrefab(prefabName, pos, dir);
		
		float radius;
		s_mSlotRadii.Find(slotLabel, radius);
		array<IEntity> trees = {};
		KSC_WorldTools.GetEntitiesByType(trees, Tree, KSC_CircleArea(pos, radius));
		
		if (!trees.IsEmpty())
		{
			array<EntityID> treeIDs = {};
			treeIDs.Reserve(trees.Count());
			foreach (IEntity tree : trees)
			{
				treeIDs.Insert(tree.GetID());
			}
		
			ACE_LoadtimeEntityManager entityManager = ACE_LoadtimeEntityManager.GetInstance();
			if (entityManager)
				entityManager.DeleteEntitiesByIdGlobal(treeIDs);	
		}
		
		AddEntity(structure);
		return structure;
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity SpawnInRandomRoadSlot(ResourceName prefabName, EEditableEntityLabel slotLabel = EEditableEntityLabel.SLOT_ROAD_MEDIUM, bool useNearestTaskDir = true)
	{
		vector pos;
		if (!GetRandomTerrainSlot(pos, slotLabel))
			return null;
		
		float dir;
		
		if (useNearestTaskDir)
		{
			KSC_BaseTask nearestTask = GetNearestTask(pos);
			if (!nearestTask)
				return null;
			
			float refDir = KSC_NumTools.WrapAngle((pos - nearestTask.GetOrigin()).ToYaw());
			float minDiff = float.INFINITY;
			
			for (int i = 0; i < 2; i++)
			{
				float otherDir = KSC_NumTools.WrapAngle(pos[1] + 180 * i);
				float diff = Math.AbsFloat(otherDir - refDir);
				
				if (diff < minDiff)
				{
					dir = otherDir;
					minDiff = diff;
				}
			}
		}
		else
		{
			dir = pos[1] + 180 * Math.RandomInt(0, 2);
		}
		
		pos[1] = SCR_TerrainHelper.GetTerrainY(pos);
		IEntity structure = KSC_GameTools.SpawnStructurePrefab(prefabName, pos, dir);
		AddEntity(structure);
		return structure;
	}
	
	//------------------------------------------------------------------------------------------------
	bool GetRandomTerrainSlot(out vector slot, EEditableEntityLabel slotLabel, bool blockSlots = true)
	{
		array<vector> slots = {};
		if (!m_mTerrainSlots.Find(slotLabel, slots))
			return false;
		
		if (slots.IsEmpty())
			return false;
		
		slot = slots.GetRandomElement();
		
		if (blockSlots)
		{
			float radius;
			s_mSlotRadii.Find(slotLabel, radius);
			radius *= 2;
			
			foreach (array<vector> slots2 :  m_mTerrainSlots)
			{
				for (int i = slots2.Count() - 1; i >= 0; i--)
				{
					if (vector.DistanceXZ(slot, slots2[i]) < radius)
						slots2.Remove(i);
				}
			}
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnTaskStateChanged(SCR_BaseTask task, SCR_TaskState previousState, SCR_TaskState newState)
	{
		if (newState != SCR_TaskState.FINISHED && newState != SCR_TaskState.CANCELLED)
			return;
		
		m_iNumCompletedTasks++;
		
		// Reveal targets if intel was picked up
		if (KSC_PickUpItemTask.Cast(task))
			RevealEnemyPositions();			
		
		if (m_iNumTasks > m_iNumCompletedTasks)
			return;
		
		KSC_LeaveAreaTaskSupportEntity supportEntity = KSC_LeaveAreaTaskSupportEntity.Cast(GetTaskManager().FindSupportEntity(KSC_LeaveAreaTaskSupportEntity));
		KSC_BaseTask exfilTask = KSC_BaseTask.Cast(supportEntity.CreateTask(m_pFactionManager.GetPlayerFaction(), GetOrigin(), m_pGameMode.GetAORadius()));
		exfilTask.GetOnStateChanged().Insert(OnExfilStateChanged);
		m_aTasks.Insert(exfilTask);
		
		for (int i = m_aEntities.Count() - 1; i >= 0; i--)
		{
			if (!AIWaypoint.Cast(m_aEntities[i]))
				continue;
			
			SCR_EntityHelper.DeleteEntityAndChildren(m_aEntities[i]);
			m_aEntities.Remove(i);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void OnExfilStateChanged(SCR_BaseTask task, SCR_TaskState previousState, SCR_TaskState newState)
	{
		if (newState != SCR_TaskState.FINISHED)
			return;
		
		m_pGameMode.ExecuteCommanderRequest(COE_ECommanderRequest.CANCEL_AO);
	}
	
	//------------------------------------------------------------------------------------------------
	void AddEnemyPositionToReveal(vector pos)
	{
		m_aEnemyPositionsToReveal.Insert(pos);
	}
	
	//------------------------------------------------------------------------------------------------
	void RevealEnemyPositions()
	{
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		if (!markerManager)
			return;
		
		foreach (vector pos : m_aEnemyPositionsToReveal)
		{
			SCR_MapMarkerBase marker = new SCR_MapMarkerBase();
			marker.SetType(SCR_EMapMarkerType.PLACED_CUSTOM);
			marker.SetIconEntry(SCR_EScenarioFrameworkMarkerCustom.MARK_QUESTION);
			marker.SetColorEntry(m_pFactionManager.GetMarkerCustomColorEntry(m_pFactionManager.GetEnemyFaction()));
			marker.SetWorldPos(pos[0], pos[2]);
			markerManager.InsertStaticMarker(marker, false, true);
			m_aMarkers.Insert(marker);
		}
	}
		
	//------------------------------------------------------------------------------------------------
	void ~COE_AO()
	{
		if (!Replication.IsServer())
			return;
		
		foreach (KSC_BaseTask task : m_aTasks)
		{
			if (!task)
				continue;
			
			KSC_BaseTaskSupportEntity supportEntity = task.GetSupportEntity();
			supportEntity.CancelTask(task.GetTaskID());
		}
		
		foreach (IEntity entity : m_aEntities)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(entity);
		}
		
		SCR_MapMarkerManagerComponent markerManager = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		if (!markerManager)
			return;
				
		foreach (SCR_MapMarkerBase marker : m_aMarkers)
		{
			markerManager.RemoveStaticMarker(marker);
		}
	}
}

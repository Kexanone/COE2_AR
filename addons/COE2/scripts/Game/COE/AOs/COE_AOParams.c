//------------------------------------------------------------------------------------------------
class COE_AOParams : Managed
{
	protected int m_iLocationIdx = -1;
	protected ref array<int> m_aTaskBuilderIds = {};
	
	//------------------------------------------------------------------------------------------------
	void SetLocation(KSC_Location location)
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return;
		
		array<ref KSC_Location> locations = gameMode.GetAvailableLocations();
		if (!locations)
			return;
		
		m_iLocationIdx = locations.Find(location);
	}
	
	//------------------------------------------------------------------------------------------------
	KSC_Location GetLocation()
	{
		if (m_iLocationIdx < 0)
			return null;
		
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return null;
		
		array<ref KSC_Location> locations = gameMode.GetAvailableLocations();
		if (!locations)
			return null;
		
		return locations[m_iLocationIdx];
	}
	
	//------------------------------------------------------------------------------------------------
	void SetTaskBuilders(array<COE_BaseTaskBuilder> taskBuilders)
	{
		m_aTaskBuilderIds.Reserve(taskBuilders.Count());
		
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return;
		
		array<ref COE_BaseTaskBuilder> allTaskBuilders = gameMode.GetAvailableTaskBuilders();
		if (!allTaskBuilders)
			return;
		
		foreach (COE_BaseTaskBuilder taskBuilder : taskBuilders)
		{
			int builderId = allTaskBuilders.Find(taskBuilder);
			if (builderId >= 0)
				m_aTaskBuilderIds.Insert(builderId);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	array<COE_BaseTaskBuilder> GetTaskBuilders()
	{
		array<COE_BaseTaskBuilder> filteredTaskBuilders = {};
		filteredTaskBuilders.Reserve(m_aTaskBuilderIds.Count());
		
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		if (!gameMode)
			return filteredTaskBuilders;
		
		array<ref COE_BaseTaskBuilder> allTaskBuilders = gameMode.GetAvailableTaskBuilders();
		if (!allTaskBuilders)
			return filteredTaskBuilders;
		
		foreach (int builderId : m_aTaskBuilderIds)
		{
			filteredTaskBuilders.Insert(allTaskBuilders[builderId]);
		}
		
		return filteredTaskBuilders;
	}
		
	//------------------------------------------------------------------------------------------------
	void Copy(COE_AOParams from)
	{
		m_iLocationIdx = from.m_iLocationIdx;
		m_aTaskBuilderIds.Copy(from.m_aTaskBuilderIds);
	}
	
	//------------------------------------------------------------------------------------------------
	void Clear()
	{
		m_iLocationIdx = -1;
		m_aTaskBuilderIds.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(COE_AOParams instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		snapshot.SerializeInt(instance.m_iLocationIdx);
		int builderCount = instance.m_aTaskBuilderIds.Count();
		snapshot.SerializeInt(builderCount);
		
		foreach (int builderId : instance.m_aTaskBuilderIds)
		{
			snapshot.SerializeInt(builderId);
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, COE_AOParams instance)
	{
		snapshot.SerializeInt(instance.m_iLocationIdx);
		int builderCount;
		snapshot.SerializeInt(builderCount);
		instance.m_aTaskBuilderIds.Reserve(builderCount);
		
		for (int i; i < builderCount; ++i)
		{
			int builderId;
			snapshot.SerializeInt(builderId);
			instance.m_aTaskBuilderIds.Insert(builderId);
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet)
	{
		// m_iLocationIdx
		snapshot.EncodeInt(packet);
		
		int builderCount;
		snapshot.SerializeBytes(builderCount, 4);
		packet.Serialize(builderCount, 32);
		
		// m_aTaskBuilderIds
		snapshot.Serialize(packet, 4 * builderCount);
	}

	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		// m_iLocationIdx
		snapshot.DecodeInt(packet);
		
		int builderCount;
		packet.Serialize(builderCount, 32);
		snapshot.SerializeBytes(builderCount, 4);
		
		// m_aTaskBuilderIds
		snapshot.Serialize(packet, 4 * builderCount);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs , ScriptCtx ctx)
	{
		// m_iLocationIdx
		if (!lhs.CompareSnapshots(rhs, 4))
			return false;
		
		int lhsbuilderCount;
		lhs.SerializeInt(lhsbuilderCount);

		int rhsbuilderCount;
		rhs.SerializeInt(rhsbuilderCount);

		if (lhsbuilderCount != rhsbuilderCount)
			return false;
		
		// m_aTaskBuilderIds
		return lhs.CompareSnapshots(rhs, 4 * lhsbuilderCount);
	}

	//------------------------------------------------------------------------------------------------
	static bool PropCompare(COE_AOParams instance, SSnapSerializerBase snapshot, ScriptCtx ctx)
	{
		if (!snapshot.CompareInt(instance.m_iLocationIdx))
			return false;
		
		if (!snapshot.CompareInt(instance.m_aTaskBuilderIds.Count()))
			return false;
		
		foreach (int builderId : instance.m_aTaskBuilderIds)
		{
			if (!snapshot.CompareInt(builderId))
				return false;
		}
		
		return true;
	}
}

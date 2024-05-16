//------------------------------------------------------------------------------------------------
class COE_AOParams : Managed
{
	int m_eTaskTypes = 0; // bit flags for tasks
	int m_iLocationIdx = -1;
	private const int SERIALIZED_BYTES = 8;
	
	//------------------------------------------------------------------------------------------------
	void Copy(COE_AOParams from)
	{
		m_eTaskTypes = from.m_eTaskTypes;
		m_iLocationIdx = from.m_iLocationIdx;
	}
	
	//------------------------------------------------------------------------------------------------
	void Clear()
	{
		m_eTaskTypes = 0;
		m_iLocationIdx = -1;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(COE_AOParams instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		snapshot.SerializeInt(instance.m_eTaskTypes);
		snapshot.SerializeInt(instance.m_iLocationIdx);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, COE_AOParams instance)
	{
		snapshot.SerializeInt(instance.m_eTaskTypes);
		snapshot.SerializeInt(instance.m_iLocationIdx);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet)
	{
		snapshot.Serialize(packet, SERIALIZED_BYTES);
	}

	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		return snapshot.Serialize(packet, SERIALIZED_BYTES);
	}

	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs , ScriptCtx ctx)
	{
		return lhs.CompareSnapshots(rhs, SERIALIZED_BYTES) && lhs.CompareStringSnapshots(rhs);
	}

	//------------------------------------------------------------------------------------------------
	static bool PropCompare(COE_AOParams instance, SSnapSerializerBase snapshot, ScriptCtx ctx)
	{
		return snapshot.CompareInt(instance.m_eTaskTypes)
			&& snapshot.CompareInt(instance.m_iLocationIdx);
	}
}
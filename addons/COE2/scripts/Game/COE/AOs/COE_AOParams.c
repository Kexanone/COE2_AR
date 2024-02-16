//------------------------------------------------------------------------------------------------
class COE_AOParams : Managed
{
	int m_eTaskTypes; // bit flags for tasks
	IEntity m_pLocation;
	private const int SERIALIZED_BYTES = 16;
	
	//------------------------------------------------------------------------------------------------
	void Copy(COE_AOParams from)
	{
		m_eTaskTypes = from.m_eTaskTypes;
		m_pLocation = from.m_pLocation;
	}
	
	//------------------------------------------------------------------------------------------------
	void Clear()
	{
		m_eTaskTypes = 0;
		m_pLocation = null;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(COE_AOParams instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		snapshot.SerializeInt(instance.m_eTaskTypes);
		
		vector pos;
		if (instance.m_pLocation)
			pos = instance.m_pLocation.GetOrigin();
		
		snapshot.SerializeVector(pos);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, COE_AOParams instance)
	{
		snapshot.SerializeInt(instance.m_eTaskTypes);
		vector pos;
		snapshot.SerializeVector(pos);
		if (!pos)
		{
			instance.m_pLocation = null;
		}
		else
		{
			instance.m_pLocation = KSC_WorldTools.GetNearestEntityByType(pos);
		};
		
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
		vector pos;
		if (instance.m_pLocation)
			pos = instance.m_pLocation.GetOrigin();
		
		return snapshot.CompareInt(instance.m_eTaskTypes)
			&& snapshot.CompareVector(pos);
	}
}
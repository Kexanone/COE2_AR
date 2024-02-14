//------------------------------------------------------------------------------------------------
class COE_AOParams : Managed
{
	KSC_ETaskType m_eTaskType;
	IEntity m_pLocation;
	private const int SERIALIZED_BYTES = 16;
	
	//------------------------------------------------------------------------------------------------
	COE_AOParams Copy()
	{
		COE_AOParams copy = new COE_AOParams();
		copy.m_eTaskType = m_eTaskType;
		copy.m_pLocation = m_pLocation;
		return copy;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(COE_AOParams instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		snapshot.SerializeInt(instance.m_eTaskType);
		vector pos = instance.m_pLocation.GetOrigin();
		snapshot.SerializeVector(pos);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, COE_AOParams instance)
	{
		snapshot.SerializeInt(instance.m_eTaskType);
		vector pos;
		snapshot.SerializeVector(pos);
		instance.m_pLocation = KSC_WorldTools.GetNearestEntityByType(pos);
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
		return snapshot.CompareInt(instance.m_eTaskType)
			&& snapshot.CompareVector(instance.m_pLocation.GetOrigin());
	}
}
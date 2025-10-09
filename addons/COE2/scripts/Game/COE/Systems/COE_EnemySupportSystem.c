//------------------------------------------------------------------------------------------------
class COE_EnemySupportSystem : GameSystem
{
	//protected ref array<COE_AO> m_aRegisteredAOs = {};
	protected COE_AO m_pRegisteredAO = null;
	protected float m_fReinforcementTimeout;
	protected float m_fReinforcementTimer;
	protected float m_fPlayerCheckTimeout = 1;
	protected float m_fPlayerCheckTimer = 0;
	protected bool m_bPlayerLocated = false;
	
	//------------------------------------------------------------------------------------------------
	static COE_EnemySupportSystem GetInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return COE_EnemySupportSystem.Cast(world.FindSystem(COE_EnemySupportSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Server)
			.AddPoint(WorldSystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePaused()
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		super.OnInit();
		Enable(false);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnUpdate(ESystemPoint point)
	{
		super.OnUpdate(point);
		
		if (!m_pRegisteredAO)
			return;
		
		if (!m_bPlayerLocated)
		{
			m_fPlayerCheckTimer += GetWorld().GetTimeSlice();
			
			if (m_fPlayerCheckTimer < m_fPlayerCheckTimeout)
				return;
			
			m_fPlayerCheckTimer = 0;
			
			foreach (AIGroup group : m_pRegisteredAO.GetAllGroups())
			{
				if (!group)
					return;
				
				SCR_AIGroupUtilityComponent utility = SCR_AIGroupUtilityComponent.Cast(group.FindComponent(SCR_AIGroupUtilityComponent));
				if (!utility || !utility.m_Perception)
					continue;
				
				if (!utility.m_Perception.m_aTargets || utility.m_Perception.m_aTargets.IsEmpty())
					continue;
				
				m_bPlayerLocated = true;
				return;
			}
		}
		else
		{
			m_fReinforcementTimer += GetWorld().GetTimeSlice();
			
			if (m_fReinforcementTimer < m_fReinforcementTimeout)
				return;
			
			m_fReinforcementTimer = 0;
			
			COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
			if (!factionManager)
				return;
			
			KSC_ParadropSystem system = KSC_ParadropSystem.GetInstance();
			if (!system)
				return;
			
			vector transform[4];
			vector pos = m_pRegisteredAO.GetOrigin();
			pos[1] = SCR_TerrainHelper.GetTerrainY(pos);
			float angle = Math.RandomFloat(0, 360);
			KSC_GameTools.GetTransformFromPosAndRot(transform, pos, angle);
			
			// Spawn two helos
			int count = SampleHeloCount();
			for (int i = 0; i < count; ++i)
			{
				Vehicle helo;
				AIGroup crewGroup;
				AIGroup passengerGroup;
				system.ScheduleParadrop(factionManager.GetEnemyFaction(), transform, helo, crewGroup, passengerGroup);
				m_pRegisteredAO.AddEntity(helo);
				m_pRegisteredAO.AddEntity(crewGroup);
				m_pRegisteredAO.AddEntity(passengerGroup);
				transform[3] = transform[3] + Math.RandomFloatInclusive(40, 60) * vector.FromYaw(Math.RandomFloatInclusive(-60, 60) - angle);
			}
			
			Enable(false);
			m_pRegisteredAO = null;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns target AI count divided by 12, rounded to int by probability of the fractional part
	protected int SampleHeloCount()
	{
		float count = COE_GameMode.GetInstance().GetTargetEnemyAICount() / 12;
	    int floorCount = Math.Floor(count);
	
	    if (Math.RandomFloat(0, 1) < count - floorCount)
	        return floorCount + 1;
	    else
	        return floorCount;
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(COE_AO ao)
	{
		COE_GameMode gameMode = COE_GameMode.GetInstance();
		m_fReinforcementTimeout = 60 * Math.RandomFloatInclusive(gameMode.GetMinEnemyReinforcementTime(), gameMode.GetMaxEnemyReinforcementTime());
		m_fReinforcementTimer = 0;
		m_fPlayerCheckTimer = 0;
		m_bPlayerLocated = false;
		m_pRegisteredAO = ao;
		//m_aRegisteredAOs.Insert(ao);
		Enable(true);
	}	
	//------------------------------------------------------------------------------------------------
	void Unregister(COE_AO ao)
	{
		//m_aRegisteredAOs.RemoveItem(ao);
		//if (m_aRegisteredAOs.IsEmpty())
		Enable(false);
		m_pRegisteredAO = null;
	}
}

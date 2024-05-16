//------------------------------------------------------------------------------------------------
class COE_AOClass : KSC_AOClass
{
}

//------------------------------------------------------------------------------------------------
class COE_AO : KSC_AO
{
	protected ref array<IEntity> m_aEntities = {};
	protected ref array<KSC_BaseTask> m_aTasks = {};
	protected COE_GameMode m_pGameMode;
	protected COE_FactionManager m_pFactionManager;
	protected COE_AOParams m_Params;
	protected ref KSC_AreaBase m_Area;
	protected int m_iNumTasks = 0;
	protected int m_iNumCompletedTasks = 0;
		
	//------------------------------------------------------------------------------------------------
	void COE_AO(IEntitySource src, IEntity parent)
	{
		if (Replication.IsRunning() && !Replication.IsServer())
			return;
		
		GetGame().GetCallqueue().CallLater(DelayedInit, 100);
	}
	
	//------------------------------------------------------------------------------------------------
	void DelayedInit()
	{
		m_pGameMode = COE_GameMode.GetInstance();
		m_pFactionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		m_Params = m_pGameMode.GetCurrentAOParams();
		m_Area = KSC_CircleArea(GetOrigin(), m_pGameMode.GetAORadius());
		SetUpTasks();
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
	void AddEntity(IEntity entity)
	{
		m_aEntities.Insert(entity);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnTaskStateChanged(SCR_BaseTask task, SCR_TaskState previousState, SCR_TaskState newState)
	{
		if (newState != SCR_TaskState.FINISHED)
			return;
		
		m_iNumCompletedTasks++;
		
		if (m_iNumTasks > m_iNumCompletedTasks)
			return;
		
		KSC_LeaveAreaTaskSupportEntity supportEntity = KSC_LeaveAreaTaskSupportEntity.Cast(GetTaskManager().FindSupportEntity(KSC_LeaveAreaTaskSupportEntity));
		KSC_BaseTask exfilTask = KSC_BaseTask.Cast(supportEntity.CreateTask(m_pFactionManager.GetPlayerFaction(), GetOrigin(), m_pGameMode.GetAORadius()));
		exfilTask.GetOnStateChanged().Insert(OnExfilStateChanged);
		m_aTasks.Insert(exfilTask);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnExfilStateChanged(SCR_BaseTask task, SCR_TaskState previousState, SCR_TaskState newState)
	{
		if (newState != SCR_TaskState.FINISHED)
			return;
		
		m_pGameMode.ExecuteCommanderRequest(COE_ECommanderRequest.CANCEL_AO);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~COE_AO()
	{
		if (Replication.IsRunning() && !Replication.IsServer())
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
	}
}

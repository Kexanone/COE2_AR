//------------------------------------------------------------------------------
class COE_MapObjectiveAreaMarkersUI : SCR_MapUIBaseComponent
{
	[Attribute(defvalue: "0.5", uiwidget: UIWidgets.Slider, params: "0 1", desc: "Alpha value of the area marker color")]
	protected float m_fColorAlpha;
		
	[Attribute(defvalue: "32", params: "3 inf", desc: "Number of verices a circle should be approximated with")]
	protected int m_iCircleVertexCount;
	
	protected CanvasWidget m_wCanvas;
	protected COE_GameMode m_pGameMode;
	protected ref array<ref CanvasWidgetCommand> m_aDrawCommands = {};
	protected int m_iColor;
			
	//------------------------------------------------------------------------------------------------
	override void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);
		
		m_wCanvas = CanvasWidget.Cast(config.RootWidgetRef.FindAnyWidget("COE_Canvas"));
		m_pGameMode = COE_GameMode.GetInstance();
		
		COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return;
		
		Color color = factionManager.GetEnemyFaction().GetFactionColor();
		color.SetA(m_fColorAlpha);
		m_iColor = color.PackToInt();
	}			
	
	//------------------------------------------------------------------------------------------------
	override void Update(float timeSlice)
	{
		super.Update(timeSlice);
		
		array<ref COE_AOParams> aoList = m_pGameMode.GetNextAOParams();
		if (!aoList)
			return;
		
		m_aDrawCommands.Clear();
		m_aDrawCommands.Reserve(aoList.Count());
		
		foreach (int i, COE_AOParams ao : aoList)
		{
			if (!ao)
				continue;
			
			KSC_Location location = ao.GetLocation();
			if (!location)
				continue;
			
			PolygonDrawCommand command = new PolygonDrawCommand();
			command.m_iColor = m_iColor;
			
			int x, y;
			m_MapEntity.WorldToScreen(location.m_vCenter[0], location.m_vCenter[2], x, y, true);
			float radius = m_pGameMode.GetAORadius() * m_MapEntity.GetCurrentZoom();
			command.m_Vertices = {};
			
			for (float angle = 0; angle < Math.PI2; angle += Math.PI2 / m_iCircleVertexCount)
			{
				command.m_Vertices.Insert(x + radius * Math.Cos(angle));
				command.m_Vertices.Insert(y + radius * Math.Sin(angle));
			}
			
			m_aDrawCommands.Insert(command);
		}
		
		m_wCanvas.SetDrawCommands(m_aDrawCommands);
	}
}

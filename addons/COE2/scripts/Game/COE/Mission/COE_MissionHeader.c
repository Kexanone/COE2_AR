class COE_MissionHeader : SCR_MissionHeader
{
	[Attribute(defvalue: "3", desc: "Total number of objectives to be generated")]
	int m_iCOE_NumberOfObjectives;
	
	[Attribute(defvalue: "3", desc: "Number of AI groups to be generated for an objectives")]
	int m_iCOE_NumberOfGroupsPerObjective;
	
	[Attribute(defvalue: "3", desc: "Maximum number of roadblocks for an objectives")]
	int m_iCOE_MaxNumberOfRoadblocks;
	
	[Attribute(defvalue: "2", desc: "Minimum number of roadblocks for an objectives")]
	int m_iCOE_MinNumberOfRoadblocks;
	
	[Attribute(defvalue: "1", desc: "Maximum number of APCs for a clear task")]
	int m_iCOE_MaxNumberOfAPCs;
	
	[Attribute(defvalue: "0", desc: "Minimum number of APCs for a clear task")]
	int m_iCOE_MinNumberOfAPCs;
}

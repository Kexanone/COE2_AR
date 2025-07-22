//------------------------------------------------------------------------------------------------
class COE_MissionHeader : SCR_MissionHeader
{
	[Attribute(defvalue: "", desc: "Default faction for the players. None if empty")]
	string m_sCOE_DefaultPlayerFactionKey;
	
	[Attribute(defvalue: "", desc: "Default faction for the enemy AI. None if empty")]
	string m_sCOE_DefaultEnemyFactionKey;
	
	[Attribute(defvalue: "", desc: "Default faction for the civilian AI. None if empty")]
	string m_sCOE_DefaultCivilianFactionKey;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(EAISkill), desc: "Default skill for enemy AI.")]
	EAISkill m_eCOE_DefaultEnemyAiSkill;
	
	[Attribute(defvalue: "-1", desc: "Default radius of the AI in meters.")]
	float m_fCOE_DefaultAORadius;
	
	[Attribute(defvalue: "-1", desc: "Default minimum enemy AI count for an AO.")]
	int m_iCOE_DefaultMinEnemyAICount;
	
	[Attribute(defvalue: "-1", desc: "Default total enemy AI count for an AO will be this multiplier times the total player count (ignored when below minimum count).")]
	float m_fCOE_DefaultEnemyAICountMultiplier;
	
	[Attribute(defvalue: "0", desc: "Default number of enemy armed vehicles for an AO.")]
	int m_iCOE_EnemyArmedVehicleCount;
	
	[Attribute(defvalue: "0", desc: "Number of enemy mortars.")]
	int m_iCOE_EnemyMortarCount;
	
	[Attribute(defvalue: "true", desc: "Whether civilians spawn on the AOs")]
	bool m_bCOE_CiviliansEnabled;
	
	[Attribute(defvalue: "false", desc: "Whether a voted in commander also becomes GM")]
	bool m_bCOE_CommanderBecomesGM;
}

modded class SCR_FactionAffiliationComponent : FactionAffiliationComponent
{
	[Attribute(defvalue: "false", desc: "If eneabled then the associated vanilla faction is used for custom factions")]
	protected bool m_bCOE_VanillaFactionOnly;
	
	//------------------------------------------------------------------------------------------------
	//! Add option to
	override static void SetFaction(IEntity owner, Faction faction)
	{
		if (!owner || !faction )
			return;
		
		SCR_FactionAffiliationComponent factionComponent = SCR_FactionAffiliationComponent.Cast(owner.FindComponent(SCR_FactionAffiliationComponent));
		if (!factionComponent)
			return;
		
		if (factionComponent.m_bCOE_VanillaFactionOnly)
		{
			COE_FactionManager factionManager = COE_FactionManager.Cast(GetGame().GetFactionManager());
			if (!factionManager)
				return;
			
			faction = factionManager.GetAssociatedVanillaFaction(faction);
		}
		
		factionComponent.SetAffiliatedFaction(faction);
	}
}

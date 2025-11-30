//------------------------------------------------------------------------------------------------
//! Add invoker for preforming category entries
[BaseContainerProps(configRoot: true)]
modded class SCR_SelectionMenu
{
	protected ref ScriptInvoker<SCR_SelectionMenu, SCR_SelectionMenuCategoryEntry> m_COE_OnPerformCategory;
	
	//------------------------------------------------------------------------------------------------
	override protected void OpenCategoryEntry(notnull SCR_SelectionMenuCategoryEntry category)
	{
		if (m_COE_OnPerformCategory)
			m_COE_OnPerformCategory.Invoke(this, category);
		
		super.OpenCategoryEntry(category);
	}
	
	//------------------------------------------------------------------------------------------------
	ScriptInvoker<SCR_SelectionMenu, SCR_SelectionMenuCategoryEntry> COE_GetOnPerformCategory()
	{
		if (!m_COE_OnPerformCategory)
			m_COE_OnPerformCategory = new ScriptInvoker<SCR_SelectionMenu, SCR_SelectionMenuCategoryEntry>();
		
		return m_COE_OnPerformCategory;
	}
}

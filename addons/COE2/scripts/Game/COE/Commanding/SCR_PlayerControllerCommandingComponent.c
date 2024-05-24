//------------------------------------------------------------------------------------------------
modded class SCR_PlayerControllerCommandingComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	//! Copy of vanilla AddElementsFromCategory, but setting name and icon
	override void AddElementsFromCategory(SCR_PlayerCommandingMenuCategoryElement category, SCR_SelectionMenuCategoryEntry rootCategory = null)
	{
		array<ref SCR_PlayerCommandingMenuBaseElement> elements = category.GetCategoryElements();

		SCR_PlayerCommandingMenuCategoryElement elementCategory;
		SCR_SelectionMenuCategoryEntry createdCategory;

		foreach (SCR_PlayerCommandingMenuBaseElement element : elements)
		{
			elementCategory = SCR_PlayerCommandingMenuCategoryElement.Cast(element);
			if (elementCategory)
			{
				createdCategory = SCR_SelectionMenuCategoryEntry.Cast(AddRadialMenuElement(elementCategory, rootCategory));
				if (!createdCategory)
					continue;

				/// START COE OVERRIDE
				createdCategory.SetIcon(category.GetIconImageset(), category.GetIconName());
				/// END COE OVERRIDE
				AddElementsFromCategory(elementCategory, createdCategory);
			}
			else
			{
				AddRadialMenuElement(element, rootCategory);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Copy of vanilla AddElementsFromCategoryToMap, but setting name and icon
	override void AddElementsFromCategoryToMap(notnull SCR_PlayerCommandingMenuCategoryElement category, SCR_SelectionMenuCategoryEntry rootCategory = null)
	{
		array<ref SCR_PlayerCommandingMenuBaseElement> elements = category.GetCategoryElements();
		SCR_SelectionMenuCategoryEntry mapEntryCategory = m_MapContextualMenu.AddRadialCategory(category.GetCategoryDisplayText()); // add map category entry
		/// START COE OVERRIDE
		mapEntryCategory.SetIcon(category.GetIconImageset(), category.GetIconName());
		/// END COE OVERRIDE
		
		SCR_PlayerCommandingMenuCategoryElement elementCategory;
		foreach (SCR_PlayerCommandingMenuBaseElement element : elements)
		{
			elementCategory = SCR_PlayerCommandingMenuCategoryElement.Cast(element);
			if (elementCategory)
			{
				if (elementCategory.GetCanShowOnMap())
					AddElementsFromCategoryToMap(elementCategory, mapEntryCategory);
			}
			else
			{
				InsertElementToMapRadial(element, category, mapEntryCategory);
			}
		}
	}
}
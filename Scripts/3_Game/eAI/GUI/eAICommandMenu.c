enum eAICommandCategories
{
	CATEGORIES,	//special category selection
	CAT_MOVEMENT,
	CAT_FORMATION,
	CAT_STATUS,
	CAT_DEBUG
}

class eAICommandMenuItem
{
	protected int m_ID;
	protected string m_Name;
	protected eAICommandCategories m_Category;
	//radial menu
	protected Widget m_RadialMenuSelector;
	protected Widget m_RadialMenuItemCard;

	void eAICommandMenuItem(int id, string name, eAICommandCategories category)
	{
		m_ID = id;
		m_Name = name;
		m_Category = category;
	}

	string GetName()
	{
		return m_Name;
	}

	int GetID()
	{
		return m_ID;
	}

	eAICommandCategories GetCategory()
	{
		return m_Category;
	}

	Widget GetRadialItemCard()
	{
		return m_RadialMenuItemCard;
	}

	void SetRadialItemCard(Widget widget)
	{
		m_RadialMenuItemCard = widget;
	}
}

class eAICommandMenu extends UIScriptedMenu
{
	protected Widget m_GestureItemCardPanel;
	protected ref array < ref eAICommandMenuItem > m_GestureItems;

	protected TextWidget m_CategoryNameText;

	//
	const string RADIAL_TEXT = "RadialText";
	const string CATEGORY_NAME = "CategoryName";
	//selections
	protected Widget m_SelectedItem;
	protected bool m_IsCategorySelected;
	protected bool m_IsMenuClosing;

	//instance
	static eAICommandMenu instance;

	//============================================
	// eAICommandMenu
	//============================================
	void eAICommandMenu()
	{
		m_GestureItems = new ref array < ref eAICommandMenuItem > ;

		if (!instance)
		{
			instance = this;
		}
	}

	void~eAICommandMenu() {}

	//============================================
	// Init &Widget Events
	//============================================
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/radial_menu/radial_gestures/day_z_gestures.layout");
		m_GestureItemCardPanel = layoutRoot.FindAnyWidget(RadialMenu.RADIAL_ITEM_CARD_CONTAINER);

		//register gestures menu
		RadialMenu.GetInstance().RegisterClass(this);

		//delay updates until fully initialized
		RadialMenu.GetInstance().SetWidgetInitialized(false);

		//set radial menu properties
		RadialMenu.GetInstance().SetWidgetProperties("gui/layouts/radial_menu/radial_gestures/day_z_gesture_delimiter.layout");

		//create content (widgets) for items
		RefreshGestures();

		#ifdef PLATFORM_WINDOWS
		Widget toolbar_panel = layoutRoot.FindAnyWidget("toolbar_bg");
		toolbar_panel.Show(!RadialMenu.GetInstance().IsUsingMouse());
		#endif

		//clear category name text
		UpdateCategoryName("");

		return layoutRoot;
	}

	override void OnShow()
	{
		super.OnShow();

		if (GetGame().GetUIManager())
	        GetGame().GetUIManager().ShowCursor(true);
	
	    if (GetGame().GetMission())// thanks Wardog
	        GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_MOUSE_RADIAL);
		
		m_IsMenuClosing = false;
	}
	
	override void OnHide()
	{
	    super.OnHide();
	
	    if (GetParentMenu()) // check if we're a child menu, we shouldn't give control back yet
	        return;
	
	    if (GetGame().GetUIManager())
	        GetGame().GetUIManager().ShowCursor(false);
	
	    if (GetGame().GetMission())
	        GetGame().GetMission().PlayerControlEnable(true);
		
		m_IsMenuClosing = true;
	}

	override bool OnController(Widget w, int control, int value)
	{
		super.OnController(w, control, value);

		RadialMenu.GetInstance().SetControlType(RadialMenuControlType.CONTROLLER);

		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		super.OnMouseEnter(w, x, y);

		RadialMenu.GetInstance().SetControlType(RadialMenuControlType.MOUSE);

		return false;
	}

	override bool UseMouse()
	{
		return true;
	}

	override bool UseGamepad()
	{
		return true;
	}

	//============================================
	// Gestures
	//============================================
	protected void RefreshGestures(eAICommandCategories category_id = -1)
	{
		//create gestures content (widgets) based on categories
		if (category_id > -1)
		{
			GetGestureItems(m_GestureItems, category_id);
		}
		else
		{
			GetGestureItems(m_GestureItems, eAICommandCategories.CATEGORIES);
		}

		CreateGestureContent();

		UpdateToolbar();
	}

	protected void GetGestureItems(out ref array < ref eAICommandMenuItem > gesture_items, eAICommandCategories category)
	{
		gesture_items.Clear();

		//All categories
		if (category == eAICommandCategories.CATEGORIES)
		{
			gesture_items.Insert(new eAICommandMenuItem(eAICommandCategories.CAT_MOVEMENT, "Movement", eAICommandCategories.CATEGORIES));
			gesture_items.Insert(new eAICommandMenuItem(eAICommandCategories.CAT_FORMATION, "Formation", eAICommandCategories.CATEGORIES));
			gesture_items.Insert(new eAICommandMenuItem(eAICommandCategories.CAT_STATUS, "Status", eAICommandCategories.CATEGORIES));
			gesture_items.Insert(new eAICommandMenuItem(eAICommandCategories.CAT_DEBUG, "Debug", eAICommandCategories.CATEGORIES));
		}

		//Category 1 - Movement
		else if (category == eAICommandCategories.CAT_MOVEMENT)
		{
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.MOV_STOP, "Stop", eAICommandCategories.CAT_MOVEMENT));
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.MOV_GOTO, "Go To...", eAICommandCategories.CAT_MOVEMENT));
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.MOV_RTF, "Rejoin", eAICommandCategories.CAT_MOVEMENT));
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.MOV_GETIN, "Get In", eAICommandCategories.CAT_MOVEMENT));
		}

		//Category 2 - Formation
		else if (category == eAICommandCategories.CAT_FORMATION)
		{
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.FOR_VEE, "Form Vee", eAICommandCategories.CAT_FORMATION));
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.FOR_FILE, "Form File", eAICommandCategories.CAT_FORMATION));
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.FOR_WALL, "Form Wall", eAICommandCategories.CAT_FORMATION));
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.FOR_COL, "Form Column", eAICommandCategories.CAT_FORMATION));
		}

		//Category 3 - Status
		else if (category == eAICommandCategories.CAT_STATUS)
		{
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.STA_SITREP, "Report Situation", eAICommandCategories.CAT_STATUS));
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.STA_POSITION, "Report Position", eAICommandCategories.CAT_STATUS));
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.STA_THREATS, "Report Threats", eAICommandCategories.CAT_STATUS));
		}

		//Category 4 - Debug
		else if (category == eAICommandCategories.CAT_DEBUG)
		{
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.DEB_SPAWNALLY, "Spawn Ally", eAICommandCategories.CAT_DEBUG));
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.DEB_CLEARALL, "Clear All AI", eAICommandCategories.CAT_DEBUG));
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.DEB_SPAWNZOM, "Spawn Zombie", eAICommandCategories.CAT_DEBUG));
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.DEB_AIMAP, "AI Menu", eAICommandCategories.CAT_DEBUG));
			gesture_items.Insert(new eAICommandMenuItem(eAICommands.DEB_GRPMGR, "Group Manager", eAICommandCategories.CAT_DEBUG));
		}
	}

	protected void CreateGestureContent()
	{
		//delete existing gesture widgets
		DeleteGestureItems();

		for (int i = 0; i < m_GestureItems.Count(); ++i)
		{
			eAICommandMenuItem gesture_item = m_GestureItems.Get(i);

			//create item card
			Widget gesture_item_card_widget = Widget.Cast(GetGame().GetWorkspace().CreateWidgets("gui/layouts/radial_menu/radial_gestures/day_z_gesture_item_card.layout", m_GestureItemCardPanel));
			gesture_item.SetRadialItemCard(gesture_item_card_widget);

			//update item card widget
			UpdateQuickbarItemCard(gesture_item);

			//set data
			gesture_item_card_widget.SetUserData(gesture_item);
		}

		//set radial parameters for content
		if (m_GestureItems.Count() > 0)
		{
			RadialMenu radial_menu = RadialMenu.GetInstance();
			radial_menu.SetRadiusOffset(0);
			radial_menu.SetExecuteDistOffset(0.5);
			radial_menu.SetOffsetFromTop(0);
			radial_menu.SetItemCardRadiusOffset(0.25);
			radial_menu.ActivateControllerTimeout(false);
			radial_menu.SetSelectorColors(ARGB(255,21,84,21), ARGB(255,21,84,21));
		}

		//refresh radial menu
		RadialMenu.GetInstance().Refresh();
	}

	protected void UpdateQuickbarItemCard(eAICommandMenuItem gesture_item)
	{
		Widget gesture_item_card_widget = gesture_item.GetRadialItemCard();

		//set text
		ButtonWidget text_widget;
		if (Class.CastTo(text_widget, gesture_item_card_widget.FindAnyWidget(RADIAL_TEXT))) text_widget.SetText(gesture_item.GetName());
	}

	protected void DeleteGestureItems()
	{
		Widget child;
		Widget child_to_destroy;

		child = m_GestureItemCardPanel.GetChildren();
		while (child)
		{
			child_to_destroy = child;
			child = child.GetSibling();

			delete child_to_destroy;
		}
	}

	protected void UpdateToolbar()
	{
		#ifdef PLATFORM_CONSOLE
		//set controller toolbar icons
		string select_img;
		//string back_img;

		ImageWidget toolbar_select = ImageWidget.Cast(layoutRoot.FindAnyWidget("SelectIcon"));
		//ImageWidget toolbar_back = ImageWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));

		if (instance.m_IsCategorySelected)
		{
			#ifdef PLATFORM_XBOX
			select_img = "set:xbox_buttons image:A";
			//back_img = "set:xbox_buttons image:B";
			#endif
			#ifdef PLATFORM_PS4
			string confirm = "cross";
			if (GetGame().GetInput().GetEnterButton() == GamepadButton.A)
			{
				confirm = "cross";
			}
			else
			{
				confirm = "circle";
			}

			select_img = "set:playstation_buttons image:" + confirm;
			//back_img = "set:playstation_buttons image:circle";
			#endif
		}
		else
		{
			#ifdef PLATFORM_XBOX
			select_img = "set:xbox_buttons image:R_up";
			//back_img = "set:xbox_buttons image:B";
			#endif
			#ifdef PLATFORM_PS4
			select_img = "set:playstation_buttons image:R_up";
			//back_img = "set:playstation_buttons image:circle";
			#endif
		}

		toolbar_select.LoadImageFile(0, select_img);
		//toolbar_back.LoadImageFile(0, back_img);
		#endif
	}

	protected void UpdateCategoryName(string name)
	{
		if (!m_CategoryNameText)
		{
			m_CategoryNameText = TextWidget.Cast(layoutRoot.FindAnyWidget(CATEGORY_NAME));
		}
		
		if (!name || name.Length() == 0)
			name = "eAI Commands";

		m_CategoryNameText.SetText(name);
	}

	//============================================
	// Radial Menu Events
	//============================================
	//Common
	void OnControlsChanged(RadialMenuControlType type)
	{
		//show/hide controller toolbar
		Widget toolbar_panel = layoutRoot.FindAnyWidget("toolbar_bg");
		if (type == RadialMenuControlType.CONTROLLER)
		{
			#ifdef PLATFORM_CONSOLE
			toolbar_panel.Show(true);
			#endif
		}
		else
		{
			toolbar_panel.Show(false);
		}
	}

	//Mouse
	void OnMouseSelect(Widget w)
	{
		MarkSelected(w);
	}

	void OnMouseDeselect(Widget w)
	{
		UnmarkSelected(w);
	}

	void OnMouseExecute(Widget w)
	{
		ExecuteSelectedCategory(w);
	}

	//Controller
	void OnControllerSelect(Widget w)
	{
		MarkSelected(w);
	}

	void OnControllerDeselect(Widget w)
	{
		UnmarkSelected(w);
	}

	void OnControllerExecute(Widget w)
	{
		ExecuteSelectedCategory(w);
	}

	void OnControllerPressSelect(Widget w)
	{
		ExecuteSelectedItem();
	}

	void OnControllerPressBack(Widget w)
	{
		//back to category or close menu?
		/*
		if (instance.m_IsCategorySelected)
		{
		instance.m_IsCategorySelected = false;	//reset category selection
		RefreshGestures();	//back to categories
		}
		else
		{
		//close menu
		CloseMenu();
		}

		*/
	}

	//Gestures Menu
	void OnMenuRelease()
	{
		//execute on release (mouse only)
		if (RadialMenu.GetInstance().IsUsingMouse())
		{
			ExecuteSelectedItem();
		}
	}

	//Actions
	protected void MarkSelected(Widget w)
	{
		instance.m_SelectedItem = w;

		if (w)
		{
			eAICommandMenuItem gesture_item;
			w.GetUserData(gesture_item);

			//is not category
			/*
			if (gesture_item && gesture_item.GetCategory() != eAICommandCategories.CATEGORIES)
			{ 
			//alter item visual
			//TextWidget text_widget = TextWidget.Cast(gesture_item.GetRadialItemCard().FindAnyWidget(RADIAL_TEXT));
			//text_widget.SetColor(ARGB(255, 66, 175, 95));
			}

			*/
		}
	}

	protected void UnmarkSelected(Widget w)
	{
		instance.m_SelectedItem = NULL;

		if (w)
		{
			eAICommandMenuItem gesture_item;
			w.GetUserData(gesture_item);

			//is not category
			/*
			if (gesture_item && gesture_item.GetCategory() != eAICommandCategories.CATEGORIES)
			{ 
			//alter item visual
			//TextWidget text_widget = TextWidget.Cast(gesture_item.GetRadialItemCard().FindAnyWidget(RADIAL_TEXT));
			//text_widget.SetColor(ARGB(255, 255, 255, 255));
			}

			*/
		}
	}

	protected void ExecuteSelectedCategory(Widget w)
	{
		//only when category is not picked yet
		if (w && !instance.m_IsCategorySelected)
		{
			eAICommandMenuItem gesture_item;
			w.GetUserData(gesture_item);

			//is category
			if (gesture_item.GetCategory() == eAICommandCategories.CATEGORIES)
			{
				//set category selected
				instance.m_IsCategorySelected = true;

				//show selected category gestures
				GetGestureItems(m_GestureItems, gesture_item.GetID());
				CreateGestureContent();
				RefreshGestures(gesture_item.GetID());

				//update category name text
				UpdateCategoryName(gesture_item.GetName());
			}
		}
	}

	protected void ExecuteSelectedItem()
	{
		if (instance.m_IsCategorySelected && instance.m_SelectedItem)
		{
			if (!GetGame().IsMultiplayer() || GetGame().IsClient())
			{
				eAICommandMenuItem selected;
				instance.m_SelectedItem.GetUserData(selected);

				if (selected)
					DayZGame.Cast(GetGame()).GetEAICommandManager().Send(selected.GetID());
			}
		}
	}

	bool IsMenuClosing()
	{
		return m_IsMenuClosing;
	}

	void SetMenuClosing(bool state)
	{
		m_IsMenuClosing = state;
	}
}
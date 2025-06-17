            /*
  Copyright (C) 2000 Kimmo Pekkola

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/CalendarWindow.cpp,v 1.23 2003/08/10 08:45:01 Rainy Exp $

  $Log: CalendarWindow.cpp,v $
  Revision 1.23  2003/08/10 08:45:01  Rainy
  Fixed the hotkeys.

  Revision 1.22  2003/08/09 16:40:32  Rainy
  Added icons to messageboxes.
  Added hotkeys.
  Removed powerbroadcast (timer should do the same thing)

  Revision 1.21  2003/06/15 09:50:54  Rainy
  Strings are read from CLanguage class.
  Added support for multiple calendars.

  Revision 1.20  2003/05/26 18:45:03  Rainy
  Added AllEvents menu item.

  Revision 1.19  2003/05/25 18:08:37  Rainy
  Added tooltip separator.

  Revision 1.18  2003/05/24 13:51:00  Rainy
  Added refresh when windows refurns from suspend.

  Revision 1.17  2003/05/07 19:16:25  rainy
  Few bugs fixed.
  Added support for Outlook.

  Revision 1.16  2003/03/22 20:31:11  rainy
  Added Move and ZPos bangs

  Revision 1.15  2002/11/25 17:11:58  rainy
  Wallpaper polling checks the timestamp too.

  Revision 1.14  2002/11/12 18:05:46  rainy
  Added support for native transparency.
  The window is created after configs are loaded.
  Snapping is disabled when control or shift is down.

  Revision 1.13  2002/09/07 09:37:38  rainy
  Fixed a bug that selected wrong day sometimes.
  Added support for OnDesktop setting.

  Revision 1.12  2002/08/24 11:14:35  rainy
  Changed the error handling.
  Added lite refreshing.
  Added a lot of logging information.

  Revision 1.11  2002/08/10 08:40:38  rainy
  Need to init common controls.
  Uncommented startup notification stuff.

  Revision 1.10  2002/08/03 16:21:52  rainy
  Added showEdges.
  Added support for skins.
  Changed to use the new ToolTip class.
  Changed to use separate skin config dialog.
  + other changes that I cannot remember now :-(

  Revision 1.9  2002/05/30 18:29:35  rainy
  Added the config stuff.
  Other small fixes.

  Revision 1.8  2002/05/23 17:33:42  rainy
  Removed all MFC stuff

  Revision 1.7  2002/02/27 19:12:50  rainy
  Wallpaper changes can be polled.
  Added ontop and normal window positioning.
  Window can be moved.
  Refreshing is automatic on resolution changes.
  Window can be hidden on mouse over.
  Added an About dialog.
  Bangs can be supplied with WM_COPYDATA.

  Revision 1.6  2002/01/29 17:35:38  rainy
  Added server communication

  Revision 1.5  2002/01/27 16:12:44  rainy
  Changed CEvent to CEventMessage to avoid name clash.
  Added Server stuff.

  Revision 1.4  2002/01/15 17:59:19  rainy
  Changed the way refreshing is done.

  Revision 1.3  2002/01/10 16:49:14  rainy
  The items weren't deallocated during refresh. Fixed and changed them to pointers.

  Revision 1.2  2001/12/23 10:03:57  rainy
  Renamed the static variables (C_ -> c_).
  Added possibility to run as wharf module.
  Quit is disabled if run as Litestep plug-in.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "CalendarWindow.h"
#include "Config.h"
#include "Error.h"
#include "EditEvent.h"
#include "NetworkThread.h"
#include "ConfigDialog.h"
#include "SkinDialog.h"
#include "Tooltip.h"
#include "RasterizerFont.h"
#include "Outlook.h"
#include "AllEventsDialog.h"
#include "../Server/EventCombiner.h"
#include <crtdbg.h>

#define ULW_ALPHA               0x00000002
#define WS_EX_LAYERED           0x00080000

// Timers
#define DATE_TIMER 1
#define OUTLOOK_TIMER 2

// Defines
#define SNAPDISTANCE 10

// Class variables
CConfig CCalendarWindow::c_Config;
SYSTEMTIME CCalendarWindow::c_TodaysDate;
SYSTEMTIME CCalendarWindow::c_MonthsFirstDate;
CLanguage CCalendarWindow::c_Language;

/* 
** CCalendarWindow
**
** Constructor
**
*/
CCalendarWindow::CCalendarWindow() 
{
	m_X=0;
	m_Y=0;
	m_Width=0;
	m_Height=0;
	m_FirstExecute=true;
	m_SelectedDate=0;
	m_MenuSelectedDate=0;

	m_Days = NULL;
	m_Event = NULL;
	m_Today = NULL;
	m_Weekdays = NULL;
	m_WeekNumbers = NULL;
	m_Month = NULL;
	m_Year = NULL;
	m_ConnectionCounter = 0;
	m_Hidden = false;
	m_Refreshing = false;
	m_WallpaperTime.dwLowDateTime = 0;
	m_WallpaperTime.dwHighDateTime = 0;
}

/* 
** ~CCalendarWindow
**
** Destructor
**
*/
CCalendarWindow::~CCalendarWindow()
{
	c_Config.WriteConfig(CConfig::WRITE_FULL);

	CRasterizerFont::ClearBuffers();

	if(m_DC) DeleteDC(m_DC);

	delete m_Days;
	delete m_Event;
	delete m_Today;
	delete m_Weekdays;
	delete m_WeekNumbers;
	delete m_Month;
	delete m_Year;

	if(m_Window) DestroyWindow(m_Window);

	// Unregister the window class
	UnregisterClass("Rainlendar", m_Instance);

	LSLog(LOG_DEBUG, "Rainlendar", "The calendar window is destroyed.");
}

/* 
** Initialize
**
** Initializes the window, creates the class and the window.
**
*/
int CCalendarWindow::Initialize(CRainlendar& Rainlendar, HWND Parent, HINSTANCE Instance)
{
	int flags;
	WNDCLASSEX wc;
	char tmpName[MAX_LINE_LENGTH];
	char* windowName = NULL;

	if(Parent==NULL || Instance==NULL) 
	{
		THROW(ERR_NULLPARAMETER);
	}

	m_Instance = Instance;
	m_Rainlendar = &Rainlendar;

	std::string PixPath, EventsPath, SkinsPath, LanguagesPath;
	std::string skin, skinIni;

	if(!CRainlendar::GetDummyLitestep()) 
	{
		GetRCString("RainlendarPath", tmpName, "C:\\Litestep\\Rainlendar\\", MAX_LINE_LENGTH);
		VarExpansion(tmpName, tmpName);
		PixPath = tmpName;

		GetRCString("RainlendarEventsPath", tmpName, "", MAX_LINE_LENGTH);
		VarExpansion(tmpName, tmpName);
		EventsPath = tmpName;

		if (!EventsPath.empty())
		{
			// Check that the path ends with "\" 
			if (*(EventsPath.end() - 1) != '\\')
			{
				EventsPath += "\\";
			}
			EventsPath += "Events.ini";
		}

		GetRCString("RainlendarSkinsPath", tmpName, "", MAX_LINE_LENGTH);
		VarExpansion(tmpName, tmpName);
		SkinsPath = tmpName;

		GetRCString("RainlendarLanguagesPath", tmpName, "", MAX_LINE_LENGTH);
		VarExpansion(tmpName, tmpName);
		LanguagesPath = tmpName;

		// Check that the path ends with "\" 
		if (!SkinsPath.empty() && *(SkinsPath.end() - 1) != '\\')
		{
			SkinsPath += "\\";
		}

		GetRCString("RainlendarCurrentSkin", tmpName, "", MAX_LINE_LENGTH);
		skin = tmpName;
		GetRCString("RainlendarCurrentSkinIni", tmpName, "", MAX_LINE_LENGTH);
		skinIni = tmpName;
	}
	else 
	{
		PixPath = CRainlendar::GetCommandLine();
	}

	// Remove quotes from the path
	int pos = PixPath.find("\"");
	while (pos != -1)
	{
		PixPath.erase(PixPath.begin() + pos, PixPath.begin() + pos + 1);
		pos = PixPath.find("\"");
	}

	// Check that the path ends with "\" 
	if (!PixPath.empty() && *(PixPath.end() - 1) != '\\')
	{
		PixPath += "\\";
	}

	if (LanguagesPath.empty())
	{
		LanguagesPath = PixPath + "Languages\\";
	}
	else if (*(LanguagesPath.end() - 1) != '\\')
	{
		LanguagesPath += "\\";
	}
	c_Language.ScanLanguages(LanguagesPath.c_str());

	if (EventsPath.empty())
	{
		EventsPath = PixPath + "Events.ini";
	}

	if (SkinsPath.empty())
	{
		// Get the DLL's directory
		GetModuleFileName(CRainlendar::GetInstance(), tmpName, MAX_LINE_LENGTH);
		char* Slash = strrchr(tmpName, '\\');
		*(Slash + 1) = 0;	// Cut the Rainlendar.dll from the name
		SkinsPath = tmpName;
		SkinsPath += "Skins\\";
	}

	LSLog(LOG_DEBUG, "Rainlendar", "Rainlendar paths:");
	LSLog(LOG_DEBUG, "Rainlendar", PixPath.c_str());
	LSLog(LOG_DEBUG, "Rainlendar", EventsPath.c_str());
	LSLog(LOG_DEBUG, "Rainlendar", SkinsPath.c_str());
	LSLog(LOG_DEBUG, "Rainlendar", LanguagesPath.c_str());

	c_Config.SetPath(PixPath);
	c_Config.SetEventsPath(EventsPath);	
	c_Config.SetSkinsPath(SkinsPath);
	c_Config.ReadConfig();

	// Register the windowclass
	memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.style = CS_NOCLOSE | CS_DBLCLKS;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = m_Instance;
	wc.lpszClassName = "Rainlendar";
	
	if(!RegisterClassEx(&wc))
	{
		if (ERROR_CLASS_ALREADY_EXISTS != GetLastError())
		{
			THROW(ERR_WINDOWCLASS);
		}
	}
	
	if (Rainlendar.GetDummyLitestep())
	{
		LSLog(LOG_DEBUG, "Rainlendar", "Running as stand-alone.");
	}
	else
	{
		LSLog(LOG_DEBUG, "Rainlendar", "Running as Litestep plugin.");
	}

	if(Rainlendar.IsWharf())
	{
		// If we're in lsBox or in wharf, this is a childwindow
		flags = WS_CHILD;
		LSLog(LOG_DEBUG, "Rainlendar", "Running as wharf module.");
	}
	else
	{
		// ... otherwise this is normal popup window
		flags = WS_POPUP;
		Parent = NULL;
	}

	if (c_Config.GetUseWindowName())
	{
		windowName = "Rainlendar";
	}

	m_Window = CreateWindowEx(WS_EX_TOOLWINDOW, 
							"Rainlendar", 
							windowName, 
							flags,
							c_Config.GetX(),
							c_Config.GetY(),
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							Parent,
							NULL,
							m_Instance,
							this);

	if(m_Window == NULL) 
	{ 
		THROW(ERR_WINDOW);
	}

	INITCOMMONCONTROLSEX cce;
	cce.dwSize = sizeof(INITCOMMONCONTROLSEX);
	cce.dwICC = ICC_DATE_CLASSES;
	InitCommonControlsEx(&cce);

	CToolTip::Instance().Initialize(m_Window, m_Instance);

	SetWindowLong(m_Window, GWL_USERDATA, magicDWord);


	// If the skin was defined explicitely, let's use it
	if (!skin.empty() && !skinIni.empty())
	{
		c_Config.SetCurrentSkin(skin);
		c_Config.SetCurrentSkinIni(skinIni);
		c_Config.WriteConfig(CConfig::WRITE_CONFIG);	// Replace the current config with the step.rc values
	}

	// Scan the skin folder for skins
	ReadSkins();

	GetLocalTime(&c_TodaysDate);
	// Store current date (the month that will be shown)
	c_MonthsFirstDate = c_TodaysDate;

	if(!c_Config.GetStartHidden()) ShowWindow();
	
	ChangeMonth(-1, -1);
	Refresh();
	
	SetTimer(m_Window, DATE_TIMER, 1000, NULL);	// Date is checked once per second

	if (c_Config.GetServerStartup())
	{
		ConnectServer(REQUEST_GETEVENTS);	// Get the events from server
	}

	return 0;
}

/* 
** FillMenu
**
** Fills the context menu with months
**
*/
void CCalendarWindow::FillMenu(HMENU subMenu, int x, int y)
{
	char tmpSz[256];

	// Localize the menu
	ModifyMenu(subMenu, 0, MF_BYPOSITION | MF_STRING, GetMenuItemID(subMenu, 0), c_Language.GetString("Menu", 0));	// About
	ModifyMenu(subMenu, 2, MF_BYPOSITION | MF_STRING, GetMenuItemID(subMenu, 2), c_Language.GetString("Menu", 25));	// Skins
	ModifyMenu(subMenu, 3, MF_BYPOSITION | MF_STRING, GetMenuItemID(subMenu, 3), c_Language.GetString("Menu", 22));	// Language
	ModifyMenu(subMenu, 4, MF_BYPOSITION | MF_STRING, GetMenuItemID(subMenu, 4), c_Language.GetString("Menu", 17));	// Refresh
	ModifyMenu(subMenu, 5, MF_BYPOSITION | MF_STRING, GetMenuItemID(subMenu, 5), c_Language.GetString("Menu", 28));	// Update Outlook
	ModifyMenu(subMenu, 6, MF_BYPOSITION | MF_STRING, GetMenuItemID(subMenu, 6), c_Language.GetString("Menu", 20));	// Config
	ModifyMenu(subMenu, 8, MF_BYPOSITION | MF_STRING, GetMenuItemID(subMenu, 8), c_Language.GetString("Menu", 1));	// Select Month
	ModifyMenu(subMenu, 9, MF_BYPOSITION | MF_STRING, GetMenuItemID(subMenu, 9), c_Language.GetString("Menu", 18));	// Set event...
	ModifyMenu(subMenu, 10, MF_BYPOSITION | MF_STRING, GetMenuItemID(subMenu, 10), c_Language.GetString("Menu", 19));	// Show all events...
	ModifyMenu(subMenu, 12, MF_BYPOSITION | MF_STRING, GetMenuItemID(subMenu, 12), c_Language.GetString("Menu", 21));	// Quit

	HMENU monthsMenu = GetSubMenu(subMenu, 7);
	if (monthsMenu)
	{
		ModifyMenu(monthsMenu, 0, MF_BYPOSITION | MF_STRING, GetMenuItemID(monthsMenu, 0), c_Language.GetString("Menu", 2));	// Next Month
		ModifyMenu(monthsMenu, 1, MF_BYPOSITION | MF_STRING, GetMenuItemID(monthsMenu, 1), c_Language.GetString("Menu", 3));	// Prev Month
		ModifyMenu(monthsMenu, 2, MF_BYPOSITION | MF_STRING, GetMenuItemID(monthsMenu, 2), c_Language.GetString("Menu", 26));	// Next Year
		ModifyMenu(monthsMenu, 3, MF_BYPOSITION | MF_STRING, GetMenuItemID(monthsMenu, 3), c_Language.GetString("Menu", 27));	// Prev Year
		ModifyMenu(monthsMenu, 4, MF_BYPOSITION | MF_STRING, GetMenuItemID(monthsMenu, 4), c_Language.GetString("Menu", 4));	// Current Month

		// Change the months in the submenu
		int year = c_TodaysDate.wYear + 1;
		std::string name;
		for(int i = 0; i < 12; i++) 
		{
			if (i + 1 == c_TodaysDate.wMonth) year--;

			sprintf(tmpSz, " %i\tF%i", year, i + 1);
			name = c_Language.GetString("Menu", i + 5);
			name += tmpSz;

			ModifyMenu(monthsMenu, i + 6, MF_BYPOSITION | MF_STRING, GetMenuItemID(monthsMenu, i + 6), name.c_str());
		}
	}
	
	// If mouse was pressed on a day, enable set event

	m_MenuSelectedDate = m_Days->HitTest(x, y);

	if (m_MenuSelectedDate != 0) 
	{
		EnableMenuItem( subMenu, ID_SETEVENT, MF_ENABLED);
	} 
	else 
	{
		EnableMenuItem( subMenu, ID_SETEVENT, MF_GRAYED);
	}

	if (!CRainlendar::GetDummyLitestep())
	{
		// Disable QUIT if LiteStep's plugin 
		EnableMenuItem( subMenu, ID_QUIT, MF_GRAYED);
	}

	if (!c_Config.GetShowOutlookAppointments())
	{
		EnableMenuItem( subMenu, ID_POPUP_UPDATEOUTLOOK, MF_GRAYED);
	}

	int index = 0;

	// Fill the menu with all the configs
	HMENU configMenu = GetSubMenu(subMenu, 2);
	if(configMenu)
	{
		ModifyMenu(configMenu, 0, MF_BYPOSITION | MF_STRING, GetMenuItemID(configMenu, 0), c_Language.GetString("Menu", 24));	// Edit Current Skin

		for(int i = 0; i < m_ConfigStrings.size(); i++)
		{
			if (m_ConfigStrings[i].iniFiles.size() > 1)
			{
				HMENU iniMenu = CreatePopupMenu();
				InsertMenu(configMenu, i + 2, MF_BYPOSITION | MF_POPUP, (UINT_PTR)iniMenu, m_ConfigStrings[i].path.c_str());
				for(int j = 0; j < m_ConfigStrings[i].iniFiles.size(); j++)
				{
					std::string iniName = m_ConfigStrings[i].iniFiles[j].c_str();
					iniName.erase(iniName.end() - 4, iniName.end());
					InsertMenu(iniMenu, j, MF_BYPOSITION, ID_SKIN + index++, iniName.c_str());

					if(m_ConfigStrings[i].path == c_Config.GetCurrentSkin() && m_ConfigStrings[i].iniFiles[j] == c_Config.GetCurrentSkinIni())
					{
						CheckMenuItem(iniMenu, j, MF_BYPOSITION | MF_CHECKED);
					}
				}
			}
			else
			{
				InsertMenu(configMenu, i + 2, MF_BYPOSITION, ID_SKIN + index++, m_ConfigStrings[i].path.c_str());

				if(m_ConfigStrings[i].path == c_Config.GetCurrentSkin())
				{
					CheckMenuItem(configMenu, i + 2, MF_BYPOSITION | MF_CHECKED);
				}
			}
		}
	}

	// Fill the menu with all the configs
	HMENU languageMenu = GetSubMenu(subMenu, 3);
	if(languageMenu)
	{
		bool languageFound = false;
		ModifyMenu(languageMenu, 0, MF_BYPOSITION | MF_STRING, GetMenuItemID(languageMenu, 0), c_Language.GetString("Menu", 23));	// English

		for(int i = 0; i < c_Language.GetLanguages().size(); i++)
		{
			InsertMenu(languageMenu, i + 1, MF_BYPOSITION, ID_LANGUAGE + i + 1, c_Language.GetLanguages()[i].c_str());

			if(c_Language.GetLanguages()[i] == c_Config.GetCurrentLanguage())
			{
				CheckMenuItem(languageMenu, i + 1, MF_BYPOSITION | MF_CHECKED);
				languageFound = true;
			}
		}

		if (!languageFound) 
		{
			CheckMenuItem(languageMenu, 0, MF_BYPOSITION | MF_CHECKED);
		}
	}

}

/* 
** CalcWindowSize
**
** Calculates the windows correct size.
**
*/
void CCalendarWindow::CalcWindowSize()
{
	m_Width=0;
	m_Height=0;

	if(c_Config.GetDaysEnable()) 
	{
		m_Width = max(m_Width, m_Days->GetX() + m_Days->GetW());
		m_Height = max(m_Height, m_Days->GetY() + m_Days->GetH());
	}
	if(c_Config.GetWeekdaysEnable()) 
	{
		m_Width = max(m_Width, m_Weekdays->GetX() + m_Weekdays->GetW());
		m_Height = max(m_Height, m_Weekdays->GetY() + m_Weekdays->GetH());
	}
	if(c_Config.GetWeekNumbersEnable()) 
	{
		m_Width = max(m_Width, m_WeekNumbers->GetX() + m_WeekNumbers->GetW());
		m_Height = max(m_Height, m_WeekNumbers->GetY() + m_WeekNumbers->GetH());
	}
	if(c_Config.GetMonthEnable()) 
	{
		m_Width = max(m_Width, m_Month->GetX() + m_Month->GetW());
		m_Height = max(m_Height, m_Month->GetY() + m_Month->GetH());
	}
	if(c_Config.GetYearEnable()) 
	{
		m_Width = max(m_Width, m_Year->GetX() + m_Year->GetW());
		m_Height = max(m_Height, m_Year->GetY() + m_Year->GetH());
	}
	if(c_Config.GetTodayEnable()) 
	{
		m_Width = max(m_Width, m_Today->GetX() + m_Today->GetW());
		m_Height = max(m_Height, m_Today->GetY() + m_Today->GetH());
	}

	m_Width *= c_Config.GetHorizontalCount();
	m_Height *= c_Config.GetVerticalCount();

	// Background already knows about vert & horz counts.
	if(m_Background.GetWidth() > m_Width) m_Width = m_Background.GetWidth();
	if(m_Background.GetHeight() > m_Height) m_Height = m_Background.GetHeight();
}

/* 
** Refresh
**
** Refresh the window
**
*/
void CCalendarWindow::Refresh(bool lite)
{
	char buffer[1024];

	DWORD time = GetTickCount();
	if (lite)
	{
		LSLog(LOG_DEBUG, "Rainlendar", "Refresh started (lite).");
	}
	else
	{
		LSLog(LOG_DEBUG, "Rainlendar", "Refresh started.");
	}

	try 
	{
		m_Refreshing = true;

		if (!lite)
		{
			c_Config.ReadConfig();

			KillTimer(m_Window, OUTLOOK_TIMER);
			if (c_Config.GetOutlookUpdate() > 0)
			{
				SetTimer(m_Window, OUTLOOK_TIMER, c_Config.GetOutlookUpdate() * 1000, NULL);	// Outlook appointments update is in secs
			}


			sprintf(buffer, "Skin: %s, Ini: %s", c_Config.GetCurrentSkin().c_str(), c_Config.GetCurrentSkinIni().c_str());
			LSLog(LOG_DEBUG, "Rainlendar", buffer);

			// Initialize
			delete m_Days;
			delete m_Event;
			delete m_Today;
			delete m_Weekdays;
			delete m_WeekNumbers;
			delete m_Month;
			delete m_Year;

			// Reset the region
			SetWindowRgn(m_Window, NULL, TRUE);

			m_Days = new CItemDays;
			m_Event = new CItemEvent;
			m_Today = new CItemToday;
			m_Weekdays = new CItemWeekdays;
			m_WeekNumbers = new CItemWeekNumbers;
			m_Month = new CItemMonth;
			m_Year = new CItemYear;

			if(m_FirstExecute || (CCalendarWindow::c_Config.GetBGCopyMode() == CConfig::BG_WALLPAPER_ALWAYS ||
								 (CCalendarWindow::c_Config.GetBGCopyMode() == CConfig::BG_NORMAL && !(m_Rainlendar->IsWharf()))))
			{
    			m_Background.Initialize();
			}
			m_Days->Initialize();
			m_Event->Initialize();
			m_Today->Initialize();
			m_Weekdays->Initialize();
			m_WeekNumbers->Initialize();
			m_Month->Initialize();
			m_Year->Initialize();

			CToolTip::Instance().SetBackgroundColor(c_Config.GetToolTipBGColor());
			CToolTip::Instance().SetFont(c_Config.GetToolTipFont());

			if (CCalendarWindow::c_Config.GetTooltipSeparator())
			{
				CToolTip::Instance().SetSeparation(5);
				CToolTip::Instance().SetSeparator(true);
			}
			else
			{
				CToolTip::Instance().SetSeparation(0);
				CToolTip::Instance().SetSeparator(false);
			}

			RegisterHotkeys();

			if (m_FirstExecute)
			{
				SyncWithOutlook();		// Only sync with first execute
			}
			GetOutlookEvents(GetEventManager());

			// Rescan the languages and re-read the current
			c_Language.ScanLanguages(NULL);	// Use the same path
			bool languageFound = false;
			for (int i = 0; i < c_Language.GetLanguages().size(); i++)
			{
				if (c_Language.GetLanguages()[i] == c_Config.GetCurrentLanguage())
				{
					c_Language.ChangeLanguage(i);
					languageFound = true;
					break;
				}
			}
			if (!languageFound)
			{
				c_Language.SetDefaultLanguage();
				c_Config.SetCurrentLanguage("");
			}
        }

		CRasterizerFont::ClearBuffers();

		if (!lite)
		{
			CalcWindowSize();

			MoveWindow(c_Config.GetX(), c_Config.GetY());

			if(m_FirstExecute || (CCalendarWindow::c_Config.GetBGCopyMode() == CConfig::BG_WALLPAPER_ALWAYS ||
								 (CCalendarWindow::c_Config.GetBGCopyMode() == CConfig::BG_NORMAL && !(m_Rainlendar->IsWharf()))))
			{
				CalcWindowSize();
				if (m_Background.Create(m_X, m_Y, m_Width, m_Height))
				{
					CalcWindowSize();	// Must be recalced if BG is bigger than current window
        			MoveWindow(m_X, m_Y);
				}
			}

			SetWindowZPos(c_Config.GetWindowPos());
		}

		// Create the DoubleBuffer
		m_DoubleBuffer.Create(m_Width, m_Height, 0x0);	// Fully transparent

		DrawCalendar();
		UpdateTransparency();

		if (!lite)
		{
			if(c_Config.GetBackgroundMode() != CBackground::MODE_COPY && !m_Background.HasAlpha())
			{
				// Set window region
				HRGN region = BitmapToRegion(m_DoubleBuffer.GetBitmap(), RGB(255,0,255), 0x101010, 0, 0);
				SetWindowRgn(m_Window, region, TRUE);
			}

			// Execute on event if this was first refresh (meaning that Rainlendar was started)
			if(m_FirstExecute) 
			{ 
				m_FirstExecute=false;
				
				ShowEventMessage();
			}
			
			PollWallpaper(true);
		}

		InvalidateRect(m_Window, NULL, false);

		m_Refreshing = false;
	} 
    catch(CError& error) 
    {
		MessageBox(m_Window, error.GetString().c_str(), APPNAME, MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
		OnCommand(WM_QUIT, NULL);	// Gotta quit
	}

	sprintf(buffer, "Refresh ended (took: %i ms).", GetTickCount() - time);
	LSLog(LOG_DEBUG, "Rainlendar", buffer);
}

/*
** ShowEventMessage
**
** Opens a message box and displays the message in it. Also executes the command.
*/
void CCalendarWindow::ShowEventMessage()
{
	if(c_Config.GetEventEnable()) 
	{
		std::string text;
		std::vector<CEventMessage*> eventList = GetEventManager()->GetEvents(c_TodaysDate.wDay, c_TodaysDate.wMonth, c_TodaysDate.wYear);
		
		if (!eventList.empty())
		{
			std::vector<CEventMessage*>::iterator i = eventList.begin();
			for( ;  i != eventList.end(); i++)
			{
				if (!((*i)->GetMessage().empty()))
				{
					text += (*i)->GetMessage();
					text += "\n";
				}
			}

			if(!text.empty())
			{
				// Remove the last \n
				text.erase(text.end() - 1, text.end());
			}
		}

		if (!text.empty())
		{
			if(c_Config.GetEventMessageBox()) 
			{
				MessageBox(m_Window, text.c_str(), "Rainlendar", MB_OK | MB_TOPMOST);
			}
			
			std::string execute = c_Config.GetEventExecute();

			if(!execute.empty()) 
			{
				// Do not run bangs if litestep is not enabled
				if(!m_Rainlendar->GetDummyLitestep() || execute[0] != '!')
				{
					char buffer[1024];
					VarExpansion(buffer, execute.c_str());
					execute = buffer;
					int pos = execute.find("%m");
					if (pos != -1)
					{
						execute.replace(execute.begin() + pos, execute.begin() + pos + 2, text);
					}
					LSExecute(NULL, execute.c_str(), SW_SHOWNORMAL);
				}
			}
		} 
	} 
}

/*
** MoveWindow
**
** Moves the window to a new location
*/
void CCalendarWindow::MoveWindow(int x, int y)
{
	RECT r;

	if (c_Config.GetNegativeCoords())
	{
		// Handle negative coordinates
		GetClientRect(GetDesktopWindow(), &r); 
		if (x < 0) x += r.right - r.left;
		if (y < 0) y += r.bottom - r.top;
	}

	m_X = x;
	m_Y = y;

	SetWindowPos(m_Window, NULL, m_X, m_Y, m_Width, m_Height, SWP_NOACTIVATE | SWP_NOZORDER);
}

/*
** SetWindowZPos
**
** Sets the window's z-position (OnTop, Normal, OnBottom, OnDesktop)
*/
void CCalendarWindow::SetWindowZPos(CConfig::WINDOWPOS pos)
{
	if(m_Rainlendar->IsWharf()) return;		// Children shouldn't change z-position

	HWND parent = NULL;
	HWND winPos = HWND_NOTOPMOST;
	LONG oldStyle = GetWindowLong(m_Window, GWL_STYLE);
	LONG style = (oldStyle & ~WS_CHILD) | WS_POPUP;		// Change from child to popup

	switch (pos)
	{
	case CConfig::WINDOWPOS_ONTOP:
		winPos = HWND_TOPMOST;
		break;

	case CConfig::WINDOWPOS_ONBOTTOM:
		winPos = HWND_BOTTOM;
		break;

	case CConfig::WINDOWPOS_ONDESKTOP:
		{
			// Set the window's parent to progman, so it stays always on desktop
			HWND ProgmanHwnd = FindWindow("Progman", "Program Manager");
			if (ProgmanHwnd && GetParent(m_Window) != ProgmanHwnd)
			{
				parent = ProgmanHwnd;
				style = (oldStyle & ~WS_POPUP) | WS_CHILD;
			}
		}
	}

	if (!m_Rainlendar->IsWharf())
	{
		if (style != oldStyle)
		{
			SetWindowLong(m_Window, GWL_STYLE, style);
			SetParent(m_Window, parent);
		}
	}

	SetWindowPos(m_Window, winPos, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);

	c_Config.SetWindowPos(pos);	// Set the config to match the chosen value
}

/*
** UpdateTransparency
**
** Updates the native Windows transparency
*/
void CCalendarWindow::UpdateTransparency()
{
	if (Is2k())
	{
		if (c_Config.GetNativeTransparency())
		{
			// Add the window flag
			LONG style = GetWindowLong(m_Window, GWL_EXSTYLE);
			SetWindowLong(m_Window, GWL_EXSTYLE, style | WS_EX_LAYERED);

			RECT r;
			GetWindowRect(m_Window, &r);

			typedef BOOL (WINAPI * FPUPDATELAYEREDWINDOW)(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);
			HINSTANCE h = LoadLibrary("user32.dll");
			FPUPDATELAYEREDWINDOW UpdateLayeredWindow = (FPUPDATELAYEREDWINDOW)GetProcAddress(h, "UpdateLayeredWindow");

			BLENDFUNCTION blendPixelFunction= {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
			POINT ptWindowScreenPosition = {r.left, r.top};
			POINT ptSrc = {0, 0};
			SIZE szWindow = {m_Width, m_Height};
			
			HDC dcScreen = GetDC(GetDesktopWindow());
			HDC dcMemory = CreateCompatibleDC(dcScreen);
			HBITMAP oldBitmap = (HBITMAP)SelectObject(dcMemory, m_DoubleBuffer.GetBitmap());
			UpdateLayeredWindow(m_Window, dcScreen, &ptWindowScreenPosition, &szWindow, dcMemory, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
			ReleaseDC(GetDesktopWindow(), dcScreen);
			SelectObject(dcMemory, oldBitmap);
			DeleteDC(dcMemory);
			FreeLibrary(h);
		}
		else
		{
			// Remove the window flag
			LONG style = GetWindowLong(m_Window, GWL_EXSTYLE);
			SetWindowLong(m_Window, GWL_EXSTYLE, style & ~WS_EX_LAYERED);
		}
	}
}

/*
** Is2k
**
** Returns true if ew're running modern OS.
**
*/
bool CCalendarWindow::Is2k()
{
	OSVERSIONINFO version;
	version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&version);

	if (version.dwPlatformId == VER_PLATFORM_WIN32_NT && version.dwMajorVersion > 4)
	{
		return true;
	}
	return false;
}

/*
** ReadSkins
**
** Scans all the subfolders and locates the ini-files.
**
*/
void CCalendarWindow::ReadSkins()
{
    WIN32_FIND_DATA fileData;      // Data structure describes the file found
    WIN32_FIND_DATA fileDataIni;   // Data structure describes the file found
    HANDLE hSearch;                // Search handle returned by FindFirstFile
    HANDLE hSearchIni;             // Search handle returned by FindFirstFile

	m_ConfigStrings.clear();

    // Start searching for .ini files in the given directory.
	std::string files = c_Config.GetSkinsPath() + "*";
    hSearch = FindFirstFile(files.c_str(), &fileData);
	do
	{
		if(hSearch == INVALID_HANDLE_VALUE) break;    // No more files found

		if(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && 
			strcmp(".", fileData.cFileName) != 0 &&
			strcmp("..", fileData.cFileName) != 0)
		{
			CONFIG config;
			config.path = fileData.cFileName;

			// Scan all .ini files from the subfolder
			std::string inis = c_Config.GetSkinsPath();
			inis += fileData.cFileName;
			inis += "\\*.ini";
			hSearchIni = FindFirstFile(inis.c_str(), &fileDataIni);

			do
			{
				if(hSearchIni == INVALID_HANDLE_VALUE) break;    // No more files found
				config.iniFiles.push_back(fileDataIni.cFileName);

			} while (FindNextFile(hSearchIni, &fileDataIni));

			if (!config.iniFiles.empty())
			{
				m_ConfigStrings.push_back(config);
			}
		    FindClose(hSearchIni);
		}
	} while(FindNextFile(hSearch, &fileData));

    FindClose(hSearch);

	// Scan the read ini files and make sure that the current skin is available
	bool found = false;
	std::vector<CONFIG>::iterator i = m_ConfigStrings.begin();
	for( ; i != m_ConfigStrings.end() && !found; i++)
	{
		if ( (*i).path == c_Config.GetCurrentSkin())
		{
			std::vector<std::string>::iterator j = (*i).iniFiles.begin();
			for( ; j != (*i).iniFiles.end(); j++)
			{
				if ( (*j) == c_Config.GetCurrentSkinIni())
				{
					found = true;
					break;
				}
			}
		}
	}

	if (!found)
	{
		if (m_ConfigStrings.empty())
		{
			c_Config.SetCurrentSkin("");
			c_Config.SetCurrentSkinIni("");
		}
		else
		{
			// Just use the first available
			char buffer[MAX_LINE_LENGTH];
			//	"Unable to find the skin: %s\\%s\nRainlendar will default to the first available skin.",
			sprintf(buffer, CCalendarWindow::c_Language.GetString("Message", 7), c_Config.GetCurrentSkin().c_str(), c_Config.GetCurrentSkinIni().c_str());
			MessageBox(m_Window, buffer, "Rainlendar", MB_OK | MB_ICONWARNING);

			c_Config.SetCurrentSkin(m_ConfigStrings[0].path);
			c_Config.SetCurrentSkinIni(m_ConfigStrings[0].iniFiles[0]);
			c_Config.WriteConfig(CConfig::WRITE_CONFIG);
		}
	}
}

/* 
** PollWallpaper
**
** Checks if the name of the wallpaper has been changed and refreshes if it has
**
*/
void CCalendarWindow::PollWallpaper(bool set)
{
	if (c_Config.GetPollWallpaper())
	{
		// Check if the wallpaper is different that what it was when we refresed last time
		DWORD size = 256;
		char str[256];
		HKEY hKey = NULL;

		// Get the wallpaper name from registry
		if(RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
		{
			if(RegQueryValueEx(hKey, "Wallpaper", NULL, NULL, (LPBYTE)&str, (LPDWORD)&size) == ERROR_SUCCESS)
			{
				if (set)
				{
					m_WallpaperName = str;
					// Get the timestamp from the file too

					HANDLE file = CreateFile(m_WallpaperName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (file != INVALID_HANDLE_VALUE)
					{
						GetFileTime(file, NULL, NULL, &m_WallpaperTime);
						CloseHandle(file);
					}
				}
				else
				{
					if (m_WallpaperName != str)
					{
						// If the names differ, refresh
						Refresh();
					}
					else
					{
						// Check the timastamp
						FILETIME newTime;
						HANDLE file = CreateFile(m_WallpaperName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
						if (file != INVALID_HANDLE_VALUE)
						{
							GetFileTime(file, NULL, NULL, &newTime);
							CloseHandle(file);

							if (newTime.dwHighDateTime != m_WallpaperTime.dwHighDateTime || newTime.dwLowDateTime != m_WallpaperTime.dwLowDateTime)
							{
								Refresh();
							}
						}
					}
				}
			}
			RegCloseKey(hKey);
		}
	}
}

/* 
** ToggleWindow
**
** Toggles window visibility 
**
*/
void CCalendarWindow::ToggleWindow()
{
	if(!m_Hidden) 
	{
		HideWindow();
	} 
	else 
	{
		ShowWindow();
	}
}

/* 
** ChangeMonth
**
** Show the given month in the window
**
*/
void CCalendarWindow::ChangeMonth(int Month, int Year)
{
	if(Month == -1) 
	{
		Month = c_TodaysDate.wMonth;	
	}
	if(Year==-1) 
	{
		Year = c_TodaysDate.wYear;	
	}

	c_MonthsFirstDate.wYear = Year;
	c_MonthsFirstDate.wMonth = Month;
	c_MonthsFirstDate.wDay = 1;

	// We must also calculate the correct day of the week
	FILETIME today, newday;
	LONGLONG large_today, large_newday, diff;
	SystemTimeToFileTime(&c_TodaysDate, &today);
	SystemTimeToFileTime(&c_MonthsFirstDate, &newday);
	large_today = today.dwHighDateTime;
	large_today <<= 32;
	large_today += today.dwLowDateTime;
	large_newday = newday.dwHighDateTime;
	large_newday <<= 32;
	large_newday += newday.dwLowDateTime;
	diff = (large_newday - large_today) / 864000000000;

	int day = (c_TodaysDate.wDayOfWeek + diff) % 7;
	if (day < 0) day += 7;
	c_MonthsFirstDate.wDayOfWeek = day;
}

/* 
** DrawCalendar
**
** Draws the calendar to DoubleBuffer
**
*/
void CCalendarWindow::DrawCalendar()
{
	int thisMonth = c_MonthsFirstDate.wMonth;
	int thisYear = c_MonthsFirstDate.wYear;

	int startMonth = thisMonth - c_Config.GetPreviousMonths();
	int startYear = thisYear;

	while (startMonth <= 0)
	{
		startYear--;
		startMonth += 12;
	}

	CToolTip::Instance().DeleteAllToolTips();

	if(m_DoubleBuffer.GetBitmap()) 
	{
		m_Background.Paint(m_DoubleBuffer);

		POINT offset; 

		for (int j = 0; j < c_Config.GetVerticalCount(); j++)
		{
			offset.y = j * m_Height / c_Config.GetVerticalCount();

			for (int i = 0; i < c_Config.GetHorizontalCount(); i++)
			{
				offset.x = i * m_Width / c_Config.GetHorizontalCount();

				ChangeMonth(startMonth, startYear);
				startMonth++;
				if (startMonth == 13)
				{
					startYear++;
					startMonth = 1;
				}

				// Reset the tooltips for the new month(s) (this also removes the old tips)
				m_Event->AddToolTips(this, offset);

				if(c_Config.GetYearEnable()) m_Year->Paint(m_DoubleBuffer, offset);
				if(c_Config.GetMonthEnable()) m_Month->Paint(m_DoubleBuffer, offset);

				if(c_Config.GetDaysEnable()) 
				{
					m_Days->Paint(m_DoubleBuffer, offset);
					if(c_Config.GetEventEnable()) m_Event->Paint(m_DoubleBuffer, offset);
					if(c_Config.GetWeekdaysEnable()) m_Weekdays->Paint(m_DoubleBuffer, offset);
					if(c_Config.GetWeekNumbersEnable()) m_WeekNumbers->Paint(m_DoubleBuffer, offset);
					if(c_Config.GetTodayEnable()) m_Today->Paint(m_DoubleBuffer, offset);
				}
			}
		}

		if (CRasterizerFont::GetColorBuffer().GetBitmap() != NULL  && CRasterizerFont::GetAlphaBuffer().GetBitmap() != NULL)
		{
			CImage textImage;
			textImage.Create(CRasterizerFont::GetColorBuffer().GetBitmap(), CRasterizerFont::GetAlphaBuffer().GetBitmap(), 0);
			m_DoubleBuffer.Blit(textImage, 0, 0, 0, 0, textImage.GetWidth(), textImage.GetHeight());
		}
	}

	// Reset the values
	ChangeMonth(thisMonth, thisYear);
}

/////////////////////////////////////////////////////////////////////////////
// CCalendarWindow message handlers

LRESULT CCalendarWindow::OnPaint(WPARAM wParam, LPARAM lParam) 
{
	if(m_DoubleBuffer.GetBitmap()) 
	{
		PAINTSTRUCT ps;
		HDC winDC = BeginPaint(m_Window, &ps);
		HDC tmpDC = CreateCompatibleDC(NULL);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(tmpDC, m_DoubleBuffer.GetBitmap());

		BitBlt(winDC, 0, 0, m_Width, m_Height, tmpDC, 0, 0, SRCCOPY);

		SelectObject(tmpDC, oldBitmap);
		DeleteDC(tmpDC);

		EndPaint(m_Window, &ps);
	}
	return 0;
}

LRESULT CCalendarWindow::OnContextMenu(WPARAM wParam, LPARAM lParam) 
{
	int xPos = (SHORT)LOWORD(lParam); 
	int yPos = (SHORT)HIWORD(lParam); 

	// Transform the point to client rect
	RECT rect;
	GetWindowRect(m_Window, &rect);
	xPos = xPos - rect.left;
	yPos = yPos - rect.top;

	HMENU menu, subMenu = NULL;
	
	menu = LoadMenu(CRainlendar::GetInstance(), MAKEINTRESOURCE(IDR_MENU1));
	if (menu) subMenu = GetSubMenu(menu, 0);

	if (subMenu)
	{
		FillMenu(subMenu, xPos, yPos);

		TrackPopupMenu(
		  subMenu,
		  TPM_RIGHTBUTTON | TPM_LEFTALIGN, 
		  (SHORT)LOWORD(lParam),
		  (SHORT)HIWORD(lParam),
		  0,
		  m_Window,
		  NULL
		);		
		DestroyMenu(subMenu);
	}
	DestroyMenu(menu);

	return 0;
}

LRESULT CCalendarWindow::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	try 
	{
		bool Handled = false;
		bool Selected = false;
		int Month, Year;

		if(wParam >= ID_LANGUAGE && wParam < ID_LANGUAGE_END)
		{
			if (wParam == ID_LANGUAGE)
			{
				c_Language.SetDefaultLanguage();
				c_Config.SetCurrentLanguage("");
			}
			else
			{
				c_Language.ChangeLanguage(wParam - ID_LANGUAGE - 1);
				c_Config.SetCurrentLanguage(c_Language.GetLanguages()[wParam - ID_LANGUAGE - 1]);
			}
			c_Config.WriteConfig(CConfig::WRITE_CONFIG);
			Refresh();
			return 0;
		}

		if(wParam >= ID_SKIN && wParam < ID_SKIN_END)
		{
			// Check which config was selected
			int index = 0;
			
			for (int i = 0; i < m_ConfigStrings.size(); i++)
			{
				for (int j = 0; j < m_ConfigStrings[i].iniFiles.size(); j++)
				{
					if (index == wParam - ID_SKIN)
					{
						c_Config.SetCurrentSkin(m_ConfigStrings[i].path);
						c_Config.SetCurrentSkinIni(m_ConfigStrings[i].iniFiles[j]);
						c_Config.WriteConfig(CConfig::WRITE_CONFIG);	// Write old config (happens only if Rainlendar.ini's not modified from outside)
						Refresh();
						return 0;
					}
					index++;
				}
			}
			return 0;
		}

		switch(LOWORD(wParam)) {

		case ID_ABOUT:
			{
				char buffer[MAX_LINE_LENGTH];
				sprintf(buffer, c_Language.GetString("About", 0), VERSION, __DATE__);
				MessageBox(m_Window, buffer, "Rainlendar", MB_OK);
			}
			break;

		case ID_REFRESH:
			Refresh();
			break;

		case ID_POPUP_UPDATEOUTLOOK:
			SyncWithOutlook();
			Refresh();
			break;

		case ID_CONFIG:
			LSLog(LOG_DEBUG, "Rainlendar", "Opening Config dialog.");
			OpenConfigDialog(m_Window, CRainlendar::GetInstance());
			LSLog(LOG_DEBUG, "Rainlendar", "Config dialog closed.");
			break;
	
		case ID_EDIT_SKIN:
			LSLog(LOG_DEBUG, "Rainlendar", "Opening Edit Skin dialog.");
			OpenSkinDialog(m_Window, CRainlendar::GetInstance());
			LSLog(LOG_DEBUG, "Rainlendar", "Edit Skin dialog closed.");
			break;
	
		case ID_SETEVENT:
			// Fake doubleclick action will show the edit event dialog
			m_SelectedDate = m_MenuSelectedDate;
			OnLButtonDblClk(0, 0);
			m_MenuSelectedDate = 0;
			break;

		case ID_ALLEVENTS:
			LSLog(LOG_DEBUG, "Rainlendar", "Opening All Events dialog.");
			if (DialogBox(CRainlendar::GetInstance(), MAKEINTRESOURCE(IDD_ALLEVENTS), m_Window, (DLGPROC)AllEventsProc) == IDOK) 
			{
				ConnectServer(REQUEST_SENDEVENTS);	// Send the events to the server
				Refresh(true);
			}
			LSLog(LOG_DEBUG, "Rainlendar", "All Events dialog closed.");
			break;

		case ID_QUIT:
			CToolTip::Instance().Finalize();
			if(CRainlendar::GetDummyLitestep()) PostQuitMessage(0);
			DestroyWindow(m_Window);
			break;

		case ID_POPUP_SELECTMONTH_CURRENTMONTH:
			Month = c_TodaysDate.wMonth;
			Year = c_TodaysDate.wYear;
			Handled = true;
			break;

		case ID_POPUP_SELECTMONTH_NEXTMONTH:
			Month = c_MonthsFirstDate.wMonth + 1;
			if (Month == 13) 
			{
				Year = c_MonthsFirstDate.wYear + 1;
				Month = 1;
			}
			else 
			{
				Year = c_MonthsFirstDate.wYear;
			}
			Handled = true;
			break;

		case ID_POPUP_SELECTMONTH_PREVMONTH:
			Month = c_MonthsFirstDate.wMonth - 1;
			if(Month == 0) 
			{
				Year = c_MonthsFirstDate.wYear - 1;
				Month = 12;
			}
			else 
			{
				Year = c_MonthsFirstDate.wYear;
			}
			Handled = true;
			break;

		case ID_POPUP_SELECTMONTH_NEXTYEAR:
			Month = c_MonthsFirstDate.wMonth;
			Year = c_MonthsFirstDate.wYear + 1;
			Handled = true;
			break;

		case ID_POPUP_SELECTMONTH_PREVYEAR:
			Month = c_MonthsFirstDate.wMonth;
			Year = max(0, c_MonthsFirstDate.wYear - 1);
			Handled = true;
			break;

		case ID_POPUP_SELECTMONTH_JANUARY:
			Month = 1;
			Selected = true;
			break;

		case ID_POPUP_SELECTMONTH_FEBRUARY:
			Month = 2;
			Selected = true;
			break;

		case ID_POPUP_SELECTMONTH_MARCH:
			Month = 3;
			Selected = true;
			break;

		case ID_POPUP_SELECTMONTH_APRIL:
			Month = 4;
			Selected = true;
			break;

		case ID_POPUP_SELECTMONTH_MAY:
			Month = 5;
			Selected = true;
			break;

		case ID_POPUP_SELECTMONTH_JUNE:
			Month = 6;
			Selected = true;
			break;

		case ID_POPUP_SELECTMONTH_JULY:
			Month = 7;
			Selected = true;
			break;

		case ID_POPUP_SELECTMONTH_AUGUST:
			Month = 8;
			Selected = true;
			break;

		case ID_POPUP_SELECTMONTH_SEPTEMBER:
			Month = 9;
			Selected = true;
			break;

		case ID_POPUP_SELECTMONTH_OCTOBER:
			Month = 10;
			Selected = true;
			break;

		case ID_POPUP_SELECTMONTH_NOVEMBER:
			Month = 11;
			Selected = true;
			break;

		case ID_POPUP_SELECTMONTH_DECEMBER:
			Month = 12;
			Selected = true;
			break;

		}

		if(Selected) 
		{
			Year = c_TodaysDate.wYear + 1;
			if(Month >= c_TodaysDate.wMonth) Year--;
		}

		if(Handled || Selected) 
		{
			ChangeMonth(Month, Year);
			Refresh(true);
		}
	} 
    catch(CError& error) 
    {
		MessageBox(m_Window, error.GetString().c_str(), "Rainlendar", MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
	}

	return 0;
}

void CCalendarWindow::ConnectServer(int type)
{
	static NetworkParams param;

	if (c_Config.GetServerEnable() && !c_Config.GetServerAddress().empty())
	{
		// Here is a slight possibility of unsafe thread access
		// If the OnServerSync() is started multiple times before the
		// thread has a chance to copy the information, the data might
		// corrupt. Hopefully this doesn't occur too often :-)
		param.serverPort = c_Config.GetServerPort();
		param.serverAddress = c_Config.GetServerAddress();
		param.userID = c_Config.GetServerID();
		param.requestType = type;
		param.window = m_Window;

		// Launch the network communication thread
		DWORD id;
		HANDLE thread = CreateThread(NULL, 0, NetworkThreadProc, &param, 0, &id);
		CloseHandle(thread);
	}
}

LRESULT CCalendarWindow::OnTimer(WPARAM wParam, LPARAM lParam) 
{
	if(wParam == DATE_TIMER) 
	{
		ShowWindowIfAppropriate();

		SYSTEMTIME today;
		GetLocalTime(&today);

		if( c_TodaysDate.wYear  != today.wYear ||
			c_TodaysDate.wMonth != today.wMonth ||
			c_TodaysDate.wDay   != today.wDay)
		{
			// Day changed!
			GetLocalTime(&c_TodaysDate);
			// Store current date (the month that will be shown)
			c_MonthsFirstDate = c_TodaysDate;

			ChangeMonth(-1, -1);
			Refresh(true);
			ShowEventMessage();
		}

		if (c_Config.GetServerFrequency() > 0)
		{
			m_ConnectionCounter++;
			if (m_ConnectionCounter >= c_Config.GetServerFrequency() * 60)
			{
				m_ConnectionCounter = 0;
				ConnectServer(REQUEST_GETEVENTS);	// Get the events from server
			}
		}

		PollWallpaper(false);
		m_Background.FlushWallpaper();		// Get rid of the wallpaper

		return 0;
	}
	else if(wParam == OUTLOOK_TIMER) 
	{
        if (SyncWithOutlook())
        {
            Refresh(true);
        }
    }

	return DefWindowProc(m_Window, WM_TIMER, wParam, lParam);
}

/*
** OnGetRevID
**
** Litestep revision control. This string is shown in the Litestep's about box.
** Not rcs-style but who cares
**
*/
LRESULT CCalendarWindow::OnGetRevID(WPARAM wParam, LPARAM lParam) 
{
	char* Buffer=(char*)lParam;

	if(Buffer != NULL)
	{
		if(wParam==0) 
		{
			sprintf(Buffer, "Rainlendar.dll: %s", VERSION);
		} 
		else if(wParam==1) 
		{
			sprintf(Buffer, "Rainlendar.dll: %s %s, Rainy", VERSION, __DATE__);
		} 
		else
		{
			Buffer[0] = 0;
		}

		return strlen(Buffer);
	}

	return 0;
}

/*
** OnWindowPosChanging
**
** Called when windows position is about to change
**
*/
LRESULT CCalendarWindow::OnWindowPosChanging(WPARAM wParam, LPARAM lParam) 
{
	LPWINDOWPOS wp=(LPWINDOWPOS)lParam;

	if(c_Config.GetWindowPos() == CConfig::WINDOWPOS_ONBOTTOM && !m_Refreshing)
	{
		// do not change the z-order. This keeps the window on bottom.
		wp->flags|=SWP_NOZORDER;
	}

	if (c_Config.GetSnapEdges() && !(GetKeyState(VK_CONTROL) & 0x8000 || GetKeyState(VK_SHIFT) & 0x8000))
	{
		RECT workArea;
		GetClientRect(GetDesktopWindow(), &workArea);

		// only process movement (ignore anything without winpos values)
		if(wp->cx != 0 && wp->cy != 0)
		{
			typedef HMONITOR (WINAPI * FPMONITORFROMWINDOW)(HWND wnd, DWORD dwFlags);
			typedef BOOL (WINAPI * FPGETMONITORINFO)(HMONITOR hMonitor, LPMONITORINFO lpmi);
			FPMONITORFROMWINDOW fpMonitorFromWindow;
			FPGETMONITORINFO fpGetMonitorInfo;

			HINSTANCE lib = LoadLibrary("user32.dll");
			
			fpMonitorFromWindow = (FPMONITORFROMWINDOW)GetProcAddress(lib, "MonitorFromWindow");
			fpGetMonitorInfo = (FPGETMONITORINFO)GetProcAddress(lib, "GetMonitorInfoA");

			if (fpMonitorFromWindow && fpGetMonitorInfo)
			{
				HMONITOR hMonitor;
				MONITORINFO mi;

				hMonitor = fpMonitorFromWindow(m_Window, MONITOR_DEFAULTTONULL);

				if(hMonitor != NULL)
				{
					mi.cbSize = sizeof(mi);
					fpGetMonitorInfo(hMonitor, &mi);
					workArea = mi.rcWork;
				}
			}

			FreeLibrary(lib);

			int w = workArea.right - m_Width;
			int h = workArea.bottom - m_Height;

			if((wp->x < SNAPDISTANCE + workArea.left) && (wp->x > workArea.left - SNAPDISTANCE)) wp->x = workArea.left;
			if((wp->y < SNAPDISTANCE + workArea.top) && (wp->y > workArea.top - SNAPDISTANCE)) wp->y = workArea.top;
			if ((wp->x < SNAPDISTANCE + w) && (wp->x > -SNAPDISTANCE + w)) wp->x = w;
			if ((wp->y < SNAPDISTANCE + h) && (wp->y > -SNAPDISTANCE + h)) wp->y = h;
		}
	}

	return DefWindowProc(m_Window, m_Message, wParam, lParam);
}

LRESULT CCalendarWindow::OnMove(WPARAM wParam, LPARAM lParam) 
{
	short int x = LOWORD(lParam);
	short int y = HIWORD(lParam);

	if(c_Config.GetMovable())
	{
		if (c_Config.GetNegativeCoords()) 
		{
			if (x < 0) x = 0;
			if (y < 0) y = 0;
		}

		c_Config.SetX(x);
		c_Config.SetY(y);
		c_Config.WriteConfig(CConfig::WRITE_POS);	// Store the new position to the ini file
	}

	if (!m_FirstExecute && (c_Config.GetBackgroundMode() == CBackground::MODE_COPY || m_Background.HasAlpha()))
	{
		m_Background.UpdateWallpaper(x, y);
		DrawCalendar();
		InvalidateRect(m_Window, NULL, false);
	}

	m_X = x;
	m_Y = y;

	return 0;
}

LRESULT CCalendarWindow::OnSettingsChange(WPARAM wParam, LPARAM lParam) 
{
	// If wallpapaer is changed, refresh
	if(wParam == SPI_SETDESKWALLPAPER) 
	{
		PollWallpaper(true);
		Refresh();
	}	

	return 0;
}

LRESULT CCalendarWindow::OnDisplayChange(WPARAM wParam, LPARAM lParam) 
{
	// Refresh when the resolution changes
	if (c_Config.GetRefreshOnResolutionChange())
	{
		Refresh(true);
	}
	return DefWindowProc(m_Window, m_Message, wParam, lParam);
}

LRESULT CCalendarWindow::OnLButtonDblClk(WPARAM wParam, LPARAM lParam) 
{
	int x = (SHORT)LOWORD(lParam);
	int y = (SHORT)HIWORD(lParam);

	if(m_Message == WM_NCLBUTTONDBLCLK)
	{
		// Transform the point to client rect
		RECT rect;
		GetWindowRect(m_Window, &rect);
		x = x - rect.left;
		y = y - rect.top;
	}

	if (m_SelectedDate == 0)
	{
		m_SelectedDate = m_Days->HitTest(x, y);
	}

	if(m_SelectedDate != 0) 
	{
		LSLog(LOG_DEBUG, "Rainlendar", "Opening Edit Event dialog.");
		OpenEditEventDialog(m_Window, m_Instance, m_SelectedDate, 0);
		LSLog(LOG_DEBUG, "Rainlendar", "Edit Event dialog closed.");

		m_SelectedDate = 0;
	}

	return 0;
}

LRESULT CCalendarWindow::OnKeyUp(WPARAM wParam, LPARAM lParam) 
{
	if(!c_Config.GetDisableHotkeys()) 
	{
		if( (GetAsyncKeyState(VK_SHIFT)) == 0 &&
			(GetAsyncKeyState(VK_CONTROL)) == 0 &&
			(GetAsyncKeyState(VK_MENU)) == 0 &&
			(GetAsyncKeyState(VK_LWIN)) == 0 &&
			(GetAsyncKeyState(VK_RWIN)) == 0) {

			switch(wParam) 
			{
			case VK_LEFT:
				OnCommand(ID_POPUP_SELECTMONTH_PREVMONTH, NULL);
				break;
			case VK_RIGHT:
				OnCommand(ID_POPUP_SELECTMONTH_NEXTMONTH, NULL);
				break;
			case VK_UP:
				OnCommand(ID_POPUP_SELECTMONTH_NEXTYEAR, NULL);
				break;
			case VK_DOWN:
				OnCommand(ID_POPUP_SELECTMONTH_PREVYEAR, NULL);
				break;
			case VK_HOME:
				OnCommand(ID_POPUP_SELECTMONTH_CURRENTMONTH, NULL);
				break;
			case VK_F1:
				OnCommand(ID_POPUP_SELECTMONTH_JANUARY, NULL);
				break;
			case VK_F2:
				OnCommand(ID_POPUP_SELECTMONTH_FEBRUARY, NULL);
				break;
			case VK_F3:
				OnCommand(ID_POPUP_SELECTMONTH_MARCH, NULL);
				break;
			case VK_F4:
				OnCommand(ID_POPUP_SELECTMONTH_APRIL, NULL);
				break;
			case VK_F5:
				OnCommand(ID_POPUP_SELECTMONTH_MAY, NULL);
				break;
			case VK_F6:
				OnCommand(ID_POPUP_SELECTMONTH_JUNE, NULL);
				break;
			case VK_F7:
				OnCommand(ID_POPUP_SELECTMONTH_JULY, NULL);
				break;
			case VK_F8:
				OnCommand(ID_POPUP_SELECTMONTH_AUGUST, NULL);
				break;
			case VK_F9:
				OnCommand(ID_POPUP_SELECTMONTH_SEPTEMBER, NULL);
				break;
			case VK_F10:
				OnCommand(ID_POPUP_SELECTMONTH_OCTOBER, NULL);
				break;
			case VK_F11:
				OnCommand(ID_POPUP_SELECTMONTH_NOVEMBER, NULL);
				break;
			case VK_F12:
				OnCommand(ID_POPUP_SELECTMONTH_DECEMBER, NULL);
				break;
			}
		}
	}
	return 0;
}


LRESULT CCalendarWindow::OnEraseBkgnd(WPARAM wParam, LPARAM lParam) 
{
	return 0;
}


// F10 Generates a syskey-message   ... wierd.
LRESULT CCalendarWindow::OnSysKeyUp(WPARAM wParam, LPARAM lParam) 
{
	if(!c_Config.GetDisableHotkeys()) 
	{
		if( (GetAsyncKeyState(VK_SHIFT)) == 0 &&
			(GetAsyncKeyState(VK_CONTROL)) == 0 &&
			(GetAsyncKeyState(VK_MENU)) == 0 &&
			(GetAsyncKeyState(VK_LWIN)) == 0 &&
			(GetAsyncKeyState(VK_RWIN)) == 0) {

			switch(wParam) 
			{
			case VK_F10:
				OnCommand(ID_POPUP_SELECTMONTH_OCTOBER, NULL);
				break;
			}
		}
	}
	
	return 0;
}

void CCalendarWindow::RegisterHotkeys()
{
	DWORD hotkey;
	DWORD modifiers;
	DWORD vk;

	for (int i = 0; i < CConfig::HOTKEY_LAST; i++)
	{
		switch(i)
		{
		case CConfig::HOTKEY_HIDE:
			hotkey = c_Config.GetHideHotkey();
			break;

		case CConfig::HOTKEY_SHOW:
			hotkey = c_Config.GetShowHotkey();
			break;

		case CConfig::HOTKEY_TOGGLE:
			hotkey = c_Config.GetToggleHotkey();
			break;

		case CConfig::HOTKEY_ACTIVATE:
			hotkey = c_Config.GetActivateHotkey();
			break;

		case CConfig::HOTKEY_REFRESH:
			hotkey = c_Config.GetRefreshHotkey();
			break;

		case CConfig::HOTKEY_CONFIG:
			hotkey = c_Config.GetConfigHotkey();
			break;

		case CConfig::HOTKEY_SKIN:
			hotkey = c_Config.GetSkinHotkey();
			break;

		case CConfig::HOTKEY_NEXT:
			hotkey = c_Config.GetNextHotkey();
			break;

		case CConfig::HOTKEY_PREVIOUS:
			hotkey = c_Config.GetPreviousHotkey();
			break;

		case CConfig::HOTKEY_NEXT_YEAR:
			hotkey = c_Config.GetNextYearHotkey();
			break;

		case CConfig::HOTKEY_PREVIOUS_YEAR:
			hotkey = c_Config.GetPreviousYearHotkey();
			break;

		case CConfig::HOTKEY_CURRENT:
			hotkey = c_Config.GetCurrentHotkey();
			break;

		case CConfig::HOTKEY_ALL:
			hotkey = c_Config.GetAllHotkey();
			break;

		case CConfig::HOTKEY_OUTLOOK:
			hotkey = c_Config.GetOutlookHotkey();
			break;
		}
		
		vk = hotkey & 0xFF;
		modifiers = (hotkey >> 8) & 0xFF;

		if (hotkey)
		{
			RegisterHotKey(m_Window, i, modifiers, vk);
		}
		else
		{
			UnregisterHotKey(m_Window, i);
		}
	}
}

LRESULT CCalendarWindow::OnHotkey(WPARAM wParam, LPARAM lParam)
{
	DWORD hotkey = HIWORD(lParam) + (LOWORD(lParam) << 8);

	if (hotkey == c_Config.GetHideHotkey())
	{
		RainlendarHide(m_Window, NULL);
	}
	else if (hotkey == c_Config.GetShowHotkey())
	{
    	RainlendarShow(m_Window, NULL);
	}
	else if (hotkey == c_Config.GetToggleHotkey())
	{
		RainlendarToggle(m_Window, NULL);
	}
	else if (hotkey == c_Config.GetActivateHotkey())
	{
    	RainlendarShow(m_Window, "Activate");
	}
	else if (hotkey == c_Config.GetRefreshHotkey())
	{
		RainlendarRefresh(m_Window, NULL);
	}
	else if (hotkey == c_Config.GetConfigHotkey())
	{
		RainlendarConfig(m_Window, NULL);
	}
	else if (hotkey == c_Config.GetSkinHotkey())
	{
		ShowEditSkin();
	}
	else if (hotkey == c_Config.GetNextHotkey())
	{
		OnCommand(ID_POPUP_SELECTMONTH_NEXTMONTH, NULL);
	}
	else if (hotkey == c_Config.GetPreviousHotkey())
	{
		OnCommand(ID_POPUP_SELECTMONTH_PREVMONTH, NULL);
	}
	else if (hotkey == c_Config.GetNextYearHotkey())
	{
		OnCommand(ID_POPUP_SELECTMONTH_NEXTYEAR, NULL);
	}
	else if (hotkey == c_Config.GetPreviousYearHotkey())
	{
		OnCommand(ID_POPUP_SELECTMONTH_PREVYEAR, NULL);
	}
	else if (hotkey == c_Config.GetCurrentHotkey())
	{
		OnCommand(ID_POPUP_SELECTMONTH_CURRENTMONTH, NULL);
	}
	else if (hotkey == c_Config.GetAllHotkey())
	{
		OnCommand(ID_ALLEVENTS, NULL); 
	}
	else if (hotkey == c_Config.GetOutlookHotkey())
	{
		OnCommand(ID_POPUP_UPDATEOUTLOOK, NULL); 
	}
	return TRUE;
}

LRESULT CCalendarWindow::OnCopyData(WPARAM wParam, LPARAM lParam)
{
	COPYDATASTRUCT* pCopyDataStruct = (COPYDATASTRUCT*) lParam;

	if (pCopyDataStruct && (pCopyDataStruct->dwData == 1) && (pCopyDataStruct->cbData > 0))
	{
		if (stricmp((const char*)pCopyDataStruct->lpData, "!RainlendarToggle") == 0)
		{
			RainlendarToggle(m_Window, NULL);
		}
		else if (stricmp((const char*)pCopyDataStruct->lpData, "!RainlendarHide") == 0)
		{
			RainlendarHide(m_Window, NULL);
		}
		else if (strnicmp((const char*)pCopyDataStruct->lpData, "!RainlendarShow", 15) == 0)
		{
			// This one takes arguments
			if (strlen((const char*)pCopyDataStruct->lpData) > 16)
			{
    			RainlendarShow(m_Window, (const char*)pCopyDataStruct->lpData + 16);
			}
            else
            {
    			RainlendarShow(m_Window, NULL);
            }
		}
		else if (stricmp((const char*)pCopyDataStruct->lpData, "!RainlendarRefresh") == 0)
		{
			RainlendarRefresh(m_Window, NULL);
		}
		else if (stricmp((const char*)pCopyDataStruct->lpData, "!RainlendarConfig") == 0)
		{
			RainlendarConfig(m_Window, NULL);
		}
		else if (stricmp((const char*)pCopyDataStruct->lpData, "!RainlendarQuit") == 0)
		{
			RainlendarQuit(m_Window, NULL);
		}
		else if (stricmp((const char*)pCopyDataStruct->lpData, "!RainlendarShowNext") == 0)
		{
			RainlendarShowNext(m_Window, NULL);
		}
		else if (stricmp((const char*)pCopyDataStruct->lpData, "!RainlendarShowPrev") == 0)
		{
			RainlendarShowPrev(m_Window, NULL);
		}
		else if (strnicmp((const char*)pCopyDataStruct->lpData, "!RainlendarShowMonth", 20) == 0)
		{
			// This one takes arguments
			if (strlen((const char*)pCopyDataStruct->lpData) > 21)
			{
				RainlendarShowMonth(m_Window, (const char*)pCopyDataStruct->lpData + 21);
			}
            else
            {
				RainlendarShowMonth(m_Window, NULL);
    		}
		}
		else if (stricmp((const char*)pCopyDataStruct->lpData, "!RainlendarShowCurrent") == 0)
		{
			RainlendarShowCurrent(m_Window, NULL);
		}
		else if (stricmp((const char*)pCopyDataStruct->lpData, "!RainlendarLsBoxHook") == 0)
		{
			// Do nothing
		}
		else if (strnicmp((const char*)pCopyDataStruct->lpData, "!RainlendarMove", 15) == 0)
		{
			// This one takes arguments
			if (strlen((const char*)pCopyDataStruct->lpData) > 16)
			{
				RainlendarMove(m_Window, (const char*)pCopyDataStruct->lpData + 16);
			}
		}
		else if (strnicmp((const char*)pCopyDataStruct->lpData, "!RainlendarZPos", 15) == 0)
		{
			// This one takes arguments
			if (strlen((const char*)pCopyDataStruct->lpData) > 16)
			{
				RainlendarZPos(m_Window, (const char*)pCopyDataStruct->lpData + 16);
			}
		}
		else
		{
			char buffer[MAX_LINE_LENGTH];
			sprintf(buffer, CCalendarWindow::c_Language.GetString("Message", 8), (const char*)pCopyDataStruct->lpData);
			MessageBox(m_Window, buffer, "Rainlendar", MB_OK | MB_ICONWARNING);			//	"Unknown !bang: %s",
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}

LRESULT CCalendarWindow::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	if(c_Config.GetMovable())
	{
		return HTCAPTION;
	}

	return DefWindowProc(m_Window, WM_NCHITTEST, wParam, lParam);
}


LRESULT CCalendarWindow::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if(c_Config.GetMouseHide())
	{
		if(GetKeyState(VK_CONTROL) & 0x8000 || GetKeyState(VK_SHIFT) & 0x8000 || GetKeyState(VK_MENU) & 0x8000)
		{
			// If Alt, shift or control is down, do not hide the window
			return 0;
		}

		// Hide window if it is visible
		if(IsWindowVisible(m_Window)) HideWindow();
	}	

	return 0;
}

LRESULT CCalendarWindow::OnServerSyncFinished(WPARAM wParam, LPARAM lParam)
{
    if (wParam == ERROR_CODE_NEWEVENTS)
    {
        Refresh();
    }
    else if (wParam == ERROR_CODE_OUTOFDATE)
    {
		//	"Not all events were updated to the server.\nThe server has more up-to-date.",
        MessageBox(m_Window, CCalendarWindow::c_Language.GetString("Message", 2), "Rainlendar", MB_OK | MB_ICONEXCLAMATION);
    }

	return 0;
}

void CCalendarWindow::ShowWindow(bool activate)
{
	if (activate)
	{
		BringWindowToTop (m_Window);
		::ShowWindow (m_Window, SW_SHOWNORMAL);
		SetForegroundWindow (m_Window);
	}
	else
	{
		::ShowWindow(m_Window, SW_SHOWNOACTIVATE); 
	}
	m_Hidden = false;
}

void CCalendarWindow::ShowWindowIfAppropriate()
{
	if(c_Config.GetMouseHide())
	{
		if(GetKeyState(VK_CONTROL) & 0x8000 || GetKeyState(VK_SHIFT) & 0x8000 || GetKeyState(VK_MENU) & 0x8000)
		{
			// If Alt, shift or control is down, do not show the window
			return;
		}

		bool inside = false;
		POINT pos;
		RECT rect;

		GetCursorPos(&pos);
		GetWindowRect(m_Window, &rect);

		if(rect.left <= pos.x && rect.right >= pos.x &&
		   rect.top <= pos.y && rect.bottom >= pos.y) inside = true;

		if(m_Hidden && !inside)
		{
			ShowWindow();
		}
	}
}

/* 
** WndProc
**
** The window procedure for the calendar
**
*/
LRESULT CALLBACK CCalendarWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CCalendarWindow* Window = NULL;

	if (Window) Window->m_Message = uMsg;

	if(uMsg == WM_CREATE) 
	{
		// Fetch this window-object from the CreateStruct
		Window=(CCalendarWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
	}

	// Run the tooltip's mouse proc
	CToolTip::MouseProc(hWnd, uMsg, wParam, lParam);

	BEGIN_MESSAGEPROC
	MESSAGE(OnPaint, WM_PAINT)
	MESSAGE(OnContextMenu, WM_CONTEXTMENU)
	MESSAGE(OnContextMenu, WM_NCRBUTTONUP)
	MESSAGE(OnGetRevID, LM_GETREVID)
	MESSAGE(OnTimer, WM_TIMER)
	MESSAGE(OnWindowPosChanging, WM_WINDOWPOSCHANGING)
	MESSAGE(OnSettingsChange, WM_SETTINGCHANGE)
	MESSAGE(OnLButtonDblClk, WM_LBUTTONDBLCLK)
	MESSAGE(OnLButtonDblClk, WM_NCLBUTTONDBLCLK)
	MESSAGE(OnEraseBkgnd, WM_ERASEBKGND)
	MESSAGE(OnKeyUp, WM_KEYUP)
	MESSAGE(OnSysKeyUp, WM_SYSKEYUP)
	MESSAGE(OnCommand, WM_COMMAND)
	MESSAGE(OnNcHitTest, WM_NCHITTEST)
	MESSAGE(OnMove, WM_MOVE)
	MESSAGE(OnMouseMove, WM_MOUSEMOVE)
	MESSAGE(OnMouseMove, WM_NCMOUSEMOVE)
	MESSAGE(OnDisplayChange, WM_DISPLAYCHANGE)
	MESSAGE(OnCopyData, WM_COPYDATA)
	MESSAGE(OnServerSyncFinished, WM_SERVER_SYNC_FINISHED)
	MESSAGE(OnHotkey, WM_HOTKEY)

//	case WM_ACTIVATE: 
//	{
//		LSLog(LOG_DEBUG, "Rainlendar", "\n----------------------WM_ACTIVATE ----------------------\n");
//		return DefWindowProc(hWnd, uMsg, wParam, lParam);
//	}

	END_MESSAGEPROC
}

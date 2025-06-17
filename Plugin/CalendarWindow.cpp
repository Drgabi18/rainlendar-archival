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
  $Header: /home/cvsroot/Rainlendar/Plugin/CalendarWindow.cpp,v 1.32 2004/04/24 11:17:20 rainy Exp $

  $Log: CalendarWindow.cpp,v $
  Revision 1.32  2004/04/24 11:17:20  rainy
  Appdata and tray icon fix for Win9x.
  Fixed few bangs.

  Revision 1.31  2004/01/28 18:04:43  rainy
  Added tray executes.
  Fixed bug in path definition.

  Revision 1.30  2004/01/25 09:58:58  rainy
  Fixed refresh.
  Added PLAY to execute.
  Fixed icon drawing.

  Revision 1.29  2004/01/10 15:17:29  rainy
  Added tray and hotkey for todo.
  Fixed message window.

  Revision 1.28  2003/12/20 22:26:57  rainy
  Added more debugging info.
  Removed unnecessary functions.
  Added new message box.

  Revision 1.27  2003/10/27 19:50:20  Rainy
  Derived from CRainWindow.

  Revision 1.26  2003/10/27 17:36:51  Rainy
  Config is now singleton.
  Added todo stuff.

  Revision 1.25  2003/10/04 14:47:54  Rainy
  Languages path is the same as the DLL's
  Skins are rescanned during refresh.
  Added powerbroadcast again since the timer didn't work for some reason.

  Revision 1.24  2003/08/23 09:14:12  Rainy
  Fixed a crash on exit if the calendar isn't initialized.
  Wrong submenu was translated.

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

#include <windows.h>
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
#include "TodoDialog.h"
#include "../Server/EventCombiner.h"
#include <crtdbg.h>
#include <algorithm>

#define ULW_ALPHA               0x00000002
#define WS_EX_LAYERED           0x00080000

// Timers
#define OUTLOOK_TIMER 2

// Defines
#define SNAPDISTANCE 10
#define WM_NOTIFYICON WM_USER + 101

// Class variables
SYSTEMTIME CCalendarWindow::c_TodaysDate;
SYSTEMTIME CCalendarWindow::c_MonthsFirstDate;
CLanguage CCalendarWindow::c_Language;

/* 
** CCalendarWindow
**
** Constructor
**
*/
CCalendarWindow::CCalendarWindow() : CRainWindow()
{
	m_WindowName = "Rainlendar";
	m_ClassName = "Rainlendar";
	m_WndProc = (WNDPROC)CCalendarWindow::WndProc;

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
	RemoveTrayIcon();

	if(m_Window) CConfig::Instance().WriteConfig(CConfig::WRITE_FULL);

	CRasterizerFont::ClearBuffers();

	delete m_Days;
	delete m_Event;
	delete m_Today;
	delete m_Weekdays;
	delete m_WeekNumbers;
	delete m_Month;
	delete m_Year;

	LSLog(LOG_DEBUG, "Rainlendar", "The calendar window is destroyed.");
}

/* 
** Initialize
**
** Initializes the window, creates the class and the window.
**
*/
bool CCalendarWindow::Initialize(HWND Parent, HINSTANCE Instance)
{
	char tmpName[MAX_LINE_LENGTH];

	std::string PixPath, EventsPath, SkinsPath, LanguagesPath, TodoPath;
	std::string skin, skinIni;

	std::string dllPath;
	// Get the DLL's directory
	GetModuleFileName(CRainlendar::GetInstance(), tmpName, MAX_LINE_LENGTH);
	char* Slash = strrchr(tmpName, '\\');
	*(Slash + 1) = 0;	// Cut the Rainlendar.dll from the name
	dllPath = tmpName;

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
			TodoPath = EventsPath + "Todo.ini";
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

		if (PixPath.empty()) 
		{
			std::string filename = dllPath + "Rainlendar.ini";

			// Check if Rainlendar.ini is in the current folder
			FILE* file = fopen(filename.c_str(), "r");
			if (file) 
			{
				fclose(file);
				PixPath = dllPath;
				LSLog(LOG_DEBUG, "Rainlendar", "Found Rainlendar.ini in current folder");
			}
			else
			{
				LSLog(LOG_DEBUG, "Rainlendar", "No Rainlendar.ini in current folder -> Using %APPDATA%.");

				char buffer[MAX_PATH + 1];
				buffer[0] = 0;
				ExpandEnvironmentStrings("%APPDATA%", buffer, MAX_PATH);
				if (strlen(buffer) == 0 || strcmp(buffer, "%APPDATA%") == 0) 
				{
					LSLog(LOG_DEBUG, "Rainlendar", "No %APPDATA% found. Falling back to the old system.");
					PixPath = dllPath;
				}
				else
				{
					PixPath = buffer;
					PixPath += "\\Rainlendar";
					CreateDirectory(PixPath.c_str(), NULL);		// Create the folder for Rainlendar
				}
			}
		}
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

	if (EventsPath.empty())
	{
		EventsPath = PixPath + "Events.ini";
	}

	if (TodoPath.empty())
	{
		TodoPath = PixPath + "Todo.ini";
	}

	if (SkinsPath.empty())
	{
		SkinsPath = dllPath + "Skins\\";
	}

	if (LanguagesPath.empty())
	{
		LanguagesPath = dllPath + "Languages\\";
	}
	else if (*(LanguagesPath.end() - 1) != '\\')
	{
		LanguagesPath += "\\";
	}
	c_Language.ScanLanguages(LanguagesPath.c_str());

	LSLog(LOG_DEBUG, "Rainlendar", "Rainlendar paths:");
	LSLog(LOG_DEBUG, "Rainlendar", PixPath.c_str());
	LSLog(LOG_DEBUG, "Rainlendar", EventsPath.c_str());
	LSLog(LOG_DEBUG, "Rainlendar", SkinsPath.c_str());
	LSLog(LOG_DEBUG, "Rainlendar", LanguagesPath.c_str());
	LSLog(LOG_DEBUG, "Rainlendar", TodoPath.c_str());

	CConfig::Instance().SetPath(PixPath);
	CConfig::Instance().SetEventsPath(EventsPath);	
	CConfig::Instance().SetSkinsPath(SkinsPath);
	CConfig::Instance().ReadConfig();

	if (CRainlendar::GetDummyLitestep())
	{
		LSLog(LOG_DEBUG, "Rainlendar", "Running as stand-alone.");
	}
	else
	{
		LSLog(LOG_DEBUG, "Rainlendar", "Running as Litestep plugin.");
	}

	if (!CConfig::Instance().GetUseWindowName())
	{
		m_WindowName = NULL;
	}

	m_X = CConfig::Instance().GetX();
	m_Y = CConfig::Instance().GetY();

	bool res = CRainWindow::Initialize(Parent, Instance);

	INITCOMMONCONTROLSEX cce;
	cce.dwSize = sizeof(INITCOMMONCONTROLSEX);
	cce.dwICC = ICC_DATE_CLASSES;
	InitCommonControlsEx(&cce);

	CToolTip::Instance().Initialize(m_Window, m_Instance);

	// If the skin was defined explicitely, let's use it
	if (!skin.empty() && !skinIni.empty())
	{
		CConfig::Instance().SetCurrentSkin(skin);
		CConfig::Instance().SetCurrentSkinIni(skinIni);
		CConfig::Instance().WriteConfig(CConfig::WRITE_CONFIG);	// Replace the current config with the step.rc values
	}

	// Scan the skin folder for skins
	ReadSkins();

	GetLocalTime(&c_TodaysDate);
	// Store current date (the month that will be shown)
	c_MonthsFirstDate = c_TodaysDate;

	m_TodoWindow.GetTodoManager().Initialize(TodoPath.c_str());
	m_TodoWindow.Initialize(m_Window, m_Instance);

	m_MessageWindow.Initialize(m_Window, m_Instance);
	
	ChangeMonth(-1, -1);
	Refresh(false);
	
	if(!CConfig::Instance().GetStartHidden()) 
	{
		ShowWindow(false);
		if (CConfig::Instance().GetTodoEnable()) 
		{
			m_TodoWindow.ShowWindow(false);
		}
	}

	if (CConfig::Instance().GetServerStartup())
	{
		ConnectServer(REQUEST_GETEVENTS);	// Get the events from server
	}

	return res;
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
	ModifyMenu(subMenu, ID_ABOUT, MF_BYCOMMAND | MF_STRING, ID_ABOUT, c_Language.GetString("Menu", 0));	// About
	ModifyMenu(subMenu, ID_POPUP_HELP, MF_BYCOMMAND | MF_STRING, ID_POPUP_HELP, c_Language.GetString("Menu", 37));	// Help
	ModifyMenu(subMenu, ID_REFRESH, MF_BYCOMMAND | MF_STRING, ID_REFRESH, c_Language.GetString("Menu", 17));	// Refresh
	ModifyMenu(subMenu, ID_POPUP_UPDATEOUTLOOK, MF_BYCOMMAND | MF_STRING, ID_POPUP_UPDATEOUTLOOK, c_Language.GetString("Menu", 28));	// Update Outlook
	ModifyMenu(subMenu, ID_CONFIG, MF_BYCOMMAND | MF_STRING, ID_CONFIG, c_Language.GetString("Menu", 20));	// Config
	ModifyMenu(subMenu, ID_SETEVENT, MF_BYCOMMAND | MF_STRING, ID_SETEVENT, c_Language.GetString("Menu", 18));	// Set event...
	ModifyMenu(subMenu, ID_ALLEVENTS, MF_BYCOMMAND | MF_STRING, ID_ALLEVENTS, c_Language.GetString("Menu", 19));	// Show all events...
	ModifyMenu(subMenu, ID_TODO, MF_BYCOMMAND | MF_STRING, ID_TODO, c_Language.GetString("Menu", 29));	// Show todo list...
	ModifyMenu(subMenu, ID_QUIT, MF_BYCOMMAND | MF_STRING, ID_QUIT, c_Language.GetString("Menu", 21));	// Quit

	// Submenus
	ModifyMenu(subMenu, 3, MF_BYPOSITION | MF_STRING, 0, c_Language.GetString("Menu", 25));	// Skins
	ModifyMenu(subMenu, 4, MF_BYPOSITION | MF_STRING, 0, c_Language.GetString("Menu", 22));	// Language
	ModifyMenu(subMenu, 5, MF_BYPOSITION | MF_STRING, 0, c_Language.GetString("Menu", 1));	// Select Month

	HMENU monthsMenu = GetSubMenu(subMenu, 5);
	if (monthsMenu)
	{
		ModifyMenu(monthsMenu, ID_POPUP_SELECTMONTH_NEXTMONTH, MF_BYCOMMAND | MF_STRING, ID_POPUP_SELECTMONTH_NEXTMONTH, c_Language.GetString("Menu", 2));	// Next Month
		ModifyMenu(monthsMenu, ID_POPUP_SELECTMONTH_PREVMONTH, MF_BYCOMMAND | MF_STRING, ID_POPUP_SELECTMONTH_PREVMONTH, c_Language.GetString("Menu", 3));	// Prev Month
		ModifyMenu(monthsMenu, ID_POPUP_SELECTMONTH_NEXTYEAR, MF_BYCOMMAND | MF_STRING, ID_POPUP_SELECTMONTH_NEXTYEAR, c_Language.GetString("Menu", 26));	// Next Year
		ModifyMenu(monthsMenu, ID_POPUP_SELECTMONTH_PREVYEAR, MF_BYCOMMAND | MF_STRING, ID_POPUP_SELECTMONTH_PREVYEAR, c_Language.GetString("Menu", 27));	// Prev Year
		ModifyMenu(monthsMenu, ID_POPUP_SELECTMONTH_CURRENTMONTH, MF_BYCOMMAND | MF_STRING, ID_POPUP_SELECTMONTH_CURRENTMONTH, c_Language.GetString("Menu", 4));	// Current Month

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
	m_MenuSelectedDate = 0;
	if (x <= m_Width && y <= m_Height) 
	{
		m_MenuSelectedDate = m_Days->HitTest(x, y);
	}

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

	if (!CConfig::Instance().GetShowOutlookAppointments())
	{
		EnableMenuItem( subMenu, ID_POPUP_UPDATEOUTLOOK, MF_GRAYED);
	}

	int index = 0;

	// Fill the menu with all the configs
	HMENU configMenu = GetSubMenu(subMenu, 3);
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

					if(m_ConfigStrings[i].path == CConfig::Instance().GetCurrentSkin() && m_ConfigStrings[i].iniFiles[j] == CConfig::Instance().GetCurrentSkinIni())
					{
						CheckMenuItem(iniMenu, j, MF_BYPOSITION | MF_CHECKED);
					}
				}
			}
			else
			{
				InsertMenu(configMenu, i + 2, MF_BYPOSITION, ID_SKIN + index++, m_ConfigStrings[i].path.c_str());

				if(m_ConfigStrings[i].path == CConfig::Instance().GetCurrentSkin())
				{
					CheckMenuItem(configMenu, i + 2, MF_BYPOSITION | MF_CHECKED);
				}
			}
		}
	}

	// Fill the menu with all the configs
	HMENU languageMenu = GetSubMenu(subMenu, 4);
	if(languageMenu)
	{
		bool languageFound = false;
		ModifyMenu(languageMenu, 0, MF_BYPOSITION | MF_STRING, GetMenuItemID(languageMenu, 0), c_Language.GetString("Menu", 23));	// English

		for(int i = 0; i < c_Language.GetLanguages().size(); i++)
		{
			InsertMenu(languageMenu, i + 1, MF_BYPOSITION, ID_LANGUAGE + i + 1, c_Language.GetLanguages()[i].c_str());

			if(c_Language.GetLanguages()[i] == CConfig::Instance().GetCurrentLanguage())
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
SIZE CCalendarWindow::CalcWindowSize()
{
	SIZE size;

	size.cx = size.cy = 0;

	if(CConfig::Instance().GetDaysEnable()) 
	{
		size.cx = max(size.cx, m_Days->GetX() + m_Days->GetW());
		size.cy = max(size.cy, m_Days->GetY() + m_Days->GetH());
	}
	if(CConfig::Instance().GetWeekdaysEnable()) 
	{
		size.cx = max(size.cx, m_Weekdays->GetX() + m_Weekdays->GetW());
		size.cy = max(size.cy, m_Weekdays->GetY() + m_Weekdays->GetH());
	}
	if(CConfig::Instance().GetWeekNumbersEnable()) 
	{
		size.cx = max(size.cx, m_WeekNumbers->GetX() + m_WeekNumbers->GetW());
		size.cy = max(size.cy, m_WeekNumbers->GetY() + m_WeekNumbers->GetH());
	}
	if(CConfig::Instance().GetMonthEnable()) 
	{
		size.cx = max(size.cx, m_Month->GetX() + m_Month->GetW());
		size.cy = max(size.cy, m_Month->GetY() + m_Month->GetH());
	}
	if(CConfig::Instance().GetYearEnable()) 
	{
		size.cx = max(size.cx, m_Year->GetX() + m_Year->GetW());
		size.cy = max(size.cy, m_Year->GetY() + m_Year->GetH());
	}
	if(CConfig::Instance().GetTodayEnable()) 
	{
		size.cx = max(size.cx, m_Today->GetX() + m_Today->GetW());
		size.cy = max(size.cy, m_Today->GetY() + m_Today->GetH());
	}

	size.cx *= CConfig::Instance().GetHorizontalCount();
	size.cy *= CConfig::Instance().GetVerticalCount();

	// Background already knows about vert & horz counts.
	if(m_Background.GetWidth() > size.cx) size.cx = m_Background.GetWidth();
	if(m_Background.GetHeight() > size.cy) size.cy = m_Background.GetHeight();

	return size;
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
		if (!lite)
		{
			ReadSkins();		// Rescan the skins
			CConfig::Instance().ReadConfig();

			KillTimer(m_Window, OUTLOOK_TIMER);
			if (CConfig::Instance().GetOutlookUpdate() > 0)
			{
				SetTimer(m_Window, OUTLOOK_TIMER, CConfig::Instance().GetOutlookUpdate() * 1000, NULL);	// Outlook appointments update is in secs
			}

			sprintf(buffer, "Skin: %s, Ini: %s", CConfig::Instance().GetCurrentSkin().c_str(), CConfig::Instance().GetCurrentSkinIni().c_str());
			LSLog(LOG_DEBUG, "Rainlendar", buffer);

			// Initialize
			delete m_Days;
			delete m_Event;
			delete m_Today;
			delete m_Weekdays;
			delete m_WeekNumbers;
			delete m_Month;
			delete m_Year;

			m_Days = new CItemDays;
			m_Event = new CItemEvent;
			m_Today = new CItemToday;
			m_Weekdays = new CItemWeekdays;
			m_WeekNumbers = new CItemWeekNumbers;
			m_Month = new CItemMonth;
			m_Year = new CItemYear;

    		m_Background.Initialize();
			m_Days->Initialize();
			m_Event->Initialize();
			m_Today->Initialize();
			m_Weekdays->Initialize();
			m_WeekNumbers->Initialize();
			m_Month->Initialize();
			m_Year->Initialize();


			CToolTip::Instance().SetBackgroundColor(CConfig::Instance().GetToolTipBGColor());
			CToolTip::Instance().SetFont(CConfig::Instance().GetToolTipFont());
			CToolTip::Instance().SetMaxWidth(CConfig::Instance().GetToolTipMaxWidth());

			if (CConfig::Instance().GetTooltipSeparator())
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

			if (CConfig::Instance().GetShowOutlookAppointments())
			{
				InitializeOutlook(m_Window);
			}
			else
			{
				FinalizeOutlook();
			}

			if (m_FirstExecute && CConfig::Instance().GetOutlookAppointmentsAtStartup())
			{
				SyncWithOutlook(m_Window);		// Only sync with first execute
			}
			GetOutlookEvents(GetEventManager());

			// Rescan the languages and re-read the current
			c_Language.ScanLanguages(NULL);	// Use the same path
			bool languageFound = false;
			for (int i = 0; i < c_Language.GetLanguages().size(); i++)
			{
				if (c_Language.GetLanguages()[i] == CConfig::Instance().GetCurrentLanguage())
				{
					c_Language.ChangeLanguage(i);
					languageFound = true;
					break;
				}
			}
			if (!languageFound)
			{
				c_Language.SetDefaultLanguage();
				CConfig::Instance().SetCurrentLanguage("");
			}

			m_X = CConfig::Instance().GetX();
			m_Y = CConfig::Instance().GetY();
		}

		CRainWindow::RefreshBegin(lite);

		if (!lite)
		{
			BackgroundCreateStruct bcs;
			bcs.pos.x = m_X;
			bcs.pos.y = m_Y;
			bcs.size.cx = m_Width;
			bcs.size.cy = m_Height;
			bcs.filename = CConfig::Instance().GetBackgroundBitmapName();
			bcs.mode = CConfig::Instance().GetBackgroundMode();
			bcs.solidBevel = CConfig::Instance().GetBackgroundBevel();
			bcs.solidColor= CConfig::Instance().GetBackgroundSolidColor();

			m_Background.Create(bcs);

			// The size could be limited by the background, so we need to reset the width and height
			if (m_Width != m_Background.GetWidth() || m_Height != m_Background.GetHeight())
			{
				m_Width = m_Background.GetWidth();
				m_Height = m_Background.GetHeight();
				MoveWindow(m_X, m_Y);	// This also resizes the window
			}

			SetWindowZPos(CConfig::Instance().GetWindowPos());
		}

		RemoveTrayIcon();
		if (CConfig::Instance().GetShowTrayIcon()) 
		{
			AddTrayIcon();
		}

		CRasterizerFont::ClearBuffers();

		CRainWindow::RefreshEnd(lite);

		if (!lite)
		{
			SYSTEMTIME today;
			GetLocalTime(&today);
			std::vector<CEventMessage*> eventList = GetEventManager()->GetEvents(c_TodaysDate.wDay, c_TodaysDate.wMonth, c_TodaysDate.wYear);

			// Mark all until now as shown
			int now = today.wHour * 60 + today.wMinute + CConfig::Instance().GetPreshowTime();
			for (int i = 0; i < eventList.size(); i++)
			{
				if (eventList[i]->GetStartTime() < now)
				{
					eventList[i]->SetShown(true);
				}
			}

			// Execute on event if this was first refresh (meaning that Rainlendar was started)
			if(m_FirstExecute) 
			{ 
				m_FirstExecute=false;
				
				m_MessageWindow.ClearEvents();
				ShowEventMessage(eventList, CConfig::Instance().GetShowAllEvents());
			}
			
			PollWallpaper(true);
		}

		// Refresh the other windows
		m_TodoWindow.Refresh(lite);
		m_MessageWindow.Refresh(lite);
	} 
    catch(CError& error) 
    {
		MessageBox(m_Window, error.GetString().c_str(), APPNAME, MB_OK | MB_TOPMOST | MB_ICONERROR);
		OnCommand(WM_QUIT, NULL);	// Gotta quit
	}

	sprintf(buffer, "Refresh ended (took: %i ms).", GetTickCount() - time);
	LSLog(LOG_DEBUG, "Rainlendar", buffer);
}

//
// Tray icon
//
BOOL CCalendarWindow::AddTrayIcon() 
{ 
    BOOL res = FALSE; 
    NOTIFYICONDATA tnid; 
	
	// Load the tray icon from resources
	CImage trayImage;
	CImage trayNumbersImage;
	if (trayImage.LoadResource(m_Instance, IDB_TRAY))
	{
		// Draw the current date into the bitmap
		if (trayNumbersImage.LoadResource(m_Instance, IDB_TRAY_NUMBERS))
		{
			int w = trayNumbersImage.GetWidth();
			int h = trayNumbersImage.GetHeight() / 10;
			if (c_TodaysDate.wDay > 9)
			{
				// Center the images to 16x16 icon
				trayImage.Blit(trayNumbersImage, (16 - w * 2) / 2, (16 - h) / 2, 0, (c_TodaysDate.wDay / 10) * h, w, h);
				trayImage.Blit(trayNumbersImage, (16 - w * 2) / 2 + w, (16 - h) / 2, 0, (c_TodaysDate.wDay % 10) * h, w, h);
			}
			else
			{
				// Center the image to 16x16 icon
				trayImage.Blit(trayNumbersImage, (16 - w) / 2, (16 - h) / 2, 0, (c_TodaysDate.wDay % 10) * h, w, h);
			}
		}

		OSVERSIONINFO version;
		version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&version);

		HICON trayIcon;

		if (version.dwPlatformId == VER_PLATFORM_WIN32_NT && version.dwMajorVersion > 4)
		{
			ICONINFO iconInfo;
			iconInfo.fIcon = TRUE;
			iconInfo.hbmColor = trayImage.GetBitmap();
			iconInfo.hbmMask = trayImage.GetBitmap();
			trayIcon = CreateIconIndirect(&iconInfo);
		}
		else
		{
			trayIcon = LoadIcon(m_Instance, MAKEINTRESOURCE(IDI_TRAY));
		}

		if (trayIcon)
		{
			tnid.cbSize = sizeof(NOTIFYICONDATA); 
			tnid.hWnd = m_Window; 
			tnid.uID = IDB_TRAY;
			tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
			tnid.uCallbackMessage = WM_NOTIFYICON; 
			tnid.hIcon = trayIcon;
			lstrcpyn(tnid.szTip, "Rainlendar", sizeof(tnid.szTip)); 
			
			res = Shell_NotifyIcon(NIM_ADD, &tnid); 
		}
	}
    return res; 
}

BOOL CCalendarWindow::RemoveTrayIcon() 
{ 
    BOOL res; 
    NOTIFYICONDATA tnid; 
	
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = m_Window; 
    tnid.uID = IDB_TRAY; 
	
    res = Shell_NotifyIcon(NIM_DELETE, &tnid); 
    return res; 
} 


/*
** ShowEventMessage
**
** Opens a message box and displays the message in it. Also executes the command.
*/
void CCalendarWindow::ShowEventMessage(std::vector<CEventMessage*>& eventList, bool messageBox)
{
	if(CConfig::Instance().GetEventEnable()) 
	{
		if (messageBox) 
		{
			m_MessageWindow.ShowEvents(eventList);
		}

		// Do the execute on event
		std::string text;
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
			std::string execute = CConfig::Instance().GetEventExecute();
			
			if(!execute.empty()) 
			{
				if (strnicmp("PLAY ", execute.c_str(), 5) == 0)
				{
					PlaySound(execute.c_str() + 5, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
				}
				else if(!CRainlendar::GetDummyLitestep() || execute[0] != '!') // Do not run bangs if litestep is not enabled
				{
					char buffer[1024];
					VarExpansion(buffer, execute.c_str());
					execute = buffer;
					int pos = execute.find("%m");		// Full text
					if (pos != -1)
					{
						execute.replace(execute.begin() + pos, execute.begin() + pos + 2, text);
					}
					pos = execute.find("%n");			// First 255 chars
					if (pos != -1)
					{
						std::string truncatedText(text.begin(), text.begin() + 255);
						execute.replace(execute.begin() + pos, execute.begin() + pos + 2, truncatedText);
					}
					LSExecute(NULL, execute.c_str(), SW_SHOWNORMAL);
				}
			}
		} 
		
	}
}


/*
** SetWindowZPos
**
** Sets the window's z-position (OnTop, Normal, OnBottom, OnDesktop)
*/
void CCalendarWindow::SetWindowZPos(CConfig::WINDOWPOS pos)
{
	CRainWindow::SetWindowZPos(pos);

	CConfig::Instance().SetWindowPos(pos);	// Set the config to match the chosen value
}

/*
** ReadSkins
**
** Scans all the subfolders and locates the ini-files.
**
*/
bool CompareConfigs(const CCalendarWindow::CONFIG& a, const CCalendarWindow::CONFIG& b) 
{
	return a.path < b.path;
}

void CCalendarWindow::ReadSkins()
{
    WIN32_FIND_DATA fileData;      // Data structure describes the file found
    WIN32_FIND_DATA fileDataIni;   // Data structure describes the file found
    HANDLE hSearch;                // Search handle returned by FindFirstFile
    HANDLE hSearchIni;             // Search handle returned by FindFirstFile

	m_ConfigStrings.clear();

    // Start searching for .ini files in the given directory.
	std::string files = CConfig::Instance().GetSkinsPath() + "*";
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
			std::string inis = CConfig::Instance().GetSkinsPath();
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
		if ( (*i).path == CConfig::Instance().GetCurrentSkin())
		{
			std::vector<std::string>::iterator j = (*i).iniFiles.begin();
			for( ; j != (*i).iniFiles.end(); j++)
			{
				if ( (*j) == CConfig::Instance().GetCurrentSkinIni())
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
			CConfig::Instance().SetCurrentSkin("");
			CConfig::Instance().SetCurrentSkinIni("");
		}
		else
		{
			// Just use the first available
			char buffer[MAX_LINE_LENGTH];
			//	"Unable to find the skin: %s\\%s\nRainlendar will default to the first available skin.",
			sprintf(buffer, CCalendarWindow::c_Language.GetString("Message", 7), CConfig::Instance().GetCurrentSkin().c_str(), CConfig::Instance().GetCurrentSkinIni().c_str());
			MessageBox(m_Window, buffer, "Rainlendar", MB_OK | MB_ICONWARNING);

			CConfig::Instance().SetCurrentSkin(m_ConfigStrings[0].path);
			CConfig::Instance().SetCurrentSkinIni(m_ConfigStrings[0].iniFiles[0]);
			CConfig::Instance().WriteConfig(CConfig::WRITE_CONFIG);
		}
	}

	// Sort
	std::sort(m_ConfigStrings.begin(), m_ConfigStrings.end(), CompareConfigs);
}

/* 
** PollWallpaper
**
** Checks if the name of the wallpaper has been changed and refreshes if it has
**
*/
void CCalendarWindow::PollWallpaper(bool set)
{
	if (CConfig::Instance().GetPollWallpaper())
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
** DrawWindow
**
** Draws the calendar to DoubleBuffer
**
*/
void CCalendarWindow::DrawWindow()
{
	int i, j;
	int thisMonth = c_MonthsFirstDate.wMonth;
	int thisYear = c_MonthsFirstDate.wYear;

	int startMonth = thisMonth - CConfig::Instance().GetPreviousMonths();
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

		for (j = 0; j < CConfig::Instance().GetVerticalCount(); j++)
		{
			offset.y = j * m_Height / CConfig::Instance().GetVerticalCount();

			for (i = 0; i < CConfig::Instance().GetHorizontalCount(); i++)
			{
				offset.x = i * m_Width / CConfig::Instance().GetHorizontalCount();

				ChangeMonth(startMonth, startYear);
				startMonth++;
				if (startMonth == 13)
				{
					startYear++;
					startMonth = 1;
				}

				// Reset the tooltips for the new month(s) (this also removes the old tips)
				m_Event->AddToolTips(this, offset);

				if(CConfig::Instance().GetYearEnable()) m_Year->Paint(m_DoubleBuffer, offset);
				if(CConfig::Instance().GetMonthEnable()) m_Month->Paint(m_DoubleBuffer, offset);

				if(CConfig::Instance().GetDaysEnable()) 
				{
					m_Days->Paint(m_DoubleBuffer, offset);
					if(CConfig::Instance().GetEventEnable()) m_Event->Paint(m_DoubleBuffer, offset);
					if(CConfig::Instance().GetWeekdaysEnable()) m_Weekdays->Paint(m_DoubleBuffer, offset);
					if(CConfig::Instance().GetWeekNumbersEnable()) m_WeekNumbers->Paint(m_DoubleBuffer, offset);
					if(CConfig::Instance().GetTodayEnable()) m_Today->Paint(m_DoubleBuffer, offset);
				}
			}
		}

		if (CRasterizerFont::GetColorBuffer().GetBitmap() != NULL  && CRasterizerFont::GetAlphaBuffer().GetBitmap() != NULL)
		{
			CImage textImage;
			textImage.Create(CRasterizerFont::GetColorBuffer().GetBitmap(), CRasterizerFont::GetAlphaBuffer().GetBitmap(), 0);
			m_DoubleBuffer.Blit(textImage, 0, 0, 0, 0, textImage.GetWidth(), textImage.GetHeight());
		}

		// Draw the icons last so the they will be on top of everything
		startMonth = thisMonth - CConfig::Instance().GetPreviousMonths();
		startYear = thisYear;
		
		while (startMonth <= 0)
		{
			startYear--;
			startMonth += 12;
		}

		for (j = 0; j < CConfig::Instance().GetVerticalCount(); j++)
		{
			offset.y = j * m_Height / CConfig::Instance().GetVerticalCount();
			
			for (i = 0; i < CConfig::Instance().GetHorizontalCount(); i++)
			{
				offset.x = i * m_Width / CConfig::Instance().GetHorizontalCount();
				
				ChangeMonth(startMonth, startYear);
				startMonth++;
				if (startMonth == 13)
				{
					startYear++;
					startMonth = 1;
				}
				if(CConfig::Instance().GetEventEnable()) m_Event->PaintIcons(m_DoubleBuffer, offset);
			}
		}
	}

	// Reset the values
	ChangeMonth(thisMonth, thisYear);
}

void CCalendarWindow::ConnectServer(int type)
{
	static NetworkParams param;

	if (CConfig::Instance().GetServerEnable() && !CConfig::Instance().GetServerAddress().empty())
	{
		// Here is a slight possibility of unsafe thread access
		// If the OnServerSync() is started multiple times before the
		// thread has a chance to copy the information, the data might
		// corrupt. Hopefully this doesn't occur too often :-)
		param.serverPort = CConfig::Instance().GetServerPort();
		param.serverAddress = CConfig::Instance().GetServerAddress();
		param.userID = CConfig::Instance().GetServerID();
		param.requestType = type;
		param.window = m_Window;

		// Launch the network communication thread
		DWORD id;
		HANDLE thread = CreateThread(NULL, 0, NetworkThreadProc, &param, 0, &id);
		CloseHandle(thread);
	}
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
			hotkey = CConfig::Instance().GetHideHotkey();
			break;

		case CConfig::HOTKEY_SHOW:
			hotkey = CConfig::Instance().GetShowHotkey();
			break;

		case CConfig::HOTKEY_TOGGLE:
			hotkey = CConfig::Instance().GetToggleHotkey();
			break;

		case CConfig::HOTKEY_ACTIVATE:
			hotkey = CConfig::Instance().GetActivateHotkey();
			break;

		case CConfig::HOTKEY_REFRESH:
			hotkey = CConfig::Instance().GetRefreshHotkey();
			break;

		case CConfig::HOTKEY_CONFIG:
			hotkey = CConfig::Instance().GetConfigHotkey();
			break;

		case CConfig::HOTKEY_SKIN:
			hotkey = CConfig::Instance().GetSkinHotkey();
			break;

		case CConfig::HOTKEY_NEXT:
			hotkey = CConfig::Instance().GetNextHotkey();
			break;

		case CConfig::HOTKEY_PREVIOUS:
			hotkey = CConfig::Instance().GetPreviousHotkey();
			break;

		case CConfig::HOTKEY_NEXT_YEAR:
			hotkey = CConfig::Instance().GetNextYearHotkey();
			break;

		case CConfig::HOTKEY_PREVIOUS_YEAR:
			hotkey = CConfig::Instance().GetPreviousYearHotkey();
			break;

		case CConfig::HOTKEY_CURRENT:
			hotkey = CConfig::Instance().GetCurrentHotkey();
			break;

		case CConfig::HOTKEY_ALL:
			hotkey = CConfig::Instance().GetAllHotkey();
			break;

		case CConfig::HOTKEY_OUTLOOK:
			hotkey = CConfig::Instance().GetOutlookHotkey();
			break;

		case CConfig::HOTKEY_TODO:
			hotkey = CConfig::Instance().GetTodoHotkey();
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

/////////////////////////////////////////////////////////////////////////////
// CCalendarWindow message handlers

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
				CConfig::Instance().SetCurrentLanguage("");
			}
			else
			{
				c_Language.ChangeLanguage(wParam - ID_LANGUAGE - 1);
				CConfig::Instance().SetCurrentLanguage(c_Language.GetLanguages()[wParam - ID_LANGUAGE - 1]);
			}
			CConfig::Instance().WriteConfig(CConfig::WRITE_CONFIG);
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
						CConfig::Instance().SetCurrentSkin(m_ConfigStrings[i].path);
						CConfig::Instance().SetCurrentSkinIni(m_ConfigStrings[i].iniFiles[j]);
						CConfig::Instance().WriteConfig(CConfig::WRITE_CONFIG);	// Write old config (happens only if Rainlendar.ini's not modified from outside)
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

		case ID_POPUP_HELP:
			ShellExecute(m_Window, "open", "Manual.html", NULL, NULL, SW_SHOWNORMAL);	
			break;

		case ID_REFRESH:
			Refresh();
			break;

		case ID_POPUP_UPDATEOUTLOOK:
			SyncWithOutlook(m_Window);
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

		case ID_TODO:
			LSLog(LOG_DEBUG, "Rainlendar", "Opening Todo dialog.");
			OpenTodoDialog(m_Window, CRainlendar::GetInstance(), -1);
			LSLog(LOG_DEBUG, "Rainlendar", "Todo dialog closed.");
			break;

		case ID_QUIT:
			FinalizeOutlook();
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


LRESULT CCalendarWindow::OnTimer(WPARAM wParam, LPARAM lParam) 
{
	if (GetEventManager() == NULL) 
	{
		return CRainWindow::OnTimer(wParam, lParam);
	}

	if(wParam == WINDOW_TIMER) 
	{
		SYSTEMTIME today;
		GetLocalTime(&today);
		int now = today.wHour * 60 + today.wMinute + CConfig::Instance().GetPreshowTime();

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

			std::vector<CEventMessage*> eventList = GetEventManager()->GetEvents(c_TodaysDate.wDay, c_TodaysDate.wMonth, c_TodaysDate.wYear);

			// Mark all until now as shown
			for (int i = 0; i < eventList.size(); i++)
			{
				if (eventList[i]->GetStartTime() < now)
				{
					eventList[i]->SetShown(true);
				}
			}

			m_MessageWindow.ClearEvents();
			ShowEventMessage(eventList, CConfig::Instance().GetShowAllEvents());
		}

		// Check for alarms
		if (CConfig::Instance().GetShowSingleEvent()) 
		{
			std::vector<CEventMessage*> newEventList;
			std::vector<CEventMessage*> eventList = GetEventManager()->GetEvents(c_TodaysDate.wDay, c_TodaysDate.wMonth, c_TodaysDate.wYear);

			for (int i = 0; i < eventList.size(); i++)
			{
				if (eventList[i]->GetStartTime() <= now && !eventList[i]->IsShown())
				{
					newEventList.push_back(eventList[i]);
					eventList[i]->SetShown(true);
				}
			}
			
			ShowEventMessage(newEventList, true);
		}

		if (CConfig::Instance().GetServerFrequency() > 0)
		{
			m_ConnectionCounter++;
			if (m_ConnectionCounter >= CConfig::Instance().GetServerFrequency() * 60)
			{
				m_ConnectionCounter = 0;
				ConnectServer(REQUEST_GETEVENTS);	// Get the events from server
			}
		}

		PollWallpaper(false);
		m_Background.FlushWallpaper();		// Get rid of the wallpaper
	}
	else if(wParam == OUTLOOK_TIMER) 
	{
        if (SyncWithOutlook(m_Window))
        {
            Refresh(true);
        }
    }

	return CRainWindow::OnTimer(wParam, lParam);
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

LRESULT CCalendarWindow::OnMove(WPARAM wParam, LPARAM lParam) 
{
	short int x = LOWORD(lParam);
	short int y = HIWORD(lParam);

	if(CConfig::Instance().GetMovable())
	{
		if (CConfig::Instance().GetNegativeCoords()) 
		{
			if (x < 0) x = 0;
			if (y < 0) y = 0;
		}

		CConfig::Instance().SetX(x);
		CConfig::Instance().SetY(y);
		CConfig::Instance().WriteConfig(CConfig::WRITE_POS);	// Store the new position to the ini file
	}

	m_X = x;
	m_Y = y;

	if (!m_FirstExecute && (CConfig::Instance().GetBackgroundMode() == MODE_COPY || m_Background.HasAlpha()))
	{
		m_Background.UpdateWallpaper(x, y);
		DrawWindow();
		InvalidateRect(m_Window, NULL, false);
	}

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
	if(!CConfig::Instance().GetDisableHotkeys()) 
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


// F10 Generates a syskey-message   ... wierd.
LRESULT CCalendarWindow::OnSysKeyUp(WPARAM wParam, LPARAM lParam) 
{
	if(!CConfig::Instance().GetDisableHotkeys()) 
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

LRESULT CCalendarWindow::OnHotkey(WPARAM wParam, LPARAM lParam)
{
	DWORD hotkey = HIWORD(lParam) + (LOWORD(lParam) << 8);

	if (hotkey == CConfig::Instance().GetHideHotkey())
	{
		RainlendarHide(m_Window, NULL);
	}
	else if (hotkey == CConfig::Instance().GetShowHotkey())
	{
    	RainlendarShow(m_Window, NULL);
	}
	else if (hotkey == CConfig::Instance().GetToggleHotkey())
	{
		RainlendarToggle(m_Window, NULL);
	}
	else if (hotkey == CConfig::Instance().GetActivateHotkey())
	{
    	RainlendarShow(m_Window, "Activate");
	}
	else if (hotkey == CConfig::Instance().GetRefreshHotkey())
	{
		RainlendarRefresh(m_Window, NULL);
	}
	else if (hotkey == CConfig::Instance().GetConfigHotkey())
	{
		RainlendarConfig(m_Window, NULL);
	}
	else if (hotkey == CConfig::Instance().GetSkinHotkey())
	{
		ShowEditSkin();
	}
	else if (hotkey == CConfig::Instance().GetNextHotkey())
	{
		OnCommand(ID_POPUP_SELECTMONTH_NEXTMONTH, NULL);
	}
	else if (hotkey == CConfig::Instance().GetPreviousHotkey())
	{
		OnCommand(ID_POPUP_SELECTMONTH_PREVMONTH, NULL);
	}
	else if (hotkey == CConfig::Instance().GetNextYearHotkey())
	{
		OnCommand(ID_POPUP_SELECTMONTH_NEXTYEAR, NULL);
	}
	else if (hotkey == CConfig::Instance().GetPreviousYearHotkey())
	{
		OnCommand(ID_POPUP_SELECTMONTH_PREVYEAR, NULL);
	}
	else if (hotkey == CConfig::Instance().GetCurrentHotkey())
	{
		OnCommand(ID_POPUP_SELECTMONTH_CURRENTMONTH, NULL);
	}
	else if (hotkey == CConfig::Instance().GetAllHotkey())
	{
		OnCommand(ID_ALLEVENTS, NULL); 
	}
	else if (hotkey == CConfig::Instance().GetOutlookHotkey())
	{
		OnCommand(ID_POPUP_UPDATEOUTLOOK, NULL); 
	}
	else if (hotkey == CConfig::Instance().GetTodoHotkey())
	{
		OnCommand(ID_TODO, NULL); 
	}
	return TRUE;
}

LRESULT CCalendarWindow::OnCopyData(WPARAM wParam, LPARAM lParam)
{
	COPYDATASTRUCT* pCopyDataStruct = (COPYDATASTRUCT*) lParam;

	if (pCopyDataStruct && (pCopyDataStruct->dwData == 1) && (pCopyDataStruct->cbData > 0))
	{
		std::string str = (const char*)pCopyDataStruct->lpData;
		std::string bang;
		std::string arg;

		// Find the first space
		std::string::size_type pos = str.find(' ');
		if (pos != -1)
		{
			bang = str.substr(0, pos);
			str.erase(0, pos + 1);
			arg = str;
		}
		else
		{
			bang = str;
		}

		if (stricmp(bang.c_str(), "!RainlendarToggle") == 0)
		{
			RainlendarToggle(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarHide") == 0)
		{
			RainlendarHide(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarShowEvents") == 0)
		{
			RainlendarShowEvents(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarRefresh") == 0)
		{
			RainlendarRefresh(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarConfig") == 0)
		{
			RainlendarConfig(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarQuit") == 0)
		{
			RainlendarQuit(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarShowNext") == 0)
		{
			RainlendarShowNext(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarShowPrev") == 0)
		{
			RainlendarShowPrev(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarShowMonth") == 0)
		{
			// This one takes arguments
			if (!arg.empty())
			{
				RainlendarShowMonth(m_Window, arg.c_str());
			}
            else
            {
				RainlendarShowMonth(m_Window, NULL);
    		}
		}
		else if (stricmp(bang.c_str(), "!RainlendarShowCurrent") == 0)
		{
			RainlendarShowCurrent(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarMove") == 0)
		{
			RainlendarMove(m_Window, arg.c_str());
		}
		else if (stricmp(bang.c_str(), "!RainlendarZPos") == 0)
		{
			RainlendarZPos(m_Window, arg.c_str());
		}
		else if (stricmp(bang.c_str(), "!RainlendarEditTodo") == 0)
		{
			RainlendarEditTodo(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarShowTodo") == 0)
		{
			RainlendarShowTodo(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarHideTodo") == 0)
		{
			RainlendarHideTodo(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarToggleTodo") == 0)
		{
			RainlendarToggleTodo(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarAddEvent") == 0)
		{
			if (!arg.empty())
			{
				RainlendarAddEvent(m_Window, arg.c_str());
			}
			else
			{
				RainlendarAddEvent(m_Window, NULL);
			}
		}
		else if (stricmp(bang.c_str(), "!RainlendarShow") == 0)
		{
			if (!arg.empty())
			{
	    		RainlendarShow(m_Window, arg.c_str());
			}
            else
            {
    			RainlendarShow(m_Window, NULL);
            }
		}
		else if (stricmp(bang.c_str(), "!Execute") == 0)
		{
			// Special case for multibang execution
			std::string allCommands = arg;
			
			while (true)
			{
				std::string::size_type start = allCommands.find('[');
				std::string::size_type end = allCommands.find(']');
				if (start != -1 && end != -1)
				{
					std::string command = allCommands.substr(start + 1, end - (start + 1));
					// trim leading whitespace
					std::string::size_type notwhite = command.find_first_not_of(" \t\n");
					command.erase(0, notwhite);

					COPYDATASTRUCT copyData;
					copyData.dwData = 1;
					copyData.cbData = strlen(command.c_str()) + 1;
					copyData.lpData = (void*)command.c_str();
					OnCopyData(NULL, (LPARAM)&copyData);
				}
				else
				{
					break;	// No more commands
				}
				allCommands = allCommands.substr(end + 1);
			}
		}
		else
		{
			char buffer[MAX_LINE_LENGTH];
			sprintf(buffer, CCalendarWindow::c_Language.GetString("Message", 8), bang.c_str());
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

LRESULT CCalendarWindow::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	UINT uID; 
	UINT uMouseMsg; 
	
	uID = (UINT)wParam; 
	uMouseMsg = (UINT)lParam; 

	std::string bang;

	switch(uMouseMsg) 
	{
	case WM_LBUTTONDOWN:
		bang = CConfig::Instance().GetTrayExecuteL();
		break;

	case WM_MBUTTONDOWN:
		bang = CConfig::Instance().GetTrayExecuteM();
		break;

	case WM_RBUTTONDOWN:
		bang = CConfig::Instance().GetTrayExecuteR();
		break;

	case WM_LBUTTONDBLCLK:
		bang = CConfig::Instance().GetTrayExecuteDL();
		break;

	case WM_MBUTTONDBLCLK:
		bang = CConfig::Instance().GetTrayExecuteDM();
		break;

	case WM_RBUTTONDBLCLK:
		bang = CConfig::Instance().GetTrayExecuteDR();
		break;
	}

	if (!bang.empty())
	{
		COPYDATASTRUCT copyData;
		
		copyData.dwData = 1;
		copyData.cbData = strlen(bang.c_str()) + 1;
		copyData.lpData = (void*)bang.c_str();
	
		OnCopyData(NULL, (LPARAM)&copyData);
	}
	else if	(uMouseMsg == WM_RBUTTONDOWN)
	{
		POINT point;
		GetCursorPos(&point);

		SetForegroundWindow(m_Window);
		OnContextMenu(0, MAKELPARAM(point.x, point.y));
		SetForegroundWindow(m_Window);
	}

	return TRUE;
}

/* 
** WndProc
**
** The window procedure
**
*/
LRESULT CALLBACK CCalendarWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CCalendarWindow* Window = NULL;

	if (Window) Window->m_Message = uMsg;

	if (uMsg == WM_CREATE) 
	{
		// Fetch this window-object from the CreateStruct
		Window=(CCalendarWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
	}

	// Run the tooltip's mouse proc
	CToolTip::MouseProc(hWnd, uMsg, wParam, lParam);

	if (Window)
	{
		switch(uMsg) 
		{
			case WM_CONTEXTMENU:
			case WM_NCRBUTTONUP:
				DebugLog("WM_CONTEXTMENU: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnContextMenu(wParam, lParam);

			case WM_COMMAND:
				DebugLog("WM_COMMAND: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnCommand(wParam, lParam);

			case WM_LBUTTONDBLCLK:
			case WM_NCLBUTTONDBLCLK:
				DebugLog("WM_COMMAND: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnLButtonDblClk(wParam, lParam);

			case WM_MOVE: 
				return Window->OnMove(wParam, lParam);

			case LM_GETREVID: 
				return Window->OnGetRevID(wParam, lParam);

			case WM_KEYUP:
				DebugLog("WM_KEYUP: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnKeyUp(wParam, lParam);

			case WM_SYSKEYUP:
				DebugLog("WM_SYSKEYUP: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnSysKeyUp(wParam, lParam);

			case WM_COPYDATA: 
				return Window->OnCopyData(wParam, lParam);
				
			case WM_SERVER_SYNC_FINISHED: 
				DebugLog("WM_SERVER_SYNC_FINISHED: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnServerSyncFinished(wParam, lParam);

			case WM_HOTKEY: 
				DebugLog("WM_COMMAND: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnHotkey(wParam, lParam);

			case WM_PAINT: 
				return Window->OnPaint(wParam, lParam);

			case WM_TIMER: 
				return Window->OnTimer(wParam, lParam);

			case WM_WINDOWPOSCHANGING: 
				return Window->OnWindowPosChanging(wParam, lParam);

			case WM_SETTINGCHANGE: 
				DebugLog("WM_SETTINGCHANGE: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnSettingsChange(wParam, lParam);

			case WM_ERASEBKGND: 
				return Window->OnEraseBkgnd(wParam, lParam);

			case WM_MOUSEMOVE: 
			case WM_NCMOUSEMOVE: 
				return Window->OnMouseMove(wParam, lParam);

			case WM_POWERBROADCAST: 
				DebugLog("WM_POWERBROADCAST: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnPowerBroadcast(wParam, lParam);

			case WM_DISPLAYCHANGE: 
				DebugLog("WM_DISPLAYCHANGE: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnDisplayChange(wParam, lParam);

			case WM_NCHITTEST: 
				return Window->OnNcHitTest(wParam, lParam);

			case WM_NOTIFYICON: 
				return Window->OnNotifyIcon(wParam, lParam);
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

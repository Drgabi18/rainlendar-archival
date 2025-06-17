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
  $Header: /home/cvsroot/Rainlendar/Library/CalendarWindow.cpp,v 1.3 2005/10/14 17:05:41 rainy Exp $

  $Log: CalendarWindow.cpp,v $
  Revision 1.3  2005/10/14 17:05:41  rainy
  no message

  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.37  2005/03/25 13:58:42  rainy
  no message

  Revision 1.36  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.35  2004/12/05 18:34:46  rainy
  Changed wallpaper polling.
  Added hotkeys.

  Revision 1.34  2004/11/06 13:38:58  rainy
  no message

  Revision 1.33  2004/06/10 15:24:59  rainy
  Changed the bang parsing routines

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
  Changed CEvent to CEntryEvent to avoid name clash.
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
#pragma warning(disable: 4996)

#include <windows.h>
#include <math.h>
#include "RainlendarDLL.h"
#include "CalendarWindow.h"
#include "Config.h"
#include "Error.h"
#include "EditEvent.h"
#include "ConfigDialog.h"
#include "SkinDialog.h"
#include "Tooltip.h"
#include "RasterizerFont.h"
#include "AllEventsDialog.h"
#include "TodoDialog.h"
#include "UpdateCheck.h"
#include <crtdbg.h>
#include <algorithm>

#define ULW_ALPHA               0x00000002
#define WS_EX_LAYERED           0x00080000

// Defines
#define SNAPDISTANCE 10
#define WM_NOTIFYICON WM_USER + 101

// Class variables
CFileTime CCalendarWindow::c_TodaysDate;
CFileTime CCalendarWindow::c_MonthsFirstDate;
CLanguage CCalendarWindow::c_Language;

extern bool CompareEvents(CEntryEvent* arg1, CEntryEvent* arg2);		// This is in EventListWindow

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

	m_Days = NULL;
	m_Event = NULL;
	m_Today = NULL;
	m_Weekdays = NULL;
	m_WeekNumbers = NULL;
	m_Month = NULL;
	m_Year = NULL;

	m_ToolBarMessage = 0;

	m_ConnectionCounter = 0;

	m_Docked = true;
	m_QuitSet = false;
	
	m_CurrentMonthOffset.x = 0;		// Mitul
	m_CurrentMonthOffset.y = 0;		// Mitul
	m_ViewMonthOffset.x = 0;
	m_ViewMonthOffset.y = 0;

	ResetSettings();
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

	c_Language.Clear();

	if(m_Window) CConfig::Instance().WriteConfig(CConfig::WRITE_FULL);

	delete m_Days;
	delete m_Event;
	delete m_Today;
	delete m_Weekdays;
	delete m_WeekNumbers;
	delete m_Month;
	delete m_Year;

	for (int i = 0; i < m_DynamicItems.size(); i++)
	{
		delete m_DynamicItems[i];
	}
	m_DynamicItems.clear();

	DebugLog("The calendar window is destroyed.");
}

void CCalendarWindow::ResetSettings()
{
	CRainWindow::ResetSettings();

	// Reset the values
	m_BackgroundBitmapName = "";
	m_BackgroundMode=MODE_TILE;
	m_BackgroundSolidColor=GetSysColor(COLOR_3DFACE);
	m_BackgroundBevel=true;
}

void CCalendarWindow::ReadSettings(const char* iniFile, const char* section)
{
	CRainWindow::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	if(GetPrivateProfileString( section, "BackgroundBitmapName", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_BackgroundBitmapName=tmpSz;
	}
	m_BackgroundMode=(BACKGROUND_MODE)GetPrivateProfileInt( section, "BackgroundMode", m_BackgroundMode, iniFile);
	m_BackgroundBevel=(1==GetPrivateProfileInt( section, "BackgroundBevel", m_BackgroundBevel?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( section, "BackgroundSolidColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_BackgroundSolidColor = CConfig::ParseColor(tmpSz);
	}
}

void CCalendarWindow::WriteSettings()
{
	char tmpSz[256];

	std::string INIPath = m_SettingsFile;

	// Background stuff
	WritePrivateProfileString( m_Section.c_str(), "BackgroundBitmapName", m_BackgroundBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i", m_BackgroundMode);
	WritePrivateProfileString( m_Section.c_str(), "BackgroundMode", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_BackgroundBevel);
	WritePrivateProfileString( m_Section.c_str(), "BackgroundBevel", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%X", m_BackgroundSolidColor);
	WritePrivateProfileString( m_Section.c_str(), "BackgroundSolidColor", tmpSz, INIPath.c_str() );
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

	std::string PixPath, SkinsPath, PluginsPath, LanguagesPath;
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

		GetRCString("RainlendarSkinsPath", tmpName, "", MAX_LINE_LENGTH);
		VarExpansion(tmpName, tmpName);
		SkinsPath = tmpName;

		GetRCString("RainlendarPluginsPath", tmpName, "", MAX_LINE_LENGTH);
		VarExpansion(tmpName, tmpName);
		PluginsPath = tmpName;

		GetRCString("RainlendarLanguagesPath", tmpName, "", MAX_LINE_LENGTH);
		VarExpansion(tmpName, tmpName);
		LanguagesPath = tmpName;

		// Check that the path ends with "\" 
		if (!SkinsPath.empty() && *(SkinsPath.end() - 1) != '\\')
		{
			SkinsPath += "\\";
		}

		if (!PluginsPath.empty() && *(PluginsPath.end() - 1) != '\\')
		{
			PluginsPath += "\\";
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
				DebugLog("Found Rainlendar.ini in current folder");
			}
			else
			{
				DebugLog("No Rainlendar.ini in current folder -> Using %APPDATA%.");

				char buffer[MAX_PATH + 1];
				buffer[0] = 0;
				ExpandEnvironmentStrings("%APPDATA%", buffer, MAX_PATH);
				if (strlen(buffer) == 0 || strcmp(buffer, "%APPDATA%") == 0) 
				{
					DebugLog("No %APPDATA% found. Falling back to the old system.");
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

	if (SkinsPath.empty())
	{
		SkinsPath = dllPath + "Skins\\";
	}

	if (PluginsPath.empty())
	{
		PluginsPath = dllPath + "Plugins\\";
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

	DebugLog("PixPath: %s", PixPath.c_str());
	DebugLog("DllPath: %s", dllPath.c_str());
	DebugLog("SkinsPath: %s", SkinsPath.c_str());
	DebugLog("LanguagesPath: %s", LanguagesPath.c_str());
	DebugLog("PluginsPath: %s", PluginsPath.c_str());

	CConfig::Instance().SetPath(PixPath);
	CConfig::Instance().SetDLLPath(dllPath);
	CConfig::Instance().SetSkinsPath(SkinsPath);
	CConfig::Instance().SetPluginsPath(PluginsPath);
	CConfig::Instance().ReadConfig();

	if (CRainlendar::GetDummyLitestep())
	{
		DebugLog("Running as stand-alone.");
	}
	else
	{
		DebugLog("Running as Litestep plugin.");
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
	cce.dwICC = ICC_WIN95_CLASSES | ICC_DATE_CLASSES;
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

	// Get today's date
	c_TodaysDate.SetToLocalTime();

	// Store current date (the month that will be shown). This will set properly in ChangeMonth().
	c_MonthsFirstDate.SetFileTime(c_TodaysDate.GetFileTime());

	m_TodoWindow.Initialize(m_Window, m_Instance);
	m_MessageWindow.Initialize(m_Window, m_Instance);
	m_EventListWindow.Initialize(m_Window, m_Instance);

	// Plugins are loaded only when the application is started (MAPI leaks memory so Outlook plugin creates problems).
	m_PluginManager.LoadPlugins(CConfig::Instance().GetPluginsPath().c_str());

	ChangeMonth(-1, -1);	// switch the calendar to today's month
	RefreshAll();

	m_ToolBarMessage = RegisterWindowMessage("TaskbarCreated");

	if(!CConfig::Instance().GetStartHidden()) 
	{
		if (CConfig::Instance().GetCalendarEnable()) 
		{
			FadeWindow(true, false);
		}
		if (CConfig::Instance().GetTodoEnable() && !CConfig::Instance().GetSmartTodo()) 
		{
			m_TodoWindow.FadeWindow(true, false);
		}
		if (CConfig::Instance().GetEventListEnable() && !CConfig::Instance().GetSmartEventList()) 
		{
			m_EventListWindow.FadeWindow(true, false);
		}
	}

	if (CConfig::Instance().GetCheckUpdates())
	{
		CheckUpdate();
	}

	if (CConfig::Instance().GetTodoLocked())
	{
		m_TodoWindow.DockWindow(false);
	}
	if (CConfig::Instance().GetEventListLocked())
	{
		m_EventListWindow.DockWindow(false);
	}

	if (CConfig::Instance().GetShowAllEvents())
	{
		ShowTodaysEvents();
	}

	return res;
}

/* 
** FillMenu
**
** Fills the context menu with months
**
*/
void CCalendarWindow::FillMenu(HMENU subMenu, int x, int y, HWND hWnd)
{
	char tmpSz[256];

	// Localize the menu
	ModifyMenu(subMenu, ID_ABOUT, MF_BYCOMMAND | MF_STRING, ID_ABOUT, c_Language.GetString("Menu", 0));	// About
	ModifyMenu(subMenu, ID_POPUP_HELP, MF_BYCOMMAND | MF_STRING, ID_POPUP_HELP, c_Language.GetString("Menu", 37));	// Help
	ModifyMenu(subMenu, ID_REFRESH, MF_BYCOMMAND | MF_STRING, ID_REFRESH, c_Language.GetString("Menu", 17));	// Refresh
	ModifyMenu(subMenu, ID_CONFIG, MF_BYCOMMAND | MF_STRING, ID_CONFIG, c_Language.GetString("Menu", 20));	// Config
	ModifyMenu(subMenu, ID_SETEVENT, MF_BYCOMMAND | MF_STRING, ID_SETEVENT, c_Language.GetString("Menu", 18));	// Set event...
	ModifyMenu(subMenu, ID_ALLEVENTS, MF_BYCOMMAND | MF_STRING, ID_ALLEVENTS, c_Language.GetString("Menu", 19));	// Show all events...
	ModifyMenu(subMenu, ID_TODO, MF_BYCOMMAND | MF_STRING, ID_TODO, c_Language.GetString("Menu", 29));	// Show todo list...
	ModifyMenu(subMenu, ID_QUIT, MF_BYCOMMAND | MF_STRING, ID_QUIT, c_Language.GetString("Menu", 21));	// Quit

	// Submenus
	ModifyMenu(subMenu, 3, MF_BYPOSITION | MF_STRING, 0, c_Language.GetString("Menu", 40));	// Windows
	ModifyMenu(subMenu, 4, MF_BYPOSITION | MF_STRING, 0, c_Language.GetString("Menu", 25));	// Skins
	ModifyMenu(subMenu, 5, MF_BYPOSITION | MF_STRING, 0, c_Language.GetString("Menu", 22));	// Language
	ModifyMenu(subMenu, 6, MF_BYPOSITION | MF_STRING, 0, c_Language.GetString("Menu", 1));	// Select Month
	ModifyMenu(subMenu, 13, MF_BYPOSITION | MF_STRING, 0, c_Language.GetString("Menu", 44));	// Event

	HMENU windowsMenu = GetSubMenu(subMenu, 3);
	if (windowsMenu)
	{
		ModifyMenu(windowsMenu, 0, MF_BYPOSITION | MF_STRING, 0, c_Language.GetString("Menu", 41));
		ModifyMenu(windowsMenu, 1, MF_BYPOSITION | MF_STRING, 0, c_Language.GetString("Menu", 42));
		ModifyMenu(windowsMenu, 2, MF_BYPOSITION | MF_STRING, 0, c_Language.GetString("Menu", 43));

		HMENU windowsSubMenu = GetSubMenu(windowsMenu, 0);
		ModifyMenu(windowsSubMenu, ID_POPUP_CALENDAR_VISIBLE, MF_BYCOMMAND | MF_STRING, ID_POPUP_CALENDAR_VISIBLE, c_Language.GetString("Menu", 47));
		if (CConfig::Instance().GetCalendarEnable())
		{
			CheckMenuItem(windowsSubMenu, ID_POPUP_CALENDAR_VISIBLE, MF_BYCOMMAND | MF_CHECKED);
		}
		
		windowsSubMenu = GetSubMenu(windowsMenu, 1);
		ModifyMenu(windowsSubMenu, ID_POPUP_TODO_VISIBLE, MF_BYCOMMAND | MF_STRING, ID_POPUP_TODO_VISIBLE, c_Language.GetString("Menu", 47));
		ModifyMenu(windowsSubMenu, ID_POPUP_TODO_LOCKED, MF_BYCOMMAND | MF_STRING, ID_POPUP_TODO_LOCKED, c_Language.GetString("Menu", 48));
		if (CConfig::Instance().GetTodoEnable())
		{
			CheckMenuItem(windowsSubMenu, ID_POPUP_TODO_VISIBLE, MF_BYCOMMAND | MF_CHECKED);
		}
		if (CConfig::Instance().GetTodoLocked())
		{
			CheckMenuItem(windowsSubMenu, ID_POPUP_TODO_LOCKED, MF_BYCOMMAND | MF_CHECKED);
		}

		windowsSubMenu = GetSubMenu(windowsMenu, 2);
		ModifyMenu(windowsSubMenu, ID_POPUP_EVENTLIST_VISIBLE, MF_BYCOMMAND | MF_STRING, ID_POPUP_EVENTLIST_VISIBLE, c_Language.GetString("Menu", 47));
		ModifyMenu(windowsSubMenu, ID_POPUP_EVENTLIST_LOCKED, MF_BYCOMMAND | MF_STRING, ID_POPUP_EVENTLIST_LOCKED, c_Language.GetString("Menu", 48));
		if (CConfig::Instance().GetEventListEnable())
		{
			CheckMenuItem(windowsSubMenu, ID_POPUP_EVENTLIST_VISIBLE, MF_BYCOMMAND | MF_CHECKED);
		}
		if (CConfig::Instance().GetEventListLocked())
		{
			CheckMenuItem(windowsSubMenu, ID_POPUP_EVENTLIST_LOCKED, MF_BYCOMMAND | MF_CHECKED);
		}
	}

	HMENU monthsMenu = GetSubMenu(subMenu, 6);
	if (monthsMenu)
	{
		ModifyMenu(monthsMenu, ID_POPUP_SELECTMONTH_NEXTMONTH, MF_BYCOMMAND | MF_STRING, ID_POPUP_SELECTMONTH_NEXTMONTH, c_Language.GetString("Menu", 2));	// Next Month
		ModifyMenu(monthsMenu, ID_POPUP_SELECTMONTH_PREVMONTH, MF_BYCOMMAND | MF_STRING, ID_POPUP_SELECTMONTH_PREVMONTH, c_Language.GetString("Menu", 3));	// Prev Month
		ModifyMenu(monthsMenu, ID_POPUP_SELECTMONTH_NEXTYEAR, MF_BYCOMMAND | MF_STRING, ID_POPUP_SELECTMONTH_NEXTYEAR, c_Language.GetString("Menu", 26));	// Next Year
		ModifyMenu(monthsMenu, ID_POPUP_SELECTMONTH_PREVYEAR, MF_BYCOMMAND | MF_STRING, ID_POPUP_SELECTMONTH_PREVYEAR, c_Language.GetString("Menu", 27));	// Prev Year
		ModifyMenu(monthsMenu, ID_POPUP_SELECTMONTH_CURRENTMONTH, MF_BYCOMMAND | MF_STRING, ID_POPUP_SELECTMONTH_CURRENTMONTH, c_Language.GetString("Menu", 4));	// Current Month

		// Change the months in the submenu
		std::string name;
		for(int i = 0; i < 12; i++) 
		{
			sprintf(tmpSz, "\tF%i", i + 1);
			name = c_Language.GetString("Menu", i + 5);
			name += tmpSz;

			ModifyMenu(monthsMenu, i + 6, MF_BYPOSITION | MF_STRING, GetMenuItemID(monthsMenu, i + 6), name.c_str());
		}
	}

	// If mouse was pressed on a day, enable set event
	bool enableDeleted = true;
	m_NewEventStartTime = c_TodaysDate;
	HMENU eventsMenu = GetSubMenu(subMenu, 13);
	if (eventsMenu) 
	{
		ModifyMenu(eventsMenu, ID_POPUP_EVENT_CREATENEWEVENT, MF_BYCOMMAND | MF_STRING, ID_POPUP_EVENT_CREATENEWEVENT, c_Language.GetString("Menu", 45));
		ModifyMenu(eventsMenu, ID_POPUP_EVENT_DELETEEVENTS, MF_BYCOMMAND | MF_STRING, ID_POPUP_EVENT_DELETEEVENTS, c_Language.GetString("Menu", 46));

		if (hWnd == m_Window)
		{
			std::vector<CEntryEvent*>* events = m_Event->HitTest(x, y, &m_NewEventStartTime);
			if (events && events->size() > 0)
			{
				m_SelectedEvents.clear();

				unsigned int i = 0;
				for ( ; i < events->size(); i++)
				{
					std::string brief;
					(*events)[i]->GetBriefMessage(brief, 100, false, false);
					m_SelectedEvents.push_back(*(*events)[i]->GetGUID());
					InsertMenu(eventsMenu, i, MF_BYPOSITION, ID_EVENT_START + i, brief.c_str());
					if ((*events)[i]->IsReadOnly())
					{
						EnableMenuItem(eventsMenu, i, MF_BYPOSITION | MF_GRAYED);
						enableDeleted = false;
					}
				}
				InsertMenu(eventsMenu, i, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
			}
			else
			{
				enableDeleted = false;
			}
		} 
		else if (hWnd == m_EventListWindow.GetWindow())
		{
			GUID* guid = m_EventListWindow.HitTest(x, y, NULL);
			if (guid)
			{
				m_SelectedEvents.clear();

				CEventManager& manager = GetRainlendar()->GetCalendarWindow().GetEventManager();
				CEntryEvent* event = manager.GetEvent(guid);

				if (event && !event->IsReadOnly())
				{
					std::string brief;
					event->GetBriefMessage(brief, 100, false, false);
					m_SelectedEvents.push_back(*guid);
					InsertMenu(eventsMenu, 0, MF_BYPOSITION, ID_EVENT_START, brief.c_str());
					if (event->IsReadOnly())
					{
						EnableMenuItem(eventsMenu, 0, MF_BYPOSITION | MF_GRAYED);
						enableDeleted = false;
					}
					InsertMenu(eventsMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
				}
			}
			else
			{
				enableDeleted = false;
			}
		}
	} 

	if (!enableDeleted) 
	{
		EnableMenuItem(eventsMenu, ID_POPUP_EVENT_DELETEEVENTS, MF_GRAYED);
	}

	if (!CRainlendar::GetDummyLitestep())
	{
		// Disable QUIT if LiteStep's plugin 
		EnableMenuItem(subMenu, ID_QUIT, MF_GRAYED);
	}

	int index = 0;

	// Fill the menu with all the configs
	HMENU configMenu = GetSubMenu(subMenu, 4);
	if(configMenu)
	{
		ModifyMenu(configMenu, 0, MF_BYPOSITION | MF_STRING, GetMenuItemID(configMenu, 0), c_Language.GetString("Menu", 24));	// Edit Current Skin

		for(UINT i = 0; i < m_ConfigStrings.size(); i++)
		{
			if (m_ConfigStrings[i].iniFiles.size() > 1)
			{
				HMENU iniMenu = CreatePopupMenu();
				InsertMenu(configMenu, i + 2, MF_BYPOSITION | MF_POPUP, (UINT_PTR)iniMenu, m_ConfigStrings[i].path.c_str());
				for(UINT j = 0; j < m_ConfigStrings[i].iniFiles.size(); j++)
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
		ModifyMenu(configMenu, ID_POPUP_SKINS_GETMORESKINS, MF_BYCOMMAND | MF_STRING, ID_POPUP_SKINS_GETMORESKINS, c_Language.GetString("Menu", 39));	// Get more skins...
	}

	// Fill the menu with all the configs
	HMENU languageMenu = GetSubMenu(subMenu, 5);
	if(languageMenu)
	{
		bool languageFound = false;
		ModifyMenu(languageMenu, 0, MF_BYPOSITION | MF_STRING, GetMenuItemID(languageMenu, 0), c_Language.GetString("Menu", 23));	// English

		for(UINT i = 0; i < c_Language.GetLanguages().size(); i++)
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

		ModifyMenu(languageMenu, ID_POPUP_LANGUAGE_GETMORELANGUAGES, MF_BYCOMMAND | MF_STRING, ID_POPUP_LANGUAGE_GETMORELANGUAGES, c_Language.GetString("Menu", 38));	// Get more languages...
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
	SIZE size = CRainWindow::CalcWindowSize();

	if (m_Days && m_Days->IsEnabled()) 
	{
		size.cx = max(size.cx, m_Days->GetX() + m_Days->GetW());
		size.cy = max(size.cy, m_Days->GetY() + m_Days->GetH());
	}
	if (m_Weekdays && m_Weekdays->IsEnabled()) 
	{
		size.cx = max(size.cx, m_Weekdays->GetX() + m_Weekdays->GetW());
		size.cy = max(size.cy, m_Weekdays->GetY() + m_Weekdays->GetH());
	}
	if (m_WeekNumbers && m_WeekNumbers->IsEnabled()) 
	{
		size.cx = max(size.cx, m_WeekNumbers->GetX() + m_WeekNumbers->GetW());
		size.cy = max(size.cy, m_WeekNumbers->GetY() + m_WeekNumbers->GetH());
	}
	if (m_Month && m_Month->IsEnabled()) 
	{
		size.cx = max(size.cx, m_Month->GetX() + m_Month->GetW());
		size.cy = max(size.cy, m_Month->GetY() + m_Month->GetH());
	}
	if (m_Year && m_Year->IsEnabled()) 
	{
		size.cx = max(size.cx, m_Year->GetX() + m_Year->GetW());
		size.cy = max(size.cy, m_Year->GetY() + m_Year->GetH());
	}
	if (m_Today && m_Today->IsEnabled()) 
	{
		size.cx = max(size.cx, m_Today->GetX() + m_Today->GetW());
		size.cy = max(size.cy, m_Today->GetY() + m_Today->GetH());
	}

	// Mitul{ : Also consider the dynamic Items
	for (int i = 0; i < m_DynamicSkinItems.size(); i++)
	{
		if ( (m_DynamicSkinItems[i]->IsIncludeSize()) && (m_DynamicSkinItems[i]->IsEnabled()) )
		{
			size.cx = max(size.cx, m_DynamicSkinItems[i]->GetX() + m_DynamicSkinItems[i]->GetW());
			size.cy = max(size.cy, m_DynamicSkinItems[i]->GetY() + m_DynamicSkinItems[i]->GetH());
		}
	}
	// Mitul}

	// Background already knows about vert & horz counts.
	if (GetBackgroundMode() != MODE_STRETCH)
	{
		if (m_Background.GetImageWidth() > size.cx) size.cx = m_Background.GetImageWidth();
		if (m_Background.GetImageHeight() > size.cy) size.cy = m_Background.GetImageHeight();

		size.cx *= CConfig::Instance().GetHorizontalCount();
		size.cy *= CConfig::Instance().GetVerticalCount();
	}
	else
	{
		size.cx *= CConfig::Instance().GetHorizontalCount();
		size.cy *= CConfig::Instance().GetVerticalCount();

		if (m_Background.GetImageWidth() > size.cx) size.cx = m_Background.GetImageWidth();
		if (m_Background.GetImageHeight() > size.cy) size.cy = m_Background.GetImageHeight();
	}

	return size;
}

void CCalendarWindow::SetTransparency(UINT transparency)
{
	UpdateTransparency(transparency);
	m_TodoWindow.UpdateTransparency(transparency);
	m_EventListWindow.UpdateTransparency(transparency);
	m_MessageWindow.UpdateTransparency(transparency);
}

void CCalendarWindow::RefreshAll()
{
	Refresh();
	m_TodoWindow.Refresh();
	m_EventListWindow.Refresh();
	m_MessageWindow.Refresh();
}

void CCalendarWindow::RedrawAll()
{
	Redraw();
	m_TodoWindow.Redraw();
	m_EventListWindow.Redraw();
	m_MessageWindow.Redraw();

	RemoveTrayIcon();
	if (CConfig::Instance().GetShowTrayIcon()) 
	{
		AddUpdateTrayIcon(NIM_ADD, false, 1);
	}
}

CItem* CCalendarWindow::GetItem(const char* section)
{
	if (section == NULL) return NULL;

	// Check the static items
	if (m_Days->GetSection() == section)
	{
		return m_Days;
	}
	else if (m_Event->GetSection() == section)
	{
		return m_Event;
	}
	else if (m_Today->GetSection() == section)
	{
		return m_Today;
	}
	else if (m_Weekdays->GetSection() == section)
	{
		return m_Weekdays;
	}
	else if (m_WeekNumbers->GetSection() == section)
	{
		return m_WeekNumbers;
	}
	else if (m_Month->GetSection() == section)
	{
		return m_Month;
	}
	else if (m_Year->GetSection() == section)
	{
		return m_Year;
	}

	for (int i = 0; i < m_DynamicItems.size(); i++)
	{
		if (m_DynamicItems[i]->GetSection() == section)
		{
			return m_DynamicItems[i];
		}
	}

	DebugLog("No such item; %s", section);

	return NULL;
}

void CCalendarWindow::ReadSkinSettings()
{
	if (!CConfig::Instance().GetCurrentSkin().empty() && !CConfig::Instance().GetCurrentSkinIni().empty())
	{
		std::string filename = CConfig::Instance().GetSkinsPath() + CConfig::Instance().GetCurrentSkin() + "/" + CConfig::Instance().GetCurrentSkinIni();
		std::string section = "Rainlendar";

		char* items = new char[MAX_LINE_LENGTH];
		int size = MAX_LINE_LENGTH;

		// Get all the sections
		while(true)
		{
			int res = GetPrivateProfileString( NULL, NULL, NULL, items, size, filename.c_str());
			if (res == 0) return;		// File not found
			if (res != size - 2) break;		// Fits in the buffer

			delete [] items;
			size *= 2;
			items = new char[size];
		};

		for (int i = 0; i < m_DynamicItems.size(); i++)
		{
			delete m_DynamicItems[i];
		}
		m_DynamicItems.clear();

		ResetSettings();
		m_EventListWindow.ResetSettings();
		m_MessageWindow.ResetSettings();
		m_TodoWindow.ResetSettings();
		CToolTip::Instance().ResetSettings();

		bool dynamic = false;
		char* pos = items;
		while(strlen(pos) > 0)
		{
			CItem* item = NULL;
			RAINWINDOW_TYPE winType = RAINWINDOW_TYPE_CALENDAR;

			if (0 == stricmp(pos, "Rainlendar"))
			{
				// Old style skin
				ReadSettings(filename.c_str(), section.c_str());
				m_Days->ReadSettings(filename.c_str(), section.c_str());
				m_Event->ReadSettings(filename.c_str(), section.c_str());
				m_Today->ReadSettings(filename.c_str(), section.c_str());
				m_Weekdays->ReadSettings(filename.c_str(), section.c_str());
				m_WeekNumbers->ReadSettings(filename.c_str(), section.c_str());
				m_Month->ReadSettings(filename.c_str(), section.c_str());
				m_Year->ReadSettings(filename.c_str(), section.c_str());

				AddSkinItem(m_Days, false);
				AddSkinItem(m_Event, false);
				AddSkinItem(m_Today, false);
				AddSkinItem(m_Weekdays, false);
				AddSkinItem(m_WeekNumbers, false);
				AddSkinItem(m_Month, false);
				AddSkinItem(m_Year, false);

				CToolTip::Instance().ReadSettings(filename.c_str(), section.c_str());

				// TODO: Put there to proper place
				m_EventListWindow.ReadSettings(filename.c_str(), section.c_str());
				m_MessageWindow.ReadSettings(filename.c_str(), section.c_str());
				m_TodoWindow.ReadSettings(filename.c_str(), section.c_str());
			}
			else if (0 == stricmp(pos, "Background"))
			{
				ReadSettings(filename.c_str(), pos);
			}
			else if (0 == stricmp(pos, "Days"))
			{
				m_Days->ReadSettings(filename.c_str(), pos);
				AddSkinItem(m_Days, false);
			}
			else if (0 == stricmp(pos, "Event"))
			{
				m_Event->ReadSettings(filename.c_str(), pos);
				AddSkinItem(m_Event, false);
			}
			else if (0 == stricmp(pos, "Today"))
			{
				m_Today->ReadSettings(filename.c_str(), pos);
				AddSkinItem(m_Today, false);
			}
			else if (0 == stricmp(pos, "Weekdays"))
			{
				m_Weekdays->ReadSettings(filename.c_str(), pos);
				AddSkinItem(m_Weekdays, false);
			}
			else if (0 == stricmp(pos, "WeekNumbers"))
			{
				m_WeekNumbers->ReadSettings(filename.c_str(), pos);
				AddSkinItem(m_WeekNumbers, false);
			}
			else if (0 == stricmp(pos, "Month"))
			{
				m_Month->ReadSettings(filename.c_str(), pos);
				AddSkinItem(m_Month, false);
			}
			else if (0 == stricmp(pos, "Year"))
			{
				m_Year->ReadSettings(filename.c_str(), pos);
				AddSkinItem(m_Year, false);
			}
			else if (0 == stricmp(pos, "ToolTip"))
			{
				CToolTip::Instance().ReadSettings(filename.c_str(), pos);
			}
			else if (0 == stricmp(pos, "EventList"))
			{
				m_EventListWindow.ReadSettings(filename.c_str(), pos);
			}
			else if (0 == stricmp(pos, "MessageBox"))
			{
				m_MessageWindow.ReadSettings(filename.c_str(), pos);
			}
			else if (0 == stricmp(pos, "Todo"))
			{
				m_TodoWindow.ReadSettings(filename.c_str(), pos);
			}
			else if (0 == strnicmp(pos, "Image", 5))
			{
				CItemImage* image = new CItemImage;
				image->ReadSettings(filename.c_str(), pos);
				image->Initialize();
				winType = image->GetWinType();
				item = image;
			}
			else if (0 == strnicmp(pos, "Button", 6))
			{
				CItemButton* button = new CItemButton;
				button->ReadSettings(filename.c_str(), pos);
				button->Initialize();
				winType = button->GetWinType();
				item = button;
				dynamic = true;		// All following images are dynamic
			}
			else if ( (0 == strnicmp(pos, "Time", 4)) || (0 == strnicmp(pos, "WorldTime", 9)) )
			{
				CItemTime* time = new CItemTime;
				time->ReadSettings(filename.c_str(), pos);
				time->Initialize();
				winType = time->GetWinType();
				item = time;
				dynamic = true;		// All following images are dynamic
			}

			if (item)
			{
				switch(winType)
				{
				case RAINWINDOW_TYPE_CALENDAR:
					AddSkinItem(item, dynamic);
					break;

				case RAINWINDOW_TYPE_MESSAGEBOX:
					m_MessageWindow.AddSkinItem(item, dynamic);
					break;

				case RAINWINDOW_TYPE_EVENTLIST:
					m_EventListWindow.AddSkinItem(item, dynamic);
					break;

				case RAINWINDOW_TYPE_TODO:
					m_TodoWindow.AddSkinItem(item, dynamic);
					break;

				case RAINWINDOW_TYPE_BALLOONTIP:  // Just keep in m_DynamicItems
                     break;                       // because balloon tip window 
                                                  // is not created by us
				}

				m_DynamicItems.push_back((CItemDynamic*)item);		// Mitul
			}

			pos = pos + strlen(pos) + 1;
		}

		delete [] items;

		CConfig::Instance().ReadProfiles(filename.c_str());
	}

}

/* 
** Refresh
**
** Refresh the window
**
*/
void CCalendarWindow::Refresh()
{
	DWORD time = GetTickCount();
	DebugLog("Refresh started.");

	try 
	{
		ReadSkins();		// Rescan the skins
		CConfig::Instance().ReadConfig();

		DebugLog("Skin: %s, Ini: %s", CConfig::Instance().GetCurrentSkin().c_str(), CConfig::Instance().GetCurrentSkinIni().c_str());

		CRainWindow::Refresh();

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

		ReadSkinSettings();

		bool loadImage = false;
		if (GetBackgroundMode() == MODE_TILE || GetBackgroundMode() == MODE_STRETCH)
		{
			loadImage = true;
		}
		m_Background.Initialize(GetBackgroundBitmapName().c_str(), loadImage);

		m_Days->Initialize();
		m_Event->Initialize();
		m_Today->Initialize();
		m_Weekdays->Initialize();
		m_WeekNumbers->Initialize();
		m_Month->Initialize();
		m_Year->Initialize();

		m_PluginManager.ReadConfigs();

	   	m_EventManager.Initialize(m_PluginManager);

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
		CToolTip::Instance().SetMaxWidth(CConfig::Instance().GetToolTipMaxWidth());

		RegisterHotkeys();

		// Rescan the languages and re-read the current
		c_Language.ScanLanguages(NULL);	// Use the same path
		bool languageFound = false;
		for (UINT i = 0; i < c_Language.GetLanguages().size(); i++)
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

		Redraw();

		RemoveTrayIcon();
		if (CConfig::Instance().GetShowTrayIcon()) 
		{
			AddUpdateTrayIcon(NIM_ADD, false, 1);
		}
			
		CBackground::PollWallpaper(true);
	} 
    catch(CError& error) 
    {
		MessageBox(m_Window, error.GetString().c_str(), APPNAME, MB_OK | MB_TOPMOST | MB_ICONERROR);
		OnCommand(WM_QUIT, NULL);	// Gotta quit
	}

	DebugLog("Refresh ended (took: %i ms).", GetTickCount() - time);
}

/* 
** Redraw
**
** Redraws the window
**
*/
void CCalendarWindow::Redraw()
{
	//CConfig::Instance().BuildMonthGrid();	// Mitul
	CRainWindow::RedrawBegin();

	BackgroundCreateStruct bcs;
	bcs.pos.x = m_X;
	bcs.pos.y = m_Y;
	bcs.size.cx = m_Width;
	bcs.size.cy = m_Height;
	bcs.mode = GetBackgroundMode();
	bcs.solidBevel = GetBackgroundBevel();
	bcs.solidColor= GetBackgroundSolidColor();

	m_Background.Create(bcs);

	CRainWindow::RedrawEnd();

	SetWindowZPos(CConfig::Instance().GetWindowPos());
}

//
// Tray icon
//
//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/tooltip/usingtooltips.asp
//
// Mitul{
BOOL CCalendarWindow::AddUpdateTrayIcon(DWORD addmodi, bool balloon, int timeout) 
{
	if (!CConfig::Instance().GetShowTrayIcon()) return FALSE;

    NOTIFYICONDATA tnid;

	BOOL res;
    DWORD dwInfoFlags = NIIF_INFO;
				// Flag Description 
			// - NIIF_ERROR     An error icon. 
			// - NIIF_INFO      An information icon. 
			// - NIIF_NONE      No icon. 
			// - NIIF_WARNING   A warning icon. 
			// - NIIF_ICON_MASK Version 6.0. Reserved. 
			// - NIIF_NOSOUND   Version 6.0. Do not play the associated sound. Applies only to balloon ToolTips 
	// Load the tray icon from resources
	CImage trayImage;
	CImage trayNumbersImage;
	
	if (!trayImage.LoadResource(m_Instance, IDB_TRAY))
	   res = FALSE;
	else
	{
		// Draw the current date into the bitmap
		if (trayNumbersImage.LoadResource(m_Instance, IDB_TRAY_NUMBERS))
		{
			int w = trayNumbersImage.GetWidth();
			int h = trayNumbersImage.GetHeight() / 10;
			const SYSTEMTIME& sysTime = c_TodaysDate.GetSystemTime();
			if (sysTime.wDay > 9)
			{
				// Center the images to 16x16 icon
				trayImage.Blit(trayNumbersImage, (16 - w * 2) / 2, (16 - h) / 2, 0, (sysTime.wDay / 10) * h, w, h);
				trayImage.Blit(trayNumbersImage, (16 - w * 2) / 2 + w, (16 - h) / 2, 0, (sysTime.wDay % 10) * h, w, h);
			}
			else
			{
				// Center the image to 16x16 icon
				trayImage.Blit(trayNumbersImage, (16 - w) / 2, (16 - h) / 2, 0, (sysTime.wDay % 10) * h, w, h);
			}
		}

		HICON trayIcon;

		if (CCalendarWindow::Is2k())
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
	
		if (!trayIcon)
		{
		   res = FALSE;
		}
		else
		{
            std::string szMsg, szTitle, tpFmt;            
            szTitle = "Rainlendar";
            
			for (int index = 0; index < m_DynamicItems.size(); index++)
			{
				if ( (m_DynamicItems[index]->IsEnabled()) && (m_DynamicItems[index]->GetWinType() == RAINWINDOW_TYPE_BALLOONTIP) )
				{
					if (m_DynamicItems[index]->ToString() != NULL)
					{
						if (!szMsg.empty()) szMsg += "\n";
						szMsg += m_DynamicItems[index]->ToString();
					}
				}
			}
            
           	tnid.cbSize = sizeof(NOTIFYICONDATA);
        	tnid.hWnd = m_Window; 
        	tnid.uID = IDB_TRAY;
        	tnid.uCallbackMessage = WM_NOTIFYICON; 
        	tnid.hIcon = trayIcon;
            strncpy(tnid.szTip, szMsg.empty() ? szTitle.c_str() : (szTitle + "\n" + szMsg).c_str(), sizeof(tnid.szTip) - 1);
			tnid.szTip[sizeof(tnid.szTip) - 1] = 0;

			tnid.uFlags = NIF_ICON | NIF_MESSAGE;
        	if (balloon)
        	{
        		tnid.uFlags |= NIF_INFO;      		
	        	tnid.dwInfoFlags = dwInfoFlags;         // Icon
	            tnid.uTimeout = timeout;                   // 5 seconds
	            strncpy(tnid.szInfo, szMsg.c_str(), sizeof(tnid.szInfo) - 1);
				tnid.szInfo[sizeof(tnid.szInfo) - 1] = 0;
	            strncpy(tnid.szInfoTitle, szTitle.c_str(), sizeof(tnid.szInfoTitle) - 1);
				tnid.szInfoTitle[sizeof(tnid.szInfoTitle) - 1] = 0;
			}
			else
				tnid.uFlags |= NIF_TIP;
			res = TRUE;
		}
		
    }

	if (res)
	    return Shell_NotifyIcon(addmodi, &tnid);		//NIM_ADD, NIM_MODIFY
    else
    	return FALSE;
}

BOOL CCalendarWindow::RemoveTrayIcon() 
{ 
    BOOL res; 
    NOTIFYICONDATA tnid; 
	
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = m_Window; 
    tnid.uID = IDB_TRAY; 
	tnid.uFlags = 0;
	
    res = Shell_NotifyIcon(NIM_DELETE, &tnid);
    return res; 
} 

/*
** ExecuteEventCommand
**
** Executes a bang or command in the execute on event
*/
void CCalendarWindow::ExecuteEventCommand(const std::string& text)
{
	std::string execute = CConfig::Instance().GetEventExecute();

	if (!execute.empty()) 
	{
		// Change \r\n to "\n"
		int pos = execute.find("\r\n");
		while (pos != -1)
		{
			execute.replace(execute.begin() + pos, execute.begin() + pos + 2, "\\n");
			pos = execute.find("\r\n", pos);
		}

		pos = execute.find("%m");		// Full text
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
		ExecuteCommand(execute.c_str());
	}
}

void CCalendarWindow::ShowTodaysEvents() 
{
	// Get today's events
	const SYSTEMTIME& sysTime = c_TodaysDate.GetSystemTime();
	std::vector<CEntryEvent*> events = GetEventManager().GetEvents(sysTime.wDay, sysTime.wMonth, sysTime.wYear, EVENT_FLAG_NO_DELETED);

	if (events.size() > 0)
	{
		std::sort(events.begin(), events.end(), CompareEvents);

		for (int i = 0; i < (int)events.size(); i++)
		{
			m_MessageWindow.AddEvent(events[i]->GetGUID(), NULL);
		}

		m_MessageWindow.ShowWindow(false);
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
** ChangeMonth
**
** Show the given month in the window
**
*/
void CCalendarWindow::ChangeMonth(int Month, int Year)
{
	const SYSTEMTIME& sysTime = c_TodaysDate.GetSystemTime();
	if(Month == -1) 
	{
		Month = sysTime.wMonth;	
	}
	if(Year == -1) 
	{
		Year = sysTime.wYear;	
	}
	
	c_MonthsFirstDate = CFileTime(1, Month, Year);
}

// This is a relative version
void CCalendarWindow::ChangeMonth(int numOfMonths)
{
	const SYSTEMTIME& sysTime = c_MonthsFirstDate.GetSystemTime();

	int month = sysTime.wMonth + numOfMonths;
	int year = sysTime.wYear;

	while (month > 12)
	{
		year += 1;
		month -= 12;
	}

	while (month < 1)
	{
		year -= 1;
		month += 12;
	}

	c_MonthsFirstDate = CFileTime(1, month, year);
}

/* 
** DrawWindow
**
** Draws the calendar to DoubleBuffer
**
*/
void CCalendarWindow::DrawWindow()
{
	SYSTEMTIME sysToday = c_TodaysDate.GetSystemTime();
	SYSTEMTIME sysTime = c_MonthsFirstDate.GetSystemTime();
	UINT i, j;
	int thisMonth = sysTime.wMonth;
	int thisYear = sysTime.wYear;

	int startMonth = thisMonth - CConfig::Instance().GetPreviousMonths();
	int startYear = thisYear;

	int tmpMonth;
	int tmpYear;

	if (CConfig::Instance().GetStartFromJanuary())
	{
		startMonth = 1;
		startYear = thisYear;
	}

	while (startMonth <= 0)
	{
		startYear--;
		startMonth += 12;
	}

	tmpMonth = startMonth + CConfig::Instance().GetMaxGridMonth();
	tmpYear = startYear;
	while (tmpMonth > 12)
	{
		tmpYear++;
		tmpMonth -= 12;
	}
	bool b = ( ((sysToday.wYear + ((sysToday.wMonth - 1) / 12.0))  < (startYear + ((startMonth - 1) / 12.0))) ||
				((sysToday.wYear + ((sysToday.wMonth - 1) / 12.0))  > (tmpYear + ((tmpMonth - 1) / 12.0))) );

	m_CurrentMonthOffset.x = -1;
	m_CurrentMonthOffset.y = -1;
	m_ViewMonthOffset.x = -1;
	m_ViewMonthOffset.y = -1;

	CToolTip::Instance().DeleteAllToolTips();

	if(m_DoubleBuffer.GetBitmap()) 
	{
		m_Background.Paint(m_DoubleBuffer);		// Background is always painted first

		POINT offset; 

		for (j = 0; j < CConfig::Instance().GetVerticalCount(); j++)
		{
			offset.y = j * m_Height / CConfig::Instance().GetVerticalCount();

			for (i = 0; i < CConfig::Instance().GetHorizontalCount(); i++)
			{
				offset.x = i * m_Width / CConfig::Instance().GetHorizontalCount();

				// Mitul{
				tmpMonth = startMonth;
				tmpYear = startYear;
				if (CConfig::Instance().GetGridMonth(j, i, tmpMonth, tmpYear) < 0) continue;
				ChangeMonth(tmpMonth, tmpYear);
				if ( (tmpMonth == sysToday.wMonth) && ((tmpYear == sysToday.wYear) || (b)) )
				{
					m_CurrentMonthOffset = offset;
				}

				if ( (tmpMonth == sysTime.wMonth) && ((tmpYear == sysTime.wYear)) )
				{
					m_ViewMonthOffset = offset;
				}

				// Reset the tooltips for the new month(s) (this also removes the old tips)
				if (m_Event) m_Event->AddToolTips(this, offset);

				for (int k = 0; k < m_StaticSkinItems.size(); k++)
				{
					if (m_StaticSkinItems[k]->IsEnabled())
					{
						m_StaticSkinItems[k]->Paint(m_DoubleBuffer, offset);
					}
				}

				// Mitul : I don't know why you have two smae loops. 
				//          So I have moved this from the bottom loop
				//          to here. 
				if (m_Event && m_Event->GetEventIconEnable() && m_Event->IsEnabled())
				{
					m_Event->PaintIcons(m_DoubleBuffer, offset);
				}
				// Mitul}
			}
		}
	}

	// Reset the values
	ChangeMonth(thisMonth, thisYear);
}

void CCalendarWindow::DrawDynamic()
{
	m_DynamicBuffer.Clear();
	m_NeedsUpdating = false;

	POINT offset = {0, 0};
	for (int index = 0; index < m_DynamicSkinItems.size(); index++)
	{
		if (m_DynamicSkinItems[index]->IsEnabled())
		{
			m_NeedsUpdating = m_NeedsUpdating || m_DynamicSkinItems[index]->NeedsUpdating();

			if (m_DynamicBuffer.GetBitmap() == NULL)
			{
				m_DynamicBuffer.Create(m_Width, m_Height, 0);
			}

			if (m_DynamicSkinItems[index]->GetRepeatType() == REPEAT_TYPE_NO)
			{
				offset.x = 0;
				offset.y = 0;
				m_DynamicSkinItems[index]->Paint(m_DynamicBuffer, offset);
			}
			else if (m_DynamicSkinItems[index]->GetRepeatType() == REPEAT_TYPE_CURRENT_MONTH)
			{
				if ((m_CurrentMonthOffset.x != -1) && (m_CurrentMonthOffset.y != -1))
					m_DynamicSkinItems[index]->Paint(m_DynamicBuffer, m_CurrentMonthOffset);
			}
			else if (m_DynamicSkinItems[index]->GetRepeatType() == REPEAT_TYPE_VIEW_MONTH)
			{
				if ((m_ViewMonthOffset.x != -1) && (m_ViewMonthOffset.y != -1))
					m_DynamicSkinItems[index]->Paint(m_DynamicBuffer, m_ViewMonthOffset);
			}
			else
			{
				for (int j = 0; j < CConfig::Instance().GetVerticalCount(); j++)
				{
					offset.y = j * m_Height / CConfig::Instance().GetVerticalCount();

					for (int i = 0; i < CConfig::Instance().GetHorizontalCount(); i++)
					{
						if (CConfig::Instance().GetGridMonth(j, i) < 0) continue;

						offset.x = i * m_Width / CConfig::Instance().GetHorizontalCount();

						m_DynamicSkinItems[index]->Paint(m_DynamicBuffer, offset);
					}
				}
			}
		}
	}
	
	AddUpdateTrayIcon(NIM_MODIFY, false, 1);
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

		case CConfig::HOTKEY_TODO:
			hotkey = CConfig::Instance().GetTodoHotkey();
			break;

		case CConfig::HOTKEY_HIDE_CALENDAR:
			hotkey = CConfig::Instance().GetHideCalendarHotkey();
			break;

		case CConfig::HOTKEY_SHOW_CALENDAR:
			hotkey = CConfig::Instance().GetShowCalendarHotkey();
			break;

		case CConfig::HOTKEY_TOGGLE_CALENDAR:
			hotkey = CConfig::Instance().GetToggleCalendarHotkey();
			break;

		case CConfig::HOTKEY_HIDE_TODO:
			hotkey = CConfig::Instance().GetHideTodoHotkey();
			break;

		case CConfig::HOTKEY_SHOW_TODO:
			hotkey = CConfig::Instance().GetShowTodoHotkey();
			break;

		case CConfig::HOTKEY_TOGGLE_TODO:
			hotkey = CConfig::Instance().GetToggleTodoHotkey();
			break;

		case CConfig::HOTKEY_HIDE_EVENTS:
			hotkey = CConfig::Instance().GetHideEventsHotkey();
			break;

		case CConfig::HOTKEY_SHOW_EVENTS:
			hotkey = CConfig::Instance().GetShowEventsHotkey();
			break;

		case CConfig::HOTKEY_TOGGLE_EVENTS:
			hotkey = CConfig::Instance().GetToggleEventsHotkey();
			break;

		case CConfig::HOTKEY_NEW_EVENT:
			hotkey = CConfig::Instance().GetAddEventHotkey();
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
	GetWindowRect((HWND)wParam, &rect);
	xPos = xPos - rect.left;
	yPos = yPos - rect.top;

	HMENU menu, subMenu = NULL;
	
	menu = LoadMenu(CRainlendar::GetInstance(), MAKEINTRESOURCE(IDR_MENU1));
	if (menu) subMenu = GetSubMenu(menu, 0);

	if (subMenu)
	{
		FillMenu(subMenu, xPos, yPos, (HWND)wParam);

		HMENU showMenu = subMenu;
		if (CConfig::Instance().GetShowContextMenu())
		{
			showMenu = GetSubMenu(subMenu, 13);		// Event menu
		}

		TrackPopupMenu(
		  showMenu,
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
			RefreshAll();
			return 0;
		}

		if(wParam >= ID_SKIN && wParam < ID_SKIN_END)
		{
			// Check which config was selected
			int index = 0;
			
			for (UINT i = 0; i < m_ConfigStrings.size(); i++)
			{
				for (UINT j = 0; j < m_ConfigStrings[i].iniFiles.size(); j++)
				{
					if (index == wParam - ID_SKIN)
					{
						CConfig::Instance().SetCurrentSkin(m_ConfigStrings[i].path);
						CConfig::Instance().SetCurrentSkinIni(m_ConfigStrings[i].iniFiles[j]);
						CConfig::Instance().WriteConfig(CConfig::WRITE_CONFIG);	// Write old config (happens only if Rainlendar.ini's not modified from outside)
						RefreshAll();
						return 0;
					}
					index++;
				}
			}
			return 0;
		}

		if(wParam >= ID_EVENT_START && wParam < ID_EVENT_END)
		{
			UINT index = wParam - ID_EVENT_START;
			if (index >= 0 && index < m_SelectedEvents.size())
			{
				OpenEditEventDialog(m_Window, m_Instance, &m_SelectedEvents[index], NULL);
			}
			return 0;
		}

		switch(LOWORD(wParam)) {

		case ID_ABOUT:
			{
				char buffer[MAX_LINE_LENGTH];
				sprintf(buffer, c_Language.GetString("About", 0), VERSION, __DATE__);

				std::string about = buffer;

				const std::vector<CPlugin*>& allPlugins = m_PluginManager.GetAllPlugins();
				
				if (allPlugins.size() > 0)
				{
					about += c_Language.GetString("General", 19);
					about += ":\n";

					for (UINT i = 0; i < allPlugins.size(); i++)
					{
						about += allPlugins[i]->GetName();

						// Version
						UINT version = allPlugins[i]->GetVersion();
						sprintf(buffer, " v%i.%i", version / 1000, version % 1000);
						about += buffer;

						about += " - ";
						about += allPlugins[i]->GetAuthor();
						about += "\n";
					}
				}

				MessageBox(m_Window, about.c_str(), "Rainlendar", MB_OK);
			}
			break;

		case ID_POPUP_HELP:
			{
				std::string helpFile;
				helpFile = CConfig::Instance().GetDLLPath();
				helpFile += "Rainlendar.chm";
				ShellExecute(m_Window, "open", helpFile.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
			break;

		case ID_REFRESH:
			RefreshAll();
			break;

		case ID_POPUP_TODO_LOCKED:
			CConfig::Instance().SetTodoLocked(!CConfig::Instance().GetTodoLocked());
			m_TodoWindow.DockWindow(!CConfig::Instance().GetTodoLocked());
			CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
			break;

		case ID_POPUP_EVENTLIST_LOCKED:
			CConfig::Instance().SetEventListLocked(!CConfig::Instance().GetEventListLocked());
			m_EventListWindow.DockWindow(!CConfig::Instance().GetEventListLocked());
			CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
			break;

		case ID_POPUP_CALENDAR_VISIBLE:
			CConfig::Instance().SetCalendarEnable(!CConfig::Instance().GetCalendarEnable());
			if (CConfig::Instance().GetCalendarEnable())
			{
				ShowWindow(false);
			}
			else
			{
				HideWindow();
			}
			CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
			break;

		case ID_POPUP_TODO_VISIBLE:
			CConfig::Instance().SetTodoEnable(!CConfig::Instance().GetTodoEnable());
			if (CConfig::Instance().GetTodoEnable())
			{
				m_TodoWindow.ShowWindow(false);
			}
			else
			{
				m_TodoWindow.HideWindow();
			}
			CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
			break;

		case ID_POPUP_EVENTLIST_VISIBLE:
			CConfig::Instance().SetEventListEnable(!CConfig::Instance().GetEventListEnable());
			if (CConfig::Instance().GetEventListEnable())
			{
				m_EventListWindow.ShowWindow(false);
			}
			else
			{
				m_EventListWindow.HideWindow();
			}
			CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
			break;

		case ID_CONFIG:
			OpenConfigDialog(m_Window, CRainlendar::GetInstance());
			break;
	
		case ID_EDIT_SKIN:
			OpenSkinDialog(m_Window, CRainlendar::GetInstance());
			break;
	
		case ID_ALLEVENTS:
			if (OpenAllEventsDialog(m_Window))
			{
				RedrawAll();
			}
			break;

		case ID_POPUP_EVENT_CREATENEWEVENT:
			OpenEditEventDialog(m_Window, m_Instance, NULL, &m_NewEventStartTime);
			break;

		case ID_POPUP_EVENT_DELETEEVENTS:
			// "Are you sure you want to delete the event(s)?"
			if (IDYES == MessageBox(m_Window, c_Language.GetString("Message", 0), "Rainlendar", MB_YESNO | MB_ICONQUESTION))
			{
				for (int i = 0; i < (int)m_SelectedEvents.size(); i++)
				{
					m_EventManager.DeleteItem(&m_SelectedEvents[i]);
				}
				RedrawAll();
			}
			break;

		case ID_TODO:
			OpenTodoDialog(m_Window, CRainlendar::GetInstance(), NULL);
			break;

		case ID_QUIT:
			CToolTip::Instance().Finalize();
			CRasterizerFont::ClearBuffers();
			if(CRainlendar::GetDummyLitestep()) PostQuitMessage(0);
			DestroyWindow(m_Window);
			GetRainlendar()->Quit(CRainlendar::GetInstance());
			break;

		case ID_POPUP_SELECTMONTH_CURRENTMONTH:
			ChangeMonth(-1, -1);
			Redraw();
			break;

		case ID_POPUP_SELECTMONTH_NEXTMONTH:
			{
				int days = c_MonthsFirstDate.GetDaysInMonth();
				c_MonthsFirstDate.Add(days * 24 * 60 * 60);
				Redraw();
			}
			break;

		case ID_POPUP_SELECTMONTH_PREVMONTH:
			{
				c_MonthsFirstDate.Substract(24 * 60 * 60);
				int days = c_MonthsFirstDate.GetDaysInMonth();
				c_MonthsFirstDate.Substract((days - 1) * 24 * 60 * 60);
				Redraw();
			}
			break;

		case ID_POPUP_SELECTMONTH_NEXTYEAR:
			{
				const SYSTEMTIME& sysTime = c_MonthsFirstDate.GetSystemTime();
				c_MonthsFirstDate = CFileTime(1, sysTime.wMonth, sysTime.wYear + 1);
				Redraw();
			}
			break;

		case ID_POPUP_SELECTMONTH_PREVYEAR:
			{
				const SYSTEMTIME& sysTime = c_MonthsFirstDate.GetSystemTime();
				c_MonthsFirstDate = CFileTime(1, sysTime.wMonth, sysTime.wYear - 1);
				Redraw();
			}
			break;

		case ID_POPUP_SELECTMONTH_JANUARY:
			ChangeMonth(1, -1);
			Redraw();
			break;

		case ID_POPUP_SELECTMONTH_FEBRUARY:
			ChangeMonth(2, -1);
			Redraw();
			break;

		case ID_POPUP_SELECTMONTH_MARCH:
			ChangeMonth(3, -1);
			Redraw();
			break;

		case ID_POPUP_SELECTMONTH_APRIL:
			ChangeMonth(4, -1);
			Redraw();
			break;

		case ID_POPUP_SELECTMONTH_MAY:
			ChangeMonth(5, -1);
			Redraw();
			break;

		case ID_POPUP_SELECTMONTH_JUNE:
			ChangeMonth(6, -1);
			Redraw();
			break;

		case ID_POPUP_SELECTMONTH_JULY:
			ChangeMonth(7, -1);
			Redraw();
			break;

		case ID_POPUP_SELECTMONTH_AUGUST:
			ChangeMonth(8, -1);
			Redraw();
			break;

		case ID_POPUP_SELECTMONTH_SEPTEMBER:
			ChangeMonth(9, -1);
			Redraw();
			break;

		case ID_POPUP_SELECTMONTH_OCTOBER:
			ChangeMonth(10, -1);
			Redraw();
			break;

		case ID_POPUP_SELECTMONTH_NOVEMBER:
			ChangeMonth(11, -1);
			Redraw();
			break;

		case ID_POPUP_SELECTMONTH_DECEMBER:
			ChangeMonth(12, -1);
			Redraw();
			break;

		case ID_POPUP_LANGUAGE_GETMORELANGUAGES:
			LSExecute(NULL, "http://www.rainlendar.net/Rainlendar.html", SW_SHOWNORMAL);
			break;

		case ID_POPUP_SKINS_GETMORESKINS:
			LSExecute(NULL, "http://customize.org/list/rainlendar", SW_SHOWNORMAL);
			break;
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
	if(wParam == WINDOW_TIMER) 
	{
		bool redraw = false;

		CFileTime today;
		today.SetToLocalTime();

		const SYSTEMTIME& sysTime = c_TodaysDate.GetSystemTime();
		int day = sysTime.wDay;	// Store the day since the next line overwrites the data
		const SYSTEMTIME& sysTime2 = today.GetSystemTime();

		if (day != sysTime2.wDay)
		{
			// If the day change happens when the computer is not on (e.g. during hibernate)
			// do a delayed redraw so that the window is drawn properly.
			if (sysTime2.wHour != 0 || sysTime2.wMinute != 0)
			{
				SetTimer(m_Window, REDRAW_TIMER, 15000, NULL);		// Wait 15 secs before redrawing
				DebugLog("Noticed a day change. Redrawing the window in 15 secs...");
			}

			// Day changed!
			c_TodaysDate = today;
			ChangeMonth(-1, -1);
			redraw = true;
		}

		// Check for alarms
		if (CConfig::Instance().GetShowSingleEvent()) 
		{
			bool checkAlarms = false;
			if (m_LastAlarmTime.GetAsSeconds() == 0 || today.GetAsSeconds() - m_LastAlarmTime.GetAsSeconds() >= 60)
			{
				bool newEvents = false;

				const SYSTEMTIME& sysTime = today.GetSystemTime();
				m_LastAlarmTime = CFileTime(sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, 0);
				int now = sysTime.wHour * 60 + sysTime.wMinute;

				for (int j = 0; j < 2; j++)
				{
					// Get today's events
					const SYSTEMTIME& sysTime = today.GetSystemTime();
					std::vector<CEntryEvent*>& events = GetEventManager().GetEvents(sysTime.wDay, sysTime.wMonth, sysTime.wYear, EVENT_FLAG_NO_DELETED);

					int i;
					for (i = 0; i < (int)events.size(); i++)
					{
						int alarm = events[i]->GetAlarm();
						if (alarm >= 0)
						{
							CFileTime start = events[i]->GetStartTime();
							const SYSTEMTIME& sysStartTime = start.GetSystemTime();
							int alarmTime = sysStartTime.wHour * 60 + sysStartTime.wMinute - alarm;

							if (alarmTime == now)
							{
								int hour = sysStartTime.wHour;
								int minute = sysStartTime.wMinute;
								const SYSTEMTIME& sysTime = today.GetSystemTime();

								CFileTime alarmTime(sysTime.wDay, sysTime.wMonth, sysTime.wYear, hour, minute, 0);
								m_MessageWindow.AddEvent(events[i]->GetGUID(), &alarmTime);
								newEvents = true;

								std::string text;
								events[i]->GetBriefMessage(text, 0, true, true);
								ExecuteEventCommand(text);
							}
						}
					}

					// Goto tomorrow
					today.Add(24 * 60 * 60);
					now = now - 24 * 60;
				}

				if (newEvents)
				{
					m_MessageWindow.ShowWindow(false);
				}
			}
		}

		if (CBackground::PollWallpaper(false))
		{
			redraw = true;
		}
		m_Background.FlushWallpaper();		// Get rid of the wallpaper

		// Flush all possible buffered items
		if (m_EventManager.FlushBufferedItems(false))
		{
			redraw = true;
		}

		if (redraw)
		{
			RedrawAll();
		}
	}
	else if(wParam == REDRAW_TIMER) 
	{
		DebugLog("... delayed redraw.");

		KillTimer(m_Window, REDRAW_TIMER);
		RedrawAll();
	}
	else if(wParam == REFRESH_TIMER) 
	{
		DebugLog("... delayed refresh.");

		KillTimer(m_Window, REFRESH_TIMER);
		RefreshAll();
	}

	if (CConfig::Instance().GetCalendarEnable())
	{
		return CRainWindow::OnTimer(wParam, lParam);
	}
	return 0;
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

	if (GetBackgroundMode() == MODE_COPY || m_Background.HasAlpha())
	{
		if (!CConfig::Instance().GetNativeTransparency())
		{
			m_Background.UpdateWallpaper(x, y);
			DrawWindow();
			InvalidateRect(m_Window, NULL, false);
		}
	}

	CRainWindow::OnMove(wParam, lParam);

	return 0;
}

LRESULT CCalendarWindow::OnLButtonDblClk(WPARAM wParam, LPARAM lParam) 
{
	POINT pos;
	pos.x = (SHORT)LOWORD(lParam);
	pos.y = (SHORT)HIWORD(lParam);

	if(m_Message == WM_NCLBUTTONDBLCLK)
	{
		// Transform the point to client rect
		MapWindowPoints(NULL, m_Window, &pos, 1);
	}

	m_NewEventStartTime = c_TodaysDate;
	std::vector<CEntryEvent*>* events = m_Event->HitTest(pos.x, pos.y, &m_NewEventStartTime);
	GUID* guid = NULL;
	UINT i = 0;

	switch(CConfig::Instance().GetCalendarAction()) 
	{
	case 1:		// Edit
		if (events->size() != 0)
		{
			for (i = 0 ; i < events->size(); i++)
			{
				if (!(*events)[i]->IsReadOnly())
				{
					guid = (*events)[i]->GetGUID();
					break;
				}
			}
		}
		OpenEditEventDialog(m_Window, m_Instance, guid, &m_NewEventStartTime);
		break;

	case 2:		// Create
		OpenEditEventDialog(m_Window, m_Instance, NULL, &m_NewEventStartTime);
		break;

	case 3:		// Delete
		m_SelectedEvents.clear();

		for (i = 0 ; i < events->size(); i++)
		{
			if (!(*events)[i]->IsReadOnly())
			{
				m_SelectedEvents.push_back(*(*events)[i]->GetGUID());
			}
		}
		if (!m_SelectedEvents.empty())
		{
			OnCommand(ID_POPUP_EVENT_DELETEEVENTS, 0);
		}
		break;

	default:
		if(events) 
		{
			if (events->size() == 0)
			{
				OpenEditEventDialog(m_Window, m_Instance, NULL, &m_NewEventStartTime);
			}
			else
			{
				HMENU menu = LoadMenu(m_Instance, MAKEINTRESOURCE(IDR_MENU1));
				HMENU subMenu = GetSubMenu(menu, 0);
				HMENU eventsMenu = GetSubMenu(subMenu, 13);		// Event menu
					
				ModifyMenu(eventsMenu, ID_POPUP_EVENT_CREATENEWEVENT, MF_BYCOMMAND | MF_STRING, ID_POPUP_EVENT_CREATENEWEVENT, c_Language.GetString("Menu", 45));
				ModifyMenu(eventsMenu, ID_POPUP_EVENT_DELETEEVENTS, MF_BYCOMMAND | MF_STRING, ID_POPUP_EVENT_DELETEEVENTS, c_Language.GetString("Menu", 46));

				m_SelectedEvents.clear();

				bool enableDeleted = true;
				for (i = 0; i < events->size(); i++)
				{
					std::string brief;
					(*events)[i]->GetBriefMessage(brief, 100, false, false);
					m_SelectedEvents.push_back(*(*events)[i]->GetGUID());
					InsertMenu(eventsMenu, i, MF_BYPOSITION, ID_EVENT_START + i, brief.c_str());
					if ((*events)[i]->IsReadOnly())
					{
						EnableMenuItem(eventsMenu, i, MF_BYPOSITION | MF_GRAYED);
						enableDeleted = false;
					}
				}

				if (!enableDeleted)
				{
					EnableMenuItem(eventsMenu, ID_POPUP_EVENT_DELETEEVENTS, MF_GRAYED);
				}

				InsertMenu(eventsMenu, i, MF_BYPOSITION | MF_SEPARATOR, 0, 0);

				MapWindowPoints(m_Window, NULL, &pos, 1);

				TrackPopupMenu(
				  eventsMenu,
				  TPM_RIGHTBUTTON | TPM_LEFTALIGN, 
				  pos.x,
				  pos.y,
				  0,
				  m_Window,
				  NULL
				);

				DestroyMenu(menu);
			}
		}
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

	if (CRainlendar::GetDummyLitestep() && !CConfig::Instance().GetDisableHotkeys())
	{
		if (GetAsyncKeyState(VK_MENU) && wParam == VK_F4 && m_QuitSet)
		{
			PostQuitMessage(0);
		}
	}

	m_QuitSet = false;

	return 0;
}

LRESULT CCalendarWindow::OnSysKeyDown(WPARAM wParam, LPARAM lParam) 
{
	if (GetAsyncKeyState(VK_MENU) && wParam == VK_F4)
	{
		m_QuitSet = true;
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
		RainlendarToggle(m_Window, "visible");
	}
	else if (hotkey == CConfig::Instance().GetActivateHotkey())
	{
    	RainlendarShow(m_Window, "activate");
	}
	else if (hotkey == CConfig::Instance().GetRefreshHotkey())
	{
		RainlendarRefresh(m_Window, NULL);
	}
	else if (hotkey == CConfig::Instance().GetConfigHotkey())
	{
		OnCommand(ID_CONFIG, NULL);
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
	else if (hotkey == CConfig::Instance().GetTodoHotkey())
	{
		OnCommand(ID_TODO, NULL); 
	}
	if (hotkey == CConfig::Instance().GetHideCalendarHotkey())
	{
		CConfig::Instance().SetCalendarEnable(false);
		HideWindow();
		CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
	}
	else if (hotkey == CConfig::Instance().GetShowCalendarHotkey())
	{
		CConfig::Instance().SetCalendarEnable(true);
		ShowWindow(false);
		CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
	}
	else if (hotkey == CConfig::Instance().GetToggleCalendarHotkey())
	{
		CConfig::Instance().SetCalendarEnable(!CConfig::Instance().GetCalendarEnable());
		ToggleWindow();
		CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
	}
	if (hotkey == CConfig::Instance().GetHideTodoHotkey())
	{
		CConfig::Instance().SetTodoEnable(false);
		m_TodoWindow.HideWindow();
		CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
	}
	else if (hotkey == CConfig::Instance().GetShowTodoHotkey())
	{
		CConfig::Instance().SetTodoEnable(true);
		m_TodoWindow.ShowWindow(false);
		CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
	}
	else if (hotkey == CConfig::Instance().GetToggleTodoHotkey())
	{
		CConfig::Instance().SetTodoEnable(!CConfig::Instance().GetTodoEnable());
		m_TodoWindow.ToggleWindow();
		CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
	}
	if (hotkey == CConfig::Instance().GetHideEventsHotkey())
	{
		CConfig::Instance().SetEventListEnable(false);
		m_EventListWindow.HideWindow();
		CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
	}
	else if (hotkey == CConfig::Instance().GetShowEventsHotkey())
	{
		CConfig::Instance().SetEventListEnable(true);
		m_EventListWindow.ShowWindow(false);
		CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
	}
	else if (hotkey == CConfig::Instance().GetToggleEventsHotkey())
	{
		CConfig::Instance().SetEventListEnable(!CConfig::Instance().GetEventListEnable());
		m_EventListWindow.ToggleWindow();
		CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
	}
	else if (hotkey == CConfig::Instance().GetAddEventHotkey())
	{
		m_NewEventStartTime = c_TodaysDate;
		OnCommand(ID_POPUP_EVENT_CREATENEWEVENT, NULL); 
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

		DebugLog("Executing bang: %s", str.c_str());

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
			RainlendarToggle(m_Window, arg.c_str());
		}
		else if (stricmp(bang.c_str(), "!RainlendarHide") == 0)
		{
			RainlendarHide(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarRefresh") == 0)
		{
			RainlendarRefresh(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarRedraw") == 0)
		{
			RainlendarRedraw(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarConfig") == 0)
		{
			RainlendarConfig(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarQuit") == 0)
		{
			RainlendarQuit(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarEditSkin") == 0)
		{
			RainlendarEditSkin(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarShowNext") == 0)
		{
			RainlendarShowNext(m_Window, arg.c_str());
		}
		else if (stricmp(bang.c_str(), "!RainlendarShowPrev") == 0)
		{
			RainlendarShowPrev(m_Window, arg.c_str());
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
		else if (stricmp(bang.c_str(), "!RainlendarShowEventList") == 0)
		{
			RainlendarShowEventList(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarHideEventList") == 0)
		{
			RainlendarHideEventList(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarToggleEventList") == 0)
		{
			RainlendarToggleEventList(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarShowCalendar") == 0)
		{
			RainlendarShowCalendar(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarHideCalendar") == 0)
		{
			RainlendarHideCalendar(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarToggleCalendar") == 0)
		{
			RainlendarToggleCalendar(m_Window, NULL);
		}
		else if (stricmp(bang.c_str(), "!RainlendarShowItem") == 0)
		{
			RainlendarShowItem(m_Window, arg.c_str());
		}
		else if (stricmp(bang.c_str(), "!RainlendarHideItem") == 0)
		{
			RainlendarHideItem(m_Window, arg.c_str());
		}
		else if (stricmp(bang.c_str(), "!RainlendarToggleItem") == 0)
		{
			RainlendarToggleItem(m_Window, arg.c_str());
		}
		else if (stricmp(bang.c_str(), "!RainlendarChangeSkin") == 0)
		{
			RainlendarChangeSkin(m_Window, arg.c_str());
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
		// Mitul{
		else if (stricmp(bang.c_str(), "!RainlendarShowTodaysEvent") == 0)
		{
			RainlendarShowTodaysEvent(m_Window, arg.c_str());
		}
		// Mitul}
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

	case NIN_BALLOONTIMEOUT:		// Don't know if these are supported
	case NIN_BALLOONUSERCLICK:		// in Pre-XP windows or not. As per
	case NIN_BALLOONHIDE:			// MSDN - NO :(
		AddUpdateTrayIcon(NIM_MODIFY, false, 1);
		break;
		
	case WM_CONTEXTMENU:			// Keyboard - Shortcut key
	case NIN_KEYSELECT:				// Keyboard - Spacebar or Enter
	case NIN_SELECT:				// Mouse - Enter key
		uMouseMsg = WM_RBUTTONDOWN;
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
	else if	(uMouseMsg == WM_LBUTTONDOWN)
	{
		AddUpdateTrayIcon(NIM_MODIFY, true, 1000);
	}
	else if	(uMouseMsg == WM_RBUTTONDOWN)
	{
		POINT point;
		GetCursorPos(&point);

		SetForegroundWindow(m_Window);

		bool menuStatus = CConfig::Instance().GetShowContextMenu();
		CConfig::Instance().SetShowContextMenu(false);		// Show always the full menu
		OnContextMenu(0, MAKELPARAM(point.x, point.y));
		CConfig::Instance().SetShowContextMenu(menuStatus);

		SetForegroundWindow(m_Window);
	}

	return TRUE;
}


LRESULT CCalendarWindow::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int dir = 1;

	if ((short)HIWORD(wParam) < 0)
	{
		dir = -1;
	}

	ChangeMonth(CConfig::Instance().GetWheelScrollMonths() * dir);
	Redraw();

	return 0;
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
				return Window->OnContextMenu((WPARAM)hWnd, lParam);

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

			case WM_SYSKEYDOWN:
				DebugLog("WM_SYSKEYDOWN: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnSysKeyDown(wParam, lParam);
				break;

			case WM_SYSKEYUP:
				DebugLog("WM_SYSKEYUP: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnSysKeyUp(wParam, lParam);

			case WM_COPYDATA: 
				return Window->OnCopyData(wParam, lParam);
				
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
				return Window->OnSettingsChange(wParam, lParam);

			case WM_ERASEBKGND: 
				return Window->OnEraseBkgnd(wParam, lParam);

			case WM_MOUSEMOVE: 
				Window->m_Message = WM_MOUSEMOVE;
				return Window->OnMouseMove(wParam, lParam);

			case WM_NCMOUSEMOVE: 
				Window->m_Message = WM_NCMOUSEMOVE;
				return Window->OnMouseMove(wParam, lParam);

			case WM_LBUTTONDOWN: 
			case WM_NCLBUTTONDOWN: 
				return Window->OnLButtonDown(wParam, lParam);

			case WM_LBUTTONUP: 
			case WM_NCLBUTTONUP: 
				return Window->OnLButtonUp(wParam, lParam);

			case 0x020A: // WM_MOUSEWHEEL
				return Window->OnMouseWheel(wParam, lParam);

			case WM_DISPLAYCHANGE: 
				DebugLog("WM_DISPLAYCHANGE: wParam=0x%08x, lParam=0x%08x", wParam, lParam);
				return Window->OnDisplayChange(wParam, lParam);

			case WM_NCHITTEST: 
				return Window->OnNcHitTest(wParam, lParam);

			case WM_NOTIFYICON: 
				return Window->OnNotifyIcon(wParam, lParam);

			case WM_DROPFILES: 
				return Window->OnDropFiles(wParam, lParam);

			case WM_DESTROY:
				if (CRainlendar::GetDummyLitestep())
				{
					PostQuitMessage(0);
				}
				return TRUE;
		}

		if (Window->m_ToolBarMessage == uMsg)
		{
			Window->AddUpdateTrayIcon(NIM_ADD, false, 1);
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

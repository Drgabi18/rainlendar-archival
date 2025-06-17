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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/CalendarWindow.cpp,v 1.14 2002/11/12 18:05:46 rainy Exp $

  $Log: CalendarWindow.cpp,v $
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
#include <crtdbg.h>

#define ULW_ALPHA               0x00000002
#define WS_EX_LAYERED           0x00080000

// Timers
#define DATE_TIMER 1

// Defines
#define SNAPDISTANCE 10

// Class variables
CConfig CCalendarWindow::c_Config;
SYSTEMTIME CCalendarWindow::c_TodaysDate;
SYSTEMTIME CCalendarWindow::c_MonthsFirstDate;

/* 
** CCalendarWindow
**
** Constructor
**
*/
CCalendarWindow::CCalendarWindow() 
{
	m_Width=0;
	m_Height=0;
	m_FirstExecute=true;
	m_SelectedDay=0;
	m_MenuSelectedDay=0;

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
	m_DllInstance = GetModuleHandle("Rainlendar.dll");
	m_Rainlendar = &Rainlendar;

	std::string PixPath, EventsPath, SkinsPath;
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

	if (EventsPath.empty())
	{
		EventsPath = PixPath + "Events.ini";
	}

	if (SkinsPath.empty())
	{
		// Get the DLL's directory
		GetModuleFileName(m_DllInstance, tmpName, MAX_LINE_LENGTH);
		char* Slash = strrchr(tmpName, '\\');
		*(Slash + 1) = 0;	// Cut the Rainlendar.dll from the name
		SkinsPath = tmpName;
		SkinsPath += "Skins\\";
	}

	LSLog(LOG_DEBUG, "Rainlendar", "Rainlendar paths:");
	LSLog(LOG_DEBUG, "Rainlendar", PixPath.c_str());
	LSLog(LOG_DEBUG, "Rainlendar", EventsPath.c_str());
	LSLog(LOG_DEBUG, "Rainlendar", SkinsPath.c_str());

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
	
	ShowMonth(-1, -1);
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

	HMENU monthsMenu = GetSubMenu(subMenu, 2);

	if (monthsMenu)
	{
		// Change the months in the submenu
		std::string monthNames = c_Config.GetMonthNames();
		std::string name;
		int year = c_TodaysDate.wYear + 1;

		for(int i = 0; i < 12; i++) 
		{
			if (i + 1 == c_TodaysDate.wMonth) year--;

			sprintf(tmpSz, " %i\tF%i", year, i+1);
			name = c_Config.GetMonthName(i);
			name += tmpSz;

			ModifyMenu(monthsMenu, i + 4, MF_BYPOSITION | MF_STRING, GetMenuItemID(monthsMenu, i + 4), name.c_str());
		}
	}
	
	// If mouse was pressed on a day, enable set event

	m_MenuSelectedDay = m_Days->HitTest(x, y);

	if (m_MenuSelectedDay != 0) 
	{
		EnableMenuItem( subMenu, ID_SETEVENT, MF_ENABLED );
	} 
	else 
	{
		EnableMenuItem( subMenu, ID_SETEVENT, MF_GRAYED );
	}

	if(!CRainlendar::GetDummyLitestep())
	{
		// Disable QUIT if LiteStep's plugin 
		EnableMenuItem( subMenu, ID_QUIT, MF_GRAYED );
	}

	int index = 0;

	// Fill the menu with all the configs
	HMENU configMenu = CreatePopupMenu();
	if(configMenu)
	{
		InsertMenu(subMenu, 6, MF_BYPOSITION | MF_POPUP, (UINT_PTR)configMenu, "Skins");

		for(int i = 0; i < m_ConfigStrings.size(); i++)
		{
			if (m_ConfigStrings[i].iniFiles.size() > 1)
			{
				HMENU iniMenu = CreatePopupMenu();
				InsertMenu(configMenu, i, MF_BYPOSITION | MF_POPUP, (UINT_PTR)iniMenu, m_ConfigStrings[i].path.c_str());
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
				InsertMenu(configMenu, i, MF_BYPOSITION, ID_SKIN + index++, m_ConfigStrings[i].path.c_str());

				if(m_ConfigStrings[i].path == c_Config.GetCurrentSkin())
				{
					CheckMenuItem(configMenu, i, MF_BYPOSITION | MF_CHECKED);
				}
			}
		}

		InsertMenu(configMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		InsertMenu(configMenu, 0, MF_BYPOSITION, ID_EDIT_SKIN, "Edit Current Skin...");
	}
}

/* 
** CalcWindowPos
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
		}

		CRasterizerFont::ClearBuffers();

		// Only days and event days change the day types
		m_Days->ResetDayTypes();
		m_Event->ResetDayTypes();

		// Reset the tooltips for the new month (this also removes the old tips)
		m_Event->AddToolTips(this);

		if (!lite)
		{
			int X, Y;
			RECT r;

			CalcWindowSize();

			// Handle negative coordinates
			GetClientRect(GetDesktopWindow(), &r); 
			X = c_Config.GetX();
			Y = c_Config.GetY();
			if (X < 0) X += r.right - r.left;
			if (Y < 0) Y += r.bottom - r.top;

			if(m_FirstExecute || (CCalendarWindow::c_Config.GetBGCopyMode() == CConfig::BG_WALLPAPER_ALWAYS ||
								 (CCalendarWindow::c_Config.GetBGCopyMode() == CConfig::BG_NORMAL && !(m_Rainlendar->IsWharf()))))
			{
				CalcWindowSize();
				if (m_Background.Create(X, Y, m_Width, m_Height))
				{
					CalcWindowSize();	// Must be recalced if BG is bigger than current window
				}
			}

			HWND winPos = HWND_NOTOPMOST;
			UINT flags = SWP_NOACTIVATE;
			LONG style = GetWindowLong(m_Window, GWL_STYLE);

			switch (c_Config.GetWindowPos())
			{
			case CConfig::WINDOWPOS_ONTOP:
				winPos = HWND_TOPMOST;
				if (!m_Rainlendar->IsWharf())
				{
					style = (style & ~WS_CHILD) | WS_POPUP;		// Change from child to popup
					SetWindowLong(m_Window, GWL_STYLE, style);
					SetParent(m_Window, NULL);
				}
				break;

			case CConfig::WINDOWPOS_ONBOTTOM:
				winPos = HWND_BOTTOM;
				if (!m_Rainlendar->IsWharf())
				{
					style = (style & ~WS_CHILD) | WS_POPUP;		// Change from child to popup
					SetWindowLong(m_Window, GWL_STYLE, style);
					SetParent(m_Window, NULL);
				}
				break;

			// CConfig::WINDOWPOS_ONDESKTOP: is handled below
			}

			if(m_Rainlendar->IsWharf()) flags |= SWP_NOZORDER;		// Children shouldn't change z-position

			SetWindowPos(m_Window, winPos, X, Y, m_Width, m_Height, flags);
		}

		// Create the DoubleBuffer
		m_DoubleBuffer.Create(m_Width, m_Height, 0x0);	// Fully transparent

		DrawCalendar();

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
				
				// Only Check if this months events are initalized
				if(CCalendarWindow::c_MonthsFirstDate.wMonth == CCalendarWindow::c_TodaysDate.wMonth &&
					CCalendarWindow::c_MonthsFirstDate.wYear == CCalendarWindow::c_TodaysDate.wYear) 
				{
					if(c_Config.GetEventEnable()) 
					{
						std::string text;
						m_Event->GetEventText(c_TodaysDate.wDay, text);

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
			}
			
			PollWallpaper(true);
		}

		UpdateTransparency();

		InvalidateRect(m_Window, NULL, false);

		if (!lite)
		{
			if (c_Config.GetWindowPos() == CConfig::WINDOWPOS_ONDESKTOP)
			{
				if (!m_Rainlendar->IsWharf())
				{
					// Set the window's parent to progman, so it stays always on desktop
					HWND ProgmanHwnd = FindWindow("Progman", "Program Manager");
					if (ProgmanHwnd && GetParent(m_Window) != ProgmanHwnd)
					{
						LONG style = GetWindowLong(m_Window, GWL_STYLE);
						style = (style & ~WS_POPUP) | WS_CHILD;
						SetWindowLong(m_Window, GWL_STYLE, style);
						SetParent(m_Window, ProgmanHwnd);
					}
				}
			}
		}

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
			std::string errMsg = "Unable to find the skin: ";
			errMsg += c_Config.GetCurrentSkin();
			errMsg += "\\";
			errMsg += c_Config.GetCurrentSkinIni();
			errMsg += "\nRainlendar will default to the first available skin.";
			MessageBox(m_Window, errMsg.c_str(), "Rainlendar", MB_OK);

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
** ShowMonth
**
** Show the given month in the window
**
*/
void CCalendarWindow::ShowMonth(int Month, int Year)
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

	char buffer[256];
	sprintf(buffer, "New month %i-%i. First weekday: %i.", Month, Year, day);
	LSLog(LOG_DEBUG, "Rainlendar", buffer);
}

/* 
** DrawCalendar
**
** Draws the calendar to DoubleBuffer
**
*/
void CCalendarWindow::DrawCalendar()
{
	if(m_DoubleBuffer.GetBitmap()) 
	{
		m_Background.Paint(m_DoubleBuffer);

		if(c_Config.GetYearEnable()) m_Year->Paint(m_DoubleBuffer);
		if(c_Config.GetMonthEnable()) m_Month->Paint(m_DoubleBuffer);

		if(c_Config.GetDaysEnable()) 
		{
			m_Days->Paint(m_DoubleBuffer);
			if(c_Config.GetEventEnable()) m_Event->Paint(m_DoubleBuffer);
			if(c_Config.GetWeekdaysEnable()) m_Weekdays->Paint(m_DoubleBuffer);
			if(c_Config.GetWeekNumbersEnable()) m_WeekNumbers->Paint(m_DoubleBuffer);
			if(c_Config.GetTodayEnable()) m_Today->Paint(m_DoubleBuffer);
		}

		if (CRasterizerFont::GetColorBuffer().GetBitmap() != NULL  && CRasterizerFont::GetAlphaBuffer().GetBitmap() != NULL)
		{
			CImage textImage;
			textImage.Create(CRasterizerFont::GetColorBuffer().GetBitmap(), CRasterizerFont::GetAlphaBuffer().GetBitmap(), 0);
			m_DoubleBuffer.Blit(textImage, 0, 0, 0, 0, textImage.GetWidth(), textImage.GetHeight());
		}
	}
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

		// TEST
//		m_DoubleBuffer.MultiplyAlpha();
//		HBITMAP bm = CreateBitmap(m_Width, m_Height, 1, 16, NULL);
//		HBITMAP bm = CreateCompatibleBitmap(tmpDC, m_Width, m_Height);

//		VOID *pvBits;
//		BITMAPINFO bmi;
//		ZeroMemory(&bmi, sizeof(BITMAPINFO));
//		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//		bmi.bmiHeader.biWidth = 200;
//		bmi.bmiHeader.biHeight = 200;
//		bmi.bmiHeader.biPlanes = 1;
//		bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
//		bmi.bmiHeader.biCompression = BI_RGB;
//		bmi.bmiHeader.biSizeImage = 200 * 200 * 4;
//		
//		// create our DIB section and select the bitmap into the dc
//		HBITMAP bm = CreateDIBSection(winDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
//    		SelectObject(tmpDC, bm);
//
//		HBRUSH br = CreateSolidBrush(RGB(255,0,0));
//		RECT r = {0, 0, 100, 100 };
//		FillRect(tmpDC, &r, br);
//		DeleteObject(br);
		
		// ~TEST

		BitBlt(winDC, 0, 0, m_Width, m_Height, tmpDC, 0, 0, SRCCOPY);

		// TEST
		//HBRUSH br = CreateSolidBrush(RGB(255,0,0));
		//RECT r = {0, 0, 100, 100 };
		//FillRect(winDC, &r, br);
		//DeleteObject(br);
		// ~TEST

		SelectObject(tmpDC, oldBitmap);
		DeleteDC(tmpDC);

		EndPaint(m_Window, &ps);
	}
	return 0;
}

LRESULT CCalendarWindow::OnContextMenu(WPARAM wParam, LPARAM lParam) 
{
	int xPos = LOWORD(lParam); 
	int yPos = HIWORD(lParam); 

	// Transform the point to client rect
	RECT rect;
	GetWindowRect(m_Window, &rect);
	xPos = xPos - rect.left;
	yPos = yPos - rect.top;

	HMENU menu, subMenu = NULL;
	
	menu = LoadMenu(m_DllInstance, MAKEINTRESOURCE(IDR_MENU1));
	if (menu) subMenu = GetSubMenu(menu, 0);

	if (subMenu)
	{
		FillMenu(subMenu, xPos, yPos);

		TrackPopupMenu(
		  subMenu,
		  TPM_RIGHTBUTTON | TPM_LEFTALIGN, 
		  LOWORD(lParam),
		  HIWORD(lParam),
		  0,
		  m_Window,
		  NULL
		);		
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
		}

		switch(LOWORD(wParam)) {

		case ID_ABOUT:
			{
				std::string sz;
				sz = "Rainlendar version ";
				sz += VERSION;
				sz += "\n(Build on ";
				sz += __DATE__;
				sz += ")\n\nGet the latest version from:\nwww.iki.fi/rainy\n\n";
									
				MessageBox(m_Window, sz.c_str(), "Rainlendar", MB_OK);
			}
			break;

		case ID_REFRESH:
			Refresh();
			break;

		case ID_CONFIG:
			LSLog(LOG_DEBUG, "Rainlendar", "Opening Config dialog.");
			OpenConfigDialog(m_Window, m_DllInstance);
			LSLog(LOG_DEBUG, "Rainlendar", "Config dialog closed.");
			break;
	
		case ID_EDIT_SKIN:
			LSLog(LOG_DEBUG, "Rainlendar", "Opening Edit Skin dialog.");
			OpenSkinDialog(m_Window, m_DllInstance);
			LSLog(LOG_DEBUG, "Rainlendar", "Edit Skin dialog closed.");
			break;
	
		case ID_SETEVENT:
			// Fake doubleclick action will show the edit event dialog
			m_SelectedDay = m_MenuSelectedDay;
			OnLButtonDblClk(0, 0);
			m_MenuSelectedDay = 0;
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
			ShowMonth(Month, Year);
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

	if (c_Config.GetServerEnable())
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

			ShowMonth(-1, -1);
			Refresh(true);
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
		RECT r;
		GetClientRect(GetDesktopWindow(), &r);
		int w = r.right - m_Width;
		int h = r.bottom - m_Height;

		if ((wp->x < SNAPDISTANCE) && (wp->x > -SNAPDISTANCE)) wp->x = 0;
		if ((wp->y < SNAPDISTANCE) && (wp->y > -SNAPDISTANCE)) wp->y = 0;
		if ((wp->x < SNAPDISTANCE + w) && (wp->x > -SNAPDISTANCE + w)) wp->x = w;
		if ((wp->y < SNAPDISTANCE + h) && (wp->y > -SNAPDISTANCE + h)) wp->y = h;
	}

	return DefWindowProc(m_Window, m_Message, wParam, lParam);
}

LRESULT CCalendarWindow::OnMove(WPARAM wParam, LPARAM lParam) 
{
	short int x = LOWORD(lParam);
	short int y = HIWORD(lParam);

	if(c_Config.GetMovable())
	{
		c_Config.SetX(x<0?0:x);
		c_Config.SetY(y<0?0:y);
		c_Config.WriteConfig(CConfig::WRITE_POS);	// Store the new position to the ini file

		if (!m_FirstExecute && (c_Config.GetBackgroundMode() == CBackground::MODE_COPY || m_Background.HasAlpha()))
		{
			m_Background.UpdateWallpaper(x, y);
			DrawCalendar();
			InvalidateRect(m_Window, NULL, false);
		}
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

LRESULT CCalendarWindow::OnDisplayChange(WPARAM wParam, LPARAM lParam) 
{
	// Refresh when the resolution changes
	Refresh();

	return 0;
}

LRESULT CCalendarWindow::OnLButtonDblClk(WPARAM wParam, LPARAM lParam) 
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	if(m_Message == WM_NCLBUTTONDBLCLK)
	{
		// Transform the point to client rect
		RECT rect;
		GetWindowRect(m_Window, &rect);
		x = x - rect.left;
		y = y - rect.top;
	}

	if (m_SelectedDay == 0)
	{
		m_SelectedDay = m_Days->HitTest(x, y);
	}

	if(m_SelectedDay != 0) 
	{
		LSLog(LOG_DEBUG, "Rainlendar", "Opening Edit Event dialog.");
		if (DialogBox(m_DllInstance, MAKEINTRESOURCE(IDD_EDITEVENT), m_Window, (DLGPROC)EditEventProc) == IDOK) 
		{
			LSLog(LOG_DEBUG, "Rainlendar", "OK pressed. Sending Events to the server.");

			ConnectServer(REQUEST_SENDEVENTS);	// Send the events to the server
			Refresh(true);
		}
		m_SelectedDay = 0;

		LSLog(LOG_DEBUG, "Rainlendar", "Edit Event dialog closed.");
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
		else if (stricmp((const char*)pCopyDataStruct->lpData, "!RainlendarShow") == 0)
		{
			RainlendarShow(m_Window, NULL);
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
		}
		else if (stricmp((const char*)pCopyDataStruct->lpData, "!RainlendarShowCurrent") == 0)
		{
			RainlendarShowCurrent(m_Window, NULL);
		}
		else if (stricmp((const char*)pCopyDataStruct->lpData, "!RainlendarLsBoxHook") == 0)
		{
			// Do nothing
		}
		else
		{
			std::string error = "Unknown !bang: ";
			error += (const char*)pCopyDataStruct->lpData;
			MessageBox(m_Window, error.c_str(), "Rainlendar", MB_OK);
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
	END_MESSAGEPROC
}

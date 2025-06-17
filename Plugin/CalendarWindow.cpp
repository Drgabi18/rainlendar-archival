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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/CalendarWindow.cpp,v 1.9 2002/05/30 18:29:35 rainy Exp $

  $Log: CalendarWindow.cpp,v $
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

// Timers
#define DATE_TIMER 1

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
	m_DoubleBuffer=0;

	m_Days = NULL;
	m_Event = NULL;
	m_Today = NULL;
	m_Weekdays = NULL;
	m_WeekNumbers = NULL;
	m_Month = NULL;
	m_Year = NULL;
	m_ConnectionCounter = 0;
	m_Hidden = false;
}

/* 
** ~CMeterWindow
**
** Destructor
**
*/
CCalendarWindow::~CCalendarWindow()
{
	if(m_DC) DeleteObject(m_DC);
	if(m_DoubleBuffer) DeleteObject(m_DoubleBuffer);

	delete m_Days;
	delete m_Event;
	delete m_Today;
	delete m_Weekdays;
	delete m_WeekNumbers;
	delete m_Month;
	delete m_Year;

	if(m_Window) DestroyWindow(m_Window);

	BOOL Result;
	int counter = 0;
	do {
		// Wait for the window to die
		Result = UnregisterClass("Rainlendar", m_Instance);
		Sleep(100);
		counter += 1;
	} while(!Result && counter < 10);
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

	if(Parent==NULL || Instance==NULL) 
	{
		throw CError(CError::ERR_NULLPARAMETER, __LINE__, __FILE__);
	}

	m_Instance = GetModuleHandle("Rainlendar.dll");
	m_Rainlendar = &Rainlendar;

	// Register the windowclass
	memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.style = CS_NOCLOSE | CS_DBLCLKS;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = Instance;
	wc.lpszClassName = "Rainlendar";
	
	if(!RegisterClassEx(&wc))
	{
		throw CError(CError::ERR_WINDOWCLASS, __LINE__, __FILE__);
	}

	if(Rainlendar.IsWharf())
	{
		// If we're in lsBox or in wharf, this is a childwindow
		flags = WS_CHILD;
	}
	else
	{
		// ... otherwise this is normal popup window
		flags = WS_POPUP;
	}

	m_Window = CreateWindowEx(WS_EX_TOOLWINDOW, 
							"Rainlendar", 
							NULL, 
							flags,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							Parent,
							NULL,
							Instance,
							this);

	if(m_Window == NULL) 
	{ 
		throw CError(CError::ERR_WINDOW, __LINE__, __FILE__);
	}

	SetWindowLong(m_Window, GWL_USERDATA, magicDWord);

	std::string PixPath, EventsPath;

	if(!CRainlendar::GetDummyLitestep()) 
	{
		GetRCString("RainlendarPath", tmpName, "C:\\Litestep\\Rainlendar\\", MAX_LINE_LENGTH);
		VarExpansion(tmpName, tmpName);
		PixPath = tmpName;

		GetRCString("RainlendarEventsPath", tmpName, "", MAX_LINE_LENGTH);
		VarExpansion(tmpName, tmpName);
		EventsPath = tmpName;

		// Check that the path ends with "\" 
		if (!EventsPath.empty() && *(EventsPath.end() - 1) != '\\')
		{
			EventsPath += "\\";
		}
		EventsPath += "Events.ini";
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
		// Get the DLL's directory
		GetModuleFileName(m_Instance, tmpName, MAX_LINE_LENGTH);

		char* Slash = strrchr(tmpName, '\\');
		if(Slash == NULL) 
		{
			strcpy(tmpName, "Events.ini");
		} 
		else 
		{
			strcpy(Slash, "\\Events.ini");
		}
		EventsPath = tmpName;
	}

	c_Config.SetPath(PixPath);
	c_Config.SetEventsPath(EventsPath);
	c_Config.ReadConfig();

    INITCOMMONCONTROLSEX iccex; 
	iccex.dwICC = ICC_BAR_CLASSES;
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCommonControlsEx(&iccex);

	// Create the tooltip window
	m_ToolTip = CreateWindowEx(WS_EX_TOPMOST,
							TOOLTIPS_CLASS,
							NULL,
							WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP | TTS_BALLOON,
							0, 0,
							0, 0,
							NULL, NULL,
							Instance,
							NULL);

	SendMessage(m_ToolTip, TTM_SETMAXTIPWIDTH, 0, 255);

	GetLocalTime(&c_TodaysDate);
	// Store current date (the month that will be shown)
	c_MonthsFirstDate = c_TodaysDate;

	if(!c_Config.GetStartHidden()) ShowWindow();
	
	ShowMonth(-1, -1);
	
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

	m_SelectedDay = m_Days->HitTest(x, y);

	if (m_SelectedDay != 0) 
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
void CCalendarWindow::Refresh()
{
	try 
	{
		RECT r;
		int X, Y;

		c_Config.WriteConfig();	// Write old config (happens only if Rainlendar.ini's not modified from outside)
		c_Config.ReadConfig();	// Read new config

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

		if(!m_Rainlendar->IsWharf() || m_FirstExecute)
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

		m_Event->AddToolTips(this);

		CalcWindowSize();

		// Handle negative coordinates
		GetClientRect(GetDesktopWindow(), &r); 
		X = c_Config.GetX();
		Y = c_Config.GetY();
		if (X < 0) X += r.right - r.left;
		if (Y < 0) Y += r.bottom - r.top;

		if(!m_Rainlendar->IsWharf() || m_FirstExecute)
		{
			if(c_Config.GetBackgroundMode() == CBackground::MODE_COPY ||
			   c_Config.GetBackgroundBitmapName().empty()) 
			{
				m_Background.CopyBackground(X, Y, m_Width, m_Height);
			}
			else 
			{
				m_Background.Load(c_Config.GetBackgroundBitmapName());
				CalcWindowSize();	// Must be recalced if BG is bigger than current window
				m_Background.Create(X, Y, m_Width, m_Height);
			}
		}

		if (c_Config.GetWindowPos() == CConfig::WINDOWPOS_ONTOP)
		{
			SetWindowPos(m_Window, HWND_TOPMOST, X, Y, m_Width, m_Height, SWP_NOACTIVATE);
		}
		else
		{
			SetWindowPos(m_Window, HWND_NOTOPMOST, X, Y, m_Width, m_Height, SWP_NOACTIVATE);
		}

		// Create the DoubleBuffer
		if(m_DoubleBuffer) DeleteObject(m_DoubleBuffer);
		HWND DeskTop = GetDesktopWindow();
		HDC DDC = GetDC(DeskTop);
		m_DoubleBuffer = CreateCompatibleBitmap(DDC, m_Width, m_Height);
		ReleaseDC(DeskTop, DDC);

		DrawCalendar();

		if(c_Config.GetBackgroundMode() != CBackground::MODE_COPY && !m_Background.HasAlpha())
		{
			// Set window region
			HRGN region = BitmapToRegion(m_DoubleBuffer, RGB(255,0,255), 0x101010, 0, 0);
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
				CEventMessage* TodaysEvent = m_Event->GetEvent(c_TodaysDate.wDay);
				if(c_Config.GetEventEnable() && TodaysEvent!=NULL) 
				{
					if(c_Config.GetEventMessageBox()) 
					{
						MessageBox(m_Window, TodaysEvent->GetMessage().c_str(), "Rainlendar", MB_OK | MB_TOPMOST);
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
								execute.replace(execute.begin() + pos, execute.begin() + pos + 1, TodaysEvent->GetMessage());
							}
							LSExecute(NULL, execute.c_str(), SW_SHOWNORMAL);
						}
					}
				} 
			}
		}

		PollWallpaper(true);

		InvalidateRect(m_Window, NULL, false);
	} 
    catch(CError& error) 
    {
		MessageBox(m_Window, error.GetString().c_str(), APPNAME, MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
		OnCommand(WM_QUIT, NULL);	// Gotta quit
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
				}
				else
				{
					if (m_WallpaperName != str)
					{
						// If the names differ, refresh
						Refresh();
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

	Refresh();
}

/* 
** DrawCalendar
**
** Draws the calendar to DoubleBuffer
**
*/
void CCalendarWindow::DrawCalendar()
{
	if(m_DoubleBuffer) 
	{
		HDC tmpDC = CreateCompatibleDC(NULL);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(tmpDC, m_DoubleBuffer);

		m_Background.Paint(tmpDC);

		if(c_Config.GetYearEnable()) m_Year->Paint(tmpDC);
		if(c_Config.GetMonthEnable()) m_Month->Paint(tmpDC);

		if(c_Config.GetDaysEnable()) 
		{
			m_Days->Paint(tmpDC);
			if(c_Config.GetEventEnable()) m_Event->Paint(tmpDC);
			if(c_Config.GetWeekdaysEnable()) m_Weekdays->Paint(tmpDC);
			if(c_Config.GetWeekNumbersEnable()) m_WeekNumbers->Paint(tmpDC);
			if(c_Config.GetTodayEnable()) m_Today->Paint(tmpDC);
		}

		SelectObject(tmpDC, oldBitmap);
		DeleteObject(tmpDC);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCalendarWindow message handlers

LRESULT CCalendarWindow::OnPaint(WPARAM wParam, LPARAM lParam) 
{
	if(m_DoubleBuffer) 
	{
		PAINTSTRUCT ps;
		HDC winDC = BeginPaint(m_Window, &ps);
		HDC tmpDC = CreateCompatibleDC(NULL);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(tmpDC, m_DoubleBuffer);
		
		BitBlt(winDC, 0, 0, m_Width, m_Height, tmpDC, 0, 0, SRCCOPY);

		SelectObject(tmpDC, oldBitmap);
		DeleteObject(tmpDC);

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
	
	menu = LoadMenu(m_Instance, MAKEINTRESOURCE(IDR_MENU1));
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

		DestroyMenu(menu);
	}

	return 0;
}

LRESULT CCalendarWindow::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	try 
	{
		bool Handled = false;
		bool Selected = false;
		int Month, Year;

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
			OpenConfigDialog(m_Window, m_Instance);
			break;

		case ID_SETEVENT:
			// Fake doubleclick action will show the edit event dialog
			OnLButtonDblClk(0, 0);
			m_SelectedDay = 0;
			break;

		case ID_QUIT:
			c_Config.WriteConfig();
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
		CreateThread(NULL, 0, NetworkThreadProc, &param, 0, &id);
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
			// c_MonthsFirstDate is set always to the first day of the month
			c_MonthsFirstDate = c_TodaysDate;
			c_MonthsFirstDate.wHour = 0;
			c_MonthsFirstDate.wMinute = 0;
			c_MonthsFirstDate.wSecond = 0;
			c_MonthsFirstDate.wMilliseconds = 0;
			c_MonthsFirstDate.wDay = 1;
			
			m_FirstExecute = true;				// Act like Rainlendar was first time executed
			Refresh();
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
	if(c_Config.GetWindowPos() == CConfig::WINDOWPOS_ONBOTTOM)
	{
		// do not change the z-order. This keeps the window on bottom.
		LPWINDOWPOS wp=(LPWINDOWPOS)lParam;
		wp->flags|=SWP_NOZORDER;
	}

	return 0;
}

LRESULT CCalendarWindow::OnMove(WPARAM wParam, LPARAM lParam) 
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);

	if(c_Config.GetMovable())
	{
		// Store the new coordinates
		c_Config.SetX(x);
		c_Config.SetY(y);

		if (!m_FirstExecute && (c_Config.GetBackgroundMode() == CBackground::MODE_COPY || m_Background.HasAlpha()))
		{
			Refresh();
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

LRESULT CCalendarWindow::OnNotify(WPARAM wParam, LPARAM lParam) 
{
	LPNMHDR pNMHDR = (LPNMHDR)lParam;

	if(pNMHDR->code == TTN_NEEDTEXT)
	{
		static char ToolTipText[MAX_LINE_LENGTH];

		CEventMessage* Event = m_Event->GetEvent(pNMHDR->idFrom);
		if (Event) 
		{
			strncpy(ToolTipText, Event->GetMessage().c_str(), MAX_LINE_LENGTH - 1);
			ToolTipText[MAX_LINE_LENGTH - 1] = 0;
			TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
			pTTT->lpszText = ToolTipText;
			return TRUE;
		}
	}

	return FALSE;
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
		if (DialogBox(m_Instance, MAKEINTRESOURCE(IDD_EDITEVENT), m_Window, (DLGPROC)EditEventProc) == IDOK) 
		{
			Refresh();
			ConnectServer(REQUEST_SENDEVENTS);	// Send the events to the server
		}
		m_SelectedDay = 0;
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
	MESSAGE(OnNotify, WM_NOTIFY)
	MESSAGE(OnCopyData, WM_COPYDATA)
	END_MESSAGEPROC
}

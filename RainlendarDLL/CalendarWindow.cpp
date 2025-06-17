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

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "CalendarWindow.h"
#include "Config.h"
#include "Error.h"
#include "EditEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define VERSION "0.6"

// Timers
#define REFRESH_START_TIMER 1
#define REFRESH_END_TIMER 2
#define DATE_TIMER 3

CConfig CCalendarWindow::C_Config;		// Static instance of CConfig
CTime CCalendarWindow::C_TodaysDate;
CTime CCalendarWindow::C_CurrentDate;

/////////////////////////////////////////////////////////////////////////////
// CCalendarWindow

CCalendarWindow::CCalendarWindow() 
{
	m_Background.SetWindow(this);
	m_Width=0;
	m_Height=0;
	m_Refreshing=false;
	m_Litestep=false;
	m_FirstExecute=true;
	m_SelectedDay=0;
	m_DoubleBuffer=0;
	m_FirstRun=true;
}

CCalendarWindow::~CCalendarWindow()
{
	if(m_DoubleBuffer) delete m_DoubleBuffer;
}


BEGIN_MESSAGE_MAP(CCalendarWindow, CWnd)
	//{{AFX_MSG_MAP(CCalendarWindow)
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_QUIT, OnQuit)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_MESSAGE(LM_GETREVID, OnGetRevID)
	ON_WM_TIMER()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_CONFIG, OnConfig)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_SETEVENT, OnSetevent)
	ON_WM_ERASEBKGND()
	ON_WM_KEYUP()
	ON_WM_SYSKEYUP()
	//}}AFX_MSG_MAP

	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify )

END_MESSAGE_MAP()

/* 
** Initialize
**
** Initializes the plugin.
**
*/
void CCalendarWindow::Initialize(HWND ParentWnd, HINSTANCE dllInst, LPCSTR IniPath)
{
	try {
		LPCTSTR Classname;
		char tmpName[MAX_LINE_LENGTH];
		CString PixPath;
		CRect R(0,0,0,0);
		int Msgs[]={LM_GETREVID, 0};

		m_DllInstance=dllInst;

		if(IniPath==NULL) m_Litestep=true;	// Ran as Litestep plugin and not standalone

		Classname=AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1));

		if(Classname==NULL) throw ERR_WINDOWCLASS;

		if(!CreateEx(WS_EX_TOOLWINDOW, Classname, "Rainlendar",  WS_POPUP, R, NULL, NULL, NULL)) throw ERR_WINDOW;

		SetWindowLong(*this, GWL_USERDATA, magicDWord);
		// Register RevID message to Litestep
		if(m_Litestep) ::SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)m_hWnd, (LPARAM)Msgs);

		if(m_Litestep) {
			GetRCString("RainlendarPath", tmpName, "C:\\Litestep\\Rainlendar\\", MAX_LINE_LENGTH);
			PixPath=tmpName;

			// Add bangs only if Litestep is running
			AddBangCommand("!RainlendarToggle", RainlendarToggle);
			AddBangCommand("!RainlendarHide", RainlendarHide);
			AddBangCommand("!RainlendarShow", RainlendarShow);
			AddBangCommand("!RainlendarRefresh", RainlendarRefresh);
			AddBangCommand("!RainlendarConfig", RainlendarConfig);
			AddBangCommand("!RainlendarQuit", RainlendarQuit);
			AddBangCommand("!RainlendarShowNext", RainlendarShowNext);
			AddBangCommand("!RainlendarShowPrev", RainlendarShowPrev);
			AddBangCommand("!RainlendarShowMonth", RainlendarShowMonth);
			AddBangCommand("!RainlendarShowCurrent", RainlendarShowCurrent);
		} else {
			PixPath=IniPath;
		}

		if(!PixPath.IsEmpty() && PixPath[PixPath.GetLength()-1]!='\\') {
			PixPath+="\\";
		}

		C_Config.SetPath(PixPath);
		C_Config.ReadConfig();

		m_ToolTip.Create(this, TTS_ALWAYSTIP);
		m_ToolTip.SetMaxTipWidth(256);

		C_TodaysDate=CTime::GetCurrentTime();	// Store today
		// Store current date (the month that will be shown)
		// C_CurrentDate is set always to the first day of the month
		C_CurrentDate=CTime(C_TodaysDate.GetYear(), C_TodaysDate.GetMonth(), 1, 0, 0, 0);

		m_Refreshing=true;
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		SetTimer(REFRESH_START_TIMER, C_Config.GetStartDelay()*1000, NULL);	

		SetTimer(DATE_TIMER, 1000, NULL);	// Date is checked once per second

	} catch(CString Filename) {
		CString ErrorStr;
		ErrorStr.Format("File not found: %s", Filename);
		::MessageBox(NULL, ErrorStr, APPNAME, MB_OK | MB_ICONERROR);

		throw ERR_NONE;
	}
	// No RAIN_ERRORs are caught here!!!
}

/* 
** FillMenu
**
** Fills the context menu with months
**
*/
void CCalendarWindow::FillMenu(CMenu& Menu, CPoint point)
{
	CString tmpString;
	CString& MonthNames=C_Config.GetMonthNames();
	int i, Index, Start, Year;
	POINT Point=point;

	CMenu* Popup=Menu.GetSubMenu(0);
	CMenu* Months=Popup->GetSubMenu(0);

	Year=C_TodaysDate.GetYear()+1;
	Start=0;
	for(i=0; i<12; i++) {
		if(i+1==C_TodaysDate.GetMonth()) Year--;

		Index=MonthNames.Find('/', Start);
		if(Index==-1) Index=MonthNames.GetLength();

		tmpString.Format("%s %i\tF%i", MonthNames.Mid(Start, Index-Start), Year, i+1);
		Months->ModifyMenu(i+4, MF_BYPOSITION | MF_STRING, Months->GetMenuItemID(i+4), tmpString);

		Start=Index+1;
	}

	ScreenToClient(&Point);
	m_SelectedDay=m_Days.HitTest(Point.x, Point.y);
	if(m_SelectedDay!=0) {
		Popup->EnableMenuItem( ID_SETEVENT, MF_ENABLED );
	} else {
		Popup->EnableMenuItem( ID_SETEVENT, MF_GRAYED );
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

	if(C_Config.GetDaysEnable()) {
		m_Width=max(m_Width, m_Days.GetX()+m_Days.GetW());
		m_Height=max(m_Height, m_Days.GetY()+m_Days.GetH());
	}
	if(C_Config.GetWeekdaysEnable()) {
		m_Width=max(m_Width, m_Weekdays.GetX()+m_Weekdays.GetW());
		m_Height=max(m_Height, m_Weekdays.GetY()+m_Weekdays.GetH());
	}
	if(C_Config.GetMonthEnable()) {
		m_Width=max(m_Width, m_Month.GetX()+m_Month.GetW());
		m_Height=max(m_Height, m_Month.GetY()+m_Month.GetH());
	}
	if(C_Config.GetYearEnable()) {
		m_Width=max(m_Width, m_Year.GetX()+m_Year.GetW());
		m_Height=max(m_Height, m_Year.GetY()+m_Year.GetH());
	}

	if(m_Background.GetWidth()>m_Width) m_Width=m_Background.GetWidth();
	if(m_Background.GetHeight()>m_Height) m_Height=m_Background.GetHeight();
}


/* 
** Refresh
**
** Refresh the window
**
*/
void CCalendarWindow::Refresh()
{
	try {
		CRect r;
		int X, Y;

		// Initialize
		m_Background.Initialize();
		m_Days.Initialize();
		m_Event.Initialize();
		m_Today.Initialize();
		m_Weekdays.Initialize();
		m_Month.Initialize();
		m_Year.Initialize();

		m_Event.AddToolTips(this);

		CalcWindowSize();

		// Handle negative coordinates
		GetDesktopWindow()->GetClientRect(&r); 
		X=C_Config.GetX();
		Y=C_Config.GetY();
		if(X<0) X+=r.Width();
		if(Y<0) Y+=r.Height();

		// Set window on top, so that desktop background is copied correctly.
		// The window is set to bottom with the first Paint()
		SetWindowPos(NULL, X, Y, m_Width, m_Height, SWP_NOACTIVATE | SWP_NOZORDER);

		if(C_Config.GetBackgroundBitmapName().IsEmpty()) {
			m_Background.CopyBackground(m_Width, m_Height);
		} else {
			m_Background.Load(C_Config.GetBackgroundBitmapName());
			CalcWindowSize();	// Must be recalced if BG is bigger than current window
			SetWindowPos(NULL, X, Y, m_Width, m_Height, SWP_NOACTIVATE | SWP_NOZORDER);
			m_Background.Create(m_Width, m_Height);
		}

		// Create the DoubleBuffer
		if(m_DoubleBuffer) delete m_DoubleBuffer;
		m_DoubleBuffer=new CBitmap;

		CWnd* DeskTop=GetDesktopWindow();
		CDC* DDC=DeskTop->GetDC();
		m_DoubleBuffer->CreateCompatibleBitmap(DDC, m_Width, m_Height);
		DeskTop->ReleaseDC(DDC);

		DrawCalendar();

		// Execute on event if this was first refresh (meaning that Rainlendar was started)
		if(m_FirstExecute) { 
			m_FirstExecute=false;
			
			// Only Check if this months events are initalized
			if(CCalendarWindow::C_CurrentDate.GetMonth()==CCalendarWindow::C_TodaysDate.GetMonth() &&
				CCalendarWindow::C_CurrentDate.GetYear()==CCalendarWindow::C_TodaysDate.GetYear()) {
	
				CEvent* TodaysEvent=m_Event.GetEvent(C_TodaysDate.GetDay());
				if(C_Config.GetEventEnable() && TodaysEvent!=NULL) {
					if(C_Config.GetEventMessageBox()) {
						MessageBox(TodaysEvent->GetMessage(), "Rainlendar", MB_OK | MB_TOPMOST);
					}
					CString& Execute=C_Config.GetEventExecute();
					if(!Execute.IsEmpty()) {
						CString tmpString;
						tmpString=Execute;
						tmpString.Replace("%m", TodaysEvent->GetMessage());
						LSExecute(NULL, tmpString, SW_SHOWNORMAL);
					}
				} 
			}
		}

	} catch(CString Filename) {
		CString ErrorStr;
		ErrorStr.Format("File not found: %s", Filename);
		::MessageBox(NULL, ErrorStr, APPNAME, MB_OK | MB_ICONERROR);

	} catch(RAIN_ERROR Err) {
		if(Err!=ERR_NONE) {
			CString ErrorStr;
			ErrorStr.Format("Unable to refresh Rainlendar!\nError:%s", GetErrorMessage(Err));

			::MessageBox(NULL, ErrorStr, APPNAME, MB_OK | MB_ICONERROR);

			OnQuit();	// Error, we have to quit!
		}
	}
}

/* 
** QuitModule
**
** Called when the module is quitted. Removes the bangs is necessary.
**
*/
void CCalendarWindow::QuitModule()
{
	int Msgs[]={LM_GETREVID, 0};

	if(m_Litestep) {
		// Unregister RevID message
		if(m_hWnd) ::SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)m_hWnd, (LPARAM)Msgs);

		RemoveBangCommand("!RainlendarToggle");
		RemoveBangCommand("!RainlendarHide");
		RemoveBangCommand("!RainlendarShow");
		RemoveBangCommand("!RainlendarRefresh");
		RemoveBangCommand("!RainlendarConfig");
		RemoveBangCommand("!RainlendarQuit");
		RemoveBangCommand("!RainlendarShowNext");
		RemoveBangCommand("!RainlendarShowPrev");
		RemoveBangCommand("!RainlendarShowMonth");
		RemoveBangCommand("!RainlendarShowCurrent");
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
	if(GetStyle() & WS_VISIBLE) {
		HideWindow();
	} else {
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
	if(Month==-1) {
		Month=C_TodaysDate.GetMonth();	
	}
	if(Year==-1) {
		Year=C_TodaysDate.GetYear();	
	}

	CTime NewDate(Year, Month, 1, 0, 0, 0);
	C_CurrentDate=NewDate;
	OnRefresh();
}

/* 
** DrawCalendar
**
** Draws the calendar to DoubleBuffer
**
*/
void CCalendarWindow::DrawCalendar()
{
	if(m_DoubleBuffer) {
		CDC tmpDC;
		tmpDC.CreateCompatibleDC(NULL);
		CBitmap* OldBitmap=tmpDC.SelectObject(m_DoubleBuffer);

		m_Background.Paint(tmpDC);

		if(C_Config.GetYearEnable()) m_Year.Paint(tmpDC);
		if(C_Config.GetMonthEnable()) m_Month.Paint(tmpDC);

		if(C_Config.GetDaysEnable()) {
			m_Days.Paint(tmpDC);
			if(C_Config.GetEventEnable()) m_Event.Paint(tmpDC);
			if(C_Config.GetWeekdaysEnable()) m_Weekdays.Paint(tmpDC);
			if(C_Config.GetTodayEnable()) m_Today.Paint(tmpDC);
		}

		tmpDC.SelectObject(OldBitmap);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCalendarWindow message handlers

void CCalendarWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(m_Refreshing) return;	// No painting while refreshing

	if(m_DoubleBuffer) {
		CDC tmpDC;
		tmpDC.CreateCompatibleDC(NULL);
		CBitmap* OldBitmap=tmpDC.SelectObject(m_DoubleBuffer);
		dc.BitBlt(0, 0, m_Width, m_Height, &tmpDC, 0, 0, SRCCOPY);
		tmpDC.SelectObject(OldBitmap);
	}

}

void CCalendarWindow::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu Menu;

	if(Menu.LoadMenu(IDR_MENU1)) {
		FillMenu(Menu, point);
		CMenu* Popup=Menu.GetSubMenu(0);
		Popup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN, point.x, point.y, this);
	}
}

void CCalendarWindow::OnQuit() 
{
	if(!m_Litestep) PostQuitMessage(0);
	DestroyWindow();
}

void CCalendarWindow::OnRefresh() 
{
	m_Refreshing=true;

	// Set window to top, so that the desktop can be painted on it fully
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	SetTimer(REFRESH_START_TIMER, C_Config.GetRefreshDelay(), NULL);	
}

void CCalendarWindow::OnConfig() 
{
	CPropertySheet Dialog;

	Dialog.AddPage(&m_DialogGeneral);
	Dialog.AddPage(&m_DialogDays);
	Dialog.AddPage(&m_DialogToday);
	Dialog.AddPage(&m_DialogWeekdays);
	Dialog.AddPage(&m_DialogMonth);
	Dialog.AddPage(&m_DialogYear);
	Dialog.AddPage(&m_DialogEvent);
	Dialog.SetTitle("Rainlendar Config");

	if(IDOK==Dialog.DoModal()) {
		C_Config.WriteConfig();
	} else {
		C_Config.ReadConfig();
	} 

	OnRefresh();
}

void CCalendarWindow::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==DATE_TIMER) {
		CTime Today=CTime::GetCurrentTime();
		if( C_TodaysDate.GetYear()!=Today.GetYear() ||
			C_TodaysDate.GetMonth()!=Today.GetMonth() ||
			C_TodaysDate.GetDay()!=Today.GetDay())
		{
			// Day changed!
			
			// Move mouse so that screensaver is deactivated
			POINT Pos;
			GetCursorPos(&Pos);
			SetCursorPos(Pos.x+50, Pos.y+50);
			
			C_TodaysDate=CTime::GetCurrentTime();
			C_CurrentDate=CTime(C_TodaysDate.GetYear(), C_TodaysDate.GetMonth(), 1, 0, 0, 0);
			nIDEvent=REFRESH_START_TIMER;		// This causes refresh!
			m_FirstExecute=true;				// Act like Rainlendar was first time executed
		}
	}

	if(nIDEvent==REFRESH_START_TIMER) {
		KillTimer(REFRESH_START_TIMER);
		// After the delay, let's refresh
		Refresh();

		// If the calendar was set to show hidden hide it
		if(m_FirstRun && C_Config.GetStartHidden()) {
			CWnd::ShowWindow(SW_HIDE);
		} else {
			SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}
		m_FirstRun=false;

		// We'll have to wait a while so thet the window has time to reposition itself
		SetTimer(REFRESH_END_TIMER, C_Config.GetRefreshDelay(), NULL);	
	
	} else if(nIDEvent==REFRESH_END_TIMER) {
		KillTimer(REFRESH_END_TIMER);
		// Refreshing is over, we can disable the Z-positioning of the window
		m_Refreshing=false;
		Invalidate();
	} 

	CWnd::OnTimer(nIDEvent);
}

// Litestep revision control
// Not rcs-style but who cares
afx_msg LRESULT CCalendarWindow::OnGetRevID(WPARAM wParam, LPARAM lParam) {
	char* Buffer=(char*)lParam;
	CString Version;

	if(wParam==0) {
		Version.Format("Rainlendar.dll: %s", VERSION);
	} else if(wParam==1) {
		Version.Format("Rainlendar.dll: %s %s, Rainy", VERSION, __DATE__);
	}

	strcpy(Buffer, Version);

	return strlen(Buffer);
}

void CCalendarWindow::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	// Disable Z-order unless we are refreshing
	if(!m_Refreshing) lpwndpos->flags|=SWP_NOZORDER;

	CWnd::OnWindowPosChanging(lpwndpos);
}


BOOL CCalendarWindow::DestroyWindow() 
{
	KillTimer(REFRESH_START_TIMER);	
	KillTimer(REFRESH_END_TIMER);	
	KillTimer(DATE_TIMER);	

	return CWnd::DestroyWindow();
}

void CCalendarWindow::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CWnd::OnSettingChange(uFlags, lpszSection);
	
	// If wallpapaer is changed, refresh
	if(uFlags==SPI_SETDESKWALLPAPER) {
		OnRefresh();
	}	
}

void CCalendarWindow::PostNcDestroy() 
{
	if(!m_Litestep) delete this;
	
	CWnd::PostNcDestroy();
}

BOOL CCalendarWindow::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	static char ToolTipText[MAX_LINE_LENGTH];
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT*)pNMHDR;

	CEvent* Event=m_Event.GetEvent(pNMHDR->idFrom);
	if(Event) {
		strncpy(ToolTipText, Event->GetMessage(), MAX_LINE_LENGTH-1);
		ToolTipText[MAX_LINE_LENGTH-1]='\0';
		pTTT->lpszText=ToolTipText;
		return TRUE;
	}
	return FALSE;
}

BOOL CCalendarWindow::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	bool Handled=false;
	bool Selected=false;
	int Month, Year;

	switch(LOWORD(wParam)) {
	case ID_POPUP_SELECTMONTH_CURRENTMONTH:
		Month=C_TodaysDate.GetMonth();
		Year=C_TodaysDate.GetYear();
		Handled=true;
		break;

	case ID_POPUP_SELECTMONTH_NEXTMONTH:
		Month=C_CurrentDate.GetMonth()+1;
		if(Month==13) {
			Year=C_CurrentDate.GetYear()+1;
			Month=1;
		} else {
			Year=C_CurrentDate.GetYear();
		}
		Handled=true;
		break;

	case ID_POPUP_SELECTMONTH_PREVMONTH:
		Month=C_CurrentDate.GetMonth()-1;
		if(Month==0) {
			Year=C_CurrentDate.GetYear()-1;
			Month=12;
		} else {
			Year=C_CurrentDate.GetYear();
		}
		Handled=true;
		break;

	case ID_POPUP_SELECTMONTH_JANUARY:
		Month=1;
		Selected=true;
		break;

	case ID_POPUP_SELECTMONTH_FEBRUARY:
		Month=2;
		Selected=true;
		break;

	case ID_POPUP_SELECTMONTH_MARCH:
		Month=3;
		Selected=true;
		break;

	case ID_POPUP_SELECTMONTH_APRIL:
		Month=4;
		Selected=true;
		break;

	case ID_POPUP_SELECTMONTH_MAY:
		Month=5;
		Selected=true;
		break;

	case ID_POPUP_SELECTMONTH_JUNE:
		Month=6;
		Selected=true;
		break;

	case ID_POPUP_SELECTMONTH_JULY:
		Month=7;
		Selected=true;
		break;

	case ID_POPUP_SELECTMONTH_AUGUST:
		Month=8;
		Selected=true;
		break;

	case ID_POPUP_SELECTMONTH_SEPTEMBER:
		Month=9;
		Selected=true;
		break;

	case ID_POPUP_SELECTMONTH_OCTOBER:
		Month=10;
		Selected=true;
		break;

	case ID_POPUP_SELECTMONTH_NOVEMBER:
		Month=11;
		Selected=true;
		break;

	case ID_POPUP_SELECTMONTH_DECEMBER:
		Month=12;
		Selected=true;
		break;

	}

	if(Selected) {
		Year=C_TodaysDate.GetYear()+1;
		if(Month>=C_TodaysDate.GetMonth()) Year--;
	}

	if(Handled || Selected) {
		CTime NewDate(Year, Month, 1, 0, 0, 0);
		C_CurrentDate=NewDate;
		OnRefresh();
		return TRUE;
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

void CCalendarWindow::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int Day=m_Days.HitTest(point.x, point.y);
	
	if(Day!=0) {
		CEditEvent EditEvent;
		
		EditEvent.SetDay(Day);
		if(IDOK==EditEvent.DoModal()) {
			EditEvent.WriteEvent();
			OnRefresh();
		}
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CCalendarWindow::OnSetevent() 
{
	CEditEvent EditEvent;
	
	if(m_SelectedDay!=0) {
		EditEvent.SetDay(m_SelectedDay);
		if(IDOK==EditEvent.DoModal()) {
			EditEvent.WriteEvent();
			OnRefresh();
		}
		m_SelectedDay=0;
	}
}


void CCalendarWindow::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(!C_Config.GetDisableHotkeys()) 
	{
		if( (GetAsyncKeyState(VK_SHIFT)) == 0 &&
			(GetAsyncKeyState(VK_CONTROL)) == 0 &&
			(GetAsyncKeyState(VK_MENU)) == 0 &&
			(GetAsyncKeyState(VK_LWIN)) == 0 &&
			(GetAsyncKeyState(VK_RWIN)) == 0) {

			switch(nChar) {
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
	
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL CCalendarWindow::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}


// F10 Generates a syskey-message   ... wierd.
void CCalendarWindow::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(!C_Config.GetDisableHotkeys()) 
	{
		if( (GetAsyncKeyState(VK_SHIFT)) == 0 &&
			(GetAsyncKeyState(VK_CONTROL)) == 0 &&
			(GetAsyncKeyState(VK_MENU)) == 0 &&
			(GetAsyncKeyState(VK_LWIN)) == 0 &&
			(GetAsyncKeyState(VK_RWIN)) == 0) {

			switch(nChar) {
			case VK_F10:
				OnCommand(ID_POPUP_SELECTMONTH_OCTOBER, NULL);
				break;
			}
		}
	}
	
	CWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

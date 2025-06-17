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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/CalendarWindow.h,v 1.7 2002/02/27 19:12:50 rainy Exp $

  $Log: CalendarWindow.h,v $
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

  Revision 1.4  2002/01/15 17:59:18  rainy
  Changed the way refreshing is done.

  Revision 1.3  2002/01/10 16:49:14  rainy
  The items weren't deallocated during refresh. Fixed and changed them to pointers.

  Revision 1.2  2001/12/23 10:02:55  rainy
  Rainlendar.dll uses a bit different interface.

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#if !defined(AFX_CALENDARWINDOW_H__C7D0FDB9_F062_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_CALENDARWINDOW_H__C7D0FDB9_F062_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalendarWindow.h : header file
//
#include "..\..\..\3rdparty\ls-b24\lsapi\lsapi.h"
#include "Background.h"
#include "Config.h"
#include "ItemDays.h"
#include "ItemEvent.h"
#include "ItemToday.h"
#include "ItemWeekdays.h"
#include "ItemWeekNumbers.h"
#include "ItemMonth.h"
#include "ItemYear.h"
#include "DialogGeneral.h"
#include "DialogDays.h"
#include "DialogToday.h"
#include "DialogWeekdays.h"
#include "DialogWeekNumbers.h"
#include "DialogMonth.h"
#include "DialogYear.h"
#include "DialogEvent.h"
#include "DialogServer.h"

/////////////////////////////////////////////////////////////////////////////
// CCalendarWindow window

class CCalendarWindow : public CWnd
{
// Construction
public:
	CCalendarWindow();

// Attributes
public:
	static CConfig c_Config;
	static CTime c_TodaysDate;
	static CTime c_CurrentDate;

protected:
	HINSTANCE m_DllInstance;
	CBackground m_Background;
	CBitmap* m_DoubleBuffer;

	CString m_WallpaperName;

	int m_Width;
	int m_Height;

	bool m_FirstExecute;
	bool m_Hidden;

	int m_SelectedDay;

	unsigned int m_ConnectionCounter;

	CItemDays* m_Days;
	CItemEvent* m_Event;
	CItemToday* m_Today;
	CItemWeekdays* m_Weekdays;
	CItemWeekNumbers* m_WeekNumbers;
	CItemMonth* m_Month;
	CItemYear* m_Year;

	CDialogGeneral m_DialogGeneral;
	CDialogDays m_DialogDays;
	CDialogToday m_DialogToday;
	CDialogWeekdays m_DialogWeekdays;
	CDialogWeekNumbers m_DialogWeekNumbers;
	CDialogMonth m_DialogMonth;
	CDialogYear m_DialogYear;
	CDialogEvent m_DialogEvent;
	CDialogServer m_DialogServer;

	CToolTipCtrl m_ToolTip;

	static wharfDataType* c_WharfData;
	static bool c_DummyLitestep;
	static LPCSTR c_CmdLine;

// Operations
public:
	void Initialize(HWND ParentWnd, HINSTANCE dllInst, wharfDataType* wd, LPCSTR szPath);
	CToolTipCtrl& GetToolTip() { return m_ToolTip; };
	void QuitModule();
	void RefreshWindow() { OnRefresh(); };
	void ShowConfig() { OnConfig(); };
	void QuitRainlendar() { OnQuit(); };
	void HideWindow() { CWnd::ShowWindow(SW_HIDE); m_Hidden = true; };
	void ShowWindow() { CWnd::ShowWindow(SW_SHOWNOACTIVATE); m_Hidden = false; };
	void ToggleWindow();
	void ShowMonth(int Month, int Year);
	void ShowNextMonth() { OnCommand(ID_POPUP_SELECTMONTH_NEXTMONTH, NULL); };
	void ShowPrevMonth() { OnCommand(ID_POPUP_SELECTMONTH_PREVMONTH, NULL); };
	void ShowCurrentMonth() { OnCommand(ID_POPUP_SELECTMONTH_CURRENTMONTH, NULL); };

	static void SetDummyLitestep(bool Dummy) { c_DummyLitestep = Dummy; };
	static bool GetDummyLitestep() { return c_DummyLitestep; };
	static void SetCommandLine(LPCSTR CmdLine) { c_CmdLine = CmdLine;};
	static LPCSTR GetCommandLine() { return c_CmdLine; };
	static wharfDataType* GetWharfData() { return c_WharfData; };

private:
	void PollWallpaper(bool set);
	void ConnectServer(int type);
	void FillMenu(CMenu& Menu, CPoint point);
	void Refresh();
	void CalcWindowSize();
	void DrawCalendar();
	void ShowWindowIfAppropriate();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendarWindow)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCalendarWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCalendarWindow)
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnQuit();
	afx_msg void OnRefresh();
	afx_msg LRESULT OnGetRevID(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnConfig();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSetevent();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnAbout();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	afx_msg void OnDisplayChange(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg LRESULT OnCopyData(WPARAM wParam, LPARAM lParam) ;
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALENDARWINDOW_H__C7D0FDB9_F062_11D3_92A3_0080AD90417B__INCLUDED_)

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
#include "ItemMonth.h"
#include "ItemYear.h"
#include "DialogGeneral.h"
#include "DialogDays.h"
#include "DialogToday.h"
#include "DialogWeekdays.h"
#include "DialogMonth.h"
#include "DialogYear.h"
#include "DialogEvent.h"

/////////////////////////////////////////////////////////////////////////////
// CCalendarWindow window

class CCalendarWindow : public CWnd
{
// Construction
public:
	CCalendarWindow();

// Attributes
public:
	static CConfig C_Config;
	static CTime C_TodaysDate;
	static CTime C_CurrentDate;

protected:
	bool m_Litestep;
	bool m_FirstRun;

	HINSTANCE m_DllInstance;
	CBackground m_Background;
	CBitmap* m_DoubleBuffer;

	int m_Width;
	int m_Height;

	bool m_Refreshing;
	bool m_FirstExecute;

	int m_SelectedDay;

	CItemDays m_Days;
	CItemEvent m_Event;
	CItemToday m_Today;
	CItemWeekdays m_Weekdays;
	CItemMonth m_Month;
	CItemYear m_Year;

	CDialogGeneral m_DialogGeneral;
	CDialogDays m_DialogDays;
	CDialogToday m_DialogToday;
	CDialogWeekdays m_DialogWeekdays;
	CDialogMonth m_DialogMonth;
	CDialogYear m_DialogYear;
	CDialogEvent m_DialogEvent;

	CToolTipCtrl m_ToolTip;

// Operations
public:
	void Initialize(HWND ParentWnd, HINSTANCE dllInst, LPCSTR IniPath);
	CToolTipCtrl& GetToolTip() { return m_ToolTip; };
	void QuitModule();
	void RefreshWindow() { OnRefresh(); };
	void ShowConfig() { OnConfig(); };
	void QuitRainlendar() { OnQuit(); };
	void HideWindow() { CWnd::ShowWindow(SW_HIDE); };
	void ShowWindow() { CWnd::ShowWindow(SW_SHOWNOACTIVATE); };
	void ToggleWindow();
	void ShowMonth(int Month, int Year);
	void ShowNextMonth() { OnCommand(ID_POPUP_SELECTMONTH_NEXTMONTH, NULL); };
	void ShowPrevMonth() { OnCommand(ID_POPUP_SELECTMONTH_PREVMONTH, NULL); };
	void ShowCurrentMonth() { OnCommand(ID_POPUP_SELECTMONTH_CURRENTMONTH, NULL); };

private:
	void FillMenu(CMenu& Menu, CPoint point);
	void Refresh();
	void CalcWindowSize();
	void DrawCalendar();

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
	//}}AFX_MSG
	afx_msg BOOL OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALENDARWINDOW_H__C7D0FDB9_F062_11D3_92A3_0080AD90417B__INCLUDED_)

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
  $Header: /home/cvsroot/Rainlendar/Library/CalendarWindow.h,v 1.3 2005/10/14 17:05:41 rainy Exp $

  $Log: CalendarWindow.h,v $
  Revision 1.3  2005/10/14 17:05:41  rainy
  no message

  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.27  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.26  2004/12/05 18:34:46  rainy
  Changed wallpaper polling.
  Added hotkeys.

  Revision 1.25  2004/11/06 13:38:58  rainy
  no message

  Revision 1.24  2004/01/28 18:08:02  rainy
  no message

  Revision 1.23  2004/01/25 09:58:58  rainy
  Fixed refresh.
  Added PLAY to execute.
  Fixed icon drawing.

  Revision 1.22  2004/01/10 15:17:29  rainy
  Added tray and hotkey for todo.
  Fixed message window.

  Revision 1.21  2003/12/20 22:26:57  rainy
  Added more debugging info.
  Removed unnecessary functions.
  Added new message box.

  Revision 1.20  2003/10/27 19:50:20  Rainy
  Derived from CRainWindow.

  Revision 1.19  2003/10/04 14:47:54  Rainy
  Languages path is the same as the DLL's
  Skins are rescanned during refresh.
  Added powerbroadcast again since the timer didn't work for some reason.

  Revision 1.18  2003/08/09 16:40:32  Rainy
  Added icons to messageboxes.
  Added hotkeys.
  Removed powerbroadcast (timer should do the same thing)

  Revision 1.17  2003/06/15 09:50:55  Rainy
  Strings are read from CLanguage class.
  Added support for multiple calendars.

  Revision 1.16  2003/05/24 13:51:00  Rainy
  Added refresh when windows refurns from suspend.

  Revision 1.15  2003/05/07 19:16:25  rainy
  Few bugs fixed.
  Added support for Outlook.

  Revision 1.14  2003/03/22 20:31:11  rainy
  Added Move and ZPos bangs

  Revision 1.13  2002/11/25 17:11:57  rainy
  Wallpaper polling checks the timestamp too.

  Revision 1.12  2002/11/12 18:12:03  rainy
  Added support for native transparency.

  Revision 1.11  2002/08/24 11:14:35  rainy
  Changed the error handling.
  Added lite refreshing.
  Added a lot of logging information.

  Revision 1.10  2002/08/03 16:21:51  rainy
  Added showEdges.
  Added support for skins.
  Changed to use the new ToolTip class.
  Changed to use separate skin config dialog.
  + other changes that I cannot remember now :-(

  Revision 1.9  2002/05/30 18:28:18  rainy
  Added WIN32_LEAN_AND_MEAN

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

  Revision 1.4  2002/01/15 17:59:18  rainy
  Changed the way refreshing is done.

  Revision 1.3  2002/01/10 16:49:14  rainy
  The items weren't deallocated during refresh. Fixed and changed them to pointers.

  Revision 1.2  2001/12/23 10:02:55  rainy
  Rainlendar.dll uses a bit different interface.

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#ifndef __CALENDARWINDOW_H__
#define __CALENDARWINDOW_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define WIN_VER

#include "Error.h"
#include "Litestep.h"
#include "Background.h"
#include "Config.h"
#include "ItemDays.h"
#include "ItemEvent.h"
#include "ItemToday.h"
#include "ItemWeekdays.h"
#include "ItemWeekNumbers.h"
#include "ItemMonth.h"
#include "ItemYear.h"
#include "Resource.h"
#include "Language.h"
#include "RainWindow.h"
#include "TodoWindow.h"
#include "MessageWindow.h"
#include "EventListWindow.h"
#include <windows.h>
#include <commctrl.h>
#include <string>
#include "PluginManager.h"

class CRainlendar;

class CCalendarWindow : public CRainWindow
{
public:
	CCalendarWindow();
	~CCalendarWindow();

	virtual bool Initialize(HWND parent, HINSTANCE instance);
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings();

	virtual void Refresh();
	virtual void Redraw();
	virtual RAINWINDOW_TYPE GetType() { return RAINWINDOW_TYPE_CALENDAR; };

	void RefreshAll();
	void RedrawAll();

	void ShowConfig() { OnCommand(ID_CONFIG, NULL); }
	void ShowEditSkin() { OnCommand(ID_EDIT_SKIN, NULL); }
	void ShowNextMonth() { OnCommand(ID_POPUP_SELECTMONTH_NEXTMONTH, NULL); }
	void ShowPrevMonth() { OnCommand(ID_POPUP_SELECTMONTH_PREVMONTH, NULL); }
	void ShowCurrentMonth() { OnCommand(ID_POPUP_SELECTMONTH_CURRENTMONTH, NULL); }
	void SetWindowZPos(CConfig::WINDOWPOS pos);
	void FillMenu(HMENU Menu, int x, int y, HWND hWnd);
	void SetTransparency(UINT transparency);

	CEventManager& GetEventManager() { return m_EventManager; }
	CPluginManager& GetPluginManager() { return m_PluginManager; }
	CTodoWindow& GetTodoWindow() { return m_TodoWindow; }
	CMessageWindow& GetMessageWindow() { return m_MessageWindow; }
	CEventListWindow& GetEventListWindow() { return m_EventListWindow; }

	static void ChangeMonth(int Month, int Year);
    static void ChangeMonth(int numOfMonths);

	static CFileTime c_TodaysDate;			// Today
	static CFileTime c_MonthsFirstDate;		// Month that is displayed in the calendar
	
	static CLanguage c_Language;

	struct CONFIG 
	{
		std::string path;
		std::vector<std::string> iniFiles;
	};

	virtual LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnGetRevID(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysKeyUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnHotkey(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCopyData(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Config
	BACKGROUND_MODE GetBackgroundMode() { return m_BackgroundMode; };
    const std::string& GetBackgroundBitmapName() { return m_BackgroundBitmapName; };
	bool GetBackgroundBevel() { return m_BackgroundBevel; };
	COLORREF GetBackgroundSolidColor() { return m_BackgroundSolidColor; };

	void SetBackgroundMode(BACKGROUND_MODE BackgroundMode ) { m_BackgroundMode=BackgroundMode; };
	void SetBackgroundBitmapName(const std::string& BackgroundBitmapName ) { m_BackgroundBitmapName=BackgroundBitmapName; };
	void SetBackgroundBevel(bool BackgroundBevel ) { m_BackgroundBevel=BackgroundBevel; };
	void SetBackgroundSolidColor(COLORREF BackgroundSolidColor ) { m_BackgroundSolidColor=BackgroundSolidColor; };

	CItem* GetItem(const char* section);

	CItemDays* GetItemDays() { return m_Days; }
	CItemEvent* GetItemEvent() { return m_Event; }
	CItemToday* GetItemToday() { return m_Today; }
	CItemWeekdays* GetItemWeekdays() { return m_Weekdays; }
	CItemWeekNumbers* GetItemWeekNumbers() { return m_WeekNumbers; }
	CItemMonth* GetItemMonth() { return m_Month; }
	CItemYear* GetItemYear() { return m_Year; }

	void ResetSettings();
	void ShowTodaysEvents();	// Mitul : Made public
	POINT GetCurrentMonthOffset() { return m_CurrentMonthOffset; }	// Mitul
	POINT GetViewMonthOffset() { return m_ViewMonthOffset; }

protected:
	virtual SIZE CalcWindowSize();
	virtual void DrawWindow();
	virtual void DrawDynamic();
	void ExecuteEventCommand(const std::string& text);
	void ReadSkinSettings();

	BOOL RemoveTrayIcon();
	BOOL AddUpdateTrayIcon(DWORD addmodi, bool balloon, int timeout);	// Mitul

	void RegisterHotkeys();
	void ReadSkins();

	CItemDays* m_Days;
	CItemEvent* m_Event;
	CItemToday* m_Today;
	CItemWeekdays* m_Weekdays;
	CItemWeekNumbers* m_WeekNumbers;
	CItemMonth* m_Month;
	CItemYear* m_Year;

	CBackground m_Background;

	std::vector<CItemDynamic*> m_DynamicItems;			// Dynamically allocated CItems	// Mitul

	std::vector<CONFIG> m_ConfigStrings;	    // All configs found in the given folder

	std::vector<GUID> m_SelectedEvents;
	CFileTime m_NewEventStartTime;

	CFileTime m_LastAlarmTime;

	bool m_FirstExecute;
	unsigned int m_ConnectionCounter;

	UINT m_ToolBarMessage;

	CEventListWindow m_EventListWindow;
	CTodoWindow m_TodoWindow;
	CMessageWindow m_MessageWindow;

	CPluginManager m_PluginManager;
	CEventManager m_EventManager;

	std::string m_BackgroundBitmapName;	// Name of the background picture
	BACKGROUND_MODE m_BackgroundMode;
	COLORREF m_BackgroundSolidColor;
	bool m_BackgroundBevel;

	POINT m_CurrentMonthOffset;		// Mitul
	POINT m_ViewMonthOffset;

	bool m_QuitSet;
};

#endif


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
  $Header: /home/cvsroot/Rainlendar/Plugin/CalendarWindow.h,v 1.24 2004/01/28 18:08:02 rainy Exp $

  $Log: CalendarWindow.h,v $
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

#ifndef __CALENDARWINDOW_H__
#define __CALENDARWINDOW_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

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
#include <windows.h>
#include <commctrl.h>
#include <string>

#define WM_SERVER_SYNC_FINISHED WM_USER + 100

class CRainlendar;

class CCalendarWindow : public CRainWindow
{
public:
	CCalendarWindow();
	~CCalendarWindow();

	virtual bool Initialize(HWND parent, HINSTANCE instance);
	virtual void Refresh(bool lite = false);

	void ShowConfig() { OnCommand(ID_CONFIG, NULL); }
	void ShowEditSkin() { OnCommand(ID_EDIT_SKIN, NULL); }
	void QuitRainlendar() { OnCommand(ID_QUIT, NULL); }
	void ShowNextMonth() { OnCommand(ID_POPUP_SELECTMONTH_NEXTMONTH, NULL); }
	void ShowPrevMonth() { OnCommand(ID_POPUP_SELECTMONTH_PREVMONTH, NULL); }
	void ShowCurrentMonth() { OnCommand(ID_POPUP_SELECTMONTH_CURRENTMONTH, NULL); }
	void SetWindowZPos(CConfig::WINDOWPOS pos);
	void ShowEventMessage(std::vector<CEventMessage*>& eventList, bool messageBox);

	void ConnectServer(int type);

	int GetSelectedDate() { return m_SelectedDate; }

	CEventManager* GetEventManager() { return m_Event != 0 ? m_Event->GetEventManager() : NULL; }
	CTodoManager& GetTodoManager() { return m_TodoWindow.GetTodoManager(); }
	CTodoWindow& GetTodoWindow() { return m_TodoWindow; }
	CMessageWindow& GetMessageWindow() { return m_MessageWindow; }

	static void ChangeMonth(int Month, int Year);
	static void AddCurrentPath(std::string& filename);

	static SYSTEMTIME c_TodaysDate;
	static SYSTEMTIME c_MonthsFirstDate;
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
	virtual LRESULT OnSettingsChange(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysKeyUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnHotkey(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCopyData(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnServerSyncFinished(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	virtual SIZE CalcWindowSize();
	virtual void DrawWindow();

	BOOL RemoveTrayIcon();
	BOOL AddTrayIcon();

	void RegisterHotkeys();
	void ReadSkins();
	void PollWallpaper(bool set);
	void FillMenu(HMENU Menu, int x, int y);

	CItemDays* m_Days;
	CItemEvent* m_Event;
	CItemToday* m_Today;
	CItemWeekdays* m_Weekdays;
	CItemWeekNumbers* m_WeekNumbers;
	CItemMonth* m_Month;
	CItemYear* m_Year;

	CBackground m_Background;

	std::vector<CONFIG> m_ConfigStrings;	    // All configs found in the given folder

	std::string m_WallpaperName;
	FILETIME m_WallpaperTime;

	bool m_FirstExecute;
	int m_SelectedDate;
	int m_MenuSelectedDate;
	unsigned int m_ConnectionCounter;

	CTodoWindow m_TodoWindow;
	CMessageWindow m_MessageWindow;
};

#endif


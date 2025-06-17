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
  $Header: /home/cvsroot/Rainlendar/Library/RainlendarDLL.h,v 1.3 2005/09/08 16:09:12 rainy Exp $

  $Log: RainlendarDLL.h,v $
  Revision 1.3  2005/09/08 16:09:12  rainy
  no message

  Revision 1.2  2005/07/20 16:34:16  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.23  2005/03/25 13:58:43  rainy
  no message

  Revision 1.22  2005/03/01 18:50:45  rainy
  no message

  Revision 1.21  2004/12/05 18:23:13  rainy
  Fixed the bangs.

  Revision 1.20  2004/11/06 13:38:59  rainy
  no message

  Revision 1.19  2004/06/10 16:17:19  rainy
  Added zpos

  Revision 1.18  2004/01/28 18:08:14  rainy
  New version.

  Revision 1.17  2004/01/25 10:01:47  rainy
  Added new bangs.

  Revision 1.16  2004/01/10 15:13:21  rainy
  Changed Toggle, Show and Hide to affect also the Todo window.

  Revision 1.15  2003/10/27 19:51:29  Rainy
  RefreshWindow is no more.

  Revision 1.14  2003/10/27 17:38:32  Rainy
  Removed wharf support.

  Revision 1.13  2003/10/04 14:52:18  Rainy
  version 0.18

  Revision 1.12  2003/08/23 09:15:02  Rainy
  0.17.1

  Revision 1.11  2003/08/09 15:26:27  Rainy
  DLL Instance is saved to global variable.

  Revision 1.10  2003/06/15 20:04:15  Rainy
  Changed the version.

  Revision 1.9  2003/06/15 09:49:55  Rainy
  ShowMonth -> ChangeMonth

  Revision 1.8  2003/03/22 20:29:52  rainy
  Added Move and ZPos bangs

  Revision 1.7  2002/11/25 17:02:23  rainy
  version 0.14

  Revision 1.6  2002/08/24 11:09:40  rainy
  Changed the error handling.

  Revision 1.5  2002/08/03 16:10:46  rainy
  Added interfaces for new bang commands.
  SetWharf is now just a boolean.

  Revision 1.4  2002/05/30 18:25:23  rainy
  Removed some Litestep related stuff.

  Revision 1.3  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.2  2001/12/23 10:01:13  rainy
  Rainlendar.dll uses a bit different interface.

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#ifndef __RAINLENDAR_H__
#define __RAINLENDAR_H__

#pragma warning(disable: 4786)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <windows.h>
#include "Litestep.h"
#include "CalendarWindow.h"

#define MAX_LINE_LENGTH 4096

#define BEGIN_MESSAGEPROC switch(uMsg) {
#define MESSAGE(handler, msg) case msg: return Window?Window->handler(wParam, lParam):DefWindowProc(hWnd, uMsg, wParam, lParam);
#define REJECT_MESSAGE(msg) case msg: return 0;
#define END_MESSAGEPROC } return DefWindowProc(hWnd, uMsg, wParam, lParam);

#define MAKE_VER(major, minor) major * 1000 + minor

#define APPNAME "Rainlendar"
#define VERSION "0.22"
#define RAINLENDAR_VERSION MAKE_VER(22, 0)

#define WM_NOTIFYICON WM_USER + 101

#define DLLDECL __declspec( dllexport )

void RainlendarToggle(HWND caller, const char* arg);
void RainlendarHide(HWND caller, const char* arg);
void RainlendarShow(HWND caller, const char* arg);
void RainlendarRefresh(HWND caller, const char* arg);
void RainlendarRedraw(HWND caller, const char* arg);
void RainlendarConfig(HWND caller, const char* arg);
void RainlendarSkinConfig(HWND caller, const char* arg);
void RainlendarQuit(HWND caller, const char* arg);
void RainlendarEditSkin(HWND caller, const char* arg);
void RainlendarShowNext(HWND caller, const char* arg);
void RainlendarShowPrev(HWND caller, const char* arg);
void RainlendarShowCurrent(HWND caller, const char* arg);
void RainlendarShowMonth(HWND caller, const char* arg);
void RainlendarLsBoxHook(HWND, const char* arg);
void RainlendarMove(HWND caller, const char* arg);
void RainlendarZPos(HWND caller, const char* arg);
void RainlendarEditTodo(HWND caller, const char* arg);
void RainlendarShowTodo(HWND caller, const char* arg);
void RainlendarHideTodo(HWND caller, const char* arg);
void RainlendarToggleTodo(HWND caller, const char* arg);
void RainlendarShowEventList(HWND caller, const char* arg);
void RainlendarHideEventList(HWND caller, const char* arg);
void RainlendarToggleEventList(HWND caller, const char* arg);
void RainlendarShowCalendar(HWND caller, const char* arg);
void RainlendarHideCalendar(HWND caller, const char* arg);
void RainlendarToggleCalendar(HWND caller, const char* arg);
void RainlendarAddEvent(HWND caller, const char* arg);
void RainlendarShowItem(HWND caller, const char* arg);
void RainlendarHideItem(HWND caller, const char* arg);
void RainlendarToggleItem(HWND caller, const char* arg);
void RainlendarChangeSkin(HWND caller, const char* arg);
void RainlendarShowTodaysEvent(HWND caller, const char* arg);	// Mitul

class CRainlendar
{
public:
	CRainlendar();
	~CRainlendar();

	int Initialize(HWND Parent, HINSTANCE Instance, LPCSTR szPath);
	void Quit(HINSTANCE dllInst);

	void HideWindow();
	void ShowWindow(bool activate);
	void ToggleWindow(bool visible);
	void RefreshWindow() { m_Calendar.RefreshAll(); };
	void ShowConfig() { m_Calendar.ShowConfig(); };
	void ShowEditSkin() { m_Calendar.ShowEditSkin(); };
	void ShowMonth(int Month, int Year) { m_Calendar.ChangeMonth(Month, Year); m_Calendar.Redraw(); };
	void ShowNextMonth(int num) { m_Calendar.ChangeMonth(num); m_Calendar.Redraw(); };
	void ShowPrevMonth(int num) { m_Calendar.ChangeMonth(-num); m_Calendar.Redraw(); };
	void ShowCurrentMonth() { m_Calendar.ShowCurrentMonth(); };
	void MoveWindow(int x, int y, RAINWINDOW_TYPE window);
	void SetWindowZPos(CConfig::WINDOWPOS pos);
	void EditTodo() { m_Calendar.GetTodoWindow().OnCommand(ID_TODO, NULL); };
	void AddEvent(int day, int month, int year);

	CCalendarWindow& GetCalendarWindow() { return m_Calendar; };

	static void SetDummyLitestep(bool Dummy) { c_DummyLitestep = Dummy; };
	static bool GetDummyLitestep() { return c_DummyLitestep; };
	static void SetCommandLine(LPCSTR CmdLine) { c_CmdLine = CmdLine;};
	static LPCSTR GetCommandLine() { return c_CmdLine.c_str(); };
	static HINSTANCE GetInstance() { return c_Instance; };
	static void SetInstance(HINSTANCE instance) { c_Instance = instance; };
	
private:
	CCalendarWindow m_Calendar;		// The calendar window
	static bool c_DummyLitestep;	// true, if not a Litestep plugin
	static std::string c_CmdLine;	// The command line arguments
	static HINSTANCE c_Instance;	// The DLL's instance
};

extern "C"
{
	DLLDECL int initWharfModule(HWND ParentWnd, HINSTANCE dllInst, void* wd);
	DLLDECL void quitWharfModule(HINSTANCE dll);
	DLLDECL HRGN getLSRegion(int xoffset, int yoffset);
	DLLDECL int initModuleEx(HWND ParentWnd, HINSTANCE dllInst, LPCSTR szPath);
	DLLDECL void quitModule(HINSTANCE dllInst);
	DLLDECL void Initialize(bool DummyLS, LPCSTR CmdLine);
}

CRainlendar* GetRainlendar();

#endif

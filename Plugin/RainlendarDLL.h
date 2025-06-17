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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/RainlendarDLL.h,v 1.4 2002/05/30 18:25:23 rainy Exp $

  $Log: RainlendarDLL.h,v $
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

#include <windows.h>
#include "Litestep.h"
#include "CalendarWindow.h"

#define MAX_LINE_LENGTH 4096

#define BEGIN_MESSAGEPROC switch(uMsg) {
#define MESSAGE(handler, msg) case msg: return Window?Window->handler(wParam, lParam):DefWindowProc(hWnd, uMsg, wParam, lParam);
#define REJECT_MESSAGE(msg) case msg: return 0;
#define END_MESSAGEPROC } return DefWindowProc(hWnd, uMsg, wParam, lParam);

#define APPNAME "Rainlendar"
#define VERSION "0.10"

#define DLLDECL __declspec( dllexport )

void RainlendarToggle(HWND caller, const char* arg);
void RainlendarHide(HWND caller, const char* arg);
void RainlendarShow(HWND caller, const char* arg);
void RainlendarRefresh(HWND caller, const char* arg);
void RainlendarConfig(HWND caller, const char* arg);
void RainlendarQuit(HWND caller, const char* arg);
void RainlendarShowNext(HWND caller, const char* arg);
void RainlendarShowPrev(HWND caller, const char* arg);
void RainlendarShowCurrent(HWND caller, const char* arg);
void RainlendarShowMonth(HWND caller, const char* arg);

class CRainlendar
{
public:
	CRainlendar();
	~CRainlendar();

	int Initialize(HWND Parent, HINSTANCE Instance, bool wd, LPCSTR szPath);
	void Quit(HINSTANCE dllInst);
	HRGN GetRegion(int xOffset, int yOffset);

	void QuitRainlendar() { m_Calendar.QuitRainlendar(); };
	void HideWindow() { m_Calendar.HideWindow(); };
	void ShowWindow() { m_Calendar.ShowWindow(); };
	void ToggleWindow() { m_Calendar.ToggleWindow(); };
	void RefreshWindow() { m_Calendar.RefreshWindow(); };
	void ShowConfig() { m_Calendar.ShowConfig(); };
	void ShowMonth(int Month, int Year) { m_Calendar.ShowMonth(Month, Year); };
	void ShowNextMonth() { m_Calendar.ShowNextMonth(); };
	void ShowPrevMonth() { m_Calendar.ShowPrevMonth(); };
	void ShowCurrentMonth() { m_Calendar.ShowCurrentMonth(); };

	bool IsWharf() { return m_Wharf; };
	CCalendarWindow& GetCalendarWindow() { return m_Calendar; };

	static void SetDummyLitestep(bool Dummy) { c_DummyLitestep = Dummy; };
	static bool GetDummyLitestep() { return c_DummyLitestep; };
	static void SetCommandLine(LPCSTR CmdLine) { c_CmdLine = CmdLine;};
	static LPCSTR GetCommandLine() { return c_CmdLine.c_str(); };

private:
	CCalendarWindow m_Calendar;		// The calendar window
	bool m_Wharf;					// true, if ran in a wharf
	static bool c_DummyLitestep;	// true, if not a Litestep plugin
	static std::string c_CmdLine;	// The command line arguments
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

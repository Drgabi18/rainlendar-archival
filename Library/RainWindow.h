/*
  Copyright (C) 2003 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Library/RainWindow.h,v 1.3 2005/10/14 17:05:29 rainy Exp $

  $Log: RainWindow.h,v $
  Revision 1.3  2005/10/14 17:05:29  rainy
  no message

  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.7  2005/03/25 13:58:43  rainy
  no message

  Revision 1.6  2005/03/01 18:50:45  rainy
  no message

  Revision 1.5  2004/11/06 13:38:59  rainy
  no message

  Revision 1.4  2004/04/24 11:21:46  rainy
  Show and hide are now virtual

  Revision 1.3  2004/01/25 10:02:03  rainy
  Separated refresh to two methods.

  Revision 1.2  2003/10/27 19:50:43  Rainy
  Fixed the WndProc (it cannot be a member).

  Revision 1.1  2003/10/27 17:34:53  Rainy
  Initial version.

*/

#ifndef __RAINWINDOW_H__
#define __RAINWINDOW_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Config.h"
#include "ItemTime.h"
#include "ItemButton.h"
#include "ItemImage.h"
#include "FileTime.h"

#define WINDOW_TIMER 1
#define FADE_TIMER 2
#define REDRAW_TIMER 3
#define REFRESH_TIMER 4

enum RAINWINDOW_TYPE
{
	RAINWINDOW_TYPE_CALENDAR,
	RAINWINDOW_TYPE_TODO,
	RAINWINDOW_TYPE_MESSAGEBOX,
	RAINWINDOW_TYPE_EVENTLIST,
	RAINWINDOW_TYPE_TODAY,
	RAINWINDOW_TYPE_BALLOONTIP
};

class CRainWindow
{
public:
	CRainWindow();
	~CRainWindow();

	virtual bool Initialize(HWND parent, HINSTANCE instance);
	virtual void ReadSettings(const char* filename, const char* section);
	virtual void WriteSettings() = 0;
	virtual void ResetSettings();

	virtual void Refresh();
	virtual void Redraw() = 0;
	virtual RAINWINDOW_TYPE GetType() = 0;

	virtual int GetX() { return m_X; };
	virtual int GetY() { return m_Y; };
	virtual int GetWidth() { return m_Width; };
	virtual int GetHeight() { return m_Height; };

	HDC GetDoubleBuffer() { return m_DC; };
	HWND GetWindow() { return m_Window; };

	virtual void ShowWindow(bool activate);
	virtual void HideWindow() { ::ShowWindow(m_Window, SW_HIDE); }
	void ToggleWindow();
	void FadeWindow(bool fadeIn, bool opaque);
	void FadeLinkedWindows(bool fadeIn, bool opaque);

	void MoveWindow(int x, int y);
	void SetWindowZPos(CConfig::WINDOWPOS pos);
	void ExecuteCommand(const char* command);

	bool IsDocked() { return m_Docked; }
	void DockWindow(bool undock);
	POINT GetDistanceFromMainWindow() { return m_DistanceFromMainWindow; }

	virtual void UpdateTransparency(UINT transparency);

	virtual LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSettingsChange(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWindowPosChanging(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnEraseBkgnd(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnDropFiles(WPARAM wParam, LPARAM lParam);

	static bool Is2k();

	void AddSkinItem(CItem* item, bool dynamic);

	// Mitul{
	bool GetSmartlyHidden() { return m_SmartlyHidden; }	
	void SetSmartlyHidden(bool SmartlyHidden) { m_SmartlyHidden = SmartlyHidden; };
	// Mitul}

protected:
	void UpdateDynamic();
	virtual void RedrawBegin();
	virtual void RedrawEnd();
	bool IsKeyPressed();

	virtual SIZE CalcWindowSize();
	virtual void DrawWindow() = 0;
	bool InsideCheck(POINT pos);

	void ShowWindowIfAppropriate();
	virtual void DrawDynamic();
	
	void SnapToWindow(CRainWindow* window, LPWINDOWPOS wp);

	std::string m_Section;
	std::string m_SettingsFile;

	std::vector<CItem*> m_StaticSkinItems;			// Sorted list of CItems
	std::vector<CItemDynamic*> m_DynamicSkinItems;			// Sorted list of CItemDynamic

	HDC m_DC;									// DC used with drawing
	CImage m_DoubleBuffer;						// Double buffer for flicker free drawing
	CImage m_DynamicBuffer;						// Double buffer for dynamic stuff (time, buttons, ...)
	HWND m_Window;								// Handle to the Rainlendar window
	HINSTANCE m_Instance;						// Handle to the main instance
	int m_X;
	int m_Y;
	int m_Width;
	int m_Height;
	bool m_Refreshing;
	UINT m_Message;								// The current window message
	char* m_ClassName;
	char* m_WindowName;
	bool m_WindowMoved;
	UINT m_TransparencyValue;
	bool m_NeedsUpdating;

	bool m_Docked;
	POINT m_DistanceFromMainWindow;

	CFileTime m_LastUpdate;

	DWORD m_FadeStartTime;
	int m_FadeStartValue;
	int m_FadeEndValue;

	WNDPROC m_WndProc;

	bool m_SmartlyHidden;	// Mitul
};

#endif

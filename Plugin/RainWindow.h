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
  $Header: /home/cvsroot/Rainlendar/Plugin/RainWindow.h,v 1.4 2004/04/24 11:21:46 rainy Exp $

  $Log: RainWindow.h,v $
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

#define WINDOW_TIMER 1

class CRainWindow
{
public:
	CRainWindow();
	~CRainWindow();

	virtual bool Initialize(HWND parent, HINSTANCE instance);
	virtual void Refresh(bool lite) = 0;

	int GetX() { return m_X; };
	int GetY() { return m_Y; };
	int GetWidth() { return m_Width; };
	int GetHeight() { return m_Height; };

	HDC GetDoubleBuffer() { return m_DC; };
	HWND GetWindow() { return m_Window; };

	virtual void ShowWindow(bool activate);
	virtual void HideWindow() { ::ShowWindow(m_Window, SW_HIDE); }
	void ToggleWindow();

	void MoveWindow(int x, int y);
	void SetWindowZPos(CConfig::WINDOWPOS pos);

	virtual LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSettingsChange(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWindowPosChanging(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnEraseBkgnd(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnPowerBroadcast(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam);

	static bool Is2k();

protected:
	virtual void RefreshBegin(bool lite);
	virtual void RefreshEnd(bool lite);

	virtual SIZE CalcWindowSize() = 0;
	virtual void DrawWindow() = 0;

	void ShowWindowIfAppropriate();
	void UpdateTransparency();

	HDC m_DC;									// DC used with drawing
	CImage m_DoubleBuffer;						// Double buffer for flicker free drawing
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

	WNDPROC m_WndProc;
};

#endif

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
  $Header: /home/cvsroot/Rainlendar/Plugin/RainWindow.cpp,v 1.4 2004/04/24 11:21:46 rainy Exp $

  $Log: RainWindow.cpp,v $
  Revision 1.4  2004/04/24 11:21:46  rainy
  Show and hide are now virtual

  Revision 1.3  2004/01/25 10:02:03  rainy
  Separated refresh to two methods.

  Revision 1.2  2003/10/27 19:50:43  Rainy
  Fixed the WndProc (it cannot be a member).

  Revision 1.1  2003/10/27 17:34:53  Rainy
  Initial version.

*/

#pragma warning(disable:4786)

#include "RainWindow.h"
#include "Litestep.h"

#define ULW_ALPHA               0x00000002
#define WS_EX_LAYERED           0x00080000

#define SNAPDISTANCE 10

/* 
** CRainWindow
**
** Constructor
**
*/
CRainWindow::CRainWindow() 
{
	m_DC = NULL;
	m_Window = NULL;
	m_Instance = NULL;
	m_X = NULL;
	m_Y = NULL;
	m_Width = 0;
	m_Height = 0;
	m_Refreshing = false;
	m_Message = 0;
	m_ClassName = NULL;
	m_WindowName = NULL;
	m_WndProc = NULL;
}

/* 
** ~CRainWindow
**
** Destructor
**
*/
CRainWindow::~CRainWindow()
{
	if (m_DC) DeleteDC(m_DC); 

	if(m_Window) DestroyWindow(m_Window);

	// Unregister the window class
	UnregisterClass(m_ClassName, m_Instance);

	LSLog(LOG_DEBUG, "Rainlendar", "The window is destroyed.");
}

/* 
** Initialize
**
** Initializes the window, creates the class and the window.
**
*/
bool CRainWindow::Initialize(HWND Parent, HINSTANCE Instance)
{
	WNDCLASSEX wc;

	m_Instance = Instance;

	// Register the windowclass
	memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.style = CS_NOCLOSE | CS_DBLCLKS;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpfnWndProc = m_WndProc;
	wc.hInstance = m_Instance;
	wc.lpszClassName = m_ClassName;
	
	if(!RegisterClassEx(&wc))
	{
		if (ERROR_CLASS_ALREADY_EXISTS != GetLastError())
		{
			LSLog(LOG_DEBUG, "Rainlendar", "Failed to register the class for the window.");
			return false;
		}
	}

	m_Window = CreateWindowEx(WS_EX_TOOLWINDOW, 
							m_ClassName, 
							m_WindowName, 
							WS_POPUP,
							m_X,
							m_Y,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							Parent,
							NULL,
							Instance,
							this);

	if (m_Window == NULL) 
	{ 
		return false;
	}

	SetWindowLong(m_Window, GWL_USERDATA, magicDWord);

	SetTimer(m_Window, WINDOW_TIMER, 1000, NULL);

	return true;
}

/* 
** RefreshBegin
**
** Refresh the window. Lite refresh only updates the background.
** Full refresh recreates everything.
**
*/
void CRainWindow::RefreshBegin(bool lite)
{
	m_Refreshing = true;

	if (!lite)
	{
		// Reset the region
		SetWindowRgn(m_Window, NULL, TRUE);

		SIZE size = CalcWindowSize();

		m_Width = size.cx;
		m_Height = size.cy;

		MoveWindow(m_X, m_Y);
	}
}

/* 
** RefreshEnd
**
** End of Refresh
**
*/
void CRainWindow::RefreshEnd(bool lite)
{
	// Create the DoubleBuffer
	m_DoubleBuffer.Create(m_Width, m_Height, 0x0);	// Fully transparent
	DrawWindow();
	UpdateTransparency();
	
	if (!lite)
	{
		// Set window region
		HRGN region = BitmapToRegion(m_DoubleBuffer.GetBitmap(), RGB(255,0,255), 0x101010, 0, 0);
		SetWindowRgn(m_Window, region, TRUE);
	}
	
	InvalidateRect(m_Window, NULL, false);
	
	m_Refreshing = false;
}

/*
** MoveWindow
**
** Moves the window to a new location.
**
*/
void CRainWindow::MoveWindow(int x, int y)
{
	RECT r;

	if (CConfig::Instance().GetNegativeCoords())
	{
		// Handle negative coordinates
		GetClientRect(GetDesktopWindow(), &r); 
		if (x < 0) x += r.right - r.left;
		if (y < 0) y += r.bottom - r.top;
	}

	m_X = x;
	m_Y = y;

	SetWindowPos(m_Window, NULL, m_X, m_Y, m_Width, m_Height, SWP_NOACTIVATE | SWP_NOZORDER);
}

/*
** SetWindowZPos
**
** Sets the window's z-position (OnTop, Normal, OnBottom, OnDesktop).
**
*/
void CRainWindow::SetWindowZPos(CConfig::WINDOWPOS pos)
{
	HWND parent = NULL;
	HWND winPos = HWND_NOTOPMOST;
	LONG oldStyle = GetWindowLong(m_Window, GWL_STYLE);
	LONG style = (oldStyle & ~WS_CHILD) | WS_POPUP;		// Change from child to popup

	switch (pos)
	{
	case CConfig::WINDOWPOS_ONTOP:
		winPos = HWND_TOPMOST;
		break;

	case CConfig::WINDOWPOS_ONBOTTOM:
		winPos = HWND_BOTTOM;
		break;

	case CConfig::WINDOWPOS_ONDESKTOP:
		{
			// Set the window's parent to progman, so it stays always on desktop
			HWND ProgmanHwnd = FindWindow("Progman", "Program Manager");
			if (ProgmanHwnd && GetParent(m_Window) != ProgmanHwnd)
			{
				parent = ProgmanHwnd;
				style = (oldStyle & ~WS_POPUP) | WS_CHILD;
			}
			else
			{
				return;		// The window is already on desktop
			}
		}
	}

	if (style != oldStyle)
	{
		SetWindowLong(m_Window, GWL_STYLE, style);
		SetParent(m_Window, parent);
	}

	SetWindowPos(m_Window, winPos, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
}

/*
** UpdateTransparency
**
** Updates the native Windows transparency
*/
void CRainWindow::UpdateTransparency()
{
	if (Is2k())
	{
		if (CConfig::Instance().GetNativeTransparency())
		{
			// Add the window flag
			LONG style = GetWindowLong(m_Window, GWL_EXSTYLE);
			SetWindowLong(m_Window, GWL_EXSTYLE, style | WS_EX_LAYERED);

			RECT r;
			GetWindowRect(m_Window, &r);

			typedef BOOL (WINAPI * FPUPDATELAYEREDWINDOW)(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);
			HINSTANCE h = LoadLibrary("user32.dll");
			FPUPDATELAYEREDWINDOW UpdateLayeredWindow = (FPUPDATELAYEREDWINDOW)GetProcAddress(h, "UpdateLayeredWindow");

			BLENDFUNCTION blendPixelFunction= {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
			POINT ptWindowScreenPosition = {r.left, r.top};
			POINT ptSrc = {0, 0};
			SIZE szWindow = {m_Width, m_Height};
			
			HDC dcScreen = GetDC(GetDesktopWindow());
			HDC dcMemory = CreateCompatibleDC(dcScreen);
			HBITMAP oldBitmap = (HBITMAP)SelectObject(dcMemory, m_DoubleBuffer.GetBitmap());
			UpdateLayeredWindow(m_Window, dcScreen, &ptWindowScreenPosition, &szWindow, dcMemory, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
			ReleaseDC(GetDesktopWindow(), dcScreen);
			SelectObject(dcMemory, oldBitmap);
			DeleteDC(dcMemory);
			FreeLibrary(h);
		}
		else
		{
			// Remove the window flag
			LONG style = GetWindowLong(m_Window, GWL_EXSTYLE);
			SetWindowLong(m_Window, GWL_EXSTYLE, style & ~WS_EX_LAYERED);
		}
	}
}

/*
** Is2k
**
** Returns true if ew're running modern OS.
**
*/
bool CRainWindow::Is2k()
{
	OSVERSIONINFO version;
	version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&version);

	if (version.dwPlatformId == VER_PLATFORM_WIN32_NT && version.dwMajorVersion > 4)
	{
		return true;
	}
	return false;
}

/* 
** ToggleWindow
**
** Toggles the window
**
*/
void CRainWindow::ToggleWindow()
{
	if (IsWindowVisible(m_Window))
	{
		HideWindow();
	}
	else
	{
		ShowWindow(false);
	}
}

typedef VOID (*SWITCHTOTHISWINDOW)(HWND hWnd, BOOL fAltTab);

/* 
** ShowWindow
**
** Shows the window and optionally activates it.
**
*/
void CRainWindow::ShowWindow(bool activate)
{
	if (activate)
	{
		BringWindowToTop(m_Window);
		::ShowWindow(m_Window, SW_SHOWNORMAL);
		SetForegroundWindow(m_Window);
	}
	else
	{
		::ShowWindow(m_Window, SW_SHOWNOACTIVATE); 
	}
}

/* 
** ShowWindowIfAppropriate
**
** Toggles the window
**
*/
void CRainWindow::ShowWindowIfAppropriate()
{
	if(CConfig::Instance().GetMouseHide())
	{
		if((GetKeyState(VK_CONTROL) & 0x8000) || (GetKeyState(VK_SHIFT) & 0x8000) || (GetKeyState(VK_MENU) & 0x8000))
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

		if (!inside)
		{
			ShowWindow(false);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// CRainWindow message handlers
//
/////////////////////////////////////////////////////////////////////////////

LRESULT CRainWindow::OnPaint(WPARAM wParam, LPARAM lParam) 
{
	if(m_DoubleBuffer.GetBitmap()) 
	{
		PAINTSTRUCT ps;
		HDC winDC = BeginPaint(m_Window, &ps);
		HDC tmpDC = CreateCompatibleDC(NULL);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(tmpDC, m_DoubleBuffer.GetBitmap());

		BitBlt(winDC, 0, 0, m_Width, m_Height, tmpDC, 0, 0, SRCCOPY);

		SelectObject(tmpDC, oldBitmap);
		DeleteDC(tmpDC);

		EndPaint(m_Window, &ps);
	}
	return 0;
}

LRESULT CRainWindow::OnWindowPosChanging(WPARAM wParam, LPARAM lParam) 
{
	LPWINDOWPOS wp=(LPWINDOWPOS)lParam;

	if(CConfig::Instance().GetWindowPos() == CConfig::WINDOWPOS_ONBOTTOM && !m_Refreshing)
	{
		// do not change the z-order. This keeps the window on bottom.
		wp->flags|=SWP_NOZORDER;
	}

	if (CConfig::Instance().GetSnapEdges() && !((GetKeyState(VK_CONTROL) & 0x8000) || (GetKeyState(VK_SHIFT) & 0x8000)))
	{
		RECT workArea;
		GetClientRect(GetDesktopWindow(), &workArea);

		// only process movement (ignore anything without winpos values)
		if(wp->cx != 0 && wp->cy != 0)
		{
			typedef HMONITOR (WINAPI * FPMONITORFROMWINDOW)(HWND wnd, DWORD dwFlags);
			typedef BOOL (WINAPI * FPGETMONITORINFO)(HMONITOR hMonitor, LPMONITORINFO lpmi);
			FPMONITORFROMWINDOW fpMonitorFromWindow;
			FPGETMONITORINFO fpGetMonitorInfo;

			HINSTANCE lib = LoadLibrary("user32.dll");
			
			fpMonitorFromWindow = (FPMONITORFROMWINDOW)GetProcAddress(lib, "MonitorFromWindow");
			fpGetMonitorInfo = (FPGETMONITORINFO)GetProcAddress(lib, "GetMonitorInfoA");

			if (fpMonitorFromWindow && fpGetMonitorInfo)
			{
				HMONITOR hMonitor;
				MONITORINFO mi;

				hMonitor = fpMonitorFromWindow(m_Window, MONITOR_DEFAULTTONULL);

				if(hMonitor != NULL)
				{
					mi.cbSize = sizeof(mi);
					fpGetMonitorInfo(hMonitor, &mi);
					workArea = mi.rcWork;
				}
			}

			FreeLibrary(lib);

			int w = workArea.right - m_Width;
			int h = workArea.bottom - m_Height;

			if((wp->x < SNAPDISTANCE + workArea.left) && (wp->x > workArea.left - SNAPDISTANCE)) wp->x = workArea.left;
			if((wp->y < SNAPDISTANCE + workArea.top) && (wp->y > workArea.top - SNAPDISTANCE)) wp->y = workArea.top;
			if ((wp->x < SNAPDISTANCE + w) && (wp->x > -SNAPDISTANCE + w)) wp->x = w;
			if ((wp->y < SNAPDISTANCE + h) && (wp->y > -SNAPDISTANCE + h)) wp->y = h;
		}
	}

	return DefWindowProc(m_Window, m_Message, wParam, lParam);
}

LRESULT CRainWindow::OnSettingsChange(WPARAM wParam, LPARAM lParam) 
{
	// If wallpaper is changed, refresh
	if(wParam == SPI_SETDESKWALLPAPER) 
	{
		Refresh(true);
	}	

	return 0;
}

LRESULT CRainWindow::OnDisplayChange(WPARAM wParam, LPARAM lParam) 
{
	// Refresh when the resolution changes
	if (CConfig::Instance().GetRefreshOnResolutionChange())
	{
		Refresh(true);
	}
	return DefWindowProc(m_Window, m_Message, wParam, lParam);
}

LRESULT CRainWindow::OnEraseBkgnd(WPARAM wParam, LPARAM lParam) 
{
	return 0;
}

LRESULT CRainWindow::OnPowerBroadcast(WPARAM wParam, LPARAM lParam) 
{
	// Refresh when the resolution changes
	if (wParam == PBT_APMRESUMESUSPEND)
	{
		Refresh(true);
	}

	return DefWindowProc(m_Window, m_Message, wParam, lParam);
}

LRESULT CRainWindow::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	if(CConfig::Instance().GetMovable())
	{
		return HTCAPTION;
	}

	return DefWindowProc(m_Window, WM_NCHITTEST, wParam, lParam);
}

LRESULT CRainWindow::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if(CConfig::Instance().GetMouseHide())
	{
		if((GetKeyState(VK_CONTROL) & 0x8000) || (GetKeyState(VK_SHIFT) & 0x8000) || (GetKeyState(VK_MENU) & 0x8000))
		{
			// If Alt, shift or control is down, do not hide the window
			return 0;
		}

		// Hide window if it is visible
		if(IsWindowVisible(m_Window)) HideWindow();
	}	

	return 0;
}

LRESULT CRainWindow::OnTimer(WPARAM wParam, LPARAM lParam) 
{
	if(wParam == WINDOW_TIMER)
	{
		ShowWindowIfAppropriate();
		return 0;
	}

	return DefWindowProc(m_Window, WM_TIMER, wParam, lParam);
}



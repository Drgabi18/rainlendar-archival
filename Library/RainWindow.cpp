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
  $Header: /home/cvsroot/Rainlendar/Library/RainWindow.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: RainWindow.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.9  2005/03/25 13:58:43  rainy
  no message

  Revision 1.8  2005/03/01 18:50:45  rainy
  no message

  Revision 1.7  2004/12/05 18:22:46  rainy
  Small fixes.

  Revision 1.6  2004/11/06 13:38:59  rainy
  no message

  Revision 1.5  2004/06/10 16:17:38  rainy
  Small fixes

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
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "RainWindow.h"
#include "Litestep.h"
#include "RasterizerFont.h"
#include <time.h>
#include <mmsystem.h>
#include <shellapi.h>

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

	m_LastUpdate = 0;
	m_WindowMoved = true;
	m_Docked = false;
	m_DistanceFromMainWindow.x = 0;
	m_DistanceFromMainWindow.y = 0;
	m_FadeStartTime = 0;
	m_FadeStartValue = 0;
	m_FadeEndValue = 0;
	m_TransparencyValue = 255;
	m_NeedsUpdating = false;

	m_Section = "Rainlendar";
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
}

void CRainWindow::AddSkinItem(CItem* item, bool dynamic)
{
	if (dynamic)
	{
		m_DynamicSkinItems.push_back(item); 
	}
	else
	{
		m_StaticSkinItems.push_back(item); 
	}
}

void CRainWindow::ResetSettings()
{
	m_StaticSkinItems.clear();
	m_DynamicSkinItems.clear();
}

void CRainWindow::ReadSettings(const char* filename, const char* section)
{
	m_Section = section;
	m_SettingsFile = filename;
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
			DebugLog("Failed to register the class for the window.");
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

	DragAcceptFiles(m_Window, TRUE);

	SetWindowLong(m_Window, GWL_USERDATA, magicDWord);

	SetTimer(m_Window, WINDOW_TIMER, 1000, NULL);

	return true;
}

/* 
** Refresh
**
** Refreshes the window. 
**
*/
void CRainWindow::Refresh()
{
	// Remove transparent flag
	LONG style = GetWindowLong(m_Window, GWL_EXSTYLE);
	if ((style & WS_EX_TRANSPARENT) != 0)
	{
		SetWindowLong(m_Window, GWL_EXSTYLE, style & ~WS_EX_TRANSPARENT);
	}

	m_DynamicBuffer.Clear();

	if (CConfig::Instance().GetTransparentOnMouseOver())
	{
		m_TransparencyValue = 255;
	}
	else
	{
		m_TransparencyValue = CConfig::Instance().GetTransparencyValue();
	}
}

/* 
** RedrawBegin
**
** Redraws the window. 
**
*/
void CRainWindow::RedrawBegin()
{
	// Reset the region
	SetWindowRgn(m_Window, NULL, TRUE);

	SIZE size = CalcWindowSize();

	m_Width = size.cx;
	m_Height = size.cy;

	bool refresh = m_Refreshing;
	m_Refreshing = true;	// Fake refreshing so that the auto snap is disabled
	MoveWindow(m_X, m_Y);
	m_Refreshing = refresh;
}

/* 
** RedrawEnd
**
** End of Redraw
**
*/
void CRainWindow::RedrawEnd()
{
	// Create the DoubleBuffer
	m_DoubleBuffer.Create(m_Width, m_Height, 0x0);	// Fully transparent

	DrawWindow();
	DrawDynamic();

	UpdateTransparency(m_TransparencyValue);
	
	// Set window region
	HRGN region = BitmapToRegion(m_DoubleBuffer.GetBitmap(), RGB(255,0,255), 0x101010, 0, 0);
	SetWindowRgn(m_Window, region, TRUE);
	
	InvalidateRect(m_Window, NULL, false);
	
	m_WindowMoved = false;
}

void CRainWindow::DrawDynamic()
{
	m_DynamicBuffer.Clear();
	m_NeedsUpdating = false;

	POINT offset = {0, 0};
	for (int i = 0; i < m_DynamicSkinItems.size(); i++)
	{
		if (m_DynamicSkinItems[i]->IsEnabled())
		{
			m_NeedsUpdating = m_NeedsUpdating || m_DynamicSkinItems[i]->NeedsUpdating();

			if (m_DynamicBuffer.GetBitmap() == NULL)
			{
				m_DynamicBuffer.Create(m_Width, m_Height, 0);
			}

			m_DynamicSkinItems[i]->Paint(m_DynamicBuffer, offset);
		}
	}
}

/*
** CalcWindowSize
**
** Calculates the size of the window.
**
*/
SIZE CRainWindow::CalcWindowSize()
{
	SIZE size;

	size.cx = 0;
	size.cy = 0;

	return size;
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

	if (CConfig::Instance().GetGrowUpwards() & (1 << (int)GetType()))
	{
		y = y - m_Height;
	}

	SetWindowPos(m_Window, NULL, x, y, m_Width, m_Height, SWP_NOACTIVATE | SWP_NOZORDER);
}

/*
** SetWindowZPos
**
** Sets the window's z-position (OnTop, Normal, OnBottom, OnDesktop).
**
*/
void CRainWindow::SetWindowZPos(CConfig::WINDOWPOS pos)
{
	HWND desktop = GetDesktopWindow();
	HWND parent = GetAncestor(m_Window, GA_PARENT);
	HWND winPos = HWND_NOTOPMOST;

	switch (pos)
	{
	case CConfig::WINDOWPOS_ONTOP:
		if (GetWindowLong(m_Window, GWL_EXSTYLE) & WS_EX_TOPMOST) 
		{
			return;
		}
		winPos = HWND_TOPMOST;
 		break;

	case CConfig::WINDOWPOS_ONBOTTOM:
		 winPos = HWND_BOTTOM;
		 break;

	case CConfig::WINDOWPOS_ONDESKTOP:
		// Set the window's parent to progman, so it stays always on desktop
		HWND ProgmanHwnd = FindWindow("Progman", "Program Manager");
		if (ProgmanHwnd && (parent == desktop))
		{
			SetParent(m_Window, ProgmanHwnd);
		}
		else
		{
			return;		// The window is already on desktop
		}
		break;
	}

	if (pos != CConfig::WINDOWPOS_ONDESKTOP && (parent != desktop))
	{
		SetParent(m_Window, NULL);
	}

	bool refresh = m_Refreshing;
	m_Refreshing = true;	// Fake refreshing so that the z-position can be changed
	SetWindowPos(m_Window, winPos, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	m_Refreshing = refresh;
}

/*
** UpdateTransparency
**
** Updates the native Windows transparency
*/
void CRainWindow::UpdateTransparency(UINT transparency)
{
	if (Is2k())
	{
		if (CConfig::Instance().GetNativeTransparency())
		{
			// Add the window flag
			LONG style = GetWindowLong(m_Window, GWL_EXSTYLE);
			if ((style & WS_EX_LAYERED) == 0)
			{
				SetWindowLong(m_Window, GWL_EXSTYLE, style | WS_EX_LAYERED);
			}

			RECT r;
			GetWindowRect(m_Window, &r);

			typedef BOOL (WINAPI * FPUPDATELAYEREDWINDOW)(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);
			HINSTANCE h = LoadLibrary("user32.dll");
			FPUPDATELAYEREDWINDOW UpdateLayeredWindow = (FPUPDATELAYEREDWINDOW)GetProcAddress(h, "UpdateLayeredWindow");

			if (UpdateLayeredWindow)
			{
				BLENDFUNCTION blendPixelFunction= {AC_SRC_OVER, 0, transparency, AC_SRC_ALPHA};
				POINT ptWindowScreenPosition = {r.left, r.top};
				POINT ptSrc = {0, 0};
				SIZE szWindow = {m_Width, m_Height};
				
				HBITMAP oldBitmap = NULL;
				CImage textImage;

				HDC dcScreen = CreateCompatibleDC(NULL);
				HDC dcMemory = CreateCompatibleDC(dcScreen);
				if (m_DynamicBuffer.GetBitmap() == NULL)
				{
					oldBitmap = (HBITMAP)SelectObject(dcMemory, m_DoubleBuffer.GetBitmap());
				}
				else
				{
					textImage.Create(m_DoubleBuffer.GetWidth(), m_DoubleBuffer.GetHeight(), 0);
					textImage.Blit(m_DoubleBuffer, 0, 0, 0, 0, m_DoubleBuffer.GetWidth(), m_DoubleBuffer.GetHeight());
					textImage.Blit(m_DynamicBuffer, 0, 0, 0, 0, m_DynamicBuffer.GetWidth(), m_DynamicBuffer.GetHeight());
					oldBitmap = (HBITMAP)SelectObject(dcMemory, textImage.GetBitmap());
				}

				UpdateLayeredWindow(m_Window, dcScreen, &ptWindowScreenPosition, &szWindow, dcMemory, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
				SelectObject(dcMemory, oldBitmap);
				DeleteDC(dcMemory);
				DeleteDC(dcScreen);
			}
			FreeLibrary(h);
		}
		else
		{
			// Remove the window flag
			LONG style = GetWindowLong(m_Window, GWL_EXSTYLE);
			if ((style & WS_EX_LAYERED) != 0)
			{
				SetWindowLong(m_Window, GWL_EXSTYLE, style & ~WS_EX_LAYERED);
			}
		}
	}
}

/*
** Is2k
**
** Returns true if we're running NT.
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

bool CRainWindow::InsideCheck(POINT pos)
{
	bool inside = false;
	RECT rect;
	GetWindowRect(m_Window, &rect);

	if(rect.left <= pos.x && rect.right >= pos.x &&
	   rect.top <= pos.y && rect.bottom >= pos.y) inside = true;

	if (IsDocked())
	{
		CRainWindow* window = &GetRainlendar()->GetCalendarWindow();
		if (!inside && GetType() != RAINWINDOW_TYPE_CALENDAR && window->IsDocked())
		{
			GetWindowRect(window->GetWindow(), &rect);
			if(rect.left <= pos.x && rect.right >= pos.x &&
			   rect.top <= pos.y && rect.bottom >= pos.y) inside = true;
		}

		window = &GetRainlendar()->GetCalendarWindow().GetEventListWindow();
		if (!inside && GetType() != RAINWINDOW_TYPE_EVENTLIST && window->IsDocked())
		{
			GetWindowRect(window->GetWindow(), &rect);
			if(rect.left <= pos.x && rect.right >= pos.x &&
			   rect.top <= pos.y && rect.bottom >= pos.y) inside = true;
		}
		
		window = &GetRainlendar()->GetCalendarWindow().GetTodoWindow();
		if (!inside && GetType() != RAINWINDOW_TYPE_TODO && window->IsDocked())
		{
			GetWindowRect(window->GetWindow(), &rect);
			if(rect.left <= pos.x && rect.right >= pos.x &&
			   rect.top <= pos.y && rect.bottom >= pos.y) inside = true;
		}
	}
	return inside;
}

void CRainWindow::FadeLinkedWindows(bool fadeIn, bool opaque)
{
	FadeWindow(fadeIn, opaque);

	if (IsDocked())
	{
		CRainWindow* window = &GetRainlendar()->GetCalendarWindow();
		if (GetType() != RAINWINDOW_TYPE_CALENDAR && window->IsDocked())
		{
			window->FadeWindow(fadeIn, opaque);
		}

		window = &GetRainlendar()->GetCalendarWindow().GetEventListWindow();
		if (GetType() != RAINWINDOW_TYPE_EVENTLIST && window->IsDocked())
		{
			window->FadeWindow(fadeIn, opaque);
		}
		
		window = &GetRainlendar()->GetCalendarWindow().GetTodoWindow();
		if (GetType() != RAINWINDOW_TYPE_TODO && window->IsDocked())
		{
			window->FadeWindow(fadeIn, opaque);
		}
	}
}

void CRainWindow::FadeWindow(bool fadeIn, bool opaque)
{
	if (!CConfig::Instance().GetNativeTransparency() || CConfig::Instance().GetFadeDuration() == 0)
	{
		if (opaque && CConfig::Instance().GetFadeDuration() == 0)
		{
			if (fadeIn)
			{
				m_TransparencyValue = 255;
			}
			else
			{
				m_TransparencyValue = CConfig::Instance().GetTransparencyValue();
			}
			UpdateTransparency(m_TransparencyValue);
		}
		else
		{
			if (fadeIn)
			{
				ShowWindow(false);
			}
			else
			{
				HideWindow();
			}
		}
	}
	else
	{
		if (fadeIn)
		{
			if (opaque)
			{
				m_FadeStartValue = CConfig::Instance().GetTransparencyValue();
				m_FadeEndValue = 255;
				m_TransparencyValue = 255;
			}
			else
			{
				UpdateTransparency(0);
				ShowWindow(false);
				m_FadeStartValue = 0;
				
				if (CConfig::Instance().GetTransparentOnMouseOver())
				{
					m_TransparencyValue = 255;
				}
				else
				{
					m_TransparencyValue = CConfig::Instance().GetTransparencyValue();
				}

				m_FadeEndValue = m_TransparencyValue;
			}
		}
		else
		{
			if (opaque)
			{
				m_FadeStartValue = 255;
				m_FadeEndValue = CConfig::Instance().GetTransparencyValue();
				m_TransparencyValue = CConfig::Instance().GetTransparencyValue();
			}
			else
			{
				m_FadeStartValue = CConfig::Instance().GetTransparencyValue();
				m_FadeEndValue = 0;
				m_TransparencyValue = 0;
			}
		}

		SetTimer(m_Window, FADE_TIMER, 10, NULL);
	}
}

bool CRainWindow::IsKeyPressed()
{
	if (!CConfig::Instance().GetDisableKeyboardOverride())
	{
		return (GetKeyState(VK_CONTROL) & 0x8000) || (GetKeyState(VK_SHIFT) & 0x8000) || (GetKeyState(VK_MENU) & 0x8000);
	}

	return false;
}

/* 
** ShowWindowIfAppropriate
**
** Toggles the window
**
*/
void CRainWindow::ShowWindowIfAppropriate()
{
	bool inside = false;
	POINT pos;

	GetCursorPos(&pos);
	inside = InsideCheck(pos);

	if (CConfig::Instance().GetClickThrough())
	{
		if (!inside || IsKeyPressed())
		{
			// If Alt, shift or control is down, remove the transparent flag
			LONG style = GetWindowLong(m_Window, GWL_EXSTYLE);
			if ((style & WS_EX_TRANSPARENT) != 0)
			{
				SetWindowLong(m_Window, GWL_EXSTYLE, style & ~WS_EX_TRANSPARENT);
			}
		}
	}

	if (CConfig::Instance().GetMouseHide())
	{
		if(IsKeyPressed())
		{
			// If Alt, shift or control is down, do not show the window
			return;
		}

		if (!inside && !IsWindowVisible(m_Window))
		{
			FadeLinkedWindows(true, false);
		}
	}
	else if (CConfig::Instance().GetOpaqueOnMouseOver())
	{
		if (!inside && IsWindowVisible(m_Window) && m_TransparencyValue != CConfig::Instance().GetTransparencyValue())
		{
			FadeLinkedWindows(false, true);
		}
	}
	else if (CConfig::Instance().GetTransparentOnMouseOver())
	{
		if (!inside && IsWindowVisible(m_Window) && m_TransparencyValue != 255)
		{
			FadeLinkedWindows(true, true);
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

		HBITMAP oldBitmap = NULL;
		if (m_DynamicBuffer.GetBitmap() == NULL)
		{
			// No dynamic items -> draw only the double buffer
			oldBitmap = (HBITMAP)SelectObject(tmpDC, m_DoubleBuffer.GetBitmap());
			BitBlt(winDC, 0, 0, m_Width, m_Height, tmpDC, 0, 0, SRCCOPY);
		}
		else
		{
			// Dynamic data -> Draw both buffers
			CImage textImage;
			textImage.Create(m_DoubleBuffer.GetWidth(), m_DoubleBuffer.GetHeight(), 0);
			textImage.Blit(m_DoubleBuffer, 0, 0, 0, 0, m_DoubleBuffer.GetWidth(), m_DoubleBuffer.GetHeight());
			textImage.Blit(m_DynamicBuffer, 0, 0, 0, 0, m_DynamicBuffer.GetWidth(), m_DynamicBuffer.GetHeight());
			oldBitmap = (HBITMAP)SelectObject(tmpDC, textImage.GetBitmap());
			BitBlt(winDC, 0, 0, m_Width, m_Height, tmpDC, 0, 0, SRCCOPY);
		}
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

	if (CConfig::Instance().GetSnapEdges() && !((GetKeyState(VK_CONTROL) & 0x8000) || (GetKeyState(VK_SHIFT) & 0x8000)) && !m_Refreshing)
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

			// Dock also to other windows
			CRainWindow* window;

			window = &GetRainlendar()->GetCalendarWindow();
			if (GetType() != RAINWINDOW_TYPE_CALENDAR && IsWindowVisible(window->GetWindow()) && (!window->IsDocked() || !IsDocked()))
			{
				SnapToWindow(window, wp);
			}

			window = &GetRainlendar()->GetCalendarWindow().GetEventListWindow();
			if (GetType() != RAINWINDOW_TYPE_EVENTLIST && IsWindowVisible(window->GetWindow()) && (!window->IsDocked() || !IsDocked()))
			{
				SnapToWindow(window, wp);
			}
			
			window = &GetRainlendar()->GetCalendarWindow().GetTodoWindow();
			if (GetType() != RAINWINDOW_TYPE_TODO && IsWindowVisible(window->GetWindow()) && (!window->IsDocked() || !IsDocked()))
			{
				SnapToWindow(window, wp);
			}
		}
	}

	return DefWindowProc(m_Window, m_Message, wParam, lParam);
}

void CRainWindow::SnapToWindow(CRainWindow* window, LPWINDOWPOS wp)
{
	int x = window->GetX();
	int y = window->GetY();
	int w = window->GetWidth();
	int h = window->GetHeight();

	if (wp->y < y + h && wp->y + m_Height > y)
	{
		if((wp->x < SNAPDISTANCE + x) && (wp->x > x - SNAPDISTANCE)) wp->x = x;
		if((wp->x < SNAPDISTANCE + x + w) && (wp->x > x + w - SNAPDISTANCE)) wp->x = x + w;

		if((wp->x + m_Width < SNAPDISTANCE + x) && (wp->x + m_Width > x - SNAPDISTANCE)) wp->x = x - m_Width;
		if((wp->x + m_Width < SNAPDISTANCE + x + w) && (wp->x + m_Width > x + w - SNAPDISTANCE)) wp->x = x + w - m_Width;
	}

	if (wp->x < x + w && wp->x + m_Width > x)
	{
		if((wp->y < SNAPDISTANCE + y) && (wp->y > y - SNAPDISTANCE)) wp->y = y;
		if((wp->y < SNAPDISTANCE + y + h) && (wp->y > y + h - SNAPDISTANCE)) wp->y = y + h;

		if((wp->y + m_Height < SNAPDISTANCE + y) && (wp->y + m_Height > y - SNAPDISTANCE)) wp->y = y - m_Height;
		if((wp->y + m_Height < SNAPDISTANCE + y + h) && (wp->y + m_Height > y + h - SNAPDISTANCE)) wp->y = y + h - m_Height;
	}
}

LRESULT CRainWindow::OnSettingsChange(WPARAM wParam, LPARAM lParam) 
{
	// If wallpaper is changed, refresh
	if(wParam == SPI_SETDESKWALLPAPER) 
	{
		Redraw();
	}	
	return 0;
}

LRESULT CRainWindow::OnDisplayChange(WPARAM wParam, LPARAM lParam) 
{
	// Refresh when the resolution changes
	if (CConfig::Instance().GetRefreshOnResolutionChange())
	{
		Refresh();
		return TRUE;
	}
	return DefWindowProc(m_Window, m_Message, wParam, lParam);
}

LRESULT CRainWindow::OnEraseBkgnd(WPARAM wParam, LPARAM lParam) 
{
	return 0;
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
	if(CConfig::Instance().GetMouseHide() || CConfig::Instance().GetClickThrough())
	{
		if(!IsKeyPressed())
		{
			// If Alt, shift or control is down, do not hide the window

			if (CConfig::Instance().GetClickThrough())
			{
				LONG style = GetWindowLong(m_Window, GWL_EXSTYLE);
				if ((style & WS_EX_TRANSPARENT) == 0)
				{
					SetWindowLong(m_Window, GWL_EXSTYLE, style | WS_EX_TRANSPARENT);
				}
			}
			else
			{
				// Hide window if it is visible
				if(IsWindowVisible(m_Window)) 
				{
					FadeLinkedWindows(false, false);
				}
			}
		}
	}
	
	if (CConfig::Instance().GetOpaqueOnMouseOver())
	{
		if (IsWindowVisible(m_Window) && m_TransparencyValue != 255) 
		{
			FadeLinkedWindows(true, true);
		}
	}
	else if (CConfig::Instance().GetTransparentOnMouseOver())
	{
		if (IsWindowVisible(m_Window) && m_TransparencyValue != CConfig::Instance().GetTransparencyValue()) 
		{
			FadeLinkedWindows(false, true);
		}
	}

	POINT pos;
	pos.x = (SHORT)LOWORD(lParam);
	pos.y = (SHORT)HIWORD(lParam);

	SIZE size;
	size.cx = m_Width;
	size.cy = m_Height;

	if(m_Message == WM_NCMOUSEMOVE)
	{
		// Transform the point to client rect
		MapWindowPoints(NULL, m_Window, &pos, 1);
	}

	bool update = false;
	for (int i = 0; i < m_DynamicSkinItems.size(); i++)
	{
		if (m_DynamicSkinItems[i]->IsEnabled())
		{
			update = update || m_DynamicSkinItems[i]->MouseMove(pos, this);
		}
	}
	if (update)
	{
		UpdateDynamic();
	}

	// This crashes when tooltip is changed for some reason
//	return DefWindowProc(m_Window, m_Message, wParam, lParam);
	return 0;
}

LRESULT CRainWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	POINT pos;
	pos.x = (SHORT)LOWORD(lParam);
	pos.y = (SHORT)HIWORD(lParam);

	SIZE size;
	size.cx = m_Width;
	size.cy = m_Height;

	if(m_Message == WM_NCLBUTTONDOWN)
	{
		// Transform the point to client rect
		MapWindowPoints(NULL, m_Window, &pos, 1);
	}

	bool update = false;
	for (int i = 0; i < m_DynamicSkinItems.size(); i++)
	{
		if (m_DynamicSkinItems[i]->IsEnabled())
		{
			update = update || m_DynamicSkinItems[i]->MouseDown(pos, this);
		}
	}
	if (update)
	{
		SetCapture(m_Window);
		UpdateDynamic();
		return 0;
	}

	return DefWindowProc(m_Window, m_Message, wParam, lParam);
}

LRESULT CRainWindow::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT pos;
	pos.x = (SHORT)LOWORD(lParam);
	pos.y = (SHORT)HIWORD(lParam);

	SIZE size;
	size.cx = m_Width;
	size.cy = m_Height;

	if(m_Message == WM_NCLBUTTONUP)
	{
		// Transform the point to client rect
		MapWindowPoints(NULL, m_Window, &pos, 1);
	}

	ReleaseCapture();

	bool update = false;
	for (int i = 0; i < m_DynamicSkinItems.size(); i++)
	{
		if (m_DynamicSkinItems[i]->IsEnabled())
		{
			update = update || m_DynamicSkinItems[i]->MouseUp(pos, this);
		}
	}
	if (update)
	{
		UpdateDynamic();
		return 0;
	}

	return DefWindowProc(m_Window, m_Message, wParam, lParam);
}

LRESULT CRainWindow::OnMove(WPARAM wParam, LPARAM lParam) 
{
	if (IsDocked())
	{
		bool snap = CConfig::Instance().GetSnapEdges();
		CConfig::Instance().SetSnapEdges(false);	// Disable snapping

		short int xPos = LOWORD(lParam);
		short int yPos = HIWORD(lParam);

		// Dock also to other windows
		CRainWindow* window;
		window = &GetRainlendar()->GetCalendarWindow();
		
		int xCal;
		int yCal;

		if (GetType() != RAINWINDOW_TYPE_CALENDAR)
		{
			xCal = xPos + m_DistanceFromMainWindow.x;
			yCal = yPos + m_DistanceFromMainWindow.y;

			int x = window->GetX();
			int y = window->GetY();

			if (CConfig::Instance().GetGrowUpwards() & (1 << (int)GetType()))
			{
				yCal = yCal + m_Height;
			}

			if (x != xCal || y != yCal)
			{
				window->MoveWindow(xCal, yCal);
			}
		}
		else
		{
			xCal = xPos;
			yCal = yPos;
		}

		window = &GetRainlendar()->GetCalendarWindow().GetEventListWindow();
		if (GetType() != RAINWINDOW_TYPE_EVENTLIST && IsWindowVisible(window->GetWindow()) && window->IsDocked())
		{
			if (window->GetX() != xCal - window->GetDistanceFromMainWindow().x || window->GetY() != yCal - window->GetDistanceFromMainWindow().y)
			{
				window->MoveWindow(xCal - window->GetDistanceFromMainWindow().x, yCal - window->GetDistanceFromMainWindow().y);
			}
		}
		
		window = &GetRainlendar()->GetCalendarWindow().GetTodoWindow();
		if (GetType() != RAINWINDOW_TYPE_TODO && IsWindowVisible(window->GetWindow()) && window->IsDocked())
		{
			if (window->GetX() != xCal - window->GetDistanceFromMainWindow().x || window->GetY() != yCal - window->GetDistanceFromMainWindow().y)
			{
				window->MoveWindow(xCal - window->GetDistanceFromMainWindow().x, yCal - window->GetDistanceFromMainWindow().y);
			}
		}

		CConfig::Instance().SetSnapEdges(snap);
	}

	m_WindowMoved = true;
	return 0;
}

void CRainWindow::DockWindow(bool undock)
{
	if (undock)
	{
		m_Docked = false;
	}
	else
	{
		m_Docked = true;

		CRainWindow* window = &GetRainlendar()->GetCalendarWindow();
		int xCal = window->GetX();
		int yCal = window->GetY();

		m_DistanceFromMainWindow.x = xCal - m_X;
		m_DistanceFromMainWindow.y = yCal - m_Y;
	}
}

LRESULT CRainWindow::OnDropFiles(WPARAM wParam, LPARAM lParam) 
{
	CPluginManager& manager = GetRainlendar()->GetCalendarWindow().GetPluginManager();
	const std::vector<CPlugin*>& plugins = manager.GetAllPlugins();

	char path[MAX_PATH];
	HDROP hDrop = (HDROP)wParam;
	int count = DragQueryFile(hDrop, -1, NULL, 0);

	bool error = false;

	for (int j = 0; j < count; j++) 
	{
		if (DragQueryFile(hDrop, j, path, MAX_PATH) > 0)
		{
			// Try to load the file with all plugins until one can import it
			int i;
			for (i = 0; i < (int)plugins.size(); i++)
			{
				if (plugins[i]->ImportItems(path, RAINLENDAR_TYPE_EVENT))
				{
					// Found a plugin that could import the file
					GetRainlendar()->GetCalendarWindow().GetEventManager().FlushBufferedItems(true);
					GetRainlendar()->GetCalendarWindow().RedrawAll();
					break;
				}
			}
			if (i == plugins.size())
			{
				error = true;
			}
		}
	}

	if (error) 
	{
		// "Unable to import the file."
		MessageBox(m_Window, CCalendarWindow::c_Language.GetString("AllEventsGUI", 10), "Rainlendar", MB_OK | MB_ICONEXCLAMATION);
	}
	return 0;
}

LRESULT CRainWindow::OnTimer(WPARAM wParam, LPARAM lParam) 
{
	if(wParam == WINDOW_TIMER)
	{
		POINT pos;
		// Transform the point to client rect
		GetCursorPos(&pos);
		MapWindowPoints(NULL, m_Window, &pos, 1);

		bool update = false;
		for (int i = 0; i < m_DynamicSkinItems.size(); i++)
		{
			if (m_DynamicSkinItems[i]->IsEnabled())
			{
				update = update || m_DynamicSkinItems[i]->MouseMove(pos, this);
			}
		}
		if (update)
		{
			UpdateDynamic();
		}

		time_t aclock;
		::time(&aclock);
	
		if (m_LastUpdate == 0)
		{
			m_LastUpdate = aclock;
		}
		else
		{
			if (aclock / 60 > m_LastUpdate / 60)
			{
				if (m_NeedsUpdating && m_DynamicBuffer.GetBitmap() != NULL)
				{
					UpdateDynamic();
				}
				m_LastUpdate = aclock;
			}
		}

		ShowWindowIfAppropriate();
		return 0;
	}
	else if(wParam == FADE_TIMER)
	{
		DWORD ticks = GetTickCount();
		if (m_FadeStartTime == 0)
		{
			m_FadeStartTime = ticks;
		}

		if (ticks - m_FadeStartTime > CConfig::Instance().GetFadeDuration())
		{
			KillTimer(m_Window, FADE_TIMER);
			m_FadeStartTime = 0;
			if (m_FadeEndValue == 0)
			{
				HideWindow();
			}
			else
			{
				UpdateTransparency(m_FadeEndValue);
			}
		}
		else
		{
			double value = (ticks - m_FadeStartTime);
			value /= CConfig::Instance().GetFadeDuration();
 			value *= m_FadeEndValue - m_FadeStartValue;
			value += m_FadeStartValue;
			value = min(value, 255);
			value = max(value, 0);

			UpdateTransparency(value);
		}
	}

	return DefWindowProc(m_Window, WM_TIMER, wParam, lParam);
}

void CRainWindow::UpdateDynamic() 
{
	if (!m_WindowMoved)
	{
		DrawDynamic();
	}
	else
	{
		Redraw();
	}
	UpdateTransparency(m_TransparencyValue);
	InvalidateRect(m_Window, NULL, false);
}

/*
** ExecuteCommand
**
** Runs the given command or bang
**
*/
void CRainWindow::ExecuteCommand(const char* command) 
{
	if (command == NULL) return;

	// Check for build-ins
	if (strncmp("PLAY ", command, 5) == 0)
	{
		BOOL ret = PlaySound(command + 5, NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
		return;
	}
	
	// Run the command
	if(command[0] == '!' && CRainlendar::GetDummyLitestep())
	{
		// Fake WM_COPY to deliver bangs
		COPYDATASTRUCT CopyDataStruct;
		CopyDataStruct.cbData = (DWORD)(strlen(command) + 1);
		CopyDataStruct.dwData = 1;
		CopyDataStruct.lpData = (void*)command;
		GetRainlendar()->GetCalendarWindow().OnCopyData(NULL, (LPARAM)&CopyDataStruct);
	}
	else
	{
		// This can run bangs also on Litestep
		LSExecute(NULL, command, SW_SHOWNORMAL);
	}
}
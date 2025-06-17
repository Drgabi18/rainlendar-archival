/*
  Copyright (C) 2002 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Plugin/Tooltip.cpp,v 1.9 2004/04/24 11:20:40 rainy Exp $

  $Log: Tooltip.cpp,v $
  Revision 1.9  2004/04/24 11:20:40  rainy
  Multimonitor fix

  Revision 1.8  2004/01/10 15:17:43  rainy
  no message

  Revision 1.7  2003/10/27 17:40:01  Rainy
  Config is now singleton.

  Revision 1.6  2003/10/04 14:52:36  Rainy
  Added word wrapping.

  Revision 1.5  2003/05/25 18:08:44  Rainy
  Added tooltip separator.

  Revision 1.4  2002/11/25 17:00:29  rainy
  Tooltip timer is only restarted if the mouse position changes.

  Revision 1.3  2002/08/24 11:08:34  rainy
  Removed hooking code.

  Revision 1.2  2002/08/10 08:38:27  rainy
  The tip is only shown when mouse is over the parent window.

  Revision 1.1  2002/08/03 16:37:10  rainy
  Intial version.

*/

#include "RainlendarDLL.h"
#include "ToolTip.h"
#include "Error.h"
#include "RasterizerFont.h"

// Timers
#define TOOLTIP_TIMER 2

// Globals
CToolTip CToolTip ::c_ToolTip;
int CToolTip::c_CurrentID = 1;
POINT CToolTip::c_MousePos = {0, 0};

#define CORNER_ROUNDING 20
#define ARROW_SIZE 10

/* 
** CToolTip
**
** Constructor
**
*/
CToolTip::CToolTip() 
{
	m_Hidden = true;
	m_Window = NULL;
	m_Delay = 500;	// Half sec
	m_Parent = NULL;
	m_Instance = NULL;
	m_Message = 0;
	m_BackgroundColor = CConfig::Instance().GetToolTipBGColor();
	m_CurrentTip = NULL;
	m_ClickedAway = false;
	m_Font = NULL;
	m_Offset.x = 20;
	m_Offset.y = 0;
	m_InsideRect.left = 10;
	m_InsideRect.top = 6;
	m_InsideRect.right = 10;
	m_InsideRect.bottom = 16;
	m_ArrowCorner = CORNER_BL;
	m_Separation = 0;
	m_Separator = false;
	m_MaxWidth = 0;
}

/* 
** ~CToolTip	
**
** Destructor
**
*/
CToolTip::~CToolTip()
{
}

void CToolTip::Finalize()
{
	DeleteAllToolTips();
	
	if (m_Font) 
	{
		DeleteObject(m_Font);
		m_Font = NULL;
	}

	if(m_Window) DestroyWindow(m_Window);
	m_Window = NULL;

	// Unregister the window class
	UnregisterClass("RainlendarToolTip", m_Instance);

	LSLog(LOG_DEBUG, "Rainlendar", "ToolTip Finalized.");
}

void CToolTip::Initialize(HWND parent, HINSTANCE instance)
{
	if(parent == NULL || instance == NULL) 
	{
		THROW(ERR_NULLPARAMETER);
	}

	m_Parent = parent;
	m_Instance = instance;

	// Register the windowclass
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(WNDCLASSEX));
	wc.style = CS_NOCLOSE;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = m_Instance;
	wc.lpszClassName = "RainlendarToolTip";
	
	if(!RegisterClassEx(&wc))
	{
		if (ERROR_CLASS_ALREADY_EXISTS != GetLastError())
		{
			THROW(ERR_WINDOWCLASS);
		}
	}

	m_Window = CreateWindowEx(WS_EX_TOOLWINDOW, 
							"RainlendarToolTip", 
							NULL, 
							WS_POPUP,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							m_Parent,
							NULL,
							m_Instance,
							this);

	if(m_Window == NULL) 
	{ 
		THROW(ERR_WINDOW);
	}

	m_Font = CRasterizerFont::CreateFont(CConfig::Instance().GetToolTipFont());

	LSLog(LOG_DEBUG, "Rainlendar", "ToolTip Initialized.");
}

void CToolTip::SetFont(const std::string& font)
{
	if (m_Font) DeleteObject(m_Font);
	m_Font = CRasterizerFont::CreateFont(font);
}

int CToolTip::CreateToolTip(const RECT& rect)
{
	ToolTip* tt = new ToolTip;
	tt->rect = rect;
	m_ToolTips[c_CurrentID] = tt;
	c_CurrentID++;

	return c_CurrentID - 1;
}

bool CToolTip::DeleteToolTip(int ID)
{
	std::map<int, ToolTip*>::iterator i = m_ToolTips.find(ID);

	if (i != m_ToolTips.end())
	{
		delete (*i).second;
		m_ToolTips.erase(i);
		return true;
	}

	return false;
}

void CToolTip::DeleteAllToolTips()
{
	std::map<int, ToolTip*>::iterator i = m_ToolTips.begin();
	for ( ; i != m_ToolTips.end(); i++)
	{
		delete (*i).second;
	}
	m_ToolTips.clear();
}

bool CToolTip::AddData(int ID, const ToolTipData& data)
{
	std::map<int, ToolTip*>::iterator i = m_ToolTips.find(ID);

	if (i != m_ToolTips.end())
	{
		((*i).second)->datas.push_back(data);
		return true;
	}
	return false;
}

CToolTip::ToolTip* CToolTip::IsOnTip(POINT p)
{
	RECT r;

	// Transform the point to client space
	GetWindowRect(m_Parent, &r);
	p.x -= r.left;
	p.y -= r.top;

	std::map<int, ToolTip*>::iterator i = m_ToolTips.begin();
	for ( ; i != m_ToolTips.end(); i++)
	{
		ToolTip* tt = (*i).second;
		
		if (tt->rect.left <= p.x && tt->rect.right >= p.x &&
			tt->rect.top <= p.y && tt->rect.bottom >= p.y)
		{
			return tt;
		}
	}
	return NULL;
}

void CToolTip::ShowTip(const POINT& pos, ToolTip* tip)
{
	m_CurrentTip = tip;

	HWND win = WindowFromPoint(pos);
	if (win == m_Parent && IsWindowVisible(m_Parent) && ResizeTipWindow(pos))
	{
		ShowWindow(m_Window, SW_SHOWNOACTIVATE);
		m_Hidden = false;
	} 
	else
	{
		HideTip();
	}
}

void CToolTip::HideTip()
{
	KillTimer(m_Window, TOOLTIP_TIMER);
	ShowWindow(m_Window, SW_HIDE);
	m_Hidden = true;
	m_CurrentTip = NULL;
}

bool CToolTip::ResizeTipWindow(const POINT& pos)
{
	bool hasText = false;

	if (m_CurrentTip)
	{
		int width = 0, height = 0;
		RECT rect;
		HDC dc = GetWindowDC(m_Window);
		HFONT oldFont = (HFONT)SelectObject(dc, m_Font);

		std::list<ToolTipData>::iterator i = m_CurrentTip->datas.begin();
		for ( ; i != m_CurrentTip->datas.end(); i++)
		{
			if (!((*i).text.empty()))
			{
				DrawText(dc, (*i).text.c_str(), (*i).text.size(), &rect, DT_NOPREFIX | DT_CALCRECT);

				if (m_MaxWidth > 0 && rect.right - rect.left > m_MaxWidth)
				{
					rect.right = m_MaxWidth;
					rect.left = 0;
					DrawText(dc, (*i).text.c_str(), (*i).text.size(), &rect, DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);
				}
				
				height += rect.bottom - rect.top;
				if (i != m_CurrentTip->datas.begin())
				{
					height += m_Separation;
				}
				width = max(width, rect.right - rect.left);
				(*i).height = rect.bottom - rect.top;
				hasText = true;
			}
			else
			{
				(*i).height = 0;
			}
		}
		SelectObject(dc, oldFont);
		ReleaseDC(m_Window, dc);

		height += m_InsideRect.top + m_InsideRect.bottom;
		width += m_InsideRect.right + m_InsideRect.left;
		width = max(width, 50);
		int x = pos.x - m_Offset.x;
		int y = pos.y - height - m_Offset.y;

		// Make sure that the tip is fully visible
		GetClientRect(GetDesktopWindow(), &rect); 

		// Check for multiple monitors
		typedef HMONITOR (WINAPI * FPMONITORFROMPOINT)(POINT pt, DWORD dwFlags);
		typedef BOOL (WINAPI * FPGETMONITORINFO)(HMONITOR hMonitor, LPMONITORINFO lpmi);
		FPMONITORFROMPOINT fpMonitorFromPoint;
		FPGETMONITORINFO fpGetMonitorInfo;

		HINSTANCE lib = LoadLibrary("user32.dll");
		
		fpMonitorFromPoint = (FPMONITORFROMPOINT)GetProcAddress(lib, "MonitorFromPoint");
		fpGetMonitorInfo = (FPGETMONITORINFO)GetProcAddress(lib, "GetMonitorInfoA");

		if (fpMonitorFromPoint && fpGetMonitorInfo)
		{
			HMONITOR hMonitor;
			MONITORINFO mi;

			hMonitor = fpMonitorFromPoint(pos, MONITOR_DEFAULTTONULL);

			if(hMonitor != NULL)
			{
				mi.cbSize = sizeof(mi);
				fpGetMonitorInfo(hMonitor, &mi);
				rect = mi.rcWork;
			}
		}
		FreeLibrary(lib);

		POINT center;
		center.x = (rect.right - rect.left) / 2 + rect.left;
		center.y = (rect.bottom - rect.top) / 2 + rect.top;

		// The horizontal orientation depends on which side the mouse is
		if (pos.x > center.x)
		{
			x = x - width + 2 * m_Offset.x;

			// The tooltip is always on top of the mouse, unless it doesn't fit there 
			// (in which case it'll depend on the position of the mouse)
			if ((pos.y - height < rect.top) && (pos.y < center.y))
			{
				m_ArrowCorner = CORNER_TR;
				y = y + height + 2 * m_Offset.y;
			}
			else
			{
				m_ArrowCorner = CORNER_BR;
			}
		}
		else
		{
			if ((pos.y - height < rect.top) && (pos.y < center.y))
			{
				m_ArrowCorner = CORNER_TL;
				y = y + height + 2 * m_Offset.y;
			}
			else
			{
				m_ArrowCorner = CORNER_BL;
			}
		}

		// Open the tip in the center of the tip are (instead of under pointer)
		SetWindowPos(m_Window, HWND_TOPMOST, x, y, width, height, SWP_NOACTIVATE);

		// Create the region for the window
		POINT points[3];

		switch(m_ArrowCorner)
		{
		case CORNER_TL:
			points[0].x = m_Offset.x;
			points[0].y = -1;
			points[1].x = m_Offset.x;
			points[1].y = ARROW_SIZE + 1;
			points[2].x = m_Offset.x + ARROW_SIZE + 3;
			points[2].y = ARROW_SIZE + 1;
			break;

		case CORNER_TR:
			points[0].x = width - m_Offset.x + 1;
			points[0].y = -1;
			points[1].x = width - m_Offset.x + 1;
			points[1].y = ARROW_SIZE + 1;
			points[2].x = width - m_Offset.x - (ARROW_SIZE + 3);
			points[2].y = ARROW_SIZE + 1;
			break;

		case CORNER_BL:
			points[0].x = m_Offset.x;
			points[0].y = 1 + height;
			points[1].x = m_Offset.x;
			points[1].y = -ARROW_SIZE - 2 + height;
			points[2].x = m_Offset.x + ARROW_SIZE + 3;
			points[2].y =  -ARROW_SIZE - 2 + height;
			break;

		case CORNER_BR:
			points[0].x = width - m_Offset.x + 1;
			points[0].y = 1 + height;
			points[1].x = width - m_Offset.x + 1;
			points[1].y = -ARROW_SIZE - 2 + height;
			points[2].x = width - m_Offset.x - (ARROW_SIZE + 3);
			points[2].y =  -ARROW_SIZE - 2 + height;
			break;
		}

		HRGN region1;
		if (m_ArrowCorner == CORNER_TR || m_ArrowCorner == CORNER_TL)
		{
			region1 = CreateRoundRectRgn(0, ARROW_SIZE + 1, width + 1, height + 1, CORNER_ROUNDING - 1, CORNER_ROUNDING - 1);
		}
		else
		{
			region1 = CreateRoundRectRgn(0, 0, width + 1, height - ARROW_SIZE + 1, CORNER_ROUNDING - 1, CORNER_ROUNDING - 1);
		}
		HRGN region2 = CreatePolygonRgn(points, 3, ALTERNATE);
		HRGN region3 = CreateRectRgn(0, 0, width, height);
		CombineRgn(region3, region1, region2, RGN_OR);
		SetWindowRgn(m_Window, region3, TRUE);
		DeleteObject(region1);
		DeleteObject(region2);
	}

	return hasText;
}

/* 
** OnPaint
**
** Handles the paint message.
**
*/
LRESULT CToolTip::OnPaint(WPARAM wParam, LPARAM lParam) 
{
	PAINTSTRUCT ps;
	HDC winDC = BeginPaint(m_Window, &ps);

	if (m_CurrentTip)
	{
		RECT rect;
		GetClientRect(m_Window, &rect);

		POINT points[3];
		int fixX, fixY;

		switch(m_ArrowCorner)
		{
		case CORNER_TL:
			points[0].x = m_Offset.x;
			points[0].y = 0;
			points[1].x = m_Offset.x;
			points[1].y = ARROW_SIZE + 2;
			points[2].x = m_Offset.x + ARROW_SIZE + 1;
			points[2].y = ARROW_SIZE + 2;
			fixX = points[1].x;
			fixY = points[1].y;
			break;

		case CORNER_TR:
			points[0].x = rect.right - m_Offset.x;
			points[0].y = 0;
			points[1].x = rect.right - m_Offset.x;
			points[1].y = ARROW_SIZE + 2;
			points[2].x = rect.right - m_Offset.x - (ARROW_SIZE + 2);
			points[2].y = ARROW_SIZE + 2;
			fixX = points[2].x;
			fixY = points[2].y;
			break;

		case CORNER_BL:
			points[0].x = m_Offset.x;
			points[0].y = rect.bottom;
			points[1].x = m_Offset.x;
			points[1].y = -ARROW_SIZE - 2 + rect.bottom;
			points[2].x = m_Offset.x + ARROW_SIZE + 2;
			points[2].y =  -ARROW_SIZE - 2 + rect.bottom;
			fixX = points[1].x;
			fixY = points[1].y;
			break;

		case CORNER_BR:
			points[0].x = rect.right - m_Offset.x;
			points[0].y = rect.bottom;
			points[1].x = rect.right - m_Offset.x;
			points[1].y = -ARROW_SIZE - 2 + rect.bottom;
			points[2].x = rect.right - m_Offset.x - (ARROW_SIZE + 2);
			points[2].y =  -ARROW_SIZE - 2 + rect.bottom;
			fixX = points[2].x;
			fixY = points[2].y;
			break;
		}

		HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		HPEN oldPen = (HPEN)SelectObject(winDC, pen);
		HBRUSH brush = CreateSolidBrush(m_BackgroundColor);
		HBRUSH oldBrush = (HBRUSH)SelectObject(winDC, brush);

		if (m_ArrowCorner == CORNER_TR || m_ArrowCorner == CORNER_TL)
		{
			RoundRect(winDC, 0, ARROW_SIZE + 1, rect.right, rect.bottom, CORNER_ROUNDING, CORNER_ROUNDING);
		}
		else
		{
			RoundRect(winDC, 0, 0, rect.right, rect.bottom - ARROW_SIZE, CORNER_ROUNDING, CORNER_ROUNDING);
		}

		Polygon(winDC, points, 3);

		// Remove the top line of the polygon
		HPEN pen2 = CreatePen(PS_SOLID, 1, m_BackgroundColor);
		SelectObject(winDC, pen2);
		MoveToEx(winDC, fixX, fixY, NULL);
		LineTo(winDC, fixX + ARROW_SIZE + 3, fixY);
		SelectObject(winDC, pen);
		
		HFONT oldFont = (HFONT)SelectObject(winDC, m_Font);
		SetBkMode(winDC, TRANSPARENT);

		rect.left += m_InsideRect.left;
		rect.right -= m_InsideRect.right;
		rect.top += m_InsideRect.top;
		rect.bottom -= m_InsideRect.bottom;

		if (m_ArrowCorner == CORNER_TR || m_ArrowCorner == CORNER_TL)
		{
			rect.top += ARROW_SIZE + 1;
			rect.bottom += ARROW_SIZE + 1;
		}

		std::list<ToolTipData>::iterator i = m_CurrentTip->datas.begin();
		for ( ; i != m_CurrentTip->datas.end(); i++)
		{
			if (m_Separator && i != m_CurrentTip->datas.begin())
			{
				MoveToEx(winDC, rect.left, rect.top - (m_Separation / 2), NULL);
				LineTo(winDC, rect.right, rect.top - (m_Separation / 2));
			}

			SetTextColor(winDC, (*i).color);
			DrawText(winDC, (*i).text.c_str(), (*i).text.size(), &rect, DT_NOPREFIX | DT_WORDBREAK);
			rect.top += (*i).height + m_Separation;
		}

		SelectObject(winDC, oldFont);
		SelectObject(winDC, oldPen);
		SelectObject(winDC, oldBrush);
		DeleteObject(pen);
		DeleteObject(pen2);
		DeleteObject(brush);
	}

	EndPaint(m_Window, &ps);
	return 0;
}

/* 
** OnTimer
**
** Handles the timer message. Show the window on timer message.
**
*/
LRESULT CToolTip::OnTimer(WPARAM wParam, LPARAM lParam) 
{
	if(wParam == TOOLTIP_TIMER) 
	{
		POINT pos;
		GetCursorPos(&pos);
		
		ToolTip* tip = IsOnTip(pos);

		if (m_Hidden && !c_ToolTip.m_ClickedAway && m_CurrentTip != tip)
		{
			ShowTip(pos, tip);
			return 0;
		}

		if (!m_Hidden && tip == NULL)
		{
			HideTip();
			return 0;
		}

		if (tip == NULL)
		{
			m_ClickedAway = false;	// This also resets the clicked away status
		}
	}

	return DefWindowProc(m_Window, WM_TIMER, wParam, lParam);
}

/* 
** OnMouse
**
** Handles the mouse events message. 
**
*/
LRESULT CToolTip::OnMouse(WPARAM wParam, LPARAM lParam) 
{
	if(!c_ToolTip.m_Hidden)
	{
		c_ToolTip.HideTip();				// Hide the tooltip
		c_ToolTip.m_ClickedAway = true;
	}
	
	return DefWindowProc(m_Window, m_Message, wParam, lParam);
}

/* 
** WndProc
**
** The window procedure for the tooltip
**
*/
LRESULT CALLBACK CToolTip::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CToolTip* Window = NULL;

	if (Window) Window->m_Message = uMsg;

	if(uMsg == WM_CREATE) 
	{
		// Fetch this window-object from the CreateStruct
		Window = (CToolTip*)((LPCREATESTRUCT)lParam)->lpCreateParams;
	}

	BEGIN_MESSAGEPROC
	MESSAGE(OnPaint, WM_PAINT)
	MESSAGE(OnTimer, WM_TIMER)
	MESSAGE(OnMouse, WM_LBUTTONDBLCLK)
	MESSAGE(OnMouse, WM_LBUTTONDOWN)
	MESSAGE(OnMouse, WM_LBUTTONUP)
	MESSAGE(OnMouse, WM_RBUTTONDBLCLK)
	MESSAGE(OnMouse, WM_RBUTTONDOWN)
	MESSAGE(OnMouse, WM_RBUTTONUP)
	MESSAGE(OnMouse, WM_NCLBUTTONDBLCLK)
	MESSAGE(OnMouse, WM_NCLBUTTONDOWN)
	MESSAGE(OnMouse, WM_NCLBUTTONUP)
	MESSAGE(OnMouse, WM_NCRBUTTONDBLCLK)
	MESSAGE(OnMouse, WM_NCRBUTTONDOWN)
	MESSAGE(OnMouse, WM_NCRBUTTONUP)
	END_MESSAGEPROC
}

/* 
** MouseProc
**
** The procedure for the 'hook'
**
*/
LRESULT CALLBACK CToolTip::MouseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_NCLBUTTONDBLCLK:
	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONDBLCLK:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
		if(!c_ToolTip.m_Hidden)
		{
			c_ToolTip.HideTip();				// Hide the tooltip
			c_ToolTip.m_ClickedAway = true;
		}
		break;

	case WM_MOUSEMOVE:
	case WM_NCMOUSEMOVE:
		POINT pt;
		pt.x = LOWORD(lParam); 
		pt.y = HIWORD(lParam); 
		
		if (uMsg == WM_MOUSEMOVE)
		{
			// Convert to screen space
			RECT r;
			GetWindowRect(hWnd, &r);
			pt.x = pt.x + r.left;
			pt.y = pt.y + r.top;
		}

		ToolTip* tip = c_ToolTip.IsOnTip(pt);

		if(!c_ToolTip.m_Hidden && !tip)
		{
			// Hide the tooltip
			c_ToolTip.HideTip();
			c_ToolTip.m_ClickedAway = false;	// This also resets the clicked away status
		}
		else if (c_ToolTip.m_CurrentTip && tip != c_ToolTip.m_CurrentTip)
		{
			// If mouse is moved to another tip, show it
			c_ToolTip.ShowTip(pt, tip);
			InvalidateRect(c_ToolTip.m_Window, NULL, FALSE);
		}
		else if (c_MousePos.x != pt.x || c_MousePos.y != pt.y)
		{
			// Restart the timer
			KillTimer(c_ToolTip.m_Window, TOOLTIP_TIMER);
			SetTimer(c_ToolTip.m_Window, TOOLTIP_TIMER, c_ToolTip.m_Delay, NULL);
		}
		
		c_MousePos.x = pt.x;
		c_MousePos.y = pt.y;
		break;
	}

	return FALSE;
}

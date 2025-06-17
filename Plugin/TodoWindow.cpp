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
  $Header: /home/cvsroot/Rainlendar/Plugin/TodoWindow.cpp,v 1.6 2004/01/28 18:07:46 rainy Exp $

  $Log: TodoWindow.cpp,v $
  Revision 1.6  2004/01/28 18:07:46  rainy
  Window visibility is not set in refresh.

  Revision 1.5  2004/01/25 10:02:28  rainy
  Fixed refresh

  Revision 1.4  2003/12/20 22:15:18  rainy
  Added OnPowerBroadcast.
  Refreshing is not done if the window is not enabled.

  Revision 1.3  2003/12/05 15:47:41  Rainy
  No bitmap necessary.

  Revision 1.2  2003/10/27 19:50:43  Rainy
  Fixed the WndProc (it cannot be a member).

  Revision 1.1  2003/10/27 17:34:54  Rainy
  Initial version.

*/

#include "RainlendarDLL.h"
#include "TodoWindow.h"
#include "CalendarWindow.h"
#include "TodoDialog.h"
#include "Litestep.h"

/* 
** CTodoWindow
**
** Constructor
**
*/
CTodoWindow::CTodoWindow() : CRainWindow()
{
	m_WindowName = "TodoWindow";
	m_ClassName = "RainlendarTodoWindow";
	m_WndProc = CTodoWindow::WndProc;

	m_ItemRasterizer = NULL;
	m_TotalHeight = 0;
	m_SelectedTodo = -1;
}

/* 
** ~CTodoWindow
**
** Destructor
**
*/
CTodoWindow::~CTodoWindow()
{
	CRasterizerFont::ClearBuffers();
}

bool CTodoWindow::Initialize(HWND parent, HINSTANCE instance)
{
	// Must set the correct position before the window is created so that it can be put in correct place
	m_X = CConfig::Instance().GetTodoX();
	m_Y = CConfig::Instance().GetTodoY();

	return CRainWindow::Initialize(parent, instance);
}

void CTodoWindow::Refresh(bool lite)
{
	if (!lite)
	{
		m_TodoManager.Read();
	}

	if (CConfig::Instance().GetTodoEnable())
	{
		if (!lite)
		{
			if (m_ItemRasterizer) delete m_ItemRasterizer;

			m_ItemRasterizer=new CRasterizerFont;
			if(m_ItemRasterizer==NULL) THROW(ERR_OUTOFMEM);

			m_ItemRasterizer->SetFont(CConfig::Instance().GetTodoFont());
			m_ItemRasterizer->SetAlign(CRasterizer::ALIGN_LEFT);
			m_ItemRasterizer->SetColor(CConfig::Instance().GetTodoFontColor());

			m_TotalHeight = CalcTextHeight();

			m_TodoBackground.Initialize();

			m_X = CConfig::Instance().GetTodoX();
			m_Y = CConfig::Instance().GetTodoY();
		}
		
		CRainWindow::RefreshBegin(lite);
		
		if (!lite)
		{
			BackgroundCreateStruct bcs;
			bcs.pos.x = m_X;
			bcs.pos.y = m_Y;
			bcs.size.cx = CConfig::Instance().GetTodoW();
			bcs.size.cy = CalcTextHeight();
			bcs.filename = CConfig::Instance().GetTodoBitmapName();
			if (CConfig::Instance().GetTodoBitmapName().empty())
			{
				bcs.mode = MODE_SOLID;
			}
			else
			{
				bcs.mode = MODE_RECT;
			}
			bcs.solidBevel = TRUE;
			bcs.solidColor= GetSysColor(COLOR_BTNFACE);
			bcs.resizeRect= CConfig::Instance().GetTodoBitmapMargins();
			
			m_TodoBackground.Create(bcs);

			// The size could be limited by the background, so we need to reset the width and height
			if (m_Width != m_TodoBackground.GetWidth() || m_Height != m_TodoBackground.GetHeight())
			{
				m_Width = m_TodoBackground.GetWidth();
				m_Height = m_TodoBackground.GetHeight();
				MoveWindow(m_X, m_Y);	// This also resizes the window
			}

			SetWindowZPos(CConfig::Instance().GetWindowPos());		// Uses the global z-pos

			m_ItemImage.Clear();
			if (!CConfig::Instance().GetTodoItemBitmapName().empty())
			{
				std::string name = CConfig::Instance().GetTodoItemBitmapName();
				CConfig::AddPath(name);

				if (!m_ItemImage.Load(name.c_str()))
				{
					char tmpSz[MAX_LINE_LENGTH];
					if (CConfig::Instance().GetTodoItemBitmapName().empty())
					{
						strcpy(tmpSz, CCalendarWindow::c_Language.GetString("Error", 9));
					}
					else
					{
						sprintf(tmpSz, CCalendarWindow::c_Language.GetString("Error", 8), CConfig::Instance().GetTodoBitmapName().c_str());
					}
					MessageBox(NULL, tmpSz, "Rainlendar", MB_OK | MB_ICONERROR);
				}
			}
		}

		CRainWindow::RefreshEnd(lite);
	}
}

int CTodoWindow::CalcTextHeight()
{
	int totalHeight = 0;
	m_Heights.clear();

	// Calc text heights
	RECT rect = {0, 0, 0, 0};
	RECT margins = CConfig::Instance().GetTodoTextMargins();
	int width = CConfig::Instance().GetTodoW() - margins.left - margins.right;
	width = max(0, width);

	for (int i = 0; i < m_TodoManager.GetTodos().size(); i++)
	{
		rect.right = width;

		const char* text = m_TodoManager.GetTodos()[i].c_str();
		m_ItemRasterizer->GetTextSize(text, &rect, true);
		
		totalHeight += rect.bottom + CConfig::Instance().GetTodoSeparation();
		m_Heights.push_back(rect.bottom);
	}
	totalHeight -= CConfig::Instance().GetTodoSeparation();

	RECT rc = CConfig::Instance().GetTodoTextMargins();

	return totalHeight + rc.top + rc.bottom;
}

SIZE CTodoWindow::CalcWindowSize()
{
	SIZE size;

	size.cx = CConfig::Instance().GetTodoW();

	if (m_TodoBackground.IsValid())
	{
		// The proper height has been calculated in the Refresh()
		size.cy = m_TodoBackground.GetHeight();
		size.cx = max(size.cx, m_TodoBackground.GetWidth());
	}
	else
	{
		size.cy = m_TotalHeight;
	}

	return size;
}

void CTodoWindow::DrawWindow()
{
	int X, Y, W, H;

	CRasterizerFont::ClearBuffers();

	RECT rc = CConfig::Instance().GetTodoTextMargins();

	SIZE size = CalcWindowSize();

	X = rc.left;
	Y = rc.top;
	W = size.cx - rc.left - rc.right;

	// Draw the background
	if (m_TodoBackground.IsValid())
	{
		m_TodoBackground.Paint(m_DoubleBuffer);
	}

	for (int i = 0; i < m_TodoManager.GetTodos().size(); i++)
	{
		H = m_Heights[i];

			// Draw the item image
			if (m_ItemImage.GetBitmap())
			{
				int x = X;
				int y = Y;

				// Align the bitmap correctly
				switch (CConfig::Instance().GetTodoItemAlign() & 0x0F)
				{
				case CRasterizer::ALIGN_LEFT:
					x = X;
					break;

				case CRasterizer::ALIGN_HCENTER:
					x = X + (W - m_ItemImage.GetWidth()) / 2;
					break;

				case CRasterizer::ALIGN_RIGHT:
					x = X + W - m_ItemImage.GetWidth();
					break;
				};

				switch (CConfig::Instance().GetTodoItemAlign() & 0x0F0)
				{
				case CRasterizer::ALIGN_TOP:
					y = Y;
					break;

				case CRasterizer::ALIGN_VCENTER:
					y = Y + (H - m_ItemImage.GetHeight()) / 2;
					break;

				case CRasterizer::ALIGN_BOTTOM:
					y = Y + (H - m_ItemImage.GetHeight());
					break;
				};

				x += CConfig::Instance().GetTodoItemOffset().x;
				y += CConfig::Instance().GetTodoItemOffset().y;

				m_DoubleBuffer.Blit(m_ItemImage, x, y, 0, 0, m_ItemImage.GetWidth(), m_ItemImage.GetHeight());
			}

		// Draw the text
		const char* text = m_TodoManager.GetTodos()[i].c_str();

		m_ItemRasterizer->WriteText(m_DoubleBuffer, X, Y, W, H, text, true);
		Y += m_Heights[i] + CConfig::Instance().GetTodoSeparation();
	}

	// The text might be written to the buffers, so draw them
	if (CRasterizerFont::GetColorBuffer().GetBitmap() != NULL  && CRasterizerFont::GetAlphaBuffer().GetBitmap() != NULL)
	{
		CImage textImage;
		textImage.Create(CRasterizerFont::GetColorBuffer().GetBitmap(), CRasterizerFont::GetAlphaBuffer().GetBitmap(), 0);
		m_DoubleBuffer.Blit(textImage, 0, 0, 0, 0, textImage.GetWidth(), textImage.GetHeight());
	}
}

/* 
** HitTest
**
** Returns the todo under the given coordinates
**
*/
int CTodoWindow::HitTest(int x, int y)
{
	RECT rc = CConfig::Instance().GetTodoTextMargins();

	if (x < rc.left || x > CConfig::Instance().GetTodoW() - rc.right) return -1;
	if (y < rc.top) return -1;

	int curH = rc.top;
	for (int i = 0; i < m_Heights.size(); i++)
	{
		curH += m_Heights[i] + CConfig::Instance().GetTodoSeparation();

		if (y < curH) 
		{
			char buffer[256];
			sprintf(buffer, "Todo HitTest index: %i", i);
			LSLog(LOG_DEBUG, "Rainlendar", buffer);
			return i;
		}
	}

	return -1;
}

LRESULT CTodoWindow::OnContextMenu(WPARAM wParam, LPARAM lParam) 
{
	int xPos = (SHORT)LOWORD(lParam); 
	int yPos = (SHORT)HIWORD(lParam); 

	// Transform the point to client rect
	RECT rect;
	GetWindowRect(m_Window, &rect);
	xPos = xPos - rect.left;
	yPos = yPos - rect.top;

	HMENU menu, subMenu = NULL;
	
	menu = LoadMenu(CRainlendar::GetInstance(), MAKEINTRESOURCE(IDR_MENU1));
	if (menu) subMenu = GetSubMenu(menu, 1);

	if (subMenu)
	{
		ModifyMenu(subMenu, ID_TODO, MF_BYCOMMAND | MF_STRING, ID_TODO, CCalendarWindow::c_Language.GetString("Menu", 29));	// Edit todo
		ModifyMenu(subMenu, ID_MARKASDONE, MF_BYCOMMAND | MF_STRING, ID_MARKASDONE, CCalendarWindow::c_Language.GetString("Menu", 30));	// Mark as done

		m_SelectedTodo = HitTest(xPos, yPos);
		if (m_SelectedTodo != -1)
		{
			EnableMenuItem( subMenu, ID_MARKASDONE, MF_ENABLED);
		} 
		else 
		{
			EnableMenuItem( subMenu, ID_MARKASDONE, MF_GRAYED);
		}

		TrackPopupMenu(
		  subMenu,
		  TPM_RIGHTBUTTON | TPM_LEFTALIGN, 
		  (SHORT)LOWORD(lParam),
		  (SHORT)HIWORD(lParam),
		  0,
		  m_Window,
		  NULL
		);		
		DestroyMenu(subMenu);
	}
	DestroyMenu(menu);

	return 0;
}

LRESULT CTodoWindow::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch(LOWORD(wParam)) 
	{
		case ID_TODO:
			return GetRainlendar()->GetCalendarWindow().OnCommand(wParam, lParam);

		case ID_MARKASDONE:
 			if (m_SelectedTodo != -1)
			{
				std::vector<std::string>& todos = m_TodoManager.GetTodos();
				std::vector<std::string>& dones = m_TodoManager.GetDones();
				dones.push_back(todos[m_SelectedTodo]);
				todos.erase(todos.begin() + m_SelectedTodo);
				m_TodoManager.Write();
				Refresh(false);		// No need to refresh the main window
			}
			break;
	}

	return 0;
}

LRESULT CTodoWindow::OnLButtonDblClk(WPARAM wParam, LPARAM lParam) 
{
	int x = (SHORT)LOWORD(lParam);
	int y = (SHORT)HIWORD(lParam);

	if(m_Message == WM_NCLBUTTONDBLCLK)
	{
		// Transform the point to client rect
		RECT rect;
		GetWindowRect(m_Window, &rect);
		x = x - rect.left;
		y = y - rect.top;
	}

	// Check the todo-list
	int index = HitTest(x, y);

	LSLog(LOG_DEBUG, "Rainlendar", "Opening Todo dialog.");
	OpenTodoDialog(m_Window, m_Instance, index);
	LSLog(LOG_DEBUG, "Rainlendar", "Todo dialog closed.");

	return 0;
}

LRESULT CTodoWindow::OnMove(WPARAM wParam, LPARAM lParam) 
{
	short int x = LOWORD(lParam);
	short int y = HIWORD(lParam);

	if(CConfig::Instance().GetMovable())
	{
		if (CConfig::Instance().GetNegativeCoords()) 
		{
			if (x < 0) x = 0;
			if (y < 0) y = 0;
		}

		CConfig::Instance().SetTodoX(x);
		CConfig::Instance().SetTodoY(y);
		CConfig::Instance().WriteConfig(CConfig::WRITE_TODOPOS);	// Store the new position to the ini file
	}

	m_X = x;
	m_Y = y;

	if (CConfig::Instance().GetBackgroundMode() == MODE_COPY || m_TodoBackground.HasAlpha())
	{
		m_TodoBackground.UpdateWallpaper(x, y);
		DrawWindow();
		InvalidateRect(m_Window, NULL, false);
	}

	return 0;
}

LRESULT CTodoWindow::OnPowerBroadcast(WPARAM wParam, LPARAM lParam) 
{
	if (CConfig::Instance().GetTodoEnable())
	{
		return CRainWindow::OnPowerBroadcast(wParam, lParam);
	}
	
	return DefWindowProc(m_Window, m_Message, wParam, lParam);
}

/* 
** WndProc
**
** The window procedure
**
*/
LRESULT CALLBACK CTodoWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CTodoWindow* Window = NULL;

	if (Window) Window->m_Message = uMsg;

	if (uMsg == WM_CREATE) 
	{
		// Fetch this window-object from the CreateStruct
		Window=(CTodoWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
	}

	if (Window)
	{
		switch(uMsg) 
		{
			case WM_CONTEXTMENU:
			case WM_NCRBUTTONUP:
				return Window->OnContextMenu(wParam, lParam);

			case WM_COMMAND:
				return Window->OnCommand(wParam, lParam);

			case WM_LBUTTONDBLCLK:
			case WM_NCLBUTTONDBLCLK:
				return Window->OnLButtonDblClk(wParam, lParam);

			case WM_MOVE: 
				return Window->OnMove(wParam, lParam);

			case WM_PAINT: 
				return Window->OnPaint(wParam, lParam);

			case WM_TIMER: 
				return Window->OnTimer(wParam, lParam);

			case WM_WINDOWPOSCHANGING: 
				return Window->OnWindowPosChanging(wParam, lParam);

			case WM_SETTINGCHANGE: 
				return Window->OnSettingsChange(wParam, lParam);

			case WM_ERASEBKGND: 
				return Window->OnEraseBkgnd(wParam, lParam);

			case WM_MOUSEMOVE: 
			case WM_NCMOUSEMOVE: 
				return Window->OnMouseMove(wParam, lParam);

			case WM_POWERBROADCAST: 
				return Window->OnPowerBroadcast(wParam, lParam);

			case WM_DISPLAYCHANGE: 
				return Window->OnDisplayChange(wParam, lParam);

			case WM_NCHITTEST: 
				return Window->OnNcHitTest(wParam, lParam);
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

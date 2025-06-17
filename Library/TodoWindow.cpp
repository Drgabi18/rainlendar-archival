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
  $Header: /home/cvsroot/Rainlendar/Library/TodoWindow.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: TodoWindow.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.11  2005/03/25 13:58:43  rainy
  no message

  Revision 1.10  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.9  2004/12/05 18:21:34  rainy
  Fixed menu position.

  Revision 1.8  2004/11/06 13:38:59  rainy
  no message

  Revision 1.7  2004/06/10 16:21:30  rainy
  Todo-list is read always.

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
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "TodoWindow.h"
#include "CalendarWindow.h"
#include "TodoDialog.h"
#include "Litestep.h"
#include "RasterizerBitmap.h"
#include <algorithm>

bool TodoCompare(CEntryTodo* a, CEntryTodo* b);		// Defined in CTodoDialog

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
	m_SelectedTodo = NULL;

	ResetSettings();
}

/* 
** ~CTodoWindow
**
** Destructor
**
*/
CTodoWindow::~CTodoWindow()
{
	if (m_ItemRasterizer) delete m_ItemRasterizer;
}

void CTodoWindow::ResetSettings()
{
	CRainWindow::ResetSettings();

	// Reset the values
	m_TodoBitmapName = "";
	m_TodoItemBitmapName = "";
	m_TodoW = 200;
	m_TodoFontColor = GetSysColor(COLOR_BTNTEXT);
	m_TodoFont = "-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_TodoBitmapMargins.left = m_TodoBitmapMargins.top = m_TodoBitmapMargins.right = m_TodoBitmapMargins.bottom = 0;
	m_TodoTextMargins.left = m_TodoTextMargins.top = m_TodoTextMargins.right = m_TodoTextMargins.bottom = 8;
	m_TodoSeparation = 5;
	m_TodoItemOffset.x = m_TodoItemOffset.y = 0;
	m_TodoItemAlign = CRasterizer::ALIGN_LEFT;
}

void CTodoWindow::ReadSettings(const char* iniFile, const char* section)
{
	CRainWindow::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	// Todo stuff
	if(GetPrivateProfileString( section, "TodoBitmapName", m_TodoBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodoBitmapName=tmpSz;
	}
	if(GetPrivateProfileString( section, "TodoBitmapMargins", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i, %i, %i", &m_TodoBitmapMargins.left, &m_TodoBitmapMargins.top, &m_TodoBitmapMargins.right, &m_TodoBitmapMargins.bottom);
	}
	if(GetPrivateProfileString( section, "TodoTextMargins", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i, %i, %i", &m_TodoTextMargins.left, &m_TodoTextMargins.top, &m_TodoTextMargins.right, &m_TodoTextMargins.bottom);
	}
	m_TodoW=GetPrivateProfileInt( section, "TodoW", m_TodoW, iniFile);
	if(GetPrivateProfileString( section, "TodoFont", m_TodoFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodoFont=tmpSz;
	}
	if(GetPrivateProfileString( section, "TodoFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodoFontColor = CConfig::ParseColor(tmpSz);
	}
	m_TodoItemAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( section, "TodoItemAlign", m_TodoItemAlign, iniFile);
	if(GetPrivateProfileString( section, "TodoItemOffset", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i", &m_TodoItemOffset.x, &m_TodoItemOffset.y);
	}
	m_TodoSeparation=GetPrivateProfileInt( section, "TodoSeparation", m_TodoSeparation, iniFile);
	if(GetPrivateProfileString( section, "TodoItemBitmapName", m_TodoItemBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodoItemBitmapName=tmpSz;
	}
}

void CTodoWindow::WriteSettings()
{
	char tmpSz[256];

	std::string INIPath = m_SettingsFile;

	// Todo stuff
	WritePrivateProfileString( m_Section.c_str(), "TodoBitmapName", m_TodoBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i, %i, %i, %i", m_TodoBitmapMargins.left, m_TodoBitmapMargins.top, m_TodoBitmapMargins.right, m_TodoBitmapMargins.bottom);
	WritePrivateProfileString( m_Section.c_str(), "TodoBitmapMargins", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i, %i, %i, %i", m_TodoTextMargins.left, m_TodoTextMargins.top, m_TodoTextMargins.right, m_TodoTextMargins.bottom);
	WritePrivateProfileString( m_Section.c_str(), "TodoTextMargins", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_TodoW);
	WritePrivateProfileString( m_Section.c_str(), "TodoW", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "TodoFont", m_TodoFont.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%X", m_TodoFontColor);
	WritePrivateProfileString( m_Section.c_str(), "TodoFontColor", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "TodoItemBitmapName", m_TodoItemBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i", m_TodoSeparation);
	WritePrivateProfileString( m_Section.c_str(), "TodoSeparation", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i, %i", m_TodoItemOffset.x, m_TodoItemOffset.y);
	WritePrivateProfileString( m_Section.c_str(), "TodoItemOffset", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_TodoItemAlign);
	WritePrivateProfileString( m_Section.c_str(), "TodoItemAlign", tmpSz, INIPath.c_str() );
}

bool CTodoWindow::Initialize(HWND parent, HINSTANCE instance)
{
	// Must set the correct position before the window is created so that it can be put in correct place
	m_X = CConfig::Instance().GetTodoX();
	m_Y = CConfig::Instance().GetTodoY();

	return CRainWindow::Initialize(parent, instance);
}

int CTodoWindow::GetY()
{
	if (CConfig::Instance().GetGrowUpwards() & (1 << (int)RAINWINDOW_TYPE_TODO))
	{
		return m_Y - m_Height;
	}
	return m_Y;
}

void CTodoWindow::Refresh()
{
	CRainWindow::Refresh();

	if (m_ItemRasterizer) delete m_ItemRasterizer;

	m_ItemRasterizer=new CRasterizerFont;
	if(m_ItemRasterizer==NULL) THROW(ERR_OUTOFMEM);

	m_ItemRasterizer->SetFont(GetTodoFont(), true);
	m_ItemRasterizer->SetAlign(CRasterizer::ALIGN_LEFT);
	m_ItemRasterizer->SetColor(GetTodoFontColor());

	m_X = CConfig::Instance().GetTodoX();
	m_Y = CConfig::Instance().GetTodoY();
		
	m_ItemImage.Clear();
	if (!GetTodoItemBitmapName().empty())
	{
		CRasterizerBitmap::LoadImage(GetTodoItemBitmapName(), &m_ItemImage);
	}

	m_TodoBackground.Initialize(GetTodoBitmapName().c_str());

	Redraw();
}

void CTodoWindow::Redraw()
{
	if (m_ItemRasterizer)
	{
		m_TotalHeight = CalcTextHeight();
		SIZE size = CalcWindowSize();

		CRainWindow::RedrawBegin();

		BackgroundCreateStruct bcs;
		bcs.pos.x = m_X;
		bcs.pos.y = m_Y;
		bcs.size.cx = size.cx;
		bcs.size.cy = size.cy;
		
		if (GetTodoBitmapName().empty())
		{
			bcs.mode = MODE_SOLID;
		}
		else
		{
			bcs.mode = MODE_RECT;
		}
		bcs.solidBevel = TRUE;
		bcs.solidColor = GetSysColor(COLOR_BTNFACE);
		bcs.resizeRect = GetTodoBitmapMargins();

		m_TodoBackground.Create(bcs);

		CRainWindow::RedrawEnd();

		SetWindowZPos(CConfig::Instance().GetWindowPos());		// Uses the global z-pos
	}
}

int CTodoWindow::CalcTextHeight()
{
	int totalHeight = 0;
	m_Heights.clear();

	// Calc text heights
	RECT rect = {0, 0, 0, 0};
	RECT margins = GetTodoTextMargins();
	int width = GetTodoW() - margins.left - margins.right;
	width = max(0, width);

	// Copy the list so that it can be sorted
	std::vector<CEntryTodo*> list = GetRainlendar()->GetCalendarWindow().GetEventManager().GetTodos();
	std::sort(list.begin(), list.end(), TodoCompare);

	for (UINT i = 0; i < list.size(); i++)
	{
		// Skip deleted
		rect.right = width;

		const char* text = list[i]->GetMessage();
		m_ItemRasterizer->GetTextSize(text, &rect, true);
		
		totalHeight += rect.bottom + GetTodoSeparation();
		m_Heights.push_back(rect.bottom);
	}
	totalHeight -= GetTodoSeparation();

	RECT rc = GetTodoTextMargins();

	return totalHeight + rc.top + rc.bottom;
}

SIZE CTodoWindow::CalcWindowSize()
{
	SIZE size;

	size.cx = GetTodoW();
	size.cy = m_TotalHeight;

	if (m_TodoBackground.IsValid())
	{
		// Check if the background image is bigger than the calculated window size
		size.cx = max(size.cx, m_TodoBackground.GetImageWidth());
		size.cy = max(size.cy, m_TodoBackground.GetImageHeight());
	}

	return size;
}

void CTodoWindow::DrawWindow()
{
	int X, Y, W, H;

	RECT rc = GetTodoTextMargins();

	SIZE size = CalcWindowSize();

	X = rc.left;
	Y = rc.top;
	W = size.cx - rc.left - rc.right;

	// Draw the background
	if (m_TodoBackground.IsValid())
	{
		m_TodoBackground.Paint(m_DoubleBuffer);
	}

	// Copy the list so that it can be sorted
	std::vector<CEntryTodo*> list = GetRainlendar()->GetCalendarWindow().GetEventManager().GetTodos();
	std::sort(list.begin(), list.end(), TodoCompare);

	for (UINT i = 0; i < list.size(); i++)
	{
		H = m_Heights[i];

		// Draw the item image
		if (m_ItemImage.GetBitmap())
		{
			int x = X;
			int y = Y;

			// Align the bitmap correctly
			switch (GetTodoItemAlign() & 0x0F)
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

			switch (GetTodoItemAlign() & 0x0F0)
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

			x += GetTodoItemOffset().x;
			y += GetTodoItemOffset().y;

			m_DoubleBuffer.Blit(m_ItemImage, x, y, 0, 0, m_ItemImage.GetWidth(), m_ItemImage.GetHeight());
		}

		// Draw the text
		const char* text = list[i]->GetMessage();

		m_ItemRasterizer->WriteText(m_DoubleBuffer, X, Y, W, H, text, true, list[i]->IsChecked());
		Y += m_Heights[i] + GetTodoSeparation();
	}

	POINT offset = {0, 0};
	for (int k = 0; k < m_StaticSkinItems.size(); k++)
	{
		if (m_StaticSkinItems[k]->IsEnabled())
		{
			m_StaticSkinItems[k]->Paint(m_DoubleBuffer, offset);
		}
	}
}

/* 
** HitTest
**
** Returns the todo under the given coordinates
**
*/
GUID* CTodoWindow::HitTest(int x, int y)
{
	RECT rc = GetTodoTextMargins();

	if (x < rc.left || x > GetTodoW() - rc.right) return NULL;
	if (y < rc.top) return NULL;

	int curH = rc.top;
	for (UINT i = 0; i < m_Heights.size(); i++)
	{
		curH += m_Heights[i] + GetTodoSeparation();

		if (y < curH) 
		{
			DebugLog("Todo HitTest index: %i", i);

			std::vector<CEntryTodo*> list = GetRainlendar()->GetCalendarWindow().GetEventManager().GetTodos();
			std::sort(list.begin(), list.end(), TodoCompare);

			if (i < list.size() && !list[i]->IsReadOnly())
			{
				return list[i]->GetGUID();
			}
			return NULL;
		}
	}

	return NULL;
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
	if (menu) subMenu = GetSubMenu(menu, 0);

	if (subMenu)
	{
		HMENU showMenu = subMenu;
		menu = LoadMenu(CRainlendar::GetInstance(), MAKEINTRESOURCE(IDR_MENU1));
		menu = GetSubMenu(menu, 1);
		
		if (!CConfig::Instance().GetShowContextMenu())
		{
			GetRainlendar()->GetCalendarWindow().FillMenu(subMenu, xPos, yPos, 0);

			// Replace Events-submenu with Todo-submenu
			RemoveMenu(subMenu, 13, MF_BYPOSITION);
			InsertMenu(subMenu, 13, MF_BYPOSITION | MF_POPUP, (UINT_PTR)menu, CCalendarWindow::c_Language.GetString("Menu", 42));	// Todo
		}
		else
		{
			showMenu = menu;
		}

		ModifyMenu(menu, ID_TODO_EDIT_ITEM, MF_BYCOMMAND | MF_STRING, ID_TODO_EDIT_ITEM, CCalendarWindow::c_Language.GetString("Menu", 49));	// Edit item...
		ModifyMenu(menu, ID_MARKASDONE, MF_BYCOMMAND | MF_STRING, ID_MARKASDONE, CCalendarWindow::c_Language.GetString("Menu", 30));	// Mark as done
		ModifyMenu(menu, ID_MARKALLCHECKEDASDONE, MF_BYCOMMAND | MF_STRING, ID_MARKALLCHECKEDASDONE, CCalendarWindow::c_Language.GetString("Menu", 50));	// Mark all checked as done
		ModifyMenu(menu, ID_TODO_CHECKITEM, MF_BYCOMMAND | MF_STRING, ID_TODO_CHECKITEM, CCalendarWindow::c_Language.GetString("Menu", 51));	// Mark all checked as done
		ModifyMenu(menu, ID_TODO_DELETEITEM, MF_BYCOMMAND | MF_STRING, ID_TODO_DELETEITEM, CCalendarWindow::c_Language.GetString("Menu", 52));	// Delete item

		m_SelectedTodo = HitTest(xPos, yPos);
		if (m_SelectedTodo != NULL)
		{
			EnableMenuItem(menu, ID_TODO_EDIT_ITEM, MF_ENABLED);
			EnableMenuItem(menu, ID_MARKASDONE, MF_ENABLED);
			EnableMenuItem(menu, ID_TODO_CHECKITEM, MF_ENABLED);
			EnableMenuItem(menu, ID_TODO_DELETEITEM, MF_ENABLED);
		} 
		else 
		{
			EnableMenuItem(menu, ID_TODO_EDIT_ITEM, MF_GRAYED);
			EnableMenuItem(menu, ID_MARKASDONE, MF_GRAYED);
			EnableMenuItem(menu, ID_TODO_CHECKITEM, MF_GRAYED);
			EnableMenuItem(menu, ID_TODO_DELETEITEM, MF_GRAYED);
		}

		TrackPopupMenu(
		  showMenu,
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
		case ID_TODO_DELETEITEM:
 			if (m_SelectedTodo != NULL)
			{
				CEntryTodo* todo = GetRainlendar()->GetCalendarWindow().GetEventManager().GetTodo(m_SelectedTodo);
				if (todo)
				{
					// "Are you sure you want to delete the item?"
					if (IDYES == MessageBox(m_Window, CCalendarWindow::c_Language.GetString("Todo", 7), "Rainlendar", MB_YESNO | MB_ICONQUESTION))
					{
						// Must create a new copy
						CEntryTodo* newTodo = new CEntryTodo(todo);
						newTodo->MarkAsDeleted();
						newTodo->UpdateTimeStamp();
						GetRainlendar()->GetCalendarWindow().GetEventManager().AddItem(newTodo, false);
						Redraw();		// No need to redraw the main window
					}
				}
				else
				{
					DebugLog("Warning: Todo item %s was not found!", CEntryItem::GetGUIDAsString(todo->GetGUID()));
				}
			}
			return TRUE;

		case ID_TODO_CHECKITEM:
 			if (m_SelectedTodo != NULL)
			{
				CEntryTodo* todo = GetRainlendar()->GetCalendarWindow().GetEventManager().GetTodo(m_SelectedTodo);
				if (todo)
				{
					// Must create a new copy
					CEntryTodo* newTodo = new CEntryTodo(todo);
					newTodo->UpdateTimeStamp();
					newTodo->SetChecked(!todo->IsChecked());
					GetRainlendar()->GetCalendarWindow().GetEventManager().AddItem(newTodo, false);
					Redraw();		// No need to redraw the main window		
				}
				else
				{
					DebugLog("Warning: Todo item %s was not found!", CEntryItem::GetGUIDAsString(todo->GetGUID()));
				}
			}
			return TRUE;

		case ID_TODO_EDIT_ITEM:
			OpenTodoDialog(m_Window, CRainlendar::GetInstance(), m_SelectedTodo);
			return TRUE;

		case ID_MARKALLCHECKEDASDONE:
			{
				// Copy the list so that it can be sorted
				std::vector<CEntryTodo*> list = GetRainlendar()->GetCalendarWindow().GetEventManager().GetTodos();

				for (UINT i = 0; i < list.size(); i++)
				{
					if (list[i]->IsChecked() && !list[i]->IsReadOnly())
					{
						// Must create a new copy
						CEntryTodo* newTodo = new CEntryTodo(list[i]);
						newTodo->SetTodoType(TODO_TYPE_DONE);
						newTodo->UpdateTimeStamp();
						GetRainlendar()->GetCalendarWindow().GetEventManager().AddItem(newTodo, false);
					}
				}
				Redraw();		// No need to redraw the main window
			}
			return TRUE;

		case ID_MARKASDONE:
 			if (m_SelectedTodo != NULL)
			{
				CEntryTodo* todo = GetRainlendar()->GetCalendarWindow().GetEventManager().GetTodo(m_SelectedTodo);
				if (todo)
				{
					// Must create a new copy
					CEntryTodo* newTodo = new CEntryTodo(todo);
					newTodo->SetTodoType(TODO_TYPE_DONE);
					newTodo->UpdateTimeStamp();
					GetRainlendar()->GetCalendarWindow().GetEventManager().AddItem(newTodo, false);
					Redraw();		// No need to redraw the main window
				}
				else
				{
					DebugLog("Warning: Todo item %s was not found!", CEntryItem::GetGUIDAsString(todo->GetGUID()));
				}
			}
			return TRUE;
	}

	return GetRainlendar()->GetCalendarWindow().OnCommand(wParam, lParam);	// Rest is handled here
}

LRESULT CTodoWindow::OnLButtonDblClk(WPARAM wParam, LPARAM lParam) 
{
	POINT pos;
	pos.x = (SHORT)LOWORD(lParam);
	pos.y = (SHORT)HIWORD(lParam);

	if(m_Message == WM_NCLBUTTONDBLCLK)
	{
		// Transform the point to client rect
		MapWindowPoints(NULL, m_Window, &pos, 1);
	}

	// Check the todo-list
	GUID* guid = HitTest(pos.x, pos.y);

	switch(CConfig::Instance().GetTodoAction()) 
	{
	case 1:		// Check
		if (guid)
		{
			m_SelectedTodo = guid;
			OnCommand(ID_TODO_CHECKITEM, 0);
		}
		break;

	case 2:		// Edit
		OpenTodoDialog(m_Window, m_Instance, guid);
		break;

	case 3:		// Create
		OpenTodoDialog(m_Window, m_Instance, NULL);
		break;

	case 4:		// Delete
		if (guid)
		{
			m_SelectedTodo = guid;
			OnCommand(ID_TODO_DELETEITEM, 0);
		}
		break;

	case 5:		// Mark as done
		if (guid)
		{
			m_SelectedTodo = guid;
			OnCommand(ID_MARKASDONE, 0);
		}
		break;

	default:
		if (guid != NULL)
		{
			m_SelectedTodo = guid;

			HMENU menu = LoadMenu(CRainlendar::GetInstance(), MAKEINTRESOURCE(IDR_MENU1));
			menu = GetSubMenu(menu, 1);

			ModifyMenu(menu, ID_TODO_EDIT_ITEM, MF_BYCOMMAND | MF_STRING, ID_TODO_EDIT_ITEM, CCalendarWindow::c_Language.GetString("Menu", 49));	// Edit item...
			ModifyMenu(menu, ID_MARKASDONE, MF_BYCOMMAND | MF_STRING, ID_MARKASDONE, CCalendarWindow::c_Language.GetString("Menu", 30));	// Mark as done
			ModifyMenu(menu, ID_MARKALLCHECKEDASDONE, MF_BYCOMMAND | MF_STRING, ID_MARKALLCHECKEDASDONE, CCalendarWindow::c_Language.GetString("Menu", 50));	// Mark all checked as done
			ModifyMenu(menu, ID_TODO_CHECKITEM, MF_BYCOMMAND | MF_STRING, ID_TODO_CHECKITEM, CCalendarWindow::c_Language.GetString("Menu", 51));	// Mark all checked as done
			ModifyMenu(menu, ID_TODO_DELETEITEM, MF_BYCOMMAND | MF_STRING, ID_TODO_DELETEITEM, CCalendarWindow::c_Language.GetString("Menu", 52));	// Delete item

			MapWindowPoints(m_Window, NULL, &pos, 1);

			TrackPopupMenu(
			  menu,
			  TPM_RIGHTBUTTON | TPM_LEFTALIGN, 
			  pos.x,
			  pos.y,
			  0,
			  m_Window,
			  NULL
			);		
			DestroyMenu(menu);
		}
		else
		{
			OpenTodoDialog(m_Window, m_Instance, NULL);
		}
	}

	return 0;
}

LRESULT CTodoWindow::OnMove(WPARAM wParam, LPARAM lParam) 
{
	short int x = LOWORD(lParam);
	short int y = HIWORD(lParam);

	m_X = x;
	m_Y = y;

	if (CConfig::Instance().GetGrowUpwards() & (1 << (int)GetType()))
	{
		m_Y += m_Height;
	}

	if(CConfig::Instance().GetMovable())
	{
		if (CConfig::Instance().GetNegativeCoords()) 
		{
			if (m_X < 0) m_X = 0;
			if (m_Y < 0) m_Y = 0;
		}

		CConfig::Instance().SetTodoX(m_X);
		CConfig::Instance().SetTodoY(m_Y);
		CConfig::Instance().WriteConfig(CConfig::WRITE_TODOPOS);	// Store the new position to the ini file
	}

	if (GetRainlendar()->GetCalendarWindow().GetBackgroundMode() == MODE_COPY || m_TodoBackground.HasAlpha())
	{
		if (!CConfig::Instance().GetNativeTransparency())
		{
			m_TodoBackground.UpdateWallpaper(x, y);
			DrawWindow();
			InvalidateRect(m_Window, NULL, false);
		}
	}

	CRainWindow::OnMove(wParam, lParam);

	return 0;
}

LRESULT CTodoWindow::OnTimer(WPARAM wParam, LPARAM lParam) 
{
	if (CConfig::Instance().GetTodoEnable())
	{
		return CRainWindow::OnTimer(wParam, lParam);
	}
	return 0;
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

			case WM_LBUTTONDOWN: 
			case WM_NCLBUTTONDOWN: 
				return Window->OnLButtonDown(wParam, lParam);

			case WM_LBUTTONUP: 
			case WM_NCLBUTTONUP: 
				return Window->OnLButtonUp(wParam, lParam);

			case WM_DISPLAYCHANGE: 
				return Window->OnDisplayChange(wParam, lParam);

			case WM_NCHITTEST: 
				return Window->OnNcHitTest(wParam, lParam);

			case WM_DROPFILES: 
				return Window->OnDropFiles(wParam, lParam);
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



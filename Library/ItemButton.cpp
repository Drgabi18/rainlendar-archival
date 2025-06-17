/*
  Copyright (C) 2004 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Library/ItemButton.cpp,v 1.3 2005/10/14 17:05:41 rainy Exp $

  $Log: ItemButton.cpp,v $
  Revision 1.3  2005/10/14 17:05:41  rainy
  no message

  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.2  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "ItemButton.h"
#include "RasterizerBitmap.h"

#define FRAMES 3

enum BUTTON_STATE
{
	BUTTON_STATE_NORMAL,
	BUTTON_STATE_DOWN,
	BUTTON_STATE_HOVER
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemButton::CItemButton() : CItemDynamic()
{
	m_State = 0;
	m_Clicked = false;
}

CItemButton::~CItemButton()
{
}

void CItemButton::ReadSettings(const char* iniFile, const char* section)
{
	CItemDynamic::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	if (GetPrivateProfileString(section, "Command", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_Command = tmpSz;
	}

	if (GetPrivateProfileString(section, "BitmapName", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		CRasterizerBitmap* BMRast = new CRasterizerBitmap;
		BMRast->Load(tmpSz);
		BMRast->SetNumOfComponents(FRAMES);
		BMRast->SetAlign(CRasterizer::ALIGN_TOP | CRasterizer::ALIGN_LEFT);
		SetRasterizer(BMRast);
	}
}

bool CItemButton::MouseUp(POINT pos, CRainWindow* window)
{
	return HandleMouseEvent(pos, window, BUTTON_MOUSE_EVENT_UP);
}

bool CItemButton::MouseDown(POINT pos, CRainWindow* window)
{
	return HandleMouseEvent(pos, window, BUTTON_MOUSE_EVENT_DOWN);
}

bool CItemButton::MouseMove(POINT pos, CRainWindow* window)
{
	return HandleMouseEvent(pos, window, BUTTON_MOUSE_EVENT_MOVE);
}

bool CItemButton::HandleMouseEvent(POINT pos, CRainWindow* window, BUTTON_MOUSE_EVENT event)
{
	bool isInside = false;
	int divX = 1, divY = 1;
	int x = m_X;
	int y = m_Y;

	if ( (window->GetType() == RAINWINDOW_TYPE_CALENDAR) && (m_RepeatType == REPEAT_TYPE_ALL) )
	{
		divX = CConfig::Instance().GetHorizontalCount();
		divY = CConfig::Instance().GetVerticalCount();
		POINT offset = {0};
		offset.x = window->GetWidth() / divX;	// Width
		offset.y = window->GetHeight() / divY;	// Height
		if (offset.x != 0)
		{
			offset.x = (pos.x / offset.x) * offset.x;	// Offset X
		}
		if (offset.y != 0)
		{
			offset.y = (pos.y / offset.y) * offset.y;	// Offset Y
		}
		pos.x -=  offset.x;
		pos.y -=  offset.y;
	}
	else if ( (window->GetType() == RAINWINDOW_TYPE_CALENDAR) && (m_RepeatType == REPEAT_TYPE_CURRENT_MONTH) )
	{
		divX = CConfig::Instance().GetHorizontalCount();
		divY = CConfig::Instance().GetVerticalCount();
		POINT offset = ((CCalendarWindow *)window)->GetCurrentMonthOffset();
		if (offset.x == -1 && offset.y == -1) 
		{
			return false;		// Not visible
		}

		pos.x -=  offset.x;
		pos.y -=  offset.y;
	}
	else if ( (window->GetType() == RAINWINDOW_TYPE_CALENDAR) && (m_RepeatType == REPEAT_TYPE_VIEW_MONTH) )
	{
		divX = CConfig::Instance().GetHorizontalCount();
		divY = CConfig::Instance().GetVerticalCount();
		POINT offset = ((CCalendarWindow *)window)->GetViewMonthOffset();
		if (offset.x == -1 && offset.y == -1) 
		{
			return false;		// Not visible
		}

		pos.x -=  offset.x;
		pos.y -=  offset.y;
	}

	if (x < 0)
		x += window->GetWidth() / divX;

	if (y < 0)
		y += window->GetHeight() / divY;

	isInside = ( (pos.x >= x && pos.x <= x + GetRasterizer()->GetWidth() / FRAMES) &&
				(pos.y >= y && pos.y <= y + GetRasterizer()->GetHeight()) );

	if (event == BUTTON_MOUSE_EVENT_UP) 
	{
		if (m_State == BUTTON_STATE_DOWN)
		{
			if (m_Clicked && isInside)
			{
				window->ExecuteCommand(m_Command.c_str());
			}
			m_State = BUTTON_STATE_NORMAL;
			m_Clicked = false;
			return true;
		}
		m_Clicked = false;
	}
	else if (event == BUTTON_MOUSE_EVENT_DOWN) 
	{
		if (isInside)
		{
			m_State = BUTTON_STATE_DOWN;
			m_Clicked = true;
			return true;
		}
	}
	else if (event == BUTTON_MOUSE_EVENT_MOVE) 
	{
		if (m_Clicked == true)
		{
			if (isInside)
			{
				if (m_State == BUTTON_STATE_NORMAL)
				{
					m_State = BUTTON_STATE_DOWN;
					return true;
				}
			}
			else
			{
				if (m_State == BUTTON_STATE_DOWN)
				{
					m_State = BUTTON_STATE_NORMAL;
					return true;
				}
			}
		}
		else
		{
			if (isInside)
			{
				if (m_State == BUTTON_STATE_NORMAL)
				{
					m_State = BUTTON_STATE_HOVER;
					return true;
				}
			}
			else
			{
				if (m_State == BUTTON_STATE_HOVER)
				{
					m_State = BUTTON_STATE_NORMAL;
					return true;
				}
			}
		}
	}
	return false;
}

/* 
** Paint
**
** Paints the items
**
*/
void CItemButton::Paint(CImage& background, POINT offset)
{
	if (GetRasterizer())
	{
		int x = m_X;
		int y = m_Y;

		if ( (m_WinType == RAINWINDOW_TYPE_CALENDAR) && (m_RepeatType != REPEAT_TYPE_NO) )
		{
			if (x < 0)
			{
				x += background.GetWidth() / CConfig::Instance().GetHorizontalCount();
			}

			if (y < 0)
			{
				y += background.GetHeight() / CConfig::Instance().GetVerticalCount();
			}
		}
		else
		{
			if (x < 0)
			{
				x += background.GetWidth();
			}

			if (y < 0)
			{
				y += background.GetHeight();
			}
		}

		x += offset.x;
		y += offset.y;

		GetRasterizer()->Paint(background, x, y, 0, 0, m_State);
	}
}


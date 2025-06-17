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
  $Header: /home/cvsroot/Rainlendar/Library/ItemButton.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemButton.cpp,v $
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

CItemButton::CItemButton() : CItem()
{
	m_X = 0;
	m_Y = 0;
	m_WinType = RAINWINDOW_TYPE_CALENDAR;
	m_State = 0;
	m_Clicked = false;
}

CItemButton::~CItemButton()
{
}

void CItemButton::ReadSettings(const char* iniFile, const char* section)
{
	CItem::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	m_X = GetPrivateProfileInt( section, "X", 0, iniFile);
 	m_Y = GetPrivateProfileInt( section, "Y", 0, iniFile);

	m_WinType = (RAINWINDOW_TYPE)GetPrivateProfileInt(section, "Window", 0, iniFile);

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

	m_Enabled=(1==GetPrivateProfileInt( section, "Enable", 1, iniFile))?true:false;
}

void CItemButton::WriteSettings()
{
	char tmpSz[256];
	std::string INIPath = m_SettingsFile;

	// Only the enable/disable state need to be written since other settings cannot be edited
	sprintf(tmpSz, "%i", m_Enabled);
	WritePrivateProfileString( m_Section.c_str(), "Enable", tmpSz, INIPath.c_str() );
}

/* 
** Initialize
**
** Initializes the items 
**
*/
void CItemButton::Initialize()
{
	if (!GetRasterizer())
	{
		m_Enabled = false;
	}
}

bool CItemButton::MouseUp(POINT pos, CRainWindow* window)
{
	int x = m_X;
	int y = m_Y;

	if (x < 0)
	{
		x += window->GetWidth();
	}

	if (y < 0)
	{
		y += window->GetHeight();
	}

	if (m_State == BUTTON_STATE_DOWN)
	{
		if (m_Clicked &&
			pos.x >= x && pos.x <= x + GetRasterizer()->GetWidth() / FRAMES &&
			pos.y >= y && pos.y <= y + GetRasterizer()->GetHeight())
		{
			window->ExecuteCommand(m_Command.c_str());
		}
		m_State = BUTTON_STATE_NORMAL;
		m_Clicked = false;
		return true;
	}
	m_Clicked = false;

	return false;
}

bool CItemButton::MouseDown(POINT pos, CRainWindow* window)
{
	int x = m_X;
	int y = m_Y;

	if (x < 0)
	{
		x += window->GetWidth();
	}

	if (y < 0)
	{
		y += window->GetHeight();
	}

	if (pos.x >= x && pos.x <= x + GetRasterizer()->GetWidth() / FRAMES &&
		pos.y >= y && pos.y <= y + GetRasterizer()->GetHeight())
	{
		m_State = BUTTON_STATE_DOWN;
		m_Clicked = true;
		return true;
	}
	return false;
}

bool CItemButton::MouseMove(POINT pos, CRainWindow* window)
{
	int x = m_X;
	int y = m_Y;

	if (x < 0)
	{
		x += window->GetWidth();
	}

	if (y < 0)
	{
		y += window->GetHeight();
	}

	if (m_Clicked == true)
	{
		if (pos.x >= x && pos.x <= x + GetRasterizer()->GetWidth() / FRAMES &&
			pos.y >= y && pos.y <= y + GetRasterizer()->GetHeight())
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
		if (pos.x >= x && pos.x <= x + GetRasterizer()->GetWidth() / FRAMES &&
			pos.y >= y && pos.y <= y + GetRasterizer()->GetHeight())
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
	int x = m_X;
	int y = m_Y;

	if (x < 0)
	{
		x += background.GetWidth();
	}

	if (y < 0)
	{
		y += background.GetHeight();
	}

	GetRasterizer()->Paint(background, x, y, 0, 0, m_State);
}

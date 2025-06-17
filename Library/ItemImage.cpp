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
  $Header: /home/cvsroot/Rainlendar/Library/ItemImage.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemImage.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.1  2005/03/01 18:42:56  rainy
  Initial version

  Revision 1.2  2004/12/05 18:28:21  rainy
  Fixed a crash bug when rasterizer was not set.

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "ItemImage.h"
#include "RasterizerBitmap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemImage::CItemImage() : CItem()
{
	m_X = 0;
	m_Y = 0;
	m_WinType = RAINWINDOW_TYPE_CALENDAR;
}

CItemImage::~CItemImage()
{
}

void CItemImage::ReadSettings(const char* iniFile, const char* section)
{
	CItem::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	m_X = GetPrivateProfileInt( section, "X", 0, iniFile);
 	m_Y = GetPrivateProfileInt( section, "Y", 0, iniFile);

	m_WinType = (RAINWINDOW_TYPE)GetPrivateProfileInt(section, "Window", 0, iniFile);

	if (GetPrivateProfileString(section, "BitmapName", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_BitmapName = tmpSz;
	}
	m_Enabled=(1==GetPrivateProfileInt( section, "Enable", 1, iniFile))?true:false;
}

void CItemImage::WriteSettings()
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
void CItemImage::Initialize()
{
	if (!m_BitmapName.empty())
	{
		CRasterizerBitmap* BMRast;

		BMRast=new CRasterizerBitmap;
		if(BMRast==NULL) THROW(ERR_OUTOFMEM);

		BMRast->Load(m_BitmapName);
		BMRast->SetNumOfComponents(1);
		SetRasterizer(BMRast);
	}
	else
	{
		m_Enabled = false;
	}
}

/* 
** Paint
**
** Paints the items
**
*/
void CItemImage::Paint(CImage& background, POINT offset)
{
	if (GetRasterizer())
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

		m_Rasterizer->Paint(background, x, y, 0, 0, 0);
	}
}

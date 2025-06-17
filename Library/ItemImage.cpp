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
  $Header: /home/cvsroot/Rainlendar/Library/ItemImage.cpp,v 1.3 2005/10/14 17:05:29 rainy Exp $

  $Log: ItemImage.cpp,v $
  Revision 1.3  2005/10/14 17:05:29  rainy
  no message

  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

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

CItemImage::CItemImage() : CItemDynamic()
{
}

CItemImage::~CItemImage()
{
}

void CItemImage::ReadSettings(const char* iniFile, const char* section)
{
	CItemDynamic::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	if (GetPrivateProfileString(section, "BitmapName", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		CRasterizerBitmap* BMRast = new CRasterizerBitmap;
		BMRast->Load(tmpSz);
		BMRast->SetNumOfComponents(1);
		BMRast->SetAlign(CRasterizer::ALIGN_HCENTER | CRasterizer::ALIGN_VCENTER);
		SetRasterizer(BMRast);
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

		m_Rasterizer->Paint(background, x, y, 0, 0, 0);
	}
}

int CItemImage::GetX()
{
	if(m_Rasterizer) 
	{
		if((m_Rasterizer->GetAlign() & 0x0F) == CRasterizer::ALIGN_LEFT)
			return m_X;

		if((m_Rasterizer->GetAlign() & 0x0F) == CRasterizer::ALIGN_HCENTER)
			return m_X - GetW() / 2;

		if((m_Rasterizer->GetAlign() & 0x0F) == CRasterizer::ALIGN_RIGHT)
			return m_X - GetW();
	}

	return 0;
}

int CItemImage::GetY()
{
	if(m_Rasterizer) 
	{
		if((m_Rasterizer->GetAlign() & 0x0F0) == CRasterizer::ALIGN_TOP)
			return m_Y;

		if((m_Rasterizer->GetAlign() & 0x0F0) == CRasterizer::ALIGN_VCENTER)
			return m_Y - GetH() / 2;

		if((m_Rasterizer->GetAlign() & 0x0F0) == CRasterizer::ALIGN_BOTTOM)
			return m_Y - GetH();
	}

	return 0;
}

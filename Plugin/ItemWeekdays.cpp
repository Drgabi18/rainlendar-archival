/*
  Copyright (C) 2000 Kimmo Pekkola

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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/ItemWeekdays.cpp,v 1.8 2003/10/27 17:37:51 Rainy Exp $

  $Log: ItemWeekdays.cpp,v $
  Revision 1.8  2003/10/27 17:37:51  Rainy
  Config is now singleton.

  Revision 1.7  2003/06/15 09:49:13  Rainy
  Added support for multiple calendars.

  Revision 1.6  2002/11/12 18:11:34  rainy
  The interface of Paint changed a little.

  Revision 1.5  2002/08/24 11:10:36  rainy
  Changed the error handling.

  Revision 1.4  2002/08/03 16:15:29  rainy
  Added color setting for the rasterizer.

  Revision 1.3  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "ItemWeekdays.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"

#define NUMOFCOMPONENTS 7

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemWeekdays::CItemWeekdays()
{

}

CItemWeekdays::~CItemWeekdays()
{

}

/* 
** Initialize
**
** 
**
*/
void CItemWeekdays::Initialize()
{
	if( CConfig::Instance().GetWeekdaysEnable() && 
		CConfig::Instance().GetWeekdaysRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CConfig::Instance().GetWeekdaysRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(CConfig::Instance().GetWeekdaysBitmapName());
			BMRast->SetNumOfComponents(NUMOFCOMPONENTS);

			BMRast->SetAlign(CConfig::Instance().GetWeekdaysAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(CConfig::Instance().GetWeekdaysFont());
			FNRast->CreateStringTable(CConfig::Instance().GetWeekdayNames(), NUMOFCOMPONENTS);
			FNRast->SetAlign(CConfig::Instance().GetWeekdaysAlign());
			FNRast->SetColor(CConfig::Instance().GetWeekdaysFontColor());
			FNRast->UpdateDimensions();
			SetRasterizer(FNRast);
			break;
		}
	}
}

/* 
** Paint
**
** Paints the weekdays in correct place (over the days)
**
*/
void CItemWeekdays::Paint(CImage& background, POINT offset)
{
	int X, Y, W, H;
	int i;

	W = CConfig::Instance().GetDaysW() / 7;	// 7 Columns
	H = CConfig::Instance().GetDaysH() / 6;	// 6 Rows

	if(m_Rasterizer != NULL) 
	{
		Y = CConfig::Instance().GetDaysY() - H;
		Y += offset.y;

		for(i = 0; i < 7; i++) 
		{
			X = CConfig::Instance().GetDaysX() + i * W;
			X += offset.x;

			if(CConfig::Instance().GetStartFromMonday()) 
			{
				if(i == 6) 
				{
					m_Rasterizer->Paint(background, X, Y, W, H, 0);
				} 
				else 
				{
					m_Rasterizer->Paint(background, X, Y, W, H, i + 1);
				}
			}
			else 
			{
				m_Rasterizer->Paint(background, X, Y, W, H, i);
			}
		}
	}
}

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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/ItemWeekdays.cpp,v 1.7 2003/06/15 09:49:13 Rainy Exp $

  $Log: ItemWeekdays.cpp,v $
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
	if( CCalendarWindow::c_Config.GetWeekdaysEnable() && 
		CCalendarWindow::c_Config.GetWeekdaysRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CCalendarWindow::c_Config.GetWeekdaysRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(CCalendarWindow::c_Config.GetWeekdaysBitmapName());
			BMRast->SetNumOfComponents(NUMOFCOMPONENTS);

			BMRast->SetAlign(CCalendarWindow::c_Config.GetWeekdaysAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(CCalendarWindow::c_Config.GetWeekdaysFont());
			FNRast->CreateStringTable(CCalendarWindow::c_Config.GetWeekdayNames(), NUMOFCOMPONENTS);
			FNRast->SetAlign(CCalendarWindow::c_Config.GetWeekdaysAlign());
			FNRast->SetColor(CCalendarWindow::c_Config.GetWeekdaysFontColor());
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

	W = CCalendarWindow::c_Config.GetDaysW() / 7;	// 7 Columns
	H = CCalendarWindow::c_Config.GetDaysH() / 6;	// 6 Rows

	if(m_Rasterizer != NULL) 
	{
		Y = CCalendarWindow::c_Config.GetDaysY() - H;
		Y += offset.y;

		for(i = 0; i < 7; i++) 
		{
			X = CCalendarWindow::c_Config.GetDaysX() + i * W;
			X += offset.x;

			if(CCalendarWindow::c_Config.GetStartFromMonday()) 
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

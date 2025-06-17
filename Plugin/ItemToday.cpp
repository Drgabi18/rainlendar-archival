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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemToday.cpp,v 1.3 2002/05/23 17:33:41 rainy Exp $

  $Log: ItemToday.cpp,v $
  Revision 1.3  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "ItemToday.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemToday::CItemToday()
{

}

CItemToday::~CItemToday()
{

}

/* 
** Initialize
**
** 
**
*/
void CItemToday::Initialize()
{
	if( CCalendarWindow::c_Config.GetTodayEnable() && 
		CCalendarWindow::c_Config.GetTodayRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CCalendarWindow::c_Config.GetTodayRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) throw CError(CError::ERR_OUTOFMEM);

			BMRast->Load(CCalendarWindow::c_Config.GetTodayBitmapName());
			BMRast->SetNumOfComponents(CCalendarWindow::c_Config.GetTodayNumOfComponents());
			BMRast->SetSeparation(CCalendarWindow::c_Config.GetTodaySeparation());

			BMRast->SetAlign(CCalendarWindow::c_Config.GetTodayAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw CError(CError::ERR_OUTOFMEM);

			FNRast->SetFont(CCalendarWindow::c_Config.GetTodayFont());
			FNRast->SetAlign(CCalendarWindow::c_Config.GetTodayAlign());
			FNRast->UpdateDimensions("XX");
			SetRasterizer(FNRast);
			break;
		}
	}
}

/* 
** Paint
**
** Paints the numbers in correct place
**
*/
void CItemToday::Paint(HDC dc)
{
	int FirstWeekday;
	int X, Y, W, H, Index;

	// Only Paint if this month is shown
	if(CCalendarWindow::c_MonthsFirstDate.wMonth == CCalendarWindow::c_TodaysDate.wMonth &&
		CCalendarWindow::c_MonthsFirstDate.wYear == CCalendarWindow::c_TodaysDate.wYear) 
	{
		FirstWeekday = CCalendarWindow::c_MonthsFirstDate.wDayOfWeek;

		if(CCalendarWindow::c_Config.GetStartFromMonday()) 
		{
			FirstWeekday = (FirstWeekday - 1);
			if(FirstWeekday == -1) FirstWeekday = 6;
		} 

		W = CCalendarWindow::c_Config.GetDaysW() / 7;	// 7 Columns
		H = CCalendarWindow::c_Config.GetDaysH() / 6;	// 6 Rows

		SetBkMode(dc, TRANSPARENT);
		SetTextColor(dc, CCalendarWindow::c_Config.GetTodayFontColor());

		if(m_Rasterizer != NULL) 
		{
			Index = CCalendarWindow::c_TodaysDate.wDay + FirstWeekday - 1;
			X = CCalendarWindow::c_Config.GetDaysX() + (Index % 7) * W;
			Y = CCalendarWindow::c_Config.GetDaysY() + (Index / 7) * H;
		
			m_Rasterizer->Paint(dc, X, Y, W, H, CCalendarWindow::c_TodaysDate.wDay);
		}
	}
}

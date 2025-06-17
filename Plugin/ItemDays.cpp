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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/ItemDays.cpp,v 1.7 2003/06/15 09:49:12 Rainy Exp $

  $Log: ItemDays.cpp,v $
  Revision 1.7  2003/06/15 09:49:12  Rainy
  Added support for multiple calendars.

  Revision 1.6  2002/11/12 18:11:41  rainy
  The interface of Paint changed a little.

  Revision 1.5  2002/08/24 11:11:54  rainy
  Added ResetDayTypes()

  Revision 1.4  2002/08/03 16:16:37  rainy
  Added separation and color setting for the rasterizer.

  Revision 1.3  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "ItemDays.h"
#include "Error.h"
#include "RasterizerFont.h"
#include "RasterizerBitmap.h"
#include "CalendarWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemDays::CItemDays()
{
}

CItemDays::~CItemDays()
{

}

/* 
** Get?
**
** Size & Position
**
*/
int CItemDays::GetX()
{
	return CCalendarWindow::c_Config.GetDaysX();
}

int CItemDays::GetY()
{
	return CCalendarWindow::c_Config.GetDaysY();
}

int CItemDays::GetW()
{
	return CCalendarWindow::c_Config.GetDaysW();
}

int CItemDays::GetH()
{
	return CCalendarWindow::c_Config.GetDaysH();
}


/* 
** Initialize
**
** 
**
*/
void CItemDays::Initialize()
{
	if( CCalendarWindow::c_Config.GetDaysEnable() && 
		CCalendarWindow::c_Config.GetDaysRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CCalendarWindow::c_Config.GetDaysRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(CCalendarWindow::c_Config.GetDaysBitmapName());
			BMRast->SetNumOfComponents(CCalendarWindow::c_Config.GetDaysNumOfComponents());
			BMRast->SetSeparation(CCalendarWindow::c_Config.GetDaysSeparation());

			BMRast->SetAlign(CCalendarWindow::c_Config.GetDaysAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(CCalendarWindow::c_Config.GetDaysFont());
			FNRast->SetAlign(CCalendarWindow::c_Config.GetDaysAlign());
			FNRast->SetColor(CCalendarWindow::c_Config.GetDaysFontColor());
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
void CItemDays::Paint(CImage& background, POINT offset)
{
	int FirstWeekday;
	int X, Y, W, H, Index, DayType, NumOfDays, i;

	// Calculate the number of days in this month
	NumOfDays = GetDaysInMonth(CCalendarWindow::c_MonthsFirstDate.wYear, CCalendarWindow::c_MonthsFirstDate.wMonth);
	FirstWeekday = CCalendarWindow::c_MonthsFirstDate.wDayOfWeek;

	if(CCalendarWindow::c_Config.GetStartFromMonday()) 
	{
		FirstWeekday = (FirstWeekday - 1);
		if(FirstWeekday == -1) FirstWeekday = 6;
	} 

	W = CCalendarWindow::c_Config.GetDaysW() / 7;	// 7 Columns
	H = CCalendarWindow::c_Config.GetDaysH() / 6;	// 6 Rows

	if(m_Rasterizer != NULL) 
	{
		for(i = 0; i < NumOfDays; i++) 
		{
			Index = i + FirstWeekday;
			DayType = GetDayType(i + 1, CCalendarWindow::c_MonthsFirstDate.wMonth, CCalendarWindow::c_MonthsFirstDate.wYear);

			// Don't show today or events if selected
			if(!(CCalendarWindow::c_Config.GetDaysIgnoreToday() && (DayType&TODAY) ||
			     CCalendarWindow::c_Config.GetDaysIgnoreEvent() && (DayType&EVENT))) 
			{	
				X = CCalendarWindow::c_Config.GetDaysX() + (Index % 7) * W;
				Y = CCalendarWindow::c_Config.GetDaysY() + (Index / 7) * H;
	
				X += offset.x;
				Y += offset.y;

				m_Rasterizer->Paint(background, X, Y, W, H, i + 1);
			}
		}
	}
}

/* 
** HitTest
**
** Returns the day under the given coordinates
**
*/
int CItemDays::HitTest(int x, int y)
{
	int thisMonth = CCalendarWindow::c_MonthsFirstDate.wMonth;
	int thisYear = CCalendarWindow::c_MonthsFirstDate.wYear;

	int startMonth = thisMonth - CCalendarWindow::c_Config.GetPreviousMonths();
	int startYear = thisYear;

	while (startMonth <= 0)
	{
		startYear--;
		startMonth += 12;
	}

	POINT offset;

	int w = GetRainlendar()->GetCalendarWindow().GetWidth() / CCalendarWindow::c_Config.GetHorizontalCount();
	int h = GetRainlendar()->GetCalendarWindow().GetHeight() / CCalendarWindow::c_Config.GetVerticalCount();

	offset.x = (x / w) * w;
	offset.y = (y / h) * h;

	startMonth += (y / h) * CCalendarWindow::c_Config.GetHorizontalCount() + (x / w);
	while (startMonth > 12)
	{
		startYear++;
		startMonth -= 12;
	}

	CCalendarWindow::ChangeMonth(startMonth, startYear);
	int NumOfDays = GetDaysInMonth(startYear, startMonth);

	x -= offset.x;
	y -= offset.y;

	int FirstWeekday;
	int X, Y, W, H, Day;

	W = CCalendarWindow::c_Config.GetDaysW() / 7;	// 7 Columns
	H = CCalendarWindow::c_Config.GetDaysH() / 6;	// 6 Rows
	X = CCalendarWindow::c_Config.GetDaysX();
	Y = CCalendarWindow::c_Config.GetDaysY();

	FirstWeekday = CCalendarWindow::c_MonthsFirstDate.wDayOfWeek;

	// Reset the values
	CCalendarWindow::ChangeMonth(thisMonth, thisYear);

	if(CCalendarWindow::c_Config.GetStartFromMonday()) 
	{
		FirstWeekday = (FirstWeekday - 1);
		if(FirstWeekday == -1) FirstWeekday = 6;
	} 

	x -= X;
	y -= Y;

	if(x < 0 || x > (W * 7) || y < 0 || y > (H * 6)) return 0;	// No Hit

	Day = (y / H) * 7 + (x / W);
	Day = (Day + 1) - FirstWeekday;

	if(NumOfDays == 0) 
	{
		if(Day < 1 || Day > 31) return 0;	// No hit
	}
	else 
	{
		if(Day < 1 || Day > NumOfDays) return 0;	// No hit
	}

	return CEventMessage::DateToValue(Day, startMonth, startYear);
}

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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/ItemDays.cpp,v 1.8 2003/10/27 17:37:51 Rainy Exp $

  $Log: ItemDays.cpp,v $
  Revision 1.8  2003/10/27 17:37:51  Rainy
  Config is now singleton.

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
	return CConfig::Instance().GetDaysX();
}

int CItemDays::GetY()
{
	return CConfig::Instance().GetDaysY();
}

int CItemDays::GetW()
{
	return CConfig::Instance().GetDaysW();
}

int CItemDays::GetH()
{
	return CConfig::Instance().GetDaysH();
}


/* 
** Initialize
**
** 
**
*/
void CItemDays::Initialize()
{
	if( CConfig::Instance().GetDaysEnable() && 
		CConfig::Instance().GetDaysRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CConfig::Instance().GetDaysRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(CConfig::Instance().GetDaysBitmapName());
			BMRast->SetNumOfComponents(CConfig::Instance().GetDaysNumOfComponents());
			BMRast->SetSeparation(CConfig::Instance().GetDaysSeparation());

			BMRast->SetAlign(CConfig::Instance().GetDaysAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(CConfig::Instance().GetDaysFont());
			FNRast->SetAlign(CConfig::Instance().GetDaysAlign());
			FNRast->SetColor(CConfig::Instance().GetDaysFontColor());
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

	if(CConfig::Instance().GetStartFromMonday()) 
	{
		FirstWeekday = (FirstWeekday - 1);
		if(FirstWeekday == -1) FirstWeekday = 6;
	} 

	W = CConfig::Instance().GetDaysW() / 7;	// 7 Columns
	H = CConfig::Instance().GetDaysH() / 6;	// 6 Rows

	if(m_Rasterizer != NULL) 
	{
		for(i = 0; i < NumOfDays; i++) 
		{
			Index = i + FirstWeekday;
			DayType = GetDayType(i + 1, CCalendarWindow::c_MonthsFirstDate.wMonth, CCalendarWindow::c_MonthsFirstDate.wYear);

			// Don't show today or events if selected
			if(!(CConfig::Instance().GetDaysIgnoreToday() && (DayType&TODAY) ||
			     CConfig::Instance().GetDaysIgnoreEvent() && (DayType&EVENT))) 
			{	
				X = CConfig::Instance().GetDaysX() + (Index % 7) * W;
				Y = CConfig::Instance().GetDaysY() + (Index / 7) * H;
	
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

	int startMonth = thisMonth - CConfig::Instance().GetPreviousMonths();
	int startYear = thisYear;

	while (startMonth <= 0)
	{
		startYear--;
		startMonth += 12;
	}

	POINT offset;

	int w = GetRainlendar()->GetCalendarWindow().GetWidth() / CConfig::Instance().GetHorizontalCount();
	int h = GetRainlendar()->GetCalendarWindow().GetHeight() / CConfig::Instance().GetVerticalCount();

	offset.x = (x / w) * w;
	offset.y = (y / h) * h;

	startMonth += (y / h) * CConfig::Instance().GetHorizontalCount() + (x / w);
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

	W = CConfig::Instance().GetDaysW() / 7;	// 7 Columns
	H = CConfig::Instance().GetDaysH() / 6;	// 6 Rows
	X = CConfig::Instance().GetDaysX();
	Y = CConfig::Instance().GetDaysY();

	FirstWeekday = CCalendarWindow::c_MonthsFirstDate.wDayOfWeek;

	// Reset the values
	CCalendarWindow::ChangeMonth(thisMonth, thisYear);

	if(CConfig::Instance().GetStartFromMonday()) 
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

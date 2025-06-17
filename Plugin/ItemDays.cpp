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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemDays.cpp,v 1.2 2001/12/23 10:00:17 rainy Exp $

  $Log: ItemDays.cpp,v $
  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "ItemDays.h"
#include "Error.h"
#include "RasterizerFont.h"
#include "RasterizerBitmap.h"
#include "CalendarWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemDays::CItemDays()
{
	m_DaysInMonth=0;
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
			if(BMRast==NULL) throw ERR_OUTOFMEM;

			BMRast->Load(CCalendarWindow::c_Config.GetDaysBitmapName());
			BMRast->SetNumOfComponents(CCalendarWindow::c_Config.GetDaysNumOfComponents());

			BMRast->SetAlign(CCalendarWindow::c_Config.GetDaysAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw ERR_OUTOFMEM;

			FNRast->SetFont(CCalendarWindow::c_Config.GetDaysFont());
			FNRast->SetAlign(CCalendarWindow::c_Config.GetDaysAlign());
			FNRast->UpdateDimensions("XX");
			SetRasterizer(FNRast);
			break;
		}
	
		// Set all days to NORMAL
		for(int i=0; i<32; i++) {
			c_DayTypes[i]=NORMAL;
		}

		// If this month is shown, mark today
		if(CCalendarWindow::c_CurrentDate.GetMonth()==CCalendarWindow::c_TodaysDate.GetMonth() &&
			CCalendarWindow::c_CurrentDate.GetYear()==CCalendarWindow::c_TodaysDate.GetYear()) {
	
			SetDayType(CCalendarWindow::c_TodaysDate.GetDay(), TODAY);
		}
	}
}

/* 
** Paint
**
** Paints the numbers in correct place
**
*/
void CItemDays::Paint(CDC& dc)
{
	int FirstWeekday;
	int X, Y, W, H, Index, DayType, NumOfDays, i;

	// Calculate the number of days in this month
	CTime ThisMonth=CCalendarWindow::c_CurrentDate;
	CTime NextMonth((ThisMonth.GetMonth()==12)?(ThisMonth.GetYear()+1):ThisMonth.GetYear(),
		(ThisMonth.GetMonth()==12)?1:ThisMonth.GetMonth()+1, 1, 0, 0, 0);
	CTimeSpan MonthSpan=NextMonth-ThisMonth;
	NumOfDays=(MonthSpan.GetTotalMinutes()+60)/(24*60);		// Add a hour for possible daylight saving

	FirstWeekday=ThisMonth.GetDayOfWeek();

	if(CCalendarWindow::c_Config.GetStartFromMonday()) {
		FirstWeekday=(FirstWeekday-1);
		if(FirstWeekday==0) FirstWeekday=7;
	} 

	W=CCalendarWindow::c_Config.GetDaysW()/7;	// 7 Columns
	H=CCalendarWindow::c_Config.GetDaysH()/6;	// 6 Rows

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(CCalendarWindow::c_Config.GetDaysFontColor());

	if(m_Rasterizer!=NULL) {
		for(i=0; i<NumOfDays; i++) {
			Index=i+FirstWeekday-1;
			DayType=GetDayType(i+1);

			// Don't show today or events if selected
			if(!(CCalendarWindow::c_Config.GetDaysIgnoreToday() && (DayType&TODAY) ||
			     CCalendarWindow::c_Config.GetDaysIgnoreEvent() && (DayType&EVENT))) {	

				X=CCalendarWindow::c_Config.GetDaysX()+(Index%7)*W;
				Y=CCalendarWindow::c_Config.GetDaysY()+(Index/7)*H;
	
				m_Rasterizer->Paint(dc, X, Y, W, H, i+1);
			}
		}
	}

	m_DaysInMonth=NumOfDays;
}

/* 
** HitTest
**
** Returns the day under the given coordinates
**
*/
int CItemDays::HitTest(int x, int y)
{
	int FirstWeekday;
	CTime MonthsFirst=CCalendarWindow::c_CurrentDate;
	int X, Y, W, H, Day;

	W=CCalendarWindow::c_Config.GetDaysW()/7;	// 7 Columns
	H=CCalendarWindow::c_Config.GetDaysH()/6;	// 6 Rows
	X=CCalendarWindow::c_Config.GetDaysX();
	Y=CCalendarWindow::c_Config.GetDaysY();

	FirstWeekday=MonthsFirst.GetDayOfWeek();

	if(CCalendarWindow::c_Config.GetStartFromMonday()) {
		FirstWeekday=(FirstWeekday-1);
		if(FirstWeekday==0) FirstWeekday=7;
	} 

	x-=X;
	y-=Y;

	if(x<0 || x>(W*7) || y<0 || y>(H*6)) return 0;	// No Hit

	Day=(y/H)*7+(x/W);
	Day=(Day+1)-(FirstWeekday-1);

	if(m_DaysInMonth==0) {
		if(Day<1 || Day>31) return 0;	// No hit
	} else {
		if(Day<1 || Day>m_DaysInMonth) return 0;	// No hit
	}

	return Day;
}

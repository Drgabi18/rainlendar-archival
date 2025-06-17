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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemToday.cpp,v 1.2 2001/12/23 10:00:17 rainy Exp $

  $Log: ItemToday.cpp,v $
  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "ItemToday.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
			if(BMRast==NULL) throw ERR_OUTOFMEM;

			BMRast->Load(CCalendarWindow::c_Config.GetTodayBitmapName());
			BMRast->SetNumOfComponents(CCalendarWindow::c_Config.GetTodayNumOfComponents());

			BMRast->SetAlign(CCalendarWindow::c_Config.GetTodayAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw ERR_OUTOFMEM;

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
void CItemToday::Paint(CDC& dc)
{
	int FirstWeekday, NumOfDays;
	int X, Y, W, H, Index;

	// Calculate the number of days until today
	CTime ThisMonth=CCalendarWindow::c_CurrentDate;
	CTime Today(ThisMonth.GetYear(), ThisMonth.GetMonth(), CCalendarWindow::c_TodaysDate.GetDay(), 0, 0, 0);
	CTimeSpan TodaySpan=Today-ThisMonth;
	NumOfDays=(TodaySpan.GetTotalMinutes()+60)/(24*60);		// Add a hour for possible daylight saving

	// Only Paint if this month is shown
	if(CCalendarWindow::c_CurrentDate.GetMonth()==CCalendarWindow::c_TodaysDate.GetMonth() &&
		CCalendarWindow::c_CurrentDate.GetYear()==CCalendarWindow::c_TodaysDate.GetYear()) {

		FirstWeekday=ThisMonth.GetDayOfWeek();

		if(CCalendarWindow::c_Config.GetStartFromMonday()) {
			FirstWeekday=(FirstWeekday-1);
			if(FirstWeekday==0) FirstWeekday=7;
		}

		W=CCalendarWindow::c_Config.GetDaysW()/7;	// 7 Columns
		H=CCalendarWindow::c_Config.GetDaysH()/6;	// 6 Rows

		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(CCalendarWindow::c_Config.GetTodayFontColor());

		if(m_Rasterizer!=NULL) {
			Index=NumOfDays+FirstWeekday-1;
			X=CCalendarWindow::c_Config.GetDaysX()+(Index%7)*W;
			Y=CCalendarWindow::c_Config.GetDaysY()+(Index/7)*H;
		
			m_Rasterizer->Paint(dc, X, Y, W, H, NumOfDays+1);
		}
	}
}

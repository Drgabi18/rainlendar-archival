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

// ItemToday.cpp: implementation of the CItemToday class.
//
//////////////////////////////////////////////////////////////////////

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
	if( CCalendarWindow::C_Config.GetTodayEnable() && 
		CCalendarWindow::C_Config.GetTodayRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CCalendarWindow::C_Config.GetTodayRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) throw ERR_OUTOFMEM;

			BMRast->Load(CCalendarWindow::C_Config.GetTodayBitmapName());
			BMRast->SetNumOfComponents(CCalendarWindow::C_Config.GetTodayNumOfComponents());

			BMRast->SetAlign(CCalendarWindow::C_Config.GetTodayAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw ERR_OUTOFMEM;

			FNRast->SetFont(CCalendarWindow::C_Config.GetTodayFont());
			FNRast->SetAlign(CCalendarWindow::C_Config.GetTodayAlign());
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
	CTime ThisMonth=CCalendarWindow::C_CurrentDate;
	CTime Today(ThisMonth.GetYear(), ThisMonth.GetMonth(), CCalendarWindow::C_TodaysDate.GetDay(), 0, 0, 0);
	CTimeSpan TodaySpan=Today-ThisMonth;
	NumOfDays=(TodaySpan.GetTotalMinutes()+60)/(24*60);		// Add a hour for possible daylight saving

	// Only Paint if this month is shown
	if(CCalendarWindow::C_CurrentDate.GetMonth()==CCalendarWindow::C_TodaysDate.GetMonth() &&
		CCalendarWindow::C_CurrentDate.GetYear()==CCalendarWindow::C_TodaysDate.GetYear()) {

		FirstWeekday=ThisMonth.GetDayOfWeek();

		if(CCalendarWindow::C_Config.GetStartFromMonday()) {
			FirstWeekday=(FirstWeekday-1);
			if(FirstWeekday==0) FirstWeekday=7;
		}

		W=CCalendarWindow::C_Config.GetDaysW()/7;	// 7 Columns
		H=CCalendarWindow::C_Config.GetDaysH()/6;	// 6 Rows

		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(CCalendarWindow::C_Config.GetTodayFontColor());

		if(m_Rasterizer!=NULL) {
			Index=NumOfDays+FirstWeekday-1;
			X=CCalendarWindow::C_Config.GetDaysX()+(Index%7)*W;
			Y=CCalendarWindow::C_Config.GetDaysY()+(Index/7)*H;
		
			m_Rasterizer->Paint(dc, X, Y, W, H, NumOfDays+1);
		}
	}
}

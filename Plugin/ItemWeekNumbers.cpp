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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemWeekNumbers.cpp,v 1.1 2002/01/10 16:40:37 rainy Exp $

  $Log: ItemWeekNumbers.cpp,v $
  Revision 1.1  2002/01/10 16:40:37  rainy
  Initial version

*/

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"
#include "ItemWeekNumbers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemWeekNumbers::CItemWeekNumbers()
{

}

CItemWeekNumbers::~CItemWeekNumbers()
{

}

/* 
** Initialize
**
** 
**
*/
void CItemWeekNumbers::Initialize()
{
	if( CCalendarWindow::c_Config.GetWeekNumbersEnable() && 
		CCalendarWindow::c_Config.GetWeekNumbersRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CCalendarWindow::c_Config.GetWeekNumbersRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) throw ERR_OUTOFMEM;

			BMRast->Load(CCalendarWindow::c_Config.GetWeekNumbersBitmapName());
			BMRast->SetNumOfComponents(CCalendarWindow::c_Config.GetWeekNumbersNumOfComponents());

			BMRast->SetAlign(CCalendarWindow::c_Config.GetWeekNumbersAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw ERR_OUTOFMEM;

			FNRast->SetFont(CCalendarWindow::c_Config.GetWeekNumbersFont());
			FNRast->SetAlign(CCalendarWindow::c_Config.GetWeekNumbersAlign());
			FNRast->UpdateDimensions();
			SetRasterizer(FNRast);
			break;
		}
	}
}

// Ripped from http://kukkalautanen.it.jyu.fi/ajk/faq/sao-faq.html/ch4.html
// This follows ISO 8601 -standard
int julian(int d, int m, int y)
{
	int n1, n2;
	n1 = 12*y+m-3;
	n2 = n1/12;
	return (734*n1+15)/24-2*n2+n2/4-n2/100+n2/400+d+1721119;
}

int weekno(int d, int m, int y)
{
	int n1, n2, w;
	n1 = julian(d, m, y);
	n2 = 7*(n1/7)+10;
	y = y+1;
	while ((w = (n2-julian(1,1,y))/7) <= 0) {
		y = y-1;
	}
	return w;
}

/* 
** Paint
**
** Paints the week numbers in correct place (next to the days)
**
*/
void CItemWeekNumbers::Paint(CDC& dc)
{
	int FirstWeekday, NumOfDays;
	int X, Y, W, H;
	int i, base, lines;

	W = CCalendarWindow::c_Config.GetDaysW() / 7;	// 7 Columns
	H = CCalendarWindow::c_Config.GetDaysH() / 6;	// 6 Rows

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(CCalendarWindow::c_Config.GetWeekNumbersFontColor());

	// Calculate if the last line (=6) contains days or not
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

	if(FirstWeekday + NumOfDays > 7 * 5 + 1)
	{
		lines = 6;
	}
	else
	{
		lines = 5;
	}

	if(m_Rasterizer!=NULL) 
	{
		X = CCalendarWindow::c_Config.GetDaysX() - W;

		for(i = 0; i < lines; i++) 
		{
			Y = CCalendarWindow::c_Config.GetDaysY() + i * H;
			base = weekno(i * 7 + 1, ThisMonth.GetMonth(), ThisMonth.GetYear());
			m_Rasterizer->Paint(dc, X, Y, W, H, base);
		}
	}
}


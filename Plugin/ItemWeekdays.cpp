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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemWeekdays.cpp,v 1.2 2001/12/23 10:00:17 rainy Exp $

  $Log: ItemWeekdays.cpp,v $
  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "ItemWeekdays.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
			if(BMRast==NULL) throw ERR_OUTOFMEM;

			BMRast->Load(CCalendarWindow::c_Config.GetWeekdaysBitmapName());
			BMRast->SetNumOfComponents(NUMOFCOMPONENTS);

			BMRast->SetAlign(CCalendarWindow::c_Config.GetWeekdaysAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw ERR_OUTOFMEM;

			FNRast->SetFont(CCalendarWindow::c_Config.GetWeekdaysFont());
			FNRast->CreateStringTable(CCalendarWindow::c_Config.GetWeekdayNames(), NUMOFCOMPONENTS);
			FNRast->SetAlign(CCalendarWindow::c_Config.GetWeekdaysAlign());
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
void CItemWeekdays::Paint(CDC& dc)
{
	int X, Y, W, H;
	int i;

	W=CCalendarWindow::c_Config.GetDaysW()/7;	// 7 Columns
	H=CCalendarWindow::c_Config.GetDaysH()/6;	// 6 Rows

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(CCalendarWindow::c_Config.GetWeekdaysFontColor());

	if(m_Rasterizer!=NULL) {
		Y=CCalendarWindow::c_Config.GetDaysY()-H;

		for(i=0; i<7; i++) {
			X=CCalendarWindow::c_Config.GetDaysX()+i*W;
	
			if(CCalendarWindow::c_Config.GetStartFromMonday()) {
				if(i==6) {
					m_Rasterizer->Paint(dc, X, Y, W, H, 0);
				} else {
					m_Rasterizer->Paint(dc, X, Y, W, H, i+1);
				}
			} else {
				m_Rasterizer->Paint(dc, X, Y, W, H, i);
			}
		}
	}
}

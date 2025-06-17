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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/ItemToday.cpp,v 1.9 2003/10/27 17:37:51 Rainy Exp $

  $Log: ItemToday.cpp,v $
  Revision 1.9  2003/10/27 17:37:51  Rainy
  Config is now singleton.

  Revision 1.8  2003/06/15 09:49:12  Rainy
  Added support for multiple calendars.

  Revision 1.7  2003/03/22 20:30:25  rainy
  Overwrote dimension getters.

  Revision 1.6  2002/11/12 18:11:34  rainy
  The interface of Paint changed a little.

  Revision 1.5  2002/08/24 11:10:36  rainy
  Changed the error handling.

  Revision 1.4  2002/08/03 16:15:17  rainy
  Added separation and color setting for the rasterizer.

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
	if( CConfig::Instance().GetTodayEnable() && 
		CConfig::Instance().GetTodayRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CConfig::Instance().GetTodayRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(CConfig::Instance().GetTodayBitmapName());
			BMRast->SetNumOfComponents(CConfig::Instance().GetTodayNumOfComponents());
			BMRast->SetSeparation(CConfig::Instance().GetTodaySeparation());
			BMRast->SetAlign(CConfig::Instance().GetTodayAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(CConfig::Instance().GetTodayFont());
			FNRast->SetAlign(CConfig::Instance().GetTodayAlign());
			FNRast->SetColor(CConfig::Instance().GetTodayFontColor());
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
void CItemToday::Paint(CImage& background, POINT offset)
{
	int FirstWeekday;
	int X, Y, W, H, Index;

	// Only Paint if this month is shown
	if(CCalendarWindow::c_MonthsFirstDate.wMonth == CCalendarWindow::c_TodaysDate.wMonth &&
		CCalendarWindow::c_MonthsFirstDate.wYear == CCalendarWindow::c_TodaysDate.wYear) 
	{
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
			Index = CCalendarWindow::c_TodaysDate.wDay + FirstWeekday - 1;
			X = CConfig::Instance().GetDaysX() + (Index % 7) * W;
			Y = CConfig::Instance().GetDaysY() + (Index / 7) * H;
		
			X += offset.x;
			Y += offset.y;

			m_Rasterizer->Paint(background, X, Y, W, H, CCalendarWindow::c_TodaysDate.wDay);
		}
	}
}

// Put the today to the lowright corner, so we know if the window must be 
// enlarged.

int CItemToday::GetX()
{
	int W = CConfig::Instance().GetDaysW() / 7;	// 7 Columns
	int X = CConfig::Instance().GetDaysX();

	return W * 6 + X;
}

int CItemToday::GetY()
{
	int H = CConfig::Instance().GetDaysH() / 6;	// 6 Rows
	int Y = CConfig::Instance().GetDaysY();

	return H * 5 + Y;
}

int CItemToday::GetW()
{
	int numOfComponents = CConfig::Instance().GetTodayNumOfComponents();

	if (m_Rasterizer->GetWidth() > m_Rasterizer->GetHeight())
	{
		return m_Rasterizer->GetWidth() / numOfComponents;
	}

	return m_Rasterizer->GetWidth();
}

int CItemToday::GetH()
{
	int numOfComponents = CConfig::Instance().GetTodayNumOfComponents();

	if (m_Rasterizer->GetHeight() > m_Rasterizer->GetWidth())
	{
		return m_Rasterizer->GetHeight() / numOfComponents;
	}

	return m_Rasterizer->GetHeight();
}

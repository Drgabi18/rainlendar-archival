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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/ItemMonth.cpp,v 1.9 2003/06/15 09:49:12 Rainy Exp $

  $Log: ItemMonth.cpp,v $
  Revision 1.9  2003/06/15 09:49:12  Rainy
  Added support for multiple calendars.

  Revision 1.8  2002/11/12 18:11:35  rainy
  The interface of Paint changed a little.

  Revision 1.7  2002/08/24 11:10:36  rainy
  Changed the error handling.

  Revision 1.6  2002/08/03 16:15:18  rainy
  Added separation and color setting for the rasterizer.

  Revision 1.5  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.4  2002/02/27 18:52:27  rainy
  Added new alignments

  Revision 1.3  2002/01/10 16:45:33  rainy
  no message

  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "ItemMonth.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"

// This is always 12 (of course)
#define NUMOFCOMPONENTS 12

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemMonth::CItemMonth()
{

}

CItemMonth::~CItemMonth()
{

}

/* 
** Get?
**
** Size & Position
**
*/
int CItemMonth::GetX()
{
	if(m_Rasterizer) 
	{
		if(m_Rasterizer->GetAlign() & 0x0F == CRasterizer::ALIGN_LEFT)
			return CCalendarWindow::c_Config.GetMonthX();

		if(m_Rasterizer->GetAlign() & 0x0F == CRasterizer::ALIGN_HCENTER)
			return CCalendarWindow::c_Config.GetMonthX() - GetW() / 2;

		if(m_Rasterizer->GetAlign() & 0x0F == CRasterizer::ALIGN_RIGHT)
			return CCalendarWindow::c_Config.GetMonthX() - GetW();
	}

	return 0;
}

int CItemMonth::GetY()
{
	if(m_Rasterizer) 
	{
		if(m_Rasterizer->GetAlign() & 0x0F0 == CRasterizer::ALIGN_TOP)
			return CCalendarWindow::c_Config.GetMonthY();

		if(m_Rasterizer->GetAlign() & 0x0F0 == CRasterizer::ALIGN_VCENTER)
			return CCalendarWindow::c_Config.GetMonthY() - GetH() / 2;

		if(m_Rasterizer->GetAlign() & 0x0F0 == CRasterizer::ALIGN_BOTTOM)
			return CCalendarWindow::c_Config.GetMonthY() - GetH();
	}

	return 0;
}

int CItemMonth::GetW()
{
	if(m_Rasterizer) 
	{
		if(m_Rasterizer->GetHeight() > m_Rasterizer->GetWidth()) 
		{
			return m_Rasterizer->GetWidth();
		} 
		else 
		{
			return m_Rasterizer->GetWidth() / NUMOFCOMPONENTS;
		}
	}

	return 0;
}

int CItemMonth::GetH()
{
	if(m_Rasterizer) 
	{
		if(m_Rasterizer->GetHeight() > m_Rasterizer->GetWidth()) 
		{
			return m_Rasterizer->GetHeight() / NUMOFCOMPONENTS;
		} 
		else 
		{
			return m_Rasterizer->GetHeight();
		}
	}

	return 0;
}

/* 
** Initialize
**
** Initializes the rasterizer 
**
*/
void CItemMonth::Initialize()
{
	if( CCalendarWindow::c_Config.GetMonthEnable() && 
		CCalendarWindow::c_Config.GetMonthRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CCalendarWindow::c_Config.GetMonthRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(CCalendarWindow::c_Config.GetMonthBitmapName());
			BMRast->SetNumOfComponents(NUMOFCOMPONENTS);
			BMRast->SetAlign(CCalendarWindow::c_Config.GetMonthAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(CCalendarWindow::c_Config.GetMonthFont());
			FNRast->CreateStringTable(CCalendarWindow::c_Config.GetMonthNames(), NUMOFCOMPONENTS);
			FNRast->SetAlign(CCalendarWindow::c_Config.GetMonthAlign());
			FNRast->SetColor(CCalendarWindow::c_Config.GetMonthFontColor());
			FNRast->UpdateDimensions();
			SetRasterizer(FNRast);
			break;
		}
	}
}

/* 
** Paint
**
** Paints the Month in correct place
**
*/
void CItemMonth::Paint(CImage& background, POINT offset)
{
	int X, Y, W, H;

	if(m_Rasterizer!=NULL) 
	{
		W = GetW();
		H = GetH();

		switch (m_Rasterizer->GetAlign() & 0x0F)
		{
		case CRasterizer::ALIGN_LEFT:
			X = CCalendarWindow::c_Config.GetMonthX();
			break;

		case CRasterizer::ALIGN_HCENTER:
			X = CCalendarWindow::c_Config.GetMonthX() - W / 2;
			break;

		case CRasterizer::ALIGN_RIGHT:
			X = CCalendarWindow::c_Config.GetMonthX() - W;
			break;
		};

		switch (m_Rasterizer->GetAlign() & 0xF0)
		{
		case CRasterizer::ALIGN_TOP:
				Y = CCalendarWindow::c_Config.GetMonthY();
				break;

		case CRasterizer::ALIGN_VCENTER:
			Y = CCalendarWindow::c_Config.GetMonthY() - H / 2;
			break;

		case CRasterizer::ALIGN_BOTTOM:
			Y = CCalendarWindow::c_Config.GetMonthY() - H;
			break;
		};

		X += offset.x;
		Y += offset.y;

		m_Rasterizer->Paint(background, X, Y, W, H, CCalendarWindow::c_MonthsFirstDate.wMonth - 1);
	}
}

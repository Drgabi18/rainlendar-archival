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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemYear.cpp,v 1.5 2002/05/23 17:33:40 rainy Exp $

  $Log: ItemYear.cpp,v $
  Revision 1.5  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.4  2002/02/27 18:52:12  rainy
  Added new alignments

  Revision 1.3  2002/01/10 16:42:02  rainy
  Fixed the height calculation

  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "ItemYear.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"

// Year must be defined with 0...9 -bitmap
#define NUMOFCOMPONENTS 10
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemYear::CItemYear()
{

}

CItemYear::~CItemYear()
{

}

/* 
** Get?
**
** Size & Position
**
*/
int CItemYear::GetX()
{
	if(m_Rasterizer) 
	{
		if(m_Rasterizer->GetAlign() & 0x0F == CRasterizer::ALIGN_LEFT)
			return CCalendarWindow::c_Config.GetYearX();

		if(m_Rasterizer->GetAlign() & 0x0F == CRasterizer::ALIGN_HCENTER)
			return CCalendarWindow::c_Config.GetYearX() - GetW() / 2;

		if(m_Rasterizer->GetAlign() & 0x0F == CRasterizer::ALIGN_RIGHT)
			return CCalendarWindow::c_Config.GetYearX() - GetW();
	}

	return 0;
}

int CItemYear::GetY()
{
	if(m_Rasterizer) 
	{
		if(m_Rasterizer->GetAlign() & 0x0F0 == CRasterizer::ALIGN_TOP)
			return CCalendarWindow::c_Config.GetYearY();

		if(m_Rasterizer->GetAlign() & 0x0F0 == CRasterizer::ALIGN_VCENTER)
			return CCalendarWindow::c_Config.GetYearY() - GetH() / 2;

		if(m_Rasterizer->GetAlign() & 0x0F0 == CRasterizer::ALIGN_BOTTOM)
			return CCalendarWindow::c_Config.GetYearY() - GetH();
	}

	return 0;
}

int CItemYear::GetW()
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

int CItemYear::GetH()
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
void CItemYear::Initialize()
{
	if( CCalendarWindow::c_Config.GetYearEnable() && 
		CCalendarWindow::c_Config.GetYearRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CCalendarWindow::c_Config.GetYearRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) throw CError(CError::ERR_OUTOFMEM);

			BMRast->Load(CCalendarWindow::c_Config.GetYearBitmapName());
			BMRast->SetNumOfComponents(NUMOFCOMPONENTS);
			BMRast->SetSeparation(CCalendarWindow::c_Config.GetYearSeparation());

			BMRast->SetAlign(CCalendarWindow::c_Config.GetYearAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw CError(CError::ERR_OUTOFMEM);

			FNRast->SetFont(CCalendarWindow::c_Config.GetYearFont());
			FNRast->SetAlign(CCalendarWindow::c_Config.GetYearAlign());
			FNRast->UpdateDimensions("XXXX");
			SetRasterizer(FNRast);
			break;
		}
	}
}

/* 
** Paint
**
** Paints the Year in correct place (over the days)
**
*/
void CItemYear::Paint(HDC dc)
{
	int X, Y, W, H;

	if(m_Rasterizer != NULL) 
	{
		W = GetW();
		H = GetH();

		switch (m_Rasterizer->GetAlign() & 0x0F)
		{
		case CRasterizer::ALIGN_LEFT:
			X = CCalendarWindow::c_Config.GetYearX();
			break;

		case CRasterizer::ALIGN_HCENTER:
			X = CCalendarWindow::c_Config.GetYearX() - W / 2;
			break;

		case CRasterizer::ALIGN_RIGHT:
			X = CCalendarWindow::c_Config.GetYearX() - W;
			break;
		};

		switch (m_Rasterizer->GetAlign() & 0x0F0)
		{
		case CRasterizer::ALIGN_TOP:
				Y = CCalendarWindow::c_Config.GetYearY();
				break;

		case CRasterizer::ALIGN_VCENTER:
			Y = CCalendarWindow::c_Config.GetYearY() - H / 2;
			break;

		case CRasterizer::ALIGN_BOTTOM:
			Y = CCalendarWindow::c_Config.GetYearY() - H;
			break;
		};

		SetBkMode(dc, TRANSPARENT);
		SetTextColor(dc, CCalendarWindow::c_Config.GetYearFontColor());

		m_Rasterizer->Paint(dc, X, Y, W, H, CCalendarWindow::c_MonthsFirstDate.wYear);
	}
}

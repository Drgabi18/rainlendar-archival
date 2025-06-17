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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemYear.cpp,v 1.2 2001/12/23 10:00:17 rainy Exp $

  $Log: ItemYear.cpp,v $
  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "ItemYear.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
	if(m_Rasterizer) {
		switch(m_Rasterizer->GetAlign()) {
		case CRasterizer::ALIGN_LEFT:
			return CCalendarWindow::c_Config.GetYearX();
		case CRasterizer::ALIGN_CENTER:
			return CCalendarWindow::c_Config.GetYearX()-GetW()/2;
		case CRasterizer::ALIGN_RIGHT:
			return CCalendarWindow::c_Config.GetYearX()-GetW();
		}
	}
	return 0;
}

int CItemYear::GetY()
{
	return CCalendarWindow::c_Config.GetYearY();
}

int CItemYear::GetW()
{
	if(m_Rasterizer) return m_Rasterizer->GetWidth()/NUMOFCOMPONENTS;

	return 0;
}

int CItemYear::GetH()
{
	if(m_Rasterizer) return m_Rasterizer->GetHeight();

	return 0;
}


/* 
** Initialize
**
** 
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
			if(BMRast==NULL) throw ERR_OUTOFMEM;

			BMRast->Load(CCalendarWindow::c_Config.GetYearBitmapName());
			BMRast->SetNumOfComponents(NUMOFCOMPONENTS);

			BMRast->SetAlign(CCalendarWindow::c_Config.GetYearAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw ERR_OUTOFMEM;

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
void CItemYear::Paint(CDC& dc)
{
	int X, Y, W, H;

	if(m_Rasterizer!=NULL) {
		W=m_Rasterizer->GetWidth();
		H=m_Rasterizer->GetHeight();
		Y=CCalendarWindow::c_Config.GetYearY();

		switch(CCalendarWindow::c_Config.GetYearAlign()) {
		case CRasterizer::ALIGN_CENTER:		
			X=CCalendarWindow::c_Config.GetYearX()-W/2;
			break;

		case CRasterizer::ALIGN_RIGHT:
			X=CCalendarWindow::c_Config.GetYearX()-W;
			break;

		default:
			X=CCalendarWindow::c_Config.GetYearX();
		}

		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(CCalendarWindow::c_Config.GetYearFontColor());

		m_Rasterizer->Paint(dc, X, Y, W, H, CCalendarWindow::c_CurrentDate.GetYear());
	}
}

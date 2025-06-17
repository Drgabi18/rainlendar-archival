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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/ItemMonth.cpp,v 1.2 2001/12/23 10:00:17 rainy Exp $

  $Log: ItemMonth.cpp,v $
  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "ItemMonth.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
	if(m_Rasterizer) {
		switch(m_Rasterizer->GetAlign()) {
		case CRasterizer::ALIGN_LEFT:
			return CCalendarWindow::c_Config.GetMonthX();
		case CRasterizer::ALIGN_CENTER:
			return CCalendarWindow::c_Config.GetMonthX()-GetW()/2;
		case CRasterizer::ALIGN_RIGHT:
			return CCalendarWindow::c_Config.GetMonthX()-GetW();
		}
	}
	return 0;
}


int CItemMonth::GetY()
{
	return CCalendarWindow::c_Config.GetMonthY();
}

int CItemMonth::GetW()
{
	if(m_Rasterizer) {
		if(m_Rasterizer->GetHeight()>m_Rasterizer->GetWidth()) {
			return m_Rasterizer->GetWidth();
		} else {
			return m_Rasterizer->GetWidth()/NUMOFCOMPONENTS;
		}
	}
	return 0;
}

int CItemMonth::GetH()
{
	if(m_Rasterizer) {
		if(m_Rasterizer->GetHeight()>m_Rasterizer->GetWidth()) {
			return m_Rasterizer->GetHeight()/NUMOFCOMPONENTS;
		} else {
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
			if(BMRast==NULL) throw ERR_OUTOFMEM;

			BMRast->Load(CCalendarWindow::c_Config.GetMonthBitmapName());
			BMRast->SetNumOfComponents(NUMOFCOMPONENTS);

			BMRast->SetAlign(CCalendarWindow::c_Config.GetMonthAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw ERR_OUTOFMEM;

			FNRast->SetFont(CCalendarWindow::c_Config.GetMonthFont());
			FNRast->CreateStringTable(CCalendarWindow::c_Config.GetMonthNames(), NUMOFCOMPONENTS);
			FNRast->SetAlign(CCalendarWindow::c_Config.GetMonthAlign());
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
void CItemMonth::Paint(CDC& dc)
{
	int X, Y, W, H;

	if(m_Rasterizer!=NULL) {
		W=GetW();
		H=GetH();
		Y=CCalendarWindow::c_Config.GetMonthY();

		switch(CCalendarWindow::c_Config.GetMonthAlign()) {
		case CRasterizer::ALIGN_CENTER:		
			X=CCalendarWindow::c_Config.GetMonthX()-W/2;
			break;

		case CRasterizer::ALIGN_RIGHT:
			X=CCalendarWindow::c_Config.GetMonthX()-W;
			break;

		default:
			X=CCalendarWindow::c_Config.GetMonthX();
		}

		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(CCalendarWindow::c_Config.GetMonthFontColor());

		m_Rasterizer->Paint(dc, X, Y, W, H, CCalendarWindow::c_CurrentDate.GetMonth()-1);
	}
}

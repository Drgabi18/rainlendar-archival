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

// ItemYear.cpp: implementation of the CItemYear class.
//
//////////////////////////////////////////////////////////////////////

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
			return CCalendarWindow::C_Config.GetYearX();
		case CRasterizer::ALIGN_CENTER:
			return CCalendarWindow::C_Config.GetYearX()-GetW()/2;
		case CRasterizer::ALIGN_RIGHT:
			return CCalendarWindow::C_Config.GetYearX()-GetW();
		}
	}
	return 0;
}

int CItemYear::GetY()
{
	return CCalendarWindow::C_Config.GetYearY();
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
	if( CCalendarWindow::C_Config.GetYearEnable() && 
		CCalendarWindow::C_Config.GetYearRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CCalendarWindow::C_Config.GetYearRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) throw ERR_OUTOFMEM;

			BMRast->Load(CCalendarWindow::C_Config.GetYearBitmapName());
			BMRast->SetNumOfComponents(NUMOFCOMPONENTS);

			BMRast->SetAlign(CCalendarWindow::C_Config.GetYearAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw ERR_OUTOFMEM;

			FNRast->SetFont(CCalendarWindow::C_Config.GetYearFont());
			FNRast->SetAlign(CCalendarWindow::C_Config.GetYearAlign());
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
		Y=CCalendarWindow::C_Config.GetYearY();

		switch(CCalendarWindow::C_Config.GetYearAlign()) {
		case CRasterizer::ALIGN_CENTER:		
			X=CCalendarWindow::C_Config.GetYearX()-W/2;
			break;

		case CRasterizer::ALIGN_RIGHT:
			X=CCalendarWindow::C_Config.GetYearX()-W;
			break;

		default:
			X=CCalendarWindow::C_Config.GetYearX();
		}

		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(CCalendarWindow::C_Config.GetYearFontColor());

		m_Rasterizer->Paint(dc, X, Y, W, H, CCalendarWindow::C_CurrentDate.GetYear());
	}
}

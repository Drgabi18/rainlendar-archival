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

// ItemMonth.cpp: implementation of the CItemMonth class.
//
//////////////////////////////////////////////////////////////////////

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
			return CCalendarWindow::C_Config.GetMonthX();
		case CRasterizer::ALIGN_CENTER:
			return CCalendarWindow::C_Config.GetMonthX()-GetW()/2;
		case CRasterizer::ALIGN_RIGHT:
			return CCalendarWindow::C_Config.GetMonthX()-GetW();
		}
	}
	return 0;
}


int CItemMonth::GetY()
{
	return CCalendarWindow::C_Config.GetMonthY();
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
	if( CCalendarWindow::C_Config.GetMonthEnable() && 
		CCalendarWindow::C_Config.GetMonthRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(CCalendarWindow::C_Config.GetMonthRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) throw ERR_OUTOFMEM;

			BMRast->Load(CCalendarWindow::C_Config.GetMonthBitmapName());
			BMRast->SetNumOfComponents(NUMOFCOMPONENTS);

			BMRast->SetAlign(CCalendarWindow::C_Config.GetMonthAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) throw ERR_OUTOFMEM;

			FNRast->SetFont(CCalendarWindow::C_Config.GetMonthFont());
			FNRast->CreateStringTable(CCalendarWindow::C_Config.GetMonthNames(), NUMOFCOMPONENTS);
			FNRast->SetAlign(CCalendarWindow::C_Config.GetMonthAlign());
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
		Y=CCalendarWindow::C_Config.GetMonthY();

		switch(CCalendarWindow::C_Config.GetMonthAlign()) {
		case CRasterizer::ALIGN_CENTER:		
			X=CCalendarWindow::C_Config.GetMonthX()-W/2;
			break;

		case CRasterizer::ALIGN_RIGHT:
			X=CCalendarWindow::C_Config.GetMonthX()-W;
			break;

		default:
			X=CCalendarWindow::C_Config.GetMonthX();
		}

		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(CCalendarWindow::C_Config.GetMonthFontColor());

		m_Rasterizer->Paint(dc, X, Y, W, H, CCalendarWindow::C_CurrentDate.GetMonth()-1);
	}
}

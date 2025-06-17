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
  $Header: /home/cvsroot/Rainlendar/Library/ItemYear.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemYear.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.13  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.12  2004/11/06 13:38:59  rainy
  no message

  Revision 1.11  2004/01/25 10:00:55  rainy
  Fixed size calculation.

  Revision 1.10  2003/10/27 17:37:52  Rainy
  Config is now singleton.

  Revision 1.9  2003/06/15 09:49:13  Rainy
  Added support for multiple calendars.

  Revision 1.8  2002/11/12 18:11:33  rainy
  The interface of Paint changed a little.

  Revision 1.7  2002/08/24 11:10:35  rainy
  Changed the error handling.

  Revision 1.6  2002/08/03 16:14:29  rainy
  Added separation and color setting for the rasterizer.

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
#pragma warning(disable: 4996)

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
	m_YearBitmapName = "";
	m_YearX=0;
	m_YearY=0;
	m_YearAlign=CRasterizer::ALIGN_LEFT;
	m_YearRasterizer=CRasterizer::TYPE_NONE;
	m_YearFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_YearFontColor=0;
	m_YearSeparation=0;

}

CItemYear::~CItemYear()
{

}

void CItemYear::ReadSettings(const char* iniFile, const char* section)
{
	CItem::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	// Year stuff
	m_Enabled=(1==GetPrivateProfileInt( section, "YearEnable", m_Enabled?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( section, "YearBitmapName", m_YearBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_YearBitmapName=tmpSz;
	}
	m_YearX=GetPrivateProfileInt( section, "YearX", m_YearX, iniFile);
	m_YearY=GetPrivateProfileInt( section, "YearY", m_YearY, iniFile);
	m_YearAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( section, "YearAlign", m_YearAlign, iniFile);
	if(GetPrivateProfileString( section, "YearRasterizer", CConfig::ConvertRasterizer(m_YearRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_YearRasterizer=CConfig::ConvertRasterizer(tmpSz);
	}
	else
	{
		m_YearRasterizer=CRasterizer::TYPE_NONE;
	}
	if(GetPrivateProfileString( section, "YearFont", m_YearFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_YearFont=tmpSz;
	}
	if(GetPrivateProfileString( section, "YearFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_YearFontColor = CConfig::ParseColor(tmpSz);
	}
	m_YearSeparation=GetPrivateProfileInt( section, "YearSeparation", m_YearSeparation, iniFile);
}

void CItemYear::WriteSettings()
{
	char tmpSz[256];

	std::string INIPath = m_SettingsFile;

	// Year stuff
	sprintf(tmpSz, "%i", m_Enabled);
	WritePrivateProfileString( m_Section.c_str(), "YearEnable", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "YearBitmapName", m_YearBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i", m_YearX);
	WritePrivateProfileString( m_Section.c_str(), "YearX", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_YearY);
	WritePrivateProfileString( m_Section.c_str(), "YearY", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_YearAlign);
	WritePrivateProfileString( m_Section.c_str(), "YearAlign", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "YearRasterizer", CConfig::ConvertRasterizer(m_YearRasterizer), INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "YearFont", m_YearFont.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%X", m_YearFontColor);
	WritePrivateProfileString( m_Section.c_str(), "YearFontColor", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_YearSeparation);
	WritePrivateProfileString( m_Section.c_str(), "YearSeparation", tmpSz, INIPath.c_str() );
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
		if((m_Rasterizer->GetAlign() & 0x0F) == CRasterizer::ALIGN_LEFT)
			return GetYearX();

		if((m_Rasterizer->GetAlign() & 0x0F) == CRasterizer::ALIGN_HCENTER)
			return GetYearX() - GetW() / 2;

		if((m_Rasterizer->GetAlign() & 0x0F) == CRasterizer::ALIGN_RIGHT)
			return GetYearX() - GetW();
	}

	return 0;
}

int CItemYear::GetY()
{
	if(m_Rasterizer) 
	{
		if((m_Rasterizer->GetAlign() & 0x0F0) == CRasterizer::ALIGN_TOP)
			return GetYearY();

		if((m_Rasterizer->GetAlign() & 0x0F0) == CRasterizer::ALIGN_VCENTER)
			return GetYearY() - GetH() / 2;

		if((m_Rasterizer->GetAlign() & 0x0F0) == CRasterizer::ALIGN_BOTTOM)
			return GetYearY() - GetH();
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
	if (GetYearRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(GetYearRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(GetYearBitmapName());
			BMRast->SetNumOfComponents(NUMOFCOMPONENTS);
			BMRast->SetSeparation(GetYearSeparation());

			BMRast->SetAlign(GetYearAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(GetYearFont());
			FNRast->SetAlign(GetYearAlign());
			FNRast->SetColor(GetYearFontColor());
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
void CItemYear::Paint(CImage& background, POINT offset)
{
	int X, Y, W, H;

	if(m_Rasterizer != NULL) 
	{
		W = GetW();
		H = GetH();

		switch (m_Rasterizer->GetAlign() & 0x0F)
		{
		case CRasterizer::ALIGN_LEFT:
			X = GetYearX();
			break;

		case CRasterizer::ALIGN_HCENTER:
			X = GetYearX() - W / 2;
			break;

		case CRasterizer::ALIGN_RIGHT:
			X = GetYearX() - W;
			break;
		};

		switch (m_Rasterizer->GetAlign() & 0x0F0)
		{
		case CRasterizer::ALIGN_TOP:
				Y = GetYearY();
				break;

		case CRasterizer::ALIGN_VCENTER:
			Y = GetYearY() - H / 2;
			break;

		case CRasterizer::ALIGN_BOTTOM:
			Y = GetYearY() - H;
			break;
		};

		X += offset.x;
		Y += offset.y;
		
		const SYSTEMTIME& sysTime = CCalendarWindow::c_MonthsFirstDate.GetSystemTime();
		m_Rasterizer->Paint(background, X, Y, W, H, sysTime.wYear);
	}
}

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
  $Header: /home/cvsroot/Rainlendar/Plugin/ItemMonth.cpp,v 1.12 2004/11/06 13:38:59 rainy Exp $

  $Log: ItemMonth.cpp,v $
  Revision 1.12  2004/11/06 13:38:59  rainy
  no message

  Revision 1.11  2004/01/25 10:00:55  rainy
  Fixed size calculation.

  Revision 1.10  2003/10/27 17:37:51  Rainy
  Config is now singleton.

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
#pragma warning(disable: 4996)

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
	m_MonthBitmapName = "";
	m_MonthX=0;
	m_MonthY=0;
	m_MonthAlign=CRasterizer::ALIGN_LEFT;
	m_MonthRasterizer=CRasterizer::TYPE_NONE;
	m_MonthFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_MonthFontColor=0;
	m_MonthNames="January/February/March/April/May/June/July/August/September/October/November/December";
}

CItemMonth::~CItemMonth()
{

}

void CItemMonth::ReadSettings(const char* iniFile, const char* section)
{
	CItem::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	// Month stuff
	m_Enabled=(1==GetPrivateProfileInt( section, "MonthEnable", m_Enabled?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( section, "MonthBitmapName", m_MonthBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MonthBitmapName=tmpSz;
	}
	m_MonthX=GetPrivateProfileInt( section, "MonthX", m_MonthX, iniFile);
	m_MonthY=GetPrivateProfileInt( section, "MonthY", m_MonthY, iniFile);
	m_MonthAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( section, "MonthAlign", m_MonthAlign, iniFile);
	if(GetPrivateProfileString( section, "MonthRasterizer", CConfig::ConvertRasterizer(m_MonthRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MonthRasterizer=CConfig::ConvertRasterizer(tmpSz);
	}
	else
	{
		m_MonthRasterizer=CRasterizer::TYPE_NONE;
	}

	if(GetPrivateProfileString( section, "MonthFont", m_MonthFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MonthFont=tmpSz;
	}
	if(GetPrivateProfileString( section, "MonthFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MonthFontColor = CConfig::ParseColor(tmpSz);
	}
	if(GetPrivateProfileString( section, "MonthNames", m_MonthNames.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MonthNames=tmpSz;
	}
}

void CItemMonth::WriteSettings()
{
	char tmpSz[256];

	std::string INIPath = m_SettingsFile;

	// Month stuff
	sprintf(tmpSz, "%i", m_Enabled);
	WritePrivateProfileString( m_Section.c_str(), "MonthEnable", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "MonthBitmapName", m_MonthBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i", m_MonthX);
	WritePrivateProfileString( m_Section.c_str(), "MonthX", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_MonthY);
	WritePrivateProfileString( m_Section.c_str(), "MonthY", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_MonthAlign);
	WritePrivateProfileString( m_Section.c_str(), "MonthAlign", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "MonthRasterizer", CConfig::ConvertRasterizer(m_MonthRasterizer), INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "MonthFont", m_MonthFont.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%X", m_MonthFontColor);
	WritePrivateProfileString( m_Section.c_str(), "MonthFontColor", tmpSz, INIPath.c_str() );
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
		if((m_Rasterizer->GetAlign() & 0x0F) == CRasterizer::ALIGN_LEFT)
			return GetMonthX();

		if((m_Rasterizer->GetAlign() & 0x0F) == CRasterizer::ALIGN_HCENTER)
			return GetMonthX() - GetW() / 2;

		if((m_Rasterizer->GetAlign() & 0x0F) == CRasterizer::ALIGN_RIGHT)
			return GetMonthX() - GetW();
	}

	return 0;
}

int CItemMonth::GetY()
{
	if(m_Rasterizer) 
	{
		if((m_Rasterizer->GetAlign() & 0x0F0) == CRasterizer::ALIGN_TOP)
			return GetMonthY();

		if((m_Rasterizer->GetAlign() & 0x0F0) == CRasterizer::ALIGN_VCENTER)
			return GetMonthY() - GetH() / 2;

		if((m_Rasterizer->GetAlign() & 0x0F0) == CRasterizer::ALIGN_BOTTOM)
			return GetMonthY() - GetH();
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
	if (GetMonthRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(GetMonthRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(GetMonthBitmapName());
			BMRast->SetNumOfComponents(NUMOFCOMPONENTS);
			BMRast->SetAlign(GetMonthAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(GetMonthFont());
			FNRast->CreateStringTable(GetMonthNames(), NUMOFCOMPONENTS);
			FNRast->SetAlign(GetMonthAlign());
			FNRast->SetColor(GetMonthFontColor());
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
			X = GetMonthX();
			break;

		case CRasterizer::ALIGN_HCENTER:
			X = GetMonthX() - W / 2;
			break;

		case CRasterizer::ALIGN_RIGHT:
			X = GetMonthX() - W;
			break;
		};

		switch (m_Rasterizer->GetAlign() & 0xF0)
		{
		case CRasterizer::ALIGN_TOP:
				Y = GetMonthY();
				break;

		case CRasterizer::ALIGN_VCENTER:
			Y = GetMonthY() - H / 2;
			break;

		case CRasterizer::ALIGN_BOTTOM:
			Y = GetMonthY() - H;
			break;
		};

		X += offset.x;
		Y += offset.y;

		const SYSTEMTIME& sysTime = CCalendarWindow::c_MonthsFirstDate.GetSystemTime();
		m_Rasterizer->Paint(background, X, Y, W, H, sysTime.wMonth - 1);
	}
}

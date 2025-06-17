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
  $Header: /home/cvsroot/Rainlendar/Plugin/ItemToday.cpp,v 1.11 2004/11/06 13:38:59 rainy Exp $

  $Log: ItemToday.cpp,v $
  Revision 1.11  2004/11/06 13:38:59  rainy
  no message

  Revision 1.10  2004/06/10 16:22:25  rainy
  New address to repository.

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
	m_TodayBitmapName = "";
	m_TodayAlign=CRasterizer::ALIGN_LEFT;
	m_TodayNumOfComponents=1;
	m_TodayRasterizer=CRasterizer::TYPE_NONE;
	m_TodayFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_TodayFontColor=0;
	m_TodaySeparation=0;
}

CItemToday::~CItemToday()
{

}

void CItemToday::ReadSettings(const char* iniFile, const char* section)
{
	CItem::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	// Today stuff
	m_Enabled=(1==GetPrivateProfileInt( section, "TodayEnable", m_Enabled?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( section, "TodayBitmapName", m_TodayBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodayBitmapName=tmpSz;
	}
	m_TodayAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( section, "TodayAlign", m_TodayAlign, iniFile);
	m_TodayNumOfComponents=GetPrivateProfileInt( section, "TodayNumOfComponents", m_TodayNumOfComponents, iniFile);
	if(GetPrivateProfileString( section, "TodayRasterizer", CConfig::ConvertRasterizer(m_TodayRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodayRasterizer=CConfig::ConvertRasterizer(tmpSz);
	}
	else 
	{
		m_TodayRasterizer=CRasterizer::TYPE_NONE;
	}

	if(GetPrivateProfileString( section, "TodayFont", m_TodayFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodayFont=tmpSz;
	}
	if(GetPrivateProfileString( section, "TodayFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodayFontColor = CConfig::ParseColor(tmpSz);
	}
	m_TodaySeparation=GetPrivateProfileInt( section, "TodaySeparation", m_TodaySeparation, iniFile);
}

void CItemToday::WriteSettings()
{
	char tmpSz[256];

	std::string INIPath = m_SettingsFile;

	// Today stuff
	sprintf(tmpSz, "%i", m_Enabled);
	WritePrivateProfileString( m_Section.c_str(), "TodayEnable", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "TodayBitmapName", m_TodayBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i", m_TodayAlign);
	WritePrivateProfileString( m_Section.c_str(), "TodayAlign", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_TodayNumOfComponents);
	WritePrivateProfileString( m_Section.c_str(), "TodayNumOfComponents", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "TodayRasterizer", CConfig::ConvertRasterizer(m_TodayRasterizer), INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "TodayFont", m_TodayFont.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%X", m_TodayFontColor);
	WritePrivateProfileString( m_Section.c_str(), "TodayFontColor", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_TodaySeparation);
	WritePrivateProfileString( m_Section.c_str(), "TodaySeparation", tmpSz, INIPath.c_str() );
}

/* 
** Initialize
**
** 
**
*/
void CItemToday::Initialize()
{
	if (GetTodayRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(GetTodayRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(GetTodayBitmapName());
			BMRast->SetNumOfComponents(GetTodayNumOfComponents());
			BMRast->SetSeparation(GetTodaySeparation());
			BMRast->SetAlign(GetTodayAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(GetTodayFont());
			FNRast->SetAlign(GetTodayAlign());
			FNRast->SetColor(GetTodayFontColor());
			FNRast->UpdateDimensions("XX");
			SetRasterizer(FNRast);
			break;
		}
	}

	m_Enabled = m_Enabled && GetRainlendar()->GetCalendarWindow().GetItemDays()->IsEnabled();
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

	CItemDays* days = GetRainlendar()->GetCalendarWindow().GetItemDays();

	SYSTEMTIME sysTime = CCalendarWindow::c_MonthsFirstDate.GetSystemTime();
	SYSTEMTIME sysTime2 = CCalendarWindow::c_TodaysDate.GetSystemTime();

	// Only Paint if this month is shown
	if(sysTime.wMonth == sysTime2.wMonth && sysTime.wYear ==sysTime2.wYear) 
	{
		FirstWeekday = sysTime.wDayOfWeek;

		if(CConfig::Instance().GetStartFromMonday()) 
		{
			FirstWeekday = (FirstWeekday - 1);
			if(FirstWeekday == -1) FirstWeekday = 6;
		} 

		switch (days->GetDaysLayout())
		{
		case CConfig::DAY_LAYOUT_HORIZONTAL:
			W = days->GetDaysW() / 31;
			H = days->GetDaysH();
			break;

		case CConfig::DAY_LAYOUT_VERTICAL:
			W = days->GetDaysW();
			H = days->GetDaysH() / 31;	// 6 Rows
			break;

		default:
			W = days->GetDaysW() / 7;	// 7 Columns
			H = days->GetDaysH() / 6;	// 6 Rows
		}

		if(m_Rasterizer != NULL) 
		{
			Index = sysTime2.wDay + FirstWeekday - 1;

			switch (days->GetDaysLayout())
			{
			case CConfig::DAY_LAYOUT_HORIZONTAL:
				X = days->GetDaysX() + (sysTime2.wDay - 1) * W;
				Y = days->GetDaysY();
				break;

			case CConfig::DAY_LAYOUT_VERTICAL:
				X = days->GetDaysX();
				Y = days->GetDaysY() + (sysTime2.wDay - 1) * H;
				break;

			default:
				X = days->GetDaysX() + (Index % 7) * W;
				Y = days->GetDaysY() + (Index / 7) * H;
			}
		
			X += offset.x;
			Y += offset.y;

			m_Rasterizer->Paint(background, X, Y, W, H, sysTime2.wDay);
		}
	}
}

// Put the today to the lowright corner, so we know if the window must be 
// enlarged.

int CItemToday::GetX()
{
	CItemDays* days = GetRainlendar()->GetCalendarWindow().GetItemDays();

	switch (days->GetDaysLayout())
	{
	case CConfig::DAY_LAYOUT_HORIZONTAL:
	case CConfig::DAY_LAYOUT_VERTICAL:
		return days->GetDaysX();

	default:
		int W = days->GetDaysW() / 7;	// 7 Columns
		int X = days->GetDaysX();
		return W * 6 + X;
	}
}

int CItemToday::GetY()
{
	CItemDays* days = GetRainlendar()->GetCalendarWindow().GetItemDays();

	switch (days->GetDaysLayout())
	{
	case CConfig::DAY_LAYOUT_HORIZONTAL:
	case CConfig::DAY_LAYOUT_VERTICAL:
		return days->GetDaysY();

	default:
		int H = days->GetDaysH() / 6;	// 6 Rows
		int Y = days->GetDaysY();
		return H * 5 + Y;
	}
}

int CItemToday::GetW()
{
	if (m_Rasterizer)
	{
		int numOfComponents = GetTodayNumOfComponents();

		if (m_Rasterizer->GetWidth() > m_Rasterizer->GetHeight())
		{
			return m_Rasterizer->GetWidth() / numOfComponents;
		}

		return m_Rasterizer->GetWidth();
	}
	return 0;
}

int CItemToday::GetH()
{
	if (m_Rasterizer) 
	{
		int numOfComponents = GetTodayNumOfComponents();

		if (m_Rasterizer->GetHeight() > m_Rasterizer->GetWidth())
		{
			return m_Rasterizer->GetHeight() / numOfComponents;
		}

		return m_Rasterizer->GetHeight();
	}
	return 0;
}

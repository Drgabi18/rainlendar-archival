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
  $Header: /home/cvsroot/Rainlendar/Library/ItemDays.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemDays.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.13  2005/03/25 13:58:43  rainy
  no message

  Revision 1.12  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.11  2004/12/05 18:30:42  rainy
  Weekends use correct color now.

  Revision 1.10  2004/11/06 13:38:59  rainy
  no message

  Revision 1.9  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.8  2003/10/27 17:37:51  Rainy
  Config is now singleton.

  Revision 1.7  2003/06/15 09:49:12  Rainy
  Added support for multiple calendars.

  Revision 1.6  2002/11/12 18:11:41  rainy
  The interface of Paint changed a little.

  Revision 1.5  2002/08/24 11:11:54  rainy
  Added ResetDayTypes()

  Revision 1.4  2002/08/03 16:16:37  rainy
  Added separation and color setting for the rasterizer.

  Revision 1.3  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "ItemDays.h"
#include "Error.h"
#include "RasterizerFont.h"
#include "RasterizerBitmap.h"
#include "CalendarWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemDays::CItemDays()
{
	m_WeekendRasterizer = NULL;

	m_DaysX=0;
	m_DaysY=0;
	m_DaysW=100;
	m_DaysH=100;
	m_DaysBitmapName = "";
	m_DaysWeekendBitmapName = "";
	m_DaysNumOfComponents=1;
	m_DaysAlign=CRasterizer::ALIGN_LEFT;
	m_DaysRasterizer=CRasterizer::TYPE_NONE;
	m_DaysFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_DaysFontColor=0;
	m_DaysWeekendFontColor=0;
	m_DaysIgnoreToday=false;
	m_DaysIgnoreEvent=false;
	m_DaysSeparation=0;
	m_DaysLayout=CConfig::DAY_LAYOUT_NORMAL;
}

CItemDays::~CItemDays()
{
	delete m_WeekendRasterizer;
}

/* 
** Get?
**
** Size & Position
**
*/
int CItemDays::GetX()
{
	return GetDaysX();
}

int CItemDays::GetY()
{
	return GetDaysY();
}

int CItemDays::GetW()
{
	return GetDaysW();
}

int CItemDays::GetH()
{
	return GetDaysH();
}

void CItemDays::ReadSettings(const char* iniFile, const char* section)
{
	CItem::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	// Day stuff
	m_Enabled=(1==GetPrivateProfileInt( section, "DaysEnable", m_Enabled?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( section, "DaysBitmapName", m_DaysBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_DaysBitmapName=tmpSz;
	}
	if(GetPrivateProfileString( section, "DaysWeekendBitmapName", m_DaysWeekendBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_DaysWeekendBitmapName=tmpSz;
	}
	m_DaysX=GetPrivateProfileInt( section, "DaysX", m_DaysX, iniFile);
	m_DaysY=GetPrivateProfileInt( section, "DaysY", m_DaysY, iniFile);
	m_DaysW=GetPrivateProfileInt( section, "DaysW", m_DaysW, iniFile);
	m_DaysH=GetPrivateProfileInt( section, "DaysH", m_DaysH, iniFile);
	m_DaysAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( section, "DaysAlign", m_DaysAlign, iniFile);
	m_DaysNumOfComponents=GetPrivateProfileInt( section, "DaysNumOfComponents", m_DaysNumOfComponents, iniFile);
	if(GetPrivateProfileString( section, "DaysRasterizer", CConfig::ConvertRasterizer(m_DaysRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_DaysRasterizer=CConfig::ConvertRasterizer(tmpSz);
	} 
	else 
	{
		m_DaysRasterizer=CRasterizer::TYPE_NONE;
	}

	if(GetPrivateProfileString( section, "DaysFont", m_DaysFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_DaysFont=tmpSz;
	}
	if(GetPrivateProfileString( section, "DaysFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_DaysFontColor = CConfig::ParseColor(tmpSz);
	}
	if(GetPrivateProfileString( section, "DaysWeekendFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_DaysWeekendFontColor = CConfig::ParseColor(tmpSz);
	}
	else
	{
		m_DaysWeekendFontColor = m_DaysFontColor;
	}
	m_DaysIgnoreToday=(1==GetPrivateProfileInt( section, "DaysIgnoreToday", m_DaysIgnoreToday?1:0, iniFile))?true:false;
	m_DaysIgnoreEvent=(1==GetPrivateProfileInt( section, "DaysIgnoreEvent", m_DaysIgnoreEvent?1:0, iniFile))?true:false;
	m_DaysSeparation=GetPrivateProfileInt( section, "DaysSeparation", m_DaysSeparation, iniFile);
	m_DaysLayout=(CConfig::DAY_LAYOUT)GetPrivateProfileInt( section, "DaysLayout", m_DaysLayout, iniFile);
}

void CItemDays::WriteSettings()
{
	char tmpSz[256];

	std::string INIPath = m_SettingsFile;

	// Day stuff
	sprintf(tmpSz, "%i", m_Enabled);
	WritePrivateProfileString( m_Section.c_str(), "DaysEnable", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "DaysBitmapName", m_DaysBitmapName.c_str(), INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "DaysWeekendBitmapName", m_DaysWeekendBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i", m_DaysX);
	WritePrivateProfileString( m_Section.c_str(), "DaysX", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_DaysY);
	WritePrivateProfileString( m_Section.c_str(), "DaysY", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_DaysW);
	WritePrivateProfileString( m_Section.c_str(), "DaysW", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_DaysH);
	WritePrivateProfileString( m_Section.c_str(), "DaysH", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_DaysAlign);
	WritePrivateProfileString( m_Section.c_str(), "DaysAlign", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_DaysNumOfComponents);
	WritePrivateProfileString( m_Section.c_str(), "DaysNumOfComponents", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "DaysRasterizer", CConfig::ConvertRasterizer(m_DaysRasterizer), INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "DaysFont", m_DaysFont.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%X", m_DaysFontColor);
	WritePrivateProfileString( m_Section.c_str(), "DaysFontColor", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%X", m_DaysWeekendFontColor);
	WritePrivateProfileString( m_Section.c_str(), "DaysWeekendFontColor", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_DaysIgnoreToday);
	WritePrivateProfileString( m_Section.c_str(), "DaysIgnoreToday", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_DaysIgnoreEvent);
	WritePrivateProfileString( m_Section.c_str(), "DaysIgnoreEvent", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_DaysSeparation);
	WritePrivateProfileString( m_Section.c_str(), "DaysSeparation", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_DaysLayout);
	WritePrivateProfileString( m_Section.c_str(), "DaysLayout", tmpSz, INIPath.c_str() );
}

/* 
** Initialize
**
** 
**
*/
void CItemDays::Initialize()
{
	if (GetDaysRasterizer() != CRasterizer::TYPE_NONE)
	{
		switch (GetDaysRasterizer()) 
		{
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(GetDaysBitmapName());
			BMRast->SetNumOfComponents(GetDaysNumOfComponents());
			BMRast->SetSeparation(GetDaysSeparation());

			BMRast->SetAlign(GetDaysAlign());
			SetRasterizer(BMRast);

			if (!GetDaysWeekendBitmapName().empty())
			{
				BMRast=new CRasterizerBitmap;
				BMRast->Load(GetDaysWeekendBitmapName());
				BMRast->SetNumOfComponents(GetDaysNumOfComponents());
				BMRast->SetSeparation(GetDaysSeparation());
				BMRast->SetAlign(GetDaysAlign());
				m_WeekendRasterizer = BMRast;
			}
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(GetDaysFont());
			FNRast->SetAlign(GetDaysAlign());
			FNRast->SetColor(GetDaysFontColor());
			FNRast->UpdateDimensions("XX");
			SetRasterizer(FNRast);

			FNRast=new CRasterizerFont;
			FNRast->SetFont(GetDaysFont());
			FNRast->SetAlign(GetDaysAlign());
			FNRast->SetColor(GetDaysWeekendFontColor());
			FNRast->UpdateDimensions("XX");
			m_WeekendRasterizer = FNRast;
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
void CItemDays::Paint(CImage& background, POINT offset)
{
	int FirstWeekday, firstday;
	int X, Y, W, H, Index, DayType, NumOfDays, i;

	// Calculate the number of days in this month
	NumOfDays = CCalendarWindow::c_MonthsFirstDate.GetDaysInMonth();
	firstday = CCalendarWindow::c_MonthsFirstDate.GetWeekday();

	if(CConfig::Instance().GetStartFromMonday()) 
	{
		FirstWeekday = (firstday - 1);
		if(FirstWeekday == -1) FirstWeekday = 6;
	} 
	else
	{
		FirstWeekday = firstday;
	}

	switch (GetDaysLayout())
	{
	case CConfig::DAY_LAYOUT_HORIZONTAL:
		W = GetDaysW() / 31;
		H = GetDaysH();
		break;

	case CConfig::DAY_LAYOUT_VERTICAL:
		W = GetDaysW();
		H = GetDaysH() / 31;	// 6 Rows
		break;

	default:
		W = GetDaysW() / 7;	// 7 Columns
		H = GetDaysH() / 6;	// 6 Rows
	}

	SYSTEMTIME sysTime = CCalendarWindow::c_MonthsFirstDate.GetSystemTime();

	if(m_Rasterizer != NULL) 
	{
		for(i = 0; i < NumOfDays; i++) 
		{
			Index = i + FirstWeekday;
			DayType = CItemEvent::GetDayType(i + 1, sysTime.wMonth, sysTime.wYear, true);

			// Don't show today or events if selected
			if(!(GetDaysIgnoreToday() && (DayType & DAYTYPE_TODAY) ||
				GetDaysIgnoreEvent() && (DayType & DAYTYPE_EVENT))) 
			{	
				switch (GetDaysLayout())
				{
				case CConfig::DAY_LAYOUT_HORIZONTAL:
					X = GetDaysX() + i * W;
					Y = GetDaysY();
					break;

				case CConfig::DAY_LAYOUT_VERTICAL:
					X = GetDaysX();
					Y = GetDaysY() + i * H;
					break;

				default:
					X = GetDaysX() + (Index % 7) * W;
					Y = GetDaysY() + (Index / 7) * H;
				}
	
				X += offset.x;
				Y += offset.y;

				// Check for weekends
				if ((((1 << (firstday + i) % 7)) & CConfig::Instance().GetWeekendDays()) && m_WeekendRasterizer)
				{
					m_WeekendRasterizer->Paint(background, X, Y, W, H, i + 1);
				}
				else
				{
					m_Rasterizer->Paint(background, X, Y, W, H, i + 1);
				}
			}
		}
	}
}


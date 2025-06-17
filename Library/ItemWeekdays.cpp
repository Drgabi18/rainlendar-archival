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
  $Header: /home/cvsroot/Rainlendar/Library/ItemWeekdays.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemWeekdays.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.11  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.10  2004/11/06 13:38:59  rainy
  no message

  Revision 1.9  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.8  2003/10/27 17:37:51  Rainy
  Config is now singleton.

  Revision 1.7  2003/06/15 09:49:13  Rainy
  Added support for multiple calendars.

  Revision 1.6  2002/11/12 18:11:34  rainy
  The interface of Paint changed a little.

  Revision 1.5  2002/08/24 11:10:36  rainy
  Changed the error handling.

  Revision 1.4  2002/08/03 16:15:29  rainy
  Added color setting for the rasterizer.

  Revision 1.3  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "ItemWeekdays.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"

#define NUMOFCOMPONENTS 7

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemWeekdays::CItemWeekdays()
{
	m_WeekdaysBitmapName = "";
	m_WeekdaysAlign=CRasterizer::ALIGN_LEFT;
	m_WeekdaysRasterizer=CRasterizer::TYPE_NONE;
	m_WeekdaysFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_WeekdaysFontColor=0;
	m_WeekdayNames = "SUN/MON/TUE/WED/THU/FRI/SAT";

}

CItemWeekdays::~CItemWeekdays()
{

}

void CItemWeekdays::ReadSettings(const char* iniFile, const char* section)
{
	CItem::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	// Weekdays stuff
	m_Enabled=(1==GetPrivateProfileInt( section, "WeekdaysEnable", m_Enabled?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( section, "WeekdaysBitmapName", m_WeekdaysBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekdaysBitmapName=tmpSz;
	}
	m_WeekdaysAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( section, "WeekdaysAlign", m_WeekdaysAlign, iniFile);
	if(GetPrivateProfileString( section, "WeekdaysRasterizer", CConfig::ConvertRasterizer(m_WeekdaysRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekdaysRasterizer=CConfig::ConvertRasterizer(tmpSz);
	} 
	else 
	{
		m_WeekdaysRasterizer=CRasterizer::TYPE_NONE;
	}

	if(GetPrivateProfileString( section, "WeekdaysFont", m_WeekdaysFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekdaysFont=tmpSz;
	}
	if(GetPrivateProfileString( section, "WeekdaysFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekdaysFontColor = CConfig::ParseColor(tmpSz);
	}
	if(GetPrivateProfileString( section, "WeekdayNames", m_WeekdayNames.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekdayNames=tmpSz;
	}
}

void CItemWeekdays::WriteSettings()
{
	char tmpSz[256];

	std::string INIPath = m_SettingsFile;

	// Weekday stuff
	sprintf(tmpSz, "%i", m_Enabled);
	WritePrivateProfileString( m_Section.c_str(), "WeekdaysEnable", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "WeekdaysBitmapName", m_WeekdaysBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i", m_WeekdaysAlign);
	WritePrivateProfileString( m_Section.c_str(), "WeekdaysAlign", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "WeekdaysRasterizer", CConfig::ConvertRasterizer(m_WeekdaysRasterizer), INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "WeekdaysFont", m_WeekdaysFont.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%X", m_WeekdaysFontColor);
	WritePrivateProfileString( m_Section.c_str(), "WeekdaysFontColor", tmpSz, INIPath.c_str() );
}

/* 
** Initialize
**
** 
**
*/
void CItemWeekdays::Initialize()
{
	if(GetWeekdaysRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(GetWeekdaysRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(GetWeekdaysBitmapName());
			BMRast->SetNumOfComponents(NUMOFCOMPONENTS);

			BMRast->SetAlign(GetWeekdaysAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(GetWeekdaysFont());
			FNRast->CreateStringTable(GetWeekdayNames(), NUMOFCOMPONENTS);
			FNRast->SetAlign(GetWeekdaysAlign());
			FNRast->SetColor(GetWeekdaysFontColor());
			FNRast->UpdateDimensions();
			SetRasterizer(FNRast);
			break;
		}
	}

	m_Enabled = m_Enabled && GetRainlendar()->GetCalendarWindow().GetItemDays()->IsEnabled();
}

/* 
** Paint
**
** Paints the weekdays in correct place (over the days)
**
*/
void CItemWeekdays::Paint(CImage& background, POINT offset)
{
	CItemDays* days = GetRainlendar()->GetCalendarWindow().GetItemDays();

	if (days->GetDaysLayout() != CConfig::DAY_LAYOUT_VERTICAL)
	{
		if (days->GetDaysLayout() == CConfig::DAY_LAYOUT_HORIZONTAL)
		{
			int FirstWeekday;
			int X, Y, W, H, NumOfDays, i;

			NumOfDays = CCalendarWindow::c_MonthsFirstDate.GetDaysInMonth();
			FirstWeekday = CCalendarWindow::c_MonthsFirstDate.GetWeekday();

			W = days->GetDaysW() / 31;	// 7 Columns
			H = days->GetDaysH();	// 6 Rows

			Y = days->GetDaysY() - H;
			Y += offset.y;

			if(m_Rasterizer != NULL) 
			{
				for (i = 0; i < NumOfDays; i++)
				{
					X = days->GetDaysX() + i * W;
					X += offset.x;

					m_Rasterizer->Paint(background, X, Y, W, H, (FirstWeekday + i) % 7);
				}
			}		
		}
		else
		{
			int X, Y, W, H;
			int i;

			W = days->GetDaysW() / 7;	// 7 Columns
			H = days->GetDaysH() / 6;	// 6 Rows

			if(m_Rasterizer != NULL) 
			{
				Y = days->GetDaysY() - H;
				Y += offset.y;

				for(i = 0; i < 7; i++) 
				{
					X = days->GetDaysX() + i * W;
					X += offset.x;

					if(CConfig::Instance().GetStartFromMonday()) 
					{
						if(i == 6) 
						{
							m_Rasterizer->Paint(background, X, Y, W, H, 0);
						} 
						else 
						{
							m_Rasterizer->Paint(background, X, Y, W, H, i + 1);
						}
					}
					else 
					{
						m_Rasterizer->Paint(background, X, Y, W, H, i);
					}
				}
			}
		}
	}
}

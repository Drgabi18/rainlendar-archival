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
  $Header: /home/cvsroot/Rainlendar/Library/ItemWeekNumbers.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: ItemWeekNumbers.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.13  2005/03/01 18:45:30  rainy
  Skin settings are not handled in CConfig anymore.

  Revision 1.12  2004/12/05 18:26:11  rainy
  Added GetWeekNumberDelta() support.

  Revision 1.11  2004/11/06 13:38:59  rainy
  no message

  Revision 1.10  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.9  2003/10/27 17:37:51  Rainy
  Config is now singleton.

  Revision 1.8  2003/06/15 09:49:13  Rainy
  Added support for multiple calendars.

  Revision 1.7  2003/05/07 19:11:14  rainy
  Added new option for weeknumbering.

  Revision 1.6  2002/11/25 17:02:45  rainy
  Fixed a bug

  Revision 1.5  2002/11/12 18:11:34  rainy
  The interface of Paint changed a little.

  Revision 1.4  2002/08/24 11:10:35  rainy
  Changed the error handling.

  Revision 1.3  2002/08/03 16:14:47  rainy
  Added separation and color setting for the rasterizer.
  Fixed the way weeknumbers are calculated.

  Revision 1.2  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.1  2002/01/10 16:40:37  rainy
  Initial version

*/
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"
#include "ItemWeekNumbers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemWeekNumbers::CItemWeekNumbers()
{
	m_WeekNumbersBitmapName = "";
	m_WeekNumbersAlign = CRasterizer::ALIGN_LEFT;
	m_WeekNumbersNumOfComponents = 10;
	m_WeekNumbersRasterizer=CRasterizer::TYPE_NONE;
	m_WeekNumbersFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_WeekNumbersFontColor=0;
	m_WeekNumbersSeparation=0;
}

CItemWeekNumbers::~CItemWeekNumbers()
{

}

void CItemWeekNumbers::ReadSettings(const char* iniFile, const char* section)
{
	CItem::ReadSettings(iniFile, section);

	char tmpSz[MAX_LINE_LENGTH];

	// Week numbers stuff
	m_Enabled=(1==GetPrivateProfileInt( section, "WeekNumbersEnable", m_Enabled?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( section, "WeekNumbersBitmapName", m_WeekNumbersBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekNumbersBitmapName=tmpSz;
	}
	m_WeekNumbersAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( section, "WeekNumbersAlign", m_WeekNumbersAlign, iniFile);
	m_WeekNumbersNumOfComponents=GetPrivateProfileInt( section, "WeekNumbersNumOfComponents", m_WeekNumbersNumOfComponents, iniFile);
	if(GetPrivateProfileString( section, "WeekNumbersRasterizer", CConfig::ConvertRasterizer(m_WeekNumbersRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekNumbersRasterizer=CConfig::ConvertRasterizer(tmpSz);
	}
	else 
	{
		m_WeekNumbersRasterizer=CRasterizer::TYPE_NONE;
	}
	if(GetPrivateProfileString( section, "WeekNumbersFont", m_WeekNumbersFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekNumbersFont=tmpSz;
	}
	if(GetPrivateProfileString( section, "WeekNumbersFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekNumbersFontColor = CConfig::ParseColor(tmpSz);
	}
	m_WeekNumbersSeparation=GetPrivateProfileInt( section, "WeekNumbersSeparation", m_WeekNumbersSeparation, iniFile);
}

void CItemWeekNumbers::WriteSettings()
{
	char tmpSz[256];

	std::string INIPath = m_SettingsFile;

	// Week numbers stuff
	sprintf(tmpSz, "%i", m_Enabled);
	WritePrivateProfileString( m_Section.c_str(), "WeekNumbersEnable", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "WeekNumbersBitmapName", m_WeekNumbersBitmapName.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%i", m_WeekNumbersAlign);
	WritePrivateProfileString( m_Section.c_str(), "WeekNumbersAlign", tmpSz, INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "WeekNumbersRasterizer", CConfig::ConvertRasterizer(m_WeekNumbersRasterizer), INIPath.c_str() );
	WritePrivateProfileString( m_Section.c_str(), "WeekNumbersFont", m_WeekNumbersFont.c_str(), INIPath.c_str() );
	sprintf(tmpSz, "%X", m_WeekNumbersFontColor);
	WritePrivateProfileString( m_Section.c_str(), "WeekNumbersFontColor", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_WeekNumbersNumOfComponents);
	WritePrivateProfileString( m_Section.c_str(), "WeekNumbersNumOfComponents", tmpSz, INIPath.c_str() );
	sprintf(tmpSz, "%i", m_WeekNumbersSeparation);
	WritePrivateProfileString( m_Section.c_str(), "WeekNumbersSeparation", tmpSz, INIPath.c_str() );

}

/* 
** Initialize
**
** 
**
*/
void CItemWeekNumbers::Initialize()
{
	if (GetWeekNumbersRasterizer()!=CRasterizer::TYPE_NONE)
	{
		switch(GetWeekNumbersRasterizer()) {
		case CRasterizer::TYPE_BITMAP:
			CRasterizerBitmap* BMRast;

			BMRast=new CRasterizerBitmap;
			if(BMRast==NULL) THROW(ERR_OUTOFMEM);

			BMRast->Load(GetWeekNumbersBitmapName());
			BMRast->SetNumOfComponents(GetWeekNumbersNumOfComponents());
			BMRast->SetSeparation(GetWeekNumbersSeparation());

			BMRast->SetAlign(GetWeekNumbersAlign());
			SetRasterizer(BMRast);
			break;

		case CRasterizer::TYPE_FONT:
			CRasterizerFont* FNRast;

			FNRast=new CRasterizerFont;
			if(FNRast==NULL) THROW(ERR_OUTOFMEM);

			FNRast->SetFont(GetWeekNumbersFont());
			FNRast->SetAlign(GetWeekNumbersAlign());
			FNRast->SetColor(GetWeekNumbersFontColor());
			FNRast->UpdateDimensions();
			SetRasterizer(FNRast);
			break;
		}
	}
	m_Enabled = m_Enabled && GetRainlendar()->GetCalendarWindow().GetItemDays()->IsEnabled();
}

// Ripped from http://kaijanaho.info/antti-juhani/iki/faq/sao-faq.html/ch4.html
// This follows ISO 8601 -standard
int julian(int d, int m, int y)
{
	int n1, n2;
	n1 = 12 * y + m - 3;
	n2 = n1 / 12;
	return (734 * n1 + 15) / 24 - 2 * n2 + n2 / 4 - n2 / 100 + n2 / 400 + d + 1721119;
}

int weekno(int d, int m, int y)
{
	int n1, n2, w;
	n1 = julian(d, m, y);
    n2 = 7 * (n1 / 7) + 10;
	y = y + 1;
	while ((w = (n2 - julian(1, 1, y)) / 7) <= 0) 
    {
		y = y - 1;
	}
	return w;
}

int weekno2(int d, int m, int y)
{
	int n1, n2, w;
	n1 = julian(d, m, y);
	if(!CConfig::Instance().GetStartFromMonday()) 
    {
        n2 = 7 * (n1 / 7) + 12;
    }
    else
    {
        n2 = 7 * (n1 / 7) + 13;
    }
	y = y + 1;
	while ((w = (n2 - julian(1, 1, y)) / 7) <= 0) 
    {
		y = y - 1;
	}

	return w;
}

/* 
** Paint
**
** Paints the week numbers in correct place (next to the days)
**
*/
void CItemWeekNumbers::Paint(CImage& background, POINT offset)
{
	CItemDays* days = GetRainlendar()->GetCalendarWindow().GetItemDays();

	if (days->GetDaysLayout() == CConfig::DAY_LAYOUT_NORMAL)
	{
		int FirstWeekday, NumOfDays;
		int X, Y, W, H;
		int i, base, lines, weeksFirstDay = 1;

		W = days->GetDaysW() / 7;	// 7 Columns
		H = days->GetDaysH() / 6;	// 6 Rows

		// Calculate if the last line (=6) contains days or not
		SYSTEMTIME sysTime = CCalendarWindow::c_MonthsFirstDate.GetSystemTime();
		NumOfDays = CCalendarWindow::c_MonthsFirstDate.GetDaysInMonth();
		FirstWeekday = sysTime.wDayOfWeek;

		if(CConfig::Instance().GetStartFromMonday()) 
		{
			FirstWeekday = (FirstWeekday - 1);
			if(FirstWeekday == -1) FirstWeekday = 6;
		}
		else
		{
			// If the days start from sunday and the month's first is sunday,
			// we'll have to use monday to determine the week's number or it's
			// calculated wrong
			if (FirstWeekday == 0)
			{
				weeksFirstDay = 2;
			}
		}

		if(FirstWeekday + NumOfDays > 7 * 5)
		{
			lines = 6;
		}
		else
		{
			lines = 5;
		}

  		if(m_Rasterizer != NULL) 
		{
			X = days->GetDaysX() - W;
			X += offset.x;

			for(i = 0; i < lines; i++) 
			{
				Y = days->GetDaysY() + i * H;
				Y += offset.y;

				if (CConfig::Instance().GetWeek1HasJanuary1st())
				{
        			base = weekno2(i * 7 + weeksFirstDay, sysTime.wMonth, sysTime.wYear);
				}
				else
				{
        			base = weekno(i * 7 + weeksFirstDay, sysTime.wMonth, sysTime.wYear);
				}
				base = (base + CConfig::Instance().GetWeekNumberDelta()) % 54;
				if (base <= 0) base += 53;

				m_Rasterizer->Paint(background, X, Y, W, H, base);
			}
		}
	}
}


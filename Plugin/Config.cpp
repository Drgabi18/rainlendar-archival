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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Config.cpp,v 1.1.1.1 2001/10/29 18:56:23 rainy Exp $

  $Log: Config.cpp,v $
  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "Config.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfig::CConfig()
{
	m_X=0;
	m_Y=0;
	m_StartFromMonday=false;
	m_StartHidden=false;
	m_DisableHotkeys=false;
	m_UseWindowName=false;
	m_StartDelay=0;
	m_RefreshDelay=100;

	m_DaysEnable=false;
	m_DaysX=0;
	m_DaysY=0;
	m_DaysW=100;
	m_DaysH=100;
	m_DaysAlign=CRasterizer::ALIGN_CENTER;
	m_DaysNumOfComponents=1;
	m_DaysIgnoreToday=false;
	m_DaysIgnoreEvent=false;

	m_TodayEnable=false;
	m_TodayAlign=CRasterizer::ALIGN_CENTER;
	m_TodayNumOfComponents=1;

	m_WeekdaysEnable=false;
	m_WeekdaysAlign=CRasterizer::ALIGN_CENTER;

	m_MonthEnable=false;
	m_MonthX=0;
	m_MonthY=0;
	m_MonthAlign=CRasterizer::ALIGN_CENTER;

	m_YearEnable=false;
	m_YearX=0;
	m_YearY=0;
	m_YearAlign=CRasterizer::ALIGN_CENTER;

	m_EventEnable=false;
	m_EventAlign=CRasterizer::ALIGN_CENTER;
	m_EventNumOfComponents=1;
	m_EventToolTips=true;
	m_EventMessageBox=true;
}


CConfig::~CConfig()
{

}

/* 
** ReadConfig
**
** Read the ini-file
**
*/
void CConfig::ReadConfig()
{
	CString INIPath;
	char tmpSz[MAX_LINE_LENGTH];

	INIPath=m_Path+"Rainlendar.ini";

	// General
	m_X=GetPrivateProfileInt( "Rainlendar", "X", 0, INIPath);
	m_Y=GetPrivateProfileInt( "Rainlendar", "Y", 0, INIPath);
	m_StartFromMonday=(1==GetPrivateProfileInt( "Rainlendar", "StartFromMonday", 0, INIPath))?true:false;
	m_StartHidden=(1==GetPrivateProfileInt( "Rainlendar", "StartHidden", 0, INIPath))?true:false;
	m_DisableHotkeys=(1==GetPrivateProfileInt( "Rainlendar", "DisableHotkeys", 0, INIPath))?true:false;
	m_UseWindowName=(1==GetPrivateProfileInt( "Rainlendar", "UseWindowName", 0, INIPath))?true:false;
	m_StartDelay=GetPrivateProfileInt( "Rainlendar", "StartDelay", 0, INIPath);
	if(GetPrivateProfileString( "Rainlendar", "BackgroundBitmapName", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_BackgroundBitmapName=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "MonthNames", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_MonthNames=tmpSz;
	} else {
		m_MonthNames="January/February/March/April/May/June/July/August/September/October/November/December";
	}
	if(GetPrivateProfileString( "Rainlendar", "WeekdayNames", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_WeekdayNames=tmpSz;
	} else {
		m_WeekdayNames="SUN/MON/TUE/WED/THU/FRI/SAT";
	}
	m_RefreshDelay=GetPrivateProfileInt( "Rainlendar", "RefreshDelay", 100, INIPath);

	// Day stuff
	m_DaysEnable=(1==GetPrivateProfileInt( "Rainlendar", "DaysEnable", 0, INIPath))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "DaysBitmapName", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_DaysBitmapName=tmpSz;
	}
	m_DaysX=GetPrivateProfileInt( "Rainlendar", "DaysX", 0, INIPath);
	m_DaysY=GetPrivateProfileInt( "Rainlendar", "DaysY", 0, INIPath);
	m_DaysW=GetPrivateProfileInt( "Rainlendar", "DaysW", 100, INIPath);
	m_DaysH=GetPrivateProfileInt( "Rainlendar", "DaysH", 100, INIPath);
	m_DaysAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "DaysAlign", 2, INIPath);
	m_DaysNumOfComponents=GetPrivateProfileInt( "Rainlendar", "DaysNumOfComponents", 1, INIPath);
	if(GetPrivateProfileString( "Rainlendar", "DaysRasterizer", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_DaysRasterizer=ConvertRasterizer(tmpSz);
	} else m_DaysRasterizer=CRasterizer::TYPE_NONE;
	if(GetPrivateProfileString( "Rainlendar", "DaysFont", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_DaysFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "DaysFontColor", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		sscanf(tmpSz, "%X", &m_DaysFontColor);
	}
	m_DaysIgnoreToday=(1==GetPrivateProfileInt( "Rainlendar", "DaysIgnoreToday", 0, INIPath))?true:false;
	m_DaysIgnoreEvent=(1==GetPrivateProfileInt( "Rainlendar", "DaysIgnoreEvent", 0, INIPath))?true:false;

	// Today stuff
	m_TodayEnable=(1==GetPrivateProfileInt( "Rainlendar", "TodayEnable", 0, INIPath))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "TodayBitmapName", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_TodayBitmapName=tmpSz;
	}
	m_TodayAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "TodayAlign", 2, INIPath);
	m_TodayNumOfComponents=GetPrivateProfileInt( "Rainlendar", "TodayNumOfComponents", 1, INIPath);
	if(GetPrivateProfileString( "Rainlendar", "TodayRasterizer", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_TodayRasterizer=ConvertRasterizer(tmpSz);
	} else m_TodayRasterizer=CRasterizer::TYPE_NONE;
	if(GetPrivateProfileString( "Rainlendar", "TodayFont", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_TodayFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "TodayFontColor", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		sscanf(tmpSz, "%X", &m_TodayFontColor);
	}

	// Weekdays stuff
	m_WeekdaysEnable=(1==GetPrivateProfileInt( "Rainlendar", "WeekdaysEnable", 0, INIPath))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "WeekdaysBitmapName", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_WeekdaysBitmapName=tmpSz;
	}
	m_WeekdaysAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "WeekdaysAlign", 2, INIPath);
	if(GetPrivateProfileString( "Rainlendar", "WeekdaysRasterizer", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_WeekdaysRasterizer=ConvertRasterizer(tmpSz);
	} else m_WeekdaysRasterizer=CRasterizer::TYPE_NONE;
	if(GetPrivateProfileString( "Rainlendar", "WeekdaysFont", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_WeekdaysFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "WeekdaysFontColor", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		sscanf(tmpSz, "%X", &m_WeekdaysFontColor);
	}

	// Month stuff
	m_MonthEnable=(1==GetPrivateProfileInt( "Rainlendar", "MonthEnable", 0, INIPath))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "MonthBitmapName", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_MonthBitmapName=tmpSz;
	}
	m_MonthX=GetPrivateProfileInt( "Rainlendar", "MonthX", 0, INIPath);
	m_MonthY=GetPrivateProfileInt( "Rainlendar", "MonthY", 0, INIPath);
	m_MonthAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "MonthAlign", 2, INIPath);
	if(GetPrivateProfileString( "Rainlendar", "MonthRasterizer", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_MonthRasterizer=ConvertRasterizer(tmpSz);
	} else m_MonthRasterizer=CRasterizer::TYPE_NONE;
	if(GetPrivateProfileString( "Rainlendar", "MonthFont", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_MonthFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "MonthFontColor", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		sscanf(tmpSz, "%X", &m_MonthFontColor);
	}

	// Year stuff
	m_YearEnable=(1==GetPrivateProfileInt( "Rainlendar", "YearEnable", 0, INIPath))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "YearBitmapName", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_YearBitmapName=tmpSz;
	}
	m_YearX=GetPrivateProfileInt( "Rainlendar", "YearX", 0, INIPath);
	m_YearY=GetPrivateProfileInt( "Rainlendar", "YearY", 0, INIPath);
	m_YearAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "YearAlign", 2, INIPath);
	if(GetPrivateProfileString( "Rainlendar", "YearRasterizer", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_YearRasterizer=ConvertRasterizer(tmpSz);
	} else m_YearRasterizer=CRasterizer::TYPE_NONE;
	if(GetPrivateProfileString( "Rainlendar", "YearFont", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_YearFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "YearFontColor", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		sscanf(tmpSz, "%X", &m_YearFontColor);
	}

	// Event stuff
	m_EventEnable=(1==GetPrivateProfileInt( "Rainlendar", "EventEnable", 0, INIPath))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "EventBitmapName", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_EventBitmapName=tmpSz;
	}
	m_EventAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "EventAlign", 2, INIPath);
	m_EventNumOfComponents=GetPrivateProfileInt( "Rainlendar", "EventNumOfComponents", 1, INIPath);
	if(GetPrivateProfileString( "Rainlendar", "EventRasterizer", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_EventRasterizer=ConvertRasterizer(tmpSz);
	} else m_EventRasterizer=CRasterizer::TYPE_NONE;
	if(GetPrivateProfileString( "Rainlendar", "EventFont", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_EventFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "EventFontColor", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		sscanf(tmpSz, "%X", &m_EventFontColor);
	}
	if(GetPrivateProfileString( "Rainlendar", "EventExecute", "", tmpSz, MAX_LINE_LENGTH, INIPath) > 0) {
		m_EventExecute=tmpSz;
	}
	m_EventToolTips=(1==GetPrivateProfileInt( "Rainlendar", "EventToolTips", 0, INIPath))?true:false;
	m_EventMessageBox=(1==GetPrivateProfileInt( "Rainlendar", "EventMessageBox", 0, INIPath))?true:false;
}

/* 
** WriteConfig
**
** Write the ini-file
**
*/
void CConfig::WriteConfig()
{
	CString INIPath;
	char tmpSz[256];

	INIPath=m_Path+"Rainlendar.ini";

	// General
	sprintf(tmpSz, "%i", m_X);
	WritePrivateProfileString( "Rainlendar", "X", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_Y);
	WritePrivateProfileString( "Rainlendar", "Y", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_StartFromMonday);
	WritePrivateProfileString( "Rainlendar", "StartFromMonday", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_StartHidden);
	WritePrivateProfileString( "Rainlendar", "StartHidden", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_DisableHotkeys);
	WritePrivateProfileString( "Rainlendar", "DisableHotkeys", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_UseWindowName);
	WritePrivateProfileString( "Rainlendar", "UseWindowName", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_StartDelay);
	WritePrivateProfileString( "Rainlendar", "StartDelay", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "BackgroundBitmapName", m_BackgroundBitmapName, INIPath );
	WritePrivateProfileString( "Rainlendar", "MonthNames", m_MonthNames, INIPath );
	WritePrivateProfileString( "Rainlendar", "WeekdayNames", m_WeekdayNames, INIPath );
	sprintf(tmpSz, "%i", m_RefreshDelay);
	WritePrivateProfileString( "Rainlendar", "RefreshDelay", tmpSz, INIPath );

	// Day stuff
	sprintf(tmpSz, "%i", m_DaysEnable);
	WritePrivateProfileString( "Rainlendar", "DaysEnable", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "DaysBitmapName", m_DaysBitmapName, INIPath );
	sprintf(tmpSz, "%i", m_DaysX);
	WritePrivateProfileString( "Rainlendar", "DaysX", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_DaysY);
	WritePrivateProfileString( "Rainlendar", "DaysY", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_DaysW);
	WritePrivateProfileString( "Rainlendar", "DaysW", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_DaysH);
	WritePrivateProfileString( "Rainlendar", "DaysH", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_DaysAlign);
	WritePrivateProfileString( "Rainlendar", "DaysAlign", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_DaysNumOfComponents);
	WritePrivateProfileString( "Rainlendar", "DaysNumOfComponents", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "DaysRasterizer", ConvertRasterizer(m_DaysRasterizer), INIPath );
	WritePrivateProfileString( "Rainlendar", "DaysFont", m_DaysFont, INIPath );
	sprintf(tmpSz, "%X", m_DaysFontColor);
	WritePrivateProfileString( "Rainlendar", "DaysFontColor", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_DaysIgnoreToday);
	WritePrivateProfileString( "Rainlendar", "DaysIgnoreToday", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_DaysIgnoreEvent);
	WritePrivateProfileString( "Rainlendar", "DaysIgnoreEvent", tmpSz, INIPath );

	// Today stuff
	sprintf(tmpSz, "%i", m_TodayEnable);
	WritePrivateProfileString( "Rainlendar", "TodayEnable", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "TodayBitmapName", m_TodayBitmapName, INIPath );
	sprintf(tmpSz, "%i", m_TodayAlign);
	WritePrivateProfileString( "Rainlendar", "TodayAlign", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_TodayNumOfComponents);
	WritePrivateProfileString( "Rainlendar", "TodayNumOfComponents", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "TodayRasterizer", ConvertRasterizer(m_TodayRasterizer), INIPath );
	WritePrivateProfileString( "Rainlendar", "TodayFont", m_TodayFont, INIPath );
	sprintf(tmpSz, "%X", m_TodayFontColor);
	WritePrivateProfileString( "Rainlendar", "TodayFontColor", tmpSz, INIPath );

	// Weekday stuff
	sprintf(tmpSz, "%i", m_WeekdaysEnable);
	WritePrivateProfileString( "Rainlendar", "WeekdaysEnable", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "WeekdaysBitmapName", m_WeekdaysBitmapName, INIPath );
	sprintf(tmpSz, "%i", m_WeekdaysAlign);
	WritePrivateProfileString( "Rainlendar", "WeekdaysAlign", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "WeekdaysRasterizer", ConvertRasterizer(m_WeekdaysRasterizer), INIPath );
	WritePrivateProfileString( "Rainlendar", "WeekdaysFont", m_WeekdaysFont, INIPath );
	sprintf(tmpSz, "%X", m_WeekdaysFontColor);
	WritePrivateProfileString( "Rainlendar", "WeekdaysFontColor", tmpSz, INIPath );

	// Month stuff
	sprintf(tmpSz, "%i", m_MonthEnable);
	WritePrivateProfileString( "Rainlendar", "MonthEnable", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "MonthBitmapName", m_MonthBitmapName, INIPath );
	sprintf(tmpSz, "%i", m_MonthX);
	WritePrivateProfileString( "Rainlendar", "MonthX", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_MonthY);
	WritePrivateProfileString( "Rainlendar", "MonthY", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_MonthAlign);
	WritePrivateProfileString( "Rainlendar", "MonthAlign", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "MonthRasterizer", ConvertRasterizer(m_MonthRasterizer), INIPath );
	WritePrivateProfileString( "Rainlendar", "MonthFont", m_MonthFont, INIPath );
	sprintf(tmpSz, "%X", m_MonthFontColor);
	WritePrivateProfileString( "Rainlendar", "MonthFontColor", tmpSz, INIPath );

	// Year stuff
	sprintf(tmpSz, "%i", m_YearEnable);
	WritePrivateProfileString( "Rainlendar", "YearEnable", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "YearBitmapName", m_YearBitmapName, INIPath );
	sprintf(tmpSz, "%i", m_YearX);
	WritePrivateProfileString( "Rainlendar", "YearX", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_YearY);
	WritePrivateProfileString( "Rainlendar", "YearY", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_YearAlign);
	WritePrivateProfileString( "Rainlendar", "YearAlign", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "YearRasterizer", ConvertRasterizer(m_YearRasterizer), INIPath );
	WritePrivateProfileString( "Rainlendar", "YearFont", m_YearFont, INIPath );
	sprintf(tmpSz, "%X", m_YearFontColor);
	WritePrivateProfileString( "Rainlendar", "YearFontColor", tmpSz, INIPath );

	// Event stuff
	sprintf(tmpSz, "%i", m_EventEnable);
	WritePrivateProfileString( "Rainlendar", "EventEnable", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "EventBitmapName", m_EventBitmapName, INIPath );
	sprintf(tmpSz, "%i", m_EventAlign);
	WritePrivateProfileString( "Rainlendar", "EventAlign", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_EventNumOfComponents);
	WritePrivateProfileString( "Rainlendar", "EventNumOfComponents", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "EventRasterizer", ConvertRasterizer(m_EventRasterizer), INIPath );
	WritePrivateProfileString( "Rainlendar", "EventFont", m_EventFont, INIPath );
	sprintf(tmpSz, "%X", m_EventFontColor);
	WritePrivateProfileString( "Rainlendar", "EventFontColor", tmpSz, INIPath );
	WritePrivateProfileString( "Rainlendar", "EventExecute", m_EventExecute, INIPath );
	sprintf(tmpSz, "%i", m_EventToolTips);
	WritePrivateProfileString( "Rainlendar", "EventToolTips", tmpSz, INIPath );
	sprintf(tmpSz, "%i", m_EventMessageBox);
	WritePrivateProfileString( "Rainlendar", "EventMessageBox", tmpSz, INIPath );

	WritePrivateProfileString( NULL, NULL, NULL, INIPath );	// FLUSH
}


/* 
** ConvertRasterizer
**
** returns the type of the rasterizer form given string.
**
*/
CRasterizer::TYPE CConfig::ConvertRasterizer(char* String) 
{
	if(String!=NULL) {
		if(0==strcmp(String, "BITMAP")) {
			return CRasterizer::TYPE_BITMAP;
		} else if(0==strcmp(String, "FONT")) {
			return CRasterizer::TYPE_FONT;
		}
	}

	return CRasterizer::TYPE_NONE;
}

/* 
** ConvertRasterizer
**
** Same as above but other way around
**
*/
char* CConfig::ConvertRasterizer(CRasterizer::TYPE Type) 
{
	switch(Type) {
		case CRasterizer::TYPE_FONT:
			return "FONT";
		case CRasterizer::TYPE_BITMAP:
			return "BITMAP";
	}
	return "";
}


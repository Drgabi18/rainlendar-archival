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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Config.cpp,v 1.11 2002/11/12 18:02:33 rainy Exp $

  $Log: Config.cpp,v $
  Revision 1.11  2002/11/12 18:02:33  rainy
  Added Native Transparency and solid background (with bevel).

  Revision 1.10  2002/08/24 11:14:01  rainy
  Changed the error handling.

  Revision 1.9  2002/08/10 08:39:51  rainy
  Loading the sections from the ini does not crash anymore if the file is missing.

  Revision 1.8  2002/08/03 16:19:07  rainy
  Added some new configs.
  Added support for profiles.

  Revision 1.7  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.6  2002/02/27 18:57:15  rainy
  Added new configs.

  Revision 1.5  2002/01/29 17:35:28  rainy
  Default frequency is 60 not 3600.

  Revision 1.4  2002/01/27 16:12:44  rainy
  Changed CEvent to CEventMessage to avoid name clash.
  Added Server stuff.

  Revision 1.3  2002/01/15 17:58:51  rainy
  Removed the StartDelay

  Revision 1.2  2002/01/10 16:48:27  rainy
  Added the weeknumber stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "Config.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfig::CConfig()
{
	m_X=0;
	m_Y=0;
	m_StartFromMonday=true;
	m_StartHidden=false;
	m_DisableHotkeys=false;
	m_UseWindowName=false;
	m_PollWallpaper=false;
	m_Movable=false;
	m_MouseHide=false;
	m_SnapEdges=false;
	m_NativeTransparency=true;
	m_RefreshDelay=100;
	m_WindowPos=WINDOWPOS_ONBOTTOM;
	m_CurrentProfile="Default";
	m_BGCopyMode=BG_NORMAL;

	m_BackgroundMode=CBackground::MODE_TILE;
	m_BackgroundSolidColor=GetSysColor(COLOR_3DFACE);
	m_BackgroundBevel=true;

	m_DaysEnable=false;
	m_DaysX=0;
	m_DaysY=0;
	m_DaysW=100;
	m_DaysH=100;
	m_DaysNumOfComponents=1;
	m_DaysAlign=CRasterizer::ALIGN_LEFT;
	m_DaysRasterizer=CRasterizer::TYPE_NONE;
	m_DaysFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_DaysFontColor=0;
	m_DaysIgnoreToday=false;
	m_DaysIgnoreEvent=false;
	m_DaysSeparation=0;

	m_TodayEnable=false;
	m_TodayAlign=CRasterizer::ALIGN_LEFT;
	m_TodayNumOfComponents=1;
	m_TodayRasterizer=CRasterizer::TYPE_NONE;
	m_TodayFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_TodayFontColor=0;
	m_TodaySeparation=0;

	m_WeekdaysEnable=false;
	m_WeekdaysAlign=CRasterizer::ALIGN_LEFT;
	m_WeekdaysRasterizer=CRasterizer::TYPE_NONE;
	m_WeekdaysFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_WeekdaysFontColor=0;
	m_WeekdayNames = "SUN/MON/TUE/WED/THU/FRI/SAT";

	m_MonthEnable=false;
	m_MonthX=0;
	m_MonthY=0;
	m_MonthAlign=CRasterizer::ALIGN_LEFT;
	m_MonthRasterizer=CRasterizer::TYPE_NONE;
	m_MonthFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_MonthFontColor=0;
	m_MonthNames="January/February/March/April/May/June/July/August/September/October/November/December";

	m_YearEnable=false;
	m_YearX=0;
	m_YearY=0;
	m_YearAlign=CRasterizer::ALIGN_LEFT;
	m_YearRasterizer=CRasterizer::TYPE_NONE;
	m_YearFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_YearFontColor=0;
	m_YearSeparation=0;

	m_EventEnable=false;
	m_EventAlign=CRasterizer::ALIGN_LEFT;
	m_EventNumOfComponents=1;
	m_EventToolTips=true;
	m_EventMessageBox=true;
	m_EventInCalendar=false;
	m_EventRasterizer=CRasterizer::TYPE_NONE;
	m_EventFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_EventFont2="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_EventFontColor=0;
	m_EventFontColor2=0;
	m_EventSeparation=0;
		
	m_WeekNumbersEnable = false;
	m_WeekNumbersAlign = CRasterizer::ALIGN_LEFT;
	m_WeekNumbersNumOfComponents = 10;
	m_WeekNumbersRasterizer=CRasterizer::TYPE_NONE;
	m_WeekNumbersFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_WeekNumbersFontColor=0;
	m_WeekNumbersSeparation=0;

	m_ServerEnable = false;
	m_ServerPort = 0;
	m_ServerFrequency = 60;		// every hour
	m_ServerStartup = false;

	m_ToolTipFontColor = GetSysColor(COLOR_INFOTEXT);
	m_ToolTipBGColor = GetSysColor(COLOR_INFOBK);
	m_ToolTipFont = "-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
}

CConfig::~CConfig()
{
	std::list<Profile*>::iterator i = m_Profiles.begin();
	for( ; i != m_Profiles.end(); i++)
	{
		delete (*i);
	}
}

void CConfig::GetIniTime(const std::string& filename)
{
	// Get the timestamp from the ini-file
	HANDLE file = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		GetFileTime(file, NULL, NULL, &m_WriteTime);
		CloseHandle(file);
	}
	else
	{
		std::string err = "The ini-file not found: ";
		err += filename;
		THROW(err);
	}
}

bool CConfig::CompareIniTime(const std::string& filename)
{
	// Get the timestamp from the ini-file and compare it with the one obtained during read time
	HANDLE file = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		FILETIME newTime;
		GetFileTime(file, NULL, NULL, &newTime);
		CloseHandle(file);
		if (0 != CompareFileTime(&newTime, &m_WriteTime)) return false;	// Do not save the config
	}

	return true;
}

/* 
** ReadConfig
**
** Read the ini-file
**
*/
void CConfig::ReadConfig()
{
	std::string INIPath;
	INIPath = m_Path + "Rainlendar.ini";

	GetIniTime(INIPath);

	ReadGeneralConfig(INIPath.c_str());
	ReadSkinConfig(INIPath.c_str());

	// Override the settings from the skin 
	if (!m_CurrentSkin.empty() && !m_CurrentSkinIni.empty())
	{
		INIPath = m_SkinsPath + m_CurrentSkin + "/" + m_CurrentSkinIni;
		ReadGeneralConfig(INIPath.c_str());
		ReadSkinConfig(INIPath.c_str());
		ReadProfiles(INIPath.c_str());
	}
}

void CConfig::ReadGeneralConfig(const char* iniFile)
{
	char tmpSz[MAX_LINE_LENGTH];

	// General
	m_X=GetPrivateProfileInt( "Rainlendar", "X", m_X, iniFile);
	m_Y=GetPrivateProfileInt( "Rainlendar", "Y", m_Y, iniFile);
	m_StartFromMonday=(1==GetPrivateProfileInt( "Rainlendar", "StartFromMonday", m_StartFromMonday?1:0, iniFile))?true:false;
	m_StartHidden=(1==GetPrivateProfileInt( "Rainlendar", "StartHidden", m_StartHidden?1:0, iniFile))?true:false;
	m_DisableHotkeys=(1==GetPrivateProfileInt( "Rainlendar", "DisableHotkeys", m_DisableHotkeys?1:0, iniFile))?true:false;
	m_UseWindowName=(1==GetPrivateProfileInt( "Rainlendar", "UseWindowName", m_UseWindowName?1:0, iniFile))?true:false;
	m_PollWallpaper=(1==GetPrivateProfileInt( "Rainlendar", "PollWallpaper", m_PollWallpaper?1:0, iniFile))?true:false;
	m_Movable=(1==GetPrivateProfileInt( "Rainlendar", "Movable", m_Movable?1:0, iniFile))?true:false;
	m_MouseHide=(1==GetPrivateProfileInt( "Rainlendar", "MouseHide", m_MouseHide?1:0, iniFile))?true:false;
	m_SnapEdges=(1==GetPrivateProfileInt( "Rainlendar", "SnapEdges", m_SnapEdges?1:0, iniFile))?true:false;
	m_NativeTransparency=(1==GetPrivateProfileInt( "Rainlendar", "NativeTransparency", m_NativeTransparency?1:0, iniFile))?true:false;
	m_RefreshDelay=GetPrivateProfileInt( "Rainlendar", "RefreshDelay", m_RefreshDelay, iniFile);
	m_WindowPos=(WINDOWPOS)GetPrivateProfileInt( "Rainlendar", "WindowPos", m_WindowPos, iniFile);
	m_BGCopyMode=(BG_COPY_MODE)GetPrivateProfileInt( "Rainlendar", "BGCopyMode", m_BGCopyMode, iniFile);

	if(GetPrivateProfileString( "Rainlendar", "CurrentProfile", m_CurrentProfile.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_CurrentProfile=tmpSz;
	}

	if(GetPrivateProfileString( "Rainlendar", "EventExecute", m_EventExecute.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventExecute=tmpSz;
	}
	m_EventToolTips=(1==GetPrivateProfileInt( "Rainlendar", "EventToolTips", m_EventToolTips?1:0, iniFile))?true:false;
	m_EventMessageBox=(1==GetPrivateProfileInt( "Rainlendar", "EventMessageBox", m_EventMessageBox?1:0, iniFile))?true:false;

	m_ServerEnable=(1==GetPrivateProfileInt( "Rainlendar", "ServerEnable", m_ServerEnable?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "ServerAddress", m_ServerAddress.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_ServerAddress=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "ServerID", m_ServerID.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_ServerID=tmpSz;
	}
	m_ServerPort=GetPrivateProfileInt( "Rainlendar", "ServerPort", m_ServerPort, iniFile);
	m_ServerFrequency=GetPrivateProfileInt( "Rainlendar", "ServerFrequency", m_ServerFrequency, iniFile);
	m_ServerStartup=(1==GetPrivateProfileInt( "Rainlendar", "ServerStartup", m_ServerStartup, iniFile))?true:false;

	if(GetPrivateProfileString( "Rainlendar", "CurrentSkin", m_CurrentSkin.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_CurrentSkin=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "CurrentSkinIni", m_CurrentSkinIni.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_CurrentSkinIni=tmpSz;
	}
}

void CConfig::ReadSkinConfig(const char* iniFile)
{
	char tmpSz[MAX_LINE_LENGTH];

	if(GetPrivateProfileString( "Rainlendar", "BackgroundBitmapName", m_BackgroundBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_BackgroundBitmapName=tmpSz;
	}
	m_BackgroundMode=(CBackground::MODE)GetPrivateProfileInt( "Rainlendar", "BackgroundMode", m_BackgroundMode, iniFile);
	m_BackgroundBevel=(1==GetPrivateProfileInt( "Rainlendar", "BackgroundBevel", m_BackgroundBevel?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "BackgroundSolidColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%X", &m_BackgroundSolidColor);
	}

	// Day stuff
	m_DaysEnable=(1==GetPrivateProfileInt( "Rainlendar", "DaysEnable", m_DaysEnable?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "DaysBitmapName", m_DaysBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_DaysBitmapName=tmpSz;
	}
	m_DaysX=GetPrivateProfileInt( "Rainlendar", "DaysX", m_DaysX, iniFile);
	m_DaysY=GetPrivateProfileInt( "Rainlendar", "DaysY", m_DaysY, iniFile);
	m_DaysW=GetPrivateProfileInt( "Rainlendar", "DaysW", m_DaysW, iniFile);
	m_DaysH=GetPrivateProfileInt( "Rainlendar", "DaysH", m_DaysH, iniFile);
	m_DaysAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "DaysAlign", m_DaysAlign, iniFile);
	m_DaysNumOfComponents=GetPrivateProfileInt( "Rainlendar", "DaysNumOfComponents", m_DaysNumOfComponents, iniFile);
	if(GetPrivateProfileString( "Rainlendar", "DaysRasterizer", ConvertRasterizer(m_DaysRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_DaysRasterizer=ConvertRasterizer(tmpSz);
	} 
	else 
	{
		m_DaysRasterizer=CRasterizer::TYPE_NONE;
	}

	if(GetPrivateProfileString( "Rainlendar", "DaysFont", m_DaysFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_DaysFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "DaysFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%X", &m_DaysFontColor);
	}
	m_DaysIgnoreToday=(1==GetPrivateProfileInt( "Rainlendar", "DaysIgnoreToday", m_DaysIgnoreToday?1:0, iniFile))?true:false;
	m_DaysIgnoreEvent=(1==GetPrivateProfileInt( "Rainlendar", "DaysIgnoreEvent", m_DaysIgnoreEvent?1:0, iniFile))?true:false;
	m_DaysSeparation=GetPrivateProfileInt( "Rainlendar", "DaysSeparation", m_DaysSeparation, iniFile);

	// Today stuff
	m_TodayEnable=(1==GetPrivateProfileInt( "Rainlendar", "TodayEnable", m_TodayEnable?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "TodayBitmapName", m_TodayBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodayBitmapName=tmpSz;
	}
	m_TodayAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "TodayAlign", m_TodayAlign, iniFile);
	m_TodayNumOfComponents=GetPrivateProfileInt( "Rainlendar", "TodayNumOfComponents", m_TodayNumOfComponents, iniFile);
	if(GetPrivateProfileString( "Rainlendar", "TodayRasterizer", ConvertRasterizer(m_TodayRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodayRasterizer=ConvertRasterizer(tmpSz);
	}
	else 
	{
		m_TodayRasterizer=CRasterizer::TYPE_NONE;
	}

	if(GetPrivateProfileString( "Rainlendar", "TodayFont", m_TodayFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodayFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "TodayFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%X", &m_TodayFontColor);
	}
	m_TodaySeparation=GetPrivateProfileInt( "Rainlendar", "TodaySeparation", m_TodaySeparation, iniFile);

	// Weekdays stuff
	m_WeekdaysEnable=(1==GetPrivateProfileInt( "Rainlendar", "WeekdaysEnable", m_WeekdaysEnable?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "WeekdaysBitmapName", m_WeekdaysBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekdaysBitmapName=tmpSz;
	}
	m_WeekdaysAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "WeekdaysAlign", m_WeekdaysAlign, iniFile);
	if(GetPrivateProfileString( "Rainlendar", "WeekdaysRasterizer", ConvertRasterizer(m_WeekdaysRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekdaysRasterizer=ConvertRasterizer(tmpSz);
	} 
	else 
	{
		m_WeekdaysRasterizer=CRasterizer::TYPE_NONE;
	}

	if(GetPrivateProfileString( "Rainlendar", "WeekdaysFont", m_WeekdaysFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekdaysFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "WeekdaysFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%X", &m_WeekdaysFontColor);
	}
	if(GetPrivateProfileString( "Rainlendar", "WeekdayNames", m_WeekdayNames.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekdayNames=tmpSz;
	}

	// Month stuff
	m_MonthEnable=(1==GetPrivateProfileInt( "Rainlendar", "MonthEnable", m_MonthEnable?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "MonthBitmapName", m_MonthBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MonthBitmapName=tmpSz;
	}
	m_MonthX=GetPrivateProfileInt( "Rainlendar", "MonthX", m_MonthX, iniFile);
	m_MonthY=GetPrivateProfileInt( "Rainlendar", "MonthY", m_MonthY, iniFile);
	m_MonthAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "MonthAlign", m_MonthAlign, iniFile);
	if(GetPrivateProfileString( "Rainlendar", "MonthRasterizer", ConvertRasterizer(m_MonthRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MonthRasterizer=ConvertRasterizer(tmpSz);
	}
	else
	{
		m_MonthRasterizer=CRasterizer::TYPE_NONE;
	}

	if(GetPrivateProfileString( "Rainlendar", "MonthFont", m_MonthFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MonthFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "MonthFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%X", &m_MonthFontColor);
	}
	if(GetPrivateProfileString( "Rainlendar", "MonthNames", m_MonthNames.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MonthNames=tmpSz;
	}

	// Year stuff
	m_YearEnable=(1==GetPrivateProfileInt( "Rainlendar", "YearEnable", m_YearEnable?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "YearBitmapName", m_YearBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_YearBitmapName=tmpSz;
	}
	m_YearX=GetPrivateProfileInt( "Rainlendar", "YearX", m_YearX, iniFile);
	m_YearY=GetPrivateProfileInt( "Rainlendar", "YearY", m_YearY, iniFile);
	m_YearAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "YearAlign", m_YearAlign, iniFile);
	if(GetPrivateProfileString( "Rainlendar", "YearRasterizer", ConvertRasterizer(m_YearRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_YearRasterizer=ConvertRasterizer(tmpSz);
	}
	else
	{
		m_YearRasterizer=CRasterizer::TYPE_NONE;
	}
	if(GetPrivateProfileString( "Rainlendar", "YearFont", m_YearFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_YearFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "YearFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%X", &m_YearFontColor);
	}
	m_YearSeparation=GetPrivateProfileInt( "Rainlendar", "YearSeparation", m_YearSeparation, iniFile);

	// Event stuff
	m_EventEnable=(1==GetPrivateProfileInt( "Rainlendar", "EventEnable", m_EventEnable?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "EventBitmapName", m_EventBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventBitmapName=tmpSz;
	}
	m_EventAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "EventAlign", m_EventAlign, iniFile);
	m_EventNumOfComponents=GetPrivateProfileInt( "Rainlendar", "EventNumOfComponents", m_EventNumOfComponents, iniFile);
	if(GetPrivateProfileString( "Rainlendar", "EventRasterizer", ConvertRasterizer(m_EventRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventRasterizer=ConvertRasterizer(tmpSz);
	}
	else 
	{
		m_EventRasterizer=CRasterizer::TYPE_NONE;
	}
	if(GetPrivateProfileString( "Rainlendar", "EventFont", m_EventFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "EventFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%X", &m_EventFontColor);
	}
	m_EventInCalendar=(1==GetPrivateProfileInt( "Rainlendar", "EventInCalendar", m_EventInCalendar?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "EventFont2", m_EventFont2.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventFont2=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "EventFontColor2", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%X", &m_EventFontColor2);
	}
	m_EventSeparation=GetPrivateProfileInt( "Rainlendar", "EventSeparation", m_EventSeparation, iniFile);

	// Week numbers stuff
	m_WeekNumbersEnable=(1==GetPrivateProfileInt( "Rainlendar", "WeekNumbersEnable", m_WeekNumbersEnable?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "WeekNumbersBitmapName", m_WeekNumbersBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekNumbersBitmapName=tmpSz;
	}
	m_WeekNumbersAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "WeekNumbersAlign", m_WeekNumbersAlign, iniFile);
	m_WeekNumbersNumOfComponents=GetPrivateProfileInt( "Rainlendar", "WeekNumbersNumOfComponents", m_WeekNumbersNumOfComponents, iniFile);
	if(GetPrivateProfileString( "Rainlendar", "WeekNumbersRasterizer", ConvertRasterizer(m_WeekNumbersRasterizer), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekNumbersRasterizer=ConvertRasterizer(tmpSz);
	}
	else 
	{
		m_WeekNumbersRasterizer=CRasterizer::TYPE_NONE;
	}
	if(GetPrivateProfileString( "Rainlendar", "WeekNumbersFont", m_WeekNumbersFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_WeekNumbersFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "WeekNumbersFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%X", &m_WeekNumbersFontColor);
	}
	m_WeekNumbersSeparation=GetPrivateProfileInt( "Rainlendar", "WeekNumbersSeparation", m_WeekNumbersSeparation, iniFile);

	// ToolTip stuff
	if(GetPrivateProfileString( "Rainlendar", "ToolTipFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%X", &m_ToolTipFontColor);
	}
	if(GetPrivateProfileString( "Rainlendar", "ToolTipBGColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%X", &m_ToolTipBGColor);
	}
	if(GetPrivateProfileString( "Rainlendar", "ToolTipFont", m_ToolTipFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_ToolTipFont=tmpSz;
	}

	SeparateMonths();
}

void CConfig::ReadProfiles(const char* iniFile)
{
	char tmpSz[MAX_LINE_LENGTH];
	char* profiles = new char[MAX_LINE_LENGTH];
	int size = MAX_LINE_LENGTH;

	// Get all the sections
	while(true)
	{
		int res = GetPrivateProfileString( NULL, NULL, NULL, profiles, size, iniFile);
		if (res == 0) return;		// File not found
		if (res != size - 2) break;		// Fits in the buffer

		delete [] profiles;
		size *= 2;
		profiles = new char[size];
	};

	std::list<Profile*>::iterator i = m_Profiles.begin();
	for( ; i != m_Profiles.end(); i++)
	{
		delete (*i);
	}
	m_Profiles.clear();

	char* pos = profiles;
	while(strlen(pos) > 0)
	{
		if (0 == strncmp(pos, "Profile", 7))
		{
			Profile* profile = new Profile;

			if(GetPrivateProfileString(pos, "Name", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				profile->name = tmpSz;
			}

			if(GetPrivateProfileString(pos, "EventFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				sscanf(tmpSz, "%X", &profile->fontColor);
			}
			else
			{
				profile->fontColor = m_EventFontColor;
			}

			if(GetPrivateProfileString(pos, "EventFontColor2", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				sscanf(tmpSz, "%X", &profile->fontColor2);
			}
			else
			{
				profile->fontColor2 = m_EventFontColor2;
			}

			if(GetPrivateProfileString(pos, "ToolTipFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				sscanf(tmpSz, "%X", &profile->toolTipColor);
			}
			else
			{
				profile->toolTipColor = m_ToolTipFontColor;
			}

			if(GetPrivateProfileString(pos, "EventBitmapName", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				profile->bitmapName = tmpSz;
			}
			
			if(GetPrivateProfileString(pos, "EventIconName", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				profile->iconName = tmpSz;
			}
			profile->iconAlign = (CRasterizer::ALIGN)GetPrivateProfileInt(pos, "EventIconAlign", m_DaysAlign, iniFile);

			// Load the images (if they are defined)
			if (!profile->bitmapName.empty())
			{
				AddPath(profile->bitmapName);
				profile->bitmap.Load(profile->bitmapName);
			}

			if (!profile->iconName.empty())
			{
				AddPath(profile->iconName);
				profile->icon.Load(profile->iconName);
			}

			if (!profile->name.empty())
			{
				m_Profiles.push_back(profile);
			}
			else
			{
				delete profile;
			}
		}
		pos = pos + strlen(pos) + 1;
	}

	delete [] profiles;
}

void CConfig::AddPath(std::string& filename)
{
	char buffer[MAX_LINE_LENGTH];

	if(!CRainlendar::GetDummyLitestep()) 
	{
		VarExpansion(buffer, filename.c_str());
		filename = buffer;
	}

	// Check for absolute path
	if(-1 == filename.find(':')) 
	{
		if (!CCalendarWindow::c_Config.GetCurrentSkin().empty())
		{
			filename.insert(0, CCalendarWindow::c_Config.GetCurrentSkin() + "\\");
		}
		filename.insert(0, CCalendarWindow::c_Config.GetSkinsPath());
	}
}

const std::list<Profile*>& CConfig::GetAllProfiles()
{
	return m_Profiles;
}

const Profile* CConfig::GetProfile(const char* name)
{
	if (name == NULL) return NULL;

	std::list<Profile*>::iterator i = m_Profiles.begin();
	for( ; i != m_Profiles.end(); i++)
	{
		if ((*i)->name == name)
		{
			return (*i);
		}
	}
	return NULL;
}

/* 
** WriteConfig
**
** Write the ini-file
**
*/
void CConfig::WriteConfig(WRITE_FLAGS flags)
{
	std::string INIPath;
	char tmpSz[256];

	INIPath = m_Path + "Rainlendar.ini";

	if (CompareIniTime(INIPath))
	{
		if (flags & WRITE_CONFIG)
		{
			// Write only general settings to the Rainlendar.ini
			WritePrivateProfileString( "Rainlendar", "CurrentSkin", m_CurrentSkin.c_str(), INIPath.c_str() );
			WritePrivateProfileString( "Rainlendar", "CurrentSkinIni", m_CurrentSkinIni.c_str(), INIPath.c_str() );
		}
		if (flags & WRITE_POS)
		{
			sprintf(tmpSz, "%i", m_X);
			WritePrivateProfileString( "Rainlendar", "X", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_Y);
			WritePrivateProfileString( "Rainlendar", "Y", tmpSz, INIPath.c_str() );
		}
		if ((flags & WRITE_FULL) == WRITE_FULL)
		{
			sprintf(tmpSz, "%i", m_StartFromMonday);
			WritePrivateProfileString( "Rainlendar", "StartFromMonday", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_StartHidden);
			WritePrivateProfileString( "Rainlendar", "StartHidden", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_PollWallpaper);
			WritePrivateProfileString( "Rainlendar", "PollWallpaper", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_DisableHotkeys);
			WritePrivateProfileString( "Rainlendar", "DisableHotkeys", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_UseWindowName);
			WritePrivateProfileString( "Rainlendar", "UseWindowName", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_Movable);
			WritePrivateProfileString( "Rainlendar", "Movable", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_SnapEdges);
			WritePrivateProfileString( "Rainlendar", "SnapEdges", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_MouseHide);
			WritePrivateProfileString( "Rainlendar", "MouseHide", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_NativeTransparency);
			WritePrivateProfileString( "Rainlendar", "NativeTransparency", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_RefreshDelay);
			WritePrivateProfileString( "Rainlendar", "RefreshDelay", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_WindowPos);
			WritePrivateProfileString( "Rainlendar", "WindowPos", tmpSz, INIPath.c_str() );
			WritePrivateProfileString( "Rainlendar", "EventExecute", m_EventExecute.c_str(), INIPath.c_str() );
			sprintf(tmpSz, "%i", m_EventToolTips);
			WritePrivateProfileString( "Rainlendar", "EventToolTips", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_EventMessageBox);
			WritePrivateProfileString( "Rainlendar", "EventMessageBox", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_BGCopyMode);
			WritePrivateProfileString( "Rainlendar", "BGCopyMode", tmpSz, INIPath.c_str() );
			WritePrivateProfileString( "Rainlendar", "CurrentProfile", m_CurrentProfile.c_str(), INIPath.c_str() );

			sprintf(tmpSz, "%i", m_ServerEnable);
			WritePrivateProfileString( "Rainlendar", "ServerEnable", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ServerPort);
			WritePrivateProfileString( "Rainlendar", "ServerPort", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ServerFrequency);
			WritePrivateProfileString( "Rainlendar", "ServerFrequency", tmpSz, INIPath.c_str() );
			WritePrivateProfileString( "Rainlendar", "ServerAddress", m_ServerAddress.c_str(), INIPath.c_str() );
			WritePrivateProfileString( "Rainlendar", "ServerID", m_ServerID.c_str(), INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ServerStartup);
			WritePrivateProfileString( "Rainlendar", "ServerStartup", tmpSz, INIPath.c_str() );
		}
	}

	if (flags & WRITE_SKIN)
	{
		// These settings are written in the skin file
		INIPath = m_SkinsPath + m_CurrentSkin + "/" + m_CurrentSkinIni;

		// Background stuff
		WritePrivateProfileString( "Rainlendar", "BackgroundBitmapName", m_BackgroundBitmapName.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%i", m_BackgroundMode);
		WritePrivateProfileString( "Rainlendar", "BackgroundMode", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_BackgroundBevel);
		WritePrivateProfileString( "Rainlendar", "BackgroundBevel", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%X", m_BackgroundSolidColor);
		WritePrivateProfileString( "Rainlendar", "BackgroundSolidColor", tmpSz, INIPath.c_str() );

		// Day stuff
		sprintf(tmpSz, "%i", m_DaysEnable);
		WritePrivateProfileString( "Rainlendar", "DaysEnable", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "DaysBitmapName", m_DaysBitmapName.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%i", m_DaysX);
		WritePrivateProfileString( "Rainlendar", "DaysX", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_DaysY);
		WritePrivateProfileString( "Rainlendar", "DaysY", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_DaysW);
		WritePrivateProfileString( "Rainlendar", "DaysW", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_DaysH);
		WritePrivateProfileString( "Rainlendar", "DaysH", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_DaysAlign);
		WritePrivateProfileString( "Rainlendar", "DaysAlign", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_DaysNumOfComponents);
		WritePrivateProfileString( "Rainlendar", "DaysNumOfComponents", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "DaysRasterizer", ConvertRasterizer(m_DaysRasterizer), INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "DaysFont", m_DaysFont.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%X", m_DaysFontColor);
		WritePrivateProfileString( "Rainlendar", "DaysFontColor", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_DaysIgnoreToday);
		WritePrivateProfileString( "Rainlendar", "DaysIgnoreToday", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_DaysIgnoreEvent);
		WritePrivateProfileString( "Rainlendar", "DaysIgnoreEvent", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_DaysSeparation);
		WritePrivateProfileString( "Rainlendar", "DaysSeparation", tmpSz, INIPath.c_str() );

		// Today stuff
		sprintf(tmpSz, "%i", m_TodayEnable);
		WritePrivateProfileString( "Rainlendar", "TodayEnable", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "TodayBitmapName", m_TodayBitmapName.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%i", m_TodayAlign);
		WritePrivateProfileString( "Rainlendar", "TodayAlign", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_TodayNumOfComponents);
		WritePrivateProfileString( "Rainlendar", "TodayNumOfComponents", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "TodayRasterizer", ConvertRasterizer(m_TodayRasterizer), INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "TodayFont", m_TodayFont.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%X", m_TodayFontColor);
		WritePrivateProfileString( "Rainlendar", "TodayFontColor", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_TodaySeparation);
		WritePrivateProfileString( "Rainlendar", "TodaySeparation", tmpSz, INIPath.c_str() );

		// Weekday stuff
		sprintf(tmpSz, "%i", m_WeekdaysEnable);
		WritePrivateProfileString( "Rainlendar", "WeekdaysEnable", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "WeekdaysBitmapName", m_WeekdaysBitmapName.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%i", m_WeekdaysAlign);
		WritePrivateProfileString( "Rainlendar", "WeekdaysAlign", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "WeekdaysRasterizer", ConvertRasterizer(m_WeekdaysRasterizer), INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "WeekdaysFont", m_WeekdaysFont.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%X", m_WeekdaysFontColor);
		WritePrivateProfileString( "Rainlendar", "WeekdaysFontColor", tmpSz, INIPath.c_str() );

		// Month stuff
		sprintf(tmpSz, "%i", m_MonthEnable);
		WritePrivateProfileString( "Rainlendar", "MonthEnable", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "MonthBitmapName", m_MonthBitmapName.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%i", m_MonthX);
		WritePrivateProfileString( "Rainlendar", "MonthX", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_MonthY);
		WritePrivateProfileString( "Rainlendar", "MonthY", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_MonthAlign);
		WritePrivateProfileString( "Rainlendar", "MonthAlign", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "MonthRasterizer", ConvertRasterizer(m_MonthRasterizer), INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "MonthFont", m_MonthFont.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%X", m_MonthFontColor);
		WritePrivateProfileString( "Rainlendar", "MonthFontColor", tmpSz, INIPath.c_str() );

		// Year stuff
		sprintf(tmpSz, "%i", m_YearEnable);
		WritePrivateProfileString( "Rainlendar", "YearEnable", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "YearBitmapName", m_YearBitmapName.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%i", m_YearX);
		WritePrivateProfileString( "Rainlendar", "YearX", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_YearY);
		WritePrivateProfileString( "Rainlendar", "YearY", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_YearAlign);
		WritePrivateProfileString( "Rainlendar", "YearAlign", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "YearRasterizer", ConvertRasterizer(m_YearRasterizer), INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "YearFont", m_YearFont.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%X", m_YearFontColor);
		WritePrivateProfileString( "Rainlendar", "YearFontColor", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_YearSeparation);
		WritePrivateProfileString( "Rainlendar", "YearSeparation", tmpSz, INIPath.c_str() );

		// Event stuff
		sprintf(tmpSz, "%i", m_EventEnable);
		WritePrivateProfileString( "Rainlendar", "EventEnable", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "EventBitmapName", m_EventBitmapName.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%i", m_EventAlign);
		WritePrivateProfileString( "Rainlendar", "EventAlign", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_EventNumOfComponents);
		WritePrivateProfileString( "Rainlendar", "EventNumOfComponents", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "EventRasterizer", ConvertRasterizer(m_EventRasterizer), INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "EventFont", m_EventFont.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%X", m_EventFontColor);
		WritePrivateProfileString( "Rainlendar", "EventFontColor", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_EventInCalendar);
		WritePrivateProfileString( "Rainlendar", "EventInCalendar", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "EventFont2", m_EventFont2.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%X", m_EventFontColor2);
		WritePrivateProfileString( "Rainlendar", "EventFontColor2", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_EventSeparation);
		WritePrivateProfileString( "Rainlendar", "EventSeparation", tmpSz, INIPath.c_str() );

		// Week numbers stuff
		sprintf(tmpSz, "%i", m_WeekNumbersEnable);
		WritePrivateProfileString( "Rainlendar", "WeekNumbersEnable", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "WeekNumbersBitmapName", m_WeekNumbersBitmapName.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%i", m_WeekNumbersAlign);
		WritePrivateProfileString( "Rainlendar", "WeekNumbersAlign", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "WeekNumbersRasterizer", ConvertRasterizer(m_WeekNumbersRasterizer), INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "WeekNumbersFont", m_WeekNumbersFont.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%X", m_WeekNumbersFontColor);
		WritePrivateProfileString( "Rainlendar", "WeekNumbersFontColor", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_WeekNumbersNumOfComponents);
		WritePrivateProfileString( "Rainlendar", "WeekNumbersNumOfComponents", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_WeekNumbersSeparation);
		WritePrivateProfileString( "Rainlendar", "WeekNumbersSeparation", tmpSz, INIPath.c_str() );

		sprintf(tmpSz, "%X", m_ToolTipFontColor);
		WritePrivateProfileString( "Rainlendar", "ToolTipFontColor", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%X", m_ToolTipBGColor);
		WritePrivateProfileString( "Rainlendar", "ToolTipBGColor", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "ToolTipFont", m_ToolTipFont.c_str(), INIPath.c_str() );
	}

	WritePrivateProfileString( NULL, NULL, NULL, INIPath.c_str() );	// FLUSH

	GetIniTime(INIPath);
}


/* 
** ConvertRasterizer
**
** returns the type of the rasterizer form given string.
**
*/
CRasterizer::TYPE CConfig::ConvertRasterizer(const char* String) 
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
const char* CConfig::ConvertRasterizer(CRasterizer::TYPE Type) 
{
	switch(Type) {
		case CRasterizer::TYPE_FONT:
			return "FONT";
		case CRasterizer::TYPE_BITMAP:
			return "BITMAP";
	}
	return "";
}

void CConfig::SeparateMonths() 
{
	std::string monthNames = m_MonthNames;
	std::string name = "none";

	for(int i = 0; i < 12; i++) 
	{
		int pos = monthNames.find('/');
		if (pos != -1) 
		{
			name.assign(monthNames.begin(), monthNames.begin() + pos);
			monthNames.erase(monthNames.begin(), monthNames.begin() + pos + 1);
		}
		else
		{
			name = monthNames;
		}
		m_MonthName.push_back(name);
	}
}
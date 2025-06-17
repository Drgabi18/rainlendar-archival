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
  $Header: /home/cvsroot/Rainlendar/Plugin/Config.cpp,v 1.25 2004/04/24 11:17:36 rainy Exp $

  $Log: Config.cpp,v $
  Revision 1.25  2004/04/24 11:17:36  rainy
  Added outlook profiles.

  Revision 1.24  2004/01/28 18:04:55  rainy
  Added tray executes.

  Revision 1.23  2004/01/25 09:59:27  rainy
  Added new dialog position.

  Revision 1.22  2004/01/10 15:16:36  rainy
  Added tray and hotkey for todo.

  Revision 1.21  2003/12/20 22:24:39  rainy
  Added stuff for the message box.

  Revision 1.20  2003/12/05 15:45:42  Rainy
  Added Reset()

  Revision 1.19  2003/10/27 17:36:51  Rainy
  Config is now singleton.
  Added todo stuff.

  Revision 1.18  2003/10/04 14:48:25  Rainy
  Added TooltipMaxWidth and priority for the profiles.

  Revision 1.17  2003/08/09 16:38:43  Rainy
  Added hotkeys and few other settings.

  Revision 1.16  2003/06/15 09:43:01  Rainy
  Added Layout stuff.

  Revision 1.15  2003/05/25 18:08:44  Rainy
  Added tooltip separator.

  Revision 1.14  2003/05/07 19:15:36  rainy
  Added few new options.

  Revision 1.13  2003/03/22 20:31:01  rainy
  Refresh on resolution change is optional.

  Revision 1.12  2002/11/25 17:11:35  rainy
  Added some stuff to the profiles.

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

CConfig CConfig::c_Config;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfig::CConfig()
{
	Reset();
}

void CConfig::Reset()
{
	m_X=0;
	m_Y=0;
	m_StartFromMonday=true;
	m_StartHidden=false;
	m_DisableHotkeys=false;
	m_UseWindowName=false;
	m_PollWallpaper=false;
	m_Movable=true;
	m_MouseHide=false;
	m_SnapEdges=true;
	m_NativeTransparency=true;
	m_RefreshOnResolutionChange=false;
	m_RefreshDelay=100;
	m_WindowPos=WINDOWPOS_NORMAL;
	m_CurrentProfile="Default";

	m_CurrentLanguage="";
	m_CurrentSkin="Default";
	m_CurrentSkinIni="Skin.ini";
    
	m_ShowOutlookAppointments=false;
    m_Week1HasJanuary1st=false;
    m_NegativeCoords=true;
	m_OutlookUpdate=0;
	m_MonthNames = "";
	m_WeekdayNames = "";
	m_EventExecute = "";
	m_GetOutlookAppointmentsAtStartup = true;
	m_ShowTrayIcon = true;

	m_TrayExecuteL = "";
	m_TrayExecuteR = "";
	m_TrayExecuteM = "";
	m_TrayExecuteDL = "!RainlendarToggle";
	m_TrayExecuteDR = "";
	m_TrayExecuteDM = "";
	
	m_OutlookProfile = "Outlook";
	m_OutlookLabels = false;
	m_OutlookKeepAlive = false;

	m_MessageBoxMaxWidth = 0;
	m_ShowAllEvents = true;
	m_ShowSingleEvent = true;
	m_SnoozeTime = 60;
	m_PreshowTime = 0;
	m_ToolTipMaxWidth = 0;

	m_VerticalCount = 1;
	m_HorizontalCount = 1;
	m_PreviousMonths = 0;
	m_StartFromJanuary = false;
	m_RememberDialogPositions=false;

	m_BackgroundBitmapName = "";
	m_BackgroundMode=MODE_TILE;
	m_BackgroundSolidColor=GetSysColor(COLOR_3DFACE);
	m_BackgroundBevel=true;

	m_DaysEnable=false;
	m_DaysX=0;
	m_DaysY=0;
	m_DaysW=100;
	m_DaysH=100;
	m_DaysBitmapName = "";
	m_DaysNumOfComponents=1;
	m_DaysAlign=CRasterizer::ALIGN_LEFT;
	m_DaysRasterizer=CRasterizer::TYPE_NONE;
	m_DaysFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_DaysFontColor=0;
	m_DaysIgnoreToday=false;
	m_DaysIgnoreEvent=false;
	m_DaysSeparation=0;

	m_TodayEnable=false;
	m_TodayBitmapName = "";
	m_TodayAlign=CRasterizer::ALIGN_LEFT;
	m_TodayNumOfComponents=1;
	m_TodayRasterizer=CRasterizer::TYPE_NONE;
	m_TodayFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_TodayFontColor=0;
	m_TodaySeparation=0;

	m_WeekdaysEnable=false;
	m_WeekdaysBitmapName = "";
	m_WeekdaysAlign=CRasterizer::ALIGN_LEFT;
	m_WeekdaysRasterizer=CRasterizer::TYPE_NONE;
	m_WeekdaysFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_WeekdaysFontColor=0;
	m_WeekdayNames = "SUN/MON/TUE/WED/THU/FRI/SAT";

	m_MonthEnable=false;
	m_MonthBitmapName = "";
	m_MonthX=0;
	m_MonthY=0;
	m_MonthAlign=CRasterizer::ALIGN_LEFT;
	m_MonthRasterizer=CRasterizer::TYPE_NONE;
	m_MonthFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_MonthFontColor=0;
	m_MonthNames="January/February/March/April/May/June/July/August/September/October/November/December";

	m_YearEnable=false;
	m_YearBitmapName = "";
	m_YearX=0;
	m_YearY=0;
	m_YearAlign=CRasterizer::ALIGN_LEFT;
	m_YearRasterizer=CRasterizer::TYPE_NONE;
	m_YearFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_YearFontColor=0;
	m_YearSeparation=0;

	m_EventEnable=false;
	m_EventBitmapName = "";
	m_EventAlign=CRasterizer::ALIGN_LEFT;
	m_EventNumOfComponents=1;
	m_EventToolTips=true;
	m_EventInCalendar=false;
	m_EventRasterizer=CRasterizer::TYPE_NONE;
	m_EventFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_EventFont2="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_EventFontColor=0;
	m_EventFontColor2=0;
	m_EventSeparation=0;
		
	m_WeekNumbersEnable = false;
	m_WeekNumbersBitmapName = "";
	m_WeekNumbersAlign = CRasterizer::ALIGN_LEFT;
	m_WeekNumbersNumOfComponents = 10;
	m_WeekNumbersRasterizer=CRasterizer::TYPE_NONE;
	m_WeekNumbersFont="-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_WeekNumbersFontColor=0;
	m_WeekNumbersSeparation=0;

	m_TodoEnable = false;
	m_TodoBitmapName = "";
	m_TodoItemBitmapName = "";
	m_TodoX = 0;
	m_TodoY = 0;
	m_TodoW = 200;
	m_TodoFontColor = GetSysColor(COLOR_BTNTEXT);
	m_TodoFont = "-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_TodoBitmapMargins.left = m_TodoBitmapMargins.top = m_TodoBitmapMargins.right = m_TodoBitmapMargins.bottom = 0;
	m_TodoTextMargins.left = m_TodoTextMargins.top = m_TodoTextMargins.right = m_TodoTextMargins.bottom = 8;
	m_TodoSeparation = 5;
	m_TodoItemOffset.x = m_TodoItemOffset.y = 0;
	m_TodoItemAlign = CRasterizer::ALIGN_LEFT;

	m_MessageBoxBitmapName = "";
	m_MessageBoxBitmapMargins.left = m_MessageBoxBitmapMargins.top = m_MessageBoxBitmapMargins.right = m_MessageBoxBitmapMargins.bottom = 0;
	m_MessageBoxTextMargins.left = m_MessageBoxTextMargins.top = m_MessageBoxTextMargins.right = m_MessageBoxTextMargins.bottom = 8;
	m_MessageBoxFontColor = 0;
	m_MessageBoxFont = "-17/0/0/0/800/0/0/0/0/3/2/1/34/Arial";
	m_MessageBoxSeparation = 5;

	m_ServerEnable = false;
	m_ServerPort = 0;
	m_ServerFrequency = 60;		// every hour
	m_ServerStartup = false;
	m_ServerSyncOnEdit = false;
	m_ServerID = "";
	m_ServerAddress = "";

	m_ToolTipFontColor = GetSysColor(COLOR_INFOTEXT);
	m_ToolTipBGColor = GetSysColor(COLOR_INFOBK);
	m_ToolTipFont = "-11/0/0/0/400/0/0/0/0/3/2/1/34/Arial";
	m_TooltipSeparator = true;

	m_HideHotkey = 0;
	m_ShowHotkey = 0;
	m_ToggleHotkey = 0;
	m_ActivateHotkey = 0;
	m_RefreshHotkey = 0;
	m_ConfigHotkey = 0;
	m_SkinHotkey = 0;
	m_NextHotkey = 0;
	m_PreviousHotkey = 0;
	m_NextYearHotkey = 0;
	m_PreviousYearHotkey = 0;
	m_CurrentHotkey = 0;
	m_AllHotkey = 0;
	m_OutlookHotkey = 0;
	m_TodoHotkey = 0;

	for (int i = 0; i < DIALOG_LAST; i++)
	{
		m_DialogPos[i].x = 0;
		m_DialogPos[i].y = 0;
	}
}

CConfig::~CConfig()
{
	std::list<Profile*>::iterator i = m_Profiles.begin();
	for( ; i != m_Profiles.end(); i++)
	{
		delete (*i);
	}
}

POINT CConfig::GetDialogPosition(DIALOG_TYPE type)
{
	return m_DialogPos[type];
}

void CConfig::SetDialogPosition(DIALOG_TYPE type, int x, int y)
{
	m_DialogPos[type].x = x;
	m_DialogPos[type].y = y;
}

UINT CConfig::GetPreviousMonths()
{
	if (m_StartFromJanuary)
	{
		// Return the 'distance' to January
		return CCalendarWindow::c_TodaysDate.wMonth - 1;
	}

	return m_PreviousMonths;
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

	Reset();

	GetIniTime(INIPath);

	ReadGeneralConfig(INIPath.c_str());

	// Read the rest of the settings from the skin 
	if (!m_CurrentSkin.empty() && !m_CurrentSkinIni.empty())
	{
		INIPath = m_SkinsPath + m_CurrentSkin + "/" + m_CurrentSkinIni;
		ReadSkinConfig(INIPath.c_str());
		ReadProfiles(INIPath.c_str());
	}

	// DEBUG: Display some of the key settings
	DebugLog("Hide=%i, Move=%i, Negative=%i, Hidden=%i, WinName=%i", m_MouseHide, m_Movable, m_NegativeCoords, m_StartHidden, m_UseWindowName);
	DebugLog("Poll=%i, Transparency=%i, ResChange=%i, Outlook=%i, WinPos=%i", m_PollWallpaper, m_NativeTransparency, m_RefreshOnResolutionChange, m_ShowOutlookAppointments, m_WindowPos);
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
	m_RefreshOnResolutionChange=(1==GetPrivateProfileInt( "Rainlendar", "RefreshOnResolutionChange", m_RefreshOnResolutionChange?1:0, iniFile))?true:false;
	m_ShowOutlookAppointments=(1==GetPrivateProfileInt( "Rainlendar", "ShowOutlookAppointments", m_ShowOutlookAppointments?1:0, iniFile))?true:false;
	m_Week1HasJanuary1st=(1==GetPrivateProfileInt( "Rainlendar", "Week1HasJanuary1st", m_Week1HasJanuary1st?1:0, iniFile))?true:false;
	m_TooltipSeparator=(1==GetPrivateProfileInt( "Rainlendar", "TooltipSeparator", m_TooltipSeparator?1:0, iniFile))?true:false;
	m_RefreshDelay=GetPrivateProfileInt( "Rainlendar", "RefreshDelay", m_RefreshDelay, iniFile);
	m_WindowPos=(WINDOWPOS)GetPrivateProfileInt( "Rainlendar", "WindowPos", m_WindowPos, iniFile);
	m_OutlookUpdate=GetPrivateProfileInt( "Rainlendar", "OutlookUpdate", m_OutlookUpdate, iniFile);
	m_RememberDialogPositions=(1==GetPrivateProfileInt( "Rainlendar", "RememberDialogPositions", m_RememberDialogPositions?1:0, iniFile))?true:false;
	m_NegativeCoords=(1==GetPrivateProfileInt( "Rainlendar", "NegativeCoords", m_NegativeCoords?1:0, iniFile))?true:false;
	m_GetOutlookAppointmentsAtStartup=(1==GetPrivateProfileInt( "Rainlendar", "GetOutlookAppointmentsAtStartup", m_GetOutlookAppointmentsAtStartup?1:0, iniFile))?true:false;
	m_ShowTrayIcon=(1==GetPrivateProfileInt( "Rainlendar", "ShowTrayIcon", m_ShowTrayIcon?1:0, iniFile))?true:false;

	if(GetPrivateProfileString( "Rainlendar", "TrayExecuteL", m_TrayExecuteL.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TrayExecuteL=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "TrayExecuteR", m_TrayExecuteR.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TrayExecuteR=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "TrayExecuteM", m_TrayExecuteM.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TrayExecuteM=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "TrayExecuteDL", m_TrayExecuteDL.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TrayExecuteDL=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "TrayExecuteDR", m_TrayExecuteDR.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TrayExecuteDR=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "TrayExecuteDM", m_TrayExecuteDM.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TrayExecuteDM=tmpSz;
	}

	if(GetPrivateProfileString( "Rainlendar", "OutlookProfile", m_OutlookProfile.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_OutlookProfile=tmpSz;
	}
	m_OutlookLabels=(1==GetPrivateProfileInt( "Rainlendar", "OutlookLabels", m_OutlookLabels?1:0, iniFile))?true:false;
	m_OutlookKeepAlive=(1==GetPrivateProfileInt( "Rainlendar", "OutlookKeepAlive", m_OutlookKeepAlive?1:0, iniFile))?true:false;

	m_VerticalCount=GetPrivateProfileInt( "Rainlendar", "VerticalCount", m_VerticalCount, iniFile);
	m_HorizontalCount=GetPrivateProfileInt( "Rainlendar", "HorizontalCount", m_HorizontalCount, iniFile);
	m_PreviousMonths=GetPrivateProfileInt( "Rainlendar", "PreviousMonths", m_PreviousMonths, iniFile);
	m_StartFromJanuary=(1==GetPrivateProfileInt( "Rainlendar", "StartFromJanuary", m_StartFromJanuary?1:0, iniFile))?true:false;

	m_HideHotkey=GetPrivateProfileInt( "Rainlendar", "HideHotkey", m_HideHotkey, iniFile);
	m_ShowHotkey=GetPrivateProfileInt( "Rainlendar", "ShowHotkey", m_ShowHotkey, iniFile);
	m_ToggleHotkey=GetPrivateProfileInt( "Rainlendar", "ToggleHotkey", m_ToggleHotkey, iniFile);
	m_ActivateHotkey=GetPrivateProfileInt( "Rainlendar", "ActivateHotkey", m_ActivateHotkey, iniFile);
	m_RefreshHotkey=GetPrivateProfileInt( "Rainlendar", "RefreshHotkey", m_RefreshHotkey, iniFile);
	m_ConfigHotkey=GetPrivateProfileInt( "Rainlendar", "ConfigHotkey", m_ConfigHotkey, iniFile);
	m_SkinHotkey=GetPrivateProfileInt( "Rainlendar", "SkinHotkey", m_SkinHotkey, iniFile);
	m_NextHotkey=GetPrivateProfileInt( "Rainlendar", "NextHotkey", m_NextHotkey, iniFile);
	m_PreviousHotkey=GetPrivateProfileInt( "Rainlendar", "PreviousHotkey", m_PreviousHotkey, iniFile);
	m_NextYearHotkey=GetPrivateProfileInt( "Rainlendar", "NextYearHotkey", m_NextYearHotkey, iniFile);
	m_PreviousYearHotkey=GetPrivateProfileInt( "Rainlendar", "PreviousYearHotkey", m_PreviousYearHotkey, iniFile);
	m_CurrentHotkey=GetPrivateProfileInt( "Rainlendar", "CurrentHotkey", m_CurrentHotkey, iniFile);
	m_AllHotkey=GetPrivateProfileInt( "Rainlendar", "AllHotkey", m_AllHotkey, iniFile);
	m_OutlookHotkey=GetPrivateProfileInt( "Rainlendar", "OutlookHotkey", m_OutlookHotkey, iniFile);
	m_TodoHotkey=GetPrivateProfileInt( "Rainlendar", "TodoHotkey", m_TodoHotkey, iniFile);

	if(GetPrivateProfileString( "Rainlendar", "CurrentProfile", m_CurrentProfile.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_CurrentProfile=tmpSz;
	}

	if(GetPrivateProfileString( "Rainlendar", "EventExecute", m_EventExecute.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventExecute=tmpSz;
	}
	m_EventToolTips=(1==GetPrivateProfileInt( "Rainlendar", "EventToolTips", m_EventToolTips?1:0, iniFile))?true:false;

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
	m_ServerSyncOnEdit=(1==GetPrivateProfileInt( "Rainlendar", "ServerSyncOnEdit", m_ServerSyncOnEdit, iniFile))?true:false;

	if(GetPrivateProfileString( "Rainlendar", "CurrentSkin", m_CurrentSkin.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_CurrentSkin=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "CurrentSkinIni", m_CurrentSkinIni.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_CurrentSkinIni=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "CurrentLanguage", m_CurrentLanguage.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_CurrentLanguage=tmpSz;
	}
	m_ToolTipMaxWidth=GetPrivateProfileInt( "Rainlendar", "ToolTipMaxWidth", m_ToolTipMaxWidth, iniFile);

	m_TodoEnable=(1==GetPrivateProfileInt( "Rainlendar", "TodoEnable", m_TodoEnable?1:0, iniFile))?true:false;
	m_TodoX=GetPrivateProfileInt( "Rainlendar", "TodoX", m_TodoX, iniFile);
	m_TodoY=GetPrivateProfileInt( "Rainlendar", "TodoY", m_TodoY, iniFile);

	m_MessageBoxMaxWidth=GetPrivateProfileInt( "Rainlendar", "MessageBoxMaxWidth", m_MessageBoxMaxWidth, iniFile);
	m_SnoozeTime=GetPrivateProfileInt( "Rainlendar", "SnoozeTime", m_SnoozeTime, iniFile);
	m_PreshowTime=GetPrivateProfileInt( "Rainlendar", "PreshowTime", m_PreshowTime, iniFile);
	m_ShowAllEvents=(1==GetPrivateProfileInt( "Rainlendar", "ShowAllEvents", m_ShowAllEvents?1:0, iniFile))?true:false;
	m_ShowSingleEvent=(1==GetPrivateProfileInt( "Rainlendar", "ShowSingleEvent", m_ShowSingleEvent?1:0, iniFile))?true:false;

	// Read dialog positions
	for (int i = 0; i < DIALOG_LAST; i++)
	{
		char title[256];
		sprintf(title, "DialogPos%i", i);
		if(GetPrivateProfileString( "Rainlendar", title, NULL, tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
		{
			sscanf(tmpSz, "%i,%i", &(m_DialogPos[i].x), &(m_DialogPos[i].y));
		}
	}
}

void CConfig::ReadSkinConfig(const char* iniFile)
{
	char tmpSz[MAX_LINE_LENGTH];

	if(GetPrivateProfileString( "Rainlendar", "BackgroundBitmapName", m_BackgroundBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_BackgroundBitmapName=tmpSz;
	}
	m_BackgroundMode=(BACKGROUND_MODE)GetPrivateProfileInt( "Rainlendar", "BackgroundMode", m_BackgroundMode, iniFile);
	m_BackgroundBevel=(1==GetPrivateProfileInt( "Rainlendar", "BackgroundBevel", m_BackgroundBevel?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "BackgroundSolidColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_BackgroundSolidColor = ParseColor(tmpSz);
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
		m_DaysFontColor = ParseColor(tmpSz);
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
		m_TodayFontColor = ParseColor(tmpSz);
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
		m_WeekdaysFontColor = ParseColor(tmpSz);
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
		m_MonthFontColor = ParseColor(tmpSz);
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
		m_YearFontColor = ParseColor(tmpSz);
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
		m_EventFontColor = ParseColor(tmpSz);
	}
	m_EventInCalendar=(1==GetPrivateProfileInt( "Rainlendar", "EventInCalendar", m_EventInCalendar?1:0, iniFile))?true:false;
	if(GetPrivateProfileString( "Rainlendar", "EventFont2", m_EventFont2.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventFont2=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "EventFontColor2", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventFontColor2 = ParseColor(tmpSz);
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
		m_WeekNumbersFontColor = ParseColor(tmpSz);
	}
	m_WeekNumbersSeparation=GetPrivateProfileInt( "Rainlendar", "WeekNumbersSeparation", m_WeekNumbersSeparation, iniFile);

	// ToolTip stuff
	if(GetPrivateProfileString( "Rainlendar", "ToolTipFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_ToolTipFontColor = ParseColor(tmpSz);
	}
	if(GetPrivateProfileString( "Rainlendar", "ToolTipBGColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_ToolTipBGColor = ParseColor(tmpSz);
	}
	if(GetPrivateProfileString( "Rainlendar", "ToolTipFont", m_ToolTipFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_ToolTipFont=tmpSz;
	}

	// Todo stuff
	if(GetPrivateProfileString( "Rainlendar", "TodoBitmapName", m_TodoBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodoBitmapName=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "TodoBitmapMargins", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i, %i, %i", &m_TodoBitmapMargins.left, &m_TodoBitmapMargins.top, &m_TodoBitmapMargins.right, &m_TodoBitmapMargins.bottom);
	}
	if(GetPrivateProfileString( "Rainlendar", "TodoTextMargins", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i, %i, %i", &m_TodoTextMargins.left, &m_TodoTextMargins.top, &m_TodoTextMargins.right, &m_TodoTextMargins.bottom);
	}
	m_TodoW=GetPrivateProfileInt( "Rainlendar", "TodoW", m_TodoW, iniFile);
	if(GetPrivateProfileString( "Rainlendar", "TodoFont", m_TodoFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodoFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "TodoFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodoFontColor = ParseColor(tmpSz);
	}
	m_TodoItemAlign=(CRasterizer::ALIGN)GetPrivateProfileInt( "Rainlendar", "TodoItemAlign", m_TodoItemAlign, iniFile);
	if(GetPrivateProfileString( "Rainlendar", "TodoItemOffset", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i", &m_TodoItemOffset.x, &m_TodoItemOffset.y);
	}
	m_TodoSeparation=GetPrivateProfileInt( "Rainlendar", "TodoSeparation", m_TodoSeparation, iniFile);
	if(GetPrivateProfileString( "Rainlendar", "TodoItemBitmapName", m_TodoItemBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_TodoItemBitmapName=tmpSz;
	}

	// MessageBox stuff
	if(GetPrivateProfileString( "Rainlendar", "MessageBoxBitmapName", m_MessageBoxBitmapName.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MessageBoxBitmapName=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "MessageBoxBitmapMargins", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i, %i, %i", &m_MessageBoxBitmapMargins.left, &m_MessageBoxBitmapMargins.top, &m_MessageBoxBitmapMargins.right, &m_MessageBoxBitmapMargins.bottom);
	}
	if(GetPrivateProfileString( "Rainlendar", "MessageBoxTextMargins", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		sscanf(tmpSz, "%i, %i, %i, %i", &m_MessageBoxTextMargins.left, &m_MessageBoxTextMargins.top, &m_MessageBoxTextMargins.right, &m_MessageBoxTextMargins.bottom);
	}
	if(GetPrivateProfileString( "Rainlendar", "MessageBoxFont", m_MessageBoxFont.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MessageBoxFont=tmpSz;
	}
	if(GetPrivateProfileString( "Rainlendar", "MessageBoxFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_MessageBoxFontColor = ParseColor(tmpSz);
	}
	m_MessageBoxSeparation=GetPrivateProfileInt( "Rainlendar", "MessageBoxSeparation", m_MessageBoxSeparation, iniFile);


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
				profile->fontColor = ParseColor(tmpSz);
			}
			else
			{
				profile->fontColor = m_EventFontColor;
			}

			if(GetPrivateProfileString(pos, "EventFontColor2", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				profile->fontColor2 = ParseColor(tmpSz);
			}
			else
			{
				profile->fontColor2 = m_EventFontColor2;
			}

			if(GetPrivateProfileString(pos, "ToolTipFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				profile->toolTipColor = ParseColor(tmpSz);
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

			profile->drawAlways = GetPrivateProfileInt(pos, "EventDrawAlways", 0, iniFile) ? true : false;

			profile->priority = GetPrivateProfileInt(pos, "Priority", 0, iniFile);

			// Load the images (if they are defined)
			if (!profile->bitmapName.empty())
			{
				AddPath(profile->bitmapName);
				if (!profile->bitmap.Load(profile->bitmapName))
				{
					sprintf(tmpSz, CCalendarWindow::c_Language.GetString("Error", 8), profile->bitmapName.c_str());
					MessageBox(NULL, tmpSz, "Rainlendar", MB_OK | MB_ICONERROR);
				}
			}

			if (!profile->iconName.empty())
			{
				AddPath(profile->iconName);
				if (!profile->icon.Load(profile->iconName))
				{
					sprintf(tmpSz, CCalendarWindow::c_Language.GetString("Error", 8), profile->iconName.c_str());
					MessageBox(NULL, tmpSz, "Rainlendar", MB_OK | MB_ICONERROR);
				}
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

COLORREF CConfig::ParseColor(const char* color)
{
	char buffer[MAX_LINE_LENGTH];
	COLORREF col;

	if (!CRainlendar::GetDummyLitestep()) 
	{
		VarExpansion(buffer, color);
		sscanf(buffer, "%X", &col);
	}
	else
	{
		sscanf(color, "%X", &col);
	}

	return col;
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
		if (!CConfig::Instance().GetCurrentSkin().empty())
		{
			filename.insert(0, CConfig::Instance().GetCurrentSkin() + "\\");
		}
		filename.insert(0, CConfig::Instance().GetSkinsPath());
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
			WritePrivateProfileString( "Rainlendar", "CurrentLanguage", m_CurrentLanguage.c_str(), INIPath.c_str() );
		}
		if (flags & WRITE_POS)
		{
			sprintf(tmpSz, "%i", m_X);
			WritePrivateProfileString( "Rainlendar", "X", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_Y);
			WritePrivateProfileString( "Rainlendar", "Y", tmpSz, INIPath.c_str() );
		}
		if (flags & WRITE_TODOPOS)
		{
			sprintf(tmpSz, "%i", m_TodoX);
			WritePrivateProfileString( "Rainlendar", "TodoX", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_TodoY);
			WritePrivateProfileString( "Rainlendar", "TodoY", tmpSz, INIPath.c_str() );
		}
		if (flags & WRITE_PROFILE)
		{
			WritePrivateProfileString( "Rainlendar", "CurrentProfile", m_CurrentProfile.c_str(), INIPath.c_str() );
		}
		if (flags & WRITE_DIALOG_POS)
		{
			// Write dialog positions
			for (int i = 0; i < DIALOG_LAST; i++)
			{
				char title[256];
				sprintf(title, "DialogPos%i", i);
				sprintf(tmpSz, "%i,%i", m_DialogPos[i].x, m_DialogPos[i].y);
				WritePrivateProfileString( "Rainlendar", title, tmpSz, INIPath.c_str() );
			}
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
			sprintf(tmpSz, "%i", m_RefreshOnResolutionChange);
			WritePrivateProfileString( "Rainlendar", "RefreshOnResolutionChange", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_NegativeCoords);
			WritePrivateProfileString( "Rainlendar", "NegativeCoords", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ShowOutlookAppointments);
			WritePrivateProfileString( "Rainlendar", "ShowOutlookAppointments", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_Week1HasJanuary1st);
			WritePrivateProfileString( "Rainlendar", "Week1HasJanuary1st", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_TooltipSeparator);
			WritePrivateProfileString( "Rainlendar", "TooltipSeparator", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_RefreshDelay);
			WritePrivateProfileString( "Rainlendar", "RefreshDelay", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_WindowPos);
			WritePrivateProfileString( "Rainlendar", "WindowPos", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ShowTrayIcon);
			WritePrivateProfileString( "Rainlendar", "ShowTrayIcon", tmpSz, INIPath.c_str() );
			WritePrivateProfileString( "Rainlendar", "EventExecute", m_EventExecute.c_str(), INIPath.c_str() );
			sprintf(tmpSz, "%i", m_EventToolTips);
			WritePrivateProfileString( "Rainlendar", "EventToolTips", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_OutlookUpdate);
			WritePrivateProfileString( "Rainlendar", "OutlookUpdate", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_GetOutlookAppointmentsAtStartup);
			WritePrivateProfileString( "Rainlendar", "GetOutlookAppointmentsAtStartup", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ToolTipMaxWidth);
			WritePrivateProfileString( "Rainlendar", "ToolTipMaxWidth", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_MessageBoxMaxWidth);
			WritePrivateProfileString( "Rainlendar", "MessageBoxMaxWidth", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ShowAllEvents);
			WritePrivateProfileString( "Rainlendar", "ShowAllEvents", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ShowSingleEvent);
			WritePrivateProfileString( "Rainlendar", "ShowSingleEvent", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_PreshowTime);
			WritePrivateProfileString( "Rainlendar", "PreshowTime", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_SnoozeTime);
			WritePrivateProfileString( "Rainlendar", "SnoozeTime", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_TodoEnable);
			WritePrivateProfileString( "Rainlendar", "TodoEnable", tmpSz, INIPath.c_str() );

			WritePrivateProfileString( "Rainlendar", "OutlookProfile", m_OutlookProfile.c_str(), INIPath.c_str() );
			sprintf(tmpSz, "%i", m_OutlookLabels);
			WritePrivateProfileString( "Rainlendar", "OutlookLabels", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_OutlookKeepAlive);
			WritePrivateProfileString( "Rainlendar", "OutlookKeepAlive", tmpSz, INIPath.c_str() );

			sprintf(tmpSz, "%i", m_VerticalCount);
			WritePrivateProfileString( "Rainlendar", "VerticalCount", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_HorizontalCount);
			WritePrivateProfileString( "Rainlendar", "HorizontalCount", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_PreviousMonths);
			WritePrivateProfileString( "Rainlendar", "PreviousMonths", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_StartFromJanuary);
			WritePrivateProfileString( "Rainlendar", "StartFromJanuary", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_RememberDialogPositions);
			WritePrivateProfileString( "Rainlendar", "RememberDialogPositions", tmpSz, INIPath.c_str() );

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
			sprintf(tmpSz, "%i", m_ServerSyncOnEdit);
			WritePrivateProfileString( "Rainlendar", "ServerSyncOnEdit", tmpSz, INIPath.c_str() );

			sprintf(tmpSz, "%i", m_HideHotkey);
			WritePrivateProfileString( "Rainlendar", "HideHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ShowHotkey);
			WritePrivateProfileString( "Rainlendar", "ShowHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ToggleHotkey);
			WritePrivateProfileString( "Rainlendar", "ToggleHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ActivateHotkey);
			WritePrivateProfileString( "Rainlendar", "ActivateHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_RefreshHotkey);
			WritePrivateProfileString( "Rainlendar", "RefreshHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ConfigHotkey);
			WritePrivateProfileString( "Rainlendar", "ConfigHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_SkinHotkey);
			WritePrivateProfileString( "Rainlendar", "SkinHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_NextHotkey);
			WritePrivateProfileString( "Rainlendar", "NextHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_PreviousHotkey);
			WritePrivateProfileString( "Rainlendar", "PreviousHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_NextYearHotkey);
			WritePrivateProfileString( "Rainlendar", "NextYearHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_PreviousYearHotkey);
			WritePrivateProfileString( "Rainlendar", "PreviousYearHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_CurrentHotkey);
			WritePrivateProfileString( "Rainlendar", "CurrentHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_AllHotkey);
			WritePrivateProfileString( "Rainlendar", "AllHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_OutlookHotkey);
			WritePrivateProfileString( "Rainlendar", "OutlookHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_TodoHotkey);
			WritePrivateProfileString( "Rainlendar", "TodoHotkey", tmpSz, INIPath.c_str() );
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

		// Tooltip
		sprintf(tmpSz, "%X", m_ToolTipFontColor);
		WritePrivateProfileString( "Rainlendar", "ToolTipFontColor", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%X", m_ToolTipBGColor);
		WritePrivateProfileString( "Rainlendar", "ToolTipBGColor", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "ToolTipFont", m_ToolTipFont.c_str(), INIPath.c_str() );

		// Todo stuff
		WritePrivateProfileString( "Rainlendar", "TodoBitmapName", m_TodoBitmapName.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%i, %i, %i, %i", m_TodoBitmapMargins.left, m_TodoBitmapMargins.top, m_TodoBitmapMargins.right, m_TodoBitmapMargins.bottom);
		WritePrivateProfileString( "Rainlendar", "TodoBitmapMargins", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i, %i, %i, %i", m_TodoTextMargins.left, m_TodoTextMargins.top, m_TodoTextMargins.right, m_TodoTextMargins.bottom);
		WritePrivateProfileString( "Rainlendar", "TodoTextMargins", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_TodoW);
		WritePrivateProfileString( "Rainlendar", "TodoW", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "TodoFont", m_TodoFont.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%X", m_TodoFontColor);
		WritePrivateProfileString( "Rainlendar", "TodoFontColor", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "TodoItemBitmapName", m_TodoItemBitmapName.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%i", m_TodoSeparation);
		WritePrivateProfileString( "Rainlendar", "TodoSeparation", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i, %i", m_TodoItemOffset.x, m_TodoItemOffset.y);
		WritePrivateProfileString( "Rainlendar", "TodoItemOffset", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_TodoItemAlign);
		WritePrivateProfileString( "Rainlendar", "TodoItemAlign", tmpSz, INIPath.c_str() );

		// MessageBox stuff
		WritePrivateProfileString( "Rainlendar", "MessageBoxBitmapName", m_MessageBoxBitmapName.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%i, %i, %i, %i", m_MessageBoxBitmapMargins.left, m_MessageBoxBitmapMargins.top, m_MessageBoxBitmapMargins.right, m_MessageBoxBitmapMargins.bottom);
		WritePrivateProfileString( "Rainlendar", "MessageBoxBitmapMargins", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i, %i, %i, %i", m_MessageBoxTextMargins.left, m_MessageBoxTextMargins.top, m_MessageBoxTextMargins.right, m_MessageBoxTextMargins.bottom);
		WritePrivateProfileString( "Rainlendar", "MessageBoxTextMargins", tmpSz, INIPath.c_str() );
		WritePrivateProfileString( "Rainlendar", "MessageBoxFont", m_MessageBoxFont.c_str(), INIPath.c_str() );
		sprintf(tmpSz, "%X", m_MessageBoxFontColor);
		WritePrivateProfileString( "Rainlendar", "MessageBoxFontColor", tmpSz, INIPath.c_str() );
		sprintf(tmpSz, "%i", m_MessageBoxSeparation);
		WritePrivateProfileString( "Rainlendar", "MessageBoxSeparation", tmpSz, INIPath.c_str() );
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
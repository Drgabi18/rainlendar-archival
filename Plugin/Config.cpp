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
  $Header: /home/cvsroot/Rainlendar/Plugin/Config.cpp,v 1.28 2004/12/05 18:34:12 rainy Exp $

  $Log: Config.cpp,v $
  Revision 1.28  2004/12/05 18:34:12  rainy
  Added some more stuff.

  Revision 1.27  2004/11/06 13:38:58  rainy
  no message

  Revision 1.26  2004/06/10 15:25:29  rainy
  Added keepalive

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
  Changed CEvent to CEntryEvent to avoid name clash.
  Added Server stuff.

  Revision 1.3  2002/01/15 17:58:51  rainy
  Removed the StartDelay

  Revision 1.2  2002/01/10 16:48:27  rainy
  Added the weeknumber stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "Config.h"
#include "ToolTip.h"

CConfig* CConfig::c_Config = NULL;

CConfig& CConfig::Instance()
{
	if (c_Config == NULL)
	{
		c_Config = new CConfig;
	}

	return *c_Config;
}

void CConfig::DeleteInstance()
{
	delete c_Config;
	c_Config = NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfig::CConfig()
{
	Reset();
}

void CConfig::Reset()
{
	TCHAR buffer[256];

	m_CalendarEnable = true;
	m_TodoEnable = true;
	m_EventListEnable = true;
	m_TodoLocked = false;
	m_EventListLocked = false;
	m_VisibleWindows = 0;

	m_X=0;
	m_Y=0;
	m_TodoX = 0;
	m_TodoY = 200;
	m_EventListX = 200;
	m_EventListY = 0;
	m_MessageBoxX = 50;
	m_MessageBoxY = 50;

	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, buffer, 256);
	if (buffer[0] == '0')
	{
		m_StartFromMonday=true;
	}
	else
	{
		m_StartFromMonday=false;
	}

	m_StartHidden=false;
	m_DisableHotkeys=false;
	m_UseWindowName=false;
	m_PollWallpaper=false;
	m_Movable=true;
	m_MouseHide=false;
	m_SnapEdges=true;
	m_NativeTransparency=true;
	m_OpaqueOnMouseOver=false;
	m_TransparentOnMouseOver=false;
	m_TransparencyValue=255;
	m_RefreshOnResolutionChange=false;
	m_WindowPos=WINDOWPOS_NORMAL;
	m_CurrentProfile="Default";
	m_EventToolTips=true;
	m_TooltipSeparator = true;

	m_CurrentLanguage="";
	m_CurrentSkin="Shadow3.1";
	m_CurrentSkinIni="English.ini";
    
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IFIRSTWEEKOFYEAR, buffer, 256);
	if (buffer[0] == '0')
	{
	    m_Week1HasJanuary1st=true;
	}
	else
	{
	    m_Week1HasJanuary1st=false;
	}

    m_NegativeCoords=false;
    m_ClickThrough=false;
	m_EventExecute = "";
	m_ShowTrayIcon = true;

	m_DisableDoneList = false;
	m_UseSkinCharSet = false;

	m_HideFromCalendar.clear();
	m_HideFromEventList.clear();

	m_TrayExecuteL = "";
	m_TrayExecuteR = "";
	m_TrayExecuteM = "";
	m_TrayExecuteDL = "!RainlendarToggle visible";
	m_TrayExecuteDR = "";
	m_TrayExecuteDM = "";

	m_ConfigEditor = "Notepad.exe";
	
	m_CheckUpdates = false;
	m_DisableKeyboardOverride = false;

	m_ShowAllEvents = true;
	m_ShowSingleEvent = true;
	m_SnoozeTime = 60;

	m_WheelScrollMonths = 1;
	m_WeekNumberDelta = 0;
	m_FadeDuration = 250;

	RECT r;
	GetClientRect(GetDesktopWindow(), &r);
	m_ToolTipMaxWidth = r.right / 2;
	m_MessageBoxMaxWidth = r.right / 2;
	
	m_VerticalCount = 1;
	m_HorizontalCount = 1;
	m_PreviousMonths = 0;
	m_StartFromJanuary = false;
	m_RememberDialogPositions=false;

	m_GrowUpwards = 0;
	m_ShowContextMenu = 0;
	m_TodoAction = 0;
	m_CalendarAction = 0;
	m_EventListAction = 0;

	m_EventListDays = 7;
	m_SubstituteDays = true;

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
	m_TodoHotkey = 0;
	m_HideCalendarHotkey = 0;
	m_ShowCalendarHotkey = 0;
	m_ToggleCalendarHotkey = 0;
	m_HideTodoHotkey = 0;
	m_ShowTodoHotkey = 0;
	m_ToggleTodoHotkey = 0;
	m_HideEventsHotkey = 0;
	m_ShowEventsHotkey = 0;
	m_ToggleEventsHotkey = 0;
	m_AddEventHotkey = 0;

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

	// DEBUG: Display some of the key settings
	DebugLog("Hide=%i, Move=%i, Negative=%i, Hidden=%i, WinName=%i, ClickThrough=%i", m_MouseHide, m_Movable, m_NegativeCoords, m_StartHidden, m_UseWindowName, m_ClickThrough);
	DebugLog("Poll=%i, Transparency=%i (%i), ResChange=%i, WinPos=%i", m_PollWallpaper, m_NativeTransparency, m_TransparencyValue, m_RefreshOnResolutionChange, m_WindowPos);
	DebugLog("V=%i H=%i, Jan=%i, Prev=%i", m_VerticalCount, m_HorizontalCount, m_StartFromJanuary, m_PreviousMonths);
}

void CConfig::ReadGeneralConfig(const char* iniFile)
{
	char tmpSz[MAX_LINE_LENGTH];

	// Windows
	m_CalendarEnable=(1==GetPrivateProfileInt( "Rainlendar", "CalendarEnable", m_CalendarEnable?1:0, iniFile))?true:false;
	m_X=GetPrivateProfileInt( "Rainlendar", "X", m_X, iniFile);
	m_Y=GetPrivateProfileInt( "Rainlendar", "Y", m_Y, iniFile);

	m_TodoEnable=(1==GetPrivateProfileInt( "Rainlendar", "TodoEnable", m_TodoEnable?1:0, iniFile))?true:false;
	m_TodoLocked=(1==GetPrivateProfileInt( "Rainlendar", "TodoLocked", m_TodoLocked?1:0, iniFile))?true:false;
	m_TodoX=GetPrivateProfileInt( "Rainlendar", "TodoX", m_TodoX, iniFile);
	m_TodoY=GetPrivateProfileInt( "Rainlendar", "TodoY", m_TodoY, iniFile);

	m_EventListEnable=(1==GetPrivateProfileInt( "Rainlendar", "EventListEnable", m_EventListEnable?1:0, iniFile))?true:false;
	m_EventListLocked=(1==GetPrivateProfileInt( "Rainlendar", "EventListLocked", m_EventListLocked?1:0, iniFile))?true:false;
	m_EventListX=GetPrivateProfileInt( "Rainlendar", "EventListX", m_EventListX, iniFile);
	m_EventListY=GetPrivateProfileInt( "Rainlendar", "EventListY", m_EventListY, iniFile);

	m_MessageBoxX=GetPrivateProfileInt( "Rainlendar", "MessageBoxX", m_MessageBoxX, iniFile);
	m_MessageBoxY=GetPrivateProfileInt( "Rainlendar", "MessageBoxY", m_MessageBoxY, iniFile);

	m_VisibleWindows=GetPrivateProfileInt( "Rainlendar", "VisibleWindows", m_VisibleWindows, iniFile);

	// General
	m_StartFromMonday=(1==GetPrivateProfileInt( "Rainlendar", "StartFromMonday", m_StartFromMonday?1:0, iniFile))?true:false;
	m_StartHidden=(1==GetPrivateProfileInt( "Rainlendar", "StartHidden", m_StartHidden?1:0, iniFile))?true:false;
	m_DisableHotkeys=(1==GetPrivateProfileInt( "Rainlendar", "DisableHotkeys", m_DisableHotkeys?1:0, iniFile))?true:false;
	m_UseWindowName=(1==GetPrivateProfileInt( "Rainlendar", "UseWindowName", m_UseWindowName?1:0, iniFile))?true:false;
	m_PollWallpaper=(1==GetPrivateProfileInt( "Rainlendar", "PollWallpaper", m_PollWallpaper?1:0, iniFile))?true:false;
	m_Movable=(1==GetPrivateProfileInt( "Rainlendar", "Movable", m_Movable?1:0, iniFile))?true:false;
	m_MouseHide=(1==GetPrivateProfileInt( "Rainlendar", "MouseHide", m_MouseHide?1:0, iniFile))?true:false;
	m_SnapEdges=(1==GetPrivateProfileInt( "Rainlendar", "SnapEdges", m_SnapEdges?1:0, iniFile))?true:false;
	m_NativeTransparency=(1==GetPrivateProfileInt( "Rainlendar", "NativeTransparency", m_NativeTransparency?1:0, iniFile))?true:false;
	m_OpaqueOnMouseOver=(1==GetPrivateProfileInt( "Rainlendar", "OpaqueOnMouseOver", m_OpaqueOnMouseOver?1:0, iniFile))?true:false;
	m_TransparentOnMouseOver=(1==GetPrivateProfileInt( "Rainlendar", "TransparentOnMouseOver", m_TransparentOnMouseOver?1:0, iniFile))?true:false;
	m_TransparencyValue=GetPrivateProfileInt( "Rainlendar", "TransparencyValue", m_TransparencyValue, iniFile);
	m_RefreshOnResolutionChange=(1==GetPrivateProfileInt( "Rainlendar", "RefreshOnResolutionChange", m_RefreshOnResolutionChange?1:0, iniFile))?true:false;
	m_Week1HasJanuary1st=(1==GetPrivateProfileInt( "Rainlendar", "Week1HasJanuary1st", m_Week1HasJanuary1st?1:0, iniFile))?true:false;
	m_TooltipSeparator=(1==GetPrivateProfileInt( "Rainlendar", "TooltipSeparator", m_TooltipSeparator?1:0, iniFile))?true:false;
	m_WindowPos=(WINDOWPOS)GetPrivateProfileInt( "Rainlendar", "WindowPos", m_WindowPos, iniFile);
	m_RememberDialogPositions=(1==GetPrivateProfileInt( "Rainlendar", "RememberDialogPositions", m_RememberDialogPositions?1:0, iniFile))?true:false;
	m_NegativeCoords=(1==GetPrivateProfileInt( "Rainlendar", "NegativeCoords", m_NegativeCoords?1:0, iniFile))?true:false;
	m_ClickThrough=(1==GetPrivateProfileInt( "Rainlendar", "ClickThrough", m_ClickThrough?1:0, iniFile))?true:false;
	m_ShowTrayIcon=(1==GetPrivateProfileInt( "Rainlendar", "ShowTrayIcon", m_ShowTrayIcon?1:0, iniFile))?true:false;
	m_DisableDoneList=(1==GetPrivateProfileInt( "Rainlendar", "DisableDoneList", m_DisableDoneList?1:0, iniFile))?true:false;
	m_UseSkinCharSet=(1==GetPrivateProfileInt( "Rainlendar", "UseSkinCharSet", m_UseSkinCharSet?1:0, iniFile))?true:false;

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

	if(GetPrivateProfileString( "Rainlendar", "ConfigEditor", m_ConfigEditor.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_ConfigEditor=tmpSz;
	}

	m_CheckUpdates=(1==GetPrivateProfileInt( "Rainlendar", "CheckUpdates", m_CheckUpdates?1:0, iniFile))?true:false;
	m_DisableKeyboardOverride=(1==GetPrivateProfileInt( "Rainlendar", "DisableKeyboardOverride", m_DisableKeyboardOverride?1:0, iniFile))?true:false;

	m_VerticalCount=GetPrivateProfileInt( "Rainlendar", "VerticalCount", m_VerticalCount, iniFile);
	m_HorizontalCount=GetPrivateProfileInt( "Rainlendar", "HorizontalCount", m_HorizontalCount, iniFile);
	m_PreviousMonths=GetPrivateProfileInt( "Rainlendar", "PreviousMonths", m_PreviousMonths, iniFile);
	m_StartFromJanuary=(1==GetPrivateProfileInt( "Rainlendar", "StartFromJanuary", m_StartFromJanuary?1:0, iniFile))?true:false;
	m_EventListDays=GetPrivateProfileInt( "Rainlendar", "EventListDays", m_EventListDays, iniFile);
	m_GrowUpwards=GetPrivateProfileInt( "Rainlendar", "GrowUpwards", m_GrowUpwards, iniFile);
	m_ShowContextMenu=(1==GetPrivateProfileInt( "Rainlendar", "ShowContextMenu", m_ShowContextMenu?1:0, iniFile))?true:false;
	m_TodoAction=GetPrivateProfileInt( "Rainlendar", "TodoAction", m_TodoAction, iniFile);
	m_CalendarAction=GetPrivateProfileInt( "Rainlendar", "CalendarAction", m_CalendarAction, iniFile);
	m_EventListAction=GetPrivateProfileInt( "Rainlendar", "EventListAction", m_EventListAction, iniFile);

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
	m_TodoHotkey=GetPrivateProfileInt( "Rainlendar", "TodoHotkey", m_TodoHotkey, iniFile);
	m_HideCalendarHotkey=GetPrivateProfileInt( "Rainlendar", "HideCalendarHotkey", m_HideCalendarHotkey, iniFile);
	m_ShowCalendarHotkey=GetPrivateProfileInt( "Rainlendar", "ShowCalendarHotkey", m_ShowCalendarHotkey, iniFile);
	m_ToggleCalendarHotkey=GetPrivateProfileInt( "Rainlendar", "ToggleCalendarHotkey", m_ToggleCalendarHotkey, iniFile);
	m_HideTodoHotkey=GetPrivateProfileInt( "Rainlendar", "HideTodoHotkey", m_HideTodoHotkey, iniFile);
	m_ShowTodoHotkey=GetPrivateProfileInt( "Rainlendar", "ShowTodoHotkey", m_ShowTodoHotkey, iniFile);
	m_ToggleTodoHotkey=GetPrivateProfileInt( "Rainlendar", "ToggleTodoHotkey", m_ToggleTodoHotkey, iniFile);
	m_HideEventsHotkey=GetPrivateProfileInt( "Rainlendar", "HideEventsHotkey", m_HideEventsHotkey, iniFile);
	m_ShowEventsHotkey=GetPrivateProfileInt( "Rainlendar", "ShowEventsHotkey", m_ShowEventsHotkey, iniFile);
	m_ToggleEventsHotkey=GetPrivateProfileInt( "Rainlendar", "ToggleEventsHotkey", m_ToggleEventsHotkey, iniFile);
	m_AddEventHotkey=GetPrivateProfileInt( "Rainlendar", "AddEventHotkey", m_AddEventHotkey, iniFile);

	if(GetPrivateProfileString( "Rainlendar", "CurrentProfile", m_CurrentProfile.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_CurrentProfile=tmpSz;
	}

	if(GetPrivateProfileString( "Rainlendar", "EventExecute", m_EventExecute.c_str(), tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		m_EventExecute=tmpSz;
	}
	m_EventToolTips=(1==GetPrivateProfileInt( "Rainlendar", "EventToolTips", m_EventToolTips?1:0, iniFile))?true:false;

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

	m_MessageBoxMaxWidth=GetPrivateProfileInt( "Rainlendar", "MessageBoxMaxWidth", m_MessageBoxMaxWidth, iniFile);
	m_SnoozeTime=GetPrivateProfileInt( "Rainlendar", "SnoozeTime", m_SnoozeTime, iniFile);
	m_WheelScrollMonths=GetPrivateProfileInt( "Rainlendar", "WheelScrollMonths", m_WheelScrollMonths, iniFile);
	m_WeekNumberDelta=GetPrivateProfileInt( "Rainlendar", "WeekNumberDelta", m_WeekNumberDelta, iniFile);
	m_FadeDuration=GetPrivateProfileInt( "Rainlendar", "FadeDuration", m_FadeDuration, iniFile);
	m_ShowAllEvents=(1==GetPrivateProfileInt( "Rainlendar", "ShowAllEvents", m_ShowAllEvents?1:0, iniFile))?true:false;
	m_ShowSingleEvent=(1==GetPrivateProfileInt( "Rainlendar", "ShowSingleEvent", m_ShowSingleEvent?1:0, iniFile))?true:false;
	m_SubstituteDays=(1==GetPrivateProfileInt( "Rainlendar", "SubstituteDays", m_SubstituteDays?1:0, iniFile))?true:false;

	if(GetPrivateProfileString( "Rainlendar", "HideFromCalendar", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		std::string tmp = tmpSz;
		SeparateProfiles(tmp, m_HideFromCalendar);
	}
	if(GetPrivateProfileString( "Rainlendar", "HideFromEventList", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
	{
		std::string tmp = tmpSz;
		SeparateProfiles(tmp, m_HideFromEventList);
	}

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
				profile->fontColor = GetRainlendar()->GetCalendarWindow().GetItemEvent()->GetEventFontColor();
			}

			if(GetPrivateProfileString(pos, "EventFontColor2", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				profile->fontColor2 = ParseColor(tmpSz);
			}
			else
			{
				profile->fontColor2 = GetRainlendar()->GetCalendarWindow().GetItemEvent()->GetEventFontColor2();
			}

			if(GetPrivateProfileString(pos, "EventListFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				profile->eventColor = ParseColor(tmpSz);
			}
			else
			{
				profile->eventColor = GetRainlendar()->GetCalendarWindow().GetEventListWindow().GetEventListFontColor();
			}

			if(GetPrivateProfileString(pos, "ToolTipFontColor", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				profile->toolTipColor = ParseColor(tmpSz);
			}
			else
			{
				profile->toolTipColor = CToolTip::Instance().GetToolTipFontColor();
			}

			if(GetPrivateProfileString(pos, "EventBitmapName", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				profile->bitmapName = tmpSz;
			}
			
			if(GetPrivateProfileString(pos, "EventIconName", "", tmpSz, MAX_LINE_LENGTH, iniFile) > 0) 
			{
				profile->iconName = tmpSz;
			}
			profile->iconAlign = (CRasterizer::ALIGN)GetPrivateProfileInt(pos, "EventIconAlign", GetRainlendar()->GetCalendarWindow().GetItemDays()->GetDaysAlign(), iniFile);

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

bool CConfig::IsHiddenProfile(const char* name, bool calendar)
{
	std::set<std::string>::iterator iter;

	if (calendar)
	{
		iter = m_HideFromCalendar.find(name);
		if (iter != m_HideFromCalendar.end())
		{
			return true;
		}
	}
	else
	{
		iter = m_HideFromEventList.find(name);
		if (iter != m_HideFromEventList.end())
		{
			return true;
		}
	}
	return false;
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
		if (flags & WRITE_EVENTLISTPOS)
		{
			sprintf(tmpSz, "%i", m_EventListX);
			WritePrivateProfileString( "Rainlendar", "EventListX", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_EventListY);
			WritePrivateProfileString( "Rainlendar", "EventListY", tmpSz, INIPath.c_str() );
		}
		if (flags & WRITE_PROFILE)
		{
			WritePrivateProfileString( "Rainlendar", "CurrentProfile", m_CurrentProfile.c_str(), INIPath.c_str() );
		}
		if (flags & WRITE_WINDOW_STATE)
		{
			sprintf(tmpSz, "%i", m_CalendarEnable);
			WritePrivateProfileString( "Rainlendar", "CalendarEnable", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_TodoEnable);
			WritePrivateProfileString( "Rainlendar", "TodoEnable", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_EventListEnable);
			WritePrivateProfileString( "Rainlendar", "EventListEnable", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_TodoLocked);
			WritePrivateProfileString( "Rainlendar", "TodoLocked", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_EventListLocked);
			WritePrivateProfileString( "Rainlendar", "EventListLocked", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_VisibleWindows);
			WritePrivateProfileString( "Rainlendar", "VisibleWindows", tmpSz, INIPath.c_str() );
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
			sprintf(tmpSz, "%i%%", m_MessageBoxX);
			WritePrivateProfileString( "Rainlendar", "MessageBoxX", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i%%", m_MessageBoxY);
			WritePrivateProfileString( "Rainlendar", "MessageBoxY", tmpSz, INIPath.c_str() );

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
			sprintf(tmpSz, "%i", m_OpaqueOnMouseOver);
			WritePrivateProfileString( "Rainlendar", "OpaqueOnMouseOver", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_TransparentOnMouseOver);
			WritePrivateProfileString( "Rainlendar", "TransparentOnMouseOver", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_TransparencyValue);
			WritePrivateProfileString( "Rainlendar", "TransparencyValue", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_RefreshOnResolutionChange);
			WritePrivateProfileString( "Rainlendar", "RefreshOnResolutionChange", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_NegativeCoords);
			WritePrivateProfileString( "Rainlendar", "NegativeCoords", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ClickThrough);
			WritePrivateProfileString( "Rainlendar", "ClickThrough", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_Week1HasJanuary1st);
			WritePrivateProfileString( "Rainlendar", "Week1HasJanuary1st", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_TooltipSeparator);
			WritePrivateProfileString( "Rainlendar", "TooltipSeparator", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_WindowPos);
			WritePrivateProfileString( "Rainlendar", "WindowPos", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ShowTrayIcon);
			WritePrivateProfileString( "Rainlendar", "ShowTrayIcon", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_DisableDoneList);
			WritePrivateProfileString( "Rainlendar", "DisableDoneList", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_UseSkinCharSet);
			WritePrivateProfileString( "Rainlendar", "UseSkinCharSet", tmpSz, INIPath.c_str() );
			WritePrivateProfileString( "Rainlendar", "EventExecute", m_EventExecute.c_str(), INIPath.c_str() );
			sprintf(tmpSz, "%i", m_EventToolTips);
			WritePrivateProfileString( "Rainlendar", "EventToolTips", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ToolTipMaxWidth);
			WritePrivateProfileString( "Rainlendar", "ToolTipMaxWidth", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_MessageBoxMaxWidth);
			WritePrivateProfileString( "Rainlendar", "MessageBoxMaxWidth", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ShowAllEvents);
			WritePrivateProfileString( "Rainlendar", "ShowAllEvents", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ShowSingleEvent);
			WritePrivateProfileString( "Rainlendar", "ShowSingleEvent", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_WheelScrollMonths);
			WritePrivateProfileString( "Rainlendar", "WheelScrollMonths", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_WeekNumberDelta);
			WritePrivateProfileString( "Rainlendar", "WeekNumberDelta", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_FadeDuration);
			WritePrivateProfileString( "Rainlendar", "FadeDuration", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_SnoozeTime);
			WritePrivateProfileString( "Rainlendar", "SnoozeTime", tmpSz, INIPath.c_str() );

			sprintf(tmpSz, "%i", m_CheckUpdates);
			WritePrivateProfileString( "Rainlendar", "CheckUpdates", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_DisableKeyboardOverride);
			WritePrivateProfileString( "Rainlendar", "DisableKeyboardOverride", tmpSz, INIPath.c_str() );

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
			sprintf(tmpSz, "%i", m_SubstituteDays);
			WritePrivateProfileString( "Rainlendar", "SubstituteDays", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_EventListDays);
			WritePrivateProfileString( "Rainlendar", "EventListDays", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_GrowUpwards);
			WritePrivateProfileString( "Rainlendar", "GrowUpwards", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ShowContextMenu);
			WritePrivateProfileString( "Rainlendar", "ShowContextMenu", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_TodoAction);
			WritePrivateProfileString( "Rainlendar", "TodoAction", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_CalendarAction);
			WritePrivateProfileString( "Rainlendar", "CalendarAction", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_EventListAction);
			WritePrivateProfileString( "Rainlendar", "EventListAction", tmpSz, INIPath.c_str() );

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
			sprintf(tmpSz, "%i", m_TodoHotkey);
			WritePrivateProfileString( "Rainlendar", "TodoHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_HideCalendarHotkey);
			WritePrivateProfileString( "Rainlendar", "HideCalendarHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ShowCalendarHotkey);
			WritePrivateProfileString( "Rainlendar", "ShowCalendarHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ToggleCalendarHotkey);
			WritePrivateProfileString( "Rainlendar", "ToggleCalendarHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_HideTodoHotkey);
			WritePrivateProfileString( "Rainlendar", "HideTodoHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ShowTodoHotkey);
			WritePrivateProfileString( "Rainlendar", "ShowTodoHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ToggleTodoHotkey);
			WritePrivateProfileString( "Rainlendar", "ToggleTodoHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_HideEventsHotkey);
			WritePrivateProfileString( "Rainlendar", "HideEventsHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ShowEventsHotkey);
			WritePrivateProfileString( "Rainlendar", "ShowEventsHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_ToggleEventsHotkey);
			WritePrivateProfileString( "Rainlendar", "ToggleEventsHotkey", tmpSz, INIPath.c_str() );
			sprintf(tmpSz, "%i", m_AddEventHotkey);
			WritePrivateProfileString( "Rainlendar", "AddEventHotkey", tmpSz, INIPath.c_str() );

		}
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

void CConfig::SeparateProfiles(std::string& profiles, std::set<std::string>& result) 
{
	std::string name;
	int pos = -1;

	do 
	{
		pos = profiles.find(';');
		if (pos != -1) 
		{
			name.assign(profiles.begin(), profiles.begin() + pos);
			profiles.erase(profiles.begin(), profiles.begin() + pos + 1);
		}
		else
		{
			name = profiles;
		}

		if (!name.empty())
		{
			result.insert(name);
		}
	} while(pos != -1);
}
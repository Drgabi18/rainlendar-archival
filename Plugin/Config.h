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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Config.h,v 1.6 2002/05/23 17:33:41 rainy Exp $

  $Log: Config.h,v $
  Revision 1.6  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.5  2002/02/27 18:57:15  rainy
  Added new configs.

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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "Rasterizer.h"
#include "Background.h"
#include <string>
#include <vector>

class CConfig  
{
public:
	enum WRITE_FLAGS
	{
		WRITE_POS = 1,
		WRITE_CONFIG = 2,
		WRITE_FULL = 3,		// FULL is 1 + 2
		WRITE_SKIN = 4
	};

	enum WINDOWPOS 
	{
		WINDOWPOS_ONBOTTOM,
		WINDOWPOS_NORMAL,
		WINDOWPOS_ONTOP
	};

	CConfig();
	~CConfig();

	const std::string& GetPath() { return m_Path; };
	void SetPath(const std::string& Path) { m_Path=Path; };
	const std::string& GetEventsPath() { return m_EventsPath; };
	void SetEventsPath(const std::string& EventsPath) { m_EventsPath=EventsPath; };
	const std::string& GetSkinsPath() { return m_SkinsPath; };
	void SetSkinsPath(const std::string& SkinsPath) { m_SkinsPath=SkinsPath; };
	const std::string& GetCurrentSkin() { return m_CurrentSkin; };
	void SetCurrentSkin(const std::string& CurrentSkin) { m_CurrentSkin=CurrentSkin; };
	const std::string& GetCurrentSkinIni() { return m_CurrentSkinIni; };
	void SetCurrentSkinIni(const std::string& CurrentSkinIni) { m_CurrentSkinIni=CurrentSkinIni; };

	const std::string GetMonthName(int index) { return m_MonthName[index]; };

	// General
	int GetX() { return m_X; };
	int GetY() { return m_Y; };
	bool GetStartFromMonday() { return m_StartFromMonday; };
    const std::string& GetWeekdayNames() { return m_WeekdayNames; };
    const std::string& GetMonthNames() { return m_MonthNames; };
	int GetRefreshDelay() { return m_RefreshDelay; };
	bool GetStartHidden() { return m_StartHidden; };
	bool GetDisableHotkeys() { return m_DisableHotkeys; };
	bool GetUseWindowName() { return m_UseWindowName; };
	bool GetPollWallpaper() { return m_PollWallpaper; };
	bool GetMovable() { return m_Movable; };
	bool GetMouseHide() { return m_MouseHide; };
	CBackground::MODE GetBackgroundMode() { return m_BackgroundMode; };
	WINDOWPOS GetWindowPos() { return m_WindowPos; };

	void SetX(int X ) { m_X=X; };
	void SetY(int Y ) { m_Y=Y; };
	void SetStartFromMonday(bool StartFromMonday ) { m_StartFromMonday=StartFromMonday; };
	void SetWeekdayNames(const std::string& WeekdayNames ) { m_WeekdayNames=WeekdayNames; };
	void SetMonthNames(const std::string& MonthNames ) { m_MonthNames=MonthNames; };
	void SetRefreshDelay(int RefreshDelay ) { m_RefreshDelay=RefreshDelay; };
	void SetStartHidden(bool StartHidden ) { m_StartHidden=StartHidden; };
	void SetDisableHotkeys(bool DisableHotkeys ) { m_DisableHotkeys=DisableHotkeys; };
	void SetUseWindowName(bool UseWindowName ) { m_UseWindowName=UseWindowName; };
	void SetPollWallpaper(bool PollWallpaper ) { m_PollWallpaper=PollWallpaper; };
	void SetMovable(bool Movable) { m_Movable=Movable; };
	void SetMouseHide(bool MouseHide) { m_MouseHide=MouseHide; };
	void SetBackgroundMode(CBackground::MODE BackgroundMode ) { m_BackgroundMode=BackgroundMode; };
	void SetWindowPos(WINDOWPOS WindowPos) { m_WindowPos=WindowPos; };

    const std::string& GetBackgroundBitmapName() { return m_BackgroundBitmapName; };
	void SetBackgroundBitmapName(const std::string& BackgroundBitmapName ) { m_BackgroundBitmapName=BackgroundBitmapName; };

	// Days
	bool GetDaysEnable() { return m_DaysEnable; };
	int GetDaysH() { return m_DaysH; };
	int GetDaysW() { return m_DaysW; };
	int GetDaysY() { return m_DaysY; };
	int GetDaysX() { return m_DaysX; };
    const std::string& GetDaysBitmapName() { return m_DaysBitmapName; };
	int GetDaysNumOfComponents() { return m_DaysNumOfComponents; };
	CRasterizer::ALIGN GetDaysAlign() { return m_DaysAlign; };
    const std::string& GetDaysFont() { return m_DaysFont; };
	CRasterizer::TYPE GetDaysRasterizer() { return m_DaysRasterizer; };
	COLORREF GetDaysFontColor() { return m_DaysFontColor; };
	bool GetDaysIgnoreToday() { return m_DaysIgnoreToday; };
	bool GetDaysIgnoreEvent() { return m_DaysIgnoreEvent; };
	int GetDaysSeparation() { return m_DaysSeparation; };

	void SetDaysEnable(bool DaysEnable ) { m_DaysEnable=DaysEnable; };
	void SetDaysH(int DaysH ) { m_DaysH=DaysH; };
	void SetDaysW(int DaysW ) { m_DaysW=DaysW; };
	void SetDaysY(int DaysY ) { m_DaysY=DaysY; };
	void SetDaysX(int DaysX ) { m_DaysX=DaysX; };
	void SetDaysBitmapName(const std::string& DaysBitmapName ) { m_DaysBitmapName=DaysBitmapName; };
	void SetDaysNumOfComponents(int DaysNumOfComponents ) { m_DaysNumOfComponents=DaysNumOfComponents; };
	void SetDaysAlign(CRasterizer::ALIGN DaysAlign ) { m_DaysAlign=DaysAlign; };
	void SetDaysFont(const std::string& DaysFont ) { m_DaysFont=DaysFont; };
	void SetDaysRasterizer(CRasterizer::TYPE DaysRasterizer ) { m_DaysRasterizer=DaysRasterizer; };
	void SetDaysFontColor(COLORREF DaysFontColor ) { m_DaysFontColor=DaysFontColor; };
	void SetDaysIgnoreToday(bool DaysIgnoreToday ) { m_DaysIgnoreToday=DaysIgnoreToday; };
	void SetDaysIgnoreEvent(bool DaysIgnoreEvent ) { m_DaysIgnoreEvent=DaysIgnoreEvent; };
	void SetDaysSeparation(int separation) { m_DaysSeparation = separation; };

	// Today
	bool GetTodayEnable() { return m_TodayEnable; };
	CRasterizer::ALIGN GetTodayAlign() { return m_TodayAlign; };
    const std::string& GetTodayBitmapName() { return m_TodayBitmapName; };
	int GetTodayNumOfComponents() { return m_TodayNumOfComponents; };
	CRasterizer::TYPE GetTodayRasterizer() { return m_TodayRasterizer; };
	COLORREF GetTodayFontColor() { return m_TodayFontColor; };
	std::string GetTodayFont() { return m_TodayFont; };
	int GetTodaySeparation() { return m_TodaySeparation; };

	void SetTodayEnable(bool TodayEnable ) { m_TodayEnable=TodayEnable; };
	void SetTodayAlign(CRasterizer::ALIGN TodayAlign ) { m_TodayAlign=TodayAlign; };
	void SetTodayBitmapName(const std::string& TodayBitmapName ) { m_TodayBitmapName=TodayBitmapName; };
	void SetTodayNumOfComponents(int TodayNumOfComponents ) { m_TodayNumOfComponents=TodayNumOfComponents; };
	void SetTodayRasterizer(CRasterizer::TYPE TodayRasterizer ) { m_TodayRasterizer=TodayRasterizer; };
	void SetTodayFontColor(COLORREF TodayFontColor ) { m_TodayFontColor=TodayFontColor; };
	void SetTodayFont(std::string TodayFont ) { m_TodayFont=TodayFont; };
	void SetTodaySeparation(int separation) { m_TodaySeparation = separation; };

	// Weekdays
	bool GetWeekdaysEnable() { return m_WeekdaysEnable; };
    const std::string& GetWeekdaysBitmapName() { return m_WeekdaysBitmapName; };
	CRasterizer::ALIGN GetWeekdaysAlign() { return m_WeekdaysAlign; };
    const std::string& GetWeekdaysFont() { return m_WeekdaysFont; };
	CRasterizer::TYPE GetWeekdaysRasterizer() { return m_WeekdaysRasterizer; };
	COLORREF GetWeekdaysFontColor() { return m_WeekdaysFontColor; };

	void SetWeekdaysEnable(bool WeekdaysEnable ) { m_WeekdaysEnable=WeekdaysEnable; };
	void SetWeekdaysBitmapName(const std::string& WeekdaysBitmapName ) { m_WeekdaysBitmapName=WeekdaysBitmapName; };
	void SetWeekdaysAlign(CRasterizer::ALIGN WeekdaysAlign ) { m_WeekdaysAlign=WeekdaysAlign; };
	void SetWeekdaysFont(const std::string& WeekdaysFont ) { m_WeekdaysFont=WeekdaysFont; };
	void SetWeekdaysRasterizer(CRasterizer::TYPE WeekdaysRasterizer ) { m_WeekdaysRasterizer=WeekdaysRasterizer; };
	void SetWeekdaysFontColor(COLORREF WeekdaysFontColor ) { m_WeekdaysFontColor=WeekdaysFontColor; };

	// Month
	bool GetMonthEnable() { return m_MonthEnable; };
    const std::string& GetMonthBitmapName() { return m_MonthBitmapName; };
	int GetMonthX() { return m_MonthX; };
	int GetMonthY() { return m_MonthY; };
	CRasterizer::ALIGN GetMonthAlign() { return m_MonthAlign; };
    const std::string& GetMonthFont() { return m_MonthFont; };
	CRasterizer::TYPE GetMonthRasterizer() { return m_MonthRasterizer; };
	COLORREF GetMonthFontColor() { return m_MonthFontColor; };

	void SetMonthEnable(bool MonthEnable ) { m_MonthEnable=MonthEnable; };
	void SetMonthBitmapName(const std::string& MonthBitmapName ) { m_MonthBitmapName=MonthBitmapName; };
	void SetMonthX(int MonthX ) { m_MonthX=MonthX; };
	void SetMonthY(int MonthY ) { m_MonthY=MonthY; };
	void SetMonthAlign(CRasterizer::ALIGN MonthAlign ) { m_MonthAlign=MonthAlign; };
	void SetMonthFont(const std::string& MonthFont ) { m_MonthFont=MonthFont; };
	void SetMonthRasterizer(CRasterizer::TYPE MonthRasterizer ) { m_MonthRasterizer=MonthRasterizer; };
	void SetMonthFontColor(COLORREF MonthFontColor ) { m_MonthFontColor=MonthFontColor; };

	// Year
	bool GetYearEnable() { return m_YearEnable; };
    const std::string& GetYearBitmapName() { return m_YearBitmapName; };
	int GetYearX() { return m_YearX; };
	int GetYearY() { return m_YearY; };
	CRasterizer::ALIGN GetYearAlign() { return m_YearAlign; };
    const std::string& GetYearFont() { return m_YearFont; };
	CRasterizer::TYPE GetYearRasterizer() { return m_YearRasterizer; };
	COLORREF GetYearFontColor() { return m_YearFontColor; };
	int GetYearSeparation() { return m_YearSeparation; };

	void SetYearEnable(bool YearEnable ) { m_YearEnable=YearEnable; };
	void SetYearBitmapName(const std::string& YearBitmapName ) { m_YearBitmapName=YearBitmapName; };
	void SetYearX(int YearX ) { m_YearX=YearX; };
	void SetYearY(int YearY ) { m_YearY=YearY; };
	void SetYearAlign(CRasterizer::ALIGN YearAlign ) { m_YearAlign=YearAlign; };
	void SetYearFont(const std::string& YearFont ) { m_YearFont=YearFont; };
	void SetYearRasterizer(CRasterizer::TYPE YearRasterizer ) { m_YearRasterizer=YearRasterizer; };
	void SetYearFontColor(COLORREF YearFontColor ) { m_YearFontColor=YearFontColor; };
	void SetYearSeparation(int separation) { m_YearSeparation = separation; };

	// Event
	bool GetEventEnable() { return m_EventEnable; };
	CRasterizer::ALIGN GetEventAlign() { return m_EventAlign; };
    const std::string& GetEventBitmapName() { return m_EventBitmapName; };
	int GetEventNumOfComponents() { return m_EventNumOfComponents; };
	CRasterizer::TYPE GetEventRasterizer() { return m_EventRasterizer; };
	COLORREF GetEventFontColor() { return m_EventFontColor; };
    const std::string& GetEventFont() { return m_EventFont; };
	bool GetEventToolTips() { return m_EventToolTips; };
	bool GetEventMessageBox() { return m_EventMessageBox; };
    const std::string& GetEventExecute() { return m_EventExecute; };
	bool GetEventInCalendar() { return m_EventInCalendar; };
	COLORREF GetEventFontColor2() { return m_EventFontColor2; };
    const std::string& GetEventFont2() { return m_EventFont2; };
	int GetEventSeparation() { return m_EventSeparation; };

	void SetEventExecute(const std::string& EventExecute ) { m_EventExecute=EventExecute; };
	void SetEventFont(const std::string& EventFont ) { m_EventFont=EventFont; };
	void SetEventToolTips(bool EventToolTips ) { m_EventToolTips=EventToolTips; };
	void SetEventMessageBox(bool EventMessageBox ) { m_EventMessageBox=EventMessageBox; };
	void SetEventEnable(bool EventEnable ) { m_EventEnable=EventEnable; };
	void SetEventAlign(CRasterizer::ALIGN EventAlign ) { m_EventAlign=EventAlign; };
	void SetEventBitmapName(const std::string& EventBitmapName ) { m_EventBitmapName=EventBitmapName; };
	void SetEventNumOfComponents(int EventNumOfComponents ) { m_EventNumOfComponents=EventNumOfComponents; };
	void SetEventRasterizer(CRasterizer::TYPE EventRasterizer ) { m_EventRasterizer=EventRasterizer; };
	void SetEventFontColor(COLORREF EventFontColor ) { m_EventFontColor=EventFontColor; };
	void SetEventInCalendar(bool EventInCalendar) { m_EventInCalendar=EventInCalendar; };
	void SetEventFont2(const std::string& EventFont2 ) { m_EventFont2=EventFont2; };
	void SetEventFontColor2(COLORREF EventFontColor2 ) { m_EventFontColor2=EventFontColor2; };
	void SetEventSeparation(int separation) { m_EventSeparation = separation; };

	// Week numbers
	bool GetWeekNumbersEnable() { return m_WeekNumbersEnable; };
    const std::string& GetWeekNumbersBitmapName() { return m_WeekNumbersBitmapName; };
	CRasterizer::ALIGN GetWeekNumbersAlign() { return m_WeekNumbersAlign; };
    const std::string& GetWeekNumbersFont() { return m_WeekNumbersFont; };
	CRasterizer::TYPE GetWeekNumbersRasterizer() { return m_WeekNumbersRasterizer; };
	COLORREF GetWeekNumbersFontColor() { return m_WeekNumbersFontColor; };
	int GetWeekNumbersNumOfComponents() { return m_WeekNumbersNumOfComponents; };
	int GetWeekNumbersSeparation() { return m_WeekNumbersSeparation; };

	void SetWeekNumbersEnable(bool WeekNumbersEnable ) { m_WeekNumbersEnable=WeekNumbersEnable; };
	void SetWeekNumbersBitmapName(const std::string& WeekNumbersBitmapName ) { m_WeekNumbersBitmapName=WeekNumbersBitmapName; };
	void SetWeekNumbersAlign(CRasterizer::ALIGN WeekNumbersAlign ) { m_WeekNumbersAlign=WeekNumbersAlign; };
	void SetWeekNumbersFont(const std::string& WeekNumbersFont ) { m_WeekNumbersFont=WeekNumbersFont; };
	void SetWeekNumbersRasterizer(CRasterizer::TYPE WeekNumbersRasterizer ) { m_WeekNumbersRasterizer=WeekNumbersRasterizer; };
	void SetWeekNumbersFontColor(COLORREF WeekNumbersFontColor ) { m_WeekNumbersFontColor=WeekNumbersFontColor; };
	void SetWeekNumbersNumOfComponents(int WeekNumbersNumOfComponents ) { m_WeekNumbersNumOfComponents=WeekNumbersNumOfComponents; };
	void SetWeekNumbersSeparation(int separation) { m_WeekNumbersSeparation = separation; };

	// Server
	UINT GetServerFrequency() { return m_ServerFrequency; };
	UINT GetServerPort() { return m_ServerPort; };
    const std::string& GetServerAddress() { return m_ServerAddress; };
    const std::string& GetServerID() { return m_ServerID; };
	bool GetServerEnable() { return m_ServerEnable; };
	bool GetServerStartup() { return m_ServerStartup; };

	void SetServerStartup(bool ServerStartup ) { m_ServerStartup=ServerStartup; };
	void SetServerFrequency(UINT ServerFrequency ) { m_ServerFrequency=ServerFrequency; };
	void SetServerPort(UINT ServerPort ) { m_ServerPort=ServerPort; };
	void SetServerAddress(const std::string& ServerAddress ) { m_ServerAddress=ServerAddress; };
	void SetServerID(const std::string& ServerID ) { m_ServerID=ServerID; };
	void SetServerEnable(bool ServerEnable ) { m_ServerEnable=ServerEnable; };

	void ReadConfig();
	void WriteConfig(WRITE_FLAGS flags);

private:
	void GetIniTime(const std::string& filename);
	bool CompareIniTime(const std::string& filename);

	void ReadGeneralConfig(const char* iniFile);
	void ReadSkinConfig(const char* iniFile);

	CRasterizer::TYPE ConvertRasterizer(const char* String);
	const char* ConvertRasterizer(CRasterizer::TYPE Type);
	void SeparateMonths();

	FILETIME m_WriteTime;	// Last write time for the events.ini

	std::string m_Path;				// Path to the .ini file
	std::string m_EventsPath;		// Path to the events.ini file
	std::string m_SkinsPath;		// Path to the skins folder
	std::string m_CurrentSkin;		// Name of the current skin path
	std::string m_CurrentSkinIni;	// Name of the current skin ini-file
	std::vector<std::string> m_MonthName;

	// General settings
	int m_X;					// Position ot the main window
	int m_Y;
	bool m_StartFromMonday;
	bool m_StartHidden;
	bool m_DisableHotkeys;
	bool m_UseWindowName;
	bool m_PollWallpaper;
	bool m_Movable;
	bool m_MouseHide;
	std::string m_MonthNames;
	std::string m_WeekdayNames;
	int m_RefreshDelay;
	WINDOWPOS m_WindowPos;
	std::string m_EventExecute;
	bool m_EventToolTips;
	bool m_EventMessageBox;

	bool m_ServerEnable;
	std::string m_ServerID;
	std::string m_ServerAddress;
	UINT m_ServerPort;
	UINT m_ServerFrequency;
	bool m_ServerStartup;

	// Skinning settings
	std::string m_BackgroundBitmapName;	// Name of the background picture
	CBackground::MODE m_BackgroundMode;

	bool m_DaysEnable;
	std::string m_DaysBitmapName;	// Name of the day number bitmap
	int m_DaysX;				// Size & position of the days
	int m_DaysY;
	int m_DaysW;
	int m_DaysH;
	int m_DaysNumOfComponents;	// Components in the bitmap
	CRasterizer::ALIGN m_DaysAlign;
	CRasterizer::TYPE m_DaysRasterizer;
	std::string m_DaysFont;
	COLORREF m_DaysFontColor;
	bool m_DaysIgnoreToday;
	bool m_DaysIgnoreEvent;
	int m_DaysSeparation;		// Separation between components

	bool m_TodayEnable;
	std::string m_TodayBitmapName;	// Name of the today bitmap
	int m_TodayNumOfComponents;	// Components in the bitmap
	CRasterizer::ALIGN m_TodayAlign;
	CRasterizer::TYPE m_TodayRasterizer;
	std::string m_TodayFont;
	COLORREF m_TodayFontColor;
	int m_TodaySeparation;		// Separation between components

	bool m_WeekdaysEnable;
	std::string m_WeekdaysBitmapName;	// Name of the today bitmap
	CRasterizer::ALIGN m_WeekdaysAlign;
	CRasterizer::TYPE m_WeekdaysRasterizer;
	std::string m_WeekdaysFont;
	COLORREF m_WeekdaysFontColor;

	bool m_MonthEnable;
	std::string m_MonthBitmapName;	// Name of the month bitmap
	int m_MonthX;				// Position of the month
	int m_MonthY;
	CRasterizer::ALIGN m_MonthAlign;
	CRasterizer::TYPE m_MonthRasterizer;
	std::string m_MonthFont;
	COLORREF m_MonthFontColor;

	bool m_YearEnable;
	std::string m_YearBitmapName;	// Name of the year bitmap
	int m_YearX;				// Position of the year
	int m_YearY;
	CRasterizer::ALIGN m_YearAlign;
	CRasterizer::TYPE m_YearRasterizer;
	std::string m_YearFont;
	COLORREF m_YearFontColor;
	int m_YearSeparation;		// Separation between components

	bool m_EventEnable;
	std::string m_EventBitmapName;	// Name of the Event bitmap
	int m_EventNumOfComponents;	// Components in the bitmap
	CRasterizer::ALIGN m_EventAlign;
	CRasterizer::TYPE m_EventRasterizer;
	std::string m_EventFont;
	COLORREF m_EventFontColor;
	bool m_EventInCalendar;
	std::string m_EventFont2;
	COLORREF m_EventFontColor2;
	int m_EventSeparation;		// Separation between components

	bool m_WeekNumbersEnable;
	std::string m_WeekNumbersBitmapName;	// Name of the today bitmap
	CRasterizer::ALIGN m_WeekNumbersAlign;
	CRasterizer::TYPE m_WeekNumbersRasterizer;
	std::string m_WeekNumbersFont;
	COLORREF m_WeekNumbersFontColor;
	int m_WeekNumbersNumOfComponents;	// Components in the bitmap
	int m_WeekNumbersSeparation;		// Separation between components
};

#endif

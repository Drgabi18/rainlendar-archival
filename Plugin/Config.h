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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Config.h,v 1.5 2002/02/27 18:57:15 rainy Exp $

  $Log: Config.h,v $
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

#if !defined(AFX_CONFIG_H__C7D0FDBB_F062_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_CONFIG_H__C7D0FDBB_F062_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Rasterizer.h"
#include "Background.h"

class CConfig  
{
public:
	enum WINDOWPOS 
	{
		WINDOWPOS_ONBOTTOM,
		WINDOWPOS_NORMAL,
		WINDOWPOS_ONTOP
	};

	CConfig();
	~CConfig();

	// General
	int GetX() { return m_X; };
	int GetY() { return m_Y; };
	CString& GetPath() { return m_Path; };
	bool GetStartFromMonday() { return m_StartFromMonday; };
	CString& GetWeekdayNames() { return m_WeekdayNames; };
	CString& GetMonthNames() { return m_MonthNames; };
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
	void SetPath(CString& Path) { m_Path=Path; };
	void SetStartFromMonday(bool StartFromMonday ) { m_StartFromMonday=StartFromMonday; };
	void SetWeekdayNames(CString& WeekdayNames ) { m_WeekdayNames=WeekdayNames; };
	void SetMonthNames(CString& MonthNames ) { m_MonthNames=MonthNames; };
	void SetRefreshDelay(int RefreshDelay ) { m_RefreshDelay=RefreshDelay; };
	void SetStartHidden(bool StartHidden ) { m_StartHidden=StartHidden; };
	void SetDisableHotkeys(bool DisableHotkeys ) { m_DisableHotkeys=DisableHotkeys; };
	void SetUseWindowName(bool UseWindowName ) { m_UseWindowName=UseWindowName; };
	void SetPollWallpaper(bool PollWallpaper ) { m_PollWallpaper=PollWallpaper; };
	void SetMovable(bool Movable) { m_Movable=Movable; };
	void SetMouseHide(bool MouseHide) { m_MouseHide=MouseHide; };
	void SetBackgroundMode(CBackground::MODE BackgroundMode ) { m_BackgroundMode=BackgroundMode; };
	void SetWindowPos(WINDOWPOS WindowPos) { m_WindowPos=WindowPos; };

	CString& GetBackgroundBitmapName() { return m_BackgroundBitmapName; };
	void SetBackgroundBitmapName(CString& BackgroundBitmapName ) { m_BackgroundBitmapName=BackgroundBitmapName; };

	// Days
	bool GetDaysEnable() { return m_DaysEnable; };
	int GetDaysH() { return m_DaysH; };
	int GetDaysW() { return m_DaysW; };
	int GetDaysY() { return m_DaysY; };
	int GetDaysX() { return m_DaysX; };
	CString& GetDaysBitmapName() { return m_DaysBitmapName; };
	int GetDaysNumOfComponents() { return m_DaysNumOfComponents; };
	CRasterizer::ALIGN GetDaysAlign() { return m_DaysAlign; };
	CString& GetDaysFont() { return m_DaysFont; };
	CRasterizer::TYPE GetDaysRasterizer() { return m_DaysRasterizer; };
	COLORREF GetDaysFontColor() { return m_DaysFontColor; };
	bool GetDaysIgnoreToday() { return m_DaysIgnoreToday; };
	bool GetDaysIgnoreEvent() { return m_DaysIgnoreEvent; };

	void SetDaysEnable(bool DaysEnable ) { m_DaysEnable=DaysEnable; };
	void SetDaysH(int DaysH ) { m_DaysH=DaysH; };
	void SetDaysW(int DaysW ) { m_DaysW=DaysW; };
	void SetDaysY(int DaysY ) { m_DaysY=DaysY; };
	void SetDaysX(int DaysX ) { m_DaysX=DaysX; };
	void SetDaysBitmapName(CString& DaysBitmapName ) { m_DaysBitmapName=DaysBitmapName; };
	void SetDaysNumOfComponents(int DaysNumOfComponents ) { m_DaysNumOfComponents=DaysNumOfComponents; };
	void SetDaysAlign(CRasterizer::ALIGN DaysAlign ) { m_DaysAlign=DaysAlign; };
	void SetDaysFont(CString& DaysFont ) { m_DaysFont=DaysFont; };
	void SetDaysRasterizer(CRasterizer::TYPE DaysRasterizer ) { m_DaysRasterizer=DaysRasterizer; };
	void SetDaysFontColor(COLORREF DaysFontColor ) { m_DaysFontColor=DaysFontColor; };
	void SetDaysIgnoreToday(bool DaysIgnoreToday ) { m_DaysIgnoreToday=DaysIgnoreToday; };
	void SetDaysIgnoreEvent(bool DaysIgnoreEvent ) { m_DaysIgnoreEvent=DaysIgnoreEvent; };

	// Today
	bool GetTodayEnable() { return m_TodayEnable; };
	CRasterizer::ALIGN GetTodayAlign() { return m_TodayAlign; };
	CString& GetTodayBitmapName() { return m_TodayBitmapName; };
	int GetTodayNumOfComponents() { return m_TodayNumOfComponents; };
	CRasterizer::TYPE GetTodayRasterizer() { return m_TodayRasterizer; };
	COLORREF GetTodayFontColor() { return m_TodayFontColor; };
	CString GetTodayFont() { return m_TodayFont; };

	void SetTodayEnable(bool TodayEnable ) { m_TodayEnable=TodayEnable; };
	void SetTodayAlign(CRasterizer::ALIGN TodayAlign ) { m_TodayAlign=TodayAlign; };
	void SetTodayBitmapName(CString& TodayBitmapName ) { m_TodayBitmapName=TodayBitmapName; };
	void SetTodayNumOfComponents(int TodayNumOfComponents ) { m_TodayNumOfComponents=TodayNumOfComponents; };
	void SetTodayRasterizer(CRasterizer::TYPE TodayRasterizer ) { m_TodayRasterizer=TodayRasterizer; };
	void SetTodayFontColor(COLORREF TodayFontColor ) { m_TodayFontColor=TodayFontColor; };
	void SetTodayFont(CString TodayFont ) { m_TodayFont=TodayFont; };

	// Weekdays
	bool GetWeekdaysEnable() { return m_WeekdaysEnable; };
	CString& GetWeekdaysBitmapName() { return m_WeekdaysBitmapName; };
	CRasterizer::ALIGN GetWeekdaysAlign() { return m_WeekdaysAlign; };
	CString& GetWeekdaysFont() { return m_WeekdaysFont; };
	CRasterizer::TYPE GetWeekdaysRasterizer() { return m_WeekdaysRasterizer; };
	COLORREF GetWeekdaysFontColor() { return m_WeekdaysFontColor; };

	void SetWeekdaysEnable(bool WeekdaysEnable ) { m_WeekdaysEnable=WeekdaysEnable; };
	void SetWeekdaysBitmapName(CString& WeekdaysBitmapName ) { m_WeekdaysBitmapName=WeekdaysBitmapName; };
	void SetWeekdaysAlign(CRasterizer::ALIGN WeekdaysAlign ) { m_WeekdaysAlign=WeekdaysAlign; };
	void SetWeekdaysFont(CString& WeekdaysFont ) { m_WeekdaysFont=WeekdaysFont; };
	void SetWeekdaysRasterizer(CRasterizer::TYPE WeekdaysRasterizer ) { m_WeekdaysRasterizer=WeekdaysRasterizer; };
	void SetWeekdaysFontColor(COLORREF WeekdaysFontColor ) { m_WeekdaysFontColor=WeekdaysFontColor; };

	// Month
	bool GetMonthEnable() { return m_MonthEnable; };
	CString& GetMonthBitmapName() { return m_MonthBitmapName; };
	int GetMonthX() { return m_MonthX; };
	int GetMonthY() { return m_MonthY; };
	CRasterizer::ALIGN GetMonthAlign() { return m_MonthAlign; };
	CString& GetMonthFont() { return m_MonthFont; };
	CRasterizer::TYPE GetMonthRasterizer() { return m_MonthRasterizer; };
	COLORREF GetMonthFontColor() { return m_MonthFontColor; };

	void SetMonthEnable(bool MonthEnable ) { m_MonthEnable=MonthEnable; };
	void SetMonthBitmapName(CString& MonthBitmapName ) { m_MonthBitmapName=MonthBitmapName; };
	void SetMonthX(int MonthX ) { m_MonthX=MonthX; };
	void SetMonthY(int MonthY ) { m_MonthY=MonthY; };
	void SetMonthAlign(CRasterizer::ALIGN MonthAlign ) { m_MonthAlign=MonthAlign; };
	void SetMonthFont(CString& MonthFont ) { m_MonthFont=MonthFont; };
	void SetMonthRasterizer(CRasterizer::TYPE MonthRasterizer ) { m_MonthRasterizer=MonthRasterizer; };
	void SetMonthFontColor(COLORREF MonthFontColor ) { m_MonthFontColor=MonthFontColor; };

	// Year
	bool GetYearEnable() { return m_YearEnable; };
	CString& GetYearBitmapName() { return m_YearBitmapName; };
	int GetYearX() { return m_YearX; };
	int GetYearY() { return m_YearY; };
	CRasterizer::ALIGN GetYearAlign() { return m_YearAlign; };
	CString& GetYearFont() { return m_YearFont; };
	CRasterizer::TYPE GetYearRasterizer() { return m_YearRasterizer; };
	COLORREF GetYearFontColor() { return m_YearFontColor; };

	void SetYearEnable(bool YearEnable ) { m_YearEnable=YearEnable; };
	void SetYearBitmapName(CString& YearBitmapName ) { m_YearBitmapName=YearBitmapName; };
	void SetYearX(int YearX ) { m_YearX=YearX; };
	void SetYearY(int YearY ) { m_YearY=YearY; };
	void SetYearAlign(CRasterizer::ALIGN YearAlign ) { m_YearAlign=YearAlign; };
	void SetYearFont(CString& YearFont ) { m_YearFont=YearFont; };
	void SetYearRasterizer(CRasterizer::TYPE YearRasterizer ) { m_YearRasterizer=YearRasterizer; };
	void SetYearFontColor(COLORREF YearFontColor ) { m_YearFontColor=YearFontColor; };

	// Event
	bool GetEventEnable() { return m_EventEnable; };
	CRasterizer::ALIGN GetEventAlign() { return m_EventAlign; };
	CString& GetEventBitmapName() { return m_EventBitmapName; };
	int GetEventNumOfComponents() { return m_EventNumOfComponents; };
	CRasterizer::TYPE GetEventRasterizer() { return m_EventRasterizer; };
	COLORREF GetEventFontColor() { return m_EventFontColor; };
	CString& GetEventFont() { return m_EventFont; };
	bool GetEventToolTips() { return m_EventToolTips; };
	bool GetEventMessageBox() { return m_EventMessageBox; };
	CString& GetEventExecute() { return m_EventExecute; };
	bool GetEventInCalendar() { return m_EventInCalendar; };
	COLORREF GetEventFontColor2() { return m_EventFontColor2; };
	CString& GetEventFont2() { return m_EventFont2; };

	void SetEventExecute(CString& EventExecute ) { m_EventExecute=EventExecute; };
	void SetEventFont(CString& EventFont ) { m_EventFont=EventFont; };
	void SetEventToolTips(bool EventToolTips ) { m_EventToolTips=EventToolTips; };
	void SetEventMessageBox(bool EventMessageBox ) { m_EventMessageBox=EventMessageBox; };
	void SetEventEnable(bool EventEnable ) { m_EventEnable=EventEnable; };
	void SetEventAlign(CRasterizer::ALIGN EventAlign ) { m_EventAlign=EventAlign; };
	void SetEventBitmapName(CString& EventBitmapName ) { m_EventBitmapName=EventBitmapName; };
	void SetEventNumOfComponents(int EventNumOfComponents ) { m_EventNumOfComponents=EventNumOfComponents; };
	void SetEventRasterizer(CRasterizer::TYPE EventRasterizer ) { m_EventRasterizer=EventRasterizer; };
	void SetEventFontColor(COLORREF EventFontColor ) { m_EventFontColor=EventFontColor; };
	void SetEventInCalendar(bool EventInCalendar) { m_EventInCalendar=EventInCalendar; };
	void SetEventFont2(CString& EventFont2 ) { m_EventFont2=EventFont2; };
	void SetEventFontColor2(COLORREF EventFontColor2 ) { m_EventFontColor2=EventFontColor2; };

	// Week numbers
	bool GetWeekNumbersEnable() { return m_WeekNumbersEnable; };
	CString& GetWeekNumbersBitmapName() { return m_WeekNumbersBitmapName; };
	CRasterizer::ALIGN GetWeekNumbersAlign() { return m_WeekNumbersAlign; };
	CString& GetWeekNumbersFont() { return m_WeekNumbersFont; };
	CRasterizer::TYPE GetWeekNumbersRasterizer() { return m_WeekNumbersRasterizer; };
	COLORREF GetWeekNumbersFontColor() { return m_WeekNumbersFontColor; };
	int GetWeekNumbersNumOfComponents() { return m_WeekNumbersNumOfComponents; };

	void SetWeekNumbersEnable(bool WeekNumbersEnable ) { m_WeekNumbersEnable=WeekNumbersEnable; };
	void SetWeekNumbersBitmapName(CString& WeekNumbersBitmapName ) { m_WeekNumbersBitmapName=WeekNumbersBitmapName; };
	void SetWeekNumbersAlign(CRasterizer::ALIGN WeekNumbersAlign ) { m_WeekNumbersAlign=WeekNumbersAlign; };
	void SetWeekNumbersFont(CString& WeekNumbersFont ) { m_WeekNumbersFont=WeekNumbersFont; };
	void SetWeekNumbersRasterizer(CRasterizer::TYPE WeekNumbersRasterizer ) { m_WeekNumbersRasterizer=WeekNumbersRasterizer; };
	void SetWeekNumbersFontColor(COLORREF WeekNumbersFontColor ) { m_WeekNumbersFontColor=WeekNumbersFontColor; };
	void SetWeekNumbersNumOfComponents(int WeekNumbersNumOfComponents ) { m_WeekNumbersNumOfComponents=WeekNumbersNumOfComponents; };

	// Server
	UINT GetServerFrequency() { return m_ServerFrequency; };
	UINT GetServerPort() { return m_ServerPort; };
	CString& GetServerAddress() { return m_ServerAddress; };
	CString& GetServerID() { return m_ServerID; };
	bool GetServerEnable() { return m_ServerEnable; };
	bool GetServerStartup() { return m_ServerStartup; };

	void SetServerStartup(bool ServerStartup ) { m_ServerStartup=ServerStartup; };
	void SetServerFrequency(UINT ServerFrequency ) { m_ServerFrequency=ServerFrequency; };
	void SetServerPort(UINT ServerPort ) { m_ServerPort=ServerPort; };
	void SetServerAddress(CString& ServerAddress ) { m_ServerAddress=ServerAddress; };
	void SetServerID(CString& ServerID ) { m_ServerID=ServerID; };
	void SetServerEnable(bool ServerEnable ) { m_ServerEnable=ServerEnable; };

	void ReadConfig();
	void WriteConfig();

private:
	CRasterizer::TYPE ConvertRasterizer(char* String);
	char* ConvertRasterizer(CRasterizer::TYPE Type);

	CString m_Path;				// Path to the .ini file

	int m_X;					// Position ot the main window
	int m_Y;
	bool m_StartFromMonday;
	bool m_StartHidden;
	bool m_DisableHotkeys;
	bool m_UseWindowName;
	bool m_PollWallpaper;
	bool m_Movable;
	bool m_MouseHide;
	CString m_BackgroundBitmapName;	// Name of the background picture
	CString m_MonthNames;
	CString m_WeekdayNames;
	int m_RefreshDelay;
	CBackground::MODE m_BackgroundMode;
	WINDOWPOS m_WindowPos;

	bool m_DaysEnable;
	CString m_DaysBitmapName;	// Name of the day number bitmap
	int m_DaysX;				// Size & position of the days
	int m_DaysY;
	int m_DaysW;
	int m_DaysH;
	int m_DaysNumOfComponents;	// Components in the bitmap
	CRasterizer::ALIGN m_DaysAlign;
	CRasterizer::TYPE m_DaysRasterizer;
	CString m_DaysFont;
	COLORREF m_DaysFontColor;
	bool m_DaysIgnoreToday;
	bool m_DaysIgnoreEvent;

	bool m_TodayEnable;
	CString m_TodayBitmapName;	// Name of the today bitmap
	int m_TodayNumOfComponents;	// Components in the bitmap
	CRasterizer::ALIGN m_TodayAlign;
	CRasterizer::TYPE m_TodayRasterizer;
	CString m_TodayFont;
	COLORREF m_TodayFontColor;

	bool m_WeekdaysEnable;
	CString m_WeekdaysBitmapName;	// Name of the today bitmap
	CRasterizer::ALIGN m_WeekdaysAlign;
	CRasterizer::TYPE m_WeekdaysRasterizer;
	CString m_WeekdaysFont;
	COLORREF m_WeekdaysFontColor;

	bool m_MonthEnable;
	CString m_MonthBitmapName;	// Name of the month bitmap
	int m_MonthX;				// Position of the month
	int m_MonthY;
	CRasterizer::ALIGN m_MonthAlign;
	CRasterizer::TYPE m_MonthRasterizer;
	CString m_MonthFont;
	COLORREF m_MonthFontColor;

	bool m_YearEnable;
	CString m_YearBitmapName;	// Name of the year bitmap
	int m_YearX;				// Position of the year
	int m_YearY;
	CRasterizer::ALIGN m_YearAlign;
	CRasterizer::TYPE m_YearRasterizer;
	CString m_YearFont;
	COLORREF m_YearFontColor;

	bool m_EventEnable;
	CString m_EventBitmapName;	// Name of the Event bitmap
	int m_EventNumOfComponents;	// Components in the bitmap
	CRasterizer::ALIGN m_EventAlign;
	CRasterizer::TYPE m_EventRasterizer;
	CString m_EventFont;
	COLORREF m_EventFontColor;
	CString m_EventExecute;
	bool m_EventToolTips;
	bool m_EventMessageBox;
	bool m_EventInCalendar;
	CString m_EventFont2;
	COLORREF m_EventFontColor2;

	bool m_WeekNumbersEnable;
	CString m_WeekNumbersBitmapName;	// Name of the today bitmap
	CRasterizer::ALIGN m_WeekNumbersAlign;
	CRasterizer::TYPE m_WeekNumbersRasterizer;
	CString m_WeekNumbersFont;
	COLORREF m_WeekNumbersFontColor;
	int m_WeekNumbersNumOfComponents;	// Components in the bitmap

	bool m_ServerEnable;
	CString m_ServerID;
	CString m_ServerAddress;
	UINT m_ServerPort;
	UINT m_ServerFrequency;
	bool m_ServerStartup;
};

#endif // !defined(AFX_CONFIG_H__C7D0FDBB_F062_11D3_92A3_0080AD90417B__INCLUDED_)

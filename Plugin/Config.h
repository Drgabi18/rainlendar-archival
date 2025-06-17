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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/Config.h,v 1.14 2003/08/09 16:38:43 Rainy Exp $

  $Log: Config.h,v $
  Revision 1.14  2003/08/09 16:38:43  Rainy
  Added hotkeys and few other settings.

  Revision 1.13  2003/06/15 09:43:01  Rainy
  Added Layout stuff.

  Revision 1.12  2003/05/25 18:08:44  Rainy
  Added tooltip separator.

  Revision 1.11  2003/05/07 19:15:35  rainy
  Added few new options.

  Revision 1.10  2003/03/22 20:31:01  rainy
  Refresh on resolution change is optional.

  Revision 1.9  2002/11/25 17:11:34  rainy
  Added some stuff to the profiles.

  Revision 1.8  2002/11/12 18:02:33  rainy
  Added Native Transparency and solid background (with bevel).

  Revision 1.7  2002/08/03 16:19:06  rainy
  Added some new configs.
  Added support for profiles.

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
#include <list>

struct Profile
{
	std::string name;
	COLORREF toolTipColor;
	COLORREF fontColor;
	COLORREF fontColor2;
	std::string bitmapName;
	CImage bitmap;
	std::string iconName;
	CRasterizer::ALIGN iconAlign;
	CImage icon;
};

class CConfig  
{
public:
	enum WRITE_FLAGS
	{
		WRITE_POS = 1,
		WRITE_CONFIG = 2,
		WRITE_PROFILE = 4,
		WRITE_FULL = 7,		// FULL is 1 + 2 + 4
		WRITE_SKIN = 8
	};

	enum WINDOWPOS 
	{
		WINDOWPOS_ONBOTTOM,
		WINDOWPOS_NORMAL,
		WINDOWPOS_ONTOP,
		WINDOWPOS_ONDESKTOP
	};

	enum BG_COPY_MODE
	{
		BG_NORMAL,
		BG_COPY_ALWAYS,
		BG_WALLPAPER_ALWAYS
	};

	enum DIALOG_TYPE
	{
		DIALOG_EDITSKIN,
		DIALOG_CONFIG,
		DIALOG_EDITEVENT,
		DIALOG_ALLEVENTS,
		DIALOG_ALLEVENTS_SIZE,
		DIALOG_LAST		// This must be last
	};

	enum HOTKEY
	{
		HOTKEY_HIDE,
		HOTKEY_SHOW,
		HOTKEY_TOGGLE,
		HOTKEY_ACTIVATE,
		HOTKEY_REFRESH,
		HOTKEY_CONFIG,
		HOTKEY_SKIN,
		HOTKEY_NEXT,
		HOTKEY_PREVIOUS,
		HOTKEY_NEXT_YEAR,
		HOTKEY_PREVIOUS_YEAR,
		HOTKEY_CURRENT,
		HOTKEY_ALL,
		HOTKEY_OUTLOOK,
		HOTKEY_LAST		// This must be last
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
	const std::string& GetCurrentLanguage() { return m_CurrentLanguage; };
	void SetCurrentLanguage(const std::string& CurrentLanguage) { m_CurrentLanguage=CurrentLanguage; };

	const std::string GetMonthName(int index) { return m_MonthName[index]; };

	const std::string& GetCurrentProfile() { return m_CurrentProfile; };
	void SetCurrentProfile(const std::string& CurrentProfile) { m_CurrentProfile=CurrentProfile; };

	POINT GetDialogPosition(DIALOG_TYPE type);
	void SetDialogPosition(DIALOG_TYPE type, int x, int y);

	// Layout
	int GetX() { return m_X; };
	int GetY() { return m_Y; };
	WINDOWPOS GetWindowPos() { return m_WindowPos; };
	bool GetMouseHide() { return m_MouseHide; };
	bool GetMovable() { return m_Movable; };
	UINT GetVerticalCount() { return m_VerticalCount; };
	UINT GetHorizontalCount() { return m_HorizontalCount; };
	UINT GetPreviousMonths();
	bool GetStartFromJanuary() { return m_StartFromJanuary; };

	void SetX(int X ) { m_X=X; };
	void SetY(int Y ) { m_Y=Y; };
	void SetWindowPos(WINDOWPOS WindowPos) { m_WindowPos=WindowPos; };
	void SetMovable(bool Movable) { m_Movable=Movable; };
	void SetMouseHide(bool MouseHide) { m_MouseHide=MouseHide; };
	void SetVerticalCount(UINT VerticalCount) { m_VerticalCount=VerticalCount; };
	void SetHorizontalCount(UINT HorizontalCount) { m_HorizontalCount=HorizontalCount; };
	void SetPreviousMonths(UINT PreviousMonths) { m_PreviousMonths=PreviousMonths; };
	void SetStartFromJanuary(bool StartFromJanuary) { m_StartFromJanuary=StartFromJanuary; };

	// General
	bool GetStartFromMonday() { return m_StartFromMonday; };
    const std::string& GetWeekdayNames() { return m_WeekdayNames; };
    const std::string& GetMonthNames() { return m_MonthNames; };
	int GetRefreshDelay() { return m_RefreshDelay; };
	int GetOutlookUpdate() { return m_OutlookUpdate; };
	bool GetStartHidden() { return m_StartHidden; };
	bool GetDisableHotkeys() { return m_DisableHotkeys; };
	bool GetUseWindowName() { return m_UseWindowName; };
	bool GetPollWallpaper() { return m_PollWallpaper; };
	bool GetSnapEdges() { return m_SnapEdges; };
	bool GetNativeTransparency() { return m_NativeTransparency; };
	bool GetRefreshOnResolutionChange() { return m_RefreshOnResolutionChange; };
	bool GetShowOutlookAppointments() { return m_ShowOutlookAppointments; };
	bool GetWeek1HasJanuary1st() { return m_Week1HasJanuary1st; };
	bool GetNegativeCoords() { return m_NegativeCoords; };
	bool GetRememberDialogPositions() { return m_RememberDialogPositions; };
	BG_COPY_MODE GetBGCopyMode() { return m_BGCopyMode; };

	void SetStartFromMonday(bool StartFromMonday ) { m_StartFromMonday=StartFromMonday; };
	void SetWeekdayNames(const std::string& WeekdayNames ) { m_WeekdayNames=WeekdayNames; };
	void SetMonthNames(const std::string& MonthNames ) { m_MonthNames=MonthNames; };
	void SetRefreshDelay(int RefreshDelay ) { m_RefreshDelay=RefreshDelay; };
	void SetOutlookUpdate(int OutlookUpdate ) { m_OutlookUpdate=OutlookUpdate; };
	void SetStartHidden(bool StartHidden ) { m_StartHidden=StartHidden; };
	void SetDisableHotkeys(bool DisableHotkeys ) { m_DisableHotkeys=DisableHotkeys; };
	void SetUseWindowName(bool UseWindowName ) { m_UseWindowName=UseWindowName; };
	void SetPollWallpaper(bool PollWallpaper ) { m_PollWallpaper=PollWallpaper; };
	void SetSnapEdges(bool SnapEdges) { m_SnapEdges=SnapEdges; };
	void SetNativeTransparency(bool NativeTransparency) { m_NativeTransparency=NativeTransparency; };
	void SetRefreshOnResolutionChange(bool RefreshOnResolutionChange) { m_RefreshOnResolutionChange=RefreshOnResolutionChange; };
	void SetShowOutlookAppointments(bool ShowOutlookAppointments) { m_ShowOutlookAppointments=ShowOutlookAppointments; };
	void SetWeek1HasJanuary1st(bool Week1HasJanuary1st) { m_Week1HasJanuary1st=Week1HasJanuary1st; };
	void SetRememberDialogPositions(bool RememberDialogPositions) { m_RememberDialogPositions=RememberDialogPositions; };
	void SetNegativeCoords(bool NegativeCoords) { m_NegativeCoords=NegativeCoords; };
	void SetBGCopyMode(BG_COPY_MODE bgMode) { m_BGCopyMode=bgMode; };

	CBackground::MODE GetBackgroundMode() { return m_BackgroundMode; };
    const std::string& GetBackgroundBitmapName() { return m_BackgroundBitmapName; };
	bool GetBackgroundBevel() { return m_BackgroundBevel; };
	COLORREF GetBackgroundSolidColor() { return m_BackgroundSolidColor; };

	void SetBackgroundMode(CBackground::MODE BackgroundMode ) { m_BackgroundMode=BackgroundMode; };
	void SetBackgroundBitmapName(const std::string& BackgroundBitmapName ) { m_BackgroundBitmapName=BackgroundBitmapName; };
	void SetBackgroundBevel(bool BackgroundBevel ) { m_BackgroundBevel=BackgroundBevel; };
	void SetBackgroundSolidColor(COLORREF BackgroundSolidColor ) { m_BackgroundSolidColor=BackgroundSolidColor; };

	// Hotkeys
	DWORD GetHideHotkey() { return m_HideHotkey; };
	void SetHideHotkey(DWORD HideHotkey) { m_HideHotkey=HideHotkey; };
	DWORD GetShowHotkey() { return m_ShowHotkey; };
	void SetShowHotkey(DWORD ShowHotkey) { m_ShowHotkey=ShowHotkey; };
	DWORD GetToggleHotkey() { return m_ToggleHotkey; };
	void SetToggleHotkey(DWORD ToggleHotkey) { m_ToggleHotkey=ToggleHotkey; };
	DWORD GetActivateHotkey() { return m_ActivateHotkey; };
	void SetActivateHotkey(DWORD ActivateHotkey) { m_ActivateHotkey=ActivateHotkey; };
	DWORD GetRefreshHotkey() { return m_RefreshHotkey; };
	void SetRefreshHotkey(DWORD RefreshHotkey) { m_RefreshHotkey=RefreshHotkey; };
	DWORD GetConfigHotkey() { return m_ConfigHotkey; };
	void SetConfigHotkey(DWORD ConfigHotkey) { m_ConfigHotkey=ConfigHotkey; };
	DWORD GetSkinHotkey() { return m_SkinHotkey; };
	void SetSkinHotkey(DWORD SkinHotkey) { m_SkinHotkey=SkinHotkey; };
	DWORD GetNextHotkey() { return m_NextHotkey; };
	void SetNextHotkey(DWORD NextHotkey) { m_NextHotkey=NextHotkey; };
	DWORD GetPreviousHotkey() { return m_PreviousHotkey; };
	void SetPreviousHotkey(DWORD PreviousHotkey) { m_PreviousHotkey=PreviousHotkey; };
	DWORD GetNextYearHotkey() { return m_NextYearHotkey; };
	void SetNextYearHotkey(DWORD NextHotkey) { m_NextYearHotkey=NextHotkey; };
	DWORD GetPreviousYearHotkey() { return m_PreviousYearHotkey; };
	void SetPreviousYearHotkey(DWORD PreviousHotkey) { m_PreviousYearHotkey=PreviousHotkey; };
	DWORD GetCurrentHotkey() { return m_CurrentHotkey; };
	void SetCurrentHotkey(DWORD CurrentHotkey) { m_CurrentHotkey=CurrentHotkey; };
	DWORD GetAllHotkey() { return m_AllHotkey; };
	void SetAllHotkey(DWORD AllHotkey) { m_AllHotkey=AllHotkey; };
	DWORD GetOutlookHotkey() { return m_OutlookHotkey; };
	void SetOutlookHotkey(DWORD OutlookHotkey) { m_OutlookHotkey=OutlookHotkey; };

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
	bool GetServerSyncOnEdit() { return m_ServerSyncOnEdit; };

	void SetServerStartup(bool ServerStartup ) { m_ServerStartup=ServerStartup; };
	void SetServerFrequency(UINT ServerFrequency ) { m_ServerFrequency=ServerFrequency; };
	void SetServerPort(UINT ServerPort ) { m_ServerPort=ServerPort; };
	void SetServerAddress(const std::string& ServerAddress ) { m_ServerAddress=ServerAddress; };
	void SetServerID(const std::string& ServerID ) { m_ServerID=ServerID; };
	void SetServerEnable(bool ServerEnable ) { m_ServerEnable=ServerEnable; };
	void SetServerSyncOnEdit(bool ServerSyncOnEdit ) { m_ServerSyncOnEdit=ServerSyncOnEdit; };

	void SetToolTipFontColor(COLORREF ToolTipFontColor ) { m_ToolTipFontColor=ToolTipFontColor; };
	COLORREF GetToolTipFontColor() { return m_ToolTipFontColor; };
	void SetToolTipBGColor(COLORREF ToolTipBGColor ) { m_ToolTipBGColor=ToolTipBGColor; };
	COLORREF GetToolTipBGColor() { return m_ToolTipBGColor; };
	void SetToolTipFont(const std::string& ToolTipFont ) { m_ToolTipFont=ToolTipFont; };
    const std::string& GetToolTipFont() { return m_ToolTipFont; };
	void SetTooltipSeparator(bool TooltipSeparator) { m_TooltipSeparator=TooltipSeparator; };
    bool GetTooltipSeparator() { return m_TooltipSeparator; };

	void ReadConfig();
	void WriteConfig(WRITE_FLAGS flags);

	const Profile* GetProfile(const char* name);
	const std::list<Profile*>& GetAllProfiles();

	static void AddPath(std::string& filename);
private:
	void GetIniTime(const std::string& filename);
	bool CompareIniTime(const std::string& filename);

	void ReadGeneralConfig(const char* iniFile);
	void ReadSkinConfig(const char* iniFile);
	void ReadProfiles(const char* iniFile);

	CRasterizer::TYPE ConvertRasterizer(const char* String);
	const char* ConvertRasterizer(CRasterizer::TYPE Type);
	void SeparateMonths();

	std::list<Profile*> m_Profiles;

	FILETIME m_WriteTime;	// Last write time for the events.ini

	std::string m_Path;				// Path to the .ini file
	std::string m_EventsPath;		// Path to the events.ini file
	std::string m_SkinsPath;		// Path to the skins folder
	std::string m_CurrentLanguage;	// Name of the current language
	std::string m_CurrentSkin;		// Name of the current skin path
	std::string m_CurrentSkinIni;	// Name of the current skin ini-file
	std::vector<std::string> m_MonthName;

	// Dialog positions
	POINT m_DialogPos[DIALOG_LAST];

	// Layout settings
	int m_X;					// Position ot the main window
	int m_Y;
	WINDOWPOS m_WindowPos;
	bool m_MouseHide;
	bool m_Movable;
	UINT m_VerticalCount;
	UINT m_HorizontalCount;
	UINT m_PreviousMonths;
	bool m_StartFromJanuary;
	bool m_RememberDialogPositions;
	bool m_NegativeCoords;

	// General settings
	bool m_StartFromMonday;
	bool m_StartHidden;
	bool m_DisableHotkeys;
	bool m_UseWindowName;
	bool m_PollWallpaper;
	bool m_SnapEdges;
	bool m_NativeTransparency;
	bool m_RefreshOnResolutionChange;
	bool m_ShowOutlookAppointments;
	int m_OutlookUpdate;
	bool m_Week1HasJanuary1st;
	std::string m_MonthNames;
	std::string m_WeekdayNames;
	int m_RefreshDelay;
	std::string m_EventExecute;
	bool m_EventToolTips;
	bool m_EventMessageBox;
	BG_COPY_MODE m_BGCopyMode;
	std::string m_CurrentProfile;

	// Hotkey settings
	DWORD m_HideHotkey;
	DWORD m_ShowHotkey;
	DWORD m_ToggleHotkey;
	DWORD m_ActivateHotkey;
	DWORD m_RefreshHotkey;
	DWORD m_ConfigHotkey;
	DWORD m_SkinHotkey;
	DWORD m_NextHotkey;
	DWORD m_PreviousHotkey;
	DWORD m_NextYearHotkey;
	DWORD m_PreviousYearHotkey;
	DWORD m_CurrentHotkey;
	DWORD m_AllHotkey;
	DWORD m_OutlookHotkey;

	// Server settings
	bool m_ServerEnable;
	std::string m_ServerID;
	std::string m_ServerAddress;
	UINT m_ServerPort;
	UINT m_ServerFrequency;
	bool m_ServerStartup;
	bool m_ServerSyncOnEdit;

	// Skinning settings
	std::string m_BackgroundBitmapName;	// Name of the background picture
	CBackground::MODE m_BackgroundMode;
	COLORREF m_BackgroundSolidColor;
	bool m_BackgroundBevel;

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

	COLORREF m_ToolTipFontColor;
	COLORREF m_ToolTipBGColor;
	std::string m_ToolTipFont;
	bool m_TooltipSeparator;
};

#endif

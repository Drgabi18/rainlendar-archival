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
  $Header: /home/cvsroot/Rainlendar/Library/Config.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: Config.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.27  2005/03/25 13:58:43  rainy
  no message

  Revision 1.26  2005/03/01 18:49:28  rainy
  Removed skin stuff.

  Revision 1.25  2004/12/05 18:34:12  rainy
  Added some more stuff.

  Revision 1.24  2004/11/06 13:38:58  rainy
  no message

  Revision 1.23  2004/06/10 15:25:29  rainy
  Added keepalive

  Revision 1.22  2004/04/24 11:17:36  rainy
  Added outlook profiles.

  Revision 1.21  2004/01/28 18:04:55  rainy
  Added tray executes.

  Revision 1.20  2004/01/25 09:59:27  rainy
  Added new dialog position.

  Revision 1.19  2004/01/10 15:16:36  rainy
  Added tray and hotkey for todo.

  Revision 1.18  2003/12/20 22:24:39  rainy
  Added stuff for the message box.

  Revision 1.17  2003/12/05 15:45:42  Rainy
  Added Reset()

  Revision 1.16  2003/10/27 17:36:51  Rainy
  Config is now singleton.
  Added todo stuff.

  Revision 1.15  2003/10/04 14:48:26  Rainy
  Added TooltipMaxWidth and priority for the profiles.

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
  Changed CEvent to CEntryEvent to avoid name clash.
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
#include <set>

struct Profile
{
	std::string name;
	COLORREF toolTipColor;
	COLORREF fontColor;
	COLORREF fontColor2;
	COLORREF eventColor;
	std::string bitmapName;
	CImage bitmap;
	std::string iconName;
	CRasterizer::ALIGN iconAlign;
	CImage icon;
	bool drawAlways;
	int priority;
};

class CConfig
{
public:
	static CConfig& Instance();
	static void DeleteInstance();

	enum WRITE_FLAGS
	{
		WRITE_POS = 1,
		WRITE_TODOPOS = 2,
		WRITE_EVENTLISTPOS = 4,
		WRITE_CONFIG = 8,
		WRITE_PROFILE = 16,
		WRITE_DIALOG_POS = 32,
		WRITE_WINDOW_STATE = 64,
		WRITE_FULL = 127,		// FULL is 1 + 2 + 4 + 8 + 16 + 32 + ...
	};

	enum WINDOWPOS 
	{
		WINDOWPOS_ONBOTTOM,
		WINDOWPOS_NORMAL,
		WINDOWPOS_ONTOP,
		WINDOWPOS_ONDESKTOP
	};

	enum DIALOG_TYPE
	{
		DIALOG_EDITSKIN,
		DIALOG_CONFIG,
		DIALOG_EDITEVENT,
		DIALOG_ALLEVENTS,
		DIALOG_ALLEVENTS_SIZE,
		DIALOG_TODO,
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
		HOTKEY_CURRENT,
		HOTKEY_ALL,
		HOTKEY_NEXT_YEAR,
		HOTKEY_PREVIOUS_YEAR,
		HOTKEY_TODO,
		HOTKEY_HIDE_CALENDAR,
		HOTKEY_SHOW_CALENDAR,
		HOTKEY_TOGGLE_CALENDAR,
		HOTKEY_HIDE_TODO,
		HOTKEY_SHOW_TODO,
		HOTKEY_TOGGLE_TODO,
		HOTKEY_HIDE_EVENTS,
		HOTKEY_SHOW_EVENTS,
		HOTKEY_TOGGLE_EVENTS,
		HOTKEY_NEW_EVENT,
		HOTKEY_LAST		// This must be last
	};

	enum DAY_LAYOUT
	{
		DAY_LAYOUT_NORMAL,
		DAY_LAYOUT_HORIZONTAL,
		DAY_LAYOUT_VERTICAL
	};

	CConfig();
	~CConfig();

	void Reset();

	bool GetCalendarEnable() { return m_CalendarEnable; };
	void SetCalendarEnable(bool CalendarEnable ) { m_CalendarEnable=CalendarEnable; };
	bool GetTodoEnable() { return m_TodoEnable; };
	void SetTodoEnable(bool TodoEnable ) { m_TodoEnable=TodoEnable; };
	bool GetEventListEnable() { return m_EventListEnable; };
	void SetEventListEnable(bool EventListEnable ) { m_EventListEnable=EventListEnable; };
	bool GetTodoLocked() { return m_TodoLocked; };
	void SetTodoLocked(bool TodoLocked ) { m_TodoLocked=TodoLocked; };
	bool GetEventListLocked() { return m_EventListLocked; };
	void SetEventListLocked(bool EventListLocked ) { m_EventListLocked=EventListLocked; };

	int GetVisibleWindows() { return m_VisibleWindows; };
	void SetVisibleWindows(int VisibleWindows) { m_VisibleWindows=VisibleWindows; };

	const std::string& GetPath() { return m_Path; };
	void SetPath(const std::string& Path) { m_Path=Path; };
	const std::string& GetDLLPath() { return m_DLLPath; };
	void SetDLLPath(const std::string& Path) { m_DLLPath=Path; };
	const std::string& GetSkinsPath() { return m_SkinsPath; };
	void SetSkinsPath(const std::string& SkinsPath) { m_SkinsPath=SkinsPath; };
	const std::string& GetPluginsPath() { return m_PluginsPath; };
	void SetPluginsPath(const std::string& PluginsPath) { m_PluginsPath=PluginsPath; };
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

	int GetX() { return m_X; };
	int GetY() { return m_Y; };
	int GetEventListX() { return m_EventListX; };
	int GetEventListY() { return m_EventListY; };
	int GetTodoX() { return m_TodoX; };
	int GetTodoY() { return m_TodoY; };
	int GetMessageBoxX() { return m_MessageBoxX; };
	int GetMessageBoxY() { return m_MessageBoxY; };
	
	void SetX(int X ) { m_X=X; };
	void SetY(int Y ) { m_Y=Y; };
	void SetEventListX(int EventListX ) { m_EventListX=EventListX; };
	void SetEventListY(int EventListY ) { m_EventListY=EventListY; };
	void SetTodoX(int TodoX ) { m_TodoX=TodoX; };
	void SetTodoY(int TodoY ) { m_TodoY=TodoY; };

	// Layout
	WINDOWPOS GetWindowPos() { return m_WindowPos; };
	bool GetMouseHide() { return m_MouseHide; };
	bool GetMovable() { return m_Movable; };
	UINT GetVerticalCount() { return m_VerticalCount; };
	UINT GetHorizontalCount() { return m_HorizontalCount; };
	UINT GetPreviousMonths() { return m_PreviousMonths; };
	bool GetStartFromJanuary() { return m_StartFromJanuary; };
	UINT GetEventListDays() { return m_EventListDays; };
	bool GetSubstituteDays() { return m_SubstituteDays; };
	UINT GetGrowUpwards() { return m_GrowUpwards; };
	bool GetShowContextMenu() { return m_ShowContextMenu; };
	int GetTodoAction() { return m_TodoAction; };
	int GetCalendarAction() { return m_CalendarAction; };
	int GetEventListAction() { return m_EventListAction; };
	
	void SetWindowPos(WINDOWPOS WindowPos) { m_WindowPos=WindowPos; };
	void SetMovable(bool Movable) { m_Movable=Movable; };
	void SetMouseHide(bool MouseHide) { m_MouseHide=MouseHide; };
	void SetVerticalCount(UINT VerticalCount) { m_VerticalCount=VerticalCount; };
	void SetHorizontalCount(UINT HorizontalCount) { m_HorizontalCount=HorizontalCount; };
	void SetPreviousMonths(UINT PreviousMonths) { m_PreviousMonths=PreviousMonths; };
	void SetStartFromJanuary(bool StartFromJanuary) { m_StartFromJanuary=StartFromJanuary; };
	void SetEventListDays(UINT EventListDays) { m_EventListDays=EventListDays; };
	void SetSubstituteDays(bool SubstituteDays) { m_SubstituteDays=SubstituteDays; };
	void SetGrowUpwards(UINT GrowUpwards) { m_GrowUpwards=GrowUpwards; };
	void SetShowContextMenu(bool ShowContextMenu) { m_ShowContextMenu=ShowContextMenu; };
	void SetTodoAction(int TodoAction) { m_TodoAction=TodoAction; };
	void SetCalendarAction(int CalendarAction) { m_CalendarAction=CalendarAction; };
	void SetEventListAction(int EventListAction) { m_EventListAction=EventListAction; };

	// General
	bool GetStartFromMonday() { return m_StartFromMonday; };
	bool GetStartHidden() { return m_StartHidden; };
	bool GetDisableHotkeys() { return m_DisableHotkeys; };
	bool GetUseWindowName() { return m_UseWindowName; };
	bool GetPollWallpaper() { return m_PollWallpaper; };
	bool GetSnapEdges() { return m_SnapEdges; };
	bool GetNativeTransparency() { return m_NativeTransparency; };
	bool GetOpaqueOnMouseOver() { return m_OpaqueOnMouseOver; };
	bool GetTransparentOnMouseOver() { return m_TransparentOnMouseOver; };
	UINT GetTransparencyValue() { return m_TransparencyValue; };
	bool GetRefreshOnResolutionChange() { return m_RefreshOnResolutionChange; };
	bool GetWeek1HasJanuary1st() { return m_Week1HasJanuary1st; };
	bool GetNegativeCoords() { return m_NegativeCoords; };
	bool GetClickThrough() { return m_ClickThrough; };
	bool GetRememberDialogPositions() { return m_RememberDialogPositions; };
	int GetToolTipMaxWidth() { return m_ToolTipMaxWidth; };
	int GetMessageBoxMaxWidth() { return m_MessageBoxMaxWidth; };
	bool GetShowAllEvents() { return m_ShowAllEvents; };
	bool GetShowSingleEvent() { return m_ShowSingleEvent; };
	int GetSnoozeTime() { return m_SnoozeTime; };
	bool GetShowTrayIcon() { return m_ShowTrayIcon; };
	bool GetDisableDoneList() { return m_DisableDoneList; };
	bool GetUseSkinCharSet() { return m_UseSkinCharSet; };
	int GetWheelScrollMonths() { return m_WheelScrollMonths; };
	int GetWeekNumberDelta() { return m_WeekNumberDelta; };
	int GetFadeDuration() { return m_FadeDuration; };
	bool GetEventToolTips() { return m_EventToolTips; };
    const std::string& GetEventExecute() { return m_EventExecute; };
    bool GetTooltipSeparator() { return m_TooltipSeparator; };
	int GetWeekendDays() { return m_WeekendDays; };

    const std::string& GetTrayExecuteL() { return m_TrayExecuteL; };
    const std::string& GetTrayExecuteR() { return m_TrayExecuteR; };
    const std::string& GetTrayExecuteM() { return m_TrayExecuteM; };
    const std::string& GetTrayExecuteDL() { return m_TrayExecuteDL; };
    const std::string& GetTrayExecuteDR() { return m_TrayExecuteDR; };
    const std::string& GetTrayExecuteDM() { return m_TrayExecuteDM; };

    const std::string& GetConfigEditor() { return m_ConfigEditor; };

	void SetStartFromMonday(bool StartFromMonday ) { m_StartFromMonday=StartFromMonday; };
	void SetStartHidden(bool StartHidden ) { m_StartHidden=StartHidden; };
	void SetDisableHotkeys(bool DisableHotkeys ) { m_DisableHotkeys=DisableHotkeys; };
	void SetUseWindowName(bool UseWindowName ) { m_UseWindowName=UseWindowName; };
	void SetPollWallpaper(bool PollWallpaper ) { m_PollWallpaper=PollWallpaper; };
	void SetSnapEdges(bool SnapEdges) { m_SnapEdges=SnapEdges; };
	void SetNativeTransparency(bool NativeTransparency) { m_NativeTransparency=NativeTransparency; };
	void SetOpaqueOnMouseOver(bool OpaqueOnMouseOver) { m_OpaqueOnMouseOver=OpaqueOnMouseOver; };
	void SetTransparentOnMouseOver(bool TransparentOnMouseOver) { m_TransparentOnMouseOver=TransparentOnMouseOver; };
	void SetTransparencyValue(UINT TransparencyValue) { m_TransparencyValue=TransparencyValue; };
	void SetRefreshOnResolutionChange(bool RefreshOnResolutionChange) { m_RefreshOnResolutionChange=RefreshOnResolutionChange; };
	void SetWeek1HasJanuary1st(bool Week1HasJanuary1st) { m_Week1HasJanuary1st=Week1HasJanuary1st; };
	void SetRememberDialogPositions(bool RememberDialogPositions) { m_RememberDialogPositions=RememberDialogPositions; };
	void SetNegativeCoords(bool NegativeCoords) { m_NegativeCoords=NegativeCoords; };
	void SetClickThrough(bool ClickThrough) { m_ClickThrough=ClickThrough; };
	void SetToolTipMaxWidth(UINT toolTipMaxWidth) { m_ToolTipMaxWidth=toolTipMaxWidth; };
	void SetMessageBoxMaxWidth(int MessageBoxW ) { m_MessageBoxMaxWidth=MessageBoxW; };
	void SetShowAllEvents(bool ShowAllEvents) { m_ShowAllEvents=ShowAllEvents; };
	void SetShowSingleEvent(bool ShowSingleEvent) { m_ShowSingleEvent=ShowSingleEvent; };
	void SetSnoozeTime(int SnoozeTime) { m_SnoozeTime=SnoozeTime; };
	void SetShowTrayIcon(bool ShowTrayIcon) { m_ShowTrayIcon=ShowTrayIcon; };
	void SetDisableDoneList(bool DisableDoneList) { m_DisableDoneList=DisableDoneList; };
	void SetUseSkinCharSet(bool UseSkinCharSet) { m_UseSkinCharSet=UseSkinCharSet; };
	void SetWheelScrollMonths(int WheelScrollMonths) { m_WheelScrollMonths=WheelScrollMonths; };
	void SetWeekNumberDelta(int WeekNumberDelta) { m_WeekNumberDelta=WeekNumberDelta; };
	void SetFadeDuration(int FadeDuration) { m_FadeDuration=FadeDuration; };
	void SetEventToolTips(bool EventToolTips ) { m_EventToolTips=EventToolTips; };
	void SetEventExecute(const std::string& EventExecute ) { m_EventExecute=EventExecute; };
	void SetTooltipSeparator(bool TooltipSeparator) { m_TooltipSeparator=TooltipSeparator; };
	void SetWeekendDays(int WeekendDays) { m_WeekendDays=WeekendDays; };

	void SetCheckUpdates(bool CheckUpdates) { m_CheckUpdates=CheckUpdates; };
	bool GetCheckUpdates() { return m_CheckUpdates; };

	void SetDisableKeyboardOverride(bool DisableKeyboardOverride) { m_DisableKeyboardOverride=DisableKeyboardOverride; };
	bool GetDisableKeyboardOverride() { return m_DisableKeyboardOverride; };

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
	DWORD GetTodoHotkey() { return m_TodoHotkey; };
	void SetTodoHotkey(DWORD TodoHotkey) { m_TodoHotkey=TodoHotkey; };
	DWORD GetHideCalendarHotkey() { return m_HideCalendarHotkey; };
	void SetHideCalendarHotkey(DWORD HideCalendarHotkey) { m_HideCalendarHotkey=HideCalendarHotkey; };
	DWORD GetShowCalendarHotkey() { return m_ShowCalendarHotkey; };
	void SetShowCalendarHotkey(DWORD ShowCalendarHotkey) { m_ShowCalendarHotkey=ShowCalendarHotkey; };
	DWORD GetToggleCalendarHotkey() { return m_ToggleCalendarHotkey; };
	void SetToggleCalendarHotkey(DWORD ToggleCalendarHotkey) { m_ToggleCalendarHotkey=ToggleCalendarHotkey; };
	DWORD GetHideTodoHotkey() { return m_HideTodoHotkey; };
	void SetHideTodoHotkey(DWORD HideTodoHotkey) { m_HideTodoHotkey=HideTodoHotkey; };
	DWORD GetShowTodoHotkey() { return m_ShowTodoHotkey; };
	void SetShowTodoHotkey(DWORD ShowTodoHotkey) { m_ShowTodoHotkey=ShowTodoHotkey; };
	DWORD GetToggleTodoHotkey() { return m_ToggleTodoHotkey; };
	void SetToggleTodoHotkey(DWORD ToggleTodoHotkey) { m_ToggleTodoHotkey=ToggleTodoHotkey; };
	DWORD GetHideEventsHotkey() { return m_HideEventsHotkey; };
	void SetHideEventsHotkey(DWORD HideEventsHotkey) { m_HideEventsHotkey=HideEventsHotkey; };
	DWORD GetShowEventsHotkey() { return m_ShowEventsHotkey; };
	void SetShowEventsHotkey(DWORD ShowEventsHotkey) { m_ShowEventsHotkey=ShowEventsHotkey; };
	DWORD GetToggleEventsHotkey() { return m_ToggleEventsHotkey; };
	void SetToggleEventsHotkey(DWORD ToggleEventsHotkey) { m_ToggleEventsHotkey=ToggleEventsHotkey; };
	DWORD GetAddEventHotkey() { return m_AddEventHotkey; };
	void SetAddEventHotkey(DWORD NewEventHotkey) { m_AddEventHotkey=NewEventHotkey; };

	void ReadConfig();
	void WriteConfig(WRITE_FLAGS flags);

	void ReadProfiles(const char* iniFile);
	const Profile* GetProfile(const char* name);
	const std::list<Profile*>& GetAllProfiles();
	bool IsHiddenProfile(const char* name, bool calendar);

	static void AddPath(std::string& filename);
	static COLORREF ParseColor(const char* color);
	static CRasterizer::TYPE ConvertRasterizer(const char* String);
	static const char* ConvertRasterizer(CRasterizer::TYPE Type);

private:
	static CConfig* c_Config;

	void GetIniTime(const std::string& filename);
	bool CompareIniTime(const std::string& filename);
	void SeparateProfiles(std::string& profiles, std::set<std::string>& result);

	void ReadGeneralConfig(const char* iniFile);

	std::list<Profile*> m_Profiles;

	FILETIME m_WriteTime;	// Last write time for the events.ini

	std::string m_Path;				// Path to the .ini file
	std::string m_DLLPath;			// Path to the Rainlendar.dll file
	std::string m_SkinsPath;		// Path to the skins folder
	std::string m_PluginsPath;		// Path to the plugins folder
	std::string m_CurrentLanguage;	// Name of the current language
	std::string m_CurrentSkin;		// Name of the current skin path
	std::string m_CurrentSkinIni;	// Name of the current skin ini-file
	std::vector<std::string> m_MonthName;

	std::set<std::string> m_HideFromCalendar;
	std::set<std::string> m_HideFromEventList;

	// Dialog positions
	POINT m_DialogPos[DIALOG_LAST];

	// Windows
	bool m_CalendarEnable;
	bool m_TodoEnable;
	bool m_EventListEnable;
	bool m_TodoLocked;
	bool m_EventListLocked;
	int m_VisibleWindows;

	int m_X;					// Position of the main window
	int m_Y;
	int m_EventListX;
	int m_EventListY;
	int m_TodoX;				// Position of the todos
	int m_TodoY;
	int m_MessageBoxX;
	int m_MessageBoxY;

	// Layout settings
	WINDOWPOS m_WindowPos;
	bool m_MouseHide;
	bool m_Movable;
	UINT m_VerticalCount;
	UINT m_HorizontalCount;
	UINT m_PreviousMonths;
	bool m_StartFromJanuary;
	bool m_RememberDialogPositions;
	bool m_NegativeCoords;
	bool m_ClickThrough;
	UINT m_EventListDays;
	bool m_SubstituteDays;
	UINT m_GrowUpwards;
	int m_TodoAction;
	int m_CalendarAction;
	int m_EventListAction;
	bool m_ShowContextMenu;
	int m_WeekendDays;

	// General settings
	bool m_StartFromMonday;
	bool m_StartHidden;
	bool m_DisableHotkeys;
	bool m_UseWindowName;
	bool m_PollWallpaper;
	bool m_SnapEdges;
	bool m_NativeTransparency;
	bool m_OpaqueOnMouseOver;
	bool m_TransparentOnMouseOver;
	UINT m_TransparencyValue;
	bool m_RefreshOnResolutionChange;
	bool m_Week1HasJanuary1st;
	std::string m_EventExecute;
	bool m_EventToolTips;
	std::string m_CurrentProfile;
	UINT m_ToolTipMaxWidth;
	UINT m_MessageBoxMaxWidth;
	bool m_ShowAllEvents;
	bool m_ShowSingleEvent;
	UINT m_SnoozeTime;
	bool m_ShowTrayIcon;
	int m_WheelScrollMonths;
	int m_WeekNumberDelta;
	int m_FadeDuration;
	bool m_TooltipSeparator;

	bool m_DisableDoneList;
	bool m_UseSkinCharSet;

	std::string m_TrayExecuteL;
	std::string m_TrayExecuteR;
	std::string m_TrayExecuteM;
	std::string m_TrayExecuteDL;
	std::string m_TrayExecuteDR;
	std::string m_TrayExecuteDM;

	std::string m_ConfigEditor;

	bool m_CheckUpdates;
	bool m_DisableKeyboardOverride;
	
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
	DWORD m_TodoHotkey;
	DWORD m_HideCalendarHotkey;
	DWORD m_ShowCalendarHotkey;
	DWORD m_ToggleCalendarHotkey;
	DWORD m_HideTodoHotkey;
	DWORD m_ShowTodoHotkey;
	DWORD m_ToggleTodoHotkey;
	DWORD m_HideEventsHotkey;
	DWORD m_ShowEventsHotkey;
	DWORD m_ToggleEventsHotkey;
	DWORD m_AddEventHotkey;
};

#endif

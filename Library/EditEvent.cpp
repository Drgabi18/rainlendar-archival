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
  $Header: /home/cvsroot/Rainlendar/Library/EditEvent.cpp,v 1.3 2005/10/14 17:05:41 rainy Exp $

  $Log: EditEvent.cpp,v $
  Revision 1.3  2005/10/14 17:05:41  rainy
  no message

  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.24  2005/03/01 18:42:44  rainy
  Opening second instance of the dialog is disabled.

  Revision 1.23  2004/12/05 18:33:55  rainy
  Added menu to the richedit.
  Small fixes.

  Revision 1.22  2004/11/06 13:38:58  rainy
  no message

  Revision 1.21  2004/01/25 10:00:18  rainy
  Fixed dialog position remembering.

  Revision 1.20  2004/01/10 15:19:52  rainy
  Added context menu.

  Revision 1.19  2003/12/20 22:24:07  rainy
  Until date is disabled for single events.

  Revision 1.18  2003/10/27 19:51:36  Rainy
  RefreshWindow is no more.

  Revision 1.17  2003/10/27 17:37:13  Rainy
  Config is now singleton.

  Revision 1.16  2003/10/04 14:49:52  Rainy
  Date for the new events is taken from global date instead of previous events.

  Revision 1.15  2003/08/10 08:44:38  Rainy
  Changed the way message text is get from the edit control (the old way didn't work in 98)

  Revision 1.14  2003/08/09 16:36:59  Rainy
  Complete rewrite.

  Revision 1.13  2003/06/15 20:04:44  Rainy
  Server must be enabled or syncing doesn't happen.

  Revision 1.12  2003/06/15 09:46:06  Rainy
  Strings are read from CLanguage class.

  Revision 1.11  2003/05/25 18:10:11  Rainy
  The GetEvent() returns now a sorted vector.

  Revision 1.10  2003/05/07 19:15:13  rainy
  Added option to sync with the server before the events can be edited.

  Revision 1.9  2002/11/25 17:10:47  rainy
  Fixed the bug with duplicating events.

  Revision 1.8  2002/08/10 08:39:20  rainy
  Added one include

  Revision 1.7  2002/08/03 16:18:04  rainy
  Added widgets for the repeating events.

  Revision 1.6  2002/05/30 18:26:40  rainy
  Includes commdlg.h

  Revision 1.5  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.4  2002/01/27 16:11:14  rainy
  Changed CEvent to CEntryEvent to avoid name clash

  Revision 1.3  2002/01/10 16:47:15  rainy
  Added support for bitmap/color,

  Revision 1.2  2001/12/23 10:01:46  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#pragma warning(disable: 4786)
#pragma warning(disable: 4996)

#include "EditEvent.h"
#include "CalendarWindow.h"
#include "RainlendarDLL.h"
#include <Commdlg.h>
#include <time.h>
#include <richedit.h>
#include <objbase.h>

//// Prototypes
INT_PTR CALLBACK EditEventProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK RecurrencyProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

struct Duration 
{
	TCHAR* title;
	int strIndex;
	int time;
};

static HWND g_Dialog = NULL;
static CEntryEvent* g_Event = NULL;
static RainlendarRecurrency* g_Recurrency = NULL;
static std::vector<RECURRENCY_TYPE> g_RecurrencyType;		// For the months

static const Duration g_DurationTable[] =
{
	"0 ", 0, 0,
	"5 ", 0, 5 * 60,
	"10 ", 0, 10 * 60,
	"15 ", 0, 15 * 60,
	"30 ", 0, 30 * 60,
	"1 ", 1, 1 * 60 * 60,
	"2 ", 2, 2 * 60 * 60,
	"3 ", 2, 3 * 60 * 60,
	"4 ", 2, 4 * 60 * 60,
	"5 ", 2, 5 * 60 * 60,
	"6 ", 2, 6 * 60 * 60,
	"7 ", 2, 7 * 60 * 60,
	"8 ", 2, 8 * 60 * 60,
	"9 ", 2, 9 * 60 * 60,
	"10 ", 2, 10 * 60 * 60,
	"11 ", 2, 11 * 60 * 60,
	"12 ", 2, 12 * 60 * 60,
	"18 ", 2, 18 * 60 * 60,
	"1 ", 3, 1 * 24 * 60 * 60,
	"2 ", 4, 2 * 24 * 60 * 60,
	"3 ", 4, 3 * 24 * 60 * 60,
	"4 ", 4, 4 * 24 * 60 * 60,
	"1 ", 5, 7 * 24 * 60 * 60,
	"2 ", 6, 14 * 24 * 60 * 60
};

bool ShowEditEvent(HWND hwndOwner, HINSTANCE instance, RainlendarEvent* event)
{
	bool ret = false;

	if (g_Dialog != NULL)
	{
		FLASHWINFO fi;
		fi.cbSize = sizeof(FLASHWINFO);
		fi.hwnd = g_Dialog;
		fi.dwFlags = FLASHW_CAPTION;
		fi.dwTimeout = 0;
		fi.uCount = 5;
		FlashWindowEx(&fi);
		return false;
	}

	g_Event = new CEntryEvent(event);

	HMODULE richDLL = LoadLibrary("RichEd32.dll");

	DWORD res = DialogBox(instance, MAKEINTRESOURCE(IDD_EDITEVENT), hwndOwner, (DLGPROC)EditEventProc);
	if (res == IDOK || res == IDC_EDITEVENT_DELETE) 
	{
		// Copy the event
		Rainlendar_CopyItem(g_Event->GetRainlendarEvent(), event);
		ret = true;
	}

	delete g_Event;
	g_Event = NULL;
	FreeLibrary(richDLL);
			
	g_Dialog = NULL;

	return ret;
}

void OpenEditEventDialog(HWND hwndOwner, HINSTANCE instance, GUID* guid, CFileTime* startTime)
{
	CPluginManager& pluginManager = GetRainlendar()->GetCalendarWindow().GetPluginManager();
	CEventManager& eventManager = GetRainlendar()->GetCalendarWindow().GetEventManager();

	CPlugin* plugin = NULL;
	RainlendarEvent* event = NULL;

	if (guid)
	{
		CEntryEvent* oldEvent = eventManager.GetEvent(guid);

		if (oldEvent)
		{

// Commented away for now.
// Need to use the default plugin always so that the server plugin works correctly.
//
//			// Get the correct plugin from the event
//			UINT index = oldEvent->GetPluginIndex();	// Index starts from 1
//			const std::vector<CPlugin*> plugins = pluginManager.GetPlugins();
//			if (index > 0 && index <= plugins.size())
//			{
//				plugin = plugins[index - 1];
//			}
//			else			
//			{
				// Use default plugin
				plugin = pluginManager.GetDefaultPlugin();
//			}

			// Copy the event, otherwise its modified by the plugin
			CEntryEvent event(oldEvent->GetRainlendarEvent());

			if (plugin && plugin->HasEditItem() && plugin->IsEnabled())
			{
				if (plugin->EditItem(hwndOwner, event.GetRainlendarItem(), RAINLENDAR_TYPE_EVENT))
				{
					eventManager.FlushBufferedItems(true);
					GetRainlendar()->GetCalendarWindow().RedrawAll();
				}
			}
			else
			{
				// Use the build-in editor
				if (ShowEditEvent(hwndOwner, instance, event.GetRainlendarEvent()))
				{
					Rainlendar_SetItem(event.GetRainlendarItem(), 0);		// 0 = not originated from any plugin
					eventManager.FlushBufferedItems(true);
					GetRainlendar()->GetCalendarWindow().RedrawAll();
				}
			}
		}
		else
		{
			DebugLog("OpenEditEventDialog(): Event not found %s", CEntryItem::GetGUIDAsString(guid));
			return;
		}
	}
	else
	{
		// New event
		if (startTime)
		{
			CEntryEvent event;
			event.SetStartTime(startTime->GetFileTime());
			event.SetAllDayEvent(true);
			event.SetProfile(CConfig::Instance().GetCurrentProfile().c_str());

			plugin = pluginManager.GetDefaultPlugin();

			if (plugin && plugin->HasEditItem() && plugin->IsEnabled())
			{
				plugin->EditItem(hwndOwner, event.GetRainlendarItem(), RAINLENDAR_TYPE_EVENT);
				eventManager.FlushBufferedItems(true);
				GetRainlendar()->GetCalendarWindow().RedrawAll();
			}
			else
			{
				// Use the build-in editor
				if (ShowEditEvent(hwndOwner, instance, event.GetRainlendarEvent()))
				{
					Rainlendar_SetItem(event.GetRainlendarItem(), 0);		// 0 = not originated from any plugin
					eventManager.FlushBufferedItems(true);
					GetRainlendar()->GetCalendarWindow().RedrawAll();
				}
			}
		}
		else
		{
			DebugLog("OpenEditEventDialog(): You need to define either the guid or the startTime.");
			return;
		}
	}
}

static RECT g_RectWindow;
static RECT g_RectClient;
static RECT g_RectMessage;
static RECT g_RectHeader;
static RECT g_RectMessageStatic;
static RECT g_RectOK;
static RECT g_RectCancel;
static RECT g_RectDelete;

static void UpdateWidgets(HWND hwndDlg)
{
	HWND widget;
	widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_ALLDAY);
	
	if (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_EDITEVENT_ALLDAY))
	{
		widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_START);
		EnableWindow(widget, FALSE);
		widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_DURATION);
		EnableWindow(widget, FALSE);
	}
	else
	{
		widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_START);
		EnableWindow(widget, TRUE);
		widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_DURATION);
		EnableWindow(widget, TRUE);
	}

	if (BST_CHECKED != IsDlgButtonChecked(hwndDlg, IDC_EDITEVENT_SHOWALARM))
	{
		widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_SHOWALARM_EDIT);
		EnableWindow(widget, FALSE);
	}
	else
	{
		widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_SHOWALARM_EDIT);
		EnableWindow(widget, TRUE);
	}
}

static BOOL OnInitDialog(HWND window)
{
	char buffer[256];
	HWND widget;

	// Set the localized texts
	SetWindowText(window, CCalendarWindow::c_Language.GetString("EventGUI", 38));

	SetDlgItemText(window, IDOK, CCalendarWindow::c_Language.GetString("General", 0));
	SetDlgItemText(window, IDCANCEL, CCalendarWindow::c_Language.GetString("General", 1));
	SetDlgItemText(window, IDC_EDITEVENT_DELETE,  CCalendarWindow::c_Language.GetString("EventGUI", 1));
	SetDlgItemText(window, IDC_EDITEVENT_ALLDAY,  CCalendarWindow::c_Language.GetString("EventGUI", 41));
	SetDlgItemText(window, IDC_EDITEVENT_DURATION_STATIC,  CCalendarWindow::c_Language.GetString("EventGUI", 40));
	SetDlgItemText(window, IDC_EDITEVENT_HEADER_STATIC,  CCalendarWindow::c_Language.GetString("EventGUI", 43));
	SetDlgItemText(window, IDC_EDITEVENT_START_STATIC,  CCalendarWindow::c_Language.GetString("EventGUI", 39));
	SetDlgItemText(window, IDC_EDITEVENT_MESSAGE_STATIC,  CCalendarWindow::c_Language.GetString("EventGUI", 42));
	SetDlgItemText(window, IDC_EDITEVENT_PROFILE_STATIC,  CCalendarWindow::c_Language.GetString("EventGUI", 9));
	SetDlgItemText(window, IDC_EDITEVENT_RECURRENCY,  CCalendarWindow::c_Language.GetString("EventGUI", 44));
	SetDlgItemText(window, IDC_EDITEVENT_SHOWALARM,  CCalendarWindow::c_Language.GetString("EventGUI", 55));
	SetDlgItemText(window, IDC_EDITEVENT_SHOWALARM_STATIC,  CCalendarWindow::c_Language.GetString("EventGUI", 56));

	// Get the positions of the widgets for resezing
	GetWindowRect(window, &g_RectWindow);
	GetClientRect(window, &g_RectClient);
	widget = GetDlgItem(window, IDC_EDITEVENT_MESSAGE);
	GetClientRect(widget, &g_RectMessage);
	MapWindowPoints(widget, window, (LPPOINT)&g_RectMessage, 2);
	widget = GetDlgItem(window, IDC_EDITEVENT_HEADER);
	GetClientRect(widget, &g_RectHeader);
	MapWindowPoints(widget, window, (LPPOINT)&g_RectHeader, 2);
	widget = GetDlgItem(window, IDC_EDITEVENT_MESSAGE_STATIC);
	GetClientRect(widget, &g_RectMessageStatic);
	MapWindowPoints(widget, window, (LPPOINT)&g_RectMessageStatic, 2);
	widget = GetDlgItem(window, IDOK);
	GetClientRect(widget, &g_RectOK);
	MapWindowPoints(widget, window, (LPPOINT)&g_RectOK, 2);
	widget = GetDlgItem(window, IDCANCEL);
	GetClientRect(widget, &g_RectCancel);
	MapWindowPoints(widget, window, (LPPOINT)&g_RectCancel, 2);
	widget = GetDlgItem(window, IDC_EDITEVENT_DELETE);
	GetClientRect(widget, &g_RectDelete);
	MapWindowPoints(widget, window, (LPPOINT)&g_RectDelete, 2);

	CEventManager& manager = GetRainlendar()->GetCalendarWindow().GetEventManager();
	if (manager.GetEvent(g_Event->GetGUID()) == NULL)
	{
		// New event -> disable delete
		EnableWindow(GetDlgItem(window, IDC_EDITEVENT_DELETE), FALSE);	// Enable delete
	}

	// Fill in the profiles from skin
	SendDlgItemMessage(window, IDC_EDITEVENT_PROFILE, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetTranslatedProfile("Default"));

	const std::list<Profile*>& allProfiles = CConfig::Instance().GetAllProfiles();
	std::list<Profile*>::const_iterator profileIter = allProfiles.begin();
	for( ; profileIter != allProfiles.end(); profileIter++)
	{
		const char* profile = (*profileIter)->name.c_str();
		SendDlgItemMessage(window, IDC_EDITEVENT_PROFILE, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetTranslatedProfile(profile));
	}

	// Fill the start times
	int i;
	CFileTime time(1, 1, 2000);
	for (i = 0; i < 48; i++)
	{
		LPCTSTR timeStr = time.ToTime();
		SendDlgItemMessage(window, IDC_EDITEVENT_START, CB_ADDSTRING, NULL, (LPARAM)timeStr);
		time.Add(30 * 60);
	}

	for (i = 0; i < sizeof(g_DurationTable) / sizeof(Duration); i++)
	{
		std::string text = g_DurationTable[i].title;
		text += CCalendarWindow::c_Language.GetString("EventGUI", 45 + g_DurationTable[i].strIndex);
		SendDlgItemMessage(window, IDC_EDITEVENT_DURATION, CB_ADDSTRING, NULL, (LPARAM)text.c_str());
	}

	widget = GetDlgItem(window, IDC_EDITEVENT_STARTDATE);
	const SYSTEMTIME& sysTime = g_Event->GetStartTime().GetSystemTime();
	DateTime_SetSystemtime(widget, GDT_VALID, &sysTime);

	// Fill in the fields
	widget = GetDlgItem(window, IDC_EDITEVENT_MESSAGE);
	LPCTSTR text = g_Event->GetMessage();
	if (text) SendMessage(widget, WM_SETTEXT, 0, (LPARAM)text);

	widget = GetDlgItem(window, IDC_EDITEVENT_HEADER);
	text = g_Event->GetHeader();
	if (text) SendMessage(widget, WM_SETTEXT, 0, (LPARAM)text);

	widget = GetDlgItem(window, IDC_EDITEVENT_PROFILE);
	text = g_Event->GetProfile();
	if (text) SendMessage(widget, WM_SETTEXT, 0, (LPARAM)CCalendarWindow::c_Language.GetTranslatedProfile(text));

	CFileTime start = g_Event->GetStartTime();
	widget = GetDlgItem(window, IDC_EDITEVENT_START);
	SendMessage(widget, WM_SETTEXT, 0, (LPARAM)start.ToTime());

	CFileTime end = g_Event->GetEndTime();
	if (end.Compare(start) < 0)
	{
		end = start;
	}

	CFileTime duration(end);
	duration.Substract(start);
	int secs = duration.GetAsSeconds();
	bool match = false;
	for (i = 0; i < sizeof(g_DurationTable) / sizeof(Duration); i++)
	{
		if (secs == g_DurationTable[i].time)
		{
			match = true;
			break;
		}
	}

	widget = GetDlgItem(window, IDC_EDITEVENT_DURATION);
	if (match)
	{
		SendMessage(widget, CB_SETCURSEL, i, 0);
	}
	else
	{
		if (secs % (60 * 60 * 24) == 0)
		{
			sprintf(buffer, "%id", secs / (60 * 60 * 24));
		}
		else if (secs % (60 * 60) == 0)
		{
			sprintf(buffer, "%ih", secs / (60 * 60));
		}
		else
		{
			sprintf(buffer, "%im", secs / 60);
		}

		SetWindowText(widget, buffer);
	}

	if (g_Event->IsAllDayEvent())
	{
		CheckDlgButton(window, IDC_EDITEVENT_ALLDAY, BST_CHECKED);
	}

	int alarm = g_Event->GetAlarm();
	if (alarm >= 0)
	{
		CheckDlgButton(window, IDC_EDITEVENT_SHOWALARM, BST_CHECKED);
	}
	else
	{
		alarm = 0;		// Reset to 0 minutes
	}

	sprintf(buffer, "%i", alarm);
	SetWindowText(GetDlgItem(window, IDC_EDITEVENT_SHOWALARM_EDIT), buffer);

	widget = GetDlgItem(window, IDC_EDITEVENT_MESSAGE);
	SetFocus(widget);
	SendMessage(widget, EM_SETSEL, 0, 0);

	SendMessage(widget, EM_SETWORDWRAPMODE, 0, 0);

	UpdateWidgets(window);

	BringWindowToTop(g_Dialog);
	SetForegroundWindow(g_Dialog);

	return TRUE;
}

/*
** UpdateEvent
**
** Reads the data from the widgets and puts it in the event.
**
*/
static void UpdateEvent(HWND window)
{
	HWND widget;
	TCHAR buffer[MAX_LINE_LENGTH];
	buffer[0] = 0;
	DWORD len;

	// Message
	widget = GetDlgItem(window, IDC_EDITEVENT_MESSAGE);
	g_Event->SetMessage(NULL);	// Clear the old message
	len = SendMessage(widget, WM_GETTEXTLENGTH, 0, 0);
	if (len > 0)
	{
		TCHAR* text = new TCHAR[len + 1];
		if (GetWindowText(widget, text, len + 1) > 0)
		{
			g_Event->SetMessage(text);
		}
		delete [] text;
	}


	// Header
	widget = GetDlgItem(window, IDC_EDITEVENT_HEADER);
	g_Event->SetHeader(NULL);	// Clear the old text
	len = SendMessage(widget, WM_GETTEXTLENGTH, 0, 0);
	if (len > 0)
	{
		TCHAR* text = new TCHAR[len + 1];
		if (GetWindowText(widget, text, len + 1) > 0)
		{
			g_Event->SetHeader(text);
		}
		delete [] text;
	}

	// Start Date
	widget = GetDlgItem(window, IDC_EDITEVENT_STARTDATE);
	SYSTEMTIME sysTime;
	DateTime_GetSystemtime(widget, &sysTime);
	CFileTime time(sysTime.wDay, sysTime.wMonth, sysTime.wYear);

	int index = SendDlgItemMessage(window, IDC_EDITEVENT_START, CB_GETCURSEL, NULL, NULL);
	if (index >= 0 && index < 48)
	{
		time = CFileTime(sysTime.wDay, sysTime.wMonth, sysTime.wYear, index / 2, 30 * (index % 2), 0);
	}
	else
	{
		// Start Time is not one from the list
		widget = GetDlgItem(window, IDC_EDITEVENT_START);
		GetWindowText(widget, buffer, MAX_LINE_LENGTH);
		time.FromTime(buffer);	// Set clock (this does not erase the date)
	}

	g_Event->SetStartTime(time);

	// Duration
	buffer[0] = 0;
	CFileTime endTime(time);
	index = SendDlgItemMessage(window, IDC_EDITEVENT_DURATION, CB_GETCURSEL, NULL, NULL);
	if (index >= 0 && index < sizeof(g_DurationTable) / sizeof(Duration))
	{
		endTime.Add(g_DurationTable[index].time);
	}
	else
	{
		// End time is not one from the list
		widget = GetDlgItem(window, IDC_EDITEVENT_DURATION);
		GetWindowText(widget, buffer, MAX_LINE_LENGTH);

		int multiplier = 60;	// 60 = mins
		int value = atoi(buffer);
		value = max(value, 0);

		std::string timeStr = buffer;
		int pos = timeStr.find_first_not_of(" 0123456789");
		if (pos != -1)
		{
			if (timeStr[pos] == 'h')
			{
				multiplier = 60 * 60;
			}
			else if (timeStr[pos] == 'd')
			{
				multiplier = 60 * 60 * 24;
			}
		}

		endTime.Add(value * multiplier);
	}
	g_Event->SetEndTime(endTime);

	// All day event
	if (IsDlgButtonChecked(window, IDC_EDITEVENT_ALLDAY))
	{
		g_Event->SetAllDayEvent(true);
	}
	else
	{
		g_Event->SetAllDayEvent(false);
	}

	// Profile
	widget = GetDlgItem(window, IDC_EDITEVENT_PROFILE);
	g_Event->SetProfile(NULL);	// Clear the old profile (i.e. use "Default")
	len = SendMessage(widget, WM_GETTEXTLENGTH, 0, 0);
	if (len > 0)
	{
		TCHAR* text = new TCHAR[len + 1];

		if (GetWindowText(widget, text, len + 1) > 0)
		{
			g_Event->SetProfile(CCalendarWindow::c_Language.GetOriginalProfile(text));

			// Mitul{ : Let's update the recurrency for Birthday and Anniversary profiles
			// TODO: It should match regardless of case
			if (stricmp(CCalendarWindow::c_Language.GetOriginalProfile(text), "Birthday") == 0 ||
				stricmp(CCalendarWindow::c_Language.GetOriginalProfile(text), "Anniversary") == 0 ||
				stricmp(CCalendarWindow::c_Language.GetOriginalProfile(text), "Birthday\\Anniversary") == 0 ||
				stricmp(CCalendarWindow::c_Language.GetOriginalProfile(text), "Birthday/Anniversary") == 0)
			{
				if (!(g_Event->GetRecurrency()))	// Update recurrency ONLY if it was not setup
				{
					g_Recurrency = new RainlendarRecurrency;
					memset(g_Recurrency, 0, sizeof(RainlendarRecurrency));
					g_Recurrency->size = sizeof(RainlendarRecurrency);
					//*g_Recurrency = *(g_Event->GetRecurrency());	// Copy recurrency data
					g_Recurrency->type = RECURRENCY_TYPE_YEARLY;
					g_Recurrency->frequency = 1;
					g_Recurrency->repeatType = RECURRENCY_REPEAT_FOREVER;
					g_Event->SetRecurrency(g_Recurrency);
					delete g_Recurrency;
				}
			}
			// Mitul}

			// Save the selected profile
			CConfig::Instance().SetCurrentProfile(CCalendarWindow::c_Language.GetOriginalProfile(text));
			CConfig::Instance().WriteConfig(CConfig::WRITE_PROFILE);
		}
		delete [] text;
	}

	// Alarm
	if (IsDlgButtonChecked(window, IDC_EDITEVENT_SHOWALARM))
	{
		char buffer[256];
		widget = GetDlgItem(window, IDC_EDITEVENT_SHOWALARM_EDIT);
		GetWindowText(widget, buffer, 256);
		g_Event->SetAlarm(atoi(buffer));
	}
	else
	{
		g_Event->SetAlarm(-1);
	}

	// Update the timestamp so that modifications are saved
	g_Event->UpdateTimeStamp();
}

static BOOL ArrangeWidgets(HWND hwndDlg)
{
	HWND widget;
	RECT rect;
	GetClientRect(hwndDlg, &rect);

	widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_MESSAGE);
	SetWindowPos(widget, NULL, 0, 0, rect.right - (g_RectClient.right - (g_RectMessage.right - g_RectMessage.left)),
		rect.bottom - (g_RectClient.bottom - (g_RectMessage.bottom - g_RectMessage.top)), SWP_NOMOVE | SWP_NOZORDER);

	widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_MESSAGE_STATIC);
	SetWindowPos(widget, NULL, 0, 0, rect.right - (g_RectClient.right - (g_RectMessageStatic.right - g_RectMessageStatic.left)),
		rect.bottom - (g_RectClient.bottom - (g_RectMessageStatic.bottom - g_RectMessageStatic.top)), SWP_NOMOVE | SWP_NOZORDER);

	widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_HEADER);
	SetWindowPos(widget, NULL, 0, 0, rect.right - (g_RectClient.right - (g_RectHeader.right - g_RectHeader.left)),
		g_RectHeader.bottom - g_RectHeader.top, SWP_NOMOVE | SWP_NOZORDER);

	widget = GetDlgItem(hwndDlg, IDOK);
	SetWindowPos(widget, NULL, rect.right - (g_RectClient.right - g_RectOK.left),
		rect.bottom - (g_RectClient.bottom - g_RectOK.top), 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	widget = GetDlgItem(hwndDlg, IDCANCEL);
	SetWindowPos(widget, NULL, rect.right - (g_RectClient.right - g_RectCancel.left),
		rect.bottom - (g_RectClient.bottom - g_RectCancel.top), 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_DELETE);
	SetWindowPos(widget, NULL, g_RectDelete.left,
		rect.bottom - (g_RectClient.bottom - g_RectDelete.top), 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	return 0;
}

INT_PTR CALLBACK EditEventProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) 
    { 
		case WM_INITDIALOG:
			g_Dialog = hwndDlg;
			return OnInitDialog(hwndDlg);

		case WM_WINDOWPOSCHANGING:
			{
				// Don't allow shrinking
				LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
				if (pos->cx < g_RectWindow.right - g_RectWindow.left)
				{
					pos->cx = g_RectWindow.right - g_RectWindow.left;
				}
				if (pos->cy < g_RectWindow.bottom - g_RectWindow.top)
				{
					pos->cy = g_RectWindow.bottom - g_RectWindow.top;
				}
			}
			break;

		case WM_SIZE:
			return ArrangeWidgets(hwndDlg);

		case WM_CONTEXTMENU:
			{
				HWND widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_MESSAGE);
				if ((HWND)wParam == widget) 
				{
					HMENU menu, subMenu = NULL;

					menu = LoadMenu(CRainlendar::GetInstance(), MAKEINTRESOURCE(IDR_MENU1));
					if (menu) subMenu = GetSubMenu(menu, 2);

					// Localize
					ModifyMenu(subMenu, ID_EDITMENU_UNDO, MF_BYCOMMAND | MF_STRING, ID_EDITMENU_UNDO, CCalendarWindow::c_Language.GetString("Menu", 31));
					ModifyMenu(subMenu, ID_EDITMENU_CUT, MF_BYCOMMAND | MF_STRING, ID_EDITMENU_CUT, CCalendarWindow::c_Language.GetString("Menu", 32));
					ModifyMenu(subMenu, ID_EDITMENU_COPY, MF_BYCOMMAND | MF_STRING, ID_EDITMENU_COPY, CCalendarWindow::c_Language.GetString("Menu", 33));
					ModifyMenu(subMenu, ID_EDITMENU_PASTE, MF_BYCOMMAND | MF_STRING, ID_EDITMENU_PASTE, CCalendarWindow::c_Language.GetString("Menu", 34));
					ModifyMenu(subMenu, ID_EDITMENU_DELETE, MF_BYCOMMAND | MF_STRING, ID_EDITMENU_DELETE, CCalendarWindow::c_Language.GetString("Menu", 35));
					ModifyMenu(subMenu, ID_EDITMENU_SELECT_ALL, MF_BYCOMMAND | MF_STRING, ID_EDITMENU_SELECT_ALL, CCalendarWindow::c_Language.GetString("Menu", 36));

					// Disable invalid menu items
					int start = 0, end = 0;
					SendMessage(widget, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);

					// Undo
					if (!SendMessage(widget, EM_CANUNDO, 0 ,0))
					{
						EnableMenuItem(subMenu, ID_EDITMENU_UNDO, MF_GRAYED);
					}
					// Cut
					if (start == end)
					{
						EnableMenuItem(subMenu, ID_EDITMENU_CUT, MF_GRAYED);
					}
					// Copy
					if (start == end)
					{
						EnableMenuItem(subMenu, ID_EDITMENU_COPY, MF_GRAYED);
					}
					// Paste
					if (!SendMessage(widget, EM_CANPASTE, 0 ,0))
					{
						EnableMenuItem(subMenu, ID_EDITMENU_PASTE, MF_GRAYED);
					}
					// Delete
					if (start == end)
					{
						EnableMenuItem(subMenu, ID_EDITMENU_DELETE, MF_GRAYED);
					}
					// Select All
					GETTEXTLENGTHEX lenEx;
					lenEx.codepage = GetACP();
					lenEx.flags = GTL_DEFAULT;
					if (SendMessage(widget, EM_GETTEXTLENGTHEX, (WPARAM)&lenEx ,0) == 0)
					{
						EnableMenuItem(subMenu, ID_EDITMENU_SELECT_ALL, MF_GRAYED);
					}
					
					TrackPopupMenu(
						subMenu,
						TPM_RIGHTBUTTON | TPM_LEFTALIGN, 
						(SHORT)LOWORD(lParam),
						(SHORT)HIWORD(lParam),
						0,
						hwndDlg,
						NULL
						);		
					DestroyMenu(subMenu);
				}
			}
			break;
        
        case WM_CLOSE:
			EndDialog(hwndDlg, IDCANCEL); 
			break;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            {
				case ID_EDITMENU_UNDO:
					{
						HWND widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_MESSAGE);
						if (widget) SendMessage(widget, EM_UNDO, 0, 0);
					}
					break;

				case ID_EDITMENU_CUT:
					{
						HWND widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_MESSAGE);
						if (widget) SendMessage(widget, WM_CUT, 0, 0);
					}
					break;

				case ID_EDITMENU_COPY:
					{
						HWND widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_MESSAGE);
						if (widget) SendMessage(widget, WM_COPY, 0, 0);
					}
					break;

				case ID_EDITMENU_PASTE:
					{
						HWND widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_MESSAGE);
						if (widget) SendMessage(widget, WM_PASTE, 0, 0);
					}
					break;

				case ID_EDITMENU_DELETE:
					{
						HWND widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_MESSAGE);
						if (widget) SendMessage(widget, WM_CLEAR, 0, 0);
					}
					break;

				case ID_EDITMENU_SELECT_ALL:
					{
						HWND widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_MESSAGE);
						if (widget) SendMessage(widget, EM_SETSEL, 0, -1);
					}
					break;

				case IDC_EDITEVENT_DELETE:
					// "Are you sure you want to delete this event?"
					if (MessageBox(hwndDlg, CCalendarWindow::c_Language.GetString("EventGUI", 57), APPNAME, MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						g_Event->MarkAsDeleted();
						g_Event->UpdateTimeStamp();
						EndDialog(hwndDlg, LOWORD(wParam)); 
					}
					// else do nothing
					break;

				case IDC_EDITEVENT_SHOWALARM:
				case IDC_EDITEVENT_ALLDAY:
					UpdateWidgets(hwndDlg);
					break;

				case IDC_EDITEVENT_RECURRENCY:
					UpdateEvent(hwndDlg);
					g_Recurrency = new RainlendarRecurrency;
					memset(g_Recurrency, 0, sizeof(RainlendarRecurrency));
					g_Recurrency->size = sizeof(RainlendarRecurrency);
					if (g_Event->GetRecurrency())
					{
						*g_Recurrency = *(g_Event->GetRecurrency());	// Copy recurrency data
					}
					if (IDOK == DialogBox((HINSTANCE)GetWindowLong(hwndDlg, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_RECURRENCY), hwndDlg, (DLGPROC)RecurrencyProc))
					{
						g_Event->SetRecurrency(g_Recurrency);
					}
					delete g_Recurrency;
					break;

				case IDOK:
					UpdateEvent(hwndDlg);
					// Fall through!
				case IDCANCEL:
					EndDialog(hwndDlg, LOWORD(wParam)); 
					break;
			}
		};

    return FALSE; 
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                        R E C U R R E N C Y   D I A L O G
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

static void UpdateRecurrencyWidgets(HWND hwndDlg)
{
	bool hideDays = true, hideMonth = true;
	HWND widget;
	widget = GetDlgItem(hwndDlg, IDC_RECURRENCY_TYPE_STATIC);

	int index = SendDlgItemMessage(hwndDlg, IDC_RECURRENCY_COMBO, CB_GETCURSEL, NULL, NULL);
	switch(index)
	{
		case 0: // Single
			hideDays = true;
			hideMonth = true;
			break;

		case 1: // Daily
			hideDays = true;
			hideMonth = true;
			SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 12));
			break;

		case 2: // Weekly
			hideDays = false;
			hideMonth = true;
			SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 13));
			break;

		case 3: // Monthly
			hideDays = true;
			hideMonth = false;
			SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 14));
			break;

		case 4: // Yearly
			hideDays = true;
			hideMonth = true;
			SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 15));
			break;
	}

	ShowWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_MON), hideDays ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_TUE), hideDays ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_WED), hideDays ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_THU), hideDays ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_FRI), hideDays ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_SAT), hideDays ? SW_HIDE : SW_SHOW);
	ShowWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_SUN), hideDays ? SW_HIDE : SW_SHOW);

	ShowWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_MONTH_COMBO), hideMonth ? SW_HIDE : SW_SHOW);

	EnableWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_EDIT), index != 0);
	EnableWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_RADIO1), index != 0);
	EnableWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_RADIO2), index != 0);
	EnableWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_RADIO3), index != 0);
	EnableWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_NUMOFTIMES), index != 0);
	EnableWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_UNTIL), index != 0);

	if (index != 0)
	{
		if (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_RECURRENCY_RADIO2))
		{
			EnableWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_NUMOFTIMES), TRUE);
		}
		else
		{
			EnableWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_NUMOFTIMES), FALSE);
		}
		if (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_RECURRENCY_RADIO3))
		{
			EnableWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_UNTIL), TRUE);
		}
		else
		{
			EnableWindow(GetDlgItem(hwndDlg, IDC_RECURRENCY_UNTIL), FALSE);
		}
	}
}

static BOOL OnInitRecurrencyDialog(HWND window)
{
	HWND widget;

	// Set the localized texts
	SetWindowText(window, CCalendarWindow::c_Language.GetString("EventGUI", 44));

	SetDlgItemText(window, IDOK, CCalendarWindow::c_Language.GetString("General", 0));
	SetDlgItemText(window, IDCANCEL, CCalendarWindow::c_Language.GetString("General", 1));

	SetDlgItemText(window, IDC_RECURRENCY_STATIC,  CCalendarWindow::c_Language.GetString("EventGUI", 52));
	SetDlgItemText(window, IDC_RECURRENCY_REPEAT_STATIC,  CCalendarWindow::c_Language.GetString("EventGUI", 8));
	SetDlgItemText(window, IDC_RECURRENCY_REPEAT_GROUP,  CCalendarWindow::c_Language.GetString("EventGUI", 10));
	SetDlgItemText(window, IDC_RECURRENCY_RADIO1,  CCalendarWindow::c_Language.GetString("EventGUI", 53));
	SetDlgItemText(window, IDC_RECURRENCY_RADIO2,  CCalendarWindow::c_Language.GetString("EventGUI", 54));
	SetDlgItemText(window, IDC_RECURRENCY_RADIO3,  CCalendarWindow::c_Language.GetString("EventGUI", 7));

	SetDlgItemText(window, IDC_RECURRENCY_MON, CCalendarWindow::c_Language.GetString("EventGUI", 16));
	SetDlgItemText(window, IDC_RECURRENCY_TUE, CCalendarWindow::c_Language.GetString("EventGUI", 17));
	SetDlgItemText(window, IDC_RECURRENCY_WED, CCalendarWindow::c_Language.GetString("EventGUI", 18));
	SetDlgItemText(window, IDC_RECURRENCY_THU, CCalendarWindow::c_Language.GetString("EventGUI", 19));
	SetDlgItemText(window, IDC_RECURRENCY_FRI, CCalendarWindow::c_Language.GetString("EventGUI", 20));
	SetDlgItemText(window, IDC_RECURRENCY_SAT, CCalendarWindow::c_Language.GetString("EventGUI", 21));
	SetDlgItemText(window, IDC_RECURRENCY_SUN, CCalendarWindow::c_Language.GetString("EventGUI", 22));

	int i;
	for (i = 0; i < 5; i++)
	{
		SendDlgItemMessage(window, IDC_RECURRENCY_COMBO, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("EventGUI", 2 + i));
	}

	// Set the type from the event
	widget = GetDlgItem(window, IDC_RECURRENCY_COMBO);
	switch (g_Recurrency->type)
	{
	case RECURRENCY_TYPE_SINGLE:
		SendMessage(widget, CB_SETCURSEL, 0, 0);
		break;
	case RECURRENCY_TYPE_DAILY:
		SendMessage(widget, CB_SETCURSEL, 1, 0);
		break;
	case RECURRENCY_TYPE_WEEKLY:
		SendMessage(widget, CB_SETCURSEL, 2, 0);
		break;
	case RECURRENCY_TYPE_MONTHLY_NTH_DAY:
	case RECURRENCY_TYPE_MONTHLY_NTH_WEEKDAY:
	case RECURRENCY_TYPE_MONTHLY_LASTWEEK_WEEKDAY:
	case RECURRENCY_TYPE_MONTHLY_LAST_DAY:
	case RECURRENCY_TYPE_MONTHLY_FIRST_WEEKDAY:
	case RECURRENCY_TYPE_MONTHLY_LAST_WEEKDAY:
		SendMessage(widget, CB_SETCURSEL, 3, 0);
		break;
	case RECURRENCY_TYPE_YEARLY:
		SendMessage(widget, CB_SETCURSEL, 4, 0);
		break;
	}

	// Get repeat from the event
	char buffer[256];
	sprintf(buffer, "%i", g_Recurrency->frequency == 0 ? 1 : g_Recurrency->frequency);
	SetWindowText(GetDlgItem(window, IDC_RECURRENCY_EDIT), buffer);

	// Get numoftimes from the event
	sprintf(buffer, "%i", g_Recurrency->count == 0 ? 1 : g_Recurrency->count);
	SetWindowText(GetDlgItem(window, IDC_RECURRENCY_NUMOFTIMES), buffer);

	// Get repeat type from the event
	CheckRadioButton(window, IDC_RECURRENCY_RADIO1, IDC_RECURRENCY_RADIO3, IDC_RECURRENCY_RADIO1 + (int)g_Recurrency->repeatType);

	// Get until from the event
	widget = GetDlgItem(window, IDC_RECURRENCY_UNTIL);
	CFileTime until(g_Recurrency->until);
	if (until.Compare(g_Event->GetStartTime()) < 0)
	{
		const SYSTEMTIME& sysTime = g_Event->GetStartTime().GetSystemTime();
		DateTime_SetSystemtime(widget, GDT_VALID, &sysTime);
	}
	else
	{
		const SYSTEMTIME& sysTime = until.GetSystemTime();
		DateTime_SetSystemtime(widget, GDT_VALID, &sysTime);
	}

	// Check which day this is and check the correct day-box
	CFileTime start = g_Event->GetStartTime();
	const SYSTEMTIME& sysTime = start.GetSystemTime();
	CheckDlgButton(window, IDC_RECURRENCY_SUN + sysTime.wDayOfWeek, BST_CHECKED);
	EnableWindow(GetDlgItem(window, IDC_RECURRENCY_SUN + sysTime.wDayOfWeek), FALSE);

	// Check the rest
	if (g_Recurrency->days & 1) CheckDlgButton(window, IDC_RECURRENCY_SUN, BST_CHECKED);
	if (g_Recurrency->days & 2) CheckDlgButton(window, IDC_RECURRENCY_MON, BST_CHECKED);
	if (g_Recurrency->days & 4) CheckDlgButton(window, IDC_RECURRENCY_TUE, BST_CHECKED);
	if (g_Recurrency->days & 8) CheckDlgButton(window, IDC_RECURRENCY_WED, BST_CHECKED);
	if (g_Recurrency->days & 16) CheckDlgButton(window, IDC_RECURRENCY_THU, BST_CHECKED);
	if (g_Recurrency->days & 32) CheckDlgButton(window, IDC_RECURRENCY_FRI, BST_CHECKED);
	if (g_Recurrency->days & 64) CheckDlgButton(window, IDC_RECURRENCY_SAT, BST_CHECKED);

	// Fill the month combo
	g_RecurrencyType.clear();
	widget = GetDlgItem(window, IDC_RECURRENCY_MONTH_COMBO);

	// On the [Nth] day of the month
	std::string entry = CCalendarWindow::c_Language.GetString("EventGUI", 23);
	int pos = entry.find("%1");
	if (pos != -1)
	{
		if (CCalendarWindow::c_Language.AreOrdinalsDefined()) 
		{
			sprintf(buffer, "%i%s", sysTime.wDay, CCalendarWindow::c_Language.GetOrdinalString(sysTime.wDay));
			entry.replace(pos, 2, buffer);
		}
		else
		{
			// Fallback
			entry.replace(pos, 2, CCalendarWindow::c_Language.GetString("Numbers", sysTime.wDay - 1));
		}
	}
	SendDlgItemMessage(window, IDC_RECURRENCY_MONTH_COMBO, CB_ADDSTRING, NULL, (LPARAM)entry.c_str());

	g_RecurrencyType.push_back(RECURRENCY_TYPE_MONTHLY_NTH_DAY);

	// On the [Nth] [weekday] of the month
	entry = CCalendarWindow::c_Language.GetString("EventGUI", 24);
	pos = entry.find("%1");
	if (pos != -1)
	{
		entry.replace(pos, 2, CCalendarWindow::c_Language.GetString("EventGUI", (sysTime.wDay - 1) / 7 + 32));
	}
	pos = entry.find("%2");
	if (pos != -1)
	{
		entry.replace(pos, 2, CCalendarWindow::c_Language.GetString("EventGUI", sysTime.wDayOfWeek + 25));
	}
	SendDlgItemMessage(window, IDC_RECURRENCY_MONTH_COMBO, CB_ADDSTRING, NULL, (LPARAM)entry.c_str());

	g_RecurrencyType.push_back(RECURRENCY_TYPE_MONTHLY_NTH_WEEKDAY);

	// On the last [weekday] of the month
	if (sysTime.wDay + 7 > start.GetDaysInMonth())
	{
		entry = CCalendarWindow::c_Language.GetString("EventGUI", 24);
		pos = entry.find("%1");
		if (pos != -1)
		{
			entry.replace(pos, 2, CCalendarWindow::c_Language.GetString("EventGUI", 58));		// last
		}
		pos = entry.find("%2");
		if (pos != -1)
		{
			entry.replace(pos, 2, CCalendarWindow::c_Language.GetString("EventGUI", sysTime.wDayOfWeek + 25));
		}
		SendDlgItemMessage(window, IDC_RECURRENCY_MONTH_COMBO, CB_ADDSTRING, NULL, (LPARAM)entry.c_str());

		g_RecurrencyType.push_back(RECURRENCY_TYPE_MONTHLY_LASTWEEK_WEEKDAY);
	}

	// On the last day of the month
	if (sysTime.wDay == start.GetDaysInMonth())
	{
		entry = CCalendarWindow::c_Language.GetString("EventGUI", 23);
		int pos = entry.find("%1");
		if (pos != -1)
		{
			entry.replace(pos, 2, CCalendarWindow::c_Language.GetString("EventGUI", 58));		// last
		}
		SendDlgItemMessage(window, IDC_RECURRENCY_MONTH_COMBO, CB_ADDSTRING, NULL, (LPARAM)entry.c_str());

		g_RecurrencyType.push_back(RECURRENCY_TYPE_MONTHLY_LAST_DAY);
	}

	
	// First weekday of the month
	if ((sysTime.wDay == 1 && sysTime.wDayOfWeek != 0 && sysTime.wDayOfWeek != 6) ||
	    (sysTime.wDay == 2 && sysTime.wDayOfWeek == 1) ||
	    (sysTime.wDay == 3 && sysTime.wDayOfWeek == 1))
	{
		entry = CCalendarWindow::c_Language.GetString("EventGUI", 24);
		pos = entry.find("%1");
		if (pos != -1)
		{
			entry.replace(pos, 2, CCalendarWindow::c_Language.GetString("EventGUI", 32));		// first
		}
		pos = entry.find("%2");
		if (pos != -1)
		{
			entry.replace(pos, 2, CCalendarWindow::c_Language.GetString("EventGUI", 37));		// weekday
		}
		SendDlgItemMessage(window, IDC_RECURRENCY_MONTH_COMBO, CB_ADDSTRING, NULL, (LPARAM)entry.c_str());	

		g_RecurrencyType.push_back(RECURRENCY_TYPE_MONTHLY_FIRST_WEEKDAY);
	}


	// Last weekday of the month
	if ((sysTime.wDay == start.GetDaysInMonth() && sysTime.wDayOfWeek != 0 && sysTime.wDayOfWeek != 6) ||
	    (sysTime.wDay == start.GetDaysInMonth() - 1 && sysTime.wDayOfWeek == 5) ||
	    (sysTime.wDay == start.GetDaysInMonth() - 2 && sysTime.wDayOfWeek == 5))
	{
		entry = CCalendarWindow::c_Language.GetString("EventGUI", 24);
		pos = entry.find("%1");
		if (pos != -1)
		{
			entry.replace(pos, 2, CCalendarWindow::c_Language.GetString("EventGUI", 58));		// last
		}
		pos = entry.find("%2");
		if (pos != -1)
		{
			entry.replace(pos, 2, CCalendarWindow::c_Language.GetString("EventGUI", 37));		// weekday
		}
		SendDlgItemMessage(window, IDC_RECURRENCY_MONTH_COMBO, CB_ADDSTRING, NULL, (LPARAM)entry.c_str());	

		g_RecurrencyType.push_back(RECURRENCY_TYPE_MONTHLY_LAST_WEEKDAY);
	}

	SendMessage(widget, CB_SETCURSEL, 0, 0);

	// Enable the correct item in month combo
	if (g_Recurrency->type == RECURRENCY_TYPE_MONTHLY_NTH_WEEKDAY)
	{
		SendMessage(widget, CB_SETCURSEL, 1, 0);
	}
	else if (g_Recurrency->type == RECURRENCY_TYPE_MONTHLY_LASTWEEK_WEEKDAY)
	{
		SendMessage(widget, CB_SETCURSEL, 2, 0);
	}
	else if (g_Recurrency->type == RECURRENCY_TYPE_MONTHLY_LAST_DAY)
	{
		SendMessage(widget, CB_SETCURSEL, 3, 0);
	}
	else if (g_Recurrency->type == RECURRENCY_TYPE_MONTHLY_FIRST_WEEKDAY || g_Recurrency->type == RECURRENCY_TYPE_MONTHLY_LAST_WEEKDAY)
	{
		// Enable the last item
		SendMessage(widget, CB_SETCURSEL, g_RecurrencyType.size() - 1, 0);
	}
	else
	{
		SendMessage(widget, CB_SETCURSEL, 0, 0);
	}

	UpdateRecurrencyWidgets(window);

	return TRUE;
}

static void UpdateRecurrency(HWND window)
{
	HWND widget;
	char buffer[256];

	int index = SendDlgItemMessage(window, IDC_RECURRENCY_COMBO, CB_GETCURSEL, NULL, NULL);
	switch (index)
	{
	case 0:		// Single
		g_Recurrency->type = RECURRENCY_TYPE_SINGLE;
		break;

	case 1:		// Daily
		g_Recurrency->type = RECURRENCY_TYPE_DAILY;
		break;

	case 2:		// Weekly
		g_Recurrency->type = RECURRENCY_TYPE_WEEKLY;
		break;

	case 3:		// Monthly
		{
			int index2 = SendDlgItemMessage(window, IDC_RECURRENCY_MONTH_COMBO, CB_GETCURSEL, NULL, NULL);
			g_Recurrency->type = g_RecurrencyType[index2];
		}
		break;

	case 4:		// Yearly
		g_Recurrency->type = RECURRENCY_TYPE_YEARLY;
		break;
	}

	// frequency
	widget = GetDlgItem(window, IDC_RECURRENCY_EDIT);
	GetWindowText(widget, buffer, 256);
	int num = atoi(buffer);
	if (num == 0) num = 1;
	g_Recurrency->frequency = num;

	// Weekly days
	g_Recurrency->days = 0;
	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_RECURRENCY_SUN))
	{
		g_Recurrency->days |= 1;
	}
	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_RECURRENCY_MON))
	{
		g_Recurrency->days |= 2;
	}
	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_RECURRENCY_TUE))
	{
		g_Recurrency->days |= 4;
	}
	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_RECURRENCY_WED))
	{
		g_Recurrency->days |= 8;
	}
	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_RECURRENCY_THU))
	{
		g_Recurrency->days |= 16;
	}
	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_RECURRENCY_FRI))
	{
		g_Recurrency->days |= 32;
	}
	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_RECURRENCY_SAT))
	{
		g_Recurrency->days |= 64;
	}

	// Repeat
	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_RECURRENCY_RADIO1))
	{
		g_Recurrency->repeatType = RECURRENCY_REPEAT_FOREVER;
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_RECURRENCY_RADIO2))
	{
		g_Recurrency->repeatType = RECURRENCY_REPEAT_COUNT;
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_RECURRENCY_RADIO3))
	{
		g_Recurrency->repeatType = RECURRENCY_REPEAT_UNTIL;
	}

	// Count (num of times)
	widget = GetDlgItem(window, IDC_RECURRENCY_NUMOFTIMES);
	GetWindowText(widget, buffer, 256);
	num = atoi(buffer);
	if (num == 0) num = 1;
	g_Recurrency->count = (UINT)num;

	// Until
	widget = GetDlgItem(window, IDC_RECURRENCY_UNTIL);
	SYSTEMTIME sysTime;
	DateTime_GetSystemtime(widget, &sysTime);
	sysTime.wMilliseconds = 999;
	sysTime.wSecond = 59;
	sysTime.wMinute= 59;
	sysTime.wHour = 23;
	CFileTime time(sysTime);
	g_Recurrency->until = time.GetFileTime();
}

INT_PTR CALLBACK RecurrencyProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) 
    { 
		case WM_INITDIALOG:
			return OnInitRecurrencyDialog(hwndDlg);

        case WM_CLOSE:
			EndDialog(hwndDlg, IDCANCEL); 
			break;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            {
				case IDC_RECURRENCY_COMBO:
				case IDC_RECURRENCY_RADIO1:
				case IDC_RECURRENCY_RADIO2:
				case IDC_RECURRENCY_RADIO3:
					UpdateRecurrencyWidgets(hwndDlg);
					break;

				case IDOK:
					UpdateRecurrency(hwndDlg);
					// Fall through!
				case IDCANCEL:
					EndDialog(hwndDlg, LOWORD(wParam)); 
					break;
			}
		};

    return FALSE; 
} 

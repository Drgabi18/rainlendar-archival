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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/EditEvent.cpp,v 1.8 2002/08/10 08:39:20 rainy Exp $

  $Log: EditEvent.cpp,v $
  Revision 1.8  2002/08/10 08:39:20  rainy
  Added one include

  Revision 1.7  2002/08/03 16:18:04  rainy
  Added widgets for the repeating events.

  Revision 1.6  2002/05/30 18:26:40  rainy
  Includes commdlg.h

  Revision 1.5  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.4  2002/01/27 16:11:14  rainy
  Changed CEvent to CEventMessage to avoid name clash

  Revision 1.3  2002/01/10 16:47:15  rainy
  Added support for bitmap/color,

  Revision 1.2  2001/12/23 10:01:46  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#pragma warning(disable: 4786)

#include "EditEvent.h"
#include "CalendarWindow.h"
#include "RainlendarDLL.h"
#include "NetworkThread.h"
#include <Commdlg.h>
#include <time.h>

// These need to be global (even though globals are evil)
static COLORREF g_FontColor = 0;
static std::vector<CEventMessage> g_Events;
int g_ActiveEvent = 0;

// Prototypes
void UpdateWidgets(HWND window, bool recreateTabs);

BOOL OnInitDialog(HWND window) 
{
	char caption[256];
	HWND widget = NULL;

	int day, month, year;
	day = GetRainlendar()->GetCalendarWindow().GetSelectedDay();
	month = CCalendarWindow::c_MonthsFirstDate.wMonth;
	year = CCalendarWindow::c_MonthsFirstDate.wYear;
	const std::string monthStr = CCalendarWindow::c_Config.GetMonthName(month - 1);

	sprintf(caption, "%i-%s-%i", day, monthStr.c_str(), year);
	SetWindowText(window, caption);

	// Get the events from event manager
	std::list<CEventMessage*> eventList = GetRainlendar()->GetCalendarWindow().GetEventManager()->GetEvents(day, CCalendarWindow::c_MonthsFirstDate.wMonth, year);

	// Copy the events to a temporary place so that they can be edited
	g_Events.clear();
	std::list<CEventMessage*>::iterator i = eventList.begin();
	for( ; i != eventList.end(); i++)
	{
		if (!((*i)->IsDeleted()))	// No deleted messages
		{
			g_Events.push_back(*(*i));
		}
	}

	if (g_Events.empty())
	{
		CEventMessage event;
		event.SetDate(CEventMessage::DateToValue(day, month, year));
		event.SetProfile(CCalendarWindow::c_Config.GetCurrentProfile());
		g_Events.push_back(event);
	}

	// The last one is active
	g_ActiveEvent = g_Events.size() - 1;

	const char* items[] = { "1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th", "9th", "10th", "11th", "12th" };
	for( int j = 0; j < 12 ; j++)
	{
		SendDlgItemMessage(window, IDC_EDITEVENT_EVERY, CB_ADDSTRING, NULL, (LPARAM)items[j]);
	}

	// Fill in the profiles from skin
	SendDlgItemMessage(window, IDC_EDITEVENT_PROFILE, CB_ADDSTRING, NULL, (LPARAM)"Default");

	const std::list<Profile>& allProfiles = CCalendarWindow::c_Config.GetAllProfiles();
	std::list<Profile>::const_iterator profileIter = allProfiles.begin();
	for( ; profileIter != allProfiles.end(); profileIter++)
	{
		SendDlgItemMessage(window, IDC_EDITEVENT_PROFILE, CB_ADDSTRING, NULL, (LPARAM)(*profileIter).name.c_str());
	}

	UpdateWidgets(window, true);

	return TRUE;
}

/*
** UpdateWidgets
**
** Reads the data from the active event and puts it in the widgets.
**
*/
void UpdateWidgets(HWND window, bool recreateTabs)
{
	HWND widget;
	int day, month, year;
	day = GetRainlendar()->GetCalendarWindow().GetSelectedDay();
	month = CCalendarWindow::c_MonthsFirstDate.wMonth;
	year = CCalendarWindow::c_MonthsFirstDate.wYear;

	// Tabbed
	int i = 0;
	widget = GetDlgItem(window, IDC_EDITEVENT_TAB);
	if (recreateTabs)
	{
		TabCtrl_DeleteAllItems(widget);

		TCITEM item;
		item.mask = TCIF_TEXT;
		std::vector<CEventMessage>::iterator eventIter = g_Events.begin();
		for( ; eventIter != g_Events.end(); eventIter++)
		{
			item.pszText = (char*)(*eventIter).GetTypeText();
			TabCtrl_InsertItem(widget, i++, &item);
		}
	}

	// Set the active tab
	if (g_ActiveEvent >= g_Events.size())
	{
		g_ActiveEvent = g_Events.size() - 1;
	}

	if (g_ActiveEvent < 0) 
	{
		// Hide all windows
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_MESSAGE), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_SINGLE), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_DAILY), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_WEEKLY), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_MONTHLY), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_ANNUALLY), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_VALID_DATE), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_VALID), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_EVERY), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_PROFILE), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_EVERY_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_PROFILE_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_REPEAT_STATIC), SW_HIDE);

		widget = GetDlgItem(window, IDC_EDITEVENT_DELETE);
		EnableWindow(widget, FALSE);		// Enable the widget
		return;
	}
	else
	{
		// Show all windows
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_MESSAGE), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_SINGLE), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_DAILY), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_WEEKLY), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_MONTHLY), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_ANNUALLY), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_VALID_DATE), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_VALID), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_EVERY), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_PROFILE), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_EVERY_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_PROFILE_STATIC), SW_SHOW);
		ShowWindow(GetDlgItem(window, IDC_EDITEVENT_REPEAT_STATIC), SW_SHOW);

		widget = GetDlgItem(window, IDC_EDITEVENT_DELETE);
		EnableWindow(widget, TRUE);		// Enable the widget
	}

	widget = GetDlgItem(window, IDC_EDITEVENT_TAB);
	TabCtrl_SetCurSel(widget, g_ActiveEvent);

	// Type
	int check;
	switch(g_Events[g_ActiveEvent].GetType())
	{
	case EVENT_SINGLE:
		check = IDC_EDITEVENT_SINGLE;
		break;

	case EVENT_DAILY:
		check = IDC_EDITEVENT_DAILY;
		break;

	case EVENT_WEEKLY:
		check = IDC_EDITEVENT_WEEKLY;
		break;

	case EVENT_MONTHLY:
		check = IDC_EDITEVENT_MONTHLY;
		break;

	case EVENT_ANNUALLY:
		check = IDC_EDITEVENT_ANNUALLY;
		break;
	}
	CheckRadioButton(window, IDC_EDITEVENT_SINGLE, IDC_EDITEVENT_ANNUALLY, check);

	// Every Nth
	int index = g_Events[g_ActiveEvent].GetEveryNth();
	SendDlgItemMessage(window, IDC_EDITEVENT_EVERY, CB_SETCURSEL, (LPARAM)index - 1, NULL);

	// Valid Until
	int state = g_Events[g_ActiveEvent].GetValidUntil() != 0 ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(window, IDC_EDITEVENT_VALID, state);

	widget = GetDlgItem(window, IDC_EDITEVENT_VALID_DATE);
	if (state == BST_CHECKED)
	{
		EnableWindow(widget, TRUE);		// Enable the widget
	}
	else
	{
		EnableWindow(widget, FALSE);		// Disable the widget
	}

	SYSTEMTIME time;
	memset(&time, 0, sizeof(SYSTEMTIME));
	if (g_Events[g_ActiveEvent].GetValidUntil() != 0)
	{
		CEventMessage::ValueToDate(g_Events[g_ActiveEvent].GetValidUntil(), &day, &month, &year);
	}
	time.wDay = day;
	time.wMonth = month;
	time.wYear = year;
	DateTime_SetSystemtime(widget, GDT_VALID, &time);

	// Message
	widget = GetDlgItem(window, IDC_EDITEVENT_MESSAGE);
	const std::string& str = g_Events[g_ActiveEvent].GetMessage();
	SetWindowText(widget, str.c_str());

	// Profile
	widget = GetDlgItem(window, IDC_EDITEVENT_PROFILE);
	SetWindowText(widget, g_Events[g_ActiveEvent].GetProfile().c_str());
}

/*
** UpdateEvent
**
** Reads the data from the widgets and puts it in the active event.
**
*/
void UpdateEvent(HWND window)
{
	HWND widget;
	bool state;

	// Type
	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EDITEVENT_DAILY))
	{
		g_Events[g_ActiveEvent].SetType(EVENT_DAILY);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EDITEVENT_WEEKLY))
	{
		g_Events[g_ActiveEvent].SetType(EVENT_WEEKLY);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EDITEVENT_MONTHLY))
	{
		g_Events[g_ActiveEvent].SetType(EVENT_MONTHLY);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EDITEVENT_ANNUALLY))
	{
		g_Events[g_ActiveEvent].SetType(EVENT_ANNUALLY);
	}
	else
	{
		g_Events[g_ActiveEvent].SetType(EVENT_SINGLE);
	}

	// Set the tab text
	widget = GetDlgItem(window, IDC_EDITEVENT_TAB);
	TCITEM item;
	item.mask = TCIF_TEXT;
	item.pszText = (char*)g_Events[g_ActiveEvent].GetTypeText();
	TabCtrl_SetItem(widget, g_ActiveEvent, &item);

	// Every Nth
	int index = SendDlgItemMessage(window, IDC_EDITEVENT_EVERY, CB_GETCURSEL, NULL, NULL);
	g_Events[g_ActiveEvent].SetEveryNth(index + 1);

	// Valid Until
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EDITEVENT_VALID));
	widget = GetDlgItem(window, IDC_EDITEVENT_VALID_DATE);
	if (state)
	{
		SYSTEMTIME time;
		DateTime_GetSystemtime(widget, &time);
		g_Events[g_ActiveEvent].SetValidUntil(CEventMessage::DateToValue(time.wDay, time.wMonth, time.wYear));
		EnableWindow(widget, TRUE);		// Enable the widget
	}
	else
	{
		g_Events[g_ActiveEvent].SetValidUntil(0);
		EnableWindow(widget, FALSE);		// Disable the widget
	}

	// Message
	char buffer[MAX_LINE_LENGTH];
	widget = GetDlgItem(window, IDC_EDITEVENT_MESSAGE);
	GetWindowText(widget, buffer, MAX_LINE_LENGTH);
	g_Events[g_ActiveEvent].SetMessage(buffer);

	// Profile
	index = SendDlgItemMessage(window, IDC_EDITEVENT_PROFILE, CB_GETCURSEL, NULL, NULL);
	if (index >= 0)
	{
		SendDlgItemMessage(window, IDC_EDITEVENT_PROFILE, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer);
		g_Events[g_ActiveEvent].SetProfile(buffer);
	}
	else
	{
		widget = GetDlgItem(window, IDC_EDITEVENT_PROFILE);
		GetWindowText(widget, buffer, MAX_LINE_LENGTH);
		if (strlen(buffer) > 0)
		{
			g_Events[g_ActiveEvent].SetProfile(buffer);
		}
	}
	CCalendarWindow::c_Config.SetCurrentProfile(buffer);
}

BOOL CALLBACK EditEventProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitDialog(hwndDlg);

        case WM_NOTIFY:
			{
				LPNMHDR pNMHDR = (LPNMHDR)lParam;

				if(pNMHDR->code == TCN_SELCHANGE)
				{
					HWND widget;
					widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_TAB);
					g_ActiveEvent = TabCtrl_GetCurSel(pNMHDR->hwndFrom);
					UpdateWidgets(hwndDlg, false);
				}
				else if(pNMHDR->code == DTN_DATETIMECHANGE)
				{
					UpdateEvent(hwndDlg);
				}
			}
			return FALSE;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            {
				case IDC_EDITEVENT_SINGLE:
				case IDC_EDITEVENT_DAILY:
				case IDC_EDITEVENT_WEEKLY:
				case IDC_EDITEVENT_MONTHLY:
				case IDC_EDITEVENT_ANNUALLY:
				case IDC_EDITEVENT_VALID:
					UpdateEvent(hwndDlg);
					break;

				case IDC_EDITEVENT_EVERY:
				case IDC_EDITEVENT_PROFILE:
					if (HIWORD(wParam) == CBN_SELCHANGE || HIWORD(wParam) == CBN_EDITUPDATE)
					{
						UpdateEvent(hwndDlg);
					}
					break;

				case IDC_EDITEVENT_MESSAGE:
					if (HIWORD(wParam) == EN_CHANGE)
					{
						UpdateEvent(hwndDlg);
					}
					break;

				case IDC_EDITEVENT_NEW:
					{
						CEventMessage event;
						int day, month, year;
						day = GetRainlendar()->GetCalendarWindow().GetSelectedDay();
						month = CCalendarWindow::c_MonthsFirstDate.wMonth;
						year = CCalendarWindow::c_MonthsFirstDate.wYear;
						event.SetDate(CEventMessage::DateToValue(day, month, year));
						event.SetProfile(CCalendarWindow::c_Config.GetCurrentProfile());
						g_Events.push_back(event);
						g_ActiveEvent = g_Events.size() - 1;
						UpdateWidgets(hwndDlg, true);
					}
					break;

				case IDC_EDITEVENT_DELETE:
					// Kill the event from eventmanager as well 
					GetRainlendar()->GetCalendarWindow().GetEventManager()->RemoveEvent(g_Events[g_ActiveEvent]);
					g_Events.erase(g_Events.begin() + g_ActiveEvent);
					UpdateWidgets(hwndDlg, true);
					break;

                case IDOK:
					{
						int day, month, year;
						day = GetRainlendar()->GetCalendarWindow().GetSelectedDay();
						month = CCalendarWindow::c_MonthsFirstDate.wMonth;
						year = CCalendarWindow::c_MonthsFirstDate.wYear;

						std::vector<CEventMessage>::iterator eventIter = g_Events.begin();
						for( ; eventIter != g_Events.end(); eventIter++)
						{
							// If the event has different type, we'll remove the old one and set this to new date
							CEventMessage* oldEvent = GetRainlendar()->GetCalendarWindow().GetEventManager()->GetEvent((*eventIter).GetID());
							if (oldEvent && oldEvent->GetType() != (*eventIter).GetType())
							{
								GetRainlendar()->GetCalendarWindow().GetEventManager()->RemoveEvent(*eventIter);
								(*eventIter).SetDate(CEventMessage::DateToValue(day, month, year));
							}

							GetRainlendar()->GetCalendarWindow().GetEventManager()->AddEvent(*eventIter);
						}
						GetRainlendar()->GetCalendarWindow().GetEventManager()->WriteEvents(day, month, year);
					}
                    // Fall through. 
 
                case IDCANCEL: 
                    EndDialog(hwndDlg, wParam); 
                    return TRUE; 
            } 
    } 
    return FALSE; 
} 


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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/EditEvent.cpp,v 1.15 2003/08/10 08:44:38 Rainy Exp $

  $Log: EditEvent.cpp,v $
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
#include "../Server/EventCombiner.h"
#include <Commdlg.h>
#include <time.h>
#include <richedit.h>

// These need to be global (even though globals are evil)
static std::vector<CEventMessage*> g_DeletedEvents;
time_t g_TimeStamp = 0;
HINSTANCE g_Instance;

// Prototypes
BOOL CALLBACK EditEventProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
int CALLBACK PropSheetProc(HWND hwndDlg, UINT uMsg, LPARAM lParam);
void UpdateWidgets(HWND window, CEventMessage& event);
BOOL CALLBACK SyncProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

VOID OpenEditEventDialog(HWND hwndOwner, HINSTANCE instance, UINT date, UINT id)
{
	char caption[256];
	int day, month, year;
    PROPSHEETPAGE* psp;
    PROPSHEETHEADER psh;
	int i;

	g_Instance = instance;

    time(&g_TimeStamp);     // This stamp will be used for the modified events

    if (CCalendarWindow::c_Config.GetServerEnable() && CCalendarWindow::c_Config.GetServerSyncOnEdit() && !CCalendarWindow::c_Config.GetServerAddress().empty())
    {
		// Open a dialog "Please wait..."
		DialogBox(instance, MAKEINTRESOURCE(IDD_SYNC_DIALOG), hwndOwner, (DLGPROC)SyncProc); 
    }

	CEventMessage::ValueToDate(date, &day, &month, &year);
	const std::string monthStr = CCalendarWindow::c_Language.GetString("Menu", 5 + month - 1);
	sprintf(caption, "%i-%s-%i", day, monthStr.c_str(), year);

	// Get the events from event manager
	std::vector<CEventMessage*> eventList = GetRainlendar()->GetCalendarWindow().GetEventManager()->GetEvents(day, month, year);

	// Remove unnecessary events
	std::vector<CEventMessage*> events;
	std::vector<CEventMessage*>::iterator iter = eventList.begin();
	for( ; iter != eventList.end(); iter++)
	{
		CEventMessage* event = new CEventMessage(*(*iter));
		if (!(event->IsDeleted()) && event->IsPermanent())	// No deleted messages and only permanent can be edited
		{
			events.push_back(event);
		}
	}

	if (events.empty())
	{
		// Create a empty event if there are no events on this day
		CEventMessage* event = new CEventMessage;
		event->SetDate(CEventMessage::DateToValue(day, month, year));
		event->SetProfile(CCalendarWindow::c_Config.GetCurrentProfile());
		events.push_back(event);
	}

	psp = new PROPSHEETPAGE[events.size()];

	int startPage = 0;
	for (i = 0; i < events.size(); i++)
	{
		const char* profile = CCalendarWindow::c_Language.GetTranslatedProfile(events[i]->GetProfile().c_str());

		if (id != 0 && events[i]->GetID() == id)
		{
			startPage = i;
		}

		psp[i].dwSize = sizeof(PROPSHEETPAGE);
		psp[i].dwFlags = PSP_USETITLE;
		psp[i].hInstance = instance;
		psp[i].pszTemplate = MAKEINTRESOURCE(IDD_EDITEVENT_PAGE);
		psp[i].pszIcon = NULL;
		psp[i].pfnDlgProc = EditEventProc;
		psp[i].pszTitle = profile;
		psp[i].lParam = (LPARAM)events[i];
		psp[i].pfnCallback = NULL;
	}

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP | PSH_USECALLBACK;
    psh.hwndParent = hwndOwner;
    psh.hInstance = instance;
    psh.pszIcon = NULL;
    psh.pszCaption = caption;
    psh.nPages = events.size();
    psh.nStartPage = startPage;
    psh.ppsp = (LPCPROPSHEETPAGE)psp;
    psh.pfnCallback = PropSheetProc;

	HMODULE richDLL = LoadLibrary("RichEd32.dll");

	if (PropertySheet(&psh) == IDOK)
	{
		GetRainlendar()->GetCalendarWindow().GetEventManager()->WriteEvents(day, month, year);
		
		LSLog(LOG_DEBUG, "Rainlendar", "OK pressed. Sending Events to the server.");		
		// Send the events to the server
		GetRainlendar()->GetCalendarWindow().ConnectServer(REQUEST_SENDEVENTS);

		// Refresh
		SendMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);
	}

	FreeLibrary(richDLL);

	delete [] psp;

	for (i = 0; i < g_DeletedEvents.size(); i++)
	{
		delete g_DeletedEvents[i];
	}
	g_DeletedEvents.clear();
}

BOOL SetPageTitle(HWND propSheet, HWND page, const char* title)
{
	int index = SendMessage(propSheet, PSM_HWNDTOINDEX, (WPARAM)page, 0);
	if (index != -1)
	{
		HWND tab = (HWND)SendMessage(propSheet, PSM_GETTABCONTROL, 0, 0);

		TC_ITEM ti;
		ti.mask = TCIF_TEXT;
		ti.pszText = (char*)title;
		SendMessage(tab, TCM_SETITEM, (WPARAM)index, (LPARAM)&ti);
	    return TRUE;
	}
	return FALSE;
}

/////////////////////////// The Property Sheet /////////////////////////////////
typedef LONG (CALLBACK *DialogProc)(HWND, UINT, WPARAM, LPARAM);

DialogProc g_DefaultDlgProc;

INT_PTR CALLBACK PropSheetDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_SIZE)
	{
		// Reposition the custom buttons
		RECT rc, btnRc;
		HWND widget;
		GetClientRect(hwndDlg, &rc);
		widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_NEW);
		GetClientRect(widget, &btnRc);
		SetWindowPos(widget, NULL, 6, rc.bottom - btnRc.bottom - 7, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
		widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_DELETE);
		SetWindowPos(widget, NULL, 12 + btnRc.right, rc.bottom - btnRc.bottom - 7, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	else if (uMsg == WM_DESTROY)
	{
		RECT rc;
		GetWindowRect(hwndDlg, &rc);
		CCalendarWindow::c_Config.SetDialogPosition(CConfig::DIALOG_EDITEVENT, rc.left, rc.top);
	}
	else if (uMsg == WM_COMMAND)
	{
		if (wParam == IDC_EDITEVENT_NEW)
		{
			CEventMessage* newEvent = new CEventMessage;
			int day, month, year;
			HWND curPage = (HWND)SendMessage(hwndDlg, PSM_GETCURRENTPAGEHWND, 0, 0);
			CEventMessage* event = (CEventMessage*)GetWindowLong(curPage, GWL_USERDATA);
			int date = event->GetDate();
			CEventMessage::ValueToDate(date, &day, &month, &year);
			newEvent->SetDate(CEventMessage::DateToValue(day, month, year));
			newEvent->SetProfile(CCalendarWindow::c_Config.GetCurrentProfile());
			const char* profile = CCalendarWindow::c_Language.GetTranslatedProfile(newEvent->GetProfile().c_str());

			if (!IsWindowEnabled(GetDlgItem(hwndDlg, IDC_EDITEVENT_DELETE)))
			{
				// Delete is disabled -> No events
				// Just enable the hidden tab
				HWND curPage = (HWND)SendMessage(hwndDlg, PSM_GETCURRENTPAGEHWND, 0, 0);
				SetPageTitle(hwndDlg, curPage, profile);
				ShowWindow(curPage, SW_SHOW);		// Show the hidden page
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDITEVENT_DELETE), TRUE);	// Enable delete

				// Fill the page from the newEvent
				UpdateWidgets(curPage, *newEvent);

				SetWindowLong(curPage, GWL_USERDATA, (LONG)newEvent);
			}
			else
			{
				// Gotta create a new tab
				PROPSHEETPAGE psp;

				psp.dwSize = sizeof(PROPSHEETPAGE);
				psp.dwFlags = PSP_USETITLE;
				psp.hInstance = g_Instance;
				psp.pszTemplate = MAKEINTRESOURCE(IDD_EDITEVENT_PAGE);
				psp.pszIcon = NULL;
				psp.pfnDlgProc = EditEventProc;
				psp.pszTitle = profile;
				psp.lParam = (LPARAM)newEvent;
				psp.pfnCallback = NULL;

				// Add new page
				HPROPSHEETPAGE newPage = CreatePropertySheetPage(&psp);
				SendMessage(hwndDlg, PSM_ADDPAGE, 0, (LPARAM)newPage);
				SendMessage(hwndDlg, PSM_RECALCPAGESIZES, 0, 0);
				// Post the activate message since the page is not actually created yet
				PostMessage(hwndDlg, PSM_SETCURSEL, 0, (WPARAM)newPage);
			}

			return TRUE;
		}
		else if (wParam == IDC_EDITEVENT_DELETE)
		{
			// Delete current page
			HWND curPage = (HWND)SendMessage(hwndDlg, PSM_GETCURRENTPAGEHWND, 0, 0);
			CEventMessage* event = (CEventMessage*)GetWindowLong(curPage, GWL_USERDATA);

			HWND tab = (HWND)SendMessage(hwndDlg, PSM_GETTABCONTROL, 0, 0);
			if (SendMessage(tab, TCM_GETITEMCOUNT, 0, 0) == 1)
			{
				// This was the last page
				ShowWindow(curPage, SW_HIDE);
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDITEVENT_DELETE), FALSE);	// Disable delete
				SetPageTitle(hwndDlg, 0, "");
				SetWindowLong(curPage, GWL_USERDATA, NULL);
			}
			else
			{
				int i = SendMessage(hwndDlg, PSM_HWNDTOINDEX, (WPARAM)curPage, 0);
				SendMessage(hwndDlg, PSM_REMOVEPAGE, i, 0);
			}
			event->SetDeleted();
			event->SetTimeStamp(g_TimeStamp);
			g_DeletedEvents.push_back(event);

			SendMessage(hwndDlg, PSM_RECALCPAGESIZES, 0, 0);

			return TRUE;
		}
	}

	return CallWindowProc(g_DefaultDlgProc, hwndDlg, uMsg, wParam, lParam);	// Call the base implementation
}

int CALLBACK PropSheetProc(HWND hwndDlg, UINT uMsg, LPARAM lParam)
{
	if (uMsg == PSCB_INITIALIZED)
	{
		// Override the WndProc so that we can catch the custom button WM_COMMANDs
		g_DefaultDlgProc = (DialogProc)GetWindowLong(hwndDlg, DWL_DLGPROC);
		SetWindowLong(hwndDlg, DWL_DLGPROC, (LONG)PropSheetDialogProc);

		// Create Add and Delete buttons
		RECT rc;
		HWND okButton = GetDlgItem(hwndDlg, IDOK);
		HWND cancelButton = GetDlgItem(hwndDlg, IDCANCEL);
		GetClientRect(okButton, &rc);
		HWND newButton = CreateWindow("BUTTON", "New", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hwndDlg, (HMENU)IDC_EDITEVENT_NEW, NULL, NULL);
		HWND deleteButton = CreateWindow("BUTTON", "Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hwndDlg, (HMENU)IDC_EDITEVENT_DELETE, NULL, NULL);

		// Set the localized texts
		SetWindowText(okButton, CCalendarWindow::c_Language.GetString("General", 0));
		SetWindowText(cancelButton, CCalendarWindow::c_Language.GetString("General", 1));
		SetWindowText(newButton, CCalendarWindow::c_Language.GetString("EventGUI", 0));
		SetWindowText(deleteButton, CCalendarWindow::c_Language.GetString("EventGUI", 1));

		// Set the same font as in Ok-button
		HFONT font = (HFONT)SendMessage(okButton, WM_GETFONT, 0, 0);
		SendMessage(newButton, WM_SETFONT, (WPARAM)font, NULL);
		SendMessage(deleteButton, WM_SETFONT, (WPARAM)font, NULL);

		if (CCalendarWindow::c_Config.GetRememberDialogPositions())
		{
			POINT pos = CCalendarWindow::c_Config.GetDialogPosition(CConfig::DIALOG_EDITEVENT);
			SetWindowPos(hwndDlg, NULL, pos.x, pos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
	}

	return 0;
}

/////////////////////////// The Property Page(s) /////////////////////////////////

BOOL OnInitDialog(HWND window, PROPSHEETPAGE* psp)
{
    HWND widget;
	CEventMessage* event = (CEventMessage*)psp->lParam;

	SetWindowLong(window, GWL_USERDATA, (LONG)event);	// Store the index to the g_Events-vector

	// Set localized strings
	widget = GetDlgItem(window, IDC_EDITEVENT_SINGLE);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 2));
	widget = GetDlgItem(window, IDC_EDITEVENT_DAILY);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 3));
	widget = GetDlgItem(window, IDC_EDITEVENT_WEEKLY);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 4));
	widget = GetDlgItem(window, IDC_EDITEVENT_MONTHLY);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 5));
	widget = GetDlgItem(window, IDC_EDITEVENT_ANNUALLY);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 6));
	widget = GetDlgItem(window, IDC_EDITEVENT_VALID);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 7));
	widget = GetDlgItem(window, IDC_EDITEVENT_EVERY_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 8));
	widget = GetDlgItem(window, IDC_EDITEVENT_PROFILE_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 9));
	widget = GetDlgItem(window, IDC_EDITEVENT_REPEAT_STATIC);
	if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 10));

	for (int j = 0; j < 12 ; j++)
	{
		SendDlgItemMessage(window, IDC_EDITEVENT_EVERY, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetString("Numbers", j));
	}

	// Fill in the profiles from skin
	SendDlgItemMessage(window, IDC_EDITEVENT_PROFILE, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetTranslatedProfile("Default"));

	const std::list<Profile*>& allProfiles = CCalendarWindow::c_Config.GetAllProfiles();
	std::list<Profile*>::const_iterator profileIter = allProfiles.begin();
	for( ; profileIter != allProfiles.end(); profileIter++)
	{
		const char* profile = (*profileIter)->name.c_str();
		SendDlgItemMessage(window, IDC_EDITEVENT_PROFILE, CB_ADDSTRING, NULL, (LPARAM)CCalendarWindow::c_Language.GetTranslatedProfile(profile));
	}

	UpdateWidgets(window, *event);

	widget = GetDlgItem(window, IDC_EDITEVENT_MESSAGE);
	SetFocus(widget);
	SendMessage(widget, EM_SETSEL, 0, 0);
	SendMessage(widget, EM_SETEVENTMASK, 0, ENM_CHANGE);

	return TRUE;
}

/*
** UpdateWidgets
**
** Reads the data from the active event and puts it in the widgets.
**
*/
void UpdateWidgets(HWND window, CEventMessage& event)
{
	HWND widget;
	int day, month, year;

	// Type
	int check;
	switch(event.GetType())
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
	int nth = event.GetEveryNth();
	SendDlgItemMessage(window, IDC_EDITEVENT_EVERY, CB_SETCURSEL, (LPARAM)nth - 1, NULL);

	// Valid Until
	int state = event.GetValidUntil() != 0 ? BST_CHECKED : BST_UNCHECKED;
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
	if (event.GetValidUntil() != 0)
	{
		CEventMessage::ValueToDate(event.GetValidUntil(), &day, &month, &year);
		time.wDay = day;
		time.wMonth = month;
		time.wYear = year;
		DateTime_SetSystemtime(widget, GDT_VALID, &time);
	}

	// Message
	widget = GetDlgItem(window, IDC_EDITEVENT_MESSAGE);
	const std::string& str = event.GetMessage();
	SendMessage(widget, WM_SETTEXT, 0, (LPARAM)str.c_str());

	// Profile
	widget = GetDlgItem(window, IDC_EDITEVENT_PROFILE);
	const char* profile = event.GetProfile().c_str();
	SetWindowText(widget, CCalendarWindow::c_Language.GetTranslatedProfile(profile));
}

/*
** UpdateEvent
**
** Reads the data from the widgets and puts it in the event.
**
*/
void UpdateEvent(HWND window, CEventMessage& event)
{
	HWND widget;
	bool state;

	// Type
	if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EDITEVENT_DAILY))
	{
		event.SetType(EVENT_DAILY);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EDITEVENT_WEEKLY))
	{
		event.SetType(EVENT_WEEKLY);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EDITEVENT_MONTHLY))
	{
		event.SetType(EVENT_MONTHLY);
	}
	else if (BST_CHECKED == IsDlgButtonChecked(window, IDC_EDITEVENT_ANNUALLY))
	{
		event.SetType(EVENT_ANNUALLY);
	}
	else
	{
		event.SetType(EVENT_SINGLE);
	}

	// Every Nth
	int index = SendDlgItemMessage(window, IDC_EDITEVENT_EVERY, CB_GETCURSEL, NULL, NULL);
	event.SetEveryNth(index + 1);

	// Valid Until
	state = (BST_CHECKED == IsDlgButtonChecked(window, IDC_EDITEVENT_VALID));
	widget = GetDlgItem(window, IDC_EDITEVENT_VALID_DATE);
	if (state)
	{
		SYSTEMTIME time;
		DateTime_GetSystemtime(widget, &time);
		event.SetValidUntil(CEventMessage::DateToValue(time.wDay, time.wMonth, time.wYear));
		EnableWindow(widget, TRUE);		// Enable the widget
	}
	else
	{
		event.SetValidUntil(0);
		EnableWindow(widget, FALSE);		// Disable the widget
	}

	// Message
	char buffer[MAX_LINE_LENGTH];
	buffer[0] = 0;
	widget = GetDlgItem(window, IDC_EDITEVENT_MESSAGE);
	SendMessage(widget, EM_SETSEL, (WPARAM)0, (LPARAM)(MAX_LINE_LENGTH - 1));
	SendMessage(widget, EM_GETSELTEXT, 0, (LPARAM)buffer);
	event.SetMessage(buffer);

	// Profile
	index = SendDlgItemMessage(window, IDC_EDITEVENT_PROFILE, CB_GETCURSEL, NULL, NULL);
	if (index >= 0)
	{
		SendDlgItemMessage(window, IDC_EDITEVENT_PROFILE, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer);
		if (strlen(buffer) > 0)
		{
			event.SetProfile(CCalendarWindow::c_Language.GetOriginalProfile(buffer));
		}
	}
	else
	{
		widget = GetDlgItem(window, IDC_EDITEVENT_PROFILE);
		GetWindowText(widget, buffer, MAX_LINE_LENGTH);
		if (strlen(buffer) > 0)
		{
			event.SetProfile(CCalendarWindow::c_Language.GetOriginalProfile(buffer));
		}
	}
}

BOOL CALLBACK EditEventProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) 
    { 
        case WM_INITDIALOG:
			return OnInitDialog(hwndDlg, (LPPROPSHEETPAGE)lParam);

        case WM_NOTIFY:
			{
				LPNMHDR pNMHDR = (LPNMHDR)lParam;

				if(pNMHDR->code == PSN_APPLY)
				{
					CEventMessage* event = (CEventMessage*)GetWindowLong(hwndDlg, GWL_USERDATA);
					if (event)	// Event can be NULL if the last one was deleted
					{
						UpdateEvent(hwndDlg, *event);

						event->SetTimeStamp(g_TimeStamp);    // The modification time was when the dialog was opened
						GetRainlendar()->GetCalendarWindow().GetEventManager()->AddEvent(*event);
						delete event;	// This is not needed anymore, since AddEvent creates a copy
					}

					// Remove all deleted messages (this is done only by the first page)
					for (int i = 0; i < g_DeletedEvents.size(); i++)
					{
						// The newly created and deleted events are just ignored since they're not in the EventManager
						GetRainlendar()->GetCalendarWindow().GetEventManager()->RemoveEvent(*g_DeletedEvents[i]);
						delete g_DeletedEvents[i];
					}
					g_DeletedEvents.clear();

					SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
					return TRUE;
				}
			}
			break;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            {
				case IDC_EDITEVENT_VALID:
					{
						int state = (BST_CHECKED == IsDlgButtonChecked(hwndDlg, IDC_EDITEVENT_VALID));
						HWND widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_VALID_DATE);
						if (state == BST_CHECKED)
						{
							EnableWindow(widget, TRUE);		// Enable the widget
						}
						else
						{
							EnableWindow(widget, FALSE);		// Disable the widget
						}
					}
					break;

				case IDC_EDITEVENT_PROFILE:
					if (HIWORD(wParam) == CBN_SELCHANGE || HIWORD(wParam) == CBN_EDITUPDATE)
					{
						// When profile changes update the caption
						CEventMessage* event = (CEventMessage*)GetWindowLong(hwndDlg, GWL_USERDATA);
						if (event)
						{
							char buffer[MAX_LINE_LENGTH];
							int index = SendDlgItemMessage(hwndDlg, IDC_EDITEVENT_PROFILE, CB_GETCURSEL, NULL, NULL);
							if (index >= 0)
							{
								SendDlgItemMessage(hwndDlg, IDC_EDITEVENT_PROFILE, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer);
							}
							else
							{
								GetWindowText(GetDlgItem(hwndDlg, IDC_EDITEVENT_PROFILE), buffer, MAX_LINE_LENGTH);
							}
							SetPageTitle(GetParent(hwndDlg), hwndDlg, buffer);
							CCalendarWindow::c_Config.SetCurrentProfile(CCalendarWindow::c_Language.GetOriginalProfile(buffer));
							CCalendarWindow::c_Config.WriteConfig(CConfig::WRITE_PROFILE);
						}
					}
					break;
            }
            break;
    } 
    return FALSE; 
} 


/////////////////////////// Please wait dialog /////////////////////////////////
#define PBS_MARQUEE 0x08
#define PBM_SETMARQUEE (WM_USER+10)

BOOL CALLBACK SyncProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) 
    { 
        case WM_INITDIALOG:
			{
				static NetworkParams param;

				// Here is a slight possibility of unsafe thread access
				// If the OnServerSync() is started multiple times before the
				// thread has a chance to copy the information, the data might
				// corrupt. Hopefully this doesn't occur too often :-)
				param.serverPort = CCalendarWindow::c_Config.GetServerPort();
				param.serverAddress = CCalendarWindow::c_Config.GetServerAddress();
				param.userID = CCalendarWindow::c_Config.GetServerID();
				param.requestType = REQUEST_GETEVENTS;
				param.window = hwndDlg;

				// Launch the network communication thread
				DWORD id;
				HANDLE thread = CreateThread(NULL, 0, NetworkThreadProc, &param, 0, &id);
				CloseHandle(thread);

				HWND widget = GetDlgItem(hwndDlg, IDC_EDITEVENT_SYNC_TEXT);
				if (widget) SetWindowText(widget, CCalendarWindow::c_Language.GetString("EventGUI", 11));			
				SetWindowText(hwndDlg, CCalendarWindow::c_Language.GetString("ServerConfigGUI", 1));			

				// Make the progress bar marquee (XP only)
				widget = GetDlgItem(hwndDlg, IDC_SYNC_PROGRESS);
				DWORD style = GetWindowLong(widget, GWL_STYLE);
				SetWindowLong(widget, GWL_STYLE, style | PBS_MARQUEE);
				SendMessage(widget, PBM_SETMARQUEE, TRUE, 50);
			}
			break;
	
        case WM_SERVER_SYNC_FINISHED:
            if (wParam == ERROR_CODE_NEWEVENTS)
            {
                GetRainlendar()->GetCalendarWindow().RefreshWindow();
            }
		    EndDialog(hwndDlg, IDCANCEL); 
            return TRUE; 
			
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            {
                case IDCANCEL:
		            EndDialog(hwndDlg, IDCANCEL); 
                    return TRUE; 
            }
            break;
    } 
    return FALSE; 
} 


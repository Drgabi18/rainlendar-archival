/*
  Copyright (C) 2003 Kimmo Pekkola

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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/Outlook.cpp,v 1.5 2003/08/09 15:27:07 Rainy Exp $

  $Log: Outlook.cpp,v $
  Revision 1.5  2003/08/09 15:27:07  Rainy
  Events are now returned with GetOutlookEvents.

  Revision 1.4  2003/06/15 20:04:05  Rainy
  The strings are free'ed.

  Revision 1.3  2003/05/24 13:52:35  Rainy
  Outlook installation path changed

  Revision 1.2  2003/05/07 19:02:26  rainy
  Initial version.

*/

#pragma warning(disable: 4786)

#include "CalendarWindow.h"
#include <assert.h>

#import "D:\Program Files\Common Files\Microsoft Shared\Office10\mso.dll" named_guids
#import "D:\Program Files\Microsoft Office\Office10\MSOUTL.OLB" no_namespace exclude("_IRecipientControl", "_DRecipientControl")

std::map<DATE, int> g_OutlookEventIDs;
std::list<CEventMessage> g_OutlookEvents;

bool GetOutlookEvents(CEventManager* eventManager)
{
    if (!eventManager || !CCalendarWindow::c_Config.GetShowOutlookAppointments())
    {
        return false;
    }

	std::list<CEventMessage>::iterator iter = g_OutlookEvents.begin();
	for ( ; iter != g_OutlookEvents.end(); iter++)
	{
		eventManager->AddEvent(*iter);
	}

	return true;
}

// This just updates the outlook events
bool SyncWithOutlook()
{
    bool ret = false;

    if (!CCalendarWindow::c_Config.GetShowOutlookAppointments())
    {
        return ret;
    }

	g_OutlookEvents.clear();	// Delete old Outlook events

    HRESULT hr;

    hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
        LSLog(LOG_DEBUG, "Rainlendar", "Unable to initialize COM.");
        return ret;
	}

	try
	{
	    _ApplicationPtr pApp;
	    _ItemsPtr pItems;
	    MAPIFolderPtr pFolder;
	    _AppointmentItemPtr pContact;

		hr = pApp.CreateInstance(__uuidof(Application));
		if (FAILED(hr))
		{
            LSLog(LOG_DEBUG, "Rainlendar", "Unable to instantiate Outlook.");
            return ret;
		}

		pFolder=pApp->GetNamespace(_bstr_t("MAPI"))->GetDefaultFolder(olFolderCalendar);
		if (pFolder==NULL)
		{
            LSLog(LOG_DEBUG, "Rainlendar", "Could not find default contacts folder.");
            return ret;
		}
			
		pItems=pFolder->GetItems();
		if (pItems==NULL)
		{
            LSLog(LOG_DEBUG, "Rainlendar", "Unable to get Contact Items from Outlook.");
            return ret;
		}
		
		pContact=pItems->GetFirst();

		while(1)
		{
			if (pContact==NULL) break;

            CEventMessage newEvent(false);      // Non-permanent event

            // Check if the event was modified
            DATE creationDate = pContact->GetCreationTime();
            std::map<DATE, int>::iterator iter = g_OutlookEventIDs.find(creationDate);
            if (iter != g_OutlookEventIDs.end())
            {
                newEvent.SetID((*iter).second);
            }
            else
            {
                ret = true;     // New event, must refresh
            }

            int sDay, sMonth, sYear;
            int sHour, sMinute, sSecond;
            int eDay, eMonth, eYear;
            int eHour, eMinute, eSecond;

            DATE start = pContact->GetStart();
            CEventMessage::GetDate(start, &sYear, &sMonth, &sDay, &sHour, &sMinute, &sSecond);

            DATE end = pContact->GetEnd();
            CEventMessage::GetDate(end, &eYear, &eMonth, &eDay, &eHour, &eMinute, &eSecond);

            newEvent.SetDate(CEventMessage::DateToValue(sDay, sMonth, sYear));
            newEvent.SetProfile("Outlook");

            BSTR strSubject = pContact->GetSubject();
            if (strSubject)
            {
                int size;
                char* buffer;
                std::string message;

                // Add the start and end time if they are different
                if (sHour != eHour || sMinute != eMinute)
                {
                    buffer = new char[256];
                    sprintf(buffer, "%i:%02i - %i:%02i ", sHour, sMinute, eHour, eMinute);
                    message += buffer;
                    delete [] buffer;
                }

                size = lstrlenW(strSubject);
                buffer = new char[size + 1];
                WideCharToMultiByte(CP_ACP, 0, strSubject, -1, buffer, size + 1, NULL, NULL);
                message += buffer;
                delete [] buffer;
				SysFreeString(strSubject);

                BSTR strLocation = pContact->GetLocation();
                if (strLocation)
                {
                    message += " (";

                    size = lstrlenW(strLocation);
                    buffer = new char[size + 1];
                    WideCharToMultiByte(CP_ACP, 0, strLocation, -1, buffer, size + 1, NULL, NULL);
                    message += buffer;
                    delete [] buffer;

                    message += ")";
                }
				SysFreeString(strLocation);

                BSTR strBody = pContact->GetBody();
                if (strBody)
                {
                    message += "\n";
                    size = lstrlenW(strBody);
                    buffer = new char[size + 1];
                    WideCharToMultiByte(CP_ACP, 0, strBody, -1, buffer, size + 1, NULL, NULL);
                    message += buffer;
                    delete [] buffer;
                }
				SysFreeString(strBody);
                newEvent.SetMessage(message);

                OlRecurrenceState state = pContact->GetRecurrenceState();
                if (state == olApptMaster)
                {
                    // Recurrening event
                    RecurrencePatternPtr pRecurrence = pContact->GetRecurrencePattern();
                    OlRecurrenceType type = pRecurrence->GetRecurrenceType();

                    switch (type)
                    {
                    case olRecursDaily:
                        newEvent.SetType(EVENT_DAILY);
                        break;

                    case olRecursWeekly:
                        newEvent.SetType(EVENT_WEEKLY);
                        break;

                    case olRecursMonthly:
                    case olRecursMonthNth:
                        newEvent.SetType(EVENT_MONTHLY);
                        break;

                    case olRecursYearly:
                    case olRecursYearNth:
                        newEvent.SetType(EVENT_ANNUALLY);
                        break;

                    default:
                        newEvent.SetType(EVENT_SINGLE);
                    }

                    if (!pRecurrence->GetNoEndDate())
                    {
                        DATE endTime = pRecurrence->GetPatternEndDate();
                        CEventMessage::GetDate(endTime, &eYear, &eMonth, &eDay, &eHour, &eMinute, &eSecond);
                        newEvent.SetValidUntil(CEventMessage::DateToValue(eDay, eMonth, eYear));
                    }

                    int interval = pRecurrence->GetInterval();

					if (type == olRecursYearly || type == olRecursYearNth)
					{
						interval /= 12;	// Outlook has 12 as yearly recurrence. Wierd.
					}
                    newEvent.SetEveryNth(interval);

                }
                else if (!pContact->GetAllDayEvent() && (sDay < eDay || sMonth < eMonth || sYear < eYear))
                {
                    // The appointment lasts longer than one day
                    newEvent.SetType(EVENT_DAILY);
                    newEvent.SetValidUntil(CEventMessage::DateToValue(eDay, eMonth, eYear));
                }

				g_OutlookEvents.push_back(newEvent);
                g_OutlookEventIDs[creationDate] = newEvent.GetID();
            }

			pContact=pItems->GetNext();
		}
	}
	catch(_com_error &e)
	{
        LSLog(LOG_DEBUG, "Rainlendar", (char*)e.Description());
	}

    CoUninitialize();

    return ret;
}

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
  $Header: /home/cvsroot/Rainlendar/Plugin/Outlook.cpp,v 1.10 2004/04/24 11:22:17 rainy Exp $

  $Log: Outlook.cpp,v $
  Revision 1.10  2004/04/24 11:22:17  rainy
  Uses MAPI.

  Revision 1.9  2004/01/28 18:05:52  rainy
  no message

  Revision 1.8  2003/10/27 17:40:01  Rainy
  Config is now singleton.

  Revision 1.7  2003/10/04 14:55:25  Rainy
  Added more room for strings.

  Revision 1.6  2003/10/04 14:52:03  Rainy
  Bug fix in error situations.
  Events are identified by their ID.
  Clock can be now in 12-hour format too.

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
#include <ole2.h>
#include <initguid.h>
#include <mapix.h>
#include <mapiutil.h>
#include <sqltypes.h>
#include <mapidefs.h>
#include <shlwapi.h>

// http://www.geocities.com/cainrandom/dev/MAPIRecurrence.html
// http://www.cdolive.com/cdo10.htm

LPMAPISESSION g_Session = NULL;
std::list<CEventMessage*> g_OutlookEvents;

enum { E_ROOT_DEFAULT = 0, E_ROOT_EID, E_ROOT_PROPS};
SizedSPropTagArray(E_ROOT_PROPS, rootProps) = 
{ E_ROOT_PROPS, 
	{	
		PR_DEFAULT_STORE, 
		PR_ENTRYID
	} 
};

enum { E_DATASTORE_EID = 0, E_DATASTORE_NAME, E_DATASTORE_CONTAINER, E_DATASTORE_PROPS};
SizedSPropTagArray(E_DATASTORE_PROPS, datastoreProps) =
{ E_DATASTORE_PROPS,
    {   
		PR_ENTRYID,
        PR_DISPLAY_NAME,
        PR_CONTAINER_CLASS
    }
}; 

SizedSPropTagArray(1, messageEIDProps) =
{
    1,
    {
		PR_ENTRYID,
	}
};

enum { E_MESSAGE_EID = 0, E_MESSAGE_SUBJECT, E_MESSAGE_BODY, E_MESSAGE_START_DATE, E_MESSAGE_END_DATE, E_MESSAGE_VALID_UNTIL, E_MESSAGE_LOCATION, E_MESSAGE_RECURRENCE, E_MESSAGE_ALLDAY, E_MESSAGE_LABEL, E_MESSAGE_PROPS};
SizedSPropTagArray(E_MESSAGE_PROPS, messageProps) =
{
    E_MESSAGE_PROPS,
    {
		PR_ENTRYID,
        PR_SUBJECT,
        PR_BODY,
		0,		// PR_APPOINTMENT_START,
		0,		// PR_APPOINTMENT_END,
		0,		// PR_APPOINTMENT_VALID_UNTIL,
		0,		// PR_APPOINTMENT_LOCATION,
		0,		// PR_APPOINTMENT_RECURRENCE,
		0,		// PR_APPOINTMENT_ALLDAY
		0		// PR_APPOINTMENT_LABEL,		(label must be that last)
    }
};

#pragma pack(1)

struct RECURRENCE_HEADER
{
	ULONG unknown1;		// 0x04, 0x30, 0x04, 0x30
	UCHAR type;
	UCHAR unknown2;		// 0x20
	ULONG type2;		// Recurrence type
};

struct RECURRENCE_DAILY
{
	ULONG unknown1;		// 0x04, 0x30, 0x04, 0x30
	UCHAR type;
	UCHAR unknown2;		// 0x20
	ULONG type2;		// Recurrence type
	// Daily stuff starts here
	ULONG separation;
	ULONG interval;
	ULONG unknown3;		// 0x00
};

struct RECURRENCE_WEEKLY
{
	ULONG unknown1;		// 0x04, 0x30, 0x04, 0x30
	UCHAR type;
	UCHAR unknown2;		// 0x20
	ULONG type2;		// Recurrence type
	// Weekly stuff starts here
	ULONG separation;
	ULONG interval;
	ULONG unknown3;		// 0x00
	ULONG bitmask;
};

struct RECURRENCE_MONTHLY
{
	ULONG unknown1;		// 0x04, 0x30, 0x04, 0x30
	UCHAR type;
	UCHAR unknown2;		// 0x20
	ULONG type2;		// Recurrence type
	// Monthly stuff starts here
	ULONG separation;
	ULONG interval;
	ULONG unknown3;		// 0x00
	ULONG day;
};

struct RECURRENCE_YEARLY
{
	ULONG unknown1;		// 0x04, 0x30, 0x04, 0x30
	UCHAR type;
	UCHAR unknown2;		// 0x20
	ULONG type2;		// Recurrence type
	// Yearly stuff starts here
	ULONG separation;
	ULONG interval;
	ULONG unknown3;		// 0x00
	ULONG day;
};

void FiletimeToDate(FILETIME* pft, int *year, int *month, int *day, int *hour, int *minute, int *second)
{
    FILETIME ft;
    SYSTEMTIME systime;
	bool dst = false;

    FileTimeToLocalFileTime(pft, &ft);
    FileTimeToSystemTime(&ft, &systime);

	// Daylight saving time compensation
	TIME_ZONE_INFORMATION tzi;
	DWORD zone = GetTimeZoneInformation(&tzi);

	// If DST is disabled, no need to do anything
	if (zone != TIME_ZONE_ID_UNKNOWN)
	{
		int dst_date = tzi.DaylightDate.wMonth * 10000 + tzi.DaylightDate.wDay * 100 + tzi.DaylightDate.wHour;
		int std_date = tzi.StandardDate.wMonth * 10000 + tzi.StandardDate.wDay * 100 + tzi.StandardDate.wHour;
		int cur_date = systime.wMonth * 10000 + systime.wDay * 100 + systime.wHour;

		if (dst_date < std_date)
		{
			if ((cur_date > dst_date) && (cur_date < std_date))
			{
				dst = true;
			}
		}
		else
		{
			if ((cur_date < dst_date) && (cur_date > std_date))
			{
				dst = true;
			}
		}

		ULARGE_INTEGER largeBias;
		largeBias.QuadPart = tzi.DaylightBias * 60;
		largeBias.QuadPart = largeBias.QuadPart * 10000000;

		if (TIME_ZONE_ID_DAYLIGHT == zone)
		{
			// DST is on, add bias to dates outside the DST
			if (!dst)
			{
				ULARGE_INTEGER largeTime;
				largeTime.LowPart = ft.dwLowDateTime;
				largeTime.HighPart = ft.dwHighDateTime;
				largeTime.QuadPart += largeBias.QuadPart;
				ft.dwLowDateTime = largeTime.LowPart;
				ft.dwHighDateTime = largeTime.HighPart;
				FileTimeToSystemTime(&ft, &systime);
			}
		}
		else
		{
			// DST is off, subtract bias from dates inside DST
			if (dst)
			{
				ULARGE_INTEGER largeTime;
				largeTime.LowPart = ft.dwLowDateTime;
				largeTime.HighPart = ft.dwHighDateTime;
				largeTime.QuadPart -= largeBias.QuadPart;
				ft.dwLowDateTime = largeTime.LowPart;
				ft.dwHighDateTime = largeTime.HighPart;
				FileTimeToSystemTime(&ft, &systime);
			}
		}
	}

	*year = systime.wYear;
	*month = systime.wMonth;
	*day = systime.wDay;
	*hour = systime.wHour;
	*minute = systime.wMinute;
	*second = systime.wSecond;
}

void SetYearlyRecurrence(LPSRow prow, CEventMessage* newEvent, RECURRENCE_YEARLY* recurrence)
{
	newEvent->SetType(EVENT_ANNUALLY);
	newEvent->SetEveryNth(1);
}

void SetMonthlyRecurrence(LPSRow prow, CEventMessage* newEvent, RECURRENCE_MONTHLY* recurrence)
{
	newEvent->SetType(EVENT_MONTHLY);
	newEvent->SetEveryNth(recurrence->interval);
}

void SetWeeklyRecurrence(LPSRow prow, CEventMessage* newEvent, RECURRENCE_WEEKLY* recurrence)
{
	newEvent->SetType(EVENT_WEEKLY);
	newEvent->SetEveryNth(recurrence->interval);

	// Create new events according to the mask
	if(prow->lpProps[E_MESSAGE_START_DATE].ulPropTag == messageProps.aulPropTag[E_MESSAGE_START_DATE])
	{
		FILETIME        ft;
		SYSTEMTIME      systime;
		FileTimeToLocalFileTime(&prow->lpProps[E_MESSAGE_START_DATE].Value.ft, &ft);
		FileTimeToSystemTime(&ft, &systime);

		// Get the start date
		int day, month, year;
		int date = newEvent->GetDate();
		CEventMessage::ValueToDate(date, &day, &month, &year);

		for (int i = systime.wDayOfWeek + 1; i < systime.wDayOfWeek + 6; i++)
		{
			day++;	// Go the next day

			// Calculate new start date
			int daysInMonth = CItem::GetDaysInMonth(year, month);
			if (day > daysInMonth)
			{
				month++;
				if (month == 13)
				{
					month = 1;
					year++;
				}
				day = 1;
			}

			if ((1 << (i % 7)) & recurrence->bitmask)
			{
				CEventMessage* newWeeklyEvent = new CEventMessage(false);      // Non-permanent event
				int id = newWeeklyEvent->GetID();
				*newWeeklyEvent = *newEvent;
				newWeeklyEvent->SetID(id);

				newWeeklyEvent->SetDate(CEventMessage::DateToValue(day, month, year));

				if (newWeeklyEvent->GetDate() <= newWeeklyEvent->GetValidUntil())
				{
					g_OutlookEvents.push_back(newWeeklyEvent);
				}
				else
				{
					delete newWeeklyEvent;
					break;
				}
			}
		}
	}
}

void SetDailyRecurrence(LPSRow prow, CEventMessage* newEvent, RECURRENCE_DAILY* recurrence)
{
	if (recurrence->type2 == 1)
	{
		// Every weekday occurring appointments are actually weekly appointments
		SetWeeklyRecurrence(prow, newEvent, (RECURRENCE_WEEKLY*)recurrence);
	}
	else
	{
		newEvent->SetType(EVENT_DAILY);
		newEvent->SetEveryNth(recurrence->interval / 24 / 60);
	}
}

void SetRecurrencePattern(LPSRow prow, CEventMessage* newEvent, LPSPropValue value)
{
	if(value->ulPropTag == messageProps.aulPropTag[E_MESSAGE_RECURRENCE])
	{
		if (value->Value.bin.cb >= 26)
		{
			RECURRENCE_HEADER* header = (RECURRENCE_HEADER*)value->Value.bin.lpb;
			switch(header->type)
			{
			case 0x0A: // daily
				SetDailyRecurrence(prow, newEvent, (RECURRENCE_DAILY*)value->Value.bin.lpb);
				break;

			case 0x0B: // weekly
				SetWeeklyRecurrence(prow, newEvent, (RECURRENCE_WEEKLY*)value->Value.bin.lpb);
				break;

			case 0x0C: // monthly
				SetMonthlyRecurrence(prow, newEvent, (RECURRENCE_MONTHLY*)value->Value.bin.lpb);
				break;

			case 0x0D: // yearly
				SetYearlyRecurrence(prow, newEvent, (RECURRENCE_YEARLY*)value->Value.bin.lpb);
				break;

			default:
				DebugLog("OUTLOOK: Unknown recurrence type (0x02x)", header->type);
				return;	// Unknown recurrency
			}
		}
		else
		{
			DebugLog("OUTLOOK: Unknow size in the recurrence structure (%i)", value->Value.bin.cb);
		}
	}
}

DEFINE_OLEGUID(PS_MESSAGE, 0x62002, 0, 0);

HRESULT GetProperties(IMAPIProp* lpMAPIProp)
{
	LPSPropTagArray lppPropTags = NULL;

	GUID mapi = PS_MESSAGE;

	const ULONG cNames = 7;

	LPMAPINAMEID id[cNames];
	MAPINAMEID startName, endName, validUntilName, locationName, labelName, recurrenceName, allDayName;
	
	startName.lpguid = &mapi;
	startName.ulKind = MNID_ID;
	startName.Kind.lID = 0x0000820D;

	endName.lpguid = &mapi;
	endName.ulKind = MNID_ID;
	endName.Kind.lID = 0x0000820E;

	validUntilName.lpguid = &mapi;
	validUntilName.ulKind = MNID_ID;
	validUntilName.Kind.lID = 0x00008236;

	locationName.lpguid = &mapi;
	locationName.ulKind = MNID_ID;
	locationName.Kind.lID = 0x00008208;

	recurrenceName.lpguid = &mapi;
	recurrenceName.ulKind = MNID_ID;
	recurrenceName.Kind.lID = 0x00008216;
	
	allDayName.lpguid = &mapi;
	allDayName.ulKind = MNID_ID;
	allDayName.Kind.lID = 0x00008215;

	labelName.lpguid = &mapi;
	labelName.ulKind = MNID_ID;
	labelName.Kind.lID = 0x00008214;

	id[0] = &startName;
	id[1] = &endName;
	id[2] = &validUntilName;
	id[3] = &locationName;
	id[4] = &recurrenceName;
	id[5] = &allDayName;
	id[6] = &labelName;

	HRESULT hr = lpMAPIProp->GetIDsFromNames(cNames, id, 0, &lppPropTags);
	if (SUCCEEDED(hr))
	{
		messageProps.aulPropTag[E_MESSAGE_START_DATE] = lppPropTags->aulPropTag[0] | PT_SYSTIME;
		messageProps.aulPropTag[E_MESSAGE_END_DATE] = lppPropTags->aulPropTag[1] | PT_SYSTIME;
		messageProps.aulPropTag[E_MESSAGE_VALID_UNTIL] = lppPropTags->aulPropTag[2] | PT_SYSTIME;
		messageProps.aulPropTag[E_MESSAGE_LOCATION] = lppPropTags->aulPropTag[3] | PT_STRING8;
		messageProps.aulPropTag[E_MESSAGE_RECURRENCE] = lppPropTags->aulPropTag[4] | PT_BINARY;
		messageProps.aulPropTag[E_MESSAGE_ALLDAY] = lppPropTags->aulPropTag[5] | PT_BOOLEAN;
		if (lppPropTags->aulPropTag[6] != PT_ERROR)		// Label is not defined in Outlook2000
		{
//			DebugLog("OUTLOOK: Message label 0x%08x", lppPropTags->aulPropTag[6]);
			messageProps.aulPropTag[E_MESSAGE_LABEL] = lppPropTags->aulPropTag[6] | PT_LONG;
		}
		else
		{
			DebugLog("OUTLOOK: Message label is not supported.");
			messageProps.cValues -= 1;
		}

		MAPIFreeBuffer(lppPropTags);
	}
	else
	{
		DebugLog("OUTLOOK: Unable to get property IDs.");
	}

	return hr;
}
 

void GetMessageData(LPMDB lpmdb, LPSPropValue value)
{
    HRESULT hr;
    LPSRowSet prows = NULL;
    ULONG ulType;
	LPMAPIFOLDER pfld = NULL;
    LPMAPITABLE ptable = NULL;
	static bool propsFound = false;
	LPMESSAGE pmsg = NULL;

    hr = lpmdb->OpenEntry(value->Value.bin.cb, 
		                  (LPENTRYID)value->Value.bin.lpb,
                          NULL, MAPI_DEFERRED_ERRORS,
                          &ulType, (LPUNKNOWN FAR *)&pfld);

	if (SUCCEEDED(hr)) 
	{
		hr = pfld->GetContentsTable(MAPI_DEFERRED_ERRORS, &ptable);
		if (SUCCEEDED(hr)) 
		{
			if (!propsFound)
			{
				// Get property IDs so that we get all the custom props that we need
				hr = HrQueryAllRows(ptable, (LPSPropTagArray)&messageEIDProps, NULL, NULL, 0, &prows);

				if (SUCCEEDED(hr))
				{
					if (prows->cRows > 0)
					{
						hr = pfld->OpenEntry(
						  prows[0].aRow[0].lpProps->Value.bin.cb,
						  (LPENTRYID)prows[0].aRow[0].lpProps->Value.bin.lpb,   
						  NULL,
						  MAPI_DEFERRED_ERRORS,
						  &ulType,
						  (LPUNKNOWN FAR *)&pmsg);

						if (SUCCEEDED(GetProperties(pmsg)) && pmsg)
						{
							propsFound = true;
						}
						FreeProws(prows);
						prows = NULL;
					}
					else
					{
						DebugLog("OUTLOOK: No messages found!");
					}
				}
				else
				{
					DebugLog("OUTLOOK: HrQueryAllRows failed: 0x%08x (%i)", hr, __LINE__);
				}
			}

			hr = HrQueryAllRows(ptable, (LPSPropTagArray)&messageProps, NULL, NULL, 0, &prows);
			if (SUCCEEDED(hr)) 
			{
				for (int i = 0; i < prows->cRows; i++)
				{
					bool allDay = false;
					bool valid = false;
					LPSRow prow = prows->aRow + i;

					int sDay = -1, sMonth = -1, sYear = -1;
					int sHour = -1, sMinute = -1, sSecond = -1;
					int eDay = -1, eMonth = -1, eYear = -1;
					int eHour = -1, eMinute = -1, eSecond = -1;
					int vDay = -1, vMonth = -1, vYear = -1;
					int vHour = -1, vMinute = -1, vSecond = -1;

					std::string message;

					if(prow->lpProps[E_MESSAGE_START_DATE].ulPropTag == messageProps.aulPropTag[E_MESSAGE_START_DATE])
					{
						FiletimeToDate(&prow->lpProps[E_MESSAGE_START_DATE].Value.ft, 
							&sYear, &sMonth, &sDay, &sHour, &sMinute, &sSecond);
						valid = true;
					}
					else
					{
						// Cannot handle events that do not have start date
						continue;
					}

					if(prow->lpProps[E_MESSAGE_END_DATE].ulPropTag == messageProps.aulPropTag[E_MESSAGE_END_DATE])
					{
						FiletimeToDate(&prow->lpProps[E_MESSAGE_END_DATE].Value.ft, 
							&eYear, &eMonth, &eDay, &eHour, &eMinute, &eSecond);
					}

					if(prow->lpProps[E_MESSAGE_VALID_UNTIL].ulPropTag == messageProps.aulPropTag[E_MESSAGE_VALID_UNTIL])
					{
						FILETIME time = prow->lpProps[E_MESSAGE_VALID_UNTIL].Value.ft;

						FiletimeToDate(&time, &vYear, &vMonth, &vDay, &vHour, &vMinute, &vSecond);

						if (vHour == 0 && vMinute == 0 && vSecond == 0)
						{
							// Go to previous day
							ULARGE_INTEGER timeNum;
							timeNum.HighPart = time.dwHighDateTime;
							timeNum.LowPart = time.dwLowDateTime;
							timeNum.QuadPart--;
							time.dwHighDateTime = timeNum.HighPart;
							time.dwLowDateTime = timeNum.LowPart;
	
							FiletimeToDate(&time, &vYear, &vMonth, &vDay, &vHour, &vMinute, &vSecond);
						}
					}

					if(prow->lpProps[E_MESSAGE_ALLDAY].ulPropTag == messageProps.aulPropTag[E_MESSAGE_ALLDAY])
					{
						allDay = prow->lpProps[E_MESSAGE_ALLDAY].Value.b ? true : false;
					}

					if(prow->lpProps[E_MESSAGE_SUBJECT].ulPropTag == PR_SUBJECT)
					{
						// Add the start and end time if they are different
						// but only if they are on the same day
						if (sDay == eDay && sMonth == eMonth && sYear == eYear)
						{
							if (sHour != -1 && eHour != -1 && (sHour != eHour || sMinute != eMinute))
							{
								TCHAR buffer[256];
								GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, buffer, 256);
								if (buffer[0] == '0')
								{
									// AM / PM 12-hour format.
									const char* sAMPM = sHour >= 12 ? "PM" : "AM";
									const char* eAMPM = eHour >= 12 ? "PM" : "AM";

									sHour = sHour % 12;
									if (sHour == 0) sHour = 12;
									eHour = eHour % 12;
									if (eHour == 0) eHour = 12;

									sprintf(buffer, "%i:%02i %s - %i:%02i %s ", sHour, sMinute, sAMPM, eHour, eMinute, eAMPM);
								}
								else
								{
									// 24-hour format
									sprintf(buffer, "%i:%02i - %i:%02i ", sHour, sMinute, eHour, eMinute);
								}
									
								message += buffer;
							} 
							else if (!allDay)
							{
								TCHAR buffer[256];
								GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, buffer, 256);
								if (buffer[0] == '0')
								{
									// AM / PM 12-hour format.
									const char* sAMPM = sHour >= 12 ? "PM" : "AM";

									sHour = sHour % 12;
									if (sHour == 0) sHour = 12;

									sprintf(buffer, "%i:%02i %s ", sHour, sMinute, sAMPM);
								}
								else
								{
									// 24-hour format
									sprintf(buffer, "%i:%02i ", sHour, sMinute);
								}
									
								message += buffer;
							}
						}
						message += prow->lpProps[E_MESSAGE_SUBJECT].Value.lpszA;

						// Add location if there is one
						if(prow->lpProps[E_MESSAGE_LOCATION].ulPropTag == messageProps.aulPropTag[E_MESSAGE_LOCATION])
						{
							if (strlen(prow->lpProps[E_MESSAGE_LOCATION].Value.lpszA) > 0)
							{
								message += " (";
								message += prow->lpProps[E_MESSAGE_LOCATION].Value.lpszA;
								message += ")";
							}
						}
					}
            
					// PR_BODY needs some special processing...
					// The table will only return a portion of the PR_BODY...if you want it all, we should
					// open the message and retrieve the property. GetProps (which HrGetOneProp calls
					// underneath) will do for most messages. For some larger messages, we would need to 
					// trap for MAPI_E_NOT_ENOUGH_MEMORY and call OpenProperty to get a stream on the body.
					
					if (MAPI_E_NOT_FOUND != prow->lpProps[E_MESSAGE_BODY].Value.l)
					{
						LPSPropValue lpProp = NULL;

						hr = lpmdb->OpenEntry(
							prow->lpProps[E_MESSAGE_EID].Value.bin.cb,
							(LPENTRYID)prow->lpProps[E_MESSAGE_EID].Value.bin.lpb,
							NULL,
							MAPI_BEST_ACCESS,
							&ulType,
							(LPUNKNOWN *) &pmsg);

						if (SUCCEEDED(hr))
						{
							hr = HrGetOneProp(pmsg, PR_BODY, &lpProp);
							if (SUCCEEDED(hr))
							{
								message += "\n";
								message += lpProp->Value.lpszA;

								MAPIFreeBuffer(lpProp);
							}
							else
							{
								DebugLog("OUTLOOK: HrGetOneProp failed: 0x%08x (%i)", hr, __LINE__);
							}
							UlRelease(pmsg);
						}
						else
						{
							DebugLog("OUTLOOK: OpenEntry failed: 0x%08x (%i)", hr, __LINE__);
						}
					}

					std::string profile = "Outlook";
					if (CConfig::Instance().GetOutlookLabels())
					{
						if(prow->lpProps[E_MESSAGE_LABEL].ulPropTag == messageProps.aulPropTag[E_MESSAGE_LABEL])
						{
							switch(prow->lpProps[E_MESSAGE_LABEL].Value.l) 
							{
							case 0:
								profile = "None";
								break;
							case 1:
								profile = "Important";
								break;
							case 2:
								profile = "Business";
								break;
							case 3:
								profile = "Personal";
								break;
							case 4:
								profile = "Vacation";
								break;
							case 5:
								profile = "Must attend";
								break;
							case 6:
								profile = "Travel Required";
								break;
							case 7:
								profile = "Needs Preparation";
								break;
							case 8:
								profile = "Birthday";
								break;
							case 9:
								profile = "Anniversary";
								break;
							case 10:
								profile = "Phone Call";
								break;
							}
						}
					}

					if (valid)
					{
						// Trim the message
						message.erase(message.find_last_not_of(" \n\r") + 1);

						CEventMessage* newEvent = new CEventMessage(false);

						newEvent->SetMessage(message);
						newEvent->SetDate(CEventMessage::DateToValue(sDay, sMonth, sYear));
						newEvent->SetProfile(profile);
						newEvent->SetValidUntil(CEventMessage::DateToValue(vDay, vMonth, vYear));

						// Recurrence pattern must be set last, since it can duplicate events
						SetRecurrencePattern(prow, newEvent, &prow->lpProps[E_MESSAGE_RECURRENCE]);

						if (newEvent->GetType() == EVENT_SINGLE && (sDay != eDay || sMonth != eMonth || sYear != eYear))
						{
							// No recurrency, but end time differs, so make it daily
							newEvent->SetType(EVENT_DAILY);
						}

						g_OutlookEvents.push_back(newEvent);
					}
				}
				FreeProws(prows);
			}
			else
			{
				DebugLog("OUTLOOK: HrQueryAllRows failed: 0x%08x (%i)", hr, __LINE__);
			}
			UlRelease(ptable);
		}
		else
		{
			DebugLog("OUTLOOK: GetContentsTable failed: 0x%08x (%i)", hr, __LINE__);
		}
		UlRelease(pfld);
	}
	else
	{
		DebugLog("OUTLOOK: OpenEntry failed: 0x%08x (%i)", hr, __LINE__);
	}
}

void ScanMessageFolders(LPMDB lpmdb)
{
    HRESULT hr;

    LPSPropValue pvalIPM = NULL;
	LPMAPIFOLDER pfld = NULL;
    LPMAPITABLE ptblHier = NULL;
    LPSRowSet pRowSet = NULL;
    ULONG ulObjType;

    hr = HrGetOneProp(lpmdb, PR_IPM_SUBTREE_ENTRYID, &pvalIPM);
	if (SUCCEEDED(hr)) 
	{
		hr = lpmdb->OpenEntry(pvalIPM->Value.bin.cb,
					(LPENTRYID)pvalIPM->Value.bin.lpb,
					NULL, MAPI_DEFERRED_ERRORS,
					 &ulObjType, (LPUNKNOWN *)&pfld);

		if (SUCCEEDED(hr)) 
		{
			hr = pfld->GetHierarchyTable(MAPI_DEFERRED_ERRORS, &ptblHier);
			
			if (SUCCEEDED(hr)) 
			{
				hr = HrQueryAllRows(ptblHier, (LPSPropTagArray)&datastoreProps, NULL, NULL, 0, &pRowSet);

				if (SUCCEEDED(hr)) 
				{
					for	(int i = 0; i < pRowSet->cRows; i++)
					{
//						DebugLog("Entry: %s", pRowSet->aRow[i].lpProps[E_DATASTORE_NAME].Value.lpszA);

						// Check only the appointments
						if (pRowSet->aRow[i].lpProps[E_DATASTORE_CONTAINER].ulPropTag == PR_CONTAINER_CLASS)
						{
							if (strcmp(pRowSet->aRow[i].lpProps[E_DATASTORE_CONTAINER].Value.lpszA, "IPF.Appointment") == 0)
							{
								GetMessageData(lpmdb, &pRowSet->aRow[i].lpProps[E_DATASTORE_EID]);
							}
						}
					}
				}
				else
				{
					DebugLog("OUTLOOK: HrQueryAllRows failed: 0x%08x (%i)", hr, __LINE__);
				}
			}
			else
			{
				DebugLog("OUTLOOK: GetHierarchyTable failed: 0x%08x (%i)", hr, __LINE__);
			}
	
			UlRelease(pfld);
		}
		else
		{
			DebugLog("OUTLOOK: OpenEntry failed: 0x%08x (%i)", hr, __LINE__);
		}

		MAPIFreeBuffer(pvalIPM);
	}
	else
	{
		DebugLog("OUTLOOK: HrGetOneProp failed: 0x%08x (%i)", hr, __LINE__);
	}
}

bool InitializeOutlook(HWND hWnd)
{
	SHRegSetUSValue("SOFTWARE\\Microsoft\\Windows Messaging Subsystem\\MSMapiApps", "Rainlendar.exe", REG_SZ, "Microsoft Outlook", 17, SHREGSET_HKLM);

	return TRUE;
}

void FinalizeOutlook()
{
	std::list<CEventMessage*>::iterator iter = g_OutlookEvents.begin();
	for	( ; iter != g_OutlookEvents.end(); iter++)
	{
		delete (*iter);
	}

	if (g_Session)
	{
		// Logout from MAPI
		MAPIUninitialize();
		g_Session = NULL;
	}
}

bool GetOutlookEvents(CEventManager* eventManager)
{
    if (!eventManager || !CConfig::Instance().GetShowOutlookAppointments())
    {
        return false;
    }

	std::list<CEventMessage*>::iterator iter = g_OutlookEvents.begin();
	for ( ; iter != g_OutlookEvents.end(); iter++)
	{
		if (*iter)
		{
			eventManager->AddEvent(**iter);
		}
	}

	return true;
}

bool SyncWithOutlook(HWND hWnd)
{
    HRESULT hr;
    bool ret = false;
	
    if (!CConfig::Instance().GetShowOutlookAppointments())
    {
        return ret;
    }

	if (g_Session == NULL)
	{
		hr = MAPIInitialize(NULL);

		if (SUCCEEDED(hr)) 
		{
			LPCTSTR profile = CConfig::Instance().GetOutlookProfile().c_str();
			LPCTSTR password = NULL;		// Make this configurable?

			// Create a MAPI session
			hr = MAPILogonEx((ULONG)hWnd, (char*)profile, (char*)password,
				MAPI_EXTENDED | MAPI_EXPLICIT_PROFILE | MAPI_LOGON_UI |	MAPI_NEW_SESSION | MAPI_NO_MAIL, &g_Session);
			
			if (SUCCEEDED(hr))
			{
				DebugLog("OUTLOOK: MAPI initialized succesfully");
				ret = true;
			}
			else
			{
				DebugLog("OUTLOOK: MAPILogonEx failed: 0x%08x", hr);
				MAPIUninitialize();
				g_Session = NULL;
			}
		}
		else
		{
			DebugLog("OUTLOOK: MAPIInitialize failed: 0x%08x", hr);
		}
	}
	
	if (g_Session)
	{
		std::list<CEventMessage*>::iterator iter = g_OutlookEvents.begin();
		for	( ; iter != g_OutlookEvents.end(); iter++)
		{
			delete (*iter);
		}
		g_OutlookEvents.clear();	// Delete old Outlook events

		LPMAPITABLE ptable = NULL;
		hr = g_Session->GetMsgStoresTable(0, &ptable);
		
		if (SUCCEEDED(hr))
		{
			LPSRowSet prows = NULL;

			hr = HrQueryAllRows(ptable, (LPSPropTagArray) &rootProps, NULL, NULL, 0, &prows);

			for	(int i = 0; i < prows->cRows; i++)
			{
				LPMDB lpmdb = NULL;

				if (prows->aRow[i].lpProps[E_ROOT_DEFAULT].Value.b == TRUE)		// Check only the default store
				{
					hr = g_Session->OpenMsgStore((ULONG)hWnd,
						prows->aRow[i].lpProps[E_ROOT_EID].Value.bin.cb,			// [1] == PR_ENTRYID
						(LPENTRYID)prows->aRow[i].lpProps[E_ROOT_EID].Value.bin.lpb,
						NULL, MAPI_DEFERRED_ERRORS, &lpmdb);

					if (SUCCEEDED(hr))
					{
						// DEBUG
						LPSPropValue pvalProp = NULL;
						hr = HrGetOneProp((LPMAPIPROP)lpmdb, PR_DISPLAY_NAME, &pvalProp);
						if (SUCCEEDED(hr))
						{
							DebugLog("MsgStore: %s", pvalProp->Value.lpszA);
							MAPIFreeBuffer(pvalProp);
						}
						// ~DEBUG

						ScanMessageFolders(lpmdb);

						UlRelease(lpmdb);
					}
				}
			}

			FreeProws(prows);
			UlRelease(ptable);
		}
		else
		{
			DebugLog("OUTLOOK: GetMsgStoresTable failed: 0x%08x", hr);
		}

	    if (!CConfig::Instance().GetOutlookKeepAlive())
		{
			// Logout from MAPI
			MAPIUninitialize();
			g_Session = NULL;
		}
	}

	return ret;
}

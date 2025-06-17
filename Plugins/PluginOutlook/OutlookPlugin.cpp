/*
  Copyright (C) 2004 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Plugins/PluginOutlook/OutlookPlugin.cpp,v 1.4 2005/10/14 17:05:29 rainy Exp $

  $Log: OutlookPlugin.cpp,v $
  Revision 1.4  2005/10/14 17:05:29  rainy
  no message

  Revision 1.3  2005/09/08 16:13:51  rainy
  no message

  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.4  2005/03/25 13:59:47  rainy
  no message

  Revision 1.3  2005/03/01 18:33:11  rainy
  Added possibility to select which message stores are imported.
  No direct linking to mapi32.dll anymore.

  Revision 1.2  2004/12/05 18:14:35  rainy
  Made the code more robust

  Revision 1.1  2004/11/06 13:32:07  rainy
  Initial version

*/

#pragma warning(disable: 4786)

#define _WIN32_WINNT 0x0400

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "../../Library/RainlendarAPI.h"
#include "resource.h"
#include "OutlookPlugin.h"
#include "AdviseSink.h"
#include <objbase.h>
#include <string>
#include <vector>
#include <tchar.h>
#include <assert.h>
#include <shlwapi.h>

#include <ole2.h>
#include <initguid.h>
#include <mapix.h>
#include <mapiutil.h>
#include <sqltypes.h>
#include <mapidefs.h>

#ifdef UNICODE
#define tstring wstring
#else
#define tstring string 
#endif

std::tstring g_OutlookProfile;
bool g_KeepAlive = false;
bool g_UseLabels = false;
bool g_NoMessageBody = false;
bool g_NoDoneTasks = false;
bool g_EnableAll = false;
bool g_WatchChanges = false;
HINSTANCE g_Instance = NULL;
UINT g_ID = 0;
LPMAPISESSION g_Session = NULL;
HINSTANCE g_hMapiLib = NULL;
static std::string g_ConfigFile;

std::vector<MessageStoreName> g_Stores;

CAdviseSink* g_AdviseSink = NULL;
std::vector<LPMDB> g_AdvisedStores;
std::vector<ULONG> g_AdvisedConnections;

std::string IDToString(LPENTRYID id, ULONG size);
int ShowException(int exNum, LPEXCEPTION_POINTERS exInfo);
void SyncWithOutlook();
void ReadSettings(LPCTSTR configFile);
void WriteSettings(LPCTSTR configFile);

bool InitMapi();
void FreeMapi();

void LogoutMAPI();
bool LoginMAPI();

LPCTSTR Plugin_GetName()
{
	return _T("Outlook Plugin");
}

LPCTSTR Plugin_GetAuthor()
{
	return _T("Rainy (rainy@iki.fi)");
}

UINT Plugin_GetVersion()
{
	return 1008;	// Rainlendar 0.22 RC3
}

void Plugin_Initialize(HINSTANCE instance, LPCTSTR configFile, UINT id)
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	SHRegSetUSValue("SOFTWARE\\Microsoft\\Windows Messaging Subsystem\\MSMapiApps", "Rainlendar.exe", REG_SZ, "Microsoft Outlook", 17, SHREGSET_HKLM);

	g_Instance = instance;
	g_ID = id;
	g_ConfigFile = configFile;

	InitMapi();
}

void Plugin_Finalize()
{
	if (g_Session)
	{
		LogoutMAPI();
		CoUninitialize();
	}

	FreeMapi();
}

void Plugin_Enable()
{
	// Read the settings when the plugin is enabled or the Rainlendar is refreshed
	ReadSettings(g_ConfigFile.c_str());
}

void Plugin_Disable()
{
	if (g_Session)
	{
		LogoutMAPI();
	}
}

void Plugin_ReadItems()
{
	SyncWithOutlook();
}

void Plugin_ItemNotification(GUID** guid)
{
	// Ignore all notifications, since this is a read-only plugin
}

bool Plugin_ShowConfig(HWND hwndParent, LPCTSTR configFile)
{
	ReadSettings(configFile);

	if (DialogBox(g_Instance, MAKEINTRESOURCE(IDD_OUTLOOK_DIALOG), hwndParent, (DLGPROC)SetupProc) == IDOK) 
	{
		WriteSettings(configFile);

		if (g_Session)
		{
			// Logout from MAPI so that the new settings are set
			LogoutMAPI();
		}

		// Return true if something has changed and all events need to be reloaded.
		return true;
	}
	return false;
}

void ReadSettings(LPCTSTR configFile)
{
	TCHAR buffer[MAX_PATH];
	TCHAR buffer2[4096];
	TCHAR tmpSz2[256];

	// Read settings from configFile
	if(GetPrivateProfileString(PLUGIN_NAME, _T("OutlookProfile"), "", buffer, MAX_PATH, configFile) > 0) 
	{
		g_OutlookProfile = buffer;
	}
	else
	{
		g_OutlookProfile = "";
	}

	g_KeepAlive = GetPrivateProfileInt(PLUGIN_NAME, _T("OutlookKeepAlive"), 1, configFile) == 1;
	g_UseLabels = GetPrivateProfileInt(PLUGIN_NAME, _T("OutlookLabels"), 0, configFile) == 1;
	g_NoMessageBody = GetPrivateProfileInt(PLUGIN_NAME, _T("NoMessageBody"), 0, configFile) == 1;
	g_NoDoneTasks = GetPrivateProfileInt(PLUGIN_NAME, _T("NoDoneTasks"), 0, configFile) == 1;
	g_WatchChanges = GetPrivateProfileInt(PLUGIN_NAME, _T("WatchChanges"), 0, configFile) == 1;

	int count = GetPrivateProfileInt(PLUGIN_NAME, _T("MessageStoreCount"), -1, configFile);

	if (count == -1)
	{
		// Value not set -> enable all
		g_EnableAll = true;
	}
	else
	{
		for (int i = 0; i < count; i++)
		{
			for (int j = 0; j < 2; j++) 
			{
				char* pos = NULL;
				char* pos2 = NULL;

				sprintf(tmpSz2, "MessageStore%i", i + 1);
				if(GetPrivateProfileString(PLUGIN_NAME, tmpSz2, "", buffer, MAX_PATH, configFile) > 0) 
				{
					pos = strchr(buffer, '\\');
					if (pos)
					{
						*pos = 0;
					}
				}

				sprintf(tmpSz2, "MessageStoreID%i", i + 1);
				if(GetPrivateProfileString(PLUGIN_NAME, tmpSz2, "", buffer2, 4096, configFile) > 0) 
				{
					pos2 = strchr(buffer2, '\\');
					if (pos2)
					{
						*pos2 = 0;
					}
				}

				if (pos && pos2) 
				{
					bool found = false;

					// Find the correct store
					for (int j = 0; j < g_Stores.size(); j++)
					{
						if (strcmp(buffer, g_Stores[j].storeID.c_str()) == 0)
						{
							// Found
							found = true;
							g_Stores[j].folders.push_back(pos + 1);
							g_Stores[j].folderIDs.push_back(pos2 + 1);
						}
					}

					if (!found)
					{
						// Not found -> Create new
						MessageStoreName store;
						store.name = buffer;
						store.storeID = buffer2;
						store.folders.push_back(pos + 1);
						store.folderIDs.push_back(pos2 + 1);
						g_Stores.push_back(store);
					}
				}
			}
		}
	}
}

void WriteSettings(LPCTSTR configFile)
{
	TCHAR tmpSz[4096];
	TCHAR tmpSz2[4096];

	WritePrivateProfileString(PLUGIN_NAME, _T("OutlookProfile"), g_OutlookProfile.c_str(), configFile);

	sprintf(tmpSz, "%i", g_KeepAlive);
	WritePrivateProfileString(PLUGIN_NAME, _T("OutlookKeepAlive"), tmpSz, configFile);

	sprintf(tmpSz, "%i", g_UseLabels);
	WritePrivateProfileString(PLUGIN_NAME, _T("OutlookLabels"), tmpSz, configFile);

	sprintf(tmpSz, "%i", g_NoMessageBody);
	WritePrivateProfileString(PLUGIN_NAME, _T("NoMessageBody"), tmpSz, configFile);
	
	sprintf(tmpSz, "%i", g_NoDoneTasks);
	WritePrivateProfileString(PLUGIN_NAME, _T("NoDoneTasks"), tmpSz, configFile);

	sprintf(tmpSz, "%i", g_WatchChanges);
	WritePrivateProfileString(PLUGIN_NAME, _T("WatchChanges"), tmpSz, configFile);

	int count = 0;
	for (int i = 0; i < g_Stores.size(); i++)
	{
		for (int j = 0; j < g_Stores[i].folders.size(); j++)
		{
			count++;
			sprintf(tmpSz2, "MessageStore%i", count);
			sprintf(tmpSz, "%s\\%s", g_Stores[i].name.c_str(), g_Stores[i].folders[j].c_str());
			WritePrivateProfileString(PLUGIN_NAME, tmpSz2, tmpSz, configFile);

			sprintf(tmpSz2, "MessageStoreID%i", count);
			sprintf(tmpSz, "%s\\%s", g_Stores[i].storeID.c_str(), g_Stores[i].folderIDs[j].c_str());
			WritePrivateProfileString(PLUGIN_NAME, tmpSz2, tmpSz, configFile);
		}
	}

	sprintf(tmpSz, "%i", count);
	WritePrivateProfileString(PLUGIN_NAME, _T("MessageStoreCount"), tmpSz, configFile);
}


// http://www.geocities.com/cainrandom/dev/MAPIRecurrence.html
// http://www.cdolive.com/cdo10.htm


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

enum { E_MESSAGE_EID = 0, E_MESSAGE_SUBJECT, E_MESSAGE_BODY, E_MESSAGE_START_DATE, E_MESSAGE_END_DATE, E_MESSAGE_VALID_UNTIL, E_MESSAGE_LOCATION, E_MESSAGE_RECURRENCE, E_MESSAGE_ALLDAY, E_MESSAGE_REMINDER, E_MESSAGE_REMINDERMINS, E_MESSAGE_LABEL, E_MESSAGE_PROPS};
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
		0,		// PR_APPOINTMENT_REMINDERSET
		0,		// PR_APPOINTMENT_REMIDERBEFOREMINS
		0		// PR_APPOINTMENT_LABEL,		(label must be that last)
    }
};

enum { E_TODO_EID = 0, E_TODO_SUBJECT, E_TODO_STATUS, E_TODO_ORDINAL, E_TODO_PROPS };
SizedSPropTagArray(E_TODO_PROPS, todoProps) =
{
    E_TODO_PROPS,
    {
		PR_ENTRYID,
        PR_SUBJECT,
		0,		// Status,
		0,		// Ordinal
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
	
	UCHAR terminationType;	// Type of pattern termination: 0x21 for terminating on a given date, 0x22 for terminating after a given number of recurrences, or 0x23 for never terminating (recurring infinitely)
	UCHAR unknown4;		// 0x20
	UCHAR unknown5;		// 0x00
	UCHAR unknown6;		// 0x00
	ULONG occurences;	// Number of occurrences in pattern: 0 for infinite recurrence, otherwise supply the value, even if it terminates on a given date, not after a given number
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

	UCHAR terminationType;	// Type of pattern termination: 0x21 for terminating on a given date, 0x22 for terminating after a given number of recurrences, or 0x23 for never terminating (recurring infinitely)
	UCHAR unknown4;		// 0x20
	UCHAR unknown5;		// 0x00
	UCHAR unknown6;		// 0x00
	ULONG occurences;	// Number of occurrences in pattern: 0 for infinite recurrence, otherwise supply the value, even if it terminates on a given date, not after a given number
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

	UCHAR terminationType;	// Type of pattern termination: 0x21 for terminating on a given date, 0x22 for terminating after a given number of recurrences, or 0x23 for never terminating (recurring infinitely)
	UCHAR unknown4;		// 0x20
	UCHAR unknown5;		// 0x00
	UCHAR unknown6;		// 0x00
	ULONG occurences;	// Number of occurrences in pattern: 0 for infinite recurrence, otherwise supply the value, even if it terminates on a given date, not after a given number
};

struct RECURRENCE_MONTHLY2
{
	ULONG unknown1;		// 0x04, 0x30, 0x04, 0x30
	UCHAR type;
	UCHAR unknown2;		// 0x20
	ULONG type2;		// Recurrence type
	// Monthly stuff starts here
	ULONG separation;
	ULONG interval;
	ULONG unknown3;		// 0x00
	ULONG y;
	ULONG x;

	UCHAR terminationType;	// Type of pattern termination: 0x21 for terminating on a given date, 0x22 for terminating after a given number of recurrences, or 0x23 for never terminating (recurring infinitely)
	UCHAR unknown4;		// 0x20
	UCHAR unknown5;		// 0x00
	UCHAR unknown6;		// 0x00
	ULONG occurences;	// Number of occurrences in pattern: 0 for infinite recurrence, otherwise supply the value, even if it terminates on a given date, not after a given number
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

	UCHAR terminationType;	// Type of pattern termination: 0x21 for terminating on a given date, 0x22 for terminating after a given number of recurrences, or 0x23 for never terminating (recurring infinitely)
	UCHAR unknown4;		// 0x20
	UCHAR unknown5;		// 0x00
	UCHAR unknown6;		// 0x00
	ULONG occurences;	// Number of occurrences in pattern: 0 for infinite recurrence, otherwise supply the value, even if it terminates on a given date, not after a given number
};


void ConvertFileTime(FILETIME* pft)
{
	SYSTEMTIME utcSysTime;
	SYSTEMTIME localSysTime;
	FileTimeToSystemTime(pft, &utcSysTime);

	if (SystemTimeToTzSpecificLocalTime(NULL, &utcSysTime, &localSysTime))
	{
		SystemTimeToFileTime(&localSysTime, pft);
	}
}


void SetMonthlyRecurrence2(LPSRow prow, RainlendarRecurrency* newRecurrency, RECURRENCE_MONTHLY2* recurrence)
{
	if (recurrence->y == 0x7f)
	{
		newRecurrency->type = RECURRENCY_TYPE_MONTHLY_LAST_DAY;		// Last day of the month
	}
	else if (recurrence->y == 0x3f && recurrence->x == 5)
	{
		newRecurrency->type = RECURRENCY_TYPE_MONTHLY_LAST_WEEKDAY;	// Last weekday of the month
	}
	else if (recurrence->y == 0x3f && recurrence->x == 1)
	{
		newRecurrency->type = RECURRENCY_TYPE_MONTHLY_FIRST_WEEKDAY;	// First weekday of the month
	}
	else if (recurrence->x == 5)
	{
		// last weekday of the month
		newRecurrency->type = RECURRENCY_TYPE_MONTHLY_LASTWEEK_WEEKDAY;
	}
	else
	{
		newRecurrency->type = RECURRENCY_TYPE_MONTHLY_NTH_WEEKDAY;
	}
	newRecurrency->frequency = recurrence->interval;

	if (recurrence->unknown4 == 0x20)
	{
		if (recurrence->terminationType == 0x23)		// Forever
		{
			newRecurrency->repeatType = RECURRENCY_REPEAT_FOREVER;
		}
		else if (recurrence->terminationType == 0x22 || recurrence->terminationType == 0x21)		// Count (use count also with UNTIL)
		{
			newRecurrency->repeatType = RECURRENCY_REPEAT_COUNT;
			newRecurrency->count = recurrence->occurences;
		}
	}
}

void SetMonthlyRecurrence(LPSRow prow, RainlendarRecurrency* newRecurrency, RECURRENCE_MONTHLY* recurrence)
{
	newRecurrency->type = RECURRENCY_TYPE_MONTHLY_NTH_DAY;
	newRecurrency->frequency = recurrence->interval;

	if (recurrence->unknown4 == 0x20)
	{
		if (recurrence->terminationType == 0x23)		// Forever
		{
			newRecurrency->repeatType = RECURRENCY_REPEAT_FOREVER;
		}
		else if (recurrence->terminationType == 0x22 || recurrence->terminationType == 0x21)		// Count (use count also with UNTIL)
		{
			newRecurrency->repeatType = RECURRENCY_REPEAT_COUNT;
			newRecurrency->count = recurrence->occurences;
		}
	}
	else
	{
		// Use monthly2
		SetMonthlyRecurrence2(prow, newRecurrency, (RECURRENCE_MONTHLY2*)recurrence);
	}
}

void SetYearlyRecurrence(LPSRow prow, RainlendarRecurrency* newRecurrency, RECURRENCE_YEARLY* recurrence)
{
	newRecurrency->type = RECURRENCY_TYPE_YEARLY;
	newRecurrency->frequency = 1;

	if (recurrence->terminationType == 0xFF)	// Birthdays are identified like this
	{
		newRecurrency->repeatType = RECURRENCY_REPEAT_FOREVER;
	}
	else if (recurrence->unknown4 == 0x20)
	{
		if (recurrence->terminationType == 0x23)		// Forever
		{
			newRecurrency->repeatType = RECURRENCY_REPEAT_FOREVER;
		}
		else if (recurrence->terminationType == 0x22 || recurrence->terminationType == 0x21)		// Count (use count also with UNTIL)
		{
			newRecurrency->repeatType = RECURRENCY_REPEAT_COUNT;
			newRecurrency->count = recurrence->occurences;
		}
	}
	else
	{
		// Try it as monthly2
		SetMonthlyRecurrence2(prow, newRecurrency, (RECURRENCE_MONTHLY2*)recurrence);
	}
}

void SetWeeklyRecurrence(LPSRow prow, RainlendarRecurrency* newRecurrency, RECURRENCE_WEEKLY* recurrence)
{
	newRecurrency->type = RECURRENCY_TYPE_WEEKLY;
	newRecurrency->frequency = recurrence->interval;
	newRecurrency->days = recurrence->bitmask;

	if (recurrence->terminationType == 0x23)		// Forever
	{
		newRecurrency->repeatType = RECURRENCY_REPEAT_FOREVER;
	}
	else if (recurrence->terminationType == 0x22 || recurrence->terminationType == 0x21)		// Count (use count also with UNTIL)
	{
		newRecurrency->repeatType = RECURRENCY_REPEAT_COUNT;
		
		// The count depends on the days in Outlook
		int days = 0, numOfDays = 0;
		days = newRecurrency->days;
		while (days != 0)
		{
			if (days & 1)
			{
				numOfDays++;
			}
			days >>= 1;
		}

		newRecurrency->count = (recurrence->occurences + (numOfDays - 1)) / numOfDays;
	}
}

void SetDailyRecurrence(LPSRow prow, RainlendarRecurrency* newRecurrency, RECURRENCE_DAILY* recurrence)
{
	if (recurrence->type2 == 1)
	{
		// Every weekday occurring appointments are actually weekly appointments
		SetWeeklyRecurrence(prow, newRecurrency, (RECURRENCE_WEEKLY*)recurrence);
	}
	else
	{
		newRecurrency->type = RECURRENCY_TYPE_DAILY;
		newRecurrency->frequency = recurrence->interval / 24 / 60;

		if (recurrence->terminationType == 0x23)		// Forever
		{
			newRecurrency->repeatType = RECURRENCY_REPEAT_FOREVER;
		}
		else if (recurrence->terminationType == 0x22 || recurrence->terminationType == 0x21)		// Count (use count also with UNTIL)
		{
			newRecurrency->repeatType = RECURRENCY_REPEAT_COUNT;
			newRecurrency->count = recurrence->occurences;
		}
	}
}

void SetRecurrencePattern(LPSRow prow, RainlendarRecurrency* newRecurrency, LPSPropValue value)
{
	if(value->ulPropTag == messageProps.aulPropTag[E_MESSAGE_RECURRENCE])
	{
		if (value->Value.bin.cb >= 26)
		{
			RECURRENCE_HEADER* header = (RECURRENCE_HEADER*)value->Value.bin.lpb;
			switch(header->type)
			{
			case 0x0A: // daily
				SetDailyRecurrence(prow, newRecurrency, (RECURRENCE_DAILY*)value->Value.bin.lpb);
				break;

			case 0x0B: // weekly
				SetWeeklyRecurrence(prow, newRecurrency, (RECURRENCE_WEEKLY*)value->Value.bin.lpb);
				break;

			case 0x0C: // monthly
				SetMonthlyRecurrence(prow, newRecurrency, (RECURRENCE_MONTHLY*)value->Value.bin.lpb);
				break;

			case 0x0D: // yearly
				SetYearlyRecurrence(prow, newRecurrency, (RECURRENCE_YEARLY*)value->Value.bin.lpb);
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
DEFINE_OLEGUID(PS_MESSAGE2, 0x62008, 0, 0);

HRESULT GetMessageProperties(IMAPIProp* lpMAPIProp)
{
	LPSPropTagArray lppPropTags = NULL;

	GUID mapi = PS_MESSAGE;
	GUID mapi2 = PS_MESSAGE2;

	const ULONG cNames = 9;

	LPMAPINAMEID id[cNames];
	MAPINAMEID startName, endName, validUntilName, locationName, labelName, recurrenceName, allDayName, reminderName, reminderMinsName;
	
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

	reminderName.lpguid = &mapi2;
	reminderName.ulKind = MNID_ID;
	reminderName.Kind.lID = 0x00008503;

	reminderMinsName.lpguid = &mapi2;
	reminderMinsName.ulKind = MNID_ID;
	reminderMinsName.Kind.lID = 0x00008501;

	labelName.lpguid = &mapi;
	labelName.ulKind = MNID_ID;
	labelName.Kind.lID = 0x00008214;

	id[0] = &startName;
	id[1] = &endName;
	id[2] = &validUntilName;
	id[3] = &locationName;
	id[4] = &recurrenceName;
	id[5] = &allDayName;
	id[6] = &reminderName;
	id[7] = &reminderMinsName;
	id[8] = &labelName;

	HRESULT hr = lpMAPIProp->GetIDsFromNames(cNames, id, 0, &lppPropTags);
	if (SUCCEEDED(hr))
	{
		messageProps.aulPropTag[E_MESSAGE_START_DATE] = lppPropTags->aulPropTag[0] | PT_SYSTIME;
		messageProps.aulPropTag[E_MESSAGE_END_DATE] = lppPropTags->aulPropTag[1] | PT_SYSTIME;
		messageProps.aulPropTag[E_MESSAGE_VALID_UNTIL] = lppPropTags->aulPropTag[2] | PT_SYSTIME;
		messageProps.aulPropTag[E_MESSAGE_LOCATION] = lppPropTags->aulPropTag[3] | PT_STRING8;
		messageProps.aulPropTag[E_MESSAGE_RECURRENCE] = lppPropTags->aulPropTag[4] | PT_BINARY;
		messageProps.aulPropTag[E_MESSAGE_ALLDAY] = lppPropTags->aulPropTag[5] | PT_BOOLEAN;
		messageProps.aulPropTag[E_MESSAGE_REMINDER] = lppPropTags->aulPropTag[6] | PT_BOOLEAN;
		messageProps.aulPropTag[E_MESSAGE_REMINDERMINS] = lppPropTags->aulPropTag[7] | PT_LONG;
		if (lppPropTags->aulPropTag[8] != PT_ERROR)		// Label is not defined in Outlook2000
		{
//			DebugLog("OUTLOOK: Message label 0x%08x", lppPropTags->aulPropTag[8]);
			messageProps.aulPropTag[E_MESSAGE_LABEL] = lppPropTags->aulPropTag[8] | PT_LONG;
		}
		else
		{
//			DebugLog("OUTLOOK: Message label is not supported.");
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
 

void GetMessageData(LPMDB lpmdb, LPSPropValue value, const char* storeName)
{
    HRESULT hr;
    LPSRowSet prows = NULL;
    ULONG ulType;
	LPMAPIFOLDER pfld = NULL;
    LPMAPITABLE ptable = NULL;
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

					if (SUCCEEDED(hr))
					{
						GetMessageProperties(pmsg);
						UlRelease(pmsg);
					}
				}
				else
				{
					DebugLog("OUTLOOK: No messages found!");
				}

				FreeProws(prows);
				prows = NULL;

				hr = HrQueryAllRows(ptable, (LPSPropTagArray)&messageProps, NULL, NULL, 0, &prows);
				if (SUCCEEDED(hr)) 
				{
					int count = 0;

					for (int i = 0; i < prows->cRows; i++)
					{
						bool valid = false;

						RainlendarEvent* newEvent = NULL;
						RainlendarRecurrency* newRecurrency = NULL;

						newEvent = new RainlendarEvent;
						memset(newEvent, 0, sizeof(RainlendarEvent));
						newEvent->size = sizeof(RainlendarEvent);
						newEvent->readOnly = true;		// All Outlook events are readonly
						newEvent->type = RAINLENDAR_TYPE_EVENT;
						CoCreateGuid(&newEvent->guid);	// Create GUID just in case it's not in the file

						newRecurrency = new RainlendarRecurrency;
						memset(newRecurrency, 0, sizeof(RainlendarRecurrency));
						newRecurrency->size = sizeof(RainlendarRecurrency);
						newRecurrency->type = RECURRENCY_TYPE_SINGLE;
						newEvent->recurrency = newRecurrency;

						LPSRow prow = prows->aRow + i;

						if(prow->lpProps[E_MESSAGE_START_DATE].ulPropTag == messageProps.aulPropTag[E_MESSAGE_START_DATE])
						{
							FILETIME time = prow->lpProps[E_MESSAGE_START_DATE].Value.ft; 
							ConvertFileTime(&time);
							newEvent->startTime = time;

							valid = true;	// Appointment must have start date
						}

						if(prow->lpProps[E_MESSAGE_END_DATE].ulPropTag == messageProps.aulPropTag[E_MESSAGE_END_DATE])
						{
							FILETIME time = prow->lpProps[E_MESSAGE_END_DATE].Value.ft; 
							ConvertFileTime(&time);
							newEvent->endTime = time;
						}

						if(prow->lpProps[E_MESSAGE_ALLDAY].ulPropTag == messageProps.aulPropTag[E_MESSAGE_ALLDAY])
						{
							newEvent->allDayEvent = prow->lpProps[E_MESSAGE_ALLDAY].Value.b ? true : false;
						}

						if(prow->lpProps[E_MESSAGE_SUBJECT].ulPropTag == PR_SUBJECT)
						{
							std::string header;
							header = prow->lpProps[E_MESSAGE_SUBJECT].Value.lpszA;

							// Add location if there is one
							if(prow->lpProps[E_MESSAGE_LOCATION].ulPropTag == messageProps.aulPropTag[E_MESSAGE_LOCATION])
							{
								if (strlen(prow->lpProps[E_MESSAGE_LOCATION].Value.lpszA) > 0)
								{
									header += " (";
									header += prow->lpProps[E_MESSAGE_LOCATION].Value.lpszA;
									header += ")";
								}
							}
							newEvent->header = _tcsdup(header.c_str());
						}

						if (!g_NoMessageBody)
						{
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
										std::string message = lpProp->Value.lpszA;

										// Trim the message
										message.erase(message.find_last_not_of(" \n\r") + 1);
										newEvent->message = _tcsdup(message.c_str());

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
						}

						std::string profile = "Outlook";
						if (g_UseLabels)
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
						newEvent->profile = _tcsdup(profile.c_str());
						
						newEvent->alarm = -1;
						if(prow->lpProps[E_MESSAGE_REMINDER].ulPropTag == messageProps.aulPropTag[E_MESSAGE_REMINDER])
						{
							if (prow->lpProps[E_MESSAGE_REMINDER].Value.b) 
							{
								if(prow->lpProps[E_MESSAGE_REMINDERMINS].ulPropTag == messageProps.aulPropTag[E_MESSAGE_REMINDERMINS])
								{
									newEvent->alarm = prow->lpProps[E_MESSAGE_REMINDERMINS].Value.l;
								}
							}
						}

						if (valid)
						{
							SetRecurrencePattern(prow, newRecurrency, &prow->lpProps[E_MESSAGE_RECURRENCE]);

							Rainlendar_SetItem(newEvent, g_ID);
							count++;
						}
						Rainlendar_DeleteItem(newEvent);
					}
					FreeProws(prows);

					DebugLog("OUTLOOK: Read %i events from %s", count, storeName);
				}
				else
				{
					DebugLog("OUTLOOK: HrQueryAllRows failed: 0x%08x (%i)", hr, __LINE__);
				}
				UlRelease(ptable);
			}
			else
			{
				DebugLog("OUTLOOK: HrQueryAllRows failed: 0x%08x (%i)", hr, __LINE__);
			}
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

DEFINE_OLEGUID(PS_TODO, 0x62003, 0, 0);

HRESULT GetTodoProperties(IMAPIProp* lpMAPIProp)
{
	LPSPropTagArray lppPropTags = NULL;

	GUID mapi = PS_TODO;

	const ULONG cNames = 2;

	LPMAPINAMEID id[cNames];
	MAPINAMEID ordinalName, statusName;
	
	ordinalName.lpguid = &mapi;
	ordinalName.ulKind = MNID_ID;
	ordinalName.Kind.lID = 0x00008123;

	statusName.lpguid = &mapi;
	statusName.ulKind = MNID_ID;
	statusName.Kind.lID = 0x00008101;

	id[0] = &statusName;
	id[1] = &ordinalName;

	HRESULT hr = lpMAPIProp->GetIDsFromNames(cNames, id, 0, &lppPropTags);
	if (SUCCEEDED(hr))
	{
		todoProps.aulPropTag[E_TODO_STATUS] = lppPropTags->aulPropTag[0] | PT_LONG;
		todoProps.aulPropTag[E_TODO_ORDINAL] = lppPropTags->aulPropTag[1] | PT_LONG;

		MAPIFreeBuffer(lppPropTags);
	}
	else
	{
		DebugLog("OUTLOOK: Unable to get property IDs.");
	}

	return hr;
}

void GetTodoData(LPMDB lpmdb, LPSPropValue value, const char* storeName)
{
    HRESULT hr;
	LPMAPIFOLDER pfld = NULL;
    ULONG ulType;
	LPSPropValue pvalIPM = NULL;
    LPMAPITABLE ptable = NULL;
	LPSRowSet prows = NULL;
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

						if (SUCCEEDED(hr))
						{
							GetTodoProperties(pmsg);
							propsFound = true;
							UlRelease(pmsg);
						}
					}
					else
					{
						DebugLog("OUTLOOK: No tasks found!");
					}

					FreeProws(prows);
					prows = NULL;
				}
				else
				{
					DebugLog("OUTLOOK: HrQueryAllRows failed: 0x%08x (%i)", hr, __LINE__);
				}
			}

			if (propsFound)
			{
				hr = HrQueryAllRows(ptable, (LPSPropTagArray)&todoProps, NULL, NULL, 0, &prows);
				if (SUCCEEDED(hr)) 
				{
					int count = 0;

					for (int i = 0; i < prows->cRows; i++)
					{
						bool valid = false;

						RainlendarTodo* newTodo = new RainlendarTodo;

						memset(newTodo, 0, sizeof(RainlendarTodo));
						newTodo->size = sizeof(RainlendarTodo);
						newTodo->readOnly = true;		// All Outlook events are readonly
						newTodo->type = RAINLENDAR_TYPE_TODO;
						newTodo->todoType = TODO_TYPE_TODO;
						CoCreateGuid(&newTodo->guid);

						LPSRow prow = prows->aRow + i;

						if(prow->lpProps[E_TODO_SUBJECT].ulPropTag == PR_SUBJECT)
						{
							newTodo->message = _tcsdup(prow->lpProps[E_TODO_SUBJECT].Value.lpszA);
							valid = true;
						}

						if(prow->lpProps[E_TODO_STATUS].ulPropTag == todoProps.aulPropTag[E_TODO_STATUS])
						{
							LONG value = prow->lpProps[E_TODO_STATUS].Value.l; 
							if (value == 2)
							{
								newTodo->checked = true;

								if (g_NoDoneTasks)
								{
									valid = false;
								}
							}
						}

						if(prow->lpProps[E_TODO_ORDINAL].ulPropTag == todoProps.aulPropTag[E_TODO_ORDINAL])
						{
							LONG value = prow->lpProps[E_TODO_ORDINAL].Value.l; 
							newTodo->position = value;
						}

						if (valid)
						{
							newTodo->profile = _tcsdup("OutlookTask");
							Rainlendar_SetItem(newTodo, g_ID);
							count++;
						}
						Rainlendar_DeleteItem(newTodo);
					}
					FreeProws(prows);

					DebugLog("OUTLOOK: Read %i tasks from %s", count, storeName);
				}
				else
				{
					DebugLog("OUTLOOK: HrQueryAllRows failed: 0x%08x (%i)", hr, __LINE__);
				}
				UlRelease(ptable);
			}
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

void ScanMessageFolders(LPMDB lpmdb, MessageStoreName* store)
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
						bool found = false;
						if (store)
						{
							if (store->storeID.empty()) 
							{
								for (int j = 0; j < store->folders.size(); j++)
								{
									if (store->folders[j] == pRowSet->aRow[i].lpProps[E_DATASTORE_NAME].Value.lpszA)
									{
										found = true;
										break;
									}
								}
							}
							else
							{
								for (int j = 0; j < store->folderIDs.size(); j++)
								{
									if (store->folderIDs[j] == IDToString((LPENTRYID)pRowSet->aRow[i].lpProps[E_DATASTORE_EID].Value.bin.lpb,
																		  pRowSet->aRow[i].lpProps[E_DATASTORE_EID].Value.bin.cb))
									{
										found = true;
										break;
									}
								}
							}
						}

						DebugLog("Entry name: %s", pRowSet->aRow[i].lpProps[E_DATASTORE_NAME].Value.lpszA);

						if (found || g_EnableAll)
						{
							// Check only the appointments and tasks
							if (pRowSet->aRow[i].lpProps[E_DATASTORE_CONTAINER].ulPropTag == PR_CONTAINER_CLASS)
							{
								if (g_AdviseSink)
								{
									LPENTRYID id = (LPENTRYID)pRowSet->aRow[i].lpProps[E_DATASTORE_EID].Value.bin.lpb;
									ULONG size = pRowSet->aRow[i].lpProps[E_DATASTORE_EID].Value.bin.cb;
									g_AdviseSink->AddParentID(id, size);
								}

								DebugLog("Entry container: %s", pRowSet->aRow[i].lpProps[E_DATASTORE_CONTAINER].Value.lpszA);

								if (strcmp(pRowSet->aRow[i].lpProps[E_DATASTORE_CONTAINER].Value.lpszA, "IPF.Appointment") == 0)
								{
									GetMessageData(lpmdb, &pRowSet->aRow[i].lpProps[E_DATASTORE_EID], pRowSet->aRow[i].lpProps[E_DATASTORE_NAME].Value.lpszA);
								}
								else if (strcmp(pRowSet->aRow[i].lpProps[E_DATASTORE_CONTAINER].Value.lpszA, "IPF.Task") == 0)
								{
									GetTodoData(lpmdb, &pRowSet->aRow[i].lpProps[E_DATASTORE_EID], pRowSet->aRow[i].lpProps[E_DATASTORE_NAME].Value.lpszA);
								}
							}
						}
					}

					FreeProws(pRowSet);
				}
				else
				{
					DebugLog("OUTLOOK: HrQueryAllRows failed: 0x%08x (%i)", hr, __LINE__);
				}

				UlRelease(ptblHier);
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

ULONG NotificationCallback(LPVOID lpvContext, ULONG cNotification, LPNOTIFICATION lpNotifications)
{
	return S_OK;
}

void SyncWithOutlook()
{
    HRESULT hr;

	if (g_hMapiLib)
	{
		LoginMAPI();

		if (g_Session)
		{
			LPMAPITABLE ptable = NULL;
			hr = g_Session->GetMsgStoresTable(0, &ptable);
			
			if (SUCCEEDED(hr))
			{
				LPSRowSet prows = NULL;

				hr = HrQueryAllRows(ptable, (LPSPropTagArray) &rootProps, NULL, NULL, 0, &prows);

				if (SUCCEEDED(hr))
				{
					bool addAdvises = false;
					if (g_WatchChanges && g_AdvisedStores.empty())
					{
						addAdvises = true;
					}

					DebugLog("OUTLOOK: Found %i message stores.", prows->cRows);

					for	(int i = 0; i < prows->cRows; i++)
					{
						LPMDB lpmdb = NULL;

						hr = g_Session->OpenMsgStore(NULL,
							prows->aRow[i].lpProps[E_ROOT_EID].Value.bin.cb,			// [1] == PR_ENTRYID
							(LPENTRYID)prows->aRow[i].lpProps[E_ROOT_EID].Value.bin.lpb,
							NULL, MAPI_BEST_ACCESS | MDB_NO_MAIL, &lpmdb);

						if (SUCCEEDED(hr))
						{
							if (addAdvises)
							{
								if (g_AdviseSink == NULL)
								{
									g_AdviseSink = new CAdviseSink;
								}

								ULONG conn;
								hr = lpmdb->Advise(0, NULL, fnevObjectCreated | fnevObjectDeleted | fnevObjectModified | fnevObjectMoved, g_AdviseSink, &conn);

								if (FAILED(hr))
								{
									DebugLog("OUTLOOK: Advise failed: 0x%08x (%i)", hr, __LINE__);
								}
								else
								{
									// Store the message stores so that advises work
									lpmdb->AddRef();
									g_AdvisedStores.push_back(lpmdb);
									g_AdvisedConnections.push_back(conn);
								}
							}

							LPSPropValue pvalProp = NULL;
							hr = HrGetOneProp((LPMAPIPROP)lpmdb, PR_DISPLAY_NAME, &pvalProp);
							if (SUCCEEDED(hr))
							{
								bool found = false;

								// Check if the name is in the include list
								int j = 0;
								for (j = 0; j < g_Stores.size(); j++)
								{
									if (g_Stores[j].storeID.empty()) 
									{
										if (g_Stores[j].name == pvalProp->Value.lpszA)
										{
											found = true;
											break;
										}
									}
									else
									{
										if (g_Stores[j].storeID == IDToString((LPENTRYID)prows->aRow[i].lpProps[E_ROOT_EID].Value.bin.lpb,
																			prows->aRow[i].lpProps[E_ROOT_EID].Value.bin.cb))
										{
											found = true;
											break;
										}
									}
								}
								if (found || g_EnableAll)
								{
									DebugLog("OUTLOOK: MsgStore: %s", pvalProp->Value.lpszA);
									ScanMessageFolders(lpmdb, (g_Stores.size()) > 0 ? &g_Stores[j] : NULL);
								}

//								DebugLog("MsgStore: %s", pvalProp->Value.lpszA);
								MAPIFreeBuffer(pvalProp);
							}

							UlRelease(lpmdb);
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
				DebugLog("OUTLOOK: GetMsgStoresTable failed: 0x%08x", hr);
			}

			if (!g_KeepAlive)
			{
				LogoutMAPI();
			}
		}
		else
		{
			DebugLog("OUTLOOK: The g_Session is not set even though login succeeded. Weird.");
		}
	}
}

bool LoginMAPI()
{
    bool ret = false;

	if (g_hMapiLib && g_Session == NULL)
	{
		__try
		{
			HRESULT hr = MAPIInitialize(NULL);
			if (SUCCEEDED(hr)) 
			{
				DebugLog("OUTLOOK: MAPIInitialize succeeded");

				TCHAR profile[65] = {0};

				// Create a MAPI session
				DWORD flags = MAPI_EXTENDED | MAPI_LOGON_UI | MAPI_NEW_SESSION | MAPI_NO_MAIL;

				if (!g_OutlookProfile.empty())
				{
					flags |= MAPI_EXPLICIT_PROFILE;
					_tcsncpy(profile, g_OutlookProfile.c_str(), 64);
					hr = MAPILogonEx(NULL, profile, NULL, flags, &g_Session);
				}
				else
				{
					flags |= MAPI_USE_DEFAULT;
					hr = MAPILogonEx(NULL, NULL, NULL, flags, &g_Session);
				}

				if (SUCCEEDED(hr))
				{
					DebugLog("OUTLOOK: MAPILogonEx succeeded");
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
		__except( ShowException(GetExceptionCode(), GetExceptionInformation()) )
		{
	//		if (mapiInit)
	//		{
	//			MAPIUninitialize();
	//		}
		}
	}
	else
	{
		ret = true;	// Already logged in
	}
	return ret;
}

void LogoutMAPI()
{
	int i;

	for (i = 0; i < g_AdvisedStores.size(); i++)
	{
		g_AdvisedStores[i]->Unadvise(g_AdvisedConnections[i]);
		g_AdvisedStores[i]->Release();
	}
	g_AdvisedStores.clear();
	g_AdvisedConnections.clear();

	if (g_AdviseSink)
	{
		g_AdviseSink->Release();
		g_AdviseSink = NULL;
	}

	// Logout from MAPI
	g_Session->Logoff(NULL, NULL, NULL);
	g_Session->Release();
	g_Session = NULL;
	MAPIUninitialize();
}

std::string IDToString(LPENTRYID id, ULONG size)
{
	std::string value;
	char buffer[4096];

	if (size < 1024) 
	{
		sprintf(buffer, "%X ", size);
		value += buffer;

		for (int i = 0; i < size; i++)
		{
			if (i != 0) 
			{
				value += " ";
			}
			sprintf(buffer, "%02X", ((BYTE*)id)[i]);
			value += buffer;
		}
	}

	return value;
}

LPENTRYID StringToID(const std::string& value, ULONG& size)
{
	const char* szValue = value.c_str();
	sscanf(szValue, "%X", &size);

	if (size > 0) 
	{
		BYTE* pData = new BYTE[size];

		for (int i = 0; i < size; i++) 
		{
			int data;
			sscanf(szValue + (i + 1) * 3, "%X", &data);
			pData[i] = (BYTE)data;
		}
		
		return (LPENTRYID)pData;
	}
	return NULL;
}


void GetMessageStores(std::vector<MessageStoreName>& stores)
{
	stores.clear();

    HRESULT hr;

	if (g_hMapiLib)
	{
		LoginMAPI();

		if (g_Session)
		{
			LPMAPITABLE ptable = NULL;
			hr = g_Session->GetMsgStoresTable(0, &ptable);
			
			if (SUCCEEDED(hr))
			{
				LPSRowSet prows = NULL;

				hr = HrQueryAllRows(ptable, (LPSPropTagArray) &rootProps, NULL, NULL, 0, &prows);

				if (SUCCEEDED(hr))
				{
					for	(int i = 0; i < prows->cRows; i++)
					{
						LPMDB lpmdb = NULL;

						hr = g_Session->OpenMsgStore(NULL,
							prows->aRow[i].lpProps[E_ROOT_EID].Value.bin.cb,			// [1] == PR_ENTRYID
							(LPENTRYID)prows->aRow[i].lpProps[E_ROOT_EID].Value.bin.lpb,
							NULL, MAPI_BEST_ACCESS | MDB_NO_MAIL, &lpmdb);

						if (SUCCEEDED(hr))
						{
							MessageStoreName storeName;
							LPSPropValue pvalProp = NULL;
							hr = HrGetOneProp((LPMAPIPROP)lpmdb, PR_DISPLAY_NAME, &pvalProp);
							if (SUCCEEDED(hr))
							{
								storeName.name = pvalProp->Value.lpszA;
								storeName.storeID = IDToString((LPENTRYID)prows->aRow[i].lpProps[E_ROOT_EID].Value.bin.lpb, 
																prows->aRow[i].lpProps[E_ROOT_EID].Value.bin.cb);
								MAPIFreeBuffer(pvalProp);
							}

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
											for	(int j = 0; j < pRowSet->cRows; j++)
											{
												// Check only the supported folders
												if (pRowSet->aRow[j].lpProps[E_DATASTORE_CONTAINER].ulPropTag == PR_CONTAINER_CLASS)
												{
													DebugLog("MsgStore: %s (%s)", pRowSet->aRow[j].lpProps[E_DATASTORE_NAME].Value.lpszA, pRowSet->aRow[j].lpProps[E_DATASTORE_CONTAINER].Value.lpszA);

													if (strcmp(pRowSet->aRow[j].lpProps[E_DATASTORE_CONTAINER].Value.lpszA, "IPF.Appointment") == 0 ||
													    strcmp(pRowSet->aRow[j].lpProps[E_DATASTORE_CONTAINER].Value.lpszA, "IPF.Task") == 0)
													{
														storeName.folders.push_back(pRowSet->aRow[j].lpProps[E_DATASTORE_NAME].Value.lpszA);
														storeName.folderIDs.push_back(IDToString((LPENTRYID)pRowSet->aRow[j].lpProps[E_DATASTORE_EID].Value.bin.lpb,
																								 pRowSet->aRow[j].lpProps[E_DATASTORE_EID].Value.bin.cb));
													}
												}
											}

											FreeProws(pRowSet);
										}
										else
										{
											DebugLog("OUTLOOK: HrQueryAllRows failed: 0x%08x (%i)", hr, __LINE__);
										}

										UlRelease(ptblHier);
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

							UlRelease(lpmdb);

							stores.push_back(storeName);
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
				DebugLog("OUTLOOK: GetMsgStoresTable failed: 0x%08x", hr);
			}

			if (!g_KeepAlive)
			{
				LogoutMAPI();
			}
		}
	}

	if (g_EnableAll)
	{
		g_Stores = stores;
		g_EnableAll = false;
	}
}

int ShowException(int exNum, LPEXCEPTION_POINTERS exInfo)
{
	DebugLog("OUTLOOK: Outlook sync crashed at 0x%08x (0x%08x).", exInfo->ExceptionRecord->ExceptionAddress, exNum);
	return EXCEPTION_EXECUTE_HANDLER;
}

void DebugLog(const char* format, ... )
{
	char buffer[4096];
    va_list args;
    va_start( args, format );
    _vsnprintf( buffer, 4096, format, args );
	Rainlendar_Log(buffer);
	va_end(args);
};

typedef void (__stdcall FREEPROWS)(LPSRowSet);
typedef ULONG (__stdcall ULRELEASE)(LPVOID);
typedef HRESULT (__stdcall HRQUERYALLROWS)(LPMAPITABLE, LPSPropTagArray, LPSRestriction, LPSSortOrderSet, LONG, LPSRowSet FAR*);
typedef HRESULT (__stdcall HRGETONEPROP)(LPMAPIPROP, ULONG, LPSPropValue FAR*);

MAPIUNINITIALIZE* fpMAPIUninitialize = NULL;
MAPIFREEBUFFER* fpMAPIFreeBuffer = NULL;
HRGETONEPROP* fpHrGetOneProp = NULL;
FREEPROWS* fpFreeProws = NULL;
ULRELEASE* fpUlRelease = NULL;
HRQUERYALLROWS* fpHrQueryAllRows = NULL;
MAPILOGONEX* fpMAPILogonEx = NULL;
MAPIINITIALIZE* fpMAPIInitialize = NULL;

bool InitMapi()
{
	g_hMapiLib = LoadLibrary("mapi32.dll");

	if (g_hMapiLib)
	{
		fpMAPIUninitialize = (MAPIUNINITIALIZE*)GetProcAddress(g_hMapiLib, "MAPIUninitialize");
		fpMAPIFreeBuffer = (MAPIFREEBUFFER*)GetProcAddress(g_hMapiLib, "MAPIFreeBuffer");
		fpHrGetOneProp = (HRGETONEPROP*)GetProcAddress(g_hMapiLib, "HrGetOneProp@12");
		fpFreeProws = (FREEPROWS*)GetProcAddress(g_hMapiLib, "FreeProws@4");
		fpUlRelease = (ULRELEASE*)GetProcAddress(g_hMapiLib, "UlRelease@4");
		fpHrQueryAllRows = (HRQUERYALLROWS*)GetProcAddress(g_hMapiLib, "HrQueryAllRows@24");
		fpMAPILogonEx = (MAPILOGONEX*)GetProcAddress(g_hMapiLib, "MAPILogonEx");
		fpMAPIInitialize = (MAPIINITIALIZE*)GetProcAddress(g_hMapiLib, "MAPIInitialize");

		if (fpMAPIUninitialize == NULL ||
			fpMAPIFreeBuffer == NULL ||
			fpHrGetOneProp == NULL ||
			fpFreeProws == NULL ||
			fpUlRelease == NULL ||
			fpHrQueryAllRows == NULL ||
			fpMAPILogonEx == NULL ||
			fpMAPIInitialize == NULL)
		{
			DebugLog("OUTLOOK: Unable to initialize Mapi32.dll.");
			FreeMapi();
			return false;
		}
	}
	else
	{
		DebugLog("OUTLOOK: Mapi32.dll not found.");
		return false;
	}

	return true;
}

void FreeMapi()
{
	if (g_hMapiLib)
	{
		FreeLibrary(g_hMapiLib);
		g_hMapiLib = NULL;
	}
}

void __stdcall MAPIUninitialize()
{
	fpMAPIUninitialize();
}

ULONG __stdcall MAPIFreeBuffer(LPVOID lpBuffer)
{
	return fpMAPIFreeBuffer(lpBuffer);
}

HRESULT __stdcall HrGetOneProp(
  LPMAPIPROP pmp,            
  ULONG ulPropTag,           
  LPSPropValue FAR * ppprop  
)
{
	return fpHrGetOneProp(pmp, ulPropTag, ppprop);
}

void __stdcall FreeProws(LPSRowSet prows)
{
	fpFreeProws(prows);
}
 
ULONG __stdcall UlRelease(LPVOID punk)
{
	return fpUlRelease(punk);
}

HRESULT __stdcall HrQueryAllRows(
  LPMAPITABLE ptable,    
  LPSPropTagArray ptaga,   
  LPSRestriction pres,   
  LPSSortOrderSet psos,  
  LONG crowsMax,         
  LPSRowSet FAR * pprows 
)
{
	return fpHrQueryAllRows(ptable, ptaga, pres, psos, crowsMax, pprows);
}


HRESULT __stdcall MAPILogonEx(
  ULONG ulUIParam,                
  LPTSTR lpszProfileName,         
  LPTSTR lpszPassword,            
  FLAGS flFlags,                  
  LPMAPISESSION FAR * lppSession  
)
{
	return fpMAPILogonEx(ulUIParam, lpszProfileName, lpszPassword, flFlags, lppSession);
}


HRESULT __stdcall MAPIInitialize(LPVOID lpMapiInit)
{
	return fpMAPIInitialize(lpMapiInit);
}

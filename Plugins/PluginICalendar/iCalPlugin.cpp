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
  $Header: /home/cvsroot/Rainlendar/Plugins/PluginICalendar/iCalPlugin.cpp,v 1.2 2005/07/20 18:47:10 rainy Exp $

  $Log: iCalPlugin.cpp,v $
  Revision 1.2  2005/07/20 18:47:10  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.2  2005/03/25 13:59:29  rainy
  no message

  Revision 1.1  2005/03/01 18:26:06  rainy
  Initial version.


*/

#pragma warning(disable: 4786)

#include "iCalPlugin.h"
#include "resource.h"
#include <time.h>
#include <vector>
#include <list>

// Ical files
extern "C"
{
#include <ical.h>
#include <icalfileset.h>
}

#ifdef _DEBUG
#pragma comment(lib, "..\\..\\3rdparty\\libical\\src\\libical\\Debug\\libical.lib")
#pragma comment(lib, "..\\..\\3rdparty\\libical\\src\\libicalss\\Debug\\libicalss.lib")
#else
#pragma comment(lib, "..\\..\\3rdparty\\libical\\src\\libical\\Release\\libical.lib")
#pragma comment(lib, "..\\..\\3rdParty\\libical\\src\\libicalss\\Release\\libicalss.lib")
#endif

enum IMPORT_ITEMS
{
	IMPORT_EVENTS = 1,
	IMPORT_TODOS = 2
};

void ReadSettings(LPCTSTR configFile);
void WriteSettings(LPCTSTR configFile);
bool ImportEvents(LPCTSTR eventFile, bool readOnly, bool setID, int items);
DWORD WINAPI MonitorThreadProc(LPVOID pParam);
void StartMonitor();
bool EndMonitor();
void DebugLog(const char* format, ... );
LPCTSTR EncodeGUID(GUID* guid);
bool DecodeGUID(GUID* guid, LPCTSTR str);
bool ExportEvents(LPCTSTR eventFile, RainlendarItem** items, bool merge);
bool ExportEvents(LPCTSTR eventFile, GUID** guids, bool merge);

std::vector<ICALFILENAME> g_EventFiles;
bool g_MonitorChanges = false;
bool g_IgnoreCompletedTasks = false;
HINSTANCE g_Instance = NULL;

static UINT g_ID = 0;
static HANDLE g_Thread = NULL;
static std::list<icalcomponent*> g_DeletedComponents;
static std::string g_ConfigFile;

static HANDLE g_ThreadStopEvent = NULL;
static HANDLE g_ThreadWaitEvent = NULL;

void Plugin_Initialize(HINSTANCE instance, LPCTSTR configFile, UINT id)
{
	g_Instance = instance;
	g_ID = id;
	g_ConfigFile = configFile;

	g_ThreadStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_ThreadWaitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void Plugin_Finalize()
{
	EndMonitor();

	CloseHandle(g_ThreadStopEvent);
	CloseHandle(g_ThreadWaitEvent);
}

void Plugin_Enable()
{
	// Read the settings when the plugin is enabled or the Rainlendar is refreshed
	ReadSettings(g_ConfigFile.c_str());

	StartMonitor();
}

void Plugin_Disable()
{
	EndMonitor();	// End the monitor thread
}

void Plugin_ReadItems()
{
	int i;

	EndMonitor();	// End the monitor thread

	for (i = 0; i < g_EventFiles.size(); i++)
	{
		ImportEvents(g_EventFiles[i].filename.c_str(), g_EventFiles[i].readOnly, true, IMPORT_EVENTS | IMPORT_TODOS);
	}

	StartMonitor();
}

LPCTSTR Plugin_GetName()
{
	return _T("iCalendar Plugin");
}

LPCTSTR Plugin_GetAuthor()
{
	return _T("Rainy (rainy@iki.fi)");
}

UINT Plugin_GetVersion()
{
	return 1004;	// Rainlendar 0.21.1
}

void Plugin_ItemNotification(GUID** guid)
{
	int i;

	EndMonitor();

	for (i = 0; i < g_EventFiles.size(); i++)
	{
		if (!g_EventFiles[i].readOnly)
		{
			ExportEvents(g_EventFiles[i].filename.c_str(), guid, true);
		}
	}

	StartMonitor();
}

bool Plugin_EditItem(HWND parent, RainlendarItem* item, RAINLENDAR_TYPE type)
{
	// Just use the default editor
	if (Rainlendar_EditItemDialog(parent, item, type))
	{
		if (item && item->type == RAINLENDAR_TYPE_EVENT)	// Todo-list works automatically
		{
			// The event was changed
			Rainlendar_SetItem(item, g_ID);
		}
		return true;
	}
	return false;
}


bool Plugin_ShowConfig(HWND hwndParent, LPCTSTR configFile)
{
	bool running = EndMonitor();

	ReadSettings(configFile);

	if (DialogBox(g_Instance, MAKEINTRESOURCE(IDD_SETUP), hwndParent, (DLGPROC)SetupProc) == IDOK) 
	{
		WriteSettings(configFile);
		if (running) StartMonitor();
		// Return true if something has changed and all events need to be reloaded.
		return true;
	}
	if (running) StartMonitor();
	return false;
}

bool Plugin_ImportItems(LPCTSTR eventFile, RAINLENDAR_TYPE type)
{
	if (type == RAINLENDAR_TYPE_EVENT)
	{
		return ImportEvents(eventFile, false, false, IMPORT_EVENTS);
	}
	else if (type == RAINLENDAR_TYPE_TODO)
	{
		return ImportEvents(eventFile, false, false, IMPORT_TODOS);
	}
	return false;
}

void Plugin_ExportItems(LPCTSTR eventFile, GUID** guids, RAINLENDAR_TYPE type)
{
	ExportEvents(eventFile, guids, false);
}

bool ExportEvents(LPCTSTR eventFile, GUID** guids, bool merge)
{
	bool bResult = false;

	std::tstring file = eventFile;

	// Make sure that the file has .ics extension
	bool extOK = false;
	if (file.size() >= 4)
	{
		int pos = file.rfind(_T('.'));
		if (pos != -1)
		{
			std::tstring ext = file.substr(pos);
			if ((_tcsicmp(ext.c_str(), _T(".ics")) == 0) ||
			    (_tcsicmp(ext.c_str(), _T(".vcf")) == 0))
			{
				extOK = true;
			}
		}
	}

	if (!extOK)
	{
		file += _T(".ics");
	}

	if (guids != NULL)		// Only save given events
	{
		RainlendarItem** items = NULL;

		// Calculate the number of events
		int count = 0;
		while (guids[count] != NULL)
		{
			count++;
		}

		items = new RainlendarItem*[count + 1];

		int index = 0;
		for (int i = 0; i < count; i++)
		{
			RainlendarItem* item = Rainlendar_GetItem(guids[i]);
			if (item)
			{
				items[index++] = item;
			}
		}
		items[index++] = NULL;

		bResult = ExportEvents(file.c_str(), items, merge);

		delete [] items;
	}

	return bResult;
}

void ReadSettings(LPCTSTR configFile)
{
	TCHAR buffer[MAX_PATH];
	int i;

	g_EventFiles.clear();

	// Read settings from configFile
	g_MonitorChanges = (GetPrivateProfileInt(PLUGIN_NAME, _T("iCalMonitorChanges"), 0, configFile) == 1);
	g_IgnoreCompletedTasks = (GetPrivateProfileInt(PLUGIN_NAME, _T("iCalIgnoreCompletedTasks"), 0, configFile) == 1);

	int count = GetPrivateProfileInt(PLUGIN_NAME, _T("iCalFileCount"), -1, configFile);
	
	for (i = 0; i < count; i++)
	{
		_stprintf(buffer, _T("iCalFile%i"), i + 1);
		if(GetPrivateProfileString(PLUGIN_NAME, buffer, NULL, buffer, MAX_PATH, configFile) > 0) 
		{
			ICALFILENAME iniFileName;
			iniFileName.filename = buffer;
			
			_stprintf(buffer, _T("iCalReadOnly%i"), i + 1);
			iniFileName.readOnly = (1 == GetPrivateProfileInt(PLUGIN_NAME, buffer, 0, configFile));

			g_EventFiles.push_back(iniFileName);
		}
	}
}

void WriteSettings(LPCTSTR configFile)
{
	int i;
	char tmpSz[256];

	sprintf(tmpSz, "%i", g_MonitorChanges);
	WritePrivateProfileString(PLUGIN_NAME, _T("iCalMonitorChanges"), tmpSz, configFile);

	sprintf(tmpSz, "%i", g_IgnoreCompletedTasks);
	WritePrivateProfileString(PLUGIN_NAME, _T("iCalIgnoreCompletedTasks"), tmpSz, configFile);

	sprintf(tmpSz, "%i", g_EventFiles.size());
	WritePrivateProfileString(PLUGIN_NAME, _T("iCalFileCount"), tmpSz, configFile);

	for (i = 0; i < g_EventFiles.size(); i++)
	{
		_stprintf(tmpSz, _T("iCalFile%i"), i + 1);
		WritePrivateProfileString(PLUGIN_NAME, tmpSz, g_EventFiles[i].filename.c_str(), configFile);
		_stprintf(tmpSz, _T("iCalReadOnly%i"), i + 1);
		WritePrivateProfileString(PLUGIN_NAME, tmpSz, g_EventFiles[i].readOnly ? _T("1") : _T("0"), configFile);
	}
}

FILETIME CreateFileTime(icaltimetype* time)
{
	FILETIME fileTime;
	SYSTEMTIME sysTime;
	memset(&sysTime, 0, sizeof(SYSTEMTIME));

	sysTime.wDay = time->day;
	sysTime.wMonth = time->month;
	sysTime.wYear = time->year;
	sysTime.wHour = time->hour;
	sysTime.wMinute = time->minute;
	sysTime.wSecond = time->second;

	if (time->is_utc && !time->is_date)
	{
		SYSTEMTIME localSysTime;

		// Convert to local time
		if (SystemTimeToTzSpecificLocalTime(NULL, &sysTime, &localSysTime))
		{
			SystemTimeToFileTime(&localSysTime, &fileTime);
		}
		else
		{
			SystemTimeToFileTime(&sysTime, &fileTime);
		}
	}
	else
	{
		SystemTimeToFileTime(&sysTime, &fileTime);
	}

	return fileTime;
}

icaltimetype* CreateFileTime(FILETIME fileTime)
{
	static icaltimetype icalTime;
	SYSTEMTIME sysTime;
	memset(&sysTime, 0, sizeof(SYSTEMTIME));
	memset(&icalTime, 0, sizeof(icaltimetype));

	FileTimeToSystemTime(&fileTime, &sysTime);

	icalTime.day = sysTime.wDay;
	icalTime.month = sysTime.wMonth;
	icalTime.year = sysTime.wYear;
	icalTime.hour = sysTime.wHour;
	icalTime.minute = sysTime.wMinute;
	icalTime.second = sysTime.wSecond;

	return &icalTime;
}

time_t CreateTime(icaltimetype* time)
{
	// TODO: Convert to local time (if it's not already)
	struct tm sysTime;
	memset(&sysTime, 0, sizeof(tm));

	sysTime.tm_mday = time->day;
	sysTime.tm_mon = time->month - 1;
	sysTime.tm_year = time->year - 1900;
	sysTime.tm_hour = time->hour;
	sysTime.tm_min = time->minute;
	sysTime.tm_sec = time->second;

	return mktime(&sysTime);
}

icaltimetype* CreateTime(time_t time)
{
	static icaltimetype icalTime;

	struct tm* sysTime;
	sysTime = gmtime(&time);

	memset(&icalTime, 0, sizeof(icaltimetype));

	icalTime.day = sysTime->tm_mday;
	icalTime.month = sysTime->tm_mon + 1;
	icalTime.year = sysTime->tm_year + 1900;
	icalTime.hour = sysTime->tm_hour;
	icalTime.minute = sysTime->tm_min;
	icalTime.second = sysTime->tm_sec;

	return &icalTime;
}

// Caller must delete the returned string!
char* ConvertStringToACP(const char* utf8)
{
	int len = (_mbstrlen(utf8) + 1) * 4;

	WCHAR* buffer = new WCHAR[len]; 
	char* buffer2 = (char*)malloc(len);
	memset(buffer, 0, sizeof(WCHAR) * len);
	memset(buffer2, 0, sizeof(char) * len);

	MultiByteToWideChar(CP_UTF8, 0, utf8, _mbstrlen(utf8), buffer, len);
	WideCharToMultiByte(GetACP(), 0, buffer, wcslen(buffer), buffer2, len, NULL, FALSE);

	delete [] buffer;

	return buffer2;
}

char* ConvertStringToUTF8(const char* str)
{
	int len = (_mbstrlen(str) + 1) * 4;

	WCHAR* buffer = new WCHAR[len]; 
	char* buffer2 = new char[len];
	memset(buffer, 0, sizeof(WCHAR) * len);
	memset(buffer2, 0, sizeof(char) * len);

	MultiByteToWideChar(GetACP(), 0, str, _mbstrlen(str), buffer, len);
	WideCharToMultiByte(CP_UTF8, 0, buffer, wcslen(buffer), buffer2, len, NULL, FALSE);

	delete [] buffer;

	return buffer2;
}

RainlendarEvent* ScanEvent(icalcomponent* component, bool readOnly, bool setID)
{
//	DebugLog(icalcomponent_as_ical_string(component));	// DEBUG

	bool valid = false;

	icaltimetype startTime = icalcomponent_get_dtstart(component);
	icaltimetype endTime = icalcomponent_get_dtend(component);
	const char* description = icalcomponent_get_description(component);
	const char* summary = icalcomponent_get_summary(component);
	const char* iCalGuid = icalcomponent_get_uid(component);
	icaltimetype timeStamp = icalcomponent_get_dtstamp(component);

	// Last Modified overrides the timestamp
	icalproperty* lmProp = icalcomponent_get_first_property(component, ICAL_LASTMODIFIED_PROPERTY);
	if (lmProp) 
	{
		icalvalue* val = icalproperty_get_value(lmProp);
		if (val)
		{
			timeStamp = icalvalue_get_datetime(val);
		}
	}

	RainlendarEvent* newEvent = new RainlendarEvent;
	memset(newEvent, 0, sizeof(RainlendarEvent));
	newEvent->size = sizeof(RainlendarEvent);
	newEvent->type = RAINLENDAR_TYPE_EVENT;

	if (icaltime_is_null_time(timeStamp))
	{
		time(&newEvent->timeStamp);
	}
	else
	{
		newEvent->timeStamp = CreateTime(&timeStamp);
	}

	if (iCalGuid == NULL || !DecodeGUID(&newEvent->guid, iCalGuid))
	{
		if (setID) 
		{
			if (!readOnly)
			{
				DebugLog("%s: Unable to decode uid \"%s\". The event is imported as read-only.", PLUGIN_NAME, iCalGuid);
			}
			readOnly = true;
		}
		else
		{
			DebugLog("%s: Unable to decode uid \"%s\". A new GUID is created.", PLUGIN_NAME, iCalGuid);
		}
		CoCreateGuid(&newEvent->guid);
	}

	newEvent->readOnly = readOnly;

	if (!icaltime_is_null_time(startTime))
	{
		valid = true;
	}

	newEvent->startTime = CreateFileTime(&startTime);
	newEvent->endTime = newEvent->startTime;
	if (!icaltime_is_null_time(endTime))
	{
		newEvent->endTime = CreateFileTime(&endTime);
	}

	if (icaltime_is_date(startTime))
	{
		newEvent->allDayEvent = true;
	}

	// Use the first category as profile
	icalproperty* catProp = icalcomponent_get_first_property(component, ICAL_CATEGORIES_PROPERTY);
	if (catProp)
	{
		icalvalue* val = icalproperty_get_value(catProp);
		if (val)
		{
			const char* cat = icalvalue_get_text(val);
			if (cat)
			{
				newEvent->profile = ConvertStringToACP(cat);
			}
		}
	}

	if (summary)
	{
		newEvent->header = ConvertStringToACP(summary);
	}

	if (description)
	{
		newEvent->message = ConvertStringToACP(description);
	}

	// Recurrence
	icalproperty* recurProp = icalcomponent_get_first_property(component, ICAL_RRULE_PROPERTY);
	if (recurProp)
	{
		icalrecurrencetype type = icalproperty_get_rrule(recurProp);
		
		RainlendarRecurrency* newRecurrency = new RainlendarRecurrency;
		memset(newRecurrency, 0, sizeof(RainlendarRecurrency));
		newRecurrency->size = sizeof(RainlendarRecurrency);

		switch(type.freq) 
		{
		case ICAL_DAILY_RECURRENCE:
			newRecurrency->type = RECURRENCY_TYPE_DAILY;
			break;

		case ICAL_WEEKLY_RECURRENCE:
			{
				newRecurrency->type = RECURRENCY_TYPE_WEEKLY;
				int i = 0;
				while (type.by_day[i] != ICAL_RECURRENCE_ARRAY_MAX)
				{
					int weekday = icalrecurrencetype_day_day_of_week(type.by_day[i]);
					newRecurrency->days |= 1 << (weekday - 1);
					i++;
				}
			}
			break;

		case ICAL_MONTHLY_RECURRENCE:
			if (type.by_day[0] != ICAL_RECURRENCE_ARRAY_MAX)
			{
				int day = icalrecurrencetype_day_position(type.by_day[0]);
				if (day == -1)
				{
					newRecurrency->type = RECURRENCY_TYPE_MONTHLY_LASTWEEK_WEEKDAY;
				}
				else
				{
					if (type.by_day[0] == 2 && type.by_day[1] == 3 && type.by_day[2] == 4 && type.by_day[3] == 5 && type.by_day[4] == 6)
					{
						if (type.by_set_pos[0] == 1)
						{
							newRecurrency->type = RECURRENCY_TYPE_MONTHLY_FIRST_WEEKDAY;
						}
						else if (type.by_set_pos[0] == -1)
						{
							newRecurrency->type = RECURRENCY_TYPE_MONTHLY_LAST_WEEKDAY;
						}
					}
					else
					{
						newRecurrency->type = RECURRENCY_TYPE_MONTHLY_NTH_WEEKDAY;
					}
				}
			}
			else if (type.by_month_day[0] == -1)
			{
				newRecurrency->type = RECURRENCY_TYPE_MONTHLY_LAST_DAY;
			}
			else
			{
				newRecurrency->type = RECURRENCY_TYPE_MONTHLY_NTH_DAY;
			}
			break;

		case ICAL_YEARLY_RECURRENCE:
			newRecurrency->type = RECURRENCY_TYPE_YEARLY;
			break;

		default:
			newRecurrency->type = RECURRENCY_TYPE_SINGLE;
			break;
		}

		newRecurrency->frequency = type.interval;

		if (type.count == 0)
		{
			if (icaltime_is_null_time(type.until))
			{
				newRecurrency->repeatType = RECURRENCY_REPEAT_FOREVER;
			}
			else
			{
				newRecurrency->repeatType = RECURRENCY_REPEAT_UNTIL;
				newRecurrency->until = CreateFileTime(&type.until);
			}
		}
		else
		{
			newRecurrency->repeatType = RECURRENCY_REPEAT_COUNT;

			if (type.freq == ICAL_WEEKLY_RECURRENCE)
			{
				// The count depends on the days
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

				if (numOfDays > 0)
				{
					newRecurrency->count = (type.count + (numOfDays - 1)) / numOfDays;
				}
			}
			else
			{
				newRecurrency->count = type.count;
			}
		}

		newEvent->recurrency = newRecurrency;
	}

	if (!valid)
	{
		Rainlendar_DeleteItem(newEvent);
		newEvent = NULL;
	}

	return newEvent;
}

RainlendarTodo* ScanTodo(icalcomponent* component, bool readOnly, bool setID)
{
	const char* summary = icalcomponent_get_summary(component);
	const char* iCalGuid = icalcomponent_get_uid(component);
	icaltimetype timeStamp = icalcomponent_get_dtstamp(component);

	// Last Modified overrides the timestamp
	icalproperty* lmProp = icalcomponent_get_first_property(component, ICAL_LASTMODIFIED_PROPERTY);
	if (lmProp) 
	{
		icalvalue* val = icalproperty_get_value(lmProp);
		if (val)
		{
			timeStamp = icalvalue_get_datetime(val);
		}
	}

	bool valid = false;

	RainlendarTodo* newTodo = new RainlendarTodo;
	memset(newTodo, 0, sizeof(RainlendarTodo));
	newTodo->size = sizeof(RainlendarTodo);
	newTodo->type = RAINLENDAR_TYPE_TODO;

	if (icaltime_is_null_time(timeStamp))
	{
		time(&newTodo->timeStamp);
	}
	else
	{
		newTodo->timeStamp = CreateTime(&timeStamp);
	}

	if (!DecodeGUID(&newTodo->guid, iCalGuid))
	{
		if (setID) 
		{
			if (!readOnly)
			{
				DebugLog("%s: Unable to decode uid \"%s\". The todo is imported as read-only.", PLUGIN_NAME, iCalGuid);
			}
			readOnly = true;
		}
		else
		{
			DebugLog("%s: Unable to decode uid \"%s\". A new GUID is created.", PLUGIN_NAME, iCalGuid);
		}
		CoCreateGuid(&newTodo->guid);
	}

	newTodo->readOnly = readOnly;

	if (summary)
	{
		newTodo->message = ConvertStringToACP(summary);
		valid = true;
	}

	if (ICAL_STATUS_COMPLETED == icalcomponent_get_status(component))
	{
		newTodo->checked = true;
	}
	else
	{
		newTodo->checked = false;
	}

	newTodo->position = 100;
	icalproperty* posProp = icalcomponent_get_first_x_property(component, "X-RAINLENDAR-POSITION");
	if (posProp) 
	{
		icalvalue* val = icalproperty_get_value(posProp);
		if (val)
		{
			const char* szPos = icalvalue_get_x(val);
			if (szPos) 
			{
				newTodo->position = atoi(szPos);
			}
		}
	}

	newTodo->todoType = TODO_TYPE_TODO;
	newTodo->profile = strdup("Todo");

	if (!valid || (g_IgnoreCompletedTasks && newTodo->checked)) 
	{
		Rainlendar_DeleteItem(newTodo);
		newTodo = NULL;
	}

	return newTodo;
}

int ScanEvent(icalcomponent* component, bool readOnly, bool setID, RAINLENDAR_TYPE type)
{
	if (type == RAINLENDAR_TYPE_EVENT)
	{
		RainlendarEvent* newEvent = ScanEvent(component, readOnly, setID);
		if (newEvent)
		{
			if (!setID)
			{
				time(&newEvent->timeStamp);
			}
			Rainlendar_SetItem(newEvent, setID ? g_ID : 0);
			Rainlendar_DeleteItem(newEvent);
			return 1;
		}
	}
	else if (type == RAINLENDAR_TYPE_TODO)
	{
		RainlendarTodo* newTodo = ScanTodo(component, readOnly, setID);
		if (newTodo)
		{
			if (!setID)
			{
				time(&newTodo->timeStamp);
			}
			Rainlendar_SetItem(newTodo, setID ? g_ID : 0);
			Rainlendar_DeleteItem(newTodo);
			return 1;
		}
	}
	return 0;
}

bool ImportEvents(LPCTSTR eventFile, bool readOnly, bool setID, int items)
{
	if (eventFile == NULL) return false;

	// Make sure the file has .ics extension
	if (_tcslen(eventFile) > 4)
	{
		LPCTSTR pos = _tcsrchr(eventFile, _T('.'));
		if (pos)
		{
			if (_tcsicmp(pos, _T(".ics")) != 0 &&
			    _tcsicmp(pos, _T(".vcs")) != 0)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	icalset* store = icalfileset_new_reader(eventFile);
	if (store)
	{
		int eventCount = 0;
		int todoCount = 0;
		icalcomponent* calendar;

		for (calendar = icalfileset_get_first_component(store);
			 calendar != 0;
			 calendar = icalfileset_get_next_component(store))
		{
			if (icalcomponent_isa(calendar) == ICAL_VCALENDAR_COMPONENT)
			{
				icalcomponent* component;

				if (items & IMPORT_EVENTS)
				{
					for (component = icalcomponent_get_first_component(calendar, ICAL_VEVENT_COMPONENT);
						 component != 0;
						 component = icalcomponent_get_next_component(calendar, ICAL_VEVENT_COMPONENT))
					{
						eventCount += ScanEvent(component, readOnly, setID, RAINLENDAR_TYPE_EVENT);
					}
				}

				if (items & IMPORT_TODOS)
				{
					for (component = icalcomponent_get_first_component(calendar, ICAL_VTODO_COMPONENT);
						 component != 0;
						 component = icalcomponent_get_next_component(calendar, ICAL_VTODO_COMPONENT))
					{
						todoCount += ScanEvent(component, readOnly, setID, RAINLENDAR_TYPE_TODO);
					}
				}
			}
			else if ((items & IMPORT_EVENTS) && icalcomponent_isa(calendar) == ICAL_VEVENT_COMPONENT)
			{
				eventCount += ScanEvent(calendar, readOnly, setID, RAINLENDAR_TYPE_EVENT);
			}
			else if ((items & IMPORT_TODOS) && icalcomponent_isa(calendar) == ICAL_VTODO_COMPONENT)
			{
				todoCount += ScanEvent(calendar, readOnly, setID, RAINLENDAR_TYPE_TODO);
			}
		}

		DebugLog("%s: Read %i VEVENTs and %i VTODOs from file \"%s\"", PLUGIN_NAME, eventCount, todoCount, eventFile);

		icalfileset_free(store);
	}
	else
	{
		DebugLog("%s: Unable to open file \"%s\"", PLUGIN_NAME, eventFile);
	}

	return true;
}

void UpdateEventData(icalcomponent* component, RainlendarItem* item, bool newEvent)
{
	SYSTEMTIME sysTime;

	icalcomponent_set_uid(component, EncodeGUID(&item->guid));

	// If there is LAST MODIFIED, update that.
	bool stampOK = false;
	icalproperty* lmProp = icalcomponent_get_first_property(component, ICAL_LASTMODIFIED_PROPERTY);
	if (lmProp) 
	{
		icalvalue* val = icalproperty_get_value(lmProp);
		if (val)
		{
			icalvalue_set_datetime(val, *CreateTime(item->timeStamp));
			stampOK = true;
		}
	}

	if (!stampOK) 
	{
		icalcomponent_set_dtstamp(component, *CreateTime(item->timeStamp));

		// Create a new property
		lmProp = icalproperty_new(ICAL_LASTMODIFIED_PROPERTY);
		icalproperty_set_lastmodified(lmProp, *CreateTime(item->timeStamp));
		icalcomponent_add_property(component, lmProp);
	}

	if (item->type == RAINLENDAR_TYPE_EVENT)
	{
		RainlendarEvent* newEvent = (RainlendarEvent*)item;

		icaltimetype startTime = *CreateFileTime(newEvent->startTime);
		icaltimetype endTime = *CreateFileTime(newEvent->endTime);
		if (newEvent->allDayEvent)
		{
			startTime.is_date = true;
			endTime.is_date = true;
			icalcomponent_set_dtstart(component, startTime);
			icalcomponent_set_dtend(component, endTime);		// End date is same as start
		}
		else
		{
			icalcomponent_set_dtstart(component, startTime);
			icalcomponent_set_dtend(component, endTime);
		}

		if (newEvent->header && strlen(newEvent->header) > 0)
		{
			icalcomponent_set_summary(component, ConvertStringToUTF8(newEvent->header));
		}
		else
		{
			icalcomponent_set_summary(component, "");		// TODO: should remove the property instead
		}

		if (newEvent->message && strlen(newEvent->message) > 0)
		{
			icalcomponent_set_description(component, ConvertStringToUTF8(newEvent->message));
		}
		else
		{
			icalcomponent_set_description(component, "");		// TODO: should remove the property instead
		}

		// Use the first category as profile
		if (newEvent->profile && strlen(newEvent->profile) > 0)
		{
			char* str = ConvertStringToACP(newEvent->profile);

			icalproperty* catProp = icalcomponent_get_first_property(component, ICAL_CATEGORIES_PROPERTY);
			if (catProp)
			{
				icalproperty_set_categories(catProp, str);
			}
			else
			{
				// Create a new property
				catProp = icalproperty_new(ICAL_CATEGORIES_PROPERTY);
				icalproperty_set_categories(catProp, str);
				icalcomponent_add_property(component, catProp);
			}
			free(str);
		}

		// Recurrence
		if (newEvent->recurrency)
		{
			RainlendarRecurrency* newRecurrency = newEvent->recurrency;

			icalproperty* recurProp = icalcomponent_get_first_property(component, ICAL_RRULE_PROPERTY);
			if (!recurProp)
			{
				// Create a new property
				recurProp = icalproperty_new(ICAL_RRULE_PROPERTY);
				icalcomponent_add_property(component, recurProp);
			}

			icalrecurrencetype type;
			icalrecurrencetype_clear(&type);

			type.interval = newRecurrency->frequency;

			switch (newRecurrency->repeatType) 
			{
			case RECURRENCY_REPEAT_COUNT:
				type.count = newRecurrency->count;

				break;

			case RECURRENCY_REPEAT_UNTIL:
				type.until = *CreateFileTime(newRecurrency->until);
				break;

			default:	// RECURRENCY_REPEAT_FOREVER
				// Do nothing
				break;
			}

			switch (newRecurrency->type) 
			{
				case RECURRENCY_TYPE_DAILY:
					type.freq = ICAL_DAILY_RECURRENCE;
					break;

				case RECURRENCY_TYPE_WEEKLY:
					{
						type.freq = ICAL_WEEKLY_RECURRENCE;
						
						int dayCount = 0;
						int days = newRecurrency->days;
						for (int i = 0; i < 7; i++)
						{
							if (days & 1)
							{
								type.by_day[dayCount++] = i + 1;	// Sunday = 1, ...
							}
							days >>= 1;
						}

						if (newRecurrency->repeatType == RECURRENCY_REPEAT_COUNT)
						{
							type.count *= dayCount;
						}
					}
					break;

				case RECURRENCY_TYPE_MONTHLY_NTH_DAY:
					type.freq = ICAL_MONTHLY_RECURRENCE;
					break;

				case RECURRENCY_TYPE_MONTHLY_NTH_WEEKDAY:
					type.freq = ICAL_MONTHLY_RECURRENCE;
					FileTimeToSystemTime(&newEvent->startTime, &sysTime);
					type.by_day[0] = (((sysTime.wDay - 1) / 7 + 1) * 8) + (sysTime.wDayOfWeek + 1);
					break;

				case RECURRENCY_TYPE_MONTHLY_LASTWEEK_WEEKDAY:
					type.freq = ICAL_MONTHLY_RECURRENCE;
					FileTimeToSystemTime(&newEvent->startTime, &sysTime);
					type.by_day[0] = -(sysTime.wDayOfWeek + 9);
					break;

				case RECURRENCY_TYPE_MONTHLY_LAST_DAY:
					type.freq = ICAL_MONTHLY_RECURRENCE;
					type.by_month_day[0] = -1;
					break;

				case RECURRENCY_TYPE_MONTHLY_FIRST_WEEKDAY:
					type.freq = ICAL_MONTHLY_RECURRENCE;
					type.by_day[0] = 2;
					type.by_day[1] = 3;
					type.by_day[2] = 4;
					type.by_day[3] = 5;
					type.by_day[4] = 6;
					type.by_set_pos[0] = 1;
					break;

				case RECURRENCY_TYPE_MONTHLY_LAST_WEEKDAY:
					type.freq = ICAL_MONTHLY_RECURRENCE;
					type.by_day[0] = 2;
					type.by_day[1] = 3;
					type.by_day[2] = 4;
					type.by_day[3] = 5;
					type.by_day[4] = 6;
					type.by_set_pos[0] = -1;
					break;

				case RECURRENCY_TYPE_YEARLY:
					type.freq = ICAL_YEARLY_RECURRENCE;
					break;

				default: // RECURRENCY_TYPE_SINGLE:
					type.freq = ICAL_NO_RECURRENCE;
					break;
			}
			icalproperty_set_rrule(recurProp, type);
		}
	}
	else if (item->type == RAINLENDAR_TYPE_TODO)
	{
		RainlendarTodo* newTodo = (RainlendarTodo*)item;

		if (newTodo->message && strlen(newTodo->message) > 0)
		{
			icalcomponent_set_summary(component, ConvertStringToUTF8(newTodo->message));
		}
		else
		{
			icalcomponent_set_summary(component, "");		// TODO: should remove the property instead
		}

		if (newTodo->checked)
		{
			icalcomponent_set_status(component, ICAL_STATUS_COMPLETED);
		}
		else
		{
			icalcomponent_set_status(component, ICAL_STATUS_INPROCESS);
		}

		// If there is X-RAINLENDAR-POSITION, update that.
		char buffer[10];
		itoa(newTodo->position, buffer, 10);

		bool posOK = false;
		icalproperty* posProp = icalcomponent_get_first_x_property(component, "X-RAINLENDAR-POSITION");
		if (posProp) 
		{
			icalvalue* val = icalproperty_get_value(posProp);
			if (val)
			{
				icalvalue_set_x(val, buffer);
				posOK = true;
			}
		}

		if (!posOK) 
		{
			// Create a new property
			icalproperty* posProp = icalproperty_new(ICAL_X_PROPERTY);
			icalproperty_set_x_name(posProp, "X-RAINLENDAR-POSITION");
			icalproperty_set_x(posProp, buffer);
			icalcomponent_add_property(component, posProp);
		}
	}

	icalcomponent_strip_errors(component);
}

int UpdateEvent(icalcomponent* component, RainlendarItem** items)
{
	const char* iCalGuid = icalcomponent_get_uid(component);

	GUID guid;
	if (DecodeGUID(&guid, iCalGuid))
	{
		int i = 0;
		while (items[i] != NULL)
		{
			if (items[i] != (RainlendarItem*)-1)
			{
				// Compare the GUIDs
				if (IsEqualGUID(guid, (items[i]->guid)))
				{
					// Handle deleted events
					if (items[i]->deleted || (items[i]->type == RAINLENDAR_TYPE_TODO && ((RainlendarTodo*)items[i])->todoType == TODO_TYPE_DONE))
					{
						g_DeletedComponents.push_back(component);
					}
					else
					{
						// Found match -> Update the data
						UpdateEventData(component, items[i], false);
					}

					// Mark the item as handled
					items[i] = (RainlendarItem*)-1;

					return 1;
				}
			}
			i++;
		}
	}

	// Unable to decode the GUID or match wasn't found
	return 0;
}

bool ExportEvents(LPCTSTR eventFile, RainlendarItem** items, bool merge)
{
	if (eventFile == NULL || items == NULL) return false;

	icalset* store = NULL;

	if (!merge) 
	{
		DeleteFile(eventFile);
	}

	store = icalfileset_new(eventFile);
	if (store)
	{
		int eventCount = 0;
		int todoCount = 0;
		icalcomponent* lastCalendar = NULL;
		icalcomponent* calendar = NULL;

		// Scan the events and update those that which are found

		for (calendar = icalfileset_get_first_component(store);
			 calendar != 0;
			 calendar = icalfileset_get_next_component(store))
		{
			if (icalcomponent_isa(calendar) == ICAL_VCALENDAR_COMPONENT)
			{
				lastCalendar = calendar;

				icalcomponent* component;

				for (component = icalcomponent_get_first_component(calendar, ICAL_VEVENT_COMPONENT);
					 component != 0;
					 component = icalcomponent_get_next_component(calendar, ICAL_VEVENT_COMPONENT))
				{
					eventCount += UpdateEvent(component, items);
				}

				for (component = icalcomponent_get_first_component(calendar, ICAL_VTODO_COMPONENT);
					 component != 0;
					 component = icalcomponent_get_next_component(calendar, ICAL_VTODO_COMPONENT))
				{
					todoCount += UpdateEvent(component, items);
				}
			}
			else if (icalcomponent_isa(calendar) == ICAL_VEVENT_COMPONENT)
			{
				eventCount += UpdateEvent(calendar, items);
			}
			else if (icalcomponent_isa(calendar) == ICAL_VTODO_COMPONENT)
			{
				todoCount += UpdateEvent(calendar, items);
			}
		}

		// Delete the deleted components
		std::list<icalcomponent*>::iterator iter = g_DeletedComponents.begin();
		for ( ; iter != g_DeletedComponents.end(); iter++)
		{
			icalcomponent* parent = icalcomponent_get_parent(*iter);
			icalcomponent_remove_component(parent, *iter);
			icalcomponent_free(*iter);
		}
		g_DeletedComponents.clear();

		// Create new events/todos for the rest of the items

		if (lastCalendar == NULL)
		{
			// Need to create a new VCALENDAR
			lastCalendar = icalcomponent_new(ICAL_VCALENDAR_COMPONENT);
			icalfileset_add_component(store, lastCalendar);
		}
		
		int i = 0;
		while (items[i] != NULL)
		{
			if (items[i] != (RainlendarItem*)-1)
			{
				if (!items[i]->deleted)
				{
					icalcomponent* component = NULL;

					// Create new event
					if (items[i]->type == RAINLENDAR_TYPE_EVENT)
					{
						eventCount++;
						component = icalcomponent_new(ICAL_VEVENT_COMPONENT);
					}
					else if (items[i]->type == RAINLENDAR_TYPE_TODO && (((RainlendarTodo*)items[i])->todoType == TODO_TYPE_TODO))
					{
						todoCount++;
						component = icalcomponent_new(ICAL_VTODO_COMPONENT);
					}

					if (component)
					{
						UpdateEventData(component, items[i], true);
						icalcomponent_add_component(lastCalendar, component);
					}
				}
			}
			i++;
		}

		if (todoCount > 0 || eventCount > 0)
		{
			icalfileset_mark(store);
		}

		DebugLog("%s: Wrote %i VEVENTs and %i VTODOs to file \"%s\"", PLUGIN_NAME, eventCount, todoCount, eventFile);

		icalfileset_free(store);
	}
	else
	{
		DebugLog("%s: Unable to open file \"%s\"", PLUGIN_NAME, eventFile);
		return false;
	}

	return true;
}

//******************************************************************
//
//                 FILE MONITORING STUFF BELOW
//
//******************************************************************

void StartMonitor()
{
	if (g_MonitorChanges && g_Thread == NULL)
	{
		// Launch the network communication thread
		DWORD threadId;
		g_Thread = CreateThread(NULL, 0, MonitorThreadProc, NULL, 0, &threadId);
	}
}

bool EndMonitor()
{
	if (g_Thread) 
	{
		 // Trigger thread to stop
		::SetEvent(g_ThreadStopEvent);

		// Wait until thread finished
		if (WAIT_TIMEOUT == WaitForSingleObject(g_ThreadWaitEvent, 1000))		// Just wait 1 sec to prevent hanging
		{
			DebugLog("%s: System monitor thread terminated", PLUGIN_NAME);
			TerminateThread(g_Thread, 0);
		}
		else
		{
			DebugLog("%s: Stopped the file system monitor thread", PLUGIN_NAME);
		}

		CloseHandle(g_Thread);
		g_Thread = NULL;
		return true;
	}
	return false;
}

DWORD WINAPI MonitorThreadProc(LPVOID pParam)
{
	std::vector<HANDLE> changeHandles;
	std::vector<DWORD> changeSizes;
    DWORD dwWaitStatus;
	char buffer[MAX_PATH];

	DebugLog("%s: Started the file system monitor thread", PLUGIN_NAME);

	int size = g_EventFiles.size();

	for (int i = 0; i < size; i++)
	{
		HANDLE file = CreateFile(g_EventFiles[i].filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file != INVALID_HANDLE_VALUE)
		{
			DWORD fileSize;
			fileSize = GetFileSize(file, NULL);
			changeSizes.push_back(fileSize);
			CloseHandle(file);

//			DebugLog("ICAL DEBUG: [%i] %s: %i", i,  g_EventFiles[i].filename.c_str(), fileSize);	// DEBUG

			strcpy(buffer, g_EventFiles[i].filename.c_str());
			char* slash = strrchr(buffer, '\\');
			*(slash + 1) = 0;	// Cut the file from the name
			HANDLE handle = FindFirstChangeNotification(buffer, FALSE, FILE_NOTIFY_CHANGE_SIZE);
			changeHandles.push_back(handle);
		}
		else
		{
			DebugLog("%s: File \"%s\" cannot be opened.", PLUGIN_NAME, g_EventFiles[i].filename.c_str());
		}
	}

	if (changeHandles.size() > 0)
	{
		changeHandles.push_back(g_ThreadStopEvent);		// The last event quits the 

		int size = changeHandles.size();

		while(TRUE)
		{
			bool refresh = false;
			HANDLE* handles = changeHandles.begin();
			dwWaitStatus = WaitForMultipleObjects(size, handles, FALSE, INFINITE);

//			DebugLog("%s: Something happened!", PLUGIN_NAME);	// DEBUG

			if (dwWaitStatus >= WAIT_OBJECT_0 && dwWaitStatus < WAIT_OBJECT_0 + size - 1)
			{
				Sleep(500);		// Wait some time so that the writing is finished

				// Verify that it was the file that changed
				int i = dwWaitStatus - WAIT_OBJECT_0;
				HANDLE file = CreateFile(g_EventFiles[i].filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (file != INVALID_HANDLE_VALUE)
				{
					DWORD fileSize;
					fileSize = GetFileSize(file, NULL);
					CloseHandle(file);

					if (fileSize != changeSizes[i] && fileSize != -1 && changeSizes[i] != -1)
					{
						DebugLog("%s: Noticed change in the file system -> Refresh", PLUGIN_NAME);
//						DebugLog("ICAL DEBUG: [%i] %s %i <-> %i", i, g_EventFiles[i].filename.c_str(), fileSize, changeSizes[i]);	// DEBUG

						changeSizes[i] = fileSize;
						refresh = true;
					}
				}

				FindNextChangeNotification(changeHandles[i]);
			}
			else if (dwWaitStatus == WAIT_OBJECT_0 + size - 1)
			{
				break;	// Stop the thread
			}

			if (refresh)
			{
				Rainlendar_Refresh(0);		// Refresh Rainlendar
			}
		}

		// NOTE: When the thread is terminated the handles are not closed -> resource leak

		for (int i = 0; i < changeHandles.size() - 1; i++)
		{
			FindCloseChangeNotification(changeHandles[i]);
		}
	}
	else
	{
		DebugLog("%s: No files -> Nothing to monitor.", PLUGIN_NAME);
	}

	SetEvent(g_ThreadWaitEvent);

	return 0;
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

LPCTSTR EncodeGUID(GUID* guid)
{
	static TCHAR buffer[256];	// 256 should be enough...
	WCHAR* wStr = NULL;
	HRESULT hr = StringFromCLSID(*guid, &wStr);

	buffer[0] = 0;
	
	if (SUCCEEDED(hr))
	{
		// Remove first and last '{'
		wStr[0] = L' ';
		wStr[wcslen(wStr) - 1] = 0;

#ifndef UNICODE
		if (WideCharToMultiByte(CP_ACP, 0, wStr, -1, buffer, 256, NULL, NULL) == 0)
		{
			// Failed
			CoTaskMemFree(wStr);
			return NULL;
		}
#else
		_tcscpy(buffer, wStr);
#endif
		CoTaskMemFree(wStr);
	}
	return buffer + 1;
}

bool DecodeGUID(GUID* guid, LPCTSTR str)
{
	WCHAR* wStr;

#ifndef UNICODE
	int len = _tcslen(str) + 1;
	wStr = new WCHAR[len * 2];
	if (MultiByteToWideChar(CP_ACP, 0, str, len, wStr, len * 2) == 0)
	{
		return false;
	}
#else
	wStr = str;
#endif

	HRESULT hr;

	if (wStr[0] != L'{')
	{
		WCHAR* wStr2 = new WCHAR[wcslen(wStr) + 3];
		wcscpy(wStr2, L"{");
		wcscat(wStr2, wStr);
		wcscat(wStr2, L"}");
		hr = CLSIDFromString(wStr2, guid);
		delete [] wStr2;
	}
	else
	{
		hr = CLSIDFromString(wStr, guid);
	}

#ifndef UNICODE
	delete [] wStr;
#endif

	if (SUCCEEDED(hr))
	{
		return true;
	}
	return false;
}

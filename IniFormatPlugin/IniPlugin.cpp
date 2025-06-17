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
  $Header: /home/cvsroot/Rainlendar/IniFormatPlugin/IniPlugin.cpp,v 1.2 2004/12/05 18:16:27 rainy Exp $

  $Log: IniPlugin.cpp,v $
  Revision 1.2  2004/12/05 18:16:27  rainy
  Timestamp is set when importing.
  Todo has profile.

  Revision 1.1  2004/11/06 14:04:44  rainy
  Initial version

*/

#pragma warning(disable: 4786)

#include "IniPlugin.h"
#include "resource.h"
#include <time.h>

LPCTSTR EncodeGUID(GUID* guid);
bool DecodeGUID(GUID* guid, LPTSTR str);
void ReadSettings(LPCTSTR configFile);
void WriteSettings(LPCTSTR configFile);
bool ImportEvents(LPCTSTR eventFile, bool readOnly, bool setID);
void ExportEvents(LPCTSTR eventFile, RainlendarItem** events);
bool ImportTodos(LPCTSTR todoFile, bool readOnly, bool setID);
void ExportTodos(LPCTSTR todoFile, RainlendarItem** todos);

std::vector<INIFILENAME> g_EventFiles;
std::vector<INIFILENAME> g_TodoFiles;
HINSTANCE g_Instance = NULL;
UINT g_ID = 0;
const UINT FILEVERSION = 1;
static std::string g_ConfigFile;

void Plugin_Initialize(HINSTANCE instance, LPCTSTR configFile, UINT id)
{
	g_Instance = instance;
	g_ID = id;
	g_ConfigFile = configFile;
}

void Plugin_Finalize()
{
	// Nothing to do
}

void Plugin_Enable()
{
	// Read the settings when the plugin is enabled or the Rainlendar is refreshed
	ReadSettings(g_ConfigFile.c_str());
}

void Plugin_Disable()
{
	// No need to do anything
}

void Plugin_ReadItems()
{
	int i;

	for (i = 0; i < g_EventFiles.size(); i++)
	{
		ImportEvents(g_EventFiles[i].filename.c_str(), g_EventFiles[i].readOnly, true);
	}

	for (i = 0; i < g_TodoFiles.size(); i++)
	{
		ImportTodos(g_TodoFiles[i].filename.c_str(), g_TodoFiles[i].readOnly, true);
	}
}

LPCTSTR Plugin_GetName()
{
	return _T("Rainlendar Ini-format");
}

LPCTSTR Plugin_GetAuthor()
{
	return _T("Rainy (rainy@iki.fi)");
}

UINT Plugin_GetVersion()
{
	return 1002;	// Rainlendar 0.21
}

void Plugin_ItemNotification(GUID** guid)
{
	int i = 0;
	bool todo = false;
	bool event = false;

	while (guid[i] != NULL)
	{
		RainlendarItem* item = Rainlendar_GetItem(guid[i]);
		if (item)
		{
			if (item->type == RAINLENDAR_TYPE_EVENT)
			{
				event = true;
			}
			else if (item->type == RAINLENDAR_TYPE_TODO)
			{
				todo = true;
			}
		}

		i++;
	}

	// TODO: Only write events if there are events (same for todos)

	// Just write'em all
	if (event)
	{
		for (i = 0; i < g_EventFiles.size(); i++)
		{
			if (!g_EventFiles[i].readOnly)
			{
				RainlendarItem** items = Rainlendar_GetAllItems(RAINLENDAR_TYPE_EVENT);
				ExportEvents(g_EventFiles[i].filename.c_str(), items);
				delete [] items;
			}
		}
	}

	// Just write'em all
	if (todo)
	{
		for (i = 0; i < g_TodoFiles.size(); i++)
		{
			if (!g_TodoFiles[i].readOnly)
			{
				RainlendarItem** items = Rainlendar_GetAllItems(RAINLENDAR_TYPE_TODO);
				ExportTodos(g_TodoFiles[i].filename.c_str(), items);
				delete [] items;
			}
		}
	}
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
	ReadSettings(configFile);

	if (DialogBox(g_Instance, MAKEINTRESOURCE(IDD_SETUP), hwndParent, (DLGPROC)SetupProc) == IDOK) 
	{
		WriteSettings(configFile);
		// Return true if something has changed and all events need to be reloaded.
		return true;
	}
	return false;
}

void Plugin_ExportItems(LPCTSTR eventFile, GUID** guids, RAINLENDAR_TYPE type)
{
	std::tstring file = eventFile;

	// Make sure that the file has .ini extension
	bool extOK = false;
	if (file.size() >= 4)
	{
		int pos = file.rfind(_T('.'));
		if (pos != -1)
		{
			std::tstring ext = file.substr(pos);
			if (_tcsicmp(ext.c_str(), _T(".ini")) == 0)
			{
				extOK = true;
			}
		}
	}

	if (!extOK)
	{
		file += _T(".ini");
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

		if (type == RAINLENDAR_TYPE_EVENT)
		{
			ExportEvents(file.c_str(), items);
		}
		else if (type == RAINLENDAR_TYPE_TODO)
		{
			ExportTodos(file.c_str(), items);
		}

		delete [] items;
	}
}

void ExportEvents(LPCTSTR eventFile, RainlendarItem** events)
{
	// TODO: Make writing more robust (take a copy first)

	HANDLE fileHandle = CreateFile(eventFile, GENERIC_WRITE, NULL, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle && events)
	{
		DWORD written;
		char buffer[256];
		
		WriteFile(fileHandle, _T("[Rainlendar]\n"), 13, &written, NULL);
		_stprintf(buffer, _T("Version=%i\n"), FILEVERSION);
		WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

		int i = 0;
		while (events[i] != NULL)
		{
			LPCTSTR data;			
			SYSTEMTIME sysTime;
			if (events[i]->type == RAINLENDAR_TYPE_EVENT)
			{
				RainlendarEvent* event = (RainlendarEvent*)events[i];

				FileTimeToSystemTime(&(event->startTime), &sysTime);
				_stprintf(buffer, _T("[%i-%i-%i]\n"), sysTime.wDay, sysTime.wMonth, sysTime.wYear);
				WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

				// GUID
				_tcscpy(buffer, _T("GUID="));
				WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
				data = EncodeGUID(&(event->guid));
				WriteFile(fileHandle, data, _tcslen(data), &written, NULL);
				WriteFile(fileHandle, _T("\n"), 1, &written, NULL);		// Write end of line

				if (!event->deleted)
				{
					// StartTime
					_stprintf(buffer, _T("StartTime=%u:%u\n"), event->startTime.dwHighDateTime, event->startTime.dwLowDateTime);
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

					// EndTime
					_stprintf(buffer, _T("EndTime=%u:%u\n"), event->endTime.dwHighDateTime, event->endTime.dwLowDateTime);
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

					// Header
					if (event->header)
					{
						_tcscpy(buffer, _T("Header="));
						WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
						data = event->header;
						WriteFile(fileHandle, data, _tcslen(data), &written, NULL);
						_tcscpy(buffer, _T("\n"));
						WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
					}

					// Message
					if (event->message)
					{
						// Change \r\n to "\n"
						std::string message = event->message;
						int pos = message.find("\r\n");
						while (pos != -1)
						{
							message.replace(message.begin() + pos, message.begin() + pos + 2, "\\n");
							pos = message.find("\r\n", pos);
						}

						_tcscpy(buffer, _T("Message="));
						WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
						data = message.c_str();
						WriteFile(fileHandle, data, _tcslen(data), &written, NULL);
						_tcscpy(buffer, _T("\n"));
						WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
					}

					// AllDayEvent
					_stprintf(buffer, _T("AllDayEvent=%u\n"), event->allDayEvent ? 1 : 0);
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

					// Alarm
					_stprintf(buffer, _T("Alarm=%i\n"), event->alarm);
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

					if (event->recurrency)
					{
						RainlendarRecurrency* recurrency = event->recurrency;

						// Recurrency type
						_stprintf(buffer, _T("RecurrencyType=%u\n"), recurrency->type);
						WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
				
						// Recurrency frequency
						_stprintf(buffer, _T("RecurrencyFrequency=%u\n"), recurrency->frequency);
						WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

						// Recurrency days
						_stprintf(buffer, _T("RecurrencyDays=%u\n"), recurrency->days);
						WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

						// Recurrency repeatType
						_stprintf(buffer, _T("RecurrencyRepeatType=%u\n"), recurrency->repeatType);
						WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

						// Recurrency count
						_stprintf(buffer, _T("RecurrencyCount=%u\n"), recurrency->count);
						WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

						// Recurrency until
						_stprintf(buffer, _T("RecurrencyUntil=%u:%u\n"), recurrency->until.dwHighDateTime, recurrency->until.dwLowDateTime);
						WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
					}
				}
				else
				{
					// Deleted
					_tcscpy(buffer, _T("Deleted=1\n"));
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
				}

				// Profile
				if (event->profile)
				{
					_tcscpy(buffer, _T("Profile="));
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
					data = event->profile;
					WriteFile(fileHandle, data, _tcslen(data), &written, NULL);
					_tcscpy(buffer, _T("\n"));
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
				}

				// TimeStamp
				_stprintf(buffer, _T("TimeStamp=%u\n"), event->timeStamp);
				WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
			}

			i++;
		}

		CloseHandle(fileHandle);

		_stprintf(buffer, _T("%i"), i);
		std::tstring err = PLUGIN_NAME;
		err += _T(": Wrote ");
		err += buffer;
		err += _T(" events to: ");
		err += eventFile;
		Rainlendar_Log(err.c_str());
	}
	else
	{
		std::tstring err = PLUGIN_NAME;
		err += _T(": Unable to create file: ");
		err += eventFile;
		Rainlendar_Log(err.c_str());
	}
}

void ExportTodos(LPCTSTR todoFile, RainlendarItem** todos)
{
	HANDLE fileHandle = CreateFile(todoFile, GENERIC_WRITE, NULL, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle && todos)
	{
		char buffer[256];
		DWORD written;
		
		WriteFile(fileHandle, _T("[Rainlendar]\n"), 13, &written, NULL);
		_stprintf(buffer, _T("Version=%i\n"), FILEVERSION);
		WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

		int i = 0;
		while (todos[i] != NULL)
		{
			LPCTSTR data;			

			if (todos[i]->type == RAINLENDAR_TYPE_TODO)
			{
				RainlendarTodo* todo = (RainlendarTodo*)todos[i];

				_stprintf(buffer, _T("[%i]\n"), i);
				WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

				// GUID
				_tcscpy(buffer, _T("GUID="));
				WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
				data = EncodeGUID(&(todo->guid));
				WriteFile(fileHandle, data, _tcslen(data), &written, NULL);
				WriteFile(fileHandle, _T("\n"), 1, &written, NULL);		// Write end of line

				if (!todo->deleted)
				{
					// Message
					if (todo->message)
					{
						_tcscpy(buffer, _T("Message="));
						WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
						data = todo->message;
						WriteFile(fileHandle, data, _tcslen(data), &written, NULL);
						_tcscpy(buffer, _T("\n"));
						WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
					}

					// Position
					_stprintf(buffer, _T("Position=%u\n"), todo->position);
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

					// Type
					_stprintf(buffer, _T("Type=%u\n"), todo->todoType);
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

					// Checked
					_stprintf(buffer, _T("Checked=%u\n"), todo->checked);
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
				}
				else
				{
					// Deleted
					_tcscpy(buffer, _T("Deleted=1\n"));
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
				}

				// TimeStamp
				_stprintf(buffer, _T("TimeStamp=%u\n"), todo->timeStamp);
				WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);

				// Profile
				if (todo->profile)
				{
					_tcscpy(buffer, _T("Profile="));
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
					data = todo->profile;
					WriteFile(fileHandle, data, _tcslen(data), &written, NULL);
					_tcscpy(buffer, _T("\n"));
					WriteFile(fileHandle, buffer, _tcslen(buffer), &written, NULL);
				}
			}

			i++;
		}

		CloseHandle(fileHandle);

		_stprintf(buffer, _T("%i"), i);
		std::tstring err = PLUGIN_NAME;
		err += _T(": Wrote ");
		err += buffer;
		err += _T(" todos to: ");
		err += todoFile;
		Rainlendar_Log(err.c_str());
	}
	else
	{
		std::tstring err = PLUGIN_NAME;
		err += _T(": Unable to create file: ");
		err += todoFile;
		Rainlendar_Log(err.c_str());
	}
}

bool Plugin_ImportItems(LPCTSTR eventFile, RAINLENDAR_TYPE type)
{
	if (type == RAINLENDAR_TYPE_EVENT)
	{
		return ImportEvents(eventFile, false, false);
	}
	else if (type == RAINLENDAR_TYPE_TODO)
	{
		return ImportTodos(eventFile, false, false);
	}
	return false;
}

bool ImportEvents(LPCTSTR eventFile, bool readOnly, bool setID)
{
	if (eventFile == NULL) return false;

	// Make sure the file has .ini extension
	if (_tcslen(eventFile) > 4)
	{
		LPCTSTR pos = _tcsrchr(eventFile, _T('.'));
		if (pos)
		{
			if (_tcsicmp(pos, _T(".ini")) != 0)
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

	int version = 0;

	HANDLE fileHandle = CreateFile(eventFile, GENERIC_READ, NULL, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle)
	{
		DWORD fileSize = GetFileSize(fileHandle, 0);

		int count = 0;
		if (fileSize > 0)
		{
			bool header = false;

			DWORD readBytes;
			TCHAR* pBuffer = new TCHAR[fileSize + 1];
			if (ReadFile(fileHandle, pBuffer, fileSize, &readBytes, NULL))
			{
				pBuffer[fileSize] = 0;

				RainlendarEvent* newEvent = NULL;
				RainlendarRecurrency* newRecurrency = NULL;
				bool recurrency = false;

				TCHAR* pos = pBuffer;
				while (pos - pBuffer <= fileSize)
				{
					// replace \n with \0
					char* endPos = _tcschr(pos, _T('\n'));
					if (endPos)
					{
						*endPos = 0;
					}

					// Remove also \r
					endPos = _tcschr(pos, _T('\r'));
					if (endPos)
					{
						*endPos = 0;
					}

					// Read the header
					if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 12, _T("[Rainlendar]"), -1))
					{
						header = true;
					}
					else if (header && CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 8, _T("Version="), -1))
					{
						version = _ttoi(pos + 8);
					}

					// Check for new event section
					else if (*pos == _T('['))
					{
						header = false;

						if (newEvent)
						{
							if (recurrency && newRecurrency)
							{
								newEvent->recurrency = newRecurrency;
							}
							else
							{
								delete newRecurrency;
							}
							recurrency = false;

							if (setID)
							{
								Rainlendar_SetItem(newEvent, g_ID);
							}
							else
							{
								// Use 0 as the ID so that this plugin also gets the events
								Rainlendar_SetItem(newEvent, 0);
							}
							Rainlendar_DeleteItem(newEvent);
							newRecurrency = NULL;
							newEvent = NULL;
							count++;
						}

						int day = -1, month = -1, year = -1;
						_stscanf(pos, _T("[%i-%i-%i"), &day, &month, &year);

						if (day != -1 && month != -1 && year != -1)
						{
							newEvent = new RainlendarEvent;
							memset(newEvent, 0, sizeof(RainlendarEvent));
							newEvent->size = sizeof(RainlendarEvent);
							newEvent->readOnly = readOnly;
							newEvent->type = RAINLENDAR_TYPE_EVENT;
							time(&newEvent->timeStamp);
							CoCreateGuid(&newEvent->guid);	// Create GUID just in case it's not in the file

							newRecurrency = new RainlendarRecurrency;
							memset(newRecurrency, 0, sizeof(RainlendarRecurrency));
							newRecurrency->size = sizeof(RainlendarRecurrency);

							// Set startTime correcly (in case it's not defined)
							SYSTEMTIME sysTime;
							memset(&sysTime, 0, sizeof(SYSTEMTIME));
							sysTime.wDay = day;
							sysTime.wMonth = month;
							sysTime.wYear = year;
							SystemTimeToFileTime(&sysTime, &newEvent->startTime);
						} 
					} 
					else if (newEvent)
					{
						if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 5, _T("GUID="), -1))
						{
							DecodeGUID(&newEvent->guid, pos + 5);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 10, _T("StartTime="), -1))
						{
							_stscanf(pos + 10, _T("%u:%u"), &newEvent->startTime.dwHighDateTime, &newEvent->startTime.dwLowDateTime);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 8, _T("EndTime="), -1))
						{
							_stscanf(pos + 8, _T("%u:%u"), &newEvent->endTime.dwHighDateTime, &newEvent->endTime.dwLowDateTime);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 7, _T("Header="), -1))
						{
							newEvent->header = _tcsdup(pos + 7);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 8, _T("Message="), -1))
						{
							// replace \n with newline
							std::string tmpString = pos + 8;
							
							int n = tmpString.find("\\n");
							while (n != -1)
							{
								tmpString.replace(tmpString.begin() + n, tmpString.begin() + n + 2, "\r\n");
								n = tmpString.find("\\n");
							}

							// Trim the message
							tmpString.erase(tmpString.find_last_not_of(" ") + 1);

							if (version == 0)
							{
								// This is an old style event (otherwise the start date would have been found before the message)

								// Parse for time in front of the message
								int hour = -1, min = -1;
								if (2 == sscanf(tmpString.c_str(), "%i:%i", &hour, &min))
								{
									int pos = tmpString.find_first_not_of(" 0123456789:");
									if (pos != -1) 
									{
										if (strnicmp(tmpString.c_str() + pos, "AM", 2) == 0) 
										{
											if (hour == 12) hour = 0;
											pos += 2;
										}
										else if (strnicmp(tmpString.c_str() + pos, "PM", 2) == 0) 
										{
											if (hour == 12) hour = 0;
											hour += 12;
											pos += 2;
										}
									}

									SYSTEMTIME sysTime;
									FileTimeToSystemTime(&newEvent->startTime, &sysTime);
									sysTime.wHour = hour;
									sysTime.wMinute = min;
									SystemTimeToFileTime(&sysTime, &newEvent->startTime);
								
									// Remove the time from the text
									tmpString.erase(0, pos);

									// Trim the message again
									tmpString.erase(tmpString.find_last_not_of(" ") + 1);
								}
								else
								{
									// else make it a all day event
									newEvent->allDayEvent = true;
								}
							}

							newEvent->message = _tcsdup(tmpString.c_str());
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 8, _T("Profile="), -1))
						{
							newEvent->profile = _tcsdup(pos + 8);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 12, _T("AllDayEvent="), -1))
						{
							newEvent->allDayEvent = (1 == _ttoi(pos + 12));
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 6, _T("Alarm="), -1))
						{
							newEvent->alarm = _ttoi(pos + 6);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 8, _T("Deleted="), -1))
						{
							newEvent->deleted = (1 == _ttoi(pos + 8));
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 10, _T("TimeStamp="), -1))
						{
							if (setID)
							{
								newEvent->timeStamp = _ttoi(pos + 10);
							}
							// Else use current time for the stamp
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 15, _T("RecurrencyType="), -1))
						{
							recurrency = true;
							newRecurrency->type = (RECURRENCY_TYPE)_ttoi(pos + 15);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 20, _T("RecurrencyFrequency="), -1))
						{
							newRecurrency->frequency = _ttoi(pos + 20);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 15, _T("RecurrencyDays="), -1))
						{
							newRecurrency->days = _ttoi(pos + 15);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 21, _T("RecurrencyRepeatType="), -1))
						{
							newRecurrency->repeatType = (RECURRENCY_REPEAT)_ttoi(pos + 21);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 16, _T("RecurrencyCount="), -1))
						{
							newRecurrency->count = _ttoi(pos + 16);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 16, _T("RecurrencyUntil="), -1))
						{
							_stscanf(pos + 16, _T("%u:%u"), &newRecurrency->until.dwHighDateTime, &newRecurrency->until.dwLowDateTime);
						}

						// Backwards compatibility support (these are not used anymore)
						if (version == 0)
						{
							if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 5, _T("Type="), -1))
							{
								// If there is a Type, this is an old Events.ini
								recurrency = true;
								switch(_ttoi(pos + 5))
								{
								case 1: // EVENT_DAILY
									newRecurrency->type = RECURRENCY_TYPE_DAILY;
									break;

								case 2: // EVENT_WEEKLY
									newRecurrency->type = RECURRENCY_TYPE_WEEKLY;
									
									// Check the weekday
									SYSTEMTIME sysTime;
									FileTimeToSystemTime(&newEvent->startTime, &sysTime);
									newRecurrency->days = 1 << sysTime.wDayOfWeek;
									break;

								case 3: // EVENT_MONTHLY
									newRecurrency->type = RECURRENCY_TYPE_MONTHLY_NTH_DAY;
									break;

								case 4:// EVENT_ANNUALLY
									newRecurrency->type = RECURRENCY_TYPE_YEARLY;
									break;

								default: // EVENT_SINGLE
									recurrency = false;
									break;
								}
							}
							else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 11, _T("ValidUntil="), -1))
							{
								int validDate = _ttoi(pos + 11);
								if (validDate != 0)
								{
									SYSTEMTIME sysTime;
									memset(&sysTime, 0, sizeof(SYSTEMTIME));
									sysTime.wDay = validDate % 100;
									sysTime.wMonth = (validDate / 100) % 100;
									sysTime.wYear = (validDate / 10000) % 10000;
									SystemTimeToFileTime(&sysTime, &newRecurrency->until);

									// Make it valid until the end of the day
									ULARGE_INTEGER value1, value2;
									value1.LowPart = newRecurrency->until.dwLowDateTime;
									value1.HighPart = newRecurrency->until.dwHighDateTime;
									value2.QuadPart = (24 * 60 * 60 - 1);
									value2.QuadPart *= 10000000;
									value1.QuadPart += value2.QuadPart;
									newRecurrency->until.dwLowDateTime = value1.LowPart;
									newRecurrency->until.dwHighDateTime = value1.HighPart;

									newRecurrency->repeatType = RECURRENCY_REPEAT_UNTIL;
								}
								else
								{
									newRecurrency->repeatType = RECURRENCY_REPEAT_FOREVER;
								}
							}
							else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 9, _T("EveryNth="), -1))
							{
								newRecurrency->frequency = _ttoi(pos + 9);
							}
						}
					}

					// Goto next line
					pos += _tcslen(pos) + 1;
				}

				if (newEvent)	// Add the last event
				{
					if (recurrency && newRecurrency)
					{
						newEvent->recurrency = newRecurrency;
					}
					else
					{
						delete newRecurrency;
					}

					if (setID)
					{
						Rainlendar_SetItem(newEvent, g_ID);
					}
					else
					{
						// Use 0 as the ID so that this plugin also gets the events
						Rainlendar_SetItem(newEvent, 0);
					}
					Rainlendar_DeleteItem(newEvent);
					newRecurrency = NULL;
					newEvent = NULL;
					count++;
				}

				TCHAR buffer[256];
				_stprintf(buffer, _T("%i"), count);
				std::tstring msg = PLUGIN_NAME;
				msg += _T(": Read ");
				msg += buffer;
				msg += _T(" events from file ");
				msg += eventFile;
				Rainlendar_Log(msg.c_str());
			}
			else
			{
				std::tstring err = PLUGIN_NAME;
				err += _T(": Read error with file: ");
				err += eventFile;
				Rainlendar_Log(err.c_str());
			}

			delete [] pBuffer;
		}

		CloseHandle(fileHandle);
	}
	else
	{
		std::tstring err = PLUGIN_NAME;
		err += _T(": File not found: ");
		err += eventFile;
		Rainlendar_Log(err.c_str());
	}

	return true;
}

bool ImportTodos(LPCTSTR todoFile, bool readOnly, bool setID)
{
	if (todoFile == NULL) return false;

	// Make sure the file has .ini extension
	if (_tcslen(todoFile) > 4)
	{
		LPCTSTR pos = _tcsrchr(todoFile, _T('.'));
		if (pos)
		{
			if (_tcsicmp(pos, _T(".ini")) != 0)
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

	HANDLE fileHandle = CreateFile(todoFile, GENERIC_READ, NULL, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle)
	{
		DWORD fileSize = GetFileSize(fileHandle, 0);

		int count = 0;
		if (fileSize > 0)
		{
			DWORD readBytes;
			TCHAR* pBuffer = new TCHAR[fileSize + 1];
			if (ReadFile(fileHandle, pBuffer, fileSize, &readBytes, NULL))
			{
				bool valid = false;
				pBuffer[fileSize] = 0;

				RainlendarTodo* newTodo = NULL;
				TCHAR* pos = pBuffer;
				while (pos - pBuffer <= fileSize)
				{
					// replace \n with \0
					char* endPos = _tcschr(pos, _T('\n'));
					if (endPos)
					{
						*endPos = 0;
					}

					// Remove also \r
					endPos = _tcschr(pos, _T('\r'));
					if (endPos)
					{
						*endPos = 0;
					}

					// Check for new todo section
					if (*pos == _T('['))
					{
						if (newTodo)
						{
							if (valid)
							{
								if (setID)
								{
									Rainlendar_SetItem(newTodo, g_ID);
									count++;
								}
								else
								{
									// Use 0 as the ID so that this plugin also gets the todos
									Rainlendar_SetItem(newTodo, 0);
									count++;
								}
							}
							Rainlendar_DeleteItem(newTodo);
							newTodo = NULL;
						}

						newTodo = new RainlendarTodo;
						memset(newTodo, 0, sizeof(RainlendarTodo));
						newTodo->size = sizeof(RainlendarTodo);
						newTodo->readOnly = readOnly;
						newTodo->type = RAINLENDAR_TYPE_TODO;
						time(&newTodo->timeStamp);
						CoCreateGuid(&newTodo->guid);	// Create GUID just in case it's not in the file

						valid = false;
					} 
					else if (newTodo)
					{
						if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 5, _T("GUID="), -1))
						{
							DecodeGUID(&newTodo->guid, pos + 5);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 8, _T("Message="), -1))
						{
							newTodo->message = _tcsdup(pos + 8);
							valid = true;
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 9, _T("Position="), -1))
						{
							newTodo->position = _ttoi(pos + 9);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 5, _T("Type="), -1))
						{
							newTodo->todoType = (TODO_TYPE)_ttoi(pos + 5);
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 8, _T("Deleted="), -1))
						{
							newTodo->deleted = (1 == _ttoi(pos + 8));
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 8, _T("Checked="), -1))
						{
							newTodo->checked = (1 == _ttoi(pos + 8));
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 10, _T("TimeStamp="), -1))
						{
							if (setID)
							{
								newTodo->timeStamp = _ttoi(pos + 10);
							}
							// else Use current time for the stamp
						}
						else if (CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, pos, 8, _T("Profile="), -1))
						{
							newTodo->profile = _tcsdup(pos + 8);
						}
					}

					// Goto next line
					pos += _tcslen(pos) + 1;
				}

				if (newTodo)	// Add the last event
				{
					if (valid)
					{
						if (setID)
						{
							Rainlendar_SetItem(newTodo, g_ID);
							count++;
						}
						else
						{
							// Use 0 as the ID so that this plugin also gets the events
							Rainlendar_SetItem(newTodo, 0);
							count++;
						}
					}
					Rainlendar_DeleteItem(newTodo);
					newTodo = NULL;
				}

				TCHAR buffer[256];
				_stprintf(buffer, _T("%i"), count);
				std::tstring msg = PLUGIN_NAME;
				msg += _T(": Read ");
				msg += buffer;
				msg += _T(" todos from file ");
				msg += todoFile;
				Rainlendar_Log(msg.c_str());
			}
			else
			{
				std::tstring err = PLUGIN_NAME;
				err += _T(": Read error with file: ");
				err += todoFile;
				Rainlendar_Log(err.c_str());
			}

			delete [] pBuffer;
		}

		CloseHandle(fileHandle);
	}
	else
	{
		std::tstring err = PLUGIN_NAME;
		err += _T(": File not found: ");
		err += todoFile;
		Rainlendar_Log(err.c_str());
	}

	return true;
}

void ReadSettings(LPCTSTR configFile)
{
	TCHAR buffer[MAX_PATH];
	int i;

	g_EventFiles.clear();
	g_TodoFiles.clear();

	// Read settings from configFile
	int count = GetPrivateProfileInt(PLUGIN_NAME, _T("EventIniFileCount"), -1, configFile);
	
	for (i = 0; i < count; i++)
	{
		_stprintf(buffer, _T("EventIniFile%i"), i + 1);
		if(GetPrivateProfileString(PLUGIN_NAME, buffer, NULL, buffer, MAX_PATH, configFile) > 0) 
		{
			INIFILENAME iniFileName;
			iniFileName.filename = buffer;

			_stprintf(buffer, _T("EventReadOnly%i"), i + 1);
			iniFileName.readOnly = (1 == GetPrivateProfileInt(PLUGIN_NAME, buffer, 0, configFile));

			g_EventFiles.push_back(iniFileName);
		}
	}

	// Read settings from configFile
	count = GetPrivateProfileInt(PLUGIN_NAME, _T("TodoIniFileCount"), -1, configFile);
	
	for (i = 0; i < count; i++)
	{
		_stprintf(buffer, _T("TodoIniFile%i"), i + 1);
		if(GetPrivateProfileString(PLUGIN_NAME, buffer, NULL, buffer, MAX_PATH, configFile) > 0) 
		{
			INIFILENAME iniFileName;
			iniFileName.filename = buffer;

			_stprintf(buffer, _T("TodoReadOnly%i"), i + 1);
			iniFileName.readOnly = (1 == GetPrivateProfileInt(PLUGIN_NAME, buffer, 0, configFile));

			g_TodoFiles.push_back(iniFileName);
		}
	}

	// if there are no settings, add defaults
	if (g_EventFiles.empty() && g_TodoFiles.empty() && count == -1)
	{
		// No event files -> use default
		INIFILENAME eventsIniFileName;
		INIFILENAME todoIniFileName;

		eventsIniFileName.filename = configFile;

		int pos = eventsIniFileName.filename.rfind(_T('\\'));
		if (pos != -1)
		{
			eventsIniFileName.filename = eventsIniFileName.filename.substr(0, pos + 1);
			todoIniFileName.filename = eventsIniFileName.filename;
			eventsIniFileName.filename += _T("Events.ini");
			todoIniFileName.filename += _T("Todo.ini");
		}
		else
		{
			eventsIniFileName.filename = _T("Events.ini");
			todoIniFileName.filename = _T("Todo.ini");
		}
		eventsIniFileName.readOnly = FALSE;
		todoIniFileName.readOnly = FALSE;

		g_EventFiles.push_back(eventsIniFileName);
		g_TodoFiles.push_back(todoIniFileName);
	}
}

void WriteSettings(LPCTSTR configFile)
{
	int i;
	char tmpSz[256];

	sprintf(tmpSz, "%i", g_EventFiles.size());
	WritePrivateProfileString(PLUGIN_NAME, _T("EventIniFileCount"), tmpSz, configFile);

	for (i = 0; i < g_EventFiles.size(); i++)
	{
		_stprintf(tmpSz, _T("EventIniFile%i"), i + 1);
		WritePrivateProfileString(PLUGIN_NAME, tmpSz, g_EventFiles[i].filename.c_str(), configFile);
		_stprintf(tmpSz, _T("EventReadOnly%i"), i + 1);
		WritePrivateProfileString(PLUGIN_NAME, tmpSz, g_EventFiles[i].readOnly ? _T("1") : _T("0"), configFile);
	}

	sprintf(tmpSz, "%i", g_TodoFiles.size());
	WritePrivateProfileString(PLUGIN_NAME, _T("TodoIniFileCount"), tmpSz, configFile);

	for (i = 0; i < g_TodoFiles.size(); i++)
	{
		_stprintf(tmpSz, _T("TodoIniFile%i"), i + 1);
		WritePrivateProfileString(PLUGIN_NAME, tmpSz, g_TodoFiles[i].filename.c_str(), configFile);
		_stprintf(tmpSz, _T("TodoReadOnly%i"), i + 1);
		WritePrivateProfileString(PLUGIN_NAME, tmpSz, g_TodoFiles[i].readOnly ? _T("1") : _T("0"), configFile);
	}
}


LPCTSTR EncodeGUID(GUID* guid)
{
	static TCHAR buffer[256];	// 256 should be enough...
	WCHAR* wStr = NULL;
	HRESULT hr = StringFromCLSID(*guid, &wStr);

	buffer[0] = 0;

	if (SUCCEEDED(hr))
	{
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
	return buffer;
}

bool DecodeGUID(GUID* guid, LPTSTR str)
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

	HRESULT hr = CLSIDFromString(wStr, guid);

#ifndef UNICODE
	delete [] wStr;
#endif

	if (SUCCEEDED(hr))
	{
		return true;
	}
	return false;
}

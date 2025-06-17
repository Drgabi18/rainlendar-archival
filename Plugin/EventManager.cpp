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
  $Header: /home/cvsroot/Rainlendar/Plugin/EventManager.cpp,v 1.11 2004/12/05 18:32:02 rainy Exp $

  $Log: EventManager.cpp,v $
  Revision 1.11  2004/12/05 18:32:02  rainy
  Fixed GetEvents.
  Added thread-safetyness.

  Revision 1.10  2004/11/06 13:38:58  rainy
  no message

  Revision 1.9  2004/04/24 11:19:10  rainy
  Some methods are now const.

  Revision 1.8  2004/01/10 15:19:31  rainy
  Fixed sorting.

  Revision 1.7  2003/10/27 17:37:13  Rainy
  Config is now singleton.

  Revision 1.6  2003/08/09 16:36:42  Rainy
  Removed event's timestamp is updated.

  Revision 1.5  2003/05/25 18:10:11  Rainy
  The GetEvent() returns now a sorted vector.

  Revision 1.4  2003/05/07 19:11:44  rainy
  The timestamp is not updated every time.

  Revision 1.3  2002/08/24 11:12:15  rainy
  Added some trimming support.

  Revision 1.2  2002/08/10 08:39:51  rainy
  Loading the sections from the ini does not crash anymore if the file is missing.

  Revision 1.1  2002/08/03 16:37:02  rainy
  Intial version.

*/
#pragma warning(disable: 4996)

#include "EventManager.h"
#include "RainlendarDLL.h"
#include <time.h>
#include <set>
#include <algorithm>

// Must be uneven
#define DAY_RANGE 1001

CEventManager::CEventManager()
{
	// Initialize the critical section for m_BufferedAddItems
	InitializeCriticalSection(&m_CriticalSection); 

	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);

	m_StartSortDate = CFileTime(sysTime.wDay, sysTime.wMonth, sysTime.wYear);

	m_PluginManager = NULL;
}

CEventManager::~CEventManager()
{
	FlushBufferedItems(false);		// Make sure that there are no pending events
	Clear();

	DeleteCriticalSection(&m_CriticalSection);
}

void CEventManager::Clear()
{
	std::map<GUID, CEntryEvent*, GUIDComparator>::iterator iter = m_AllEvents.begin();
	for ( ; iter != m_AllEvents.end(); iter++)
	{
		CEntryEvent* event = (*iter).second;
		delete event;
	}

	m_AllEvents.clear();
	m_SortedEvents.clear();

	std::map<GUID, CEntryTodo*, GUIDComparator>::iterator iter2 = m_AllTodos.begin();
	for ( ; iter2 != m_AllTodos.end(); iter2++)
	{
		delete (*iter2).second;
	}

	m_AllTodos.clear();
	m_Todos.clear();
	m_Dones.clear();
	m_Notes.clear();
}

void CEventManager::Initialize(CPluginManager& pluginManager)
{
	m_PluginManager = &pluginManager;

	Clear();

	// Resize the m_SortedEvents to fit 1000 days
	m_SortedEvents.resize(DAY_RANGE);

	const std::vector<CPlugin*>& allPlugins = m_PluginManager->GetPlugins();
	for (UINT i = 0; i < allPlugins.size(); i++)
	{
		allPlugins[i]->ReadItems();
	}

	FlushBufferedItems(false);
}

// Thread safe only if the buffer is true!
bool CEventManager::AddItem(CEntryItem* newItem, bool buffer)
{
	bool result = false;
	if (buffer)
	{
		EnterCriticalSection(&m_CriticalSection); 		// Request ownership of the critical section.
		m_BufferedAddItems.push_back(newItem);
		LeaveCriticalSection(&m_CriticalSection);		// Release ownership of the critical section.
	}
	else
	{
		CEntryItem* item = GetItem(newItem->GetGUID());
		if (item)
		{
			// Item is already in the manager, check if the new one is newer
			if (item->GetTimeStamp() < newItem->GetTimeStamp())
			{
				// Replace and delete the old one
				RemoveItem(item);
				InsertItem(newItem);

				delete item;		// Delete the replaced item
				result = true;
			}
			else
			{
				delete newItem;
				result = false;
			}
		}
		else
		{
			InsertItem(newItem);
			result = true;
		}
	}

	// Inform the plugins about the added/modified/deleted events (Read only events are not sent to the plugins)
	if (result && !newItem->IsReadOnly())
	{
		GUID* guids[2];
		guids[0] = newItem->GetGUID();
		guids[1] = NULL;

		const std::vector<CPlugin*>& allPlugins = m_PluginManager->GetPlugins();
		for (UINT i = 0; i < allPlugins.size(); i++)
		{
			// Unbuffered action comes always from Rainlendar so send it to all plugins
			allPlugins[i]->ItemNotification(guids);
		}
	}

	return result;
}

bool CEventManager::DeleteItem(GUID* guid)
{
	CEntryItem* item = GetEvent(guid);
	if (item == NULL)
	{
		// If it wasn't an event try todos
		item = GetTodo(guid);
	}

	if (item)
	{
		// Mark the event deleted
		item->MarkAsDeleted();
		item->UpdateTimeStamp();

		// Inform the plugins about the added/modified/deleted events (Read only events are not sent to the plugins)
		if (!item->IsReadOnly())
		{
			GUID* guids[2];
			guids[0] = item->GetGUID();
			guids[1] = NULL;

			const std::vector<CPlugin*>& allPlugins = m_PluginManager->GetPlugins();
			for (UINT i = 0; i < allPlugins.size(); i++)
			{
				// Delete-action comes always from Rainlendar so send it to all plugins
				allPlugins[i]->ItemNotification(guids);
			}
		}
		return true;
	}
	return false;
}

bool CEventManager::FlushBufferedItems(bool sendToAll)
{
	// Items from plugins are buffered
	std::vector<CEntryItem*> newItems;
	bool changed = false;
	UINT i, count;

	// Request ownership of the critical section.
	EnterCriticalSection(&m_CriticalSection); 

	// Apply added events
	if (m_BufferedAddItems.size() > 0)
	{
		for (i = 0; i < m_BufferedAddItems.size(); i++)
		{
			CEntryItem* item = GetItem(m_BufferedAddItems[i]->GetGUID());
			if (item)
			{
				// Event is already in the manager, check if the new one is newer
				if (item->GetTimeStamp() < m_BufferedAddItems[i]->GetTimeStamp())
				{
					// Replace and delete the old one
					RemoveItem(item);
					InsertItem(m_BufferedAddItems[i]);

					if (!m_BufferedAddItems[i]->IsReadOnly())
					{
						newItems.push_back(m_BufferedAddItems[i]);
					}

					delete item;		// Delete the replaced item
					changed = true;
				}
				else
				{
					delete m_BufferedAddItems[i];
				}
			}
			else
			{
				InsertItem(m_BufferedAddItems[i]);
				newItems.push_back(m_BufferedAddItems[i]);
				changed = true;
			}
		}
		m_BufferedAddItems.clear();
	}
	
	LeaveCriticalSection(&m_CriticalSection);		// Release ownership of the critical section.

	if (changed)
	{
		// Inform the plugins about the added items
		const std::vector<CPlugin*>& allPlugins = m_PluginManager->GetPlugins();
		for (i = 0; i < allPlugins.size(); i++)
		{
			count = 0;
			GUID** guids = new GUID*[newItems.size() + 1];
			for (UINT j = 0; j < newItems.size(); j++)
			{
				if (sendToAll || newItems[j]->GetPluginID() != allPlugins[i]->GetPluginID())	// Don't include events that came from the plugin
				{
					if (!newItems[j]->IsReadOnly())
					{
						guids[count++] = newItems[j]->GetGUID();
					}
				}
			}
			guids[count] = NULL;

			if (count > 0)
			{
				allPlugins[i]->ItemNotification(guids);
			}

			delete [] guids;
		}
	}

	return changed;
}

void CEventManager::RemoveItem(CEntryItem* item)
{
	if (item->GetType() == RAINLENDAR_TYPE_EVENT)
	{
		CEntryEvent* event = (CEntryEvent*)item;

		std::map<GUID, CEntryEvent*, GUIDComparator>::iterator iter = m_AllEvents.find(*event->GetGUID());
		if (iter != m_AllEvents.end())
		{
			m_AllEvents.erase(iter);
		}
		else
		{
			DebugLog("Warning: Event %s wasn't found from the AllEvents structure!", CEntryItem::GetGUIDAsString(event->GetGUID()));
		}

		CFileTime beginTime = event->GetBegin();
		CFileTime endTime = event->GetEnd();

		int begin = m_StartSortDate.DifferenceInDays(beginTime);
		int end = m_StartSortDate.DifferenceInDays(endTime);

		if (end < begin)
		{
			end = begin;	// No negative events allowed
		}

		begin = begin + DAY_RANGE / 2;
		begin = max(0, begin);
		begin = min(DAY_RANGE - 1, begin);

		end = end + DAY_RANGE / 2;
		end = max(0, end);
		end = min(DAY_RANGE - 1, end);

		bool found = false;
		for (int i = begin; i <= end; i++)
		{
			for (UINT j = 0; j <= m_SortedEvents[i].size(); j++)
			{
				if (m_SortedEvents[i][j] == event)
				{
					found = true;
					m_SortedEvents[i].erase(m_SortedEvents[i].begin() + j);
					break;
				}
			}
		}

		if (!found)
		{
			DebugLog("Warning: Event %s wasn't found from the SortedEvents[%i] structure!",  CEntryItem::GetGUIDAsString(event->GetGUID()), i);
		}
	}
	else if (item->GetType() == RAINLENDAR_TYPE_TODO)
	{
		CEntryTodo* todo = (CEntryTodo*)item;

		std::map<GUID, CEntryTodo*, GUIDComparator>::iterator iter = m_AllTodos.find(*todo->GetGUID());
		if (iter != m_AllTodos.end())
		{
			m_AllTodos.erase(iter);
		}
		else
		{
			DebugLog("Warning: Todo %s wasn't found from the AllTodos structure!", CEntryItem::GetGUIDAsString(item->GetGUID()));
		}

		// Check all vectors (the type might be wrong)
		int i;
		bool found = false;
		for (i = 0; i < (int)m_Todos.size(); i++)
		{
			if (item == m_Todos[i])
			{
				m_Todos.erase(m_Todos.begin() + i);
				found = true;
				break;
			}
		}
		if (!found)
		{
			// Check next vector
			for (i = 0; i < (int)m_Dones.size(); i++)
			{
				if (item == m_Dones[i])
				{
					m_Dones.erase(m_Dones.begin() + i);
					found = true;
					break;
				}
			}
		}
		if (!found)
		{
			// Check next vector
			for (i = 0; i < (int)m_Notes.size(); i++)
			{
				if (item == m_Notes[i])
				{
					m_Notes.erase(m_Notes.begin() + i);
					found = true;
					break;
				}
			}
		}

		if (!found)
		{
			DebugLog("Warning: Todo %s wasn't found from any of the vectors!", CEntryItem::GetGUIDAsString(item->GetGUID()));
		}
	}
}

void CEventManager::InsertItem(CEntryItem* newItem)
{
	if (newItem->GetType() == RAINLENDAR_TYPE_EVENT)
	{
		CEntryEvent* newEvent = (CEntryEvent*)newItem;

		// Safety check
		if (newEvent->GetRecurrency())
		{
			newEvent->GetRecurrency()->frequency = max(newEvent->GetRecurrency()->frequency, 1);
		}

		m_AllEvents[*newEvent->GetGUID()] = newEvent;

		if (!newEvent->IsDeleted())		// No need to add deleted events to the vectors
		{
			// Add it to vector
			CFileTime beginTime = newEvent->GetBegin();
			CFileTime endTime = newEvent->GetEnd();

			int begin = m_StartSortDate.DifferenceInDays(beginTime);
			int end = m_StartSortDate.DifferenceInDays(endTime);

			if (end < begin)
			{
				end = begin;	// No negative events allowed
			}

			begin = begin + DAY_RANGE / 2;
			begin = max(0, begin);
			begin = min(DAY_RANGE - 1, begin);

			end = end + DAY_RANGE / 2;
			end = max(0, end);
			end = min(DAY_RANGE - 1, end);

			for (int i = begin; i <= end; i++)
			{
				// Add the event to all days it passes
				m_SortedEvents[i].push_back(newEvent);
			}
		}
	}
	else if (newItem->GetType() == RAINLENDAR_TYPE_TODO)
	{
		CEntryTodo* newTodo = (CEntryTodo*)newItem;
		m_AllTodos[*newTodo->GetGUID()] = newTodo;

		if (!newTodo->IsDeleted())		// No need to add deleted todos to the vectors
		{
			switch (newTodo->GetTodoType())
			{
			case TODO_TYPE_TODO:
				m_Todos.push_back(newTodo);
				break;

			case TODO_TYPE_DONE:
				m_Dones.push_back(newTodo);
				break;

			case TODO_TYPE_NOTE:
				m_Notes.push_back(newTodo);
				break;

			default:
				DebugLog("Unknown Todo type: %i", newTodo->GetTodoType());
			}
		}
	}
}

CEntryItem* CEventManager::GetItem(GUID* guid)
{
	CEntryItem* item = NULL;

	if (guid)
	{
		item = GetEvent(guid);
		if (item == NULL)
		{
			item = GetTodo(guid);
		}
	}

	return item;
}

CEntryEvent* CEventManager::GetEvent(GUID* guid)
{
	if (guid)
	{
		std::map<GUID, CEntryEvent*, GUIDComparator>::iterator iter = m_AllEvents.find(*guid);
		if (iter != m_AllEvents.end())
		{
			return (*iter).second;
		}
	}
//	DebugLog("Warning: There is no such event: %s", CEntryEvent::GetGUIDAsString(guid));
	return NULL;
}

std::vector<CEntryEvent*>& CEventManager::GetEvents(int day, int month, int year, DWORD flags)
{
	static std::vector<CEntryEvent*> result;
	CFileTime time(day, month, year);

	int index = m_StartSortDate.DifferenceInDays(time);
	index = index + DAY_RANGE / 2;
	index = max(0, index);
	index = min(DAY_RANGE - 1, index);

	result.clear();
	if (m_SortedEvents.size() > 0)
	{
		std::vector<CEntryEvent*>& rest = m_SortedEvents[index];

		for (UINT j = 0; j < rest.size(); j++)
		{
			CEntryEvent* event = rest[j];

			if ((flags & EVENT_FLAG_NO_DELETED) && event->IsDeleted()) continue;
			if ((flags & EVENT_FLAG_NO_READ_ONLY) && event->IsReadOnly()) continue;
			if ((flags & EVENT_FLAG_NO_CALENDAR_HIDDEN) && CConfig::Instance().IsHiddenProfile(event->GetProfile(), true)) continue;
			if ((flags & EVENT_FLAG_NO_EVENTLIST_HIDDEN) && CConfig::Instance().IsHiddenProfile(event->GetProfile(), false)) continue;

			CFileTime start = event->GetStartTime();
			CFileTime end = event->GetEndTime();
			end.Substract(1);	// This is so that if the end time is right on the next day it's not shown on that day.

			CFileTime tmpStart(start);
			tmpStart.Substract(24 * 60 * 60);
			if (tmpStart.Compare(time) != -1) continue;	// Start is after the given time

			int diffStart = time.DifferenceInDays(start);
			int diffEnd = time.DifferenceInDays(end);

			if (diffEnd < diffStart) diffEnd = diffStart;

			if (event->GetRecurrency() && event->GetRecurrency()->type != RECURRENCY_TYPE_SINGLE)
			{
				RainlendarRecurrency* recurrency = event->GetRecurrency();

				if (recurrency->repeatType == RECURRENCY_REPEAT_UNTIL)
				{
					// If the date is passed the until-time, ignore this event
					CFileTime until(recurrency->until);
					if (start.Compare(until) > 0)
					{
						continue;
					}
				}
				else if (recurrency->repeatType == RECURRENCY_REPEAT_COUNT)
				{
					// If the date is passed the count, ignore this event
					bool ignore = false;
					switch (recurrency->type)
					{
					case RECURRENCY_TYPE_DAILY:
						if (recurrency->frequency * recurrency->count <= -diffStart)
						{
							ignore = true;
						}
						break;

					case RECURRENCY_TYPE_WEEKLY:
						// TODO: The count defines individual events, not all in one week
						if (recurrency->frequency * recurrency->count <= (-diffStart) / 7)
						{
							ignore = true;
						}
						break;

					case RECURRENCY_TYPE_MONTHLY_NTH_DAY:
						// TODO: Some months may not have the [Nth] day
					case RECURRENCY_TYPE_MONTHLY_NTH_WEEKDAY:
						// TODO: Some months may not have the [Nth] week
					case RECURRENCY_TYPE_MONTHLY_LASTWEEK_WEEKDAY:
					case RECURRENCY_TYPE_MONTHLY_LAST_DAY:
					case RECURRENCY_TYPE_MONTHLY_FIRST_WEEKDAY:
					case RECURRENCY_TYPE_MONTHLY_LAST_WEEKDAY:
						{
							const SYSTEMTIME& sysTime = start.GetSystemTime();
							int diffMonths = (month + 12 * year) - (sysTime.wMonth + 12 * sysTime.wYear);
							if (recurrency->frequency * recurrency->count <= diffMonths)
							{
								ignore = true;
							}
						}
						break;

					case RECURRENCY_TYPE_YEARLY:
						{
							// TODO: Some years may not have the [Nth] day
							const SYSTEMTIME& sysTime = start.GetSystemTime();
							int diffYears = year - sysTime.wYear;
							if (recurrency->frequency * recurrency->count <= diffYears)
							{
								ignore = true;
							}
						}
						break;
					}

					if (ignore) continue;
				}
				// else FOREVER

				if (recurrency->type == RECURRENCY_TYPE_DAILY)
				{
					int newStart = diffStart + (recurrency->frequency * (-diffStart / recurrency->frequency));
					int newEnd = newStart + (diffEnd - diffStart);
					if (newStart <= 0 && newEnd >= 0)
					{
						result.push_back(event);
					}
				}
				else if (recurrency->type == RECURRENCY_TYPE_WEEKLY)
				{
					int freq = recurrency->frequency * 7;
					int newStart = diffStart + (freq * (-diffStart / freq));
					int newEnd = newStart + (diffEnd - diffStart);

					int weekday = start.GetWeekday();
					for (int i = 0; i < 7; i++)
					{
						if (recurrency->days & (1 << ((weekday + i) % 7)))
						{
							if (newStart + i <= 0 && newEnd + i >= 0)
							{
								result.push_back(event);
								break;
							}
						}
					}
				}
				else if (recurrency->type == RECURRENCY_TYPE_MONTHLY_NTH_DAY)	// On the [Nth] day of the month
				{
					int dist = diffEnd - diffStart;
					for (int i = 0; i <= dist; i++)
					{
						const SYSTEMTIME& sysTime = start.GetSystemTime();
						if (sysTime.wDay == day && ((((month + 12 * year) - (sysTime.wMonth + 12 * sysTime.wYear)) % recurrency->frequency) == 0))
						{
							result.push_back(event);
							break;
						}
						start.Add(24 * 60 * 60);
					}
				}
				else if (recurrency->type == RECURRENCY_TYPE_MONTHLY_NTH_WEEKDAY)	// On the [Nth] [weekday] of the month
				{
					int tmpDay = day;
					int dist = diffEnd - diffStart;
					CFileTime currentTime(tmpDay, month, year);
					for (int i = 0; i <= dist && currentTime.IsValid(); i++)
					{
						int weekday = currentTime.GetWeekday();
						SYSTEMTIME sysTime = start.GetSystemTime();

						if (weekday == sysTime.wDayOfWeek && 
							(((tmpDay - 1) / 7) == ((sysTime.wDay - 1) / 7)) &&
							((((month + 12 * year) - (sysTime.wMonth + 12 * sysTime.wYear)) % recurrency->frequency) == 0))
						{
							result.push_back(event);
							break;
						}
						currentTime.Substract(24 * 60 * 20);
						tmpDay = currentTime.GetSystemTime().wDay;
					}
				}
				else if (recurrency->type == RECURRENCY_TYPE_MONTHLY_LASTWEEK_WEEKDAY)	// On the last [weekday] of the month
				{
					int tmpDay = day;
					int dist = diffEnd - diffStart;
					CFileTime currentTime(tmpDay, month, year);
					for (int i = 0; i <= dist; i++)
					{
						int weekday = currentTime.GetWeekday();
						SYSTEMTIME sysTime = start.GetSystemTime();

						if (weekday == sysTime.wDayOfWeek && 
							(tmpDay + 7 > currentTime.GetDaysInMonth() && sysTime.wDay + 7 > start.GetDaysInMonth()) &&
							((((month + 12 * year) - (sysTime.wMonth + 12 * sysTime.wYear)) % recurrency->frequency) == 0))
						{
							result.push_back(event);
							break;
						}
						currentTime.Substract(24 * 60 * 20);
						tmpDay = currentTime.GetSystemTime().wDay;
					}
				}
				else if (recurrency->type == RECURRENCY_TYPE_MONTHLY_LAST_DAY)	// On the last day of the month
				{
					int tmpDay = day;
					int dist = diffEnd - diffStart;
					CFileTime currentTime(tmpDay, month, year);
					for (int i = 0; i <= dist; i++)
					{
						SYSTEMTIME sysTime = start.GetSystemTime();

						if (tmpDay == currentTime.GetDaysInMonth() &&
							((((month + 12 * year) - (sysTime.wMonth + 12 * sysTime.wYear)) % recurrency->frequency) == 0))
						{
							result.push_back(event);
							break;
						}
						currentTime.Substract(24 * 60 * 20);
						tmpDay = currentTime.GetSystemTime().wDay;
					}
				}
				else if (recurrency->type == RECURRENCY_TYPE_MONTHLY_FIRST_WEEKDAY)	// First weekday of the month
				{
					int tmpDay = day;
					int dist = diffEnd - diffStart;
					for (int i = 0; i <= dist; i++)
					{
						CFileTime currentTime(tmpDay, month, year);
						SYSTEMTIME sysTime = start.GetSystemTime();
						int weekday = currentTime.GetWeekday();
						int monthsDays = currentTime.GetDaysInMonth();

						if (((tmpDay == 1 && weekday != 0 && weekday != 6) ||
							(tmpDay == 2 && weekday == 1) ||
							(tmpDay == 3 && weekday == 1)) &&
							((((month + 12 * year) - (sysTime.wMonth + 12 * sysTime.wYear)) % recurrency->frequency) == 0))
						{
							result.push_back(event);
							break;
						}
						tmpDay -= 1;
						if (tmpDay <= 0) break;
					}
				}
				else if (recurrency->type == RECURRENCY_TYPE_MONTHLY_LAST_WEEKDAY)	// Last weekday of the month
				{
					int tmpDay = day;
					int dist = diffEnd - diffStart;
					CFileTime currentTime(tmpDay, month, year);
					for (int i = 0; i <= dist; i++)
					{
						SYSTEMTIME sysTime = start.GetSystemTime();
						int weekday = currentTime.GetWeekday();
						int monthsDays = currentTime.GetDaysInMonth();

						if (((tmpDay == monthsDays && weekday != 0 && weekday != 6) ||
						 	 (tmpDay == monthsDays - 1 && weekday == 5) ||
						 	 (tmpDay == monthsDays - 2 && weekday == 5)) &&
							 ((((month + 12 * year) - (sysTime.wMonth + 12 * sysTime.wYear)) % recurrency->frequency) == 0))
						{
							result.push_back(event);
							break;
						}
						currentTime.Substract(24 * 60 * 20);
						tmpDay = currentTime.GetSystemTime().wDay;
					}
				}
				else if (recurrency->type == RECURRENCY_TYPE_YEARLY)
				{
					int dist = diffEnd - diffStart;
					for (int i = 0; i <= dist; i++)
					{
						const SYSTEMTIME& sysTime = start.GetSystemTime();
						if (sysTime.wDay == day && sysTime.wMonth == month && ((year - sysTime.wYear) % recurrency->frequency) == 0)
						{
							result.push_back(event);
							break;
						}
						start.Add(24 * 60 * 60);
					}
				}
			}
			else
			{
				if (diffStart <= 0 && diffEnd >= 0)
				{
					result.push_back(event);
				}
			}
		}
	}

	return result;
}

/*
std::vector<CEntryEvent*>& CEventManager::GetEvents(CFileTime* rStart, CFileTime* rEnd, DWORD flags)
{
	static std::vector<CEntryEvent*> result;

	result.clear();

	int begin = m_StartSortDate.DifferenceInDays(*rStart);
	int end = m_StartSortDate.DifferenceInDays(*rEnd);

	begin = begin + DAY_RANGE / 2;
	begin = max(0, begin);
	begin = min(DAY_RANGE - 1, begin);

	end = end + DAY_RANGE / 2;
	end = max(0, end);
	end = min(DAY_RANGE - 1, end);

	for (int index = begin; index <= end; index++)
	{
		std::vector<CEntryEvent*>& rest = m_SortedEvents[index];

		for (UINT i = 0; i < rest.size(); i++)
		{
			CEntryEvent* event = rest[i];
			CFileTime eStart = event->GetBegin();
			CFileTime eEnd = event->GetEnd();

			// TODO: recurrency check

			//                                   
			//           |             |                     
			//  ##################################
			//        #############    |
			//           |  #################
			//           | ######      |
			//           |             |

			int a = rStart->Compare(eStart);
			int b = rEnd->Compare(eStart);
			int c = rStart->Compare(eEnd);
			int d = rEnd->Compare(eEnd);

			if ((a <= 0 && b >= 0) || (c >= 0 && d <= 0))
			{
				if (!((flags & EVENT_FLAG_NO_DELETED) && event->IsDeleted()) || ((flags & EVENT_FLAG_NO_READ_ONLY) && event->IsReadOnly()))
				{
					result.push_back(event);
				}
			}
		}
	}
	return result;
}
*/

const std::map<GUID, CEntryEvent*, GUIDComparator>& CEventManager::GetAllEvents()
{
	return m_AllEvents;
}

CEntryTodo* CEventManager::GetTodo(GUID* guid)
{
	if (guid)
	{
		std::map<GUID, CEntryTodo*, GUIDComparator>::iterator iter = m_AllTodos.find(*guid);
		if (iter != m_AllTodos.end())
		{
			return (*iter).second;
		}
	}
	return NULL;
}



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
  $Header: /home/cvsroot/Rainlendar/Library/RainlendarAPI.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: RainlendarAPI.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.4  2005/03/25 13:58:43  rainy
  no message

  Revision 1.3  2005/03/01 18:25:33  rainy
  Updated the interface.

  Revision 1.2  2004/12/05 18:24:24  rainy
  Added todo's profile is copied too.

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#pragma warning(disable: 4786)
#pragma warning(disable: 4996)

#include "RainlendarAPI.h"
#include "RainlendarDll.h"
#include "Litestep.h"
#include "CalendarWindow.h"
#include "EditEvent.h"
#include "TodoDialog.h"

std::vector<CEntryEvent*> g_AddedEvents;
std::vector<GUID*> g_DeletedEvents;

RainlendarItem* Rainlendar_GetItem(GUID* guid)
{
	CEventManager& manager = GetRainlendar()->GetCalendarWindow().GetEventManager();
	CEntryEvent* event = manager.GetEvent(guid);
	if (event)
	{
		return event->GetRainlendarEvent();
	}
	else
	{
		CEntryTodo* todo = manager.GetTodo(guid);
		if (todo)
		{
			return todo->GetRainlendarTodo();
		}
	}
	return NULL;
}

RainlendarItem** Rainlendar_GetAllItems(RAINLENDAR_TYPE type)
{
	CEventManager& manager = GetRainlendar()->GetCalendarWindow().GetEventManager();
	if (type == RAINLENDAR_TYPE_EVENT)
	{
		RainlendarItem** eventTable = NULL;

		const std::map<GUID, CEntryEvent*, GUIDComparator>& allEvents = manager.GetAllEvents();

		int counter = 0;
		eventTable = new RainlendarItem*[allEvents.size() + 1];

		std::map<GUID, CEntryEvent*, GUIDComparator>::const_iterator iter = allEvents.begin();

		for ( ; iter != allEvents.end(); iter++)
		{
			CEntryEvent* event = (*iter).second;

			// Filter read only events away
			if (!event->IsReadOnly())
			{
				eventTable[counter++] = event->GetRainlendarEvent();
			}
		}
		eventTable[counter++] = NULL;		// end with NULL

		return eventTable;		// This must be deleted by the caller!
	}
	else if (type == RAINLENDAR_TYPE_TODO)
	{
		RainlendarItem** todoTable = NULL;

		const std::map<GUID, CEntryTodo*, GUIDComparator>& allTodos = manager.GetAllTodos();

		int counter = 0;

		todoTable = new RainlendarItem*[allTodos.size() + 1];

		std::map<GUID, CEntryTodo*, GUIDComparator>::const_iterator iter = allTodos.begin();

		for ( ; iter != allTodos.end(); iter++)
		{
			CEntryTodo* todo = (*iter).second;

			// Filter read only events away
			if (!todo->IsReadOnly())
			{
				todoTable[counter++] = todo->GetRainlendarTodo();
			}
		}
		todoTable[counter++] = NULL;		// end with NULL

		return todoTable;		// This must be deleted by the caller!
	}
	return NULL;
}

void Rainlendar_SetItem(RainlendarItem* item, UINT id)
{
	if (item)
	{
		CEventManager& manager = GetRainlendar()->GetCalendarWindow().GetEventManager();
		if (item->type == RAINLENDAR_TYPE_EVENT)
		{
			CEntryEvent* newEvent = new CEntryEvent(item);
			newEvent->SetPluginID(id);	// The originator
			manager.AddItem(newEvent, true);	// Buffer Events
		}
		else if (item->type == RAINLENDAR_TYPE_TODO)
		{
			CEntryTodo* newTodo = new CEntryTodo(item);
			newTodo->SetPluginID(id);	// The originator
			manager.AddItem(newTodo, true);	// Buffer Events
		}
		else
		{
			DebugLog("Unknown item type: %i", item->type);
		}
	}
}

void Rainlendar_Log(LPCTSTR debugStr)
{
	DebugLog(debugStr);
}

void Rainlendar_Redraw()
{
	GetRainlendar()->GetCalendarWindow().RedrawAll();
}

void Rainlendar_Refresh(int delay)
{
	if (delay == 0)
	{
		// Delay refresh
		PostMessage(GetRainlendar()->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_REFRESH, NULL);
	}
	else
	{
		SetTimer(GetRainlendar()->GetCalendarWindow().GetWindow(), REFRESH_TIMER, delay, NULL);
	}
}

void Rainlendar_FlushItems()
{
	GetRainlendar()->GetCalendarWindow().GetEventManager().FlushBufferedItems(false);
}

bool Rainlendar_EditItemDialog(HWND parent, RainlendarItem* item, RAINLENDAR_TYPE type)
{
	if (type == RAINLENDAR_TYPE_EVENT)
	{
		return ShowEditEvent(parent, GetRainlendar()->GetInstance(), (RainlendarEvent*)item);
	}
	else if (type == RAINLENDAR_TYPE_TODO)
	{
		return ShowTodoDialog(parent, GetRainlendar()->GetInstance(), item ? &item->guid : NULL);
	}

	return false;
}

LPCTSTR Rainlendar_GetString(LPCTSTR section, int index, LPCTSTR defaultString)
{
	LPCTSTR str = CCalendarWindow::c_Language.GetString(section, index);
	if (str == NULL)
	{
		return defaultString;
	}
	return str;
}

bool Rainlendar_CopyItem(RainlendarItem* from, RainlendarItem* to)
{
	if (from->type == to->type)
	{
		if (from->type == RAINLENDAR_TYPE_EVENT)
		{
			if (((RainlendarEvent*)to)->recurrency)
			{
				delete ((RainlendarEvent*)to)->recurrency;
			}

			*(RainlendarEvent*)to = *(RainlendarEvent*)from;

			// Duplicate the strings
			if (((RainlendarEvent*)from)->header)
			{
				((RainlendarEvent*)to)->header = strdup(((RainlendarEvent*)from)->header);
			}
			if (((RainlendarEvent*)from)->message)
			{
				((RainlendarEvent*)to)->message = strdup(((RainlendarEvent*)from)->message);
			}
			if (((RainlendarEvent*)from)->profile)
			{
				((RainlendarEvent*)to)->profile = strdup(((RainlendarEvent*)from)->profile);
			}
			
			if (((RainlendarEvent*)from)->recurrency)
			{
				((RainlendarEvent*)to)->recurrency = new RainlendarRecurrency;
				*((RainlendarEvent*)to)->recurrency = *((RainlendarEvent*)from)->recurrency;
			}

			return true;
		}
		else if (from->type == RAINLENDAR_TYPE_TODO)
		{
			*(RainlendarTodo*)to = *(RainlendarTodo*)from;

			if (((RainlendarTodo*)from)->message)
			{
				((RainlendarTodo*)to)->message = strdup(((RainlendarTodo*)from)->message);
			}
			if (((RainlendarTodo*)from)->profile)
			{
				((RainlendarTodo*)to)->profile = strdup(((RainlendarTodo*)from)->profile);
			}
			
			return true;
		}
		else
		{
			DebugLog("Unknown item type (%i), unable to copy it.", from->type);
		}
	}
	else
	{
		DebugLog("Different item types, unable to make a copy.");
	}
	return false;
}

void Rainlendar_DeleteItem(RainlendarItem* item)
{
	if (item->type == RAINLENDAR_TYPE_EVENT)
	{
		RainlendarEvent* event = (RainlendarEvent*)item;

		if (event->header)
		{
			free(event->header);
		}
		if (event->message)
		{
			free(event->message);
		}
		if (event->profile)
		{
			free(event->profile);
		}
		if (event->recurrency)
		{
			free(event->recurrency);
		}
		delete event;
	}	
	else if (item->type == RAINLENDAR_TYPE_TODO)
	{
		RainlendarTodo* todo = (RainlendarTodo*)item;

		if (todo->message)
		{
			free(todo->message);
		}
		if (todo->profile)
		{
			free(todo->profile);
		}
		delete todo;
	}
	else
	{
		DebugLog("Unknown item type (%i), unable to delete it.", item->type);
	}
}

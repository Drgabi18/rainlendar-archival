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
  $Header: /home/cvsroot/Rainlendar/Library/EventManager.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: EventManager.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.7  2005/03/01 18:50:45  rainy
  no message

  Revision 1.6  2004/12/05 18:32:02  rainy
  Fixed GetEvents.
  Added thread-safetyness.

  Revision 1.5  2004/11/06 13:38:59  rainy
  no message

  Revision 1.4  2004/04/24 11:19:10  rainy
  Some methods are now const.

  Revision 1.3  2003/05/26 18:44:52  Rainy
  Added GetAllEvents().

  Revision 1.2  2003/05/25 18:10:11  Rainy
  The GetEvent() returns now a sorted vector.

  Revision 1.1  2002/08/03 16:37:08  rainy
  Intial version.

*/

#ifndef __EVENTMANAGER_H__
#define __EVENTMANAGER_H__

#pragma warning(disable: 4786)

#include <windows.h>
#include "EntryEvent.h"
#include "EntryTodo.h"
#include "PluginManager.h"
#include "FileTime.h"
#include <map>
#include <list>
#include <vector>

#define EVENT_FLAG_NO_DELETED			1
#define EVENT_FLAG_NO_READ_ONLY			2
#define EVENT_FLAG_NO_CALENDAR_HIDDEN	4
#define EVENT_FLAG_NO_EVENTLIST_HIDDEN	8

class GUIDComparator
{
public:
  bool operator () (const GUID& guid1, const GUID& guid2) const
  {
      if (guid1.Data1 < guid2.Data1)
		  return true;
	  else if (guid1.Data1 > guid2.Data1)
		  return false;
	  
	  if (guid1.Data2 < guid2.Data2)
		  return true;
	  else if (guid1.Data2 > guid2.Data2)
		  return false;
	  
	  if (guid1.Data3 < guid2.Data3)
		  return true;
	  else if (guid1.Data3 > guid2.Data3)
		  return false;
	  
	  return memcmp(guid1.Data4, guid2.Data4, sizeof(guid1.Data4)) < 0;
  }
};

class CEventManager
{
public:
	CEventManager();
	~CEventManager();

	void Initialize(CPluginManager& pluginManager);
	
	bool AddItem(CEntryItem* newItem, bool buffer);
	bool DeleteItem(GUID* guid);

	bool FlushBufferedItems(bool sendToAll);

	CEntryItem* GetItem(GUID* guid);

	CEntryEvent* GetEvent(GUID* guid);
	std::vector<CEntryEvent*>& GetEvents(int day, int month, int year, DWORD flags);
//	std::vector<CEntryEvent*>& GetEvents(CFileTime* start, CFileTime* end, DWORD flags);
	const std::map<GUID, CEntryEvent*, GUIDComparator>& GetAllEvents();

	CEntryTodo* GetTodo(GUID* guid);
	const std::map<GUID, CEntryTodo*, GUIDComparator>& GetAllTodos() { return m_AllTodos; };
	std::vector<CEntryTodo*>& GetTodos() { return m_Todos; };
	std::vector<CEntryTodo*>& GetDones() { return m_Dones; };
	std::vector<CEntryTodo*>& GetNotes() { return m_Notes; };

private:
	void Clear();
	void RemoveItem(CEntryItem* item);
	void InsertItem(CEntryItem* newItem);

	std::map<GUID, CEntryEvent*, GUIDComparator> m_AllEvents;
	std::vector< std::vector<CEntryEvent*> > m_SortedEvents;	// 1st index = today - year ... today + year, first & last contains the rest

	std::map<GUID, CEntryTodo*, GUIDComparator> m_AllTodos;
	std::vector<CEntryTodo*> m_Todos;
	std::vector<CEntryTodo*> m_Dones;
	std::vector<CEntryTodo*> m_Notes;
	
	CFileTime m_StartSortDate;
	CPluginManager* m_PluginManager;

	std::vector<CEntryItem*> m_BufferedAddItems;

	CRITICAL_SECTION m_CriticalSection; 
};

#endif

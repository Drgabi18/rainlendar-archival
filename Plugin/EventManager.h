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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/EventManager.h,v 1.3 2003/05/26 18:44:52 Rainy Exp $

  $Log: EventManager.h,v $
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
#include "Event.h"
#include <map>
#include <list>
#include <vector>

class CEventManager
{
public:
	CEventManager();
	~CEventManager();

	void ReadEvents();
	std::vector<CEventMessage*> GetEvents(int day, int month, int year);
	void AddEvent(CEventMessage& event);
	void RemoveEvent(CEventMessage& event);
	void WriteEvents(int day, int month, int year);
	void WriteEvent(CEventMessage& event);
	CEventMessage* GetEvent(int ID);
	const std::list<CEventMessage>& GetAllEvents() { return m_AllEvents; };

private:

	// Better data structure might be faster...
	std::list<CEventMessage> m_AllEvents;
};

#endif

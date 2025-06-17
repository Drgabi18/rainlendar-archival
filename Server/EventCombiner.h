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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Server/EventCombiner.h,v 1.4 2002/08/03 16:17:23 rainy Exp $

  $Log: EventCombiner.h,v $
  Revision 1.4  2002/08/03 16:17:23  rainy
  Changed to support repeating events.

  Revision 1.3  2002/02/27 18:14:43  rainy
  Removed size from the packages

  Revision 1.2  2002/01/29 16:23:35  rainy
  Added win32 defines.

  Revision 1.1  2002/01/27 16:21:04  rainy
  Initial version

*/

#ifndef __EVENTCOMBINER_H__
#define __EVENTCOMBINER_H__

#ifdef _WIN32
#pragma warning(disable: 4786)
#endif

#include <string>
#include <map>
#include <list>
#include <clientconnector.h>

const unsigned32 PROTOCOL_VERSION = 200;

enum
{
	pcRequestEvents = ssobjects::PacketBuffer::pcUser,
	pcUpdateEvents,
	pcWaitingForEvents,
	pcSendingEvents,
	pcEvent,
	pcIncorrectVersion,
	pcServerBusy
};

class CEventCombiner  
{
public:
	CEventCombiner();
	~CEventCombiner();

	bool ReadEvents(const char* filename);
	bool WriteEvents(const char* filename);
	bool DecodePacket(ssobjects::PacketBuffer* packet);
	std::list<ssobjects::PacketBuffer*>& EncodePackets();
	unsigned32 GetEventCount() { return m_Events.size(); };

private:
	// We are not using the CEventMessage class here, cooz we are re-using this class in the server/client
	// in the server
	struct EventInfo
	{
		unsigned32 stamp;
		std::string message;
		std::string profile;
		unsigned32 validUntil;
		unsigned32 everyNth;
		unsigned32 type;
		unsigned32 deleted;
	};

	bool AddEvent(std::string& date, EventInfo* newEvent);

	std::map<std::string, EventInfo*> m_Events;
	std::list<ssobjects::PacketBuffer*> m_Packets;
};

#endif

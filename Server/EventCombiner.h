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
  $Header: //RAINBOX/cvsroot/Rainlendar/Server/EventCombiner.h,v 1.8 2003/08/09 15:23:08 Rainy Exp $

  $Log: EventCombiner.h,v $
  Revision 1.8  2003/08/09 15:23:08  Rainy
  Added LastModified and CreatedBy fields for server.

  Revision 1.7  2003/05/25 18:10:47  Rainy
  A message is set to the client if the event is filtered.

  Revision 1.6  2003/05/25 15:11:44  Rainy
  Added fltering support for the incoming & outgoing events.

  Revision 1.5  2003/05/07 19:13:19  rainy
  The protocol version is increased.

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
#ifdef SERVER
#include "IPNumber.h"
#endif

const unsigned32 PROTOCOL_VERSION = 300;

class CIPFilter;

enum
{
	pcRequestEvents = ssobjects::PacketBuffer::pcUser,
	pcUpdateEvents,
	pcWaitingForEvents,
	pcSendingEvents,
	pcEvent,
	pcIncorrectVersion,
	pcServerBusy,
	pcResult,
	pcFiltered,
	pcError
};

enum RAINLENDAR_ERROR_CODE
{
    ERROR_CODE_NONE,
    ERROR_CODE_OUTOFDATE,
    ERROR_CODE_NEWEVENTS
};

class CEventCombiner  
{
public:
	CEventCombiner();
	~CEventCombiner();

	bool ReadEvents(const char* filename);
	bool WriteEvents(const char* filename);
	bool DecodePacket(ssobjects::PacketBuffer* packet, CIPFilter* filter, ULONG ipAddr);
	std::list<ssobjects::PacketBuffer*>& EncodePackets(CIPFilter* filter, ULONG ipAddr);
	unsigned32 GetEventCount() { return m_Events.size(); };
	unsigned32 GetErrorCode() { return m_ErrorCode; };

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
#ifdef SERVER
		CIPNumber createdBy;
		CIPNumber lastModified;
#endif
	};

	RAINLENDAR_ERROR_CODE m_ErrorCode;

	bool AddEvent(std::string& date, EventInfo* newEvent);

	std::map<std::string, EventInfo*> m_Events;
	std::list<ssobjects::PacketBuffer*> m_Packets;
};

#endif

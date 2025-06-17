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
  $Header: /home/cvsroot/Rainlendar/Server/EventCombiner.h,v 1.6 2005/10/12 15:55:05 rainy Exp $

  $Log: EventCombiner.h,v $
  Revision 1.6  2005/10/12 15:55:05  rainy
  no message

  Revision 1.5  2005/10/12 15:45:31  rainy
  no message

  Revision 1.4  2005/09/08 16:13:51  rainy
  no message

  Revision 1.3  2005/09/08 16:09:12  rainy
  no message

  Revision 1.2  2005/07/11 16:16:37  rainy
  *** empty log message ***

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.12  2005/03/01 19:33:51  rainy
  *** empty log message ***

  Revision 1.11  2004/11/14 12:11:56  rainy
  Linux fixes

  Revision 1.10  2004/11/14 11:37:31  rainy
  Changes for 0.20.

  Revision 1.9  2003/08/23 09:15:57  Rainy
  Added some more logging.
  The date format is verified.

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
#include <vector>
#include <clientconnector.h>
#include "IPNumber.h"
#include "../Library/RainlendarAPI.h"

const unsigned32 PROTOCOL_VERSION = 800;

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
	pcError,
	pcNotFound
};

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

class CEventCombiner  
{
public:
	CEventCombiner();
	~CEventCombiner();

	bool ReadEvents(const char* filename);
	bool WriteEvents(const char* filename);
	bool DecodePacket(ssobjects::PacketBuffer* packet, CIPFilter* filter, ULONG ipAddr);
	std::list<ssobjects::PacketBuffer*>& EncodePackets(std::vector<GUID>& requestedEvents, CIPFilter* filter, ULONG ipAddr);
	unsigned32 GetEventCount() { return m_Events.size(); };

	static bool CheckFilename(std::string fileName);

private:
	struct EventInfo
	{
		RainlendarItem* item;
		CIPNumber createdBy;
		CIPNumber lastModified;
	};

	ssobjects::PacketBuffer* EncodePacket(EventInfo* info, CIPFilter* filter, ULONG ipAddr);

	bool AddEvent(EventInfo* newEvent);
	void DeleteEvent(EventInfo* event);

	const char* GuidToString(GUID guid);
	GUID GuidFromString(const char* str);

	int CompareFileTime(FILETIME* ft1, FILETIME* ft2);

	std::map<GUID, EventInfo*, GUIDComparator> m_Events;
	std::list<ssobjects::PacketBuffer*> m_Packets;
};

#endif

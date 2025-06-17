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
  $Header: /home/cvsroot/Rainlendar/Server/RainlendarServer.cpp,v 1.2 2005/10/14 17:05:29 rainy Exp $

  $Log: RainlendarServer.cpp,v $
  Revision 1.2  2005/10/14 17:05:29  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.10  2005/03/01 17:02:52  rainy
  Path for the files is selectable.

  Revision 1.9  2004/11/14 11:38:13  rainy
  Changes for 0.20.

  Revision 1.8  2003/05/25 18:15:32  Rainy
  Checks that the ID doesn't contain illegal chars.

  Revision 1.7  2003/05/25 15:13:12  Rainy
  Added fltering support.

  Revision 1.6  2003/05/24 14:05:07  Rainy
  Removed a warning

  Revision 1.5  2003/05/07 19:02:01  rainy
  Modifies the protocol to send a confirmation after all events has been received.

  Revision 1.4  2003/04/22 17:58:57  rainy
  Added more logging info

  Revision 1.3  2002/08/03 16:09:31  rainy
  Added one new LOG string.

  Revision 1.2  2002/01/29 17:36:22  rainy
  Changed the LOG strings a bit.

  Revision 1.1  2002/01/27 16:21:05  rainy
  Initial version

*/

#include "RainlendarServer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace ssobjects;

CRainlendarServer::CRainlendarServer(SockAddr& saBind, CIPFilter* filter, CStr& targetPath)
	: SimpleServer(saBind)
{
	m_Busy = false;
	m_Combiner = NULL;
	m_EventCount = 0;
	m_NewEventsFound = false;
	m_BusyTime = 0;
	m_Filter = filter;
	m_TargetPath = targetPath;
}

CRainlendarServer::~CRainlendarServer()
{
	if (m_Combiner)
	{
		delete m_Combiner;
	}
}

void CRainlendarServer::processSingleMsg(PacketMessage* pmsg)
{
	ServerSocket* psocket = (ServerSocket*)pmsg->socket();
	PacketBuffer* ppacket = pmsg->packet();

	switch(ppacket->getCmd())
	{
	case pcUpdateEvents:
		{
			LOG("Got pcUpdateEvents from %s", psocket->getAddr().dottedDecimal());
			
			if (ppacket->getBufferSize() > 0)
			{
				// Check the protocol version
				unsigned32 version;
				(*ppacket) >> version;
				
				if (version != PROTOCOL_VERSION)
				{
					PacketBuffer reply(pcIncorrectVersion);
					sendPacket(psocket, reply);
					LOG("The client is using wrong protocol version. Please upgrade!");
				}
				else if (m_Busy)
				{
					PacketBuffer reply(pcServerBusy);
					sendPacket(psocket, reply);
					LOG("The server is busy serving another client.");
				}
				else
				{
					CStr iniFile;

					(*ppacket) >> iniFile;
					(*ppacket) >> m_EventCount;

					// Make sure that the ID doesn't include illegal characters
					if (!CEventCombiner::CheckFilename((char*)iniFile))
					{
						PacketBuffer reply(pcError);
						sendPacket(psocket, reply);
						LOG("Sent pcError. The ID-string contains illegal characters.");
						// Reset members. Server is available again
						Reset();
						break;
					}

					m_IniFile = m_TargetPath;
					m_IniFile += iniFile;
					m_IniFile += ".rdb";

					if (m_EventCount > 0)
					{
						m_Busy = true;
						m_Combiner = new CEventCombiner;
						m_Combiner->ReadEvents(m_IniFile);	 // Read the old events
					}

					// Send reply that the client can send the events
					PacketBuffer reply(pcWaitingForEvents);
					sendPacket(psocket, reply);
					LOG("Sent pcWaitingForEvents (waiting for %i events from the client)", m_EventCount);
				}
			}
		}
		break;

	case pcEvent:
		{
			if (m_Combiner != NULL)
			{
				if (ppacket->getBufferSize() > 0)
				{
					if (m_Combiner->DecodePacket(ppacket, m_Filter, psocket->getAddr().ipAddr()))
					{
						m_NewEventsFound = true;
					}

					m_EventCount--;

					if (m_EventCount == 0)
					{
						if (m_NewEventsFound)
						{
							m_Combiner->WriteEvents(m_IniFile);	 // Write the new events if new ones are received
							LOG("Wrote Events to file: %s", m_IniFile.String());
						}

						// Send reply that the client can send the events
						PacketBuffer reply(pcResult);
						sendPacket(psocket, reply);
						LOG("Got all events and sent pcResult");

						// Reset members. Server is available again
						Reset();
						LOG("Server is ready to serve!");
					}
				}
			}
		}
		break;

	case pcRequestEvents:
		{
			LOG("Got pcRequestEvents from %s", psocket->getAddr().dottedDecimal());

			if (ppacket->getBufferSize() > 0)
			{
				unsigned32 version;
				(*ppacket) >> version;
				
				if (version != PROTOCOL_VERSION)
				{
					PacketBuffer reply(pcIncorrectVersion);
					sendPacket(psocket, reply);
					LOG("The client is using wrong protocol version. Please upgrade!");
				}
				else if (m_Busy)
				{
					PacketBuffer reply(pcServerBusy);
					sendPacket(psocket, reply);
					LOG("The server is busy serving another client.");
				}
				else
				{
					signed32 eventCount;
					CStr iniFile;

					m_Busy = true;
					(*ppacket) >> iniFile;
					(*ppacket) >> eventCount;

					// If event count is > 0, check which events are requested
					if (eventCount > 0)
					{
						unsigned char* buffer = ppacket->getPointer();
						for (int i = 0; i < eventCount; i++)
						{
							m_RequestedEvents.push_back(*(GUID*)buffer);
							buffer += sizeof(GUID);
						}
					}

					// Make sure that the ID doesn't include illegal characters
					if (!CEventCombiner::CheckFilename((char*)iniFile))
					{
						PacketBuffer reply(pcError);
						sendPacket(psocket, reply);
						LOG("Sent pcError. The ID-string contains illegal characters.");
						// Reset members. Server is available again
						Reset();
						break;
					}

					m_IniFile = m_TargetPath;
					m_IniFile += iniFile;
					m_IniFile += ".rdb";

					m_Combiner = new CEventCombiner;
					m_Combiner->ReadEvents(m_IniFile);	 // Read the old events

					if (m_RequestedEvents.size() > 0)
					{
						m_EventCount = m_RequestedEvents.size();
					}
					else
					{
						m_EventCount = m_Combiner->GetEventCount();
					}

					PacketBuffer reply(pcSendingEvents);
					reply << m_EventCount;
					sendPacket(psocket, reply);
					LOG("Sent pcSendingEvents.");
				}
			}
		}
		break;

	case pcWaitingForEvents:
		{
			if (m_Combiner != NULL)
			{
				// Send the events
				std::list<PacketBuffer*>& packetList = m_Combiner->EncodePackets(m_RequestedEvents, m_Filter, psocket->getAddr().ipAddr());

				std::list<PacketBuffer*>::iterator listIter = packetList.begin();
				for( ; listIter != packetList.end(); listIter++)
				{
					sendPacket(psocket, (*listIter));
				}

				LOG("Sent %i events to the client.", packetList.size());
			}
		}
		break;

	case pcResult:
		{
			// Client received the events successfully

			// Reset members. Server is available again
			Reset();
			LOG("Server is ready to serve!");
		}
		
	case PacketBuffer::pcClosed:
	case PacketBuffer::pcNewConnection:
		break;

	default:
		LOG("Unknown event %i", ppacket->getCmd());
		break;
	}
	
	DELETE_NULL(ppacket);
}

void CRainlendarServer::idle(unsigned32 /*nTimer*/)
{
	// If the server is busy too long, reset it
	if (m_Busy) m_BusyTime++;

	if (m_BusyTime > 30)
	{
		LOG("Server was idle too long -> Resetting the server");

		// Reset server after 30 secs of busy time
		Reset();
	}
}

void CRainlendarServer::Reset()
{
	m_Busy = false;
	delete m_Combiner;
	m_Combiner = NULL;
	m_EventCount = 0;
	m_NewEventsFound = false;
	m_BusyTime = 0;
	m_IniFile = "";
	m_RequestedEvents.clear();
}


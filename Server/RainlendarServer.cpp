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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Server/RainlendarServer.cpp,v 1.2 2002/01/29 17:36:22 rainy Exp $

  $Log: RainlendarServer.cpp,v $
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

CRainlendarServer::CRainlendarServer(SockAddr& saBind) 
	: SimpleServer(saBind)
{
	m_Busy = false;
	m_Combiner = NULL;
	m_EventCount = 0;
	m_NewEventsFound = false;
	m_BusyTime = 0;
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
			LOG("Got pcUpdateEvents");
			
			if (ppacket->getBufferSize() > 0)
			{
				// Check the protocol version
				unsigned32 version;
				(*ppacket) >> version;
				
				if (version != PROTOCOL_VERSION)
				{
					PacketBuffer reply(pcIncorrectVersion);
					sendPacket(psocket, reply);
					LOG("The client is using wrong protocol version.");
				}
				else if (m_Busy)
				{
					PacketBuffer reply(pcServerBusy);
					sendPacket(psocket, reply);
					LOG("The server is busy serving another client.");
				}
				else
				{
					(*ppacket) >> m_IniFile;
					(*ppacket) >> m_EventCount;

					if (m_EventCount > 0)
					{
						m_IniFile += ".ini";

						m_Busy = true;
						m_Combiner = new CEventCombiner;
						m_Combiner->ReadEvents(m_IniFile);	 // Read the old events
					}

					// Send reply that the client can send the events
					PacketBuffer reply(pcWaitingForEvents);
					sendPacket(psocket, reply);
					LOG("Sent pcWaitingForEvents");
				}
			}
		}
		break;

	case pcEvent:
		{
			if (ppacket->getBufferSize() > 0)
			{
				if (m_Combiner->DecodePacket(ppacket))
				{
					m_NewEventsFound = true;
				}

				m_EventCount--;

				if (m_EventCount == 0)
				{
					if (m_NewEventsFound)
					{
						m_Combiner->WriteEvents(m_IniFile);	 // Write the new events if new ones are received
					}

					// Reset members. Server is available again
					Reset();

					LOG("Server is ready to serve!");
				}
			}
		}
		break;

	case pcRequestEvents:
		{
			LOG("Got pcRequestEvents");

			if (ppacket->getBufferSize() > 0)
			{
				unsigned32 version;
				(*ppacket) >> version;
				
				if (version != PROTOCOL_VERSION)
				{
					PacketBuffer reply(pcIncorrectVersion);
					sendPacket(psocket, reply);
					LOG("The client is using wrong protocol version.");
				}
				else if (m_Busy)
				{
					PacketBuffer reply(pcServerBusy);
					sendPacket(psocket, reply);
					LOG("The server is busy serving another client.");
				}
				else
				{
					m_Busy = true;
					(*ppacket) >> m_IniFile;
					m_IniFile += ".ini";

					m_Combiner = new CEventCombiner;
					m_Combiner->ReadEvents(m_IniFile);	 // Read the old events
					m_EventCount = m_Combiner->GetEventCount();

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
			// Send the events
			std::list<PacketBuffer*>& packetList = m_Combiner->EncodePackets();

			std::list<PacketBuffer*>::iterator listIter = packetList.begin();
			for( ; listIter != packetList.end(); listIter++)
			{
				sendPacket(psocket, (*listIter));
			}
			
			// Reset members. Server is available again
			Reset();

			LOG("Server is ready to serve!");
		}
		break;

	case PacketBuffer::pcClosed:
	case PacketBuffer::pcNewConnection:
		break;

	default:
		printf("Unknown event %i\n", ppacket->getCmd());
		break;
	}
	
	DELETE_NULL(ppacket);
}

void CRainlendarServer::idle(unsigned32 nTimer)
{
	// If the server is busy too long, reset it
	if (m_Busy) m_BusyTime++;

	if (m_BusyTime > 30)
	{
		// Reset server after 30 secs of busy time
		Reset();

		LOG("Resetting the server");
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
}


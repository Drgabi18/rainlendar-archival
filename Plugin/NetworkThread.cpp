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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/NetworkThread.cpp,v 1.9 2003/08/09 16:34:12 Rainy Exp $

  $Log: NetworkThread.cpp,v $
  Revision 1.9  2003/08/09 16:34:12  Rainy
  Fixed a hang if there are no events to send to the server.

  Revision 1.8  2003/06/15 09:47:57  Rainy
  Strings are read from CLanguage class.

  Revision 1.7  2003/05/25 18:13:31  Rainy
  Updated for new protocol.

  Revision 1.6  2003/05/07 19:03:44  rainy
  Added error codes for the methods.

  Revision 1.5  2002/05/30 18:26:03  rainy
  The logging stuff is back.

  Revision 1.4  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.3  2002/01/29 17:34:38  rainy
  Displays the ID in the lid.

  Revision 1.2  2002/01/27 16:20:45  rainy
  Added header comment

*/

#include "../Server/EventCombiner.h"
#include "RainlendarDLL.h"
#include "NetworkThread.h"
#include "CalendarWindow.h"
#include "ConfigDialog.h"

using namespace ssobjects;

unsigned32 Update(NetworkParams& param, ClientConnector& connection);
unsigned32 Request(NetworkParams& param, ClientConnector& connection);

DWORD WINAPI NetworkThreadProc(LPVOID pParam)
{
    unsigned32 error = 0;
    
    char tmpSz[256];

	NetworkParams param = *(NetworkParams*)pParam;	// Take a copy of the parameters

	if (param.userID.empty())
	{
		param.userID = "Rainlendar";	// Gotta have a name
	}

	char buffer[MAX_LINE_LENGTH];
	itoa(param.serverPort, tmpSz, 10);
	sprintf(buffer, CCalendarWindow::c_Language.GetString("NetStatus", 0), param.serverAddress.c_str(), tmpSz, param.userID.c_str());
	AddStatusString(buffer);

	try
	{
		ClientConnector connection(param.serverAddress.c_str(), param.serverPort);
		AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 1));

		switch (param.requestType)
		{
		case REQUEST_SYNCRONIZATION:
            // Ignore possible update errors when syncing
			Update(param, connection);	// Send all events to the server
			error = Request(param, connection);	// Get events from the server
			break;

		case REQUEST_SENDEVENTS:
			error = Update(param, connection);	// Send all events to the server
			break;

		case REQUEST_GETEVENTS:
			error = Request(param, connection);
			break;

		default:
			AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 2));
		}
	}
	catch(GeneralException& e)
	{
		char buffer[MAX_LINE_LENGTH];
		sprintf(buffer, CCalendarWindow::c_Language.GetString("NetStatus", 3), e.getErrorMsg());
		AddStatusString(buffer);
	}

	AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 4));

	// Notify that the server sync was finished
	if (param.window)
	{
		SendMessage(param.window, WM_SERVER_SYNC_FINISHED, error, 0);
	}

    return 0;   // thread completed successfully
}

unsigned32 Update(NetworkParams& param, ClientConnector& connection)
{
    unsigned32 error = ERROR_CODE_NONE;

	// Read Events.ini and send it to the server
	// Wait for confirmation from server
	const char* buffer = CCalendarWindow::c_Config.GetEventsPath().c_str();
	
	AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 5));

	CEventCombiner combiner;
	combiner.ReadEvents(buffer);
	std::list<PacketBuffer*>& packetList = combiner.EncodePackets(NULL, 0);

	if (packetList.size() > 0)	// The protocol will stuck if there are no events
	{
		PacketBuffer update(pcUpdateEvents), reply;

		update << PROTOCOL_VERSION;
		update << param.userID.c_str();
		update << (unsigned32)packetList.size();

		// Send the packet
		connection << update;

		// wait for reply
		connection >> reply;

		switch (reply.getCmd())
		{
		case pcWaitingForEvents:
			{
				// Send the Events
				std::list<PacketBuffer*>::iterator listIter = packetList.begin();
				for( ; listIter != packetList.end(); listIter++)
				{
					connection << (*listIter);
				}

				// wait for reply
				connection >> reply;
				reply >> error;

				AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 6));
			}
			break;

		case pcIncorrectVersion:
			AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 7));
			break;

		case pcServerBusy:
			AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 8));
			break;

		case pcError:
			AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 9));
			break;

		default:
			AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 10));
		}
	}
    return error;
}

unsigned32 Request(NetworkParams& param, ClientConnector& connection)
{
    unsigned32 error = ERROR_CODE_NONE;

	PacketBuffer request(pcRequestEvents), reply;

	AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 11));

	// Fill the package
	request << PROTOCOL_VERSION;
	// The id
	request << param.userID.c_str();
	// Send request to the server
	connection << request;

	// Wait for the events
	connection >> reply;

	switch (reply.getCmd())
	{
	case pcSendingEvents:
		{
			const char* buffer = CCalendarWindow::c_Config.GetEventsPath().c_str();
			CEventCombiner combiner;
			combiner.ReadEvents(buffer);
			unsigned32 eventCount;
			bool newEventsFound = false;

			reply >> eventCount;

			// Let the server know that we're ready to recieve
			PacketBuffer wait(pcWaitingForEvents);
			connection << wait;

			for (int i = 0; i < eventCount; i++)
			{
				PacketBuffer event;
				connection >> event;	// Get the events from server

				if (event.getCmd() == pcEvent)
				{
					if(combiner.DecodePacket(&event, NULL, 0))
					{
						newEventsFound = true;
					}
				}
				else if (event.getCmd() == pcFiltered)
				{
					// Do nothing
				}
				else
				{
					AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 12));
				}
			}

			if (newEventsFound)
			{
				combiner.WriteEvents(buffer);	 // Write the new events if new ones are received
                error = ERROR_CODE_NEWEVENTS;
			}
			AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 13));
		}
		break;

	case pcIncorrectVersion:
		AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 7));
		break;

	case pcServerBusy:
		AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 8));
		break;

	case pcError:
		AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 9));
		break;

	default:
		AddStatusString(CCalendarWindow::c_Language.GetString("NetStatus", 10));
	}

    return error;
}


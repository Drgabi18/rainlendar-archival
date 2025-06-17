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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/NetworkThread.cpp,v 1.5 2002/05/30 18:26:03 rainy Exp $

  $Log: NetworkThread.cpp,v $
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

void Update(NetworkParams& param, ClientConnector& connection);
void Request(NetworkParams& param, ClientConnector& connection);

DWORD WINAPI NetworkThreadProc(LPVOID pParam)
{
	char tmpSz[256];
	std::string string;

	NetworkParams param = *(NetworkParams*)pParam;	// Take a copy of the parameters

	if (param.userID.empty())
	{
		param.userID = "Rainlendar";	// Gotta have a name
	}

	string = "Connecting to ";
	string += param.serverAddress;
	string += ":";
	itoa(param.serverPort, tmpSz, 10);
	string += tmpSz;
	string += " (";
	string += param.userID;
	string += ")";
	AddStatusString(string.c_str());

	try
	{
		ClientConnector connection(param.serverAddress.c_str(), param.serverPort);
		AddStatusString("Connection successful");

		switch (param.requestType)
		{
		case REQUEST_SYNCRONIZATION:
			Update(param, connection);	// Send all events to the server
			Request(param, connection);	// Get events from the server
			break;

		case REQUEST_SENDEVENTS:
			Update(param, connection);	// Send all events to the server
			break;

		case REQUEST_GETEVENTS:
			Request(param, connection);
			break;

		default:
			AddStatusString("Unknown request type");
		}
	}
	catch(GeneralException& e)
	{
		string = "Connection failed: ";
		string += e.getErrorMsg();
		AddStatusString(string.c_str());
	}

	AddStatusString("Network thread completed successfully");

    return 0;   // thread completed successfully
}

void Update(NetworkParams& param, ClientConnector& connection)
{
	// Read Events.ini and send it to the server
	// Wait for confirmation from server
	const char* buffer = CCalendarWindow::c_Config.GetEventsPath().c_str();
	
	AddStatusString("Sending the events to the server");

	CEventCombiner combiner;
	combiner.ReadEvents(buffer);
	std::list<PacketBuffer*>& packetList = combiner.EncodePackets();

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
			AddStatusString("Update successful");
		}
		break;

	case pcIncorrectVersion:
		AddStatusString("The server uses a different protocol version than the client");
		break;

	case pcServerBusy:
		AddStatusString("The server is busy");
		break;

	default:
		AddStatusString("Unknown reply received from the server");
	}
}

void Request(NetworkParams& param, ClientConnector& connection)
{
	PacketBuffer request(pcRequestEvents), reply;

	AddStatusString("Requesting the events from the server");

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

				if(combiner.DecodePacket(&event))
				{
					newEventsFound = true;
				}
			}

			if (newEventsFound)
			{
				combiner.WriteEvents(buffer);	 // Write the new events if new ones are received

				// Refresh the calendar
				if (param.window)
				{
					SendMessage(param.window, WM_COMMAND, ID_REFRESH, 0);
				}
			}
			AddStatusString("Request successful");
		}
		break;

	case pcIncorrectVersion:
		AddStatusString("The server uses a different protocol version than the client");
		break;

	case pcServerBusy:
		AddStatusString("The server is busy");
		break;

	default:
		AddStatusString("Unknown reply received from the server");
	}
}


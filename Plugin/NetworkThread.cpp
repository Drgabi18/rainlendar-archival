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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/NetworkThread.cpp,v 1.3 2002/01/29 17:34:38 rainy Exp $

  $Log: NetworkThread.cpp,v $
  Revision 1.3  2002/01/29 17:34:38  rainy
  Displays the ID in the lid.

  Revision 1.2  2002/01/27 16:20:45  rainy
  Added header comment

*/

#include "stdafx.h"
#include "../Server/EventCombiner.h"
#include "rainlendardll.h"
#include "NetworkThread.h"
#include "CalendarWindow.h"

using namespace ssobjects;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void Update(NetworkParams& param, ClientConnector& connection);
void Request(NetworkParams& param, ClientConnector& connection);

UINT NetworkThreadProc(LPVOID pParam)
{
	CString string;
	NetworkParams param = *(NetworkParams*)pParam;	// Take a copy of the parameters
	if (param.userID.IsEmpty())
	{
		param.userID = "Rainlendar";	// Gotta have a name
	}

	string.Format("Connecting to %s:%i (%s)", param.serverAddress, param.serverPort, param.userID);
	param.dialogServer->AddStatusString(string);

	try
	{
		ClientConnector connection(param.serverAddress, param.serverPort);
		param.dialogServer->AddStatusString("Connection successful");

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
			param.dialogServer->AddStatusString("Unknown request type");
		}
	}
	catch(GeneralException& e)
	{
		string.Format("Connection failed: %s", e.getErrorMsg());
		param.dialogServer->AddStatusString(string);
	}

	param.dialogServer->AddStatusString("Network thread completed successfully");

    return 0;   // thread completed successfully
}

void Update(NetworkParams& param, ClientConnector& connection)
{
	// Read Events.ini and send it to the server
	// Wait for confirmation from server
	char buffer[MAX_LINE_LENGTH];
	char* Slash;
	
	// Get the DLL's directory
	GetModuleFileName(GetModuleHandle("Rainlendar.dll"), buffer, MAX_LINE_LENGTH);

	Slash = strrchr(buffer, '\\');
	if (Slash == NULL) 
	{
		strcpy(buffer, "Events.ini");
	} 
	else 
	{
		strcpy(Slash, "\\Events.ini");
	}

	param.dialogServer->AddStatusString("Sending the events to the server");

	CEventCombiner combiner;
	combiner.ReadEvents(buffer);
	std::list<PacketBuffer*>& packetList = combiner.EncodePackets();

	PacketBuffer update(pcUpdateEvents), reply;

	update << PROTOCOL_VERSION;
	update << param.userID;
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
			param.dialogServer->AddStatusString("Update successful");
		}
		break;

	case pcIncorrectVersion:
		param.dialogServer->AddStatusString("The server uses a different protocol version than the client");
		break;

	case pcServerBusy:
		param.dialogServer->AddStatusString("The server is busy");
		break;

	default:
		param.dialogServer->AddStatusString("Unknown reply received from the server");
	}
}

void Request(NetworkParams& param, ClientConnector& connection)
{
	PacketBuffer request(pcRequestEvents), reply;

	param.dialogServer->AddStatusString("Requesting the events from the server");

	// Fill the package
	request << PROTOCOL_VERSION;
	// The id
	request << param.userID;
	// Send request to the server
	connection << request;

	// Wait for the events
	connection >> reply;

	switch (reply.getCmd())
	{
	case pcSendingEvents:
		{
			char buffer[MAX_LINE_LENGTH];
			char* Slash;
			
			// Get the DLL's directory
			GetModuleFileName(GetModuleHandle("Rainlendar.dll"), buffer, MAX_LINE_LENGTH);

			Slash = strrchr(buffer, '\\');
			if (Slash == NULL) 
			{
				strcpy(buffer, "Events.ini");
			} 
			else 
			{
				strcpy(Slash, "\\Events.ini");
			}

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
			param.dialogServer->AddStatusString("Request successful");
		}
		break;

	case pcIncorrectVersion:
		param.dialogServer->AddStatusString("The server uses a different protocol version than the client");
		break;

	case pcServerBusy:
		param.dialogServer->AddStatusString("The server is busy");
		break;

	default:
		param.dialogServer->AddStatusString("Unknown reply received from the server");
	}
}


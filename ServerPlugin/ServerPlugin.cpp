/*
  Copyright (C) 2004 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/ServerPlugin/ServerPlugin.cpp,v 1.2 2004/12/05 18:18:31 rainy Exp $

  $Log: ServerPlugin.cpp,v $
  Revision 1.2  2004/12/05 18:18:31  rainy
  Added new event handling.

  Revision 1.1  2004/11/06 13:37:33  rainy
  Initial version

*/

#pragma warning(disable: 4786)

#include "../plugin/RainlendarAPI.h"
#include "resource.h"
#include "ServerPlugin.h"
#include <string>
#include <vector>
#include <list>
#include <tchar.h>
#include <ssobjects.h>
#include <ClientConnector.h>

using namespace ssobjects;

#ifdef UNICODE
#define tstring wstring
#else
#define tstring string 
#endif

HINSTANCE g_Instance = NULL;
UINT g_ID = 0;
std::tstring g_UserID;
std::tstring g_ServerAddress;
UINT g_ServerPort = 9999;
bool g_ServerPoll = false;
UINT g_ServerPollFrequency = 60;
HANDLE g_Thread = NULL;
std::list<PacketBuffer*> g_PacketList;
CRITICAL_SECTION g_PacketListCriticalSection; 
static std::string g_ConfigFile;

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

enum SIGNALS
{
	SIGNAL_REQUEST = 0,
	SIGNAL_SEND,
	SIGNAL_COUNT
};

HANDLE g_Signals[SIGNAL_COUNT] = {0};

DWORD WINAPI NetworkThreadProc(LPVOID pParam);
void DebugLog(const char* format, ... );
RainlendarItem* DecodePacket(PacketBuffer& packet);
void EncodePackets(GUID** guids);
void RequestEventsFromServer(GUID* guid);
void SendEventsToServer();
PacketBuffer* EncodePacket(RainlendarItem* item);
void ReadSettings(LPCTSTR configFile);

const unsigned32 PROTOCOL_VERSION = 400;

LPCTSTR Plugin_GetName()
{
	return _T("Rainlendar Server");
}

LPCTSTR Plugin_GetAuthor()
{
	return _T("Rainy (rainy@iki.fi)");
}

UINT Plugin_GetVersion()
{
	return 1002;	// Rainlendar 0.21
}

void Plugin_Initialize(HINSTANCE instance, LPCTSTR configFile, UINT id)
{
	g_Instance = instance;
	g_ID = id;
	g_ConfigFile = configFile;

	// Create signal event
	g_Signals[SIGNAL_REQUEST] = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_Signals[SIGNAL_SEND] = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Initialize the critical section.
	InitializeCriticalSection(&g_PacketListCriticalSection); 
}

void Plugin_Finalize()
{
	Plugin_Disable();

	for (int i = 0; i < SIGNAL_COUNT; i++)
	{
		if (g_Signals[i]) 
		{
			CloseHandle(g_Signals[i]);
			g_Signals[i] = NULL;
		}
	}

	// Request ownership of the critical section.
	try 
	{
		EnterCriticalSection(&g_PacketListCriticalSection); 

		std::list<PacketBuffer*>::iterator listIter = g_PacketList.begin();
		for( ; listIter != g_PacketList.end(); listIter++)
		{
			delete (*listIter);
		}
		g_PacketList.clear();

		// Release ownership of the critical section.
		LeaveCriticalSection(&g_PacketListCriticalSection);
	}
	catch(...)
	{
		// Release ownership of the critical section.
		LeaveCriticalSection(&g_PacketListCriticalSection);
	}

	DeleteCriticalSection(&g_PacketListCriticalSection);
}

void Plugin_Enable()
{
	ReadSettings(g_ConfigFile.c_str());

	if (g_Thread == NULL)
	{
		// Launch the network communication thread
		DWORD threadId;
		g_Thread = CreateThread(NULL, 0, NetworkThreadProc, NULL, 0, &threadId);
	}
}

void Plugin_Disable()
{
	if (g_Thread) 
	{
		// Kill the thread (hopefully its not doing anything at the moment...)
		TerminateThread(g_Thread, 0);
		CloseHandle(g_Thread);
		g_Thread = NULL;
	}
}

void ReadSettings(LPCTSTR configFile)
{
	TCHAR buffer[MAX_PATH];

	// Read settings from configFile
	if(GetPrivateProfileString(PLUGIN_NAME, _T("UserID"), "Rainlendar", buffer, MAX_PATH, configFile) > 0) 
	{
		g_UserID = buffer;
	}
	else
	{
		g_UserID = "";
	}

	if(GetPrivateProfileString(PLUGIN_NAME, _T("ServerAddress"), "localhost", buffer, MAX_PATH, configFile) > 0) 
	{
		g_ServerAddress = buffer;
	}
	else
	{
		g_ServerAddress = "";
	}

	g_ServerPort = GetPrivateProfileInt(PLUGIN_NAME, _T("ServerPort"), 9999, configFile);

	g_ServerPoll = (1 == GetPrivateProfileInt(PLUGIN_NAME, _T("ServerPoll"), 0, configFile));
	g_ServerPollFrequency = GetPrivateProfileInt(PLUGIN_NAME, _T("ServerPollFrequency"), 60, configFile);
}

void Plugin_ReadItems()
{
	SetEvent(g_Signals[SIGNAL_REQUEST]);		// Request all events from server
}

void Plugin_ItemNotification(GUID** guids)
{
	// Encode packets and put them in queue
	EncodePackets(guids);

	SetEvent(g_Signals[SIGNAL_SEND]);		// Send events to server
}

bool Plugin_ShowConfig(HWND hwndParent, LPCTSTR configFile)
{
	ReadSettings(configFile);

	if (DialogBox(g_Instance, MAKEINTRESOURCE(IDD_SERVER_DIALOG), hwndParent, (DLGPROC)SetupProc) == IDOK) 
	{
		char tmpSz[256];

		WritePrivateProfileString(PLUGIN_NAME, _T("UserID"), g_UserID.c_str(), configFile);
		WritePrivateProfileString(PLUGIN_NAME, _T("ServerAddress"), g_ServerAddress.c_str(), configFile);

		sprintf(tmpSz, "%i", g_ServerPort);
		WritePrivateProfileString(PLUGIN_NAME, _T("ServerPort"), tmpSz, configFile);

		sprintf(tmpSz, "%i", g_ServerPoll);
		WritePrivateProfileString(PLUGIN_NAME, _T("ServerPoll"), tmpSz, configFile);

		sprintf(tmpSz, "%i", g_ServerPollFrequency);
		WritePrivateProfileString(PLUGIN_NAME, _T("ServerPollFrequency"), tmpSz, configFile);

		if (g_Thread) 
		{
			// Kill the thread (hopefully its not doing anything at the moment...)
			TerminateThread(g_Thread, 0);
			CloseHandle(g_Thread);
			g_Thread = NULL;
		}

		// Launch the network communication thread
		DWORD threadId;
		g_Thread = CreateThread(NULL, 0, NetworkThreadProc, NULL, 0, &threadId);

		// Return true if something has changed and all events need to be reloaded.
		return true;
	}
	return false;
}

bool Plugin_EditItem(HWND parent, RainlendarItem* item, RAINLENDAR_TYPE type)
{
	// Check event from the server and open the default editor
	try
	{
		if (item) 
		{
			RequestEventsFromServer(&item->guid);
			Rainlendar_FlushItems();
			RainlendarItem* updatedItem = Rainlendar_GetItem(&item->guid);
			if (updatedItem)
			{
				item = updatedItem;
			}
		}
	}
	catch(GeneralException& e)
	{
		DebugLog(_T("%s: ERROR: %s"), PLUGIN_NAME, e.getErrorMsg());
	}

	// Just use the default editor
	if (Rainlendar_EditItemDialog(parent, item, type))
	{
		if (item->type == RAINLENDAR_TYPE_EVENT)	// Todo-list works automatically
		{
			// The event was changed
			Rainlendar_SetItem(item, g_ID);
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////


DWORD WINAPI NetworkThreadProc(LPVOID pParam)
{
    unsigned32 error = 0;
	DWORD timeout = INFINITE;

	if (g_ServerPoll && g_ServerPollFrequency > 0)
	{
		timeout = g_ServerPollFrequency * 60 * 1000;
	}

	DebugLog(_T("%s: Network thread started!"), PLUGIN_NAME);

	while (true)
	{
		DWORD res = WaitForMultipleObjects(2, g_Signals, FALSE, timeout);

		if (res == WAIT_OBJECT_0 || res == WAIT_TIMEOUT)
		{
			try
			{
				RequestEventsFromServer(NULL);
			}
			catch(GeneralException& e)
			{
				DebugLog(_T("%s: ERROR: %s"), PLUGIN_NAME, e.getErrorMsg());
			}
		}
		else if (res == WAIT_OBJECT_0 + 1)
		{
			SendEventsToServer();
		}
		else
		{
			DebugLog(_T("%s: ERROR: Unknown signal!"), PLUGIN_NAME);
		}
	}

	DebugLog(_T("%s: Network thread ended!"), PLUGIN_NAME);

    return 0;   // thread completed successfully
}

void SendEventsToServer()
{
	DebugLog("%s: Sending events to the server.", PLUGIN_NAME);

	if (g_ServerAddress.empty())
	{
		DebugLog("%s: ERROR: Server address is not defined.", PLUGIN_NAME);
		return;
	}

	if (g_UserID.empty())
	{
		DebugLog("%s: ERROR: User ID is not defined.", PLUGIN_NAME);
		return;
	}

	std::list<PacketBuffer*> packetList;

	try
	{
		// Request ownership of the critical section.
		try 
		{
			EnterCriticalSection(&g_PacketListCriticalSection); 

			// Take a local copy of the packets
			packetList = g_PacketList;
			g_PacketList.clear();

			// Release ownership of the critical section.
			LeaveCriticalSection(&g_PacketListCriticalSection);
		}
		catch(...)
		{
			// Release ownership of the critical section.
			LeaveCriticalSection(&g_PacketListCriticalSection);
		}

		if (packetList.size() > 0)	// The protocol will stuck if there are no events
		{
			ClientConnector connection(g_ServerAddress.c_str(), g_ServerPort);

			PacketBuffer update(pcUpdateEvents), reply;

			update << PROTOCOL_VERSION;
			update << g_UserID.c_str();
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
				}
				break;

			case pcIncorrectVersion:
				DebugLog("%s: ERROR: The server is using different protocol version than the client.", PLUGIN_NAME);
				break;

			case pcServerBusy:
				DebugLog("%s: ERROR: The server is busy serving another client at the moment.", PLUGIN_NAME);
				break;

			case pcError:
				DebugLog("%s: ERROR: The server returned error.", PLUGIN_NAME);
				break;

			default:
				DebugLog("%s: ERROR: Unknown reply from the server.", PLUGIN_NAME);
			}
		}
	}
	catch(GeneralException& e)
	{
		DebugLog(_T("%s: ERROR: %s"), PLUGIN_NAME, e.getErrorMsg());
	}

	std::list<PacketBuffer*>::iterator listIter = packetList.begin();
	for( ; listIter != packetList.end(); listIter++)
	{
		delete (*listIter);
	}
	packetList.clear();
}

void RequestEventsFromServer(GUID* guid)
{
	DebugLog("%s: Requesting events from the server.", PLUGIN_NAME);

	if (g_ServerAddress.empty())
	{
		DebugLog("%s: ERROR: Server address is not defined.", PLUGIN_NAME);
		return;
	}

	if (g_UserID.empty())
	{
		DebugLog("%s: ERROR: User ID is not defined.", PLUGIN_NAME);
		return;
	}

	// Request all events from the server
	ClientConnector connection(g_ServerAddress.c_str(), g_ServerPort);

	PacketBuffer request(pcRequestEvents), reply;

	// Fill the package
	request << PROTOCOL_VERSION;
	// The id
	request << g_UserID.c_str();
	
	if (guid == NULL)
	{
		request << (signed32)0;	// This means all events
	}
	else
	{
		request << (signed32)1;
		request.append((unsigned char*)guid, sizeof(GUID));
	}

	// Send request to the server
	connection << request;

	// Wait for the events
	connection >> reply;

	switch (reply.getCmd())
	{
	case pcSendingEvents:
		{
			unsigned32 eventCount;

			reply >> eventCount;

			// Let the server know that we're ready to receive
			PacketBuffer wait(pcWaitingForEvents);
			connection << wait;

			for (int i = 0; i < eventCount; i++)
			{
				PacketBuffer event;
				connection >> event;	// Get the events from server

				if (event.getCmd() == pcEvent)
				{
					RainlendarItem* item = DecodePacket(event);
					if (item)
					{
						Rainlendar_SetItem(item, g_ID);		// These must be thread-safe!
						Rainlendar_DeleteItem(item);
					}
				}
				else if (event.getCmd() != pcNotFound && event.getCmd() != pcFiltered)
				{
					DebugLog("%s: ERROR: The server send an unknown packet as pcEvent.", PLUGIN_NAME);
				}
			}

			PacketBuffer result(pcResult);
			connection << result;	// All ok
		}
		break;

	case pcIncorrectVersion:
		DebugLog("%s: ERROR: The server is using different protocol version than the client.", PLUGIN_NAME);
		break;

	case pcServerBusy:
		DebugLog("%s: ERROR: The server is busy serving another client at the moment.", PLUGIN_NAME);
		break;

	case pcError:
		DebugLog("%s: ERROR: The server returned error.", PLUGIN_NAME);
		break;

	default:
		DebugLog("%s: ERROR: Unknown reply from the server.", PLUGIN_NAME);
	}
}

RainlendarItem* DecodePacket(PacketBuffer& packet)
{
	RainlendarItem* item = NULL;
	
	unsigned char* buffer = packet.getBuffer();
	if (buffer)
	{
		// Size is the first
		DWORD size = *((DWORD*)buffer);
		if (size == sizeof(RainlendarEvent))
		{
			RainlendarEvent* newEvent = new RainlendarEvent;

			// Copy the data
			*newEvent = *((RainlendarEvent*)buffer);

			buffer = buffer + sizeof(RainlendarEvent);

			// Copy the recurrency
			if (newEvent->recurrency)
			{
				newEvent->recurrency = new RainlendarRecurrency;
				*(newEvent->recurrency) = *((RainlendarRecurrency*)buffer);

				buffer = buffer + sizeof(RainlendarRecurrency);
			}

			// Copy the strings;
			if (newEvent->header)
			{
				newEvent->header = strdup((char*)buffer);
				buffer = buffer + strlen((char*)buffer) + 1;
			}
			if (newEvent->message)
			{
				newEvent->message = strdup((char*)buffer);
				buffer = buffer + strlen((char*)buffer) + 1;
			}
			if (newEvent->profile)
			{
				newEvent->profile = strdup((char*)buffer);
				buffer = buffer + strlen((char*)buffer) + 1;
			}

			item = newEvent;
		}
		else if (size == sizeof(RainlendarTodo))
		{
			RainlendarTodo* newTodo = new RainlendarTodo;

			// Copy the data
			*newTodo = *((RainlendarTodo*)buffer);

			buffer = buffer + sizeof(RainlendarTodo);

			// Copy the strings;
			if (newTodo->message)
			{
				newTodo->message = strdup((char*)buffer);
				buffer = buffer + strlen((char*)buffer) + 1;
			}
			if (newTodo->profile)
			{
				newTodo->profile = strdup((char*)buffer);
				buffer = buffer + strlen((char*)buffer) + 1;
			}

			item = newTodo;
		}
		else
		{
			DebugLog("%s: ERROR: Something's wrong. The size of the item doesn't match.", PLUGIN_NAME);
		}
	}

	return item;
}

PacketBuffer* EncodePacket(RainlendarItem* item)
{
	PacketBuffer* newPacket = NULL;

	try
	{
		newPacket = new PacketBuffer(pcEvent);

		if (item->type == RAINLENDAR_TYPE_EVENT)
		{
			RainlendarEvent* event =  (RainlendarEvent*)item;

			// Add the raw-data
			newPacket->append((unsigned char*)item, sizeof(RainlendarEvent));

			// Add the recurrency
			if (event->recurrency) 
			{
				newPacket->append((unsigned char*)event->recurrency, sizeof(RainlendarRecurrency));
			}

			// Add the strings
			if (event->header) (*newPacket) << event->header;
			if (event->message) (*newPacket) << event->message;
			if (event->profile) (*newPacket) << event->profile;
		}
		else if (item->type == RAINLENDAR_TYPE_TODO)
		{
			RainlendarTodo* todo = (RainlendarTodo*)item;

			// Add the raw-data
			newPacket->append((unsigned char*)todo, sizeof(RainlendarTodo));

			// Add the strings
			if (todo->message) (*newPacket) << todo->message;
			if (todo->profile) (*newPacket) << todo->profile;
		}
	}
	catch(GeneralException& e)
	{
		delete newPacket;
		DebugLog(_T("%s: ERROR: Unable to encode packet: %s"), PLUGIN_NAME, e.getErrorMsg());
	}

	return newPacket;
}

void EncodePackets(GUID** guids)
{
	int i = 0;
	bool todo = false;
	bool event = false;

	while (guids[i] != NULL)
	{
		RainlendarItem* item = Rainlendar_GetItem(guids[i]);
		if (item)
		{
			PacketBuffer* newPacket = NULL;
			newPacket = EncodePacket(item);

			// Request ownership of the critical section.
			try 
			{
				EnterCriticalSection(&g_PacketListCriticalSection); 

				g_PacketList.push_back(newPacket);

				// Release ownership of the critical section.
				LeaveCriticalSection(&g_PacketListCriticalSection);
			}
			catch(...)
			{
				// Release ownership of the critical section.
				LeaveCriticalSection(&g_PacketListCriticalSection);
			}
		}

		i++;
	}

}

void DebugLog(const char* format, ... )
{
	char buffer[4096];
    va_list args;
    va_start( args, format );
    _vsnprintf( buffer, 4096, format, args );
	Rainlendar_Log(buffer);
	va_end(args);
};
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
  $Header: /home/cvsroot/Rainlendar/Server/EventCombiner.cpp,v 1.17 2004/11/14 12:11:53 rainy Exp $

  $Log: EventCombiner.cpp,v $
  Revision 1.17  2004/11/14 12:11:53  rainy
  Linux fixes

  Revision 1.16  2004/11/14 11:37:31  rainy
  Changes for 0.20.

  Revision 1.15  2003/08/23 09:15:57  Rainy
  Added some more logging.
  The date format is verified.

  Revision 1.14  2003/08/09 15:23:08  Rainy
  Added LastModified and CreatedBy fields for server.

  Revision 1.13  2003/05/25 18:10:47  Rainy
  A message is set to the client if the event is filtered.

  Revision 1.12  2003/05/25 15:11:43  Rainy
  Added fltering support for the incoming & outgoing events.

  Revision 1.11  2003/05/24 16:02:09  Rainy
  The timestamp check was wrong. fixed.

  Revision 1.10  2003/05/24 14:05:26  Rainy
  Fixed an error message.

  Revision 1.9  2003/05/07 19:12:07  rainy
  Added more logging.

  Revision 1.8  2002/08/24 11:07:36  rainy
  Stings are truncated before sent to the server.

  Revision 1.7  2002/08/06 15:29:25  rainy
  The event is now filled with default values while reading.

  Revision 1.6  2002/08/05 15:41:17  rainy
  Fixed Profile loading

  Revision 1.5  2002/08/03 16:17:23  rainy
  Changed to support repeating events.

  Revision 1.4  2002/02/27 18:14:43  rainy
  Removed size from the packages

  Revision 1.3  2002/01/29 17:35:59  rainy
  Removed one unnecessary variable.

  Revision 1.2  2002/01/29 16:24:26  rainy
  Added returns to the end of the lines when writing the ini-file.

  Revision 1.1  2002/01/27 16:21:04  rainy
  Initial version

*/

#include "EventCombiner.h"
#include "IPFilter.h"
#include "IniFile.h"

using namespace ssobjects;

#define FILE_VERSION 1
#define FILE_HEADER "RainlendarServer"

#ifndef _WIN32
#define _strdup strdup
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventCombiner::CEventCombiner()
{
}

CEventCombiner::~CEventCombiner()
{
	std::map<GUID, EventInfo*, GUIDComparator>::iterator i = m_Events.begin();
	for ( ; i != m_Events.end(); i++)
	{
		DeleteEvent((*i).second);
	}

	std::list<PacketBuffer*>::iterator listIter = m_Packets.begin();
	for( ; listIter != m_Packets.end(); listIter++)
	{
		delete (*listIter);
	}
}

bool CEventCombiner::ReadEvents(const char* filename)
{
	CIniFile file(filename, FILE_HEADER);

	if (file.LoadFile())
	{
		for (int i = 0; i < file.GetSectionCount(); i++)
		{
			const char* data = NULL;
			EventInfo* info = new EventInfo;
			RainlendarItem* item = NULL;
			RAINLENDAR_TYPE type;
			bool valid = false;

			// Common items
			data = file.GetSectionData(i, "type");
			if (data)
			{
				type = (RAINLENDAR_TYPE)atoi(data);
				if (type == RAINLENDAR_TYPE_EVENT)
				{
					item = new RainlendarEvent;
					memset(item, 0, sizeof(RainlendarEvent));
					item->size = sizeof(RainlendarEvent);
				}
				else if (type == RAINLENDAR_TYPE_TODO)
				{
					item = new RainlendarTodo;
					memset(item, 0, sizeof(RainlendarTodo));
					item->size = sizeof(RainlendarTodo);
				}
				else
				{
					delete info;
					LOG("Unknown item type (%i), unable to read it.", type);
					continue;		// Unknown type
				}

				item->type = type;

				data = file.GetSectionData(i, "guid");
				if (data)
				{
					item->guid = GuidFromString(data);
					valid = true;
				}
				else
				{
					LOG("Section [%i] is not a valid item.", file.GetSectionName(i));
				}
			}

			if (valid)
			{
				data = file.GetSectionData(i, "deleted");
				if (data)
				{
					item->deleted = (atoi(data) == 1);
				}

				data = file.GetSectionData(i, "timeStamp");
				if (data)
				{
					item->timeStamp = atoi(data);
				}

				data = file.GetSectionData(i, "createdBy");
				if (data)
				{
					info->createdBy = CIPNumber(data);
				}

				data = file.GetSectionData(i, "lastModified");
				if (data)
				{
					info->lastModified = CIPNumber(data);
				}

				if (type == RAINLENDAR_TYPE_EVENT)
				{
					RainlendarEvent* event = (RainlendarEvent*)item;
					item->size = sizeof(RainlendarEvent);

					data = file.GetSectionData(i, "header");
					if (data)
					{
						event->header = _strdup(data);
					}
					
					data = file.GetSectionData(i, "message");
					if (data)
					{
						event->message = _strdup(data);
					}

					data = file.GetSectionData(i, "profile");
					if (data)
					{
						event->profile = _strdup(data);
					}
					
					data = file.GetSectionData(i, "startTime");
					if (data)
					{
						sscanf(data, "%u:%u", &event->startTime.dwHighDateTime, &event->startTime.dwLowDateTime);
					}

					data = file.GetSectionData(i, "endTime");
					if (data)
					{
						sscanf(data, "%u:%u", &event->endTime.dwHighDateTime, &event->endTime.dwLowDateTime);
					}

					data = file.GetSectionData(i, "allDayEvent");
					if (data)
					{
						event->allDayEvent = (atoi(data) == 1);
					}

					data = file.GetSectionData(i, "alarm");
					if (data)
					{
						event->alarm = atoi(data);
					}
					
					data = file.GetSectionData(i, "recurrencyType");
					if (data)
					{
						RainlendarRecurrency* recurrency = new RainlendarRecurrency;
						memset(recurrency, 0, sizeof(RainlendarRecurrency));
						recurrency->size = sizeof(RainlendarRecurrency);

						recurrency->type = (RECURRENCY_TYPE)atoi(data);

						data = file.GetSectionData(i, "recurrencyFrequency");
						if (data)
						{
							recurrency->frequency = atoi(data);
						}

						data = file.GetSectionData(i, "recurrencyDays");
						if (data)
						{
							recurrency->days = atoi(data);
						}

						data = file.GetSectionData(i, "recurrencyRepeatType");
						if (data)
						{
							recurrency->repeatType = (RECURRENCY_REPEAT)atoi(data);
						}

						data = file.GetSectionData(i, "recurrencyCount");
						if (data)
						{
							recurrency->count = atoi(data);
						}

						data = file.GetSectionData(i, "recurrencyUntil");
						if (data)
						{
							sscanf(data, "%u:%u", &recurrency->until.dwHighDateTime, &recurrency->until.dwLowDateTime);
						}

						event->recurrency = recurrency;
					}
				}
				else if (type == RAINLENDAR_TYPE_TODO)
				{
					RainlendarTodo* todo = (RainlendarTodo*)item;
					item->size = sizeof(RainlendarTodo);

					data = file.GetSectionData(i, "todoType");
					if (data)
					{
						todo->todoType = (TODO_TYPE)atoi(data);
					}

					data = file.GetSectionData(i, "position");
					if (data)
					{
						todo->position = atoi(data);
					}

					data = file.GetSectionData(i, "checked");
					if (data)
					{
						todo->checked = (atoi(data) == 1);
					}
				
					data = file.GetSectionData(i, "message");
					if (data)
					{
						todo->message = _strdup(data);
					}

					data = file.GetSectionData(i, "profile");
					if (data)
					{
						todo->profile = _strdup(data);
					}
				}

				item->readOnly = false;

				info->item = item;
				AddEvent(info);
			}
			else
			{
				delete item;
				delete info;
			}
		}
	}
	else
	{
		LOG("Unable to load file: %s", filename);
		return false;
	}

	return true;
}

bool CEventCombiner::WriteEvents(const char* filename)
{
	char buffer[256];
	CIniFile file(filename, FILE_HEADER);
	file.SetFileVersion(FILE_VERSION);

	std::map<GUID, EventInfo*, GUIDComparator>::iterator iter = m_Events.begin();
	for ( ; iter != m_Events.end(); iter++)
	{
		EventInfo* info = (*iter).second;

		if (info && info->item)
		{
			int index;
			if (info->item->type == RAINLENDAR_TYPE_EVENT)
			{
				index = file.CreateSection("Event");
			}
			else if (info->item->type == RAINLENDAR_TYPE_TODO)
			{
				index = file.CreateSection("Todo");
			}
			else
			{
				index = file.CreateSection("Unknown");
			}

			sprintf(buffer, "%i", info->item->type);
			file.AddSectionData(index, "type", buffer);

			file.AddSectionData(index, "guid", GuidToString(info->item->guid));

			sprintf(buffer, "%i", info->item->deleted);
			file.AddSectionData(index, "deleted", buffer);

			sprintf(buffer, "%i", info->item->timeStamp);
			file.AddSectionData(index, "timeStamp", buffer);

			file.AddSectionData(index, "createdBy", info->createdBy.GetString());
			file.AddSectionData(index, "lastModified", info->lastModified.GetString());

			if (info->item->type == RAINLENDAR_TYPE_EVENT)
			{
				RainlendarEvent* event = (RainlendarEvent*)info->item;

				file.AddSectionData(index, "profile", event->profile);

				if (!event->deleted)
				{
					file.AddSectionData(index, "header", event->header);
					file.AddSectionData(index, "message", event->message);

					sprintf(buffer, "%u:%u", event->startTime.dwHighDateTime, event->startTime.dwLowDateTime);
					file.AddSectionData(index, "startTime", buffer);

					sprintf(buffer, "%u:%u", event->endTime.dwHighDateTime, event->endTime.dwLowDateTime);
					file.AddSectionData(index, "endTime", buffer);

					sprintf(buffer, "%i", event->allDayEvent);
					file.AddSectionData(index, "allDayEvent", buffer);

					sprintf(buffer, "%i", event->alarm);
					file.AddSectionData(index, "alarm", buffer);

					if (event->recurrency)
					{
						sprintf(buffer, "%i", event->recurrency->type);
						file.AddSectionData(index, "recurrencyType", buffer);

						sprintf(buffer, "%i", event->recurrency->frequency);
						file.AddSectionData(index, "recurrencyFrequency", buffer);

						sprintf(buffer, "%i", event->recurrency->days);
						file.AddSectionData(index, "recurrencyDays", buffer);

						sprintf(buffer, "%i", event->recurrency->repeatType);
						file.AddSectionData(index, "recurrencyRepeatType", buffer);

						sprintf(buffer, "%i", event->recurrency->count);
						file.AddSectionData(index, "recurrencyCount", buffer);

						sprintf(buffer, "%u:%u", event->recurrency->until.dwHighDateTime, event->recurrency->until.dwLowDateTime);
						file.AddSectionData(index, "recurrencyUntil", buffer);
					}
				}
			}
			else if (info->item->type == RAINLENDAR_TYPE_TODO)
			{
				RainlendarTodo* todo = (RainlendarTodo*)info->item;

				file.AddSectionData(index, "profile", todo->profile);

				if (!todo->deleted)
				{
					file.AddSectionData(index, "message", todo->message);

					sprintf(buffer, "%i", todo->todoType);
					file.AddSectionData(index, "todoType", buffer);

					sprintf(buffer, "%i", todo->position);
					file.AddSectionData(index, "position", buffer);

					sprintf(buffer, "%i", todo->checked);
					file.AddSectionData(index, "checked", buffer);
				}
			}
			else
			{
				LOG("Unknown item type (%i), unable to write it.", info->item->type);
			}
		}
	}

	if (!file.SaveFile())
	{
		LOG("Unable to save file: %s", filename);
		return false;
	}

	return true;
}

bool CEventCombiner::AddEvent(EventInfo* newEvent)
{
	if (newEvent && newEvent->item)
	{
		std::map<GUID, EventInfo*, GUIDComparator>::iterator i = m_Events.find(newEvent->item->guid);

		if (i != m_Events.end())
		{
			// The event is already in the map
			if ((*i).second->item->timeStamp < newEvent->item->timeStamp)
			{
				// Overwrite the event cooz it's newer
				newEvent->createdBy = (*i).second->createdBy;	// Preserve the created by value
				DeleteEvent((*i).second);
				m_Events[newEvent->item->guid] = newEvent;
				return true;
			}
			else
			{
				// Event's timestamp is less or equal -> Delete it
				DeleteEvent(newEvent);
				return false;
			}
		}
		else
		{
			// Event not found -> Add it

			newEvent->createdBy = newEvent->lastModified;
			m_Events[newEvent->item->guid] = newEvent;
			return true;
		}
	}
	else
	{
		LOG("Tried to add an empty event.");
	}

	return false;
}

std::list<PacketBuffer*>& CEventCombiner::EncodePackets(std::vector<GUID>& requestedEvents, CIPFilter* filter, ULONG ipAddr)
{
	// First clear old packages
	std::list<PacketBuffer*>::iterator listIter = m_Packets.begin();
	for( ; listIter != m_Packets.end(); listIter++)
	{
		delete (*listIter);
	}
	m_Packets.clear();

	PacketBuffer* newPacket = NULL;

	if (requestedEvents.size() > 0)
	{
		for (int i = 0; i < requestedEvents.size(); i++)
		{
			std::map<GUID, EventInfo*, GUIDComparator>::iterator iter = m_Events.find(requestedEvents[i]);
			if (iter != m_Events.end())
			{
				newPacket = EncodePacket((*iter).second, filter, ipAddr);
			}
			else
			{
				newPacket = new PacketBuffer(pcNotFound);
				LOG("Requested event %s was not found.", GuidToString(requestedEvents[i]));
			}
			m_Packets.push_back(newPacket);
		}
	}
	else
	{
		// Encode all packages
		std::map<GUID, EventInfo*, GUIDComparator>::iterator iter = m_Events.begin();
		for ( ; iter != m_Events.end(); iter++)
		{
			newPacket = EncodePacket((*iter).second, filter, ipAddr);
			m_Packets.push_back(newPacket);
		}
	}
	return m_Packets;
}

PacketBuffer* CEventCombiner::EncodePacket(EventInfo* info, CIPFilter* filter, ULONG ipAddr)
{
	PacketBuffer* newPacket = NULL;

	if (info && info->item)
	{
		char* profile = NULL;
		newPacket = new PacketBuffer(pcEvent);

		if (info->item->type == RAINLENDAR_TYPE_EVENT)
		{
			RainlendarEvent* event = (RainlendarEvent*)info->item;

			// Add the raw-data
			newPacket->append((unsigned char*)event, sizeof(RainlendarEvent));

			// Add the recurrency
			if (event->recurrency) 
			{
				newPacket->append((unsigned char*)event->recurrency, sizeof(RainlendarRecurrency));
			}

			// Add the strings
			if (event->header) (*newPacket) << event->header;
			if (event->message) (*newPacket) << event->message;
			if (event->profile) (*newPacket) << event->profile;

			profile = event->profile;
		}
		else if (info->item->type == RAINLENDAR_TYPE_TODO)
		{
			RainlendarTodo* todo = (RainlendarTodo*)info->item;

			// Add the raw-data
			newPacket->append((unsigned char*)todo, sizeof(RainlendarTodo));

			// Add the strings
			if (todo->message) (*newPacket) << todo->message;
			if (todo->profile) (*newPacket) << todo->profile;

			profile = todo->profile;
		}

		if (filter)
		{
			if (!filter->CheckOutgoing(CIPNumber(ipAddr, 32), profile))
			{
				LOG("The outgoing event %s was filtered (rule: %s).", GuidToString(info->item->guid), filter->GetRule());
				delete newPacket;
				newPacket = new PacketBuffer(pcFiltered);
			}
		}
	}
	return newPacket;
}

bool CEventCombiner::DecodePacket(PacketBuffer* packet, CIPFilter* filter, ULONG ipAddr)
{
	EventInfo* info = new EventInfo;

	unsigned char* buffer = packet->getBuffer();
	if (buffer)
	{
		char* profile = NULL;

		RainlendarItem* item = NULL;

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
				newEvent->header = _strdup((char*)buffer);
				buffer = buffer + strlen((char*)buffer) + 1;
			}
			if (newEvent->message)
			{
				newEvent->message = _strdup((char*)buffer);
				buffer = buffer + strlen((char*)buffer) + 1;
			}
			if (newEvent->profile)
			{
				newEvent->profile = _strdup((char*)buffer);
				buffer = buffer + strlen((char*)buffer) + 1;
			}

			item = newEvent;
			profile = newEvent->profile;
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
				newTodo->message = _strdup((char*)buffer);
				buffer = buffer + strlen((char*)buffer) + 1;
			}
			if (newTodo->profile)
			{
				newTodo->profile = _strdup((char*)buffer);
				buffer = buffer + strlen((char*)buffer) + 1;
			}

			item = newTodo;
			profile = newTodo->profile;
		}
		else
		{
			LOG("Something's wrong. The size of the item %s doesn't match.", GuidToString(item->guid));
		}

		info->item = item;

		if (filter)
		{
			if (!filter->CheckIncoming(CIPNumber(ipAddr, 32), profile))
			{
				LOG("The incoming event %s was filtered (rule: %s).", GuidToString(item->guid), filter->GetRule());
				DeleteEvent(info);
				return false;
			}
		}

		info->lastModified = CIPNumber(ipAddr, 255);

		return AddEvent(info);
	}

	return false;
}

bool CEventCombiner::CheckFilename(std::string fileName)
{
	// Make sure that the ID doesn't include illegal characters
	if (fileName.find('/') != -1 ||
		fileName.find('\\') != -1 || 
		fileName.find(':') != -1 || 
		fileName.find('*') != -1 || 
		fileName.find('?') != -1 || 
		fileName.find('\"') != -1 || 
		fileName.find('<') != -1 || 
		fileName.find('>') != -1 || 
		fileName.find('|') != -1)
	{
		return false;
	}
	return true;
}

//    unsigned long  Data1;
//    unsigned short Data2;
//    unsigned short Data3;
//    byte           Data4[ 8 ];
// {21081121-E1E3-4696-B606-D8A5403972DE}

const char* CEventCombiner::GuidToString(GUID guid)
{
	static char buffer[256];

	unsigned char* data = (unsigned char*)&guid;

	for (int i = 0; i < sizeof(GUID); i++)
	{
		sprintf(buffer + (i * 3), "%02X ", data[i]); 
	}

	return buffer;
}

GUID CEventCombiner::GuidFromString(const char* str)
{
	GUID guid;
	unsigned char* data = (unsigned char*)&guid;

	memset(&guid, 0, sizeof(GUID));

	if (str && strlen(str) >= sizeof(GUID) * 3 - 1)
	{
		for (int i = 0; i < sizeof(GUID); i++)
		{
			int val = 0;
			sscanf(str + i * 3, "%X", &val);
			data[i] = val;
		}
	}

	return guid;
}

// From RainlendarAPI.cpp
void CEventCombiner::DeleteEvent(EventInfo* deletedEvent)
{
	RainlendarItem* item = deletedEvent->item;

	if (item->type == RAINLENDAR_TYPE_EVENT)
	{
		RainlendarEvent* event = (RainlendarEvent*)item;

		if (event->header)
		{
			delete event->header;
		}
		if (event->message)
		{
			delete event->message;
		}
		if (event->profile)
		{
			delete event->profile;
		}
		if (event->recurrency)
		{
			delete event->recurrency;
		}
		delete event;
	}	
	else if (item->type == RAINLENDAR_TYPE_TODO)
	{
		RainlendarTodo* todo = (RainlendarTodo*)item;

		if (todo->message)
		{
			delete todo->message;
		}
		if (todo->profile)
		{
			delete todo->profile;
		}
		delete todo;
	}
	else
	{
		LOG("Unknown item type (%i), unable to delete it.", item->type);
	}

	delete deletedEvent;
}

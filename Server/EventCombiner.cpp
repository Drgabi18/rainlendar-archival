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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Server/EventCombiner.cpp,v 1.8 2002/08/24 11:07:36 rainy Exp $

  $Log: EventCombiner.cpp,v $
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

using namespace ssobjects;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventCombiner::CEventCombiner()
{
}

CEventCombiner::~CEventCombiner()
{
	std::map<std::string, EventInfo*>::iterator i = m_Events.begin();

	for ( ; i != m_Events.end(); i++)
	{
		delete (*i).second;
	}

	std::list<PacketBuffer*>::iterator listIter = m_Packets.begin();
	for( ; listIter != m_Packets.end(); listIter++)
	{
		delete (*listIter);
	}
}

bool CEventCombiner::ReadEvents(const char* filename)
{
	char buffer[1100];
	EventInfo* newEvent = NULL;
	std::string date;

	FILE* file = fopen(filename, "r");

	if (file)
	{
		while (feof(file) == 0)
		{
			if (fgets(buffer, 1100, file) != NULL)
			{
				if (strlen(buffer) > 0)
				{
					// Remove the \n from the end
					buffer[strlen(buffer) - 1] = 0;

					if (buffer[0] == '[')
					{
						AddEvent(date, newEvent);
						newEvent = new EventInfo;
						newEvent->stamp = 0;
						newEvent->type = 0;
						newEvent->profile = "Default";
						newEvent->deleted = 0;
						newEvent->validUntil = 0;
						newEvent->everyNth = 0;
						date = buffer;
					}
					else
					{
						if (newEvent) 
						{
							if (strncmp(buffer, "Message=", 8) == 0)
							{
								newEvent->message = buffer + 8;

								// Trim the message
								newEvent->message.erase(newEvent->message.find_last_not_of(" ") + 1);
							}
							else if (strncmp(buffer, "TimeStamp=", 10) == 0)
							{
								newEvent->stamp = atoi(buffer + 10);
							}
							else if (strncmp(buffer, "Type=", 5) == 0)
							{
								newEvent->type = atoi(buffer + 5);
							}
							else if (strncmp(buffer, "Profile=", 8) == 0)
							{
								newEvent->profile = buffer + 8;
								// Trim the profile
								newEvent->profile.erase(newEvent->profile.find_last_not_of(" ") + 1);
							}
							else if (strncmp(buffer, "Deleted=", 8) == 0)
							{
								newEvent->deleted = atoi(buffer + 8) == 1 ? true : false;
							}
							else if (strncmp(buffer, "ValidUntil=", 11) == 0)
							{
								newEvent->validUntil = atoi(buffer + 11);
							}
							else if (strncmp(buffer, "EveryNth=", 9) == 0)
							{
								newEvent->everyNth = atoi(buffer + 9);
							}
						}
					}
				}
			}
		}
	
		// Add the last event to the map
		AddEvent(date, newEvent);
		fclose(file);
	}
	else
	{
		return false;
	}

	return true;
}

bool CEventCombiner::WriteEvents(const char* filename)
{
	char buffer[1100];
	std::string date;

#ifdef SERVER
	// Server doesn't use Windows specific ini-file functions

	FILE* file = fopen(filename, "w");

	if (file)
	{
		std::map<std::string, EventInfo*>::iterator i = m_Events.begin();

		for ( ; i != m_Events.end(); i++)
		{
			strcpy(buffer, (*i).first.c_str());
			strcat(buffer, "\n");
			fputs(buffer, file);	// The date

			if ((*i).second->deleted == 0)
			{
				// No need to write these if the event is deleted
				strcpy(buffer, "Message=");
				strcat(buffer, (*i).second->message.c_str());
				strcat(buffer, "\n");
				fputs(buffer, file);

				strcpy(buffer, "Profile=");
				strcat(buffer, (*i).second->profile.c_str());
				strcat(buffer, "\n");
				fputs(buffer, file);

				strcpy(buffer, "Type=");
				sprintf(buffer + 5, "%u\n", (*i).second->type);
				fputs(buffer, file);

				strcpy(buffer, "ValidUntil=");
				sprintf(buffer + 11, "%u\n", (*i).second->validUntil);
				fputs(buffer, file);

				strcpy(buffer, "EveryNth=");
				sprintf(buffer + 9, "%u\n", (*i).second->everyNth);
				fputs(buffer, file);
			}
			strcpy(buffer, "Deleted=");
			sprintf(buffer + 8, "%u\n", (*i).second->deleted);
			fputs(buffer, file);

			strcpy(buffer, "TimeStamp=");
			sprintf(buffer + 10, "%u\n", (*i).second->stamp);
			fputs(buffer, file);
		}

		fclose(file);
	}
	else
	{
		return false;
	}

#else

	char stamp[256];
	std::map<std::string, EventInfo*>::iterator i = m_Events.begin();
	
	for ( ; i != m_Events.end(); i++)
	{
		// Remove [ and ] from the date
		if((*i).first.size() > 2)
		{
			strcpy(buffer, (*i).first.c_str() + 1);
			buffer[strlen(buffer) - 1] = 0;

			// We'll use these so that the local settings are not overwritten

			if ((*i).second->deleted == 1)
			{
				// Remove all entries	
				WritePrivateProfileString( buffer, "Type", NULL, filename );
				WritePrivateProfileString( buffer, "Message", NULL, filename );
				WritePrivateProfileString( buffer, "Profile", NULL, filename );
				WritePrivateProfileString( buffer, "ValidUntil", NULL, filename );
				WritePrivateProfileString( buffer, "EveryNth", NULL, filename );
			}
			else
			{
				WritePrivateProfileString( buffer, "Message", (*i).second->message.c_str(), filename );
				sprintf(stamp, "%u", (*i).second->type);
				WritePrivateProfileString( buffer, "Type", stamp, filename );
				WritePrivateProfileString( buffer, "Profile", (*i).second->profile.c_str(), filename );
				sprintf(stamp, "%u", (*i).second->validUntil);
				WritePrivateProfileString( buffer, "ValidUntil", stamp, filename );
				sprintf(stamp, "%u", (*i).second->everyNth);
				WritePrivateProfileString( buffer, "EveryNth", stamp, filename );
			}
			sprintf(stamp, "%u", (*i).second->deleted);
			WritePrivateProfileString( buffer, "Deleted", stamp, filename );
			sprintf(stamp, "%u", (*i).second->stamp);
			WritePrivateProfileString( buffer, "TimeStamp", stamp, filename );
		}
	}

#endif	// SERVER

	return true;
}

bool CEventCombiner::AddEvent(std::string& date, EventInfo* newEvent)
{
	if (newEvent)
	{
		std::map<std::string, EventInfo*>::iterator i = m_Events.find(date);

		if (i != m_Events.end())
		{
			// The event is already in the map
			if ((*i).second->stamp < newEvent->stamp)
			{
				// Overwrite the event cooz it's newer
				m_Events[date] = newEvent;
				return true;
			}
			else
			{
				delete newEvent;
				return false;
			}
		}
		else
		{
			m_Events[date] = newEvent;
			return true;
		}
	}

	return false;
}

std::list<PacketBuffer*>& CEventCombiner::EncodePackets()
{
	// First clear old packages
	std::list<PacketBuffer*>::iterator listIter = m_Packets.begin();
	for( ; listIter != m_Packets.end(); listIter++)
	{
		delete (*listIter);
	}
	m_Packets.clear();

	PacketBuffer* newPacket = NULL;

	std::map<std::string, EventInfo*>::iterator i = m_Events.begin();
	i = m_Events.begin();
	for ( ; i != m_Events.end(); i++)
	{
		newPacket = new PacketBuffer(pcEvent);

		EventInfo* info = (*i).second;

		// Check that the buffer can hold the whole message
		int size = (*i).first.size() + 1 + 4 + info->profile.size() + 1 + 4 + 4 + 4 + 4;
		if (size + info->message.size() + 1 >= newPacket->getBufferSizeMax())
		{
			// Need to truncate the message
			info->message.erase(info->message.begin() + newPacket->getBufferSizeMax() - size - 1, info->message.end());
			size = info->message.size();
		}

		(*newPacket) << (*i).first.c_str();	 // The date
		(*newPacket) << info->stamp;		   // The datestamp
		(*newPacket) << info->message.c_str(); // The message 
		(*newPacket) << info->profile.c_str();
		(*newPacket) << info->type;
		(*newPacket) << info->deleted;
		(*newPacket) << info->validUntil;
		(*newPacket) << info->everyNth;

		m_Packets.push_back(newPacket);
	}

	return m_Packets;
}

bool CEventCombiner::DecodePacket(PacketBuffer* packet)
{
	bool newEventsAvailable = false;
	EventInfo* newEvent = new EventInfo;
	std::string date;
	CStr str;

	(*packet) >> str;
	date = str;
	(*packet) >> newEvent->stamp;
	(*packet) >> str;
	if (str) newEvent->message = str;
	(*packet) >> str;
	if (str) newEvent->profile = str;
	(*packet) >> newEvent->type;
	(*packet) >> newEvent->deleted;
	(*packet) >> newEvent->validUntil;
	(*packet) >> newEvent->everyNth;

	if (AddEvent(date, newEvent))
	{
		newEventsAvailable = true;
	}

	return newEventsAvailable;
}


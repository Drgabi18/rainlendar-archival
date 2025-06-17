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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Server/EventCombiner.cpp,v 1.4 2002/02/27 18:14:43 rainy Exp $

  $Log: EventCombiner.cpp,v $
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
						date = buffer;
					}
					else
					{
						if (strncmp(buffer, "Message=", 8) == 0)
						{
							if (newEvent) 
							{
								newEvent->message = buffer + 8;
							}
						}
						else if (strncmp(buffer, "TimeStamp=", 10) == 0)
						{
							if (newEvent) 
							{
								newEvent->stamp = atoi(buffer + 10);
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

			strcpy(buffer, "Message=");
			strcat(buffer, (*i).second->message.c_str());
			strcat(buffer, "\n");
			fputs(buffer, file);	// The date

			strcpy(buffer, "TimeStamp=");
			sprintf(buffer + 10, "%u\n", (*i).second->stamp);
			fputs(buffer, file);	// The date
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
			WritePrivateProfileString( buffer, "Message", (*i).second->message.c_str(), filename );
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

		(*newPacket) << (*i).first.c_str();	 // The date
		(*newPacket) << info->stamp;			 // The datestamp
		(*newPacket) << info->message.c_str(); // The message

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
	if (str)
	{
		newEvent->message = str;
	}

	if (AddEvent(date, newEvent))
	{
		newEventsAvailable = true;
	}

	return newEventsAvailable;
}


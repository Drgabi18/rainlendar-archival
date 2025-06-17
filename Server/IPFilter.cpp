/*
  Copyright (C) 2003 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Server/IPFilter.cpp,v 1.3 2005/10/14 15:18:43 rainy Exp $

  $Log: IPFilter.cpp,v $
  Revision 1.3  2005/10/14 15:18:43  rainy
  no message

  Revision 1.2  2005/10/12 15:45:31  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.5  2004/11/14 11:37:46  rainy
  no message

  Revision 1.4  2003/05/25 18:12:28  Rainy
  Fixed the parsing routine.

  Revision 1.2  2003/05/25 15:12:20  Rainy
  Added possibility to get the last rule from the class.

  Revision 1.1  2003/05/24 20:06:44  Rainy
  Initial version.

*/

#include "IPFilter.h"

using namespace ssobjects;

CIPFilter::CIPFilter(const ssobjects::CStr& filterFile)
{
	m_FilterFile = filterFile;
}

CIPFilter::~CIPFilter()
{
	FilterItem* item;
	for (item = m_AcceptIncoming.getHead(); item != NULL; item = m_AcceptIncoming.getNext())
	{
		delete item;
	}
	for (item = m_DenyIncoming.getHead(); item != NULL; item = m_DenyIncoming.getNext())
	{
		delete item;
	}
	for (item = m_AcceptOutgoing.getHead(); item != NULL; item = m_AcceptOutgoing.getNext())
	{
		delete item;
	}
	for (item = m_DenyOutgoing.getHead(); item != NULL; item = m_DenyOutgoing.getNext())
	{
		delete item;
	}
}

bool CIPFilter::Parse()
{
	enum STATE
	{
		STATE_NONE,
		STATE_ACCEPT_INCOMING,
		STATE_DENY_INCOMING,
		STATE_ACCEPT_OUTGOING,
		STATE_DENY_OUTGOING
	} state;
	char buffer[1024];
	FilterItem* item;

	state = STATE_NONE;

	FILE* file = fopen(m_FilterFile, "r");

	if (file)
	{
		while(!feof(file))
		{
			buffer[0] = 0;
			fgets(buffer, 1024, file);
			if (buffer[0] == ';' || buffer[0] == '#') continue; // Comment
			if (buffer[0] == '[') state = STATE_NONE; // New state
			
			char* eol = strchr(buffer, '\n');
			if (eol)
			{
				*eol = 0;
			}

			switch (state)
			{
			case STATE_NONE:
				if (strncmp("[AcceptIncoming]", buffer, 16) == 0) 
				{
					state = STATE_ACCEPT_INCOMING;
				}
				else if (strncmp("[DenyIncoming]", buffer, 14) == 0) 
				{
					state = STATE_DENY_INCOMING;
				}
				else if (strncmp("[AcceptOutgoing]", buffer, 16) == 0) 
				{
					state = STATE_ACCEPT_OUTGOING;
				}
				else if (strncmp("[DenyOutgoing]", buffer, 14) == 0) 
				{
					state = STATE_DENY_OUTGOING;
				}
				break;

			case STATE_ACCEPT_INCOMING:
				item = ParseItem(buffer);
				if (item) m_AcceptIncoming.addHead(item);
				break;

			case STATE_DENY_INCOMING:
				item = ParseItem(buffer);
				if (item) m_DenyIncoming.addHead(item);
				break;

			case STATE_ACCEPT_OUTGOING:
				item = ParseItem(buffer);
				if (item) m_AcceptOutgoing.addHead(item);
				break;

			case STATE_DENY_OUTGOING:
				item = ParseItem(buffer);
				if (item) m_DenyOutgoing.addHead(item);
				break;
			}

			if (ferror(file))
			{
				LOG("Error reading the filter file.");
				fclose(file);
				return false;
			}
		}
		fclose(file);

		LOG("Read %i rules from the file \"%s\".", 
			m_AcceptIncoming.getNumEntries() + m_DenyIncoming.getNumEntries() +
			m_AcceptOutgoing.getNumEntries() + m_DenyOutgoing.getNumEntries(),
			(char*)m_FilterFile);
	}
	else
	{
		LOG("Unable to open the filter file.");
		return false;
	}

	return true;
}

FilterItem* CIPFilter::ParseItem(const char* buffer)
{
	FilterItem* item = NULL;

	if (buffer)
	{
		const char* equal = strchr(buffer, '=');
		if (equal != NULL)
		{
			int a, b, c, d, mask;
			CStr profiles(equal + 1);
			profiles += ";";	// Add ;-char to the end in case it's missing
			sscanf(buffer, "%i.%i.%i.%i/%i", &a, &b, &c, &d, &mask);
			item = new FilterItem;
			item->ip = CIPNumber(a, b, c, d, mask);
			item->profiles = profiles;
		}
	}
	return item;
}

bool CIPFilter::CheckIncoming(const CIPNumber& ip, const char* profile)
{
	return Check(ip, profile, m_AcceptIncoming, m_DenyIncoming);
}

bool CIPFilter::CheckOutgoing(const CIPNumber& ip, const char* profile)
{
	return Check(ip, profile, m_AcceptOutgoing, m_DenyOutgoing);
}

bool CIPFilter::Check(const CIPNumber& ip, const char* profile, LinkedList<FilterItem>& accept, LinkedList<FilterItem>& deny)
{
	// Check the accept list first
	FilterItem* item;
	CStr profileName(profile);
	profileName += ";";

	for (item = accept.getHead(); item != NULL; item = accept.getNext())
	{
		// Check if the ip matches
		if (item->ip.Compare(ip))
		{
			if (strncmp("*", item->profiles, 1) == 0)
			{
				m_LastRule = item->ip.GetString();
				m_LastRule += "=";
				m_LastRule += item->profiles;
				return true;	// Match every profile
			}
			else if (strstr(item->profiles, profileName) != NULL)
			{
				m_LastRule = item->ip.GetString();
				m_LastRule += "=";
				m_LastRule += item->profiles;
				return true;	// Match found
			}
		}
	}

	// If accept list didn't match, check the deny list
	for (item = deny.getHead(); item != NULL; item = deny.getNext())
	{
		// Check if the ip matches
		if (item->ip.Compare(ip))
		{
			if (strncmp("*", item->profiles, 1) == 0)
			{
				m_LastRule = item->ip.GetString();
				m_LastRule += "=";
				m_LastRule += item->profiles;
				return false;	// Match every profile
			}
			else if (strstr(item->profiles, profileName) != NULL)
			{
				m_LastRule = item->ip.GetString();
				m_LastRule += "=";
				m_LastRule += item->profiles;
				return false;	// Match found
			}
		}
	}

	m_LastRule = "None";
	return true;	// If lists do not match, we'll accept
}

const char* CIPFilter::GetRule()
{
	return m_LastRule;
}

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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/TodoManager.cpp,v 1.1 2003/10/27 17:34:54 Rainy Exp $

  $Log: TodoManager.cpp,v $
  Revision 1.1  2003/10/27 17:34:54  Rainy
  Initial version.

*/

#include "TodoManager.h"
#include "Litestep.h"
#include "RainlendarDLL.h"
#include <algorithm>


CTodoManager::CTodoManager()
{
}

CTodoManager::~CTodoManager()
{
}


void CTodoManager::Initialize(const char* file)
{
	m_Filename = file;
}

bool CTodoManager::Read()
{
	char* sections;
	char* buffer;
	int len, readLen;

	m_Todos.clear();
	m_Dones.clear();
	m_Notes.clear();

	// Get all section names
	len = MAX_LINE_LENGTH;
	sections = new char[len];
	while(true)
	{
		readLen = GetPrivateProfileString( NULL, NULL, NULL, sections, len, m_Filename.c_str());
		if (readLen == len - 1) 
		{
			// Buffer was not big enough
			delete [] sections;
			len *= len;
			sections = new char[len];
		}
		else break;
	}

	len = MAX_LINE_LENGTH;
	buffer = new char[len];
	char* pos = sections;
	while(strlen(pos) > 0)
	{
		// Get the string
		while (true)
		{
			readLen = GetPrivateProfileString(pos, "Message", "", buffer, len, m_Filename.c_str());
			if (readLen == len - 1) 
			{
				// Buffer was not big enough
				delete [] buffer;
				len *= len;
				buffer = new char[len];
			}
			else break;
		} 

		int type = GetPrivateProfileInt(pos, "Type", TODO_TYPE_NOTE, m_Filename.c_str());

		switch(type) 
		{
		case TODO_TYPE_TODO:
			m_Todos.push_back(buffer);
			break;

		case TODO_TYPE_DONE:
			m_Dones.push_back(buffer);
			break;

		default:
			m_Notes.push_back(buffer);
		}
			
		pos = pos + strlen(pos) + 1;
	}

	delete [] buffer;
	delete [] sections;

	return true;
}

bool CTodoManager::Write()
{
	int counter = 0, i;
	char section[16];
	char type[16];

	// Delete the Todo.ini, we'll create it again
	FILE* file = fopen(m_Filename.c_str(), "w");
	fclose(file);

	for (i = 0; i < m_Todos.size(); i++)
	{
		counter += 1;
		itoa(counter, section, 10);
		WritePrivateProfileString( section, "Message", m_Todos[i].c_str(), m_Filename.c_str());
		itoa(TODO_TYPE_TODO, type, 10);
		WritePrivateProfileString( section, "Type", type, m_Filename.c_str());
	}

	for (i = 0; i < m_Dones.size(); i++)
	{
		counter += 1;
		itoa(counter, section, 10);
		WritePrivateProfileString( section, "Message", m_Dones[i].c_str(), m_Filename.c_str());
		itoa(TODO_TYPE_DONE, type, 10);
		WritePrivateProfileString( section, "Type", type, m_Filename.c_str());
	}

	for (i = 0; i < m_Notes.size(); i++)
	{
		counter += 1;
		itoa(counter, section, 10);
		WritePrivateProfileString( section, "Message", m_Notes[i].c_str(), m_Filename.c_str());
		itoa(TODO_TYPE_NOTE, type, 10);
		WritePrivateProfileString( section, "Type", type, m_Filename.c_str());
	}
	return true;
}

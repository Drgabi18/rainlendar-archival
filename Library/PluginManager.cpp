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
  $Header: /home/cvsroot/Rainlendar/Library/PluginManager.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: PluginManager.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.2  2005/03/01 18:50:45  rainy
  no message

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#pragma warning(disable: 4786)
#pragma warning(disable: 4996)

#include "PluginManager.h"
#include "Litestep.h"
#include "RainlendarDll.h"
#include <tchar.h>

CPluginManager::CPluginManager()
{
}

CPluginManager::~CPluginManager()
{
	for (UINT i = 0; i < m_Plugins.size(); i++)
	{
		delete m_Plugins[i];
	}
}

void CPluginManager::LoadPlugins(LPCTSTR path)
{
    WIN32_FIND_DATA fileData;      // Data structure describes the file found
    HANDLE hSearch;                // Search handle returned by FindFirstFile

	for (UINT i = 0; i < m_Plugins.size(); i++)
	{
		delete m_Plugins[i];
	}
	m_Plugins.clear();

	UINT id = 1;	// plugin ids start from 1

    // Start searching for .dll files in the given directory.
	std::string files = path;
	files += "*.dll";
    hSearch = FindFirstFile(files.c_str(), &fileData);
	do
	{
		if(hSearch == INVALID_HANDLE_VALUE) break;    // No more files found

		CPlugin* plugin = new CPlugin;

		std::string fileName = path;
		fileName += fileData.cFileName;
		if (plugin->Initialize(fileName.c_str(), id))
		{
			DebugLog("Found plugin: %s", fileData.cFileName);
			m_Plugins.push_back(plugin);
			id++;
		}
		else
		{
			DebugLog("%s is not a Rainlendar plugin.", fileData.cFileName);
			delete plugin;
		}

	} while(FindNextFile(hSearch, &fileData));

    FindClose(hSearch);
}

std::vector<CPlugin*> CPluginManager::GetPlugins()
{
	std::vector<CPlugin*> activePlugins;
	
	for (UINT i = 0; i < m_Plugins.size(); i++)
	{
		if (m_Plugins[i]->IsEnabled())
		{
			activePlugins.push_back(m_Plugins[i]);
		}
	}

	return activePlugins;
}

CPlugin* CPluginManager::GetDefaultPlugin()
{
	for (UINT i = 0; i < m_Plugins.size(); i++)
	{
		if (strcmp(m_Plugins[i]->GetName(), m_DefaultPluginName.c_str()) == 0)
		{
			return m_Plugins[i];
		}
	}

	return NULL;
}

void CPluginManager::ReadConfigs()
{
	char tmpSz[MAX_LINE_LENGTH];

	std::string configFile = CConfig::Instance().GetPath();
	configFile += "Rainlendar.ini";

	if(GetPrivateProfileString("Plugins", "DefaultPlugin", "", tmpSz, MAX_LINE_LENGTH, configFile.c_str()) > 0) 
	{
		m_DefaultPluginName=tmpSz;
	}

	UINT i;
	bool found = false;
	for (i = 0; i < m_Plugins.size(); i++)
	{
		int enabled = GetPrivateProfileInt("Plugins", m_Plugins[i]->GetName(), -1, configFile.c_str());

		if (enabled == 0)
		{
			found = true;
			m_Plugins[i]->Disable();
		} 
		else if (enabled == 1)
		{
			found = true;
			m_Plugins[i]->Enable();
		}
		else
		{
			m_Plugins[i]->Disable();	// If there are no settings, disable the plugin
		}
	}

	// If there are no enabled configs, enable the IniFormatPlugin
	if (!found)
	{
		for (i = 0; i < m_Plugins.size(); i++)
		{
			if (strcmp(m_Plugins[i]->GetName(), "Rainlendar Ini-format") == 0)
			{
				m_Plugins[i]->Enable();
			}
		}
	}
}

void CPluginManager::WriteConfigs()
{
	std::string configFile = CConfig::Instance().GetPath();
	configFile += "Rainlendar.ini";

	WritePrivateProfileString("Plugins", "DefaultPlugin", m_DefaultPluginName.c_str(), configFile.c_str());

	for (UINT i = 0; i < m_Plugins.size(); i++)
	{
		WritePrivateProfileString("Plugins", m_Plugins[i]->GetName(), m_Plugins[i]->IsEnabled() ? "1" : "0", configFile.c_str() );
	}
}
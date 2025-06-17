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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/Language.cpp,v 1.2 2003/08/09 16:34:53 Rainy Exp $

  $Log: Language.cpp,v $
  Revision 1.2  2003/08/09 16:34:53  Rainy
  Added new strings.
  Profiles can be translated.

  Revision 1.1  2003/06/15 09:47:31  Rainy
  Intial version.


*/

#include "Language.h"
#include "Litestep.h"
#include "RainlendarDLL.h"

const char* g_DefaultGeneralStrings[] = {
	"OK",
	"Cancel",
	"Apply",
	"General",
	"Layout",
	"Server",
	"Rainlendar Config",
	"Background",
	"Days",
	"Today",
	"Event",
	"Weekdays",
	"Week Numbers",
	"Month",
	"Year",
	"Hotkeys",
    NULL
};

const char* g_DefaultAboutStrings[] = {
	"Rainlendar version %s \n(Build on %s)\n\nGet the latest version from:\nwww.iki.fi/rainy\n\n",
    NULL
};

const char* g_DefaultMessageStrings[] = {
	"Are you sure you want to delete this event?",
	"The event is read-only and cannot be deleted!",
	"Not all events were updated to the server.\nThe server has more up-to-date.",
	"You can use only values positive starting from 1 values in the vertical and horizontal count fields.",
	"%i? Are you mad? Please try to use some more reasonable value in the vertical and horizontal count fields.",
	"Please use only numbers (0-9)\nand letters (A-Z, a-z) in the ID.\nThe illegal chars are automatically changed to '_'",
	"Sorry!\n\nSkin editing is only possible with separate skin configurations.\n\nWhat you need to do is to remove all the skinning stuff from Rainlendar.ini\nand put them in a separate ini-file inside a subfolder under Rainlendar's folder.\nConsult the documentation for more info.",
	"Unable to find the skin: %s\\%s\nRainlendar will default to the first available skin.",
	"Unknown !bang: %s",
    NULL
};

const char* g_DefaultErrorStrings[] = {
	"No Errors!",
	"Out of memory!",
	"Unable to create the window class!",
    "Unable to create the main window!",
	"Unable to fetch the desktop's background!",
	"Unable to create font!",
	"Unable to calculate text dimensions!",
    "NULL parameter!",
    "File not found: %s",		// 8
    "Bitmap name was not specified in the ini-file!",		// 9
	NULL
};
		
const char* g_DefaultMenuStrings[] = {
	"About...",
	"Select Month",
	"Next Month\tRight",
    "Prev Month\tLeft",
	"Current Month\tHome",
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",			// 10
	"July",
	"August",
	"September",
	"October",
	"November",
	"December",
	"Refresh",
	"Set event...",
    "Show all events...",
	"Config...",		// 20
	"Quit",
	"Language",
	"English (default)",
	"Edit Current Skin...",
	"Skins",
	"Next Year\tUp",
    "Prev Year\tDown",
    "Update Outlook",
    NULL
};

const char* g_DefaultGeneralConfigGUIStrings[] = {
	"Show event message in tooltip",
	"Show event message in a messagebox",
	"Start hidden",
    "Start weekdays from Monday",
	"Disable keyboard shortcuts",
	"Use window name",
	"Poll wallpaper changes",
	"Event",
	"Execute on event:",
	"Other settings",
	"Snap to screen edges",		// 10
	"Use native Windows transparency (Win2k/XP only)",
	"Refresh when screen resolution changes",
	"Show Outlook appointments",
	"Week containing Jan 1st is the first week of the year",
	"Use separator in tooltips",
	"Outlook Settings",
	"Update every",
	"seconds (0 = disable)",	// 18
	"It is not possible to use native transparency with OnDesktop setting.\nNative transparency is disabled.",
    NULL
};

const char* g_DefaultLayoutConfigGUIStrings[] = {
	"OnBottom",
	"Normal",
	"OnTop",
	"Movable window",
	"Hide on mouse over",
	"X:",
	"Y:",
	"Position",
	"OnDesktop (Experimental)",
	"Layout",
	"Vertical count:",		// 10
	"Horizontal count:",
	"Previous months:",
	"Start from January",
	"Remember dialog positions",	// 14
	"Make negative coordinates relative to bottom right corner",	// 15
	"The window's position cannot be set to OnDesktop if native transparency is used.\nThe position is changed to Normal.",
	NULL
};

const char* g_DefaultHotkeysConfigGUIStrings[] = {
	"Hide",
	"Show",
	"Toggle",
	"Activate",
	"Refresh",
	"Edit config",
	"Edit skin",
	"Next month",
	"Previous month",
	"Current month",
	"Show all events",	// 10
	"Previous year",
	"Current year",
	NULL
};

const char* g_DefaultServerConfigGUIStrings[] = {
	"Enable server synchronization",
	"Request events on startup",
	"Sync now!",
	"Address:",
	"Port:",
	"ID:",
	"Update:",
	"minutes (0 = disable)",
	"Copy to Clipboard",
	"Sync event before editing",
    NULL
};

const char* g_DefaultSkinGUIStrings[] = {
	"Enable Days",
	"Do not draw days on today",
	"Do not draw days on event days",
	"Browse",
	"X:",
	"Y:",
	"W:",
	"H:",
	"Rasterizer:",
	"Align:",
	"Bitmap",
	"Bitmap:",
	"Components:",
	"Separation:",
	"Font",
	"Font:",
	"Select",
	"Color:",
	"Stretch",
	"Tile",
	"Transparent",
	"Background",
	"Filename:",
	"Text Color:",
	"ToolTip",
	"Background Color:",
	"Solid color",
	"Bevel edge",
	"Enable Event Days",
	"Show events in the calendar",
	"Enable Today",
	"Enable Weekdays",
	"Enable Week Numbers",
	"Enable Month",
	"Enable Year",
	"Top Left",
	"Top Center",
	"Top Right",
	"Left",
	"Center",
	"Right",
	"Bottom Left",
	"Bottom Center",
	"Bottom Right",
	NULL
};
  
const char* g_DefaultEventGUIStrings[] = {
	"New",
	"Delete",
	"Single",
	"Daily",
	"Weekly",
	"Monthly",
	"Annually",
	"Valid until:",
	"Every:",
	"Profile:",
	"Repeat",
	"Please wait while synchronizing events with the server",
    NULL
};

const char* g_DefaultAllEventsGUIStrings[] = {
	"Date",
	"Type",
	"Profile",
	"Text",
    NULL
};

const char* g_DefaultNetStatusStrings[] = {
	"Connecting to %s:%s (%s)",
	"Connection successful.",
	"Unknown request type.",
	"Connection failed: %s.",
	"Network thread completed successfully.",
	"Sending the events to the server.",
	"Update successful.",
	"The server uses a different protocol version than the client.",
	"The server is busy.",
	"The server replied with an error.",
	"Unknown reply received from the server.",
	"Requesting the events from the server.",
	"Got unknown packet from the server (was expecting a pcEvent).",
	"Request successful.",
    NULL
};

const char* g_DefaultNumbersStrings[] = {
	"1st",
	"2nd",
	"3th",
	"4th",
	"5th",
	"6th",
	"7th",
	"8th",
	"9th",
	"10th",
	"11th",
	"12th",
    NULL
};

CLanguage::CLanguage()
{
    SetDefaultLanguage();
}

CLanguage::~CLanguage()
{
	for (int i = 0; i < m_Strings.size(); i++)
	{
		delete m_Strings[i];
	}
}

bool CLanguage::ScanLanguages(const char* path)
{
    WIN32_FIND_DATA fileData;   // Data structure describes the file found
    HANDLE hSearch;             // Search handle returned by FindFirstFile

	m_Languages.clear();
	
	if (path != NULL)
	{
		m_Path = path;
	}

    // Start searching for .ini files in the given directory.
	std::string files = m_Path;
	files += "*.ini";
    hSearch = FindFirstFile(files.c_str(), &fileData);
	do
	{
		if(hSearch == INVALID_HANDLE_VALUE) break;    // No more files found

		if(!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			std::string language = fileData.cFileName;
			language.erase(language.end() - 4, language.end());
			m_Languages.push_back(language);
		}
	} while(FindNextFile(hSearch, &fileData));

    FindClose(hSearch);

	return true;
}

bool CLanguage::ChangeLanguage(int index)
{
	char buffer[MAX_LINE_LENGTH];
	char buffer2[MAX_LINE_LENGTH];
	char buffer3[MAX_LINE_LENGTH];

	SetDefaultLanguage();

	if (index < m_Languages.size())
	{
		std::string language = m_Path + m_Languages[index] + ".ini";
		GetPrivateProfileString( NULL, NULL, NULL, buffer, MAX_LINE_LENGTH, language.c_str());

		char* pos = buffer;
		while(strlen(pos) > 0)
		{
			GetPrivateProfileString(pos, NULL, NULL, buffer2, MAX_LINE_LENGTH, language.c_str());
			char* pos2 = buffer2;

			if (strcmp(pos, "Profiles") == 0)
			{
				while(strlen(pos2) > 0)
				{
					GetPrivateProfileString(pos, pos2, NULL, buffer3, MAX_LINE_LENGTH, language.c_str());
					m_ProfileMap[pos2] = buffer3;

					pos2 = pos2 + strlen(pos2) + 1;
				}
			}
			else
			{
				while(strlen(pos2) > 0)
				{
					GetPrivateProfileString(pos, pos2, NULL, buffer3, MAX_LINE_LENGTH, language.c_str());
					SetString(pos, atoi(pos2), buffer3);

					pos2 = pos2 + strlen(pos2) + 1;
				}
				
			}
			pos = pos + strlen(pos) + 1;
		}
	}
	else
	{
		char buffer[256];
		sprintf(buffer, "No such language. Index: %i", index);
		LSLog(LOG_DEBUG, "Rainlendar", buffer);
		
		return false;
	}

	return true;
}

void CLanguage::SetDefaultLanguage()
{
	int i;

    // Clear all strings first
	for (i = 0; i < m_Strings.size(); i++)
	{
		delete m_Strings[i];
	}
    m_Strings.clear();
    m_IndexMap.clear();
	m_ProfileMap.clear();

    i = 0;
    while (g_DefaultGeneralStrings[i] != NULL)
    {
        SetString("General", i, g_DefaultGeneralStrings[i]);
		i++;
    } 

    i = 0;
    while (g_DefaultAboutStrings[i] != NULL)
    {
        SetString("About", i, g_DefaultAboutStrings[i]);
		i++;
    } 

    i = 0;
    while (g_DefaultMessageStrings[i] != NULL)
    {
        SetString("Message", i, g_DefaultMessageStrings[i]);
		i++;
    } 

    i = 0;
    while (g_DefaultErrorStrings[i] != NULL)
    {
        SetString("Error", i, g_DefaultErrorStrings[i]);
		i++;
    } 

    i = 0;
    while (g_DefaultMenuStrings[i] != NULL)
    {
        SetString("Menu", i, g_DefaultMenuStrings[i]);
		i++;
    } 

    i = 0;
    while (g_DefaultGeneralConfigGUIStrings[i] != NULL)
    {
        SetString("GeneralConfigGUI", i, g_DefaultGeneralConfigGUIStrings[i]);
		i++;
    } 

    i = 0;
    while (g_DefaultLayoutConfigGUIStrings[i] != NULL)
    {
        SetString("LayoutConfigGUI", i, g_DefaultLayoutConfigGUIStrings[i]);
		i++;
    } 

    i = 0;
    while (g_DefaultHotkeysConfigGUIStrings[i] != NULL)
    {
        SetString("HotkeysConfigGUI", i, g_DefaultHotkeysConfigGUIStrings[i]);
		i++;
    } 

    i = 0;
    while (g_DefaultServerConfigGUIStrings[i] != NULL)
    {
        SetString("ServerConfigGUI", i, g_DefaultServerConfigGUIStrings[i]);
		i++;
    } 

    i = 0;
    while (g_DefaultSkinGUIStrings[i] != NULL)
    {
        SetString("SkinGUI", i, g_DefaultSkinGUIStrings[i]);
		i++;
    } 

    i = 0;
    while (g_DefaultEventGUIStrings[i] != NULL)
    {
        SetString("EventGUI", i, g_DefaultEventGUIStrings[i]);
		i++;
    } 

    i = 0;
    while (g_DefaultAllEventsGUIStrings[i] != NULL)
    {
        SetString("AllEventsGUI", i, g_DefaultAllEventsGUIStrings[i]);
		i++;
    } 

    i = 0;
    while (g_DefaultNetStatusStrings[i] != NULL)
    {
        SetString("NetStatus", i, g_DefaultNetStatusStrings[i]);
		i++;
	} 

    i = 0;
    while (g_DefaultNumbersStrings[i] != NULL)
    {
        SetString("Numbers", i, g_DefaultNumbersStrings[i]);
		i++;
	} 
}

const std::vector<std::string>& CLanguage::GetLanguages()
{
    return m_Languages;
}

const char* CLanguage::GetTranslatedProfile(const char* profile)
{
    std::map<std::string, std::string>::iterator iter = m_ProfileMap.find(profile);
    
    if (iter != m_ProfileMap.end())
    {
        return (*iter).second.c_str();
    } 

	return profile;
}

const char* CLanguage::GetOriginalProfile(const char* profile)
{
    std::map<std::string, std::string>::iterator iter = m_ProfileMap.begin();

	for ( ; iter != m_ProfileMap.end(); iter++)
	{
		if ((*iter).second == profile)
		{
			return (*iter).first.c_str();
		}
	}

	return profile;
}


const char* CLanguage::GetString(const char* section, int index)
{
    std::map<std::string, int>::iterator iter = m_IndexMap.find(section);
    
    if (iter != m_IndexMap.end())
    {
        int i = (*iter).second;
        if (m_Strings.size() > i && m_Strings[i] != NULL && (*m_Strings[i]).size() > index)
        {
            return (*m_Strings[i])[index].c_str();
        }
    } 

	char buffer[256];
	sprintf(buffer, "Requested a non-existing string. Section; %s, index: %i", section, index);
	LSLog(LOG_DEBUG, "Rainlendar", buffer);

    return NULL;
}

void CLanguage::SetString(const char* section, int index, const char* string)
{
    int sectionIndex = 0;
    std::map<std::string, int>::iterator iter = m_IndexMap.find(section);
    
    if (iter != m_IndexMap.end())
    {
        sectionIndex = (*iter).second;
    }
    else
    {
        // Need to create a new section
        sectionIndex = m_IndexMap.size();
        m_IndexMap[section] = sectionIndex;
		m_Strings.resize(sectionIndex + 1);
		m_Strings[sectionIndex] = new std::vector<std::string>();
    }

	if (m_Strings[sectionIndex])
	{
		if ((*m_Strings[sectionIndex]).size() <= index)
		{
			(*m_Strings[sectionIndex]).resize(index + 1);
		} 
	
		size_t j;
		std::string tmpString = string;
		while ((j = tmpString.find("\\n")) != std::string::npos)
		{
			tmpString.replace(j, 2, "\n");
		}
		while ((j = tmpString.find("\\t")) != std::string::npos)
		{
			tmpString.replace(j, 2, "\t");
		}

		(*m_Strings[sectionIndex])[index] = tmpString;
	}
}
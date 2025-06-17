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
  $Header: /home/cvsroot/Rainlendar/Library/Language.cpp,v 1.3 2005/10/14 17:05:29 rainy Exp $

  $Log: Language.cpp,v $
  Revision 1.3  2005/10/14 17:05:29  rainy
  no message

  Revision 1.2  2005/09/08 16:09:12  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.12  2005/03/01 18:50:45  rainy
  no message

  Revision 1.11  2004/12/05 18:25:22  rainy
  Added new strings.

  Revision 1.10  2004/11/06 13:38:59  rainy
  no message

  Revision 1.9  2004/04/24 11:23:18  rainy
  3th -> 3rd

  Revision 1.8  2004/01/25 10:01:05  rainy
  Added new string.

  Revision 1.7  2004/01/10 15:19:11  rainy
  New strings added.

  Revision 1.6  2003/12/20 22:22:52  rainy
  Added some new strings

  Revision 1.5  2003/12/05 15:46:43  Rainy
  Changed some sentences.

  Revision 1.4  2003/10/27 17:38:09  Rainy
  Added todo strings.

  Revision 1.3  2003/08/23 09:14:50  Rainy
  Fixed some strings.

  Revision 1.2  2003/08/09 16:34:53  Rainy
  Added new strings.
  Profiles can be translated.

  Revision 1.1  2003/06/15 09:47:31  Rainy
  Intial version.


*/
#pragma warning(disable: 4996)

#include "Language.h"
#include "Litestep.h"
#include "RainlendarDLL.h"
#include <algorithm>
#include <math.h>

const char* g_DefaultGeneralStrings[] = {		// General
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
	"Event",	// 10
	"Weekdays",
	"Week Numbers",
	"Month",
	"Year",
	"Hotkeys",
	"Todo",
	"MessageBox",	// 17
	"Skin Config",
	"Plugins",
	"Unknown",		// 20
	" (Due in %i minutes)",
	"Today",
	"Tomorrow",
	"EventList",
	" (%i minutes overdue)",
    NULL
};

const char* g_DefaultAboutStrings[] = {		// About
	"Rainlendar version %s \n(Build on %s)\n\nGet the latest version from:\nhttp://www.rainlendar.net\n\n",
    NULL
};

const char* g_DefaultMessageStrings[] = {	// Message
	"Are you sure you want to delete the event(s)?",
	"Read-only events cannot be deleted!",
	"Not all events were updated to the server.\nThe server has more up-to-date.",
	"You can use only values positive starting from 1 values in the vertical and horizontal count fields.",
	"%i? Are you mad? Please try to use some more reasonable value in the vertical and horizontal count fields.",
	"Please use only numbers (0-9)\nand letters (A-Z, a-z) in the ID.\nThe illegal chars are automatically changed to '_'",
	"Sorry!\n\nSkin editing is only possible with separate skin configurations.\n\nWhat you need to do is to remove all the skinning stuff from Rainlendar.ini\nand put them in a separate ini-file inside a subfolder under Rainlendar's folder.\nConsult the documentation for more info.",
	"Unable to find the skin: %s\\%s\nRainlendar will default to the first available skin.",
	"Unknown !bang: %s",
	"A new version of Rainlendar is available at http://www.rainlendar.net\nDo you want to go there now?",
    NULL
};

const char* g_DefaultErrorStrings[] = {		// Error
	"No Errors!",
	"Out of memory!",
	"Unable to create the window class!",
    "Unable to create the main window!",
	"Unable to fetch the desktop's background!",
	"Unable to create font!",
	"Unable to calculate text dimensions!",
    "NULL parameter!",
    "File cannot be opened: %s",		// 8
    "Bitmap name was not specified in the ini-file!",		// 9
	NULL
};
		
const char* g_DefaultMenuStrings[] = {		// Menu
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
    "Edit todo list...",
    "Mark as done",		// 30
	"Undo",
	"Cut",
	"Copy",
	"Paste",
	"Delete",
	"Select All",
	"Help",	
	"Get More Languages...",
	"Get More Skins...",
	"Windows",		// 40
	"Calendar",
	"Todo",
	"Event list",
	"Event",
	"Create new event...",
	"Delete event(s)",
	"Visible",
	"Locked",
	"Edit item...",
	"Mark all checked as done",   // 50
	"Check item",
	"Delete item",
    NULL
};

const char* g_DefaultGeneralConfigGUIStrings[] = {		// GeneralConfigGUI
	"Show event message in tooltip",
	"*NotInUse*",
	"Start hidden",
    "Start weekdays from Monday",
	"Disable keyboard shortcuts",
	"Use window name",
	"Poll wallpaper changes",
	"Event Settings",
	"Execute on event:",
	"General settings",
	"Snap to screen edges",		// 10
	"Use native Windows transparency (Win2k/XP only)",
	"Refresh when screen resolution changes",
	"Show Outlook appointments",
	"Week containing Jan 1st is the first week of the year",
	"Use separator in tooltips",
	"Outlook Settings",
	"Update every",
	"seconds (0 = disable)",
	"It is not possible to use native transparency with OnDesktop setting.\nNative transparency is disabled.",
	"Get Outlook events at startup",  // 20
	"Max tooltip width:",
	"Show all today's messages at startup",
	"Show message alarms",
	"Max message width:",
	"Show tray icon",
	"Transparency",
	"Transparent",
	"Opaque",
	"Log",
	"Enable Log",		// 30
	"Disable Log",
	"Show Log",
	"Advanced Edit",
	"Advanced",
	"Hide event and todo lists when there are no items",
    NULL
};

const char* g_DefaultLayoutConfigGUIStrings[] = {		// LayoutConfigGUI
	"OnBottom",
	"Normal",
	"OnTop",
	"Movable window",
	"Hide on mouse over (hold Shift-key down to deactivate)",
	"X:",
	"Y:",
	"Position",
	"OnDesktop",
	"Multiple calendars",
	"Vertical count:",		// 10
	"Horizontal count:",
	"Previous months:",
	"Start from January",
	"Remember dialog positions",	// 14
	"Make negative coordinates relative to bottom right corner",	// 15
	"The window's position cannot be set to OnDesktop if native transparency is used.\nThe position is changed to Normal.",
	"Todo list",
	"Show Todo list",
	"Other settings",
	"Click through (hold Shift-key down to deactivate)",	// 20
	"Show",
	"days",
	"Substitute today and tomorrow",
	"Event list window",
	"Opaque on mouse over",
	"Transparent on mouse over",
	"Layout %c",
	"Layout type:",
	NULL
};

const char* g_DefaultHotkeysConfigGUIStrings[] = {		// HotkeysConfigGUI
	"Hide all windows",
	"Show all windows",
	"Toggle all windows",
	"Activate calendar",
	"Refresh",
	"Edit config",
	"Edit skin",
	"Show next month",
	"Show previous month",
	"Show current month",
	"Show all events",	// 10
	"Show next year",
	"Show previous year",
	"Update Outlook",
	"Add new todo-item",
	"Action",
	"Hotkey",
	"Set",
	"Hotkeys",		// 18
	"Hide Calendar window",
	"Show Calendar window", // 20
	"Toggle Calendar window",
	"Hide Todo window",
	"Show Todo window",
	"Toggle Todo window",
	"Hide Events window",
	"Show Events window",
	"Toggle Events window",
	"Add new event",		// 28
	NULL
};

const char* g_DefaultSkinGUIStrings[] = {		// SkinGUI
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
	"Bitmap",		// 10
	"Bitmap:",
	"Components:",
	"Separation:",
	"Font",
	"Font:",
	"Select",
	"Color:",
	"Stretch",
	"Tile",
	"Transparent",	// 20
	"Background",
	"Filename:",
	"Text Color:",
	"ToolTip",
	"Background Color:",
	"Solid color",
	"Bevel edge",
	"Enable Event Days",
	"Show events in the calendar",
	"Enable Today",		// 30
	"Enable Weekdays",
	"Enable Week Numbers",
	"Enable Month",
	"Enable Year",
	"Top Left",
	"Top Center",
	"Top Right",
	"Left",
	"Center",
	"Right",			// 40
	"Bottom Left",
	"Bottom Center",
	"Bottom Right",
	"Width:",
	"Bitmap Margins:",
	"Text Margins:",
	"Todo Item",
	"Offset X:",
	"Todo Text",
	"Message Text",		// 50
	"Weekend Color:",
	"Weekend Bitmap:",
	"Layout",
	"Day layout:",
	"Normal (7x6 grid)",
	"Horizontal",
	"Vertical",
	"Advanced",
	"EventList Background",
	"EventList Headers",	// 60
	"EventList Text",
	NULL
};
  
const char* g_DefaultEventGUIStrings[] = {		// EventGUI
	"New",
	"Delete",
	"Single",
	"Daily",
	"Weekly",
	"Monthly",
	"Yearly",
	"Until:",
	"Repeat every:",
	"Profile:",
	"Repeat",				// 10
	"Please wait while synchronizing events with the server",
	"day(s)",
	"week(s)",
	"month(s)",
	"year(s)",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",				// 20
	"Sat",
	"Sun",
	"On the %1 day of the month",
	"On the %1 %2 of the month",
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",       // 30
	"Saturday",
	"first",
	"second",
	"third",
	"fourth",
	"fifth",
	"weekday",
	"Edit Event",
	"Start:",
	"Duration:", 		// 40
	"All day event",
	"Message:",
	"Header:",
	"Recurrence",
	"minutes",
	"hour",
	"hours",
	"day",
	"days",
	"week",			// 50
	"weeks",
	"Recurrence:",
	"Forever",
	"Number of times:",
	"Show alarm",
	"minutes before the event",
	"Are you sure you want to delete this event?",
	"last",
    NULL
};

const char* g_DefaultAllEventsGUIStrings[] = {		// AllEventsGUI
	"Date",
	"Type",
	"Profile",
	"Text",
	"All Events",
	"Start date",
	"End date",
	"Import",
	"Export",
	"None of the plugins is able to export events.",
	"Unable to import the file.",			// 10
	"Do you want to export only the selected events?\n(Press \"No\" to export all events.)",
    NULL
};

const char* g_DefaultNumbersStrings[] = {		// Numbers
	"1st",
	"2nd",
	"3rd",
	"4th",
	"5th",
	"6th",
	"7th",
	"8th",
	"9th",
	"10th",
	"11th",
	"12th",
	"13th",
	"14th",
	"15th",
	"16th",
	"17th",
	"18th",
	"19th",
	"20th",
	"21st",
	"22nd",
	"23rd",
	"24th",
	"25th",
	"26th",
	"27th",
	"28th",
	"29th",
	"30th",
	"31st",
    NULL
};

const char* g_DefaultTodoStrings[] = {		// Todo
	"Todo",
	"Done",
	"Notes",
	"New",
	"Modify",
	"Delete",
	"Undo",
	"Are you sure you want to delete the item?",
	"Are you sure you want to delete these items?", // 8
	"Move up",
	"Move down",
    NULL
};

const char* g_DefaultPluginsStrings[] = {		// Plugins
	"Plugins",
	"Name",
	"Version",
	"Default",
	"Set as default",
	"Settings",
	"The plugins can be enabled/disabled from the checkbox.",	// 6
    NULL
};

CLanguage::CLanguage()
{
    SetDefaultLanguage();
}

CLanguage::~CLanguage()
{
	Clear();
}

void CLanguage::Clear()
{
	for (UINT i = 0; i < m_Strings.size(); i++)
	{
		delete m_Strings[i];
	}
	m_Strings.clear();
	m_IndexMap.clear();
	m_ProfileMap.clear();
	m_Languages.clear();
	m_Path.erase();
	m_OrdinalNums.clear();
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

	// Sort the language list
	std::sort(m_Languages.begin(), m_Languages.end());

	return true;
}

bool CLanguage::ChangeLanguage(int index)
{
	char buffer[MAX_LINE_LENGTH];
	char buffer2[MAX_LINE_LENGTH];
	char buffer3[MAX_LINE_LENGTH];

	SetDefaultLanguage();

	if (index < (int)m_Languages.size())
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
			else if (strcmp(pos, "OrdinalNumbers") == 0)
			{
				m_OrdinalNums.clear();
				m_OrdinalsDefined = true;
				while(strlen(pos2) > 0)
				{
					GetPrivateProfileString(pos, pos2, NULL, buffer3, MAX_LINE_LENGTH, language.c_str());
					if (strcmp(pos2, "Default") == 0)
					{
						m_DefaultOrdinal.Reminder = 0;
						m_DefaultOrdinal.Divider = 1;
						m_DefaultOrdinal.EndText = buffer3;
					}
					else
					{
						OrdinalNumber ON;
						ON.Reminder = atoi(pos2);
						if (pos2[strlen(pos2)-1] == '%')
							ON.Divider = 1;
						else
							ON.Divider = pow (10, strlen(pos2));
						ON.EndText = buffer3;
						m_OrdinalNums.push_back(ON);
					}
					
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

	    const char* loc = GetString("Locale", 0);
		if (loc)
		{
			if (NULL == setlocale(LC_ALL, loc))
			{
				DebugLog("Incorrect locale: %s", loc);
			}
		}
	}
	else
	{
		DebugLog("No such language. Index: %i", index);
		return false;
	}

	return true;
}

void CLanguage::SetDefaultLanguage()
{
	UINT i;

	setlocale(LC_ALL, "English");

    // Clear all strings first
	for (i = 0; i < m_Strings.size(); i++)
	{
		delete m_Strings[i];
	}
    m_Strings.clear();
    m_IndexMap.clear();
	m_ProfileMap.clear();

	// Mitul{
	m_OrdinalNums.clear();
	m_OrdinalsDefined = false;

	// The default is English
	m_DefaultOrdinal.Reminder = 0;
	m_DefaultOrdinal.Divider = 1;
	m_DefaultOrdinal.EndText = "th";

	OrdinalNumber ON;
	ON.Reminder = 1;
	ON.Divider = 10;
	ON.EndText = "st";
	m_OrdinalNums.push_back(ON);

	ON.Reminder = 2;
	ON.Divider = 10;
	ON.EndText = "nd";
	m_OrdinalNums.push_back(ON);

	ON.Reminder = 3;
	ON.Divider = 10;
	ON.EndText = "rd";
	m_OrdinalNums.push_back(ON);

	ON.Reminder = 11;
	ON.Divider = 100;
	ON.EndText = "th";
	m_OrdinalNums.push_back(ON);

	ON.Reminder = 12;
	ON.Divider = 100;
	ON.EndText = "th";
	m_OrdinalNums.push_back(ON);

	ON.Reminder = 13;
	ON.Divider = 100;
	ON.EndText = "th";
	m_OrdinalNums.push_back(ON);
	// Mitul}

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
    while (g_DefaultNumbersStrings[i] != NULL)
    {
        SetString("Numbers", i, g_DefaultNumbersStrings[i]);
		i++;
	} 

    i = 0;
    while (g_DefaultTodoStrings[i] != NULL)
    {
        SetString("Todo", i, g_DefaultTodoStrings[i]);
		i++;
	} 

    i = 0;
    while (g_DefaultPluginsStrings[i] != NULL)
    {
        SetString("Plugins", i, g_DefaultPluginsStrings[i]);
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
        if ((int)m_Strings.size() > i && m_Strings[i] != NULL && (int)(*m_Strings[i]).size() > index)
        {
            return (*m_Strings[i])[index].c_str();
        }
    } 

	DebugLog("Requested a non-existing string. Section; %s, index: %i", section, index);

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
		if ((int)(*m_Strings[sectionIndex]).size() <= index)
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

//Mitul{
const char* CLanguage::GetOrdinalString(int Num)
{
//	DebugLog("Testing: Number received: %i", Num);
	for (int index = (int)m_OrdinalNums.size() - 1; index >=0; index--)
	{
//		DebugLog("Testing: Divider: %i   Reminder: %i", m_OrdinalNums[index].Divider, m_OrdinalNums[index].Reminder);

		if ((m_OrdinalNums[index].Divider == 1) && (m_OrdinalNums[index].Reminder == Num))
			return m_OrdinalNums[index].EndText.c_str();
		else if (Num % m_OrdinalNums[index].Divider == m_OrdinalNums[index].Reminder)
			return m_OrdinalNums[index].EndText.c_str();
	}

	return m_DefaultOrdinal.EndText.c_str();
}
// Mitul}

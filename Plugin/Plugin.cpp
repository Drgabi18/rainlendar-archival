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
  $Header: /home/cvsroot/Rainlendar/Plugin/Plugin.cpp,v 1.1 2004/11/06 13:38:59 rainy Exp $

  $Log: Plugin.cpp,v $
  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#pragma warning(disable: 4786)
#pragma warning(disable: 4996)

#include "Plugin.h"
#include "Config.h"
#include "CalendarWindow.h"
#include <tchar.h>

CPlugin::CPlugin()
{
	m_Module = NULL;
	m_Enabled = false;
}

CPlugin::~CPlugin()
{
	if (m_Module)
	{
		FreeLibrary(m_Module);
	}
}

bool CPlugin::Initialize(LPCSTR pluginName, UINT id)
{
	m_Module = LoadLibrary(pluginName);
	m_ID = id;

	if (m_Module)
	{
		fpPluginInitialize = (FPPLUGININITIALIZE)GetProcAddress(m_Module, "Plugin_Initialize");
		fpPluginFinalize = (FPPLUGINFINALIZE)GetProcAddress(m_Module, "Plugin_Finalize");

		fpPluginEnable = (FPPLUGINENABLE)GetProcAddress(m_Module, "Plugin_Enable");
		fpPluginDisable = (FPPLUGINDISABLE)GetProcAddress(m_Module, "Plugin_Disable");

		fpPluginGetName = (FPPLUGINGETNAME)GetProcAddress(m_Module, "Plugin_GetName");
		fpPluginGetAuthor = (FPPLUGINGETAUTHOR)GetProcAddress(m_Module, "Plugin_GetAuthor");
		fpPluginGetVersion = (FPPLUGINGETVERSION)GetProcAddress(m_Module, "Plugin_GetVersion");

		fpPluginReadItems = (FPPLUGINREADITEMS)GetProcAddress(m_Module, "Plugin_ReadItems");
		fpPluginItemNotification = (FPPLUGINITEMNOTIFICATION)GetProcAddress(m_Module, "Plugin_ItemNotification");

		fpPluginEditItem = (FPPLUGINEDITITEM)GetProcAddress(m_Module, "Plugin_EditItem");
		fpPluginExportItems = (FPPLUGINEXPORTITEMS)GetProcAddress(m_Module, "Plugin_ExportItems");
		fpPluginImportItems = (FPPLUGINIMPORTITEMS)GetProcAddress(m_Module, "Plugin_ImportItems");
		fpPluginShowConfig = (FPPLUGINSHOWCONFIG)GetProcAddress(m_Module, "Plugin_ShowConfig");

		if (fpPluginInitialize)
		{
			std::string configFile = CConfig::Instance().GetPath();
			configFile += _T("Rainlendar.ini");
			fpPluginInitialize(m_Module, configFile.c_str(), id);
		}

		if (fpPluginGetName)	// This must exist
		{
			return true;
		}
	}

	return false;
}

LPCTSTR CPlugin::GetName()
{
	if (fpPluginGetName)
	{
		return fpPluginGetName();
	}
	return CCalendarWindow::c_Language.GetString("General", 20);	// "Unknown"
}

LPCTSTR CPlugin::GetAuthor()
{
	if (fpPluginGetAuthor)
	{
		return fpPluginGetAuthor();
	}
	return CCalendarWindow::c_Language.GetString("General", 20);	// "Unknown"
}

UINT CPlugin::GetVersion()
{
	if (fpPluginGetVersion)
	{
		return fpPluginGetVersion();
	}
	return 0;
}

void CPlugin::ReadItems()
{
	if (fpPluginReadItems)
	{
		fpPluginReadItems();
	}
}

void CPlugin::ItemNotification(GUID** guid)
{
	if (fpPluginItemNotification)
	{
		fpPluginItemNotification(guid);
	}
}

bool CPlugin::EditItem(HWND parent, RainlendarItem* item, RAINLENDAR_TYPE type)
{
	if (fpPluginEditItem)
	{
		return fpPluginEditItem(parent, item, type);
	}
	return FALSE;
}

void CPlugin::ExportItems(LPCTSTR configFile, GUID** guids, RAINLENDAR_TYPE type)
{
	if (fpPluginExportItems)
	{
		fpPluginExportItems(configFile, guids, type);
	}
}

bool CPlugin::ImportItems(LPCTSTR configFile, RAINLENDAR_TYPE type)
{
	if (fpPluginImportItems)
	{
		return fpPluginImportItems(configFile, type);
	}
	return false;
}

bool CPlugin::ShowConfig(HWND hwndParent, LPCTSTR configFile)
{
	if (fpPluginShowConfig)
	{
		return fpPluginShowConfig(hwndParent, configFile);
	}
	return false;
}

void CPlugin::Enable()
{
	m_Enabled = true;

	if (fpPluginEnable)
	{
		fpPluginEnable();
	}
}

void CPlugin::Disable()
{
	m_Enabled = false;

	if (fpPluginDisable)
	{
		fpPluginDisable();
	}
}

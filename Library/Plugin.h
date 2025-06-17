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
  $Header: /home/cvsroot/Rainlendar/Library/Plugin.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: Plugin.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.2  2005/03/01 18:50:45  rainy
  no message

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <windows.h>
#include "RainlendarAPI.h"

typedef void (*FPPLUGININITIALIZE)(HINSTANCE, LPCTSTR, UINT);
typedef void (*FPPLUGINFINALIZE)();
typedef void (*FPPLUGINENABLE)();
typedef void (*FPPLUGINDISABLE)();
typedef LPCTSTR (*FPPLUGINGETNAME)();
typedef LPCTSTR (*FPPLUGINGETAUTHOR)();
typedef UINT (*FPPLUGINGETVERSION)();
typedef void (*FPPLUGINREADITEMS)();
typedef void (*FPPLUGINITEMNOTIFICATION)(GUID**);
typedef bool (*FPPLUGINEDITITEM)(HWND, RainlendarItem*, RAINLENDAR_TYPE);
typedef void (*FPPLUGINEXPORTITEMS)(LPCTSTR, GUID**, RAINLENDAR_TYPE);
typedef bool (*FPPLUGINIMPORTITEMS)(LPCTSTR, RAINLENDAR_TYPE);
typedef bool (*FPPLUGINSHOWCONFIG)(HWND, LPCTSTR);

class CPlugin
{
public:
	CPlugin();
	~CPlugin();

	bool Initialize(LPCSTR pluginName, UINT id);

	LPCTSTR GetName();
	LPCTSTR GetAuthor();
	UINT GetVersion();

	void Enable();
	void Disable();
	bool IsEnabled() { return m_Enabled; };

	void ReadItems();
	void ItemNotification(GUID** guids);
	bool EditItem(HWND parent, RainlendarItem* item, RAINLENDAR_TYPE type);
	void ExportItems(LPCTSTR eventFile, GUID** guids, RAINLENDAR_TYPE type);
	bool ImportItems(LPCTSTR eventFile, RAINLENDAR_TYPE type);
	bool ShowConfig(HWND hwndParent, LPCTSTR configFile);

	UINT GetPluginID() { return m_ID; };

	bool HasConfig() { return (fpPluginShowConfig != NULL); };
	bool HasImport() { return (fpPluginImportItems != NULL); };
	bool HasExport() { return (fpPluginExportItems != NULL); };
	bool HasEditItem() { return (fpPluginEditItem != NULL); };

private:
	HMODULE m_Module;
	bool m_Enabled;
	UINT m_ID;

	FPPLUGININITIALIZE fpPluginInitialize;
	FPPLUGINFINALIZE fpPluginFinalize;
	FPPLUGINFINALIZE fpPluginEnable;
	FPPLUGINFINALIZE fpPluginDisable;
	FPPLUGINGETNAME fpPluginGetName;
	FPPLUGINGETAUTHOR fpPluginGetAuthor;
	FPPLUGINGETVERSION fpPluginGetVersion;
	FPPLUGINREADITEMS fpPluginReadItems;
	FPPLUGINITEMNOTIFICATION fpPluginItemNotification;
	FPPLUGINEDITITEM fpPluginEditItem;
	FPPLUGINEXPORTITEMS fpPluginExportItems;
	FPPLUGINIMPORTITEMS fpPluginImportItems;
	FPPLUGINSHOWCONFIG fpPluginShowConfig;
};

#endif

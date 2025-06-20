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
  $Header: /home/cvsroot/Rainlendar/Library/PluginManager.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: PluginManager.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <windows.h>
#include <vector>
#include "Plugin.h"

class CPluginManager
{
public:
	CPluginManager();
	~CPluginManager();

	void LoadPlugins(LPCTSTR path);
	std::vector<CPlugin*> GetPlugins();
	const std::vector<CPlugin*>& GetAllPlugins() { return m_Plugins; };
	CPlugin* GetDefaultPlugin();

	void ReadConfigs();
	void WriteConfigs();

	const std::string& GetDefaultPluginName() { return m_DefaultPluginName; };
	void SetDefaultPluginName(LPCTSTR name) { m_DefaultPluginName = name; };

private:
	std::vector<CPlugin*> m_Plugins;
	std::string m_DefaultPluginName;
};

#endif


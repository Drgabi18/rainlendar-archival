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
  $Header: /home/cvsroot/Rainlendar/Plugins/PluginOutlook/OutlookPlugin.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: OutlookPlugin.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.3  2005/03/25 13:59:47  rainy
  no message

  Revision 1.2  2005/03/01 18:33:11  rainy
  Added possibility to select which message stores are imported.
  No direct linking to mapi32.dll anymore.

  Revision 1.1  2004/12/05 18:14:07  rainy
  Initial version


*/

#ifndef _OUTLOOKPLUGIN_H_
#define _OUTLOOKPLUGIN_H_

#include <string>
#include <vector>

#define PLUGIN_NAME _T("OutlookPlugin")

BOOL CALLBACK SetupProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
void DebugLog(const char* format, ... );

struct MessageStoreName
{
	std::string name;	
	std::vector<std::string> folders;
};

#endif

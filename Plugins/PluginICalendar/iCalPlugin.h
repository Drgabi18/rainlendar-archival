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
  $Header: /home/cvsroot/Rainlendar/Plugins/PluginICalendar/iCalPlugin.h,v 1.2 2005/10/14 17:05:41 rainy Exp $

  $Log: iCalPlugin.h,v $
  Revision 1.2  2005/10/14 17:05:41  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.1  2005/03/01 18:26:06  rainy
  Initial version.

*/

#ifndef _ICALPLUGIN_H_
#define _ICALPLUGIN_H_

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "../../Library/RainlendarAPI.h"
#include <string>
#include <vector>
#include <tchar.h>

#define PLUGIN_NAME _T("iCalPlugin")

#ifdef UNICODE
#define tstring wstring
#else
#define tstring string 
#endif

struct ICALFILENAME
{
	std::tstring filename;
	bool readOnly;	
};

BOOL CALLBACK SetupProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif

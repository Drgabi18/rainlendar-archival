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
  $Header: /home/cvsroot/Rainlendar/Plugins/PluginIniFormat/IniPlugin.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: IniPlugin.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.1  2004/11/06 14:04:44  rainy
  Initial version

*/

#ifndef _INIPLUGIN_H_
#define _INIPLUGIN_H_

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "../../Library/RainlendarAPI.h"
#include <string>
#include <vector>
#include <tchar.h>

#define PLUGIN_NAME _T("IniFormatPlugin")

#ifdef UNICODE
#define tstring wstring
#else
#define tstring string 
#endif

struct INIFILENAME
{
	std::tstring filename;
	bool readOnly;	
};

BOOL CALLBACK SetupProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif

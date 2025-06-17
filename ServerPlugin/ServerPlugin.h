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
  $Header: /home/cvsroot/Rainlendar/ServerPlugin/ServerPlugin.h,v 1.1 2004/12/05 18:17:47 rainy Exp $

  $Log: ServerPlugin.h,v $
  Revision 1.1  2004/12/05 18:17:47  rainy
  Initial version.


*/

#ifndef _SERVERPLUGIN_H_
#define _SERVERPLUGIN_H_

#define PLUGIN_NAME _T("ServerPlugin")

BOOL CALLBACK SetupProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif

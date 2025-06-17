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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/AllEventsDialog.h,v 1.1 2003/05/26 18:44:27 Rainy Exp $

  $Log: AllEventsDialog.h,v $
  Revision 1.1  2003/05/26 18:44:27  Rainy
  Initial version.

*/

#ifndef _ALLEVENTS_H_
#define _ALLEVENTS_H_

#include "CalendarWindow.h"

BOOL CALLBACK AllEventsProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif


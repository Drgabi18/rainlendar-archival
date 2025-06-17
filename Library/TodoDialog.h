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
  $Header: /home/cvsroot/Rainlendar/Library/TodoDialog.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: TodoDialog.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.3  2004/11/06 13:38:59  rainy
  no message

  Revision 1.2  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.1  2003/10/27 17:34:54  Rainy
  Initial version.

*/

#ifndef _TODODIALOG_H_
#define _TODODIALOG_H_

#include "CalendarWindow.h"

void OpenTodoDialog(HWND hwndOwner, HINSTANCE instance, GUID* guid);
bool ShowTodoDialog(HWND hwndOwner, HINSTANCE instance, GUID* guid);

#endif


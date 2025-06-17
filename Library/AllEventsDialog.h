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
  $Header: /home/cvsroot/Rainlendar/Library/AllEventsDialog.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: AllEventsDialog.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.3  2005/03/01 18:42:44  rainy
  Opening second instance of the dialog is disabled.

  Revision 1.2  2004/04/24 11:16:25  rainy
  Fixed a crash bug.

  Revision 1.1  2003/05/26 18:44:27  Rainy
  Initial version.

*/

#ifndef _ALLEVENTS_H_
#define _ALLEVENTS_H_

#include "CalendarWindow.h"

bool OpenAllEventsDialog(HWND parent);

#endif


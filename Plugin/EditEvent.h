/*
  Copyright (C) 2000 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Plugin/EditEvent.h,v 1.6 2004/11/06 13:38:58 rainy Exp $

  $Log: EditEvent.h,v $
  Revision 1.6  2004/11/06 13:38:58  rainy
  no message

  Revision 1.5  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.4  2003/08/09 16:36:59  Rainy
  Complete rewrite.

  Revision 1.3  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.2  2002/01/10 16:47:15  rainy
  Added support for bitmap/color,

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef _EDITEVENT_H_
#define _EDITEVENT_H_

#include "CalendarWindow.h"

void OpenEditEventDialog(HWND hwndOwner, HINSTANCE instance, GUID* guid, CFileTime* startTime);
bool ShowEditEvent(HWND hwndOwner, HINSTANCE instance, RainlendarEvent* event);

#endif


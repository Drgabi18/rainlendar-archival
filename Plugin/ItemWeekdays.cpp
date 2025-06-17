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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/ItemWeekdays.cpp,v 1.7 2003/06/15 09:49:13 Rainy Exp $

  $Log: ItemWeekdays.cpp,v $
  Revision 1.7  2003/06/15 09:49:13  Rainy
  Added support for multiple calendars.

  Revision 1.6  2002/11/12 18:11:34  rainy
  The interface of Paint changed a little.

  Revision 1.5  2002/08/24 11:10:36  rainy
  Changed the error handling.

  Revision 1.4  2002/08/03 16:15:29  rainy
  Added color setting for the rasterizer.

  Revision 1.3  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.2  2001/12/23 10:00:17  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "ItemWeekdays.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include "RasterizerFont.h"
#include "CalendarWindow.h"

#define NUMOine NUMOine NUMOine NUMOine NUMOine NUMOine NUMOine NUMOine NUMOin
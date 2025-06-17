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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Error.h,v 1.1.1.1 2001/10/29 18:56:23 rainy Exp $

  $Log: Error.h,v $
  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __ERROR_H__
#define __ERROR_H__

enum RAIN_ERROR {
	ERR_NONE,
	ERR_OUTOFMEM,
	ERR_WINDOWCLASS,
	ERR_WINDOW,
	ERR_BACKGROUND,
	ERR_BACKGROUNDALPHASIZE,
	ERR_ALPHASIZE,
	ERR_CREATEFONT,
	ERR_TEXTDIMENSIONS,
};

static const char* ErrorMessages[] = {
	"No Errors!",
	"Out of memory!",
	"Unable to create the window class!",
	"Unable to create the main window!",
	"Unable to fetch the desktop's background!",
	"Background and it's alpha-map must be same size!!",
	"Bitmap and it's alpha-map must be same size!!",
	"Unable to create font!",
	"Unable to calculate text dimensions!",
};

static const char* GetErrorMessage(RAIN_ERROR Error) { return ErrorMessages[Error]; };

#endif

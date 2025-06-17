/*
  Copyright (C) 2002 Kimmo Pekkola + few lsapi developers

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
  $Header: //RAINBOX/cvsroot/Rainlendar/Plugin/Litestep.h,v 1.4 2003/08/09 16:34:21 Rainy Exp $

  $Log: Litestep.h,v $
  Revision 1.4  2003/08/09 16:34:21  Rainy
  Comment changed

  Revision 1.3  2003/03/22 20:30:04  rainy
  no more Lean and Mean

  Revision 1.2  2002/08/24 11:10:07  rainy
  Added support for logging.

  Revision 1.1  2002/05/30 18:27:42  rainy
  Initial version

*/

#ifndef __LITESTEP_H__
#define __LITESTEP_H__

#include <windows.h>

#define magicDWord             0x49474541
#define LM_GETREVID            9265
#define LM_REGISTERMESSAGE     9263
#define LM_UNREGISTERMESSAGE   9264

// log level constants
#define LOG_ERROR 1
#define LOG_WARNING 2
#define LOG_NOTICE 3
#define LOG_DEBUG 4

typedef void (BangCommand)(HWND sender, LPCSTR args);

// Call this if you want to use lsapi.dll's functions instead of stubs
void InitalizeLitestep();

// The stubs
BOOL AddBangCommand(LPCSTR command, BangCommand f);
HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance, int xoffset, int yoffset);
HWND GetLitestepWnd(void);
BOOL GetRCString(LPCSTR lpKeyName, LPSTR value, LPCSTR defStr, int maxLen);
HINSTANCE LSExecute(HWND Owner, LPCSTR szCommand, int nShowCmd);
HBITMAP LoadLSImage(LPCSTR szFile, LPCSTR szImage);
BOOL RemoveBangCommand(LPCSTR command);
void TransparentBltLS (HDC dc, int nXDest, int nYDest, int nWidth, int nHeight, HDC tempDC, int nXSrc, int nYSrc, COLORREF colorTransparent);
void VarExpansion(LPSTR buffer, LPCSTR value);
BOOL LSLog(int nLevel, LPCSTR pszModule, LPCSTR pszMessage);

#endif
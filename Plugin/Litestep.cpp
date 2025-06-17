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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Litestep.cpp,v 1.1 2002/05/30 18:27:42 rainy Exp $

  $Log: Litestep.cpp,v $
  Revision 1.1  2002/05/30 18:27:42  rainy
  Initial version

*/

#include "Litestep.h"
#include "Error.h"
#include <shellapi.h>

typedef BOOL (*FPADDBANGCOMMAND)(LPCSTR command, BangCommand f);
FPADDBANGCOMMAND fpAddBangCommand = NULL;

typedef HRGN (*FPBITMAPTOREGION)(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance, int xoffset, int yoffset);
FPBITMAPTOREGION fpBitmapToRegion = NULL;

typedef HWND (*FPGETLITESTEPWND)(void);
FPGETLITESTEPWND fpGetLitestepWnd = NULL;

typedef BOOL (*FPGETRCSTRING)(LPCSTR lpKeyName, LPSTR value, LPCSTR defStr, int maxLen);
FPGETRCSTRING fpGetRCString = NULL;

typedef HINSTANCE (*FPLSEXECUTE)(HWND Owner, LPCSTR szCommand, int nShowCmd);
FPLSEXECUTE fpLSExecute = NULL;

typedef HBITMAP (*FPLOADLSIMAGE)(LPCSTR szFile, LPCSTR szImage);
FPLOADLSIMAGE fpLoadLSImage = NULL;

typedef BOOL (*FPREMOVEBANGCOMMAND)(LPCSTR command);
FPREMOVEBANGCOMMAND fpRemoveBangCommand = NULL;

typedef void (*FPTRANSPARENTBLTLS)(HDC dc, int nXDest, int nYDest, int nWidth, int nHeight, HDC tempDC, int nXSrc, int nYSrc, COLORREF colorTransparent);
FPTRANSPARENTBLTLS fpTransparentBltLS = NULL;

typedef void (*FPVAREXPANSION)(LPSTR buffer, LPCSTR value);
FPVAREXPANSION fpVarExpansion = NULL;
 

void InitalizeLitestep()
{
	// Use lsapi's methods instead of the stubs
	HINSTANCE h = LoadLibrary("lsapi.dll");
	if (h == NULL) throw CError("Unable to find lsapi.dll", __LINE__, __FILE__);

	fpAddBangCommand = (FPADDBANGCOMMAND)GetProcAddress(h, "AddBangCommand");
	if (fpAddBangCommand == NULL) throw CError("Unable to find AddBangCommand in lsapi.dll", __LINE__, __FILE__); 

	fpBitmapToRegion = (FPBITMAPTOREGION)GetProcAddress(h, "BitmapToRegion");
	if (fpBitmapToRegion == NULL) throw CError("Unable to find BitmapToRegion in lsapi.dll", __LINE__, __FILE__); 

	fpGetLitestepWnd = (FPGETLITESTEPWND)GetProcAddress(h, "GetLitestepWnd");
	if (fpGetLitestepWnd == NULL) throw CError("Unable to find GetLitestepWnd in lsapi.dll", __LINE__, __FILE__); 

	fpGetRCString = (FPGETRCSTRING)GetProcAddress(h, "GetRCString");
	if (fpGetRCString == NULL) throw CError("Unable to find GetRCString in lsapi.dll", __LINE__, __FILE__); 

	fpLSExecute = (FPLSEXECUTE)GetProcAddress(h, "LSExecute");
	if (fpLSExecute == NULL) throw CError("Unable to find LSExecute in lsapi.dll", __LINE__, __FILE__); 

	fpLoadLSImage = (FPLOADLSIMAGE)GetProcAddress(h, "LoadLSImage");
	if (fpLoadLSImage == NULL) throw CError("Unable to find LoadLSImage in lsapi.dll", __LINE__, __FILE__); 

	fpRemoveBangCommand = (FPREMOVEBANGCOMMAND)GetProcAddress(h, "RemoveBangCommand");
	if (fpRemoveBangCommand == NULL) throw CError("Unable to find RemoveBangCommand in lsapi.dll", __LINE__, __FILE__); 

	fpTransparentBltLS = (FPTRANSPARENTBLTLS)GetProcAddress(h, "TransparentBltLS");
	if (fpTransparentBltLS == NULL) throw CError("Unable to find TransparentBltLS in lsapi.dll", __LINE__, __FILE__); 

	fpVarExpansion = (FPVAREXPANSION)GetProcAddress(h, "VarExpansion");
	if (fpVarExpansion == NULL) throw CError("Unable to find VarExpansion in lsapi.dll", __LINE__, __FILE__); 
}

BOOL AddBangCommand(LPCSTR command, BangCommand f)
{
	// Use the lsapi.dll version of the method if possible
	if (fpAddBangCommand) return fpAddBangCommand(command, f);

	// The stub implementation
	return true;
}

HWND GetLitestepWnd(void)
{
	// Use the lsapi.dll version of the method if possible
	if (fpGetLitestepWnd) return fpGetLitestepWnd();

	// The stub implementation
	return NULL;
}

BOOL RemoveBangCommand(LPCSTR command)
{
	// Use the lsapi.dll version of the method if possible
	if (fpRemoveBangCommand) return fpRemoveBangCommand(command);

	// The stub implementation
	return true;
}

BOOL GetRCString(LPCSTR lpKeyName, LPSTR value, LPCSTR defStr, int maxLen)
{
	// Use the lsapi.dll version of the method if possible
	if (fpGetRCString) return fpGetRCString(lpKeyName, value, defStr, maxLen);

	// The stub implementation
	return true;
}

void VarExpansion(LPSTR buffer, LPCSTR value)
{
	// Use the lsapi.dll version of the method if possible
	if (fpVarExpansion) 
	{
		fpVarExpansion(buffer, value);
	}
	else
	{
		// The stub implementation
		if (buffer != value)
		{
			strcpy(buffer, value);
		}
	}
}

HRGN BitmapToRegion(HBITMAP hbm, COLORREF clrTransp, COLORREF clrTolerance, int xoffset, int yoffset)
{
	// Use the lsapi.dll version of the method if possible
	if (fpBitmapToRegion) return fpBitmapToRegion(hbm, clrTransp, clrTolerance, xoffset, yoffset);

	// start with a completely transparent rgn
	// this is more correct as no bmp, should render a transparent background
	HRGN hRgn = CreateRectRgn(0, 0, 0, 0);

	if (hbm)
	{
		// create a dc for the 32 bit dib
		HDC hdcMem = CreateCompatibleDC(NULL);
		if (hdcMem)
		{
			VOID *pbits32;
			HBITMAP hbm32;
			BITMAP bm;
			// get the size
			GetObject(hbm, sizeof(BITMAP), &bm);

			BITMAPINFOHEADER bmpInfo32;
			bmpInfo32.biSize	= sizeof(BITMAPINFOHEADER);
			bmpInfo32.biWidth	= bm.bmWidth;
			bmpInfo32.biHeight	= bm.bmHeight;
			bmpInfo32.biPlanes	= 1;
			bmpInfo32.biBitCount	= 32;
			bmpInfo32.biCompression	= BI_RGB;
			bmpInfo32.biSizeImage	= 0;
			bmpInfo32.biXPelsPerMeter	= 0;
			bmpInfo32.biYPelsPerMeter	= 0;
			bmpInfo32.biClrUsed	= 0;
			bmpInfo32.biClrImportant	= 0;

			hbm32 = CreateDIBSection(hdcMem, (BITMAPINFO *) & bmpInfo32, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				HBITMAP hbmOld32 = (HBITMAP)SelectObject(hdcMem, hbm32);

				// Create a DC just to copy the bitmap into the memory D
				HDC hdcTmp = CreateCompatibleDC(hdcMem);
				if (hdcTmp)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits
					int y = 0;
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// get the limits for the colors
					BYTE clrHiR = ( 0xff - GetRValue( clrTolerance ) > GetRValue( clrTransp ) ) ? GetRValue( clrTransp ) + GetRValue( clrTolerance ) : 0xff;
					BYTE clrHiG = ( 0xff - GetGValue( clrTolerance ) > GetGValue( clrTransp ) ) ? GetGValue( clrTransp ) + GetGValue( clrTolerance ) : 0xff;
					BYTE clrHiB = ( 0xff - GetBValue( clrTolerance ) > GetBValue( clrTransp ) ) ? GetBValue( clrTransp ) + GetBValue( clrTolerance ) : 0xff;
					BYTE clrLoR = ( GetRValue( clrTolerance ) < GetRValue( clrTransp ) ) ? GetRValue( clrTransp ) - GetRValue( clrTolerance ) : 0x00;
					BYTE clrLoG = ( GetGValue( clrTolerance ) < GetGValue( clrTransp ) ) ? GetGValue( clrTransp ) - GetGValue( clrTolerance ) : 0x00;
					BYTE clrLoB = ( GetBValue( clrTolerance ) < GetBValue( clrTransp ) ) ? GetBValue( clrTransp ) - GetBValue( clrTolerance ) : 0x00;

					// Copy the bitmap into the memory D
					HBITMAP hbmOld = (HBITMAP)SelectObject(hdcTmp, hbm);

					BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcTmp, 0, 0, SRCCOPY);

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically
					BYTE *p;
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					while (y < bm.bmHeight)
					{
						int x = 0;
						while ( x < bm.bmWidth )
						{
							int x0 = 0;
							// loop through all transparent pixels...
							while ( x < bm.bmWidth )
							{
								p = p32 + 4 * x;

								// if the pixel is non-transparent
								{
									bool isOpaque = *p < clrLoB || *p > clrHiB;
									p++;
									isOpaque |= *p < clrLoG || *p > clrHiG;
									p++;
									isOpaque |= *p < clrLoR || *p > clrHiR;
									if (isOpaque)
										break;
								}

								x++;
							}
							// set first non transparent pixel
							x0 = x;
							// loop through all non transparent pixels
							while ( x < bm.bmWidth )
							{
								p = p32 + 4 * x;
								// if the pixel is transparent, then break
								if (*p >= clrLoB && *p <= clrHiB)
								{
									p++;
									if (*p >= clrLoG && *p <= clrHiG)
									{
										p++;
										if (*p >= clrLoR && *p <= clrHiR)
											break;
									}
								}
								x++;
							}
							// if found one or more non-transparent pixels in a row, add them to the rgn...
							if ((x - x0) > 0)
							{
								HRGN hTempRgn = CreateRectRgn(x0 + xoffset, y + yoffset, x + xoffset, y + 1 + yoffset);
								CombineRgn(hRgn, hRgn, hTempRgn, RGN_OR);
								DeleteObject(hTempRgn);
							}
							x++;
						}
						y++;
						p32 -= bm32.bmWidthBytes;
					}
					// Clean up
					SelectObject(hdcTmp, hbmOld);
					DeleteDC(hdcTmp);
				}
				SelectObject(hdcMem, hbmOld32);
				DeleteObject(hbm32);
			}
			DeleteDC(hdcMem);
		}
	}
	return hRgn;
}

HINSTANCE LSExecute(HWND Owner, LPCSTR szCommand, int nShowCmd)
{
	// Use the lsapi.dll version of the method if possible
	if (fpLSExecute) return fpLSExecute(Owner, szCommand, nShowCmd);

	// The stub implementation (some of this code is taken from lsapi.cpp)
	if(szCommand == NULL || strlen(szCommand) == 0) return NULL;

	char dir[_MAX_DIR];
	char full_directory[_MAX_DIR + _MAX_DRIVE + 1];
	char* command = _strdup(szCommand);
	char* args = NULL;
	DWORD type;

	// find the space and split the command from there
	args = strchr(command, ' ');
	if (args)
	{
		*args = 0;
		args++;
	}

	_splitpath(command, full_directory, dir, NULL, NULL);
	strcat(full_directory, dir);

	type = GetFileAttributes(command);
	if (type & FILE_ATTRIBUTE_DIRECTORY && type != 0xFFFFFFFF)
	{
		HINSTANCE instance = ShellExecute(Owner, "open", command, args, NULL, nShowCmd ? nShowCmd : SW_SHOWNORMAL);
		free(command);
		return instance;
	}

	SHELLEXECUTEINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(SHELLEXECUTEINFO);
	si.hwnd = Owner;
	si.lpVerb = "open";
	si.lpFile = command;
	si.lpParameters = args;
	si.lpDirectory = full_directory;
	si.nShow = nShowCmd ? nShowCmd : SW_SHOWNORMAL;
	si.fMask = SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_NO_UI;
	ShellExecuteEx(&si);
	free(command);
	return si.hInstApp;
}

HBITMAP LoadLSImage(LPCSTR szFile, LPCSTR szImage)
{
	// Use the lsapi.dll version of the method if possible
	if (fpLoadLSImage) return fpLoadLSImage(szFile, szImage);

	// The stub implementation
	return (HBITMAP)LoadImage(NULL, szFile, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
}

void TransparentBltLS(HDC hdcDst, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, COLORREF colorTransparent)
{
	// Use the lsapi.dll version of the method if possible
	if (fpTransparentBltLS) 
	{
		fpTransparentBltLS(hdcDst, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, colorTransparent);
	}
	else
	{
		HDC hdcMem, hdcMask, hdcDstCpy;
		HBITMAP hbmMask, hbmMem, hbmDstCpy;
		HBITMAP hbmOldMem, hbmOldMask, hbmOldDstCpy;

		// create a destination compatble dc containing
		// a copy of the destination dc
		hdcDstCpy	= CreateCompatibleDC(hdcDst);
		hbmDstCpy	= CreateCompatibleBitmap(hdcDst, nWidth, nHeight);
		hbmOldDstCpy = (HBITMAP)SelectObject(hdcDstCpy, hbmDstCpy);

		BitBlt(hdcDstCpy, 0, 0, nWidth, nHeight, hdcDst, nXDest, nYDest, SRCCOPY);

		// create a destination compatble dc containing
		// a copy of the source dc
		hdcMem	= CreateCompatibleDC(hdcDst);
		hbmMem	= CreateCompatibleBitmap(hdcDst, nWidth, nHeight);
		hbmOldMem = (HBITMAP)SelectObject(hdcMem, hbmMem);

		BitBlt(hdcMem, 0, 0, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, SRCCOPY);

		// the transparent color should be selected as
		// bkcolor into the memory dc
		SetBkColor(hdcMem, colorTransparent);

		// Create monochrome bitmap for the mask
		hdcMask	= CreateCompatibleDC(hdcDst);
		hbmMask = CreateBitmap(nWidth, nHeight, 1, 1, NULL);
		hbmOldMask = (HBITMAP)SelectObject(hdcMask, hbmMask);

		// Create the mask from the memory dc
		BitBlt(hdcMask, 0, 0, nWidth, nHeight, hdcMem, 0, 0, SRCCOPY);

		// Set the background in hdcMem to black. Using SRCPAINT with black
		// and any other color results in the other color, thus making
		// black the transparent color
		SetBkColor(hdcMem, RGB(0, 0, 0));
		SetTextColor(hdcMem, RGB(255, 255, 255));

		BitBlt(hdcMem, 0, 0, nWidth, nHeight, hdcMask, 0, 0, SRCAND);

		// Set the foreground to black. See comment above.
		SetBkColor(hdcDst, RGB(255, 255, 255));
		SetTextColor(hdcDst, RGB(0, 0, 0));

		BitBlt(hdcDstCpy, 0, 0, nWidth, nHeight, hdcMask, 0, 0, SRCAND);

		// Combine the foreground with the background
		BitBlt(hdcDstCpy, 0, 0, nWidth, nHeight, hdcMem, 0, 0, SRCPAINT);

		// now we have created the image we want to blt
		// in the destination copy dc
		BitBlt(hdcDst, nXDest, nYDest, nWidth, nHeight, hdcDstCpy, 0, 0, SRCCOPY);

		SelectObject(hdcMask, hbmOldMask);
		DeleteObject(hbmMask);
		DeleteDC(hdcMask);

		SelectObject(hdcMem, hbmOldMem);
		DeleteObject(hbmMem);
		DeleteDC(hdcMem);

		SelectObject(hdcDstCpy, hbmOldDstCpy);
		DeleteObject(hbmDstCpy);
		DeleteDC(hdcDstCpy);
	}
}


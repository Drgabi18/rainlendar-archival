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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Background.cpp,v 1.6 2002/05/30 18:28:05 rainy Exp $

  $Log: Background.cpp,v $
  Revision 1.6  2002/05/30 18:28:05  rainy
  Removed some Litestep related stuff.

  Revision 1.5  2002/05/23 17:33:42  rainy
  Removed all MFC stuff

  Revision 1.4  2002/02/27 18:59:00  rainy
  Fixed multimonitor stuff.
  Added support for background stretching.

  Revision 1.3  2002/01/15 17:59:44  rainy
  Now uses different way to get the desktop image.

  Revision 1.2  2001/12/23 10:02:31  rainy
  The background is grapped only once if ran as wharf module.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "Background.h"
#include "CalendarWindow.h"
#include "Error.h"
#include "RasterizerBitmap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBackground::CBackground()
{
	m_Width = 0;
	m_Height = 0;
	m_Alpha = false;
	m_AlphaImage = NULL;
	m_Image = NULL;
	m_Background = NULL;
}

CBackground::~CBackground()
{
	Initialize();	// This gets rid of the bitmaps
}

/* 
** Initialize
**
** Intializes the background
**
*/
void CBackground::Initialize()
{
	if (m_AlphaImage) DeleteObject(m_AlphaImage);
	if (m_Image) DeleteObject(m_Image);
	if (m_Background) DeleteObject(m_Background);

	m_Width = 0;
	m_Height = 0;
	m_Alpha = false;
	m_AlphaImage = NULL;
	m_Image = NULL;
	m_Background = NULL;
}

/* 
** Load
**
** Loads the given image.
**
*/
void CBackground::Load(std::string Filename)
{
	BITMAP bm;
	char buffer[1024];

	if(!CRainlendar::GetDummyLitestep()) 
	{
		VarExpansion(buffer, Filename.c_str());
		Filename = buffer;
	}

	// Check for absolute path
	if(-1 == Filename.find(':')) 
	{
		Filename.insert(0, CCalendarWindow::c_Config.GetPath());
	} 

	m_Image = LoadLSImage(Filename.c_str(), NULL);

	if(m_Image == NULL) 
	{
		std::string err = "File not found: ";
		err += Filename;
		throw CError(err, __LINE__, __FILE__);
	}

	GetObject(m_Image, sizeof(BITMAP), &bm);
	m_Width = bm.bmWidth;
	m_Height = bm.bmHeight;

	// Check for Alpha-map
	m_Alpha = false;
	int pos = Filename.rfind('.');
	if (pos != -1)
	{
		std::string alphaName;

		alphaName = Filename;
		alphaName.insert(pos, "-Alpha");

		m_AlphaImage = LoadLSImage(alphaName.c_str(), NULL);

		if(m_AlphaImage != NULL) 
		{
			// Check that it is correct size
			GetObject(m_AlphaImage, sizeof(BITMAP), &bm);
			if (m_Width != bm.bmWidth || m_Height != bm.bmHeight) throw CError(CError::ERR_BACKGROUNDALPHASIZE, __LINE__, __FILE__);
			m_Alpha = true;
		}
	}
}

/* 
** Create
**
** Creates the background image from the desktop and loaded image.
** Width and height are from current window size.
** Window's size must be correct when calling this.
**
*/
void CBackground::Create(int X, int Y, int Width, int Height)
{
	int i, j;
	HWND Desktop;
	HDC DDC;
	HDC tmpDC;
	HDC BGDC;
	HBITMAP OldBitmap;
	HBITMAP OldBitmap2;
	HBITMAP tmpBitmap;
	HBITMAP tmpBitmapA;
	int RealWidth, RealHeight;

	Desktop = GetDesktopWindow();
	if(Desktop == NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);

	DDC = GetDC(Desktop);
	if(DDC == NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);
		
	tmpDC = CreateCompatibleDC(DDC);
	if(tmpDC == NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);

	BGDC = CreateCompatibleDC(DDC);
	if(BGDC == NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);

	ReleaseDC(Desktop, DDC);

	RealWidth = max(Width, m_Width);
	RealHeight = max(Height, m_Height);

	if(m_Width < Width || m_Height < Height) 
	{
		// Must tile the background and possible alphamap

		OldBitmap2 = (HBITMAP)SelectObject(BGDC, m_Image);

		tmpBitmap = CreateCompatibleBitmap(BGDC, RealWidth, RealHeight);
		if(tmpBitmap == NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);

		if(m_Alpha) 
		{
			tmpBitmapA = CreateCompatibleBitmap(BGDC, RealWidth, RealHeight);
			if(tmpBitmapA == NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);
		}

		OldBitmap = (HBITMAP)SelectObject(tmpDC, tmpBitmap);

		if(CCalendarWindow::c_Config.GetBackgroundMode() == MODE_TILE)
		{
			// Tile the background
			for(j=0; j<RealHeight; j+=m_Height) 
			{
				for(i=0; i<RealWidth; i+=m_Width) 
				{
					BitBlt(tmpDC, i, j, min(m_Width, RealWidth-i), min(m_Height, RealHeight-j), BGDC, 0, 0, SRCCOPY);
				}
			}

			// Alpha must be tiled too
			if(m_Alpha) 
			{
				SelectObject(tmpDC, tmpBitmapA);
				SelectObject(BGDC, m_AlphaImage);
				for(j=0; j<RealHeight; j+=m_Height) 
				{
					for(i=0; i<RealWidth; i+=m_Width) 
					{
						BitBlt(tmpDC, i, j, min(m_Width, RealWidth-i), min(m_Height, RealHeight-j), BGDC, 0, 0, SRCCOPY);
					}
				}
			}
		}
		else
		{
			SetStretchBltMode(tmpDC, HALFTONE);
			SetBrushOrgEx(tmpDC, 0, 0, NULL);
			StretchBlt(tmpDC, 0, 0, Width, Height, BGDC, 0, 0, m_Width, m_Width, SRCCOPY);

			// Alpha must be stretched too
			if(m_Alpha) 
			{
				SelectObject(tmpDC, tmpBitmapA);
				SelectObject(BGDC, m_AlphaImage);
				StretchBlt(tmpDC, 0, 0, Width, Height, BGDC, 0, 0, m_Width, m_Width, SRCCOPY);
			}
		}

		SelectObject(tmpDC, OldBitmap);
		SelectObject(BGDC, OldBitmap2);

		DeleteObject(m_Image);
		m_Image = tmpBitmap;

		if(m_Alpha) 
		{
			DeleteObject(m_AlphaImage);
			m_AlphaImage = tmpBitmapA;
		}

		m_Width = RealWidth;
		m_Height = RealHeight;
	}

	if(m_Width==0 || m_Height==0) return;

	if (m_Alpha || CCalendarWindow::c_Config.GetBackgroundMode() == CBackground::MODE_COPY)
	{
		// Take a copy of the BG
		CopyBackground(X, Y, m_Width, m_Height);

		if(m_Alpha) 
		{
			// ..and create and alphamasked version of the background bitmap
			CRasterizerBitmap::CreateAlpha(m_Image, m_AlphaImage, m_Background);
			DeleteObject(m_AlphaImage);		// We won't need this one anymore
			DeleteObject(m_Background);		// Delete the old (if there was one)
			m_Background = m_Image;
		} 
	}
	else 
	{
		if (m_Background) DeleteObject(m_Background);			// Delete the old (if there was one)
		m_Background = m_Image;
	}

	DeleteDC(BGDC);
	DeleteDC(tmpDC);
}

/* 
** CopyBackground
**
** Takes a copy of the desktop to use as background
** The copy goes to m_Background.
*/
void CBackground::CopyBackground(int X, int Y, int Width, int Height) 
{
	HDC tmpDC;
	HBITMAP OldBitmap;
	HDC WinDC;

	if (m_Background) DeleteObject(m_Background);	// Delete the old (if there was one)
	
	if(GetRainlendar() && !(GetRainlendar()->IsWharf()))
	{
		// The new way
		m_Background = GetWallpaper(X, Y, Width, Height);
	}

	if(m_Background == NULL)
	{
		// If the new way fails, use the old way
		WinDC = GetWindowDC(GetRainlendar()->GetCalendarWindow().GetWindow());
		if(WinDC == NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);

		// These are initialized only if grapping is successful
		m_Width=0;
		m_Height=0;

		tmpDC = CreateCompatibleDC(WinDC);
		if(tmpDC==NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);

		m_Background = CreateCompatibleBitmap(WinDC, Width, Height);
		if(m_Background==NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);

		// Fetch the background
		OldBitmap = (HBITMAP)SelectObject(tmpDC, m_Background);
		BitBlt(tmpDC, 0, 0, Width, Height, WinDC, 0, 0, SRCCOPY);
		
		ReleaseDC(GetRainlendar()->GetCalendarWindow().GetWindow(), WinDC);

		SelectObject(tmpDC, OldBitmap);
		DeleteDC(tmpDC);
	}

	m_Width = Width;
	m_Height = Height;
}

/*
** GetWallpaper
**
** Loads the background image and cuts a given part from it.
**
*/
HBITMAP CBackground::GetWallpaper(int X, int Y, int Width, int Height)
{
	DWORD size = 256;
	char str[256];
	bool tile = false;
	bool stretch = false;
	COLORREF bgColor = 0;
	HBITMAP bitmap = NULL;
	HBITMAP bgBitmap = NULL;
	
    HKEY hKey = NULL;

	// Get the wallpaper name and options from registry
    if(RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
	{
		if(RegQueryValueEx(hKey, "Wallpaper", NULL, NULL, (LPBYTE)&str, (LPDWORD)&size) == ERROR_SUCCESS)
		{
			if(strlen(str) > 0)
			{
				bitmap = LoadLSImage(str, NULL);
				if(bitmap == NULL) return NULL;	// Unable to load the bitmap :(
			}
		}
		size = 256;
		if(RegQueryValueEx(hKey, "TileWallpaper", NULL, NULL, (LPBYTE)&str, (LPDWORD)&size) == ERROR_SUCCESS)
		{
			if(strcmp(str, "1") == 0) tile = true;
		}
		size = 256;
		if(RegQueryValueEx(hKey, "WallpaperStyle", NULL, NULL, (LPBYTE)&str, (LPDWORD)&size) == ERROR_SUCCESS)
		{
			if(strcmp(str, "2") == 0) stretch = true;
		}
		RegCloseKey(hKey);
	}

	size = 256;
	// Also get the background color
    if(RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Colors", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
	{
		if(RegQueryValueEx(hKey, "Background", NULL, NULL, (LPBYTE)&str, (LPDWORD)&size) == ERROR_SUCCESS)
		{
			int R, G, B;
			sscanf(str, "%i %i %i", &R, &G, &B);
			bgColor = RGB(R, G, B);
		}
		RegCloseKey(hKey);
	}

	// Get the screen size
	RECT screenRect;
	GetClientRect(GetDesktopWindow(), &screenRect); 
	
	HDC winDC = GetDC(GetDesktopWindow());
	if(winDC == NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);

	HDC tmpDC = CreateCompatibleDC(winDC);
	if(tmpDC == NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);

	bgBitmap = CreateCompatibleBitmap(winDC, Width, Height);
	if(bgBitmap==NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);

	ReleaseDC(GetDesktopWindow(), winDC);

	HBITMAP oldBitmap = (HBITMAP)SelectObject(tmpDC, bgBitmap);
	
	// Fill the bitmap with bgcolor
	HBRUSH brush = CreateSolidBrush(bgColor);
	RECT r = { 0, 0, Width, Height };
	FillRect(tmpDC, &r, brush);
	DeleteObject(brush);

	if(bitmap)
	{
		HDC bgDC = CreateCompatibleDC(tmpDC);
		if(bgDC == NULL) throw CError(CError::ERR_BACKGROUND, __LINE__, __FILE__);
		HBITMAP oldBitmap2 = (HBITMAP)SelectObject(bgDC, bitmap);

		// Get the size of the loaded wallpaper
		BITMAP bm;
		GetObject(bitmap, sizeof(BITMAP), &bm);

		// We'll have to do the multimonitor stuff like this, cooz 95 or NT don't have the functions.
		// Multimon?
		if (GetSystemMetrics(SM_CMONITORS) > 0)
		{
			typedef HMONITOR (WINAPI * FPMONITORFROMPOINT)( POINT pt, DWORD dwFlags);
			typedef BOOL (WINAPI * FPGETMONITORINFO)(HMONITOR hMonitor, LPMONITORINFO lpmi);
			FPMONITORFROMPOINT fpMonitorFromPoint;
			FPGETMONITORINFO	fpGetMonitorInfo;

			HINSTANCE h = LoadLibrary("user32.dll");
			
			fpMonitorFromPoint = (FPMONITORFROMPOINT)GetProcAddress(h, "MonitorFromPoint");
			fpGetMonitorInfo = (FPGETMONITORINFO)GetProcAddress(h, "GetMonitorInfoA");
			
			// If the window is off the primary monitor, we'll have to modify the values a bit
			POINT point = { X, Y };
			HMONITOR monitor = fpMonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);

			if (monitor)
			{
				MONITORINFO monInfo;
				monInfo.cbSize = sizeof(MONITORINFO);
				fpGetMonitorInfo(monitor, &monInfo);
				if(monInfo.dwFlags != MONITORINFOF_PRIMARY)
				{
					// Not a primary monitor -> adjust the values
					if(tile)
					{
						screenRect.left = 0;
						screenRect.top = 0;
						screenRect.right = monInfo.rcMonitor.right;
						screenRect.bottom = monInfo.rcMonitor.bottom;
						if(screenRect.left > screenRect.right) 
						{
							screenRect.left = screenRect.right;
							screenRect.right = 0;
						}
						if(screenRect.top > screenRect.bottom) 
						{
							screenRect.top = screenRect.bottom;
							screenRect.bottom = 0;
						}
					}
					else if(stretch)
					{
						X -= monInfo.rcMonitor.left + (((monInfo.rcMonitor.right - monInfo.rcMonitor.left) - screenRect.right) / 2);
						Y -= monInfo.rcMonitor.top + (((monInfo.rcMonitor.bottom - monInfo.rcMonitor.top) - screenRect.bottom) / 2);;
					}
					else
					{
						X -= monInfo.rcMonitor.left;
						Y -= monInfo.rcMonitor.top;
						screenRect.right = (monInfo.rcMonitor.right - monInfo.rcMonitor.left);
						screenRect.bottom = (monInfo.rcMonitor.bottom - monInfo.rcMonitor.top);
					}
				}
			}
			FreeLibrary(h);
		}

		if(stretch)
		{
			SetStretchBltMode(tmpDC, HALFTONE);
			SetBrushOrgEx(tmpDC, 0, 0, NULL);

			StretchBlt(tmpDC, -X, -Y, screenRect.right - screenRect.left, screenRect.bottom - screenRect.top,
					   bgDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		}
		else if(tile)
		{
			// We should probably calculate the proper starting point, but why bother
			for(int i = screenRect.left; i < screenRect.right; i += bm.bmWidth)
			{
				for(int j = screenRect.top; j < screenRect.bottom; j += bm.bmHeight)
				{
					BitBlt(tmpDC, i - X, j - Y, bm.bmWidth, bm.bmHeight,
						   bgDC, 0, 0, SRCCOPY);
				}
			}
		}
		else // Center
		{
			int imageLeft = (screenRect.right - screenRect.left) / 2 - bm.bmWidth / 2;
			int imageTop = (screenRect.bottom - screenRect.top) / 2 - bm.bmHeight / 2;
			BitBlt(tmpDC, imageLeft - X, imageTop - Y, bm.bmWidth, bm.bmHeight,
				   bgDC, 0, 0, SRCCOPY);
		}

		SelectObject(bgDC, oldBitmap2);
		DeleteDC(bgDC);
		DeleteObject(bitmap);	// We do not need this one anymore
	}

	SelectObject(tmpDC, oldBitmap);
	DeleteDC(tmpDC);

	return bgBitmap;
}

/* 
** Paint
**
** Paints the background
**
*/
void CBackground::Paint(HDC dc)
{
	HBITMAP OldBitmap;
	HDC tmpDC;

	tmpDC = CreateCompatibleDC(dc);
	OldBitmap = (HBITMAP)SelectObject(tmpDC, m_Background);

	BitBlt(dc, 0, 0, m_Width, m_Height, tmpDC, 0, 0, SRCCOPY);

	SelectObject(tmpDC, OldBitmap);
	DeleteDC(tmpDC);
}

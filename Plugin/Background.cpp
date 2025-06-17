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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Background.cpp,v 1.11 2002/11/25 17:12:24 rainy Exp $

  $Log: Background.cpp,v $
  Revision 1.11  2002/11/25 17:12:24  rainy
  Now uses the AddPath method

  Revision 1.10  2002/11/12 18:01:41  rainy
  Modified to use the CImage class.

  Revision 1.9  2002/09/07 09:35:30  rainy
  Added Solid backgrounds.
  Fixed a bug that copied the background from wrong place.

  Revision 1.8  2002/08/24 11:14:44  rainy
  Changed the error handling.

  Revision 1.7  2002/08/03 16:22:37  rainy
  Changed the background handling to cache the wallpaper when
  the window is moved.

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

HBITMAP CBackground::c_Wallpaper = NULL;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBackground::CBackground()
{
	m_Width = 0;
	m_Height = 0;
	m_Alpha = false;
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
	m_Width = 0;
	m_Height = 0;
	m_Alpha = false;
}

/* 
** Load
**
** Loads the given image and puts it in the m_BGImage.
**
*/
void CBackground::Load(const std::string& filename)
{
	std::string name = filename;
	CConfig::AddPath(name);
	if (!m_BGImage.Load(name.c_str()))
	{
		std::string err = "Unable to load file: ";
		err += name;
		THROW(err);
	}
}

/* 
** Create
**
** Creates the background image from the desktop and loaded image.
** Width and height are from current window size.
** Window's size must be correct when calling this.
** Returns true, if the background image is larger than the Width and Height.
*/
bool CBackground::Create(int X, int Y, int Width, int Height)
{
	HWND Desktop;
	HDC DDC;
	HDC BGDC;
	HBITMAP OldBitmap;
	bool resize = false;

	m_Image.Clear();
	m_BGImage.Clear();

	m_Width = Width;
	m_Height = Height;

	// If the background is set as solid, we'll create a solid bitmap
	if (CCalendarWindow::c_Config.GetBackgroundMode() == CBackground::MODE_SOLID)
	{
		Desktop = GetDesktopWindow();
		if(Desktop == NULL) THROW(ERR_BACKGROUND);

		DDC = GetDC(Desktop);
		if(DDC == NULL) THROW(ERR_BACKGROUND);
			
		BGDC = CreateCompatibleDC(DDC);
		if(BGDC == NULL) THROW(ERR_BACKGROUND);

		// Create background from solid color
		HBITMAP background = CreateCompatibleBitmap(DDC, Width, Height);
		if(background == NULL) THROW(ERR_BACKGROUND);
		OldBitmap = (HBITMAP)SelectObject(BGDC, background);

		ReleaseDC(Desktop, DDC);
		
		RECT r = { 0, 0, Width, Height };
		HBRUSH brush = CreateSolidBrush(CCalendarWindow::c_Config.GetBackgroundSolidColor());
		FillRect(BGDC, &r, brush);
		DeleteObject(brush);

		if(CCalendarWindow::c_Config.GetBackgroundBevel())
		{
			// Draw bevel
			DrawEdge(BGDC, &r, EDGE_RAISED, BF_RECT);
		}

		SelectObject(BGDC, OldBitmap);
		DeleteDC(BGDC);

		m_BGImage.Create(background, NULL, 0x0ff);

		m_Alpha = false;
	}
	else if (!CCalendarWindow::c_Config.GetBackgroundBitmapName().empty() && CCalendarWindow::c_Config.GetBackgroundMode() != CBackground::MODE_COPY)
	{
		// Otherwise we'll load the background image and grab a piece of wallpaper
		Load(CCalendarWindow::c_Config.GetBackgroundBitmapName());

		if (m_BGImage.GetWidth() < Width || m_BGImage.GetHeight() < Height)
		{
			m_Width = max(Width, m_BGImage.GetWidth());
			m_Height = max(Height, m_BGImage.GetHeight());

			// We need to resize the image to match the desired windowsize
			if(CCalendarWindow::c_Config.GetBackgroundMode() == MODE_TILE)
			{
				m_BGImage.Resize(m_Width, m_Height, IMAGE_RESIZE_TYPE_TILE);
			}
			else 
			{
				m_BGImage.Resize(m_Width, m_Height, IMAGE_RESIZE_TYPE_STRETCH);
			}
		}

		if (m_BGImage.GetWidth() > Width || m_BGImage.GetHeight() > Height)
		{
			m_Width = max(Width, m_BGImage.GetWidth());
			m_Height = max(Height, m_BGImage.GetHeight());
			resize = true;
		}
		m_Alpha = m_BGImage.HasAlpha();
	}

	if (m_Alpha || CCalendarWindow::c_Config.GetBackgroundMode() == CBackground::MODE_COPY)
	{
		// Take a copy of the BG
		UpdateWallpaper(X, Y);
	}

	return resize;
}

/* 
** CopyBackground
**
** Takes a copy of the desktop to use as background
*/
HBITMAP CBackground::CopyBackground(int X, int Y, int Width, int Height) 
{
	HDC tmpDC;
	HBITMAP OldBitmap;
	HBITMAP wallpaper = NULL;
	HDC WinDC;

	if(GetRainlendar() && (CCalendarWindow::c_Config.GetBGCopyMode() == CConfig::BG_WALLPAPER_ALWAYS ||
						  (CCalendarWindow::c_Config.GetBGCopyMode() == CConfig::BG_NORMAL && !(GetRainlendar()->IsWharf()))))
	{
		// The new way
		wallpaper = GetWallpaper(X, Y, Width, Height);
	}

	if(wallpaper == NULL)
	{
		// If the new way fails, use the old way
		WinDC = GetWindowDC(GetDesktopWindow());
		if(WinDC == NULL) THROW(ERR_BACKGROUND);

		tmpDC = CreateCompatibleDC(WinDC);
		if(tmpDC==NULL) THROW(ERR_BACKGROUND);

		wallpaper = CreateCompatibleBitmap(WinDC, Width, Height);
		if(wallpaper==NULL) THROW(ERR_BACKGROUND);

		// Fetch the background
		OldBitmap = (HBITMAP)SelectObject(tmpDC, wallpaper);
		BitBlt(tmpDC, 0, 0, Width, Height, WinDC, CCalendarWindow::c_Config.GetX(), CCalendarWindow::c_Config.GetY(), SRCCOPY);
		
		ReleaseDC(GetDesktopWindow(), WinDC);

		SelectObject(tmpDC, OldBitmap);
		DeleteDC(tmpDC);
	}

	return wallpaper;
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
	HBITMAP bgBitmap = NULL;
    HKEY hKey = NULL;
	
	// Get the wallpaper name and options from registry
	if(RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
	{
		if (c_Wallpaper == NULL)
		{
			if(RegQueryValueEx(hKey, "Wallpaper", NULL, NULL, (LPBYTE)&str, (LPDWORD)&size) == ERROR_SUCCESS)
			{
				if(strlen(str) > 0)
				{
					c_Wallpaper = LoadLSImage(str, NULL);
				}
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
	if(winDC == NULL) THROW(ERR_BACKGROUND);

	HDC tmpDC = CreateCompatibleDC(winDC);
	if(tmpDC == NULL) THROW(ERR_BACKGROUND);

	bgBitmap = CreateCompatibleBitmap(winDC, Width, Height);
	if(bgBitmap==NULL) THROW(ERR_BACKGROUND);

	ReleaseDC(GetDesktopWindow(), winDC);

	HBITMAP oldBitmap = (HBITMAP)SelectObject(tmpDC, bgBitmap);
	
	// Fill the bitmap with bgcolor
	HBRUSH brush = CreateSolidBrush(bgColor);
	RECT r = { 0, 0, Width, Height };
	FillRect(tmpDC, &r, brush);
	DeleteObject(brush);

	if(c_Wallpaper)
	{
		HDC bgDC = CreateCompatibleDC(tmpDC);
		if(bgDC == NULL) THROW(ERR_BACKGROUND);
		HBITMAP oldBitmap2 = (HBITMAP)SelectObject(bgDC, c_Wallpaper);

		// Get the size of the loaded wallpaper
		BITMAP bm;
		GetObject(c_Wallpaper, sizeof(BITMAP), &bm);

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
	}

	SelectObject(tmpDC, oldBitmap);
	DeleteDC(tmpDC);

	return bgBitmap;
}

/* 
** Paint
**
** Paints the background to the give DC
**
*/
void CBackground::Paint(CImage& background)
{
	if (m_Image.GetBitmap() != NULL)
	{
		background.Blit(m_Image, 0, 0, 0, 0, m_Width, m_Height);
	}
	else if (m_BGImage.GetBitmap() != NULL)
	{
		background.Blit(m_BGImage, 0, 0, 0, 0, m_Width, m_Height);
	}
}

/* 
** FlushWallpaper
**
** Removes the wallpaper from memory
**
*/
void CBackground::FlushWallpaper()
{
	if (c_Wallpaper)
	{
		DeleteObject(c_Wallpaper);
		c_Wallpaper = NULL;
	}
}

/* 
** UpdateWallpaper
**
** Updates the wallpaper from the new location and recreates the background image
**
*/
void CBackground::UpdateWallpaper(int X, int Y)
{
	if (!CCalendarWindow::Is2k() || !CCalendarWindow::c_Config.GetNativeTransparency())	// No need to mess with wallpaper if we're using real transparency
	{
		// Take a copy of the BG
		HBITMAP wallpaper = CopyBackground(X, Y, m_Width, m_Height);
		m_Image.Create(wallpaper, NULL, 0x0ff);

		if (m_BGImage.GetBitmap() != NULL)
		{
			m_Image.Blit(m_BGImage, 0, 0, 0, 0, m_Width, m_Height);
		}
	}
}

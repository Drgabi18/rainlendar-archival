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
  $Header: /home/cvsroot/Rainlendar/Library/Background.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: Background.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.20  2005/03/01 18:44:33  rainy
  Added loadimage to Initialize

  Revision 1.19  2004/12/05 18:34:54  rainy
  Changed wallpaper polling.

  Revision 1.18  2004/11/06 13:38:58  rainy
  no message

  Revision 1.17  2004/01/28 18:04:10  rainy
  no message

  Revision 1.16  2003/10/27 17:36:11  Rainy
  Settings are not read directly from the config anymore.

  Revision 1.15  2003/08/09 16:38:55  Rainy
  Added a check if the file exists.

  Revision 1.14  2003/06/15 20:05:11  Rainy
  The correct width and height is set if there is background.

  Revision 1.13  2003/06/15 09:42:41  Rainy
  Added support for multiple calendars.

  Revision 1.12  2003/05/07 19:18:29  rainy
  The member variables are initialized correctly.

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
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "Background.h"
#include "CalendarWindow.h"
#include "Error.h"
#include "Image.h"
#include "RasterizerBitmap.h"

CImage CBackground::c_Wallpaper;
std::string CBackground::c_WallpaperName;
FILETIME CBackground::c_WallpaperTime;

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
	Initialize(NULL);	// This gets rid of the bitmaps
}

/* 
** Initialize
**
** Intializes the background
**
*/
void CBackground::Initialize(LPCTSTR filename, bool loadImage)
{
    m_Filename = "";
	m_Width = 0;
	m_Height = 0;
	m_Alpha = false;
	m_Image.Clear();
	m_BGImage.Clear();

	if (filename && strlen(filename) > 0 && loadImage) 
	{
		Load(filename);
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
bool CBackground::Create(BackgroundCreateStruct& bcs)
{
	HWND Desktop;
	HDC DDC;
	HDC BGDC;
	HBITMAP OldBitmap;
	bool resize = false;

	m_Image.Clear();
	m_Wallpaper.Clear();

	m_Width = bcs.size.cx;
	m_Height = bcs.size.cy;

	// If the background is set as solid, we'll create a solid bitmap
	if (bcs.mode == MODE_SOLID)
	{
		Desktop = GetDesktopWindow();
		if(Desktop == NULL) THROW(ERR_BACKGROUND);

		DDC = GetDC(Desktop);
		if(DDC == NULL) THROW(ERR_BACKGROUND);
			
		BGDC = CreateCompatibleDC(DDC);
		if(BGDC == NULL) THROW(ERR_BACKGROUND);

		// Create background from solid color
		HBITMAP background = CreateCompatibleBitmap(DDC, m_Width, m_Height);
		if(background == NULL) THROW(ERR_BACKGROUND);
		OldBitmap = (HBITMAP)SelectObject(BGDC, background);

		ReleaseDC(Desktop, DDC);
		
		RECT r = { 0, 0, m_Width, m_Height };
		HBRUSH brush = CreateSolidBrush(bcs.solidColor);
		FillRect(BGDC, &r, brush);
		DeleteObject(brush);

		if (bcs.solidBevel)
		{
			// Draw bevel
			DrawEdge(BGDC, &r, EDGE_RAISED, BF_RECT);
		}

		SelectObject(BGDC, OldBitmap);
		DeleteDC(BGDC);

		m_Image.Create(background, NULL, 0x0ff);

		m_Alpha = false;
	}
	else if (m_BGImage.GetBitmap() != NULL && bcs.mode != MODE_COPY)
	{
		// Otherwise we'll load the background image and grab a piece of wallpaper

		if (m_BGImage.GetWidth() < m_Width || m_BGImage.GetHeight() < m_Height)
		{
			m_Image.Create(m_BGImage.GetWidth(), m_BGImage.GetHeight(), 0);
			m_Image.Blit(m_BGImage, 0, 0, 0, 0, m_BGImage.GetWidth(), m_BGImage.GetHeight());

			// We need to resize the image to match the desired windowsize
			switch (bcs.mode) 
			{
			case MODE_TILE:
				// If we're tiling, we use the bg-image as one calendar
				m_Width = max(m_Width, m_BGImage.GetWidth() * (int)CConfig::Instance().GetHorizontalCount());
				m_Height = max(m_Height, m_BGImage.GetHeight() * (int)CConfig::Instance().GetVerticalCount());
				m_Image.Resize(m_Width, m_Height, IMAGE_RESIZE_TYPE_TILE, NULL);
				break;

			case MODE_STRETCH:
				m_Width = max(m_Width, m_BGImage.GetWidth());
				m_Height = max(m_Height, m_BGImage.GetHeight());
				m_Image.Resize(m_Width, m_Height, IMAGE_RESIZE_TYPE_STRETCH, NULL);
				break;

			case MODE_RECT:
				m_Width = max(m_Width, m_BGImage.GetWidth());
				m_Height = max(m_Height, m_BGImage.GetHeight());
				m_Image.Resize(m_Width, m_Height, IMAGE_RESIZE_TYPE_STRETCH, &bcs.resizeRect);
				break;
			}
		}
		else
		{
			m_Width = max(m_Width, m_BGImage.GetWidth());
			m_Height = max(m_Height, m_BGImage.GetHeight());
		}
		resize = true;

		m_Alpha = m_BGImage.HasAlpha();
	}

	if (m_Alpha || bcs.mode == MODE_COPY)
	{
		// Take a copy of the BG
		UpdateWallpaper(bcs.pos.x, bcs.pos.y);
	}

	return resize;
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
		char tmpSz[MAX_LINE_LENGTH];
		sprintf(tmpSz, CCalendarWindow::c_Language.GetString("Error", 8), filename.c_str());
		MessageBox(NULL, tmpSz, "Rainlendar", MB_OK | MB_ICONERROR);
	}
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

	// The new way
	wallpaper = GetWallpaper(X, Y, Width, Height);

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
		BitBlt(tmpDC, 0, 0, Width, Height, WinDC, X, Y, SRCCOPY);
		
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
	const char* str = NULL;
	int tile, style, size;
	COLORREF bgColor = 0;
	HBITMAP bgBitmap = NULL;
    HKEY hKey = NULL;
	char buffer[256];
	
	// Get the wallpaper name and options from registry
	str = GetWallpaperFile(&tile, &style);
	if (str && strlen(str) > 0 && c_Wallpaper.GetBitmap() == NULL)
	{
		c_Wallpaper.Load(str);
	}

	// Also get the background color
	size = 256;
    if(RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Colors", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
	{
		if(RegQueryValueEx(hKey, "Background", NULL, NULL, (LPBYTE)&buffer, (LPDWORD)&size) == ERROR_SUCCESS)
		{
			int R, G, B;
			sscanf(buffer, "%i %i %i", &R, &G, &B);
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

	if(c_Wallpaper.GetBitmap())
	{
		HDC bgDC = CreateCompatibleDC(tmpDC);
		if(bgDC == NULL) THROW(ERR_BACKGROUND);
		HBITMAP oldBitmap2 = (HBITMAP)SelectObject(bgDC, c_Wallpaper.GetBitmap());

		// Get the size of the loaded wallpaper
		BITMAP bm;
		GetObject(c_Wallpaper.GetBitmap(), sizeof(BITMAP), &bm);

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
					if(tile == 1)
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
					else if (style == 2)
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

		if (style == 2)
		{
			SetStretchBltMode(tmpDC, HALFTONE);
			SetBrushOrgEx(tmpDC, 0, 0, NULL);

			StretchBlt(tmpDC, -X, -Y, screenRect.right - screenRect.left, screenRect.bottom - screenRect.top,
					   bgDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		}
		else if(tile == 1)
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
	if (m_Wallpaper.GetBitmap() != NULL)
	{
		background.Blit(m_Wallpaper, 0, 0, 0, 0, m_Width, m_Height);
	}

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
	c_Wallpaper.Clear();
}

/* 
** UpdateWallpaper
**
** Updates the wallpaper from the new location and recreates the background image
**
*/
void CBackground::UpdateWallpaper(int X, int Y)
{
	if (!CCalendarWindow::Is2k() || !CConfig::Instance().GetNativeTransparency())	// No need to mess with wallpaper if we're using real transparency
	{
		// Take a copy of the BG
		HBITMAP wallpaper = CopyBackground(X, Y, m_Width, m_Height);
		m_Wallpaper.Create(wallpaper, NULL, 0x0ff);
		DeleteObject(wallpaper);
	}
}

/* 
** PollWallpaper
**
** Checks if the name of the wallpaper has been changed and refreshes if it has
**
*/
bool CBackground::PollWallpaper(bool set)
{
	if (CConfig::Instance().GetPollWallpaper())
	{
		// Check if the wallpaper is different that what it was when we refresed last time
		const char* str = GetWallpaperFile(NULL, NULL);

		if (set)
		{
			c_WallpaperName = str;

			// Get the timestamp from the file too
			HANDLE file = CreateFile(c_WallpaperName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (file != INVALID_HANDLE_VALUE)
			{
				GetFileTime(file, NULL, NULL, &c_WallpaperTime);
				CloseHandle(file);
			}
		}
		else
		{
			if (c_WallpaperName != str)
			{
				// If the names differ, refresh
				c_WallpaperName = str;
				return true;
			}
			else
			{
				// Check the timestamp
				FILETIME newTime;
				HANDLE file = CreateFile(c_WallpaperName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (file != INVALID_HANDLE_VALUE)
				{
					GetFileTime(file, NULL, NULL, &newTime);
					CloseHandle(file);

					if (newTime.dwHighDateTime != c_WallpaperTime.dwHighDateTime || newTime.dwLowDateTime != c_WallpaperTime.dwLowDateTime)
					{
						c_WallpaperTime.dwHighDateTime = newTime.dwHighDateTime;
						c_WallpaperTime.dwLowDateTime = newTime.dwLowDateTime;

						return true;
					}
				}
			}
		}
	}
	return false;
}

const char* CBackground::GetWallpaperFile(int* tile, int* style)
{
	DWORD size = MAX_PATH;
	char buffer[MAX_PATH];
	static char str[MAX_PATH];
	HKEY hKey = NULL;

	// Get the wallpaper name from registry
	if(!RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Internet Explorer\\Desktop\\General", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
	{
		if(!RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
		{
			return NULL;
		}
	}

	if (hKey)
	{
		RegQueryValueEx(hKey, "Wallpaper", NULL, NULL, (LPBYTE)&buffer, (LPDWORD)&size);
		ExpandEnvironmentStrings(buffer, str, MAX_PATH);

		if (tile)
		{
			size = MAX_PATH;
			if(RegQueryValueEx(hKey, "TileWallpaper", NULL, NULL, (LPBYTE)&buffer, (LPDWORD)&size) == ERROR_SUCCESS)
			{
				*tile = atoi(buffer);
			}
		}

		if (style)
		{
			size = MAX_PATH;
			if(RegQueryValueEx(hKey, "WallpaperStyle", NULL, NULL, (LPBYTE)&buffer, (LPDWORD)&size) == ERROR_SUCCESS)
			{
				*style = atoi(buffer);
			}
		}

		RegCloseKey(hKey);
	}

	return str;
}
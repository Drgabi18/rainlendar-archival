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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Background.cpp,v 1.3 2002/01/15 17:59:44 rainy Exp $

  $Log: Background.cpp,v $
  Revision 1.3  2002/01/15 17:59:44  rainy
  Now uses different way to get the desktop image.

  Revision 1.2  2001/12/23 10:02:31  rainy
  The background is grapped only once if ran as wharf module.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "Background.h"
#include "CalendarWindow.h"
#include "Error.h"
#include "RasterizerBitmap.h"
#include <assert.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBackground::CBackground()
{
	m_Width = 0;
	m_Height = 0;
	m_CalendarWindow = NULL;
}

CBackground::~CBackground()
{

}

/* 
** Initialize
**
** Intializes the background
**
*/
void CBackground::Initialize()
{
	m_Width=0;
	m_Height=0;
}

/* 
** Load
**
** Loads the given image.
**
*/
void CBackground::Load(CString& Filename)
{
	HBITMAP BM;
	HBITMAP ABM;
	BITMAP bm;
	CString PicName;
	int End, Len;

	if(-1==Filename.Find(':', 0)) {
		PicName.Format("%s%s", CCalendarWindow::c_Config.GetPath(), Filename);
	} else {
		PicName=Filename;
	}
	BM=LoadLSImage(PicName, NULL);

	if(BM==NULL) throw Filename;		// File not found

	m_Image.Attach(BM);

	GetObject(BM, sizeof(BITMAP), &bm);
	m_Width=bm.bmWidth;
	m_Height=bm.bmHeight;

	// Check for Alpha-map
	m_Alpha=false;
	End=PicName.ReverseFind('.');
	Len=PicName.GetLength();
	if(End!=-1) {
		CString AlphaName;
		AlphaName.Format("%s-Alpha%s", PicName.Left(Len-(Len-End)), Filename.Right(Len-End));
		ABM=LoadLSImage(AlphaName, NULL);
		if(ABM!=NULL) {
			m_Alpha=true;		// Alphamap found!
			m_AlphaImage.Attach(ABM);

			// Check that it is correct size
			GetObject(ABM, sizeof(BITMAP), &bm);
			if(m_Width!=bm.bmWidth || m_Height!=bm.bmHeight) throw ERR_BACKGROUNDALPHASIZE;
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
void CBackground::Create(int Width, int Height)
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

	Desktop=GetDesktopWindow();
	if(Desktop==NULL) throw ERR_BACKGROUND;

	DDC=GetDC(Desktop);
	if(DDC==NULL) throw ERR_BACKGROUND;
		
	tmpDC=CreateCompatibleDC(DDC);
	if(tmpDC==NULL) throw ERR_BACKGROUND;

	BGDC=CreateCompatibleDC(DDC);
	if(BGDC==NULL) throw ERR_BACKGROUND;

	ReleaseDC(Desktop, DDC);

	RealWidth=max(Width, m_Width);
	RealHeight=max(Height, m_Height);

	if(m_Width<Width || m_Height<Height) {
		// Must tile the background and possible alphamap

		OldBitmap2=(HBITMAP)SelectObject(BGDC, m_Image);

		tmpBitmap=CreateCompatibleBitmap(BGDC, RealWidth, RealHeight);
		if(tmpBitmap==NULL) throw ERR_BACKGROUND;

		if(m_Alpha) {
			tmpBitmapA=CreateCompatibleBitmap(BGDC, RealWidth, RealHeight);
			if(tmpBitmapA==NULL) throw ERR_BACKGROUND;
		}

		OldBitmap=(HBITMAP)SelectObject(tmpDC, tmpBitmap);

		// Tile the background
		for(j=0; j<RealHeight; j+=m_Height) {
			for(i=0; i<RealWidth; i+=m_Width) {
				BitBlt(tmpDC, i, j, min(m_Width, RealWidth-i), min(m_Height, RealHeight-j), BGDC, 0, 0, SRCCOPY);
			}
		}

		// Alpha must be tiled too
		if(m_Alpha) {
			SelectObject(tmpDC, tmpBitmapA);
			SelectObject(BGDC, m_AlphaImage);
			for(j=0; j<RealHeight; j+=m_Height) {
				for(i=0; i<RealWidth; i+=m_Width) {
					BitBlt(tmpDC, i, j, min(m_Width, RealWidth-i), min(m_Height, RealHeight-j), BGDC, 0, 0, SRCCOPY);
				}
			}
		}

		SelectObject(tmpDC, OldBitmap);
		SelectObject(BGDC, OldBitmap2);

		m_Image.DeleteObject();
		m_Image.Attach(tmpBitmap);

		if(m_Alpha) {
			m_AlphaImage.DeleteObject();
			m_AlphaImage.Attach(tmpBitmapA);
		}

		m_Width=RealWidth;
		m_Height=RealHeight;
	}

	if(m_Width==0 || m_Height==0) return;
	
	// Take a copy of the BG
	CopyBackground(m_Width, m_Height);

	if(m_Alpha) {
		// ..and create and alphamasked version of the background bitmap
		CRasterizerBitmap::CreateAlpha(m_Image, m_AlphaImage, m_Background);
		m_AlphaImage.DeleteObject();	// We won't need this one anymore
		m_Background.DeleteObject();	// Delete the old (if there was one)
		m_Background.Attach(m_Image.Detach());
	} else {
		// Transparentblit for the magic pink
		OldBitmap=(HBITMAP)SelectObject(tmpDC, m_Image);
		OldBitmap2=(HBITMAP)SelectObject(BGDC, m_Background);
		TransparentBltLS(BGDC, 0, 0, m_Width, m_Height, tmpDC, 0, 0, RGB(255,0,255));
		SelectObject(tmpDC, OldBitmap);
		SelectObject(BGDC, OldBitmap2);
		m_Image.DeleteObject();	// No need for this
	}

	assert(m_Image.Detach()==NULL);
	assert(m_AlphaImage.Detach()==NULL);

	DeleteDC(BGDC);
	DeleteDC(tmpDC);
}

/* 
** CopyBackground
**
** Takes a copy of the desktop to use as background
** The copy goes to m_Background.
*/
void CBackground::CopyBackground(int Width, int Height) 
{
	HDC tmpDC;
	HBITMAP OldBitmap;
	HBITMAP tmpBitmap = NULL;
	CDC* WinDC;

	if(m_CalendarWindow==NULL) throw ERR_BACKGROUND;

	if(CCalendarWindow::GetWharfData() == NULL)
	{
		// The new way
		tmpBitmap = GetWallpaper();
	}

	if(tmpBitmap == NULL)
	{
		// If the new way fails, use the old way

		WinDC=m_CalendarWindow->GetWindowDC();
		if(WinDC==NULL) throw ERR_BACKGROUND;

		if(CCalendarWindow::GetWharfData() == NULL)
		{
			// Paint the desktop, so we'll get correct background
			PaintDesktop(*WinDC);
		}

		// These are initialized only if grapping is successful
		m_Width=0;
		m_Height=0;

		tmpDC=CreateCompatibleDC(*WinDC);
		if(tmpDC==NULL) throw ERR_BACKGROUND;

		m_Background.DeleteObject();	// Delete the old (if there was one)
		tmpBitmap=CreateCompatibleBitmap(*WinDC, Width, Height);
		if(tmpBitmap==NULL) throw ERR_BACKGROUND;

		m_Background.Attach(tmpBitmap);

		// Fetch the background
		OldBitmap=(HBITMAP)SelectObject(tmpDC, m_Background);
		BitBlt(tmpDC, 0, 0, Width, Height, *WinDC, 0, 0, SRCCOPY);
		
		m_CalendarWindow->ReleaseDC(WinDC);

		SelectObject(tmpDC, OldBitmap);
		DeleteDC(tmpDC);
	}
	else
	{
		m_Background.DeleteObject();	// Delete the old (if there was one)
		m_Background.Attach(tmpBitmap);
	}

	m_Width=Width;
	m_Height=Height;
}

/*
** GetWallpaper
**
** Loads the background image and cuts a given part from it.
**
*/
HBITMAP CBackground::GetWallpaper()
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

	// Get the window's pos and size
	RECT windowRect, screenRect;
	m_CalendarWindow->GetWindowRect(&windowRect);

	// Get the screen size
	GetClientRect(GetDesktopWindow(), &screenRect); 

	HDC winDC = GetWindowDC(*m_CalendarWindow);
	if(winDC == NULL) throw ERR_BACKGROUND;

	HDC tmpDC = CreateCompatibleDC(winDC);
	if(tmpDC == NULL) throw ERR_BACKGROUND;

	bgBitmap = CreateCompatibleBitmap(winDC, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
	if(bgBitmap==NULL) throw ERR_BACKGROUND;

	ReleaseDC(*m_CalendarWindow, winDC);

	HBITMAP oldBitmap = (HBITMAP)SelectObject(tmpDC, bgBitmap);
	
	// Fill the bitmap with bgcolor
	HBRUSH brush = CreateSolidBrush(bgColor);
	RECT r = { 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top };
	FillRect(tmpDC, &r, brush);
	DeleteObject(brush);

	if(bitmap)
	{
		HDC bgDC = CreateCompatibleDC(tmpDC);
		if(bgDC == NULL) throw ERR_BACKGROUND;
		HBITMAP oldBitmap2 = (HBITMAP)SelectObject(bgDC, bitmap);

		// Get the size of the loaded wallpaper
		BITMAP bm;
		GetObject(bitmap, sizeof(BITMAP), &bm);

		// If the window is off the primary monitor, we'll have to modify the values a bit
		POINT point = {windowRect.left, windowRect.top };
		HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTOPRIMARY);
		if(monitor)
		{
			MONITORINFO monInfo;
			monInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(monitor, &monInfo);
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
				else
				{
					windowRect.left -= monInfo.rcMonitor.left;
					windowRect.right -= monInfo.rcMonitor.left;
					windowRect.top -= monInfo.rcMonitor.top;
					windowRect.bottom -= monInfo.rcMonitor.top;
					screenRect.right = (monInfo.rcMonitor.right - monInfo.rcMonitor.left);
					screenRect.bottom = (monInfo.rcMonitor.bottom - monInfo.rcMonitor.top);
				}
			}
		}

		if(stretch)
		{
			StretchBlt(tmpDC, -windowRect.left, -windowRect.top, screenRect.right - screenRect.left, screenRect.bottom - screenRect.top,
					   bgDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		}
		else if(tile)
		{
			// We should probably calculate the proper starting point, but why bother
			for(int i = screenRect.left; i < screenRect.right; i += bm.bmWidth)
			{
				for(int j = screenRect.top; j < screenRect.bottom; j += bm.bmHeight)
				{
					BitBlt(tmpDC, i - windowRect.left, j - windowRect.top, bm.bmWidth, bm.bmHeight,
						   bgDC, 0, 0, SRCCOPY);
				}
			}
		}
		else // Center
		{
			int imageLeft = (screenRect.right - screenRect.left) / 2 - bm.bmWidth / 2;
			int imageTop = (screenRect.bottom - screenRect.top) / 2 - bm.bmHeight / 2;
			BitBlt(tmpDC, imageLeft - windowRect.left, imageTop - windowRect.top, bm.bmWidth, bm.bmHeight,
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
** TODO: Blit only the invalidated area
*/
void CBackground::Paint(CDC& dc)
{
	HBITMAP OldBitmap;
	HDC tmpDC;
	HDC DDC=NULL;
	HWND Desktop;

	Desktop=GetDesktopWindow();
	if(Desktop!=NULL) DDC=GetDC(Desktop);

	if(DDC!=NULL) {

		tmpDC=CreateCompatibleDC(DDC);
		ReleaseDC(Desktop, DDC);

		OldBitmap=(HBITMAP)SelectObject(tmpDC, m_Background);
	
		BitBlt(dc, 0, 0, m_Width, m_Height, tmpDC, 0, 0, SRCCOPY);

		SelectObject(tmpDC, OldBitmap);

		DeleteDC(tmpDC);
	}
}

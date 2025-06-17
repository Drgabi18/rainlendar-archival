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

// Background.cpp: implementation of the CBackground class.
//
//////////////////////////////////////////////////////////////////////

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
	m_Width=0;
	m_Height=0;
	m_CalendarWindow=NULL;
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
		PicName.Format("%s%s", CCalendarWindow::C_Config.GetPath(), Filename);
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
	HBITMAP tmpBitmap;
	CDC* WinDC;

	if(m_CalendarWindow==NULL) throw ERR_BACKGROUND;
	
	// Paint the desktop, so we'll get correct background
	WinDC=m_CalendarWindow->GetWindowDC();
	if(WinDC==NULL) throw ERR_BACKGROUND;
	PaintDesktop(*WinDC);

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

	m_Width=Width;
	m_Height=Height;
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

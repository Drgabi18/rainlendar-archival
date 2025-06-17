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

// RasterizerBitmap.cpp: implementation of the CRasterizerBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "RasterizerBitmap.h"
#include "CalendarWindow.h"
#include "Error.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRasterizerBitmap::CRasterizerBitmap()
{
	m_NumOfComponents=0;
	m_Alpha=false;
}

CRasterizerBitmap::~CRasterizerBitmap()
{
}

/* 
** Load
**
** Loads the given image. 
**
*/
void CRasterizerBitmap::Load(CString& Filename)
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

			// Check that it is correct size
			GetObject(ABM, sizeof(BITMAP), &bm);
			if(m_Width!=bm.bmWidth || m_Height!=bm.bmHeight) throw ERR_ALPHASIZE;

			m_AlphaBitmap.DeleteObject();	// Delete the old (if there was one)
			m_AlphaBitmap.Attach(ABM);
		}
	}

	m_Bitmap.DeleteObject();	// Delete the old (if there was one)
	m_Bitmap.Attach(BM);
}

/* 
** Paint
**
** Paints the given part of the image. W & H are for align.
**
*/
void CRasterizerBitmap::Paint(CDC& dc, int X, int Y, int W, int H, int Index)
{
	HBITMAP OldBitmap=NULL;
	HDC tmpDC=NULL;
	int Number, NumOfNums, ItemWidth, ItemHeight;

	if(m_Height>m_Width) {
		// The items are stacked from top to bottom
		ItemWidth=m_Width;						
		ItemHeight=m_Height/m_NumOfComponents;
	} else {
		// The items are stacked from left to right
		ItemWidth=m_Width/m_NumOfComponents;	
		ItemHeight=m_Height;
	}

	// We'll blit the numbers in reverse order, so lets find the number of the numbers ;-)
	Number=Index;
	NumOfNums=1;	// At least one number
	if(m_NumOfComponents>1) {
		while(Number>=m_NumOfComponents) {
			Number=Number/m_NumOfComponents;
			NumOfNums++;
		}
	}

	// Align the bitmap correctly
	if(m_Align==ALIGN_LEFT) {
		X=X+NumOfNums*ItemWidth;
	} else if(m_Align==ALIGN_CENTER) {
		X=X+(W-NumOfNums*ItemWidth)/2+NumOfNums*ItemWidth;
	} else {	// RIGHT
		X=X+W;
	}

	// Center the numbers horizontally
	Y=Y+(H-ItemHeight)/2;

	if(m_Alpha) {
		PaintAlpha(dc, X, Y, NumOfNums, Index);
	} else {
		tmpDC=CreateCompatibleDC(dc);
		if(tmpDC==NULL) return;
		OldBitmap=(HBITMAP)SelectObject(tmpDC, m_Bitmap);

		// Blit all necessary numbers
		do {
			Number=Index%m_NumOfComponents;
			Index=Index/m_NumOfComponents;
			X-=ItemWidth;

			if(m_Height>m_Width) {
				TransparentBltLS(dc, X, Y, ItemWidth, ItemHeight, tmpDC, 0, Number*ItemHeight, RGB(255,0,255));
			} else {
				TransparentBltLS(dc, X, Y, ItemWidth, ItemHeight, tmpDC, Number*ItemWidth, 0, RGB(255,0,255));
			}
		} while(Index>0 && m_NumOfComponents>1);

		SelectObject(tmpDC, OldBitmap);
		DeleteDC(tmpDC);
	}

}

/* 
** PaintAlpha
**
** Paints the alphamapped bitmaps
**
*/
void CRasterizerBitmap::PaintAlpha(CDC& dc, int X, int Y, int NumOfNums, int Index)
{
	CBitmap* BackgroundBM=NULL;
	CBitmap* SourceBM=NULL;
	CBitmap* AlphaBM=NULL;
	HBITMAP OldBitmap=NULL;
	HBITMAP OldBitmap2=NULL;
	CDC tmpDC;
	CDC tmpDC2;
	int FullWidth;	
	int Number;
	int tmpX, tmpIndex;
	int ItemWidth, ItemHeight;

	if(m_Height>m_Width) {
		// The items are stacked from top to bottom
		ItemWidth=m_Width;						
		ItemHeight=m_Height/m_NumOfComponents;
	} else {
		// The items are stacked from left to right
		ItemWidth=m_Width/m_NumOfComponents;	
		ItemHeight=m_Height;
	}


	FullWidth=NumOfNums*ItemWidth;
	// Calculate the correct X
	X=X-FullWidth;

	if(!tmpDC.CreateCompatibleDC(&dc)) return;		// Out of mem
	OldBitmap=(HBITMAP)SelectObject(tmpDC, m_Bitmap);

	// Draw the alphablended bitmap

	BackgroundBM=new CBitmap;
	SourceBM=new CBitmap;
	AlphaBM=new CBitmap;

	if(BackgroundBM==NULL || SourceBM==NULL || AlphaBM==NULL) return;		// Out of mem

	if(!BackgroundBM->CreateCompatibleBitmap(&tmpDC, FullWidth, ItemHeight)) return;		// Out of mem
	if(!SourceBM->CreateCompatibleBitmap(&tmpDC, FullWidth, ItemHeight)) return;		// Out of mem
	if(!AlphaBM->CreateCompatibleBitmap(&tmpDC, FullWidth, ItemHeight)) return;		// Out of mem

	if(!tmpDC2.CreateCompatibleDC(&dc)) return;		// Out of mem
	OldBitmap2=(HBITMAP)SelectObject(tmpDC2, *SourceBM);


	// Hmm... probably should free allocated memory if it ran out. But in that
	// situation everything is fucked up anyway, so who cares.


	// If we have real alpha, we must fetch the background first
	GetBackground(dc, *BackgroundBM, X, Y, FullWidth, ItemHeight);

	// Then we must get the correct component from the bitmap and alphamap

	tmpX=FullWidth;
	tmpIndex=Index;
	do {
		Number=tmpIndex%m_NumOfComponents;
		tmpIndex=tmpIndex/m_NumOfComponents;
		tmpX-=ItemWidth;

		// Fetch the normal component
		SelectObject(tmpDC, m_Bitmap);
		SelectObject(tmpDC2, *SourceBM);

		if(m_Height>m_Width) {
			BitBlt(tmpDC2, tmpX, 0, ItemWidth, ItemHeight, tmpDC, 0, Number*ItemHeight, SRCCOPY);
		} else {
			BitBlt(tmpDC2, tmpX, 0, ItemWidth, ItemHeight, tmpDC, Number*ItemWidth, 0, SRCCOPY);
		}
			
		// Fetch the alpha component
		SelectObject(tmpDC, m_AlphaBitmap);
		SelectObject(tmpDC2, *AlphaBM);

		if(m_Height>m_Width) {
			BitBlt(tmpDC2, tmpX, 0, ItemWidth, ItemHeight, tmpDC, 0, Number*ItemHeight, SRCCOPY);
		} else {
			BitBlt(tmpDC2, tmpX, 0, ItemWidth, ItemHeight, tmpDC, Number*ItemWidth, 0, SRCCOPY);
		}
	} while(tmpIndex>0 && m_NumOfComponents>1);

	SelectObject(tmpDC2, OldBitmap2);

	// None of the bitmaps mustn't be in any DC!
	CreateAlpha(*SourceBM, *AlphaBM, *BackgroundBM);

	SelectObject(tmpDC, *SourceBM);
	BitBlt(dc, X, Y, FullWidth, ItemHeight, tmpDC, 0, 0, SRCCOPY);

	SelectObject(tmpDC, OldBitmap);
	SelectObject(tmpDC2, OldBitmap2);

	// No need for these anymore
	delete BackgroundBM;
	delete AlphaBM;
	delete SourceBM;
}

/* 
** GetBackground
**
** Takes a copy of the given part of the window and blits it to the given bitmap.
** The bitmap must be already initialized!
**
*/
void CRasterizerBitmap::GetBackground(CDC& dc, CBitmap& Bitmap, int X, int Y, int Width, int Height) 
{
	HDC tmpDC;
	HBITMAP OldBitmap;

	tmpDC=CreateCompatibleDC(dc);
	if(tmpDC!=NULL) {
		// Fetch the background
		OldBitmap=(HBITMAP)SelectObject(tmpDC, Bitmap);
		BitBlt(tmpDC, 0, 0, Width, Height, dc, X, Y, SRCCOPY);
		SelectObject(tmpDC, OldBitmap);
		DeleteDC(tmpDC);
	}
}

/* 
** CreateAlpha
**
** Fills the Source-bitmap with new pixelvalues combined from Background, Source and Alpha.
** All bitmaps MUST be same size and same bitdepth. Only 16, 24 and 32 bit are supported.
** This is a classfunction!
*/
bool CRasterizerBitmap::CreateAlpha(HBITMAP Source, HBITMAP Alpha, HBITMAP Background)
{
	int i,j, RealWidth;
	LPBYTE BBuffer;
	LPBYTE SBuffer;
	LPBYTE ABuffer;
	BITMAP bm, abm, sbm;
	int Value;
	BITMAPINFO BI;
	
	GetObject(Background, sizeof(BITMAP), &bm);
	GetObject(Source, sizeof(BITMAP), &abm);
	GetObject(Alpha, sizeof(BITMAP), &sbm);

	// Check the sizes
	if(bm.bmHeight!=abm.bmHeight || bm.bmHeight!=sbm.bmHeight ||
	   bm.bmWidth!=abm.bmWidth || bm.bmWidth!=sbm.bmWidth) return false;

	RealWidth=(bm.bmWidth*3+3)/4*4;

	// All the bitmaps are same size and same colordepth (I hope :)
	BBuffer=new BYTE[RealWidth*bm.bmHeight];
	SBuffer=new BYTE[RealWidth*bm.bmHeight*3];
	ABuffer=new BYTE[RealWidth*bm.bmHeight*3];

	BI.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	BI.bmiHeader.biWidth=bm.bmWidth;
	BI.bmiHeader.biHeight=bm.bmHeight;
	BI.bmiHeader.biPlanes=1;
	BI.bmiHeader.biBitCount=24;
	BI.bmiHeader.biCompression=BI_RGB;
	BI.bmiHeader.biSizeImage=0;
	BI.bmiHeader.biXPelsPerMeter=0;
	BI.bmiHeader.biYPelsPerMeter=0;
	BI.bmiHeader.biClrUsed=0;
	BI.bmiHeader.biClrImportant=0;

	HDC dc=GetDC(GetDesktopWindow());
	GetDIBits(dc, Background, 0, bm.bmHeight, BBuffer, &BI, DIB_RGB_COLORS);
	GetDIBits(dc, Source, 0, bm.bmHeight, SBuffer, &BI, DIB_RGB_COLORS);
	GetDIBits(dc, Alpha, 0, bm.bmHeight, ABuffer, &BI, DIB_RGB_COLORS);
	ReleaseDC(GetDesktopWindow(), dc);

	for(j=0; j<bm.bmHeight; j++) {
		for(i=0; i<RealWidth; i++) {
			Value=( SBuffer[j*RealWidth+i]*ABuffer[j*RealWidth+i]+
					BBuffer[j*RealWidth+i]*(0x0FF-ABuffer[j*RealWidth+i]) )>>8;
			BBuffer[j*RealWidth+i]=(BYTE)Value;
		}
	}

	dc=GetDC(GetDesktopWindow());
	SetDIBits(dc, Source, 0, bm.bmHeight, BBuffer, &BI, DIB_RGB_COLORS);
	ReleaseDC(GetDesktopWindow(), dc);

	delete [] BBuffer;
	delete [] SBuffer;
	delete [] ABuffer;

	return true;
}

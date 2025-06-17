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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/RasterizerBitmap.cpp,v 1.8 2002/08/24 11:09:00 rainy Exp $

  $Log: RasterizerBitmap.cpp,v $
  Revision 1.8  2002/08/24 11:09:00  rainy
  Changed the error handling.

  Revision 1.7  2002/08/03 16:08:59  rainy
  Added support for profiles.

  Revision 1.6  2002/05/30 18:23:56  rainy
  Image loading uses LoadLSImage always.

  Revision 1.5  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.4  2002/02/27 18:49:33  rainy
  Alignment takes now 9 different positions.
  Added support for $var$

  Revision 1.3  2002/01/10 16:41:10  rainy
  Added vertical positioning.

  Revision 1.2  2001/12/23 10:00:37  rainy
  Renamed the static variables (C_ -> c_)

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "RasterizerBitmap.h"
#include "CalendarWindow.h"
#include "Error.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRasterizerBitmap::CRasterizerBitmap()
{
	m_NumOfComponents = 0;
	m_Alpha = false;
	m_Bitmap = NULL;
	m_AlphaBitmap = NULL;
	m_Separation = 0;
}

CRasterizerBitmap::~CRasterizerBitmap()
{
	std::map<const Profile*, BitmapData>::iterator i = m_ProfileBitmaps.begin();
	for ( ; i != m_ProfileBitmaps.end(); i++)
	{
		if (((*i).second).bitmap) DeleteObject(((*i).second).bitmap);
		if (((*i).second).alphaBitmap) DeleteObject(((*i).second).alphaBitmap);
	}
}

/* 
** Load
**
** Loads the given image. 
**
*/
void CRasterizerBitmap::Load(std::string filename)
{
	BITMAP bm;
	char buffer[1024];

	if(!CRainlendar::GetDummyLitestep()) 
	{
		VarExpansion(buffer, filename.c_str());
		filename = buffer;
	}

	// Check for absolute path
	if(-1 == filename.find(':')) 
	{
		if (!CCalendarWindow::c_Config.GetCurrentSkin().empty())
		{
			filename.insert(0, CCalendarWindow::c_Config.GetCurrentSkin() + "\\");
		}
		filename.insert(0, CCalendarWindow::c_Config.GetSkinsPath());
	} 

	m_Bitmap = LoadLSImage(filename.c_str(), NULL);

	if(m_Bitmap == NULL) 
	{
		std::string err = "File not found: ";
		err += filename;
		THROW(err);
	}

	GetObject(m_Bitmap, sizeof(BITMAP), &bm);
	m_Width = bm.bmWidth;
	m_Height = bm.bmHeight;

	// Check for Alpha-map
	m_Alpha = false;
	int pos = filename.rfind('.');
	if (pos != -1)
	{
		std::string alphaName;

		alphaName = filename;
		alphaName.insert(pos, "-Alpha");
		m_AlphaBitmap = LoadLSImage(alphaName.c_str(), NULL);

		if(m_AlphaBitmap != NULL) 
		{
			// Check that it is correct size
			GetObject(m_AlphaBitmap, sizeof(BITMAP), &bm);
			if (m_Width != bm.bmWidth || m_Height != bm.bmHeight) THROW(ERR_BACKGROUNDALPHASIZE);
			m_Alpha = true;
		}
	}

	// If the default profile is not in the map, put it there
	std::map<const Profile*, BitmapData>::iterator i = m_ProfileBitmaps.find(m_Profile);
	if (i == m_ProfileBitmaps.end())
	{
		BitmapData data;
		data.height = m_Height;
		data.width = m_Width;
		data.bitmap = m_Bitmap;
		data.alpha = m_Alpha;
		data.alphaBitmap = m_AlphaBitmap;
		m_ProfileBitmaps[m_Profile] = data;
	}
}

/* 
** Paint
**
** Paints the given part of the image. W & H are for align.
**
*/
void CRasterizerBitmap::Paint(HDC dc, int X, int Y, int W, int H, int Index)
{
	HBITMAP OldBitmap = NULL;
	HDC tmpDC=NULL;
	int Number, NumOfNums, ItemWidth, ItemHeight;

	// Check the current profile and load the bitmap if necessary
	std::map<const Profile*, BitmapData>::iterator i = m_ProfileBitmaps.find(m_Profile);
	if (i == m_ProfileBitmaps.end())
	{
		try 
		{
			Load(m_Profile->bitmapName);
		}
		catch(CError& error) 
		{
			MessageBox(NULL, error.GetString().c_str(), APPNAME, MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);

			// Create a dummy profile, so that the error won't come again
			i = m_ProfileBitmaps.find(NULL);
			if (i != m_ProfileBitmaps.end())
			{
				m_ProfileBitmaps[m_Profile] = (*i).second;
				BitmapData& data = (*i).second;
				m_Height = data.height;
				m_Width = data.width;
				m_Bitmap = data.bitmap;
				m_Alpha = data.alpha;
				m_AlphaBitmap = data.alphaBitmap;
			}
		}
	}
	else
	{
		BitmapData& data = (*i).second;
		m_Height = data.height;
		m_Width = data.width;
		m_Bitmap = data.bitmap;
		m_Alpha = data.alpha;
		m_AlphaBitmap = data.alphaBitmap;
	}

	if(m_Height > m_Width) 
	{
		// The items are stacked from top to bottom
		ItemWidth = m_Width;						
		ItemHeight = m_Height / m_NumOfComponents;
	} 
	else 
	{
		// The items are stacked from left to right
		ItemWidth = m_Width / m_NumOfComponents;	
		ItemHeight = m_Height;
	}

	// Lets find the number of the numbers ;-)
	Number = Index;
	NumOfNums = 1;	// At least one number
	if (m_NumOfComponents > 1) 
	{
		while(Number >= m_NumOfComponents) 
		{
			Number = Number / m_NumOfComponents;
			NumOfNums++;
		}
	}

	// Align the bitmap correctly
	switch (m_Align & 0x0F)
	{
	case CRasterizer::ALIGN_LEFT:
		X = X;
		break;

	case CRasterizer::ALIGN_HCENTER:
		X = X + (W - (NumOfNums * ItemWidth + (NumOfNums - 1) * m_Separation)) / 2;
		break;

	case CRasterizer::ALIGN_RIGHT:
		X = X + W - (NumOfNums * ItemWidth + (NumOfNums - 1) * m_Separation);
		break;
	};

	switch (m_Align & 0x0F0)
	{
	case CRasterizer::ALIGN_TOP:
		Y = Y;
		break;

	case CRasterizer::ALIGN_VCENTER:
		Y = Y + (H - ItemHeight) / 2;
		break;

	case CRasterizer::ALIGN_BOTTOM:
		Y = Y + (H - ItemHeight);
		break;
	};

	if(m_Alpha)
	{
		PaintAlpha(dc, X, Y, NumOfNums, Index);
	} 
	else 
	{
		tmpDC = CreateCompatibleDC(dc);
		if(tmpDC == NULL) return;
		OldBitmap = (HBITMAP)SelectObject(tmpDC, m_Bitmap);

		// Blit all necessary numbers
		int power = pow(10, NumOfNums - 1);
		do 
		{
			if (m_NumOfComponents == 1)
			{
				Number = 0;
				power = 0;
			} 
			else
			{
				Number = Index / power;
				Index -= Number * power;
				power /= 10;
			}

			if (m_Height > m_Width) 
			{
				TransparentBltLS(dc, X, Y, ItemWidth, ItemHeight, tmpDC, 0, Number * ItemHeight, RGB(255,0,255));
			} 
			else 
			{
				TransparentBltLS(dc, X, Y, ItemWidth, ItemHeight, tmpDC, Number * ItemWidth, 0, RGB(255,0,255));
			}

			X += ItemWidth + m_Separation;

		} while(power > 0 && m_NumOfComponents > 1);

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
void CRasterizerBitmap::PaintAlpha(HDC dc, int X, int Y, int NumOfNums, int Index)
{
	HBITMAP BackgroundBM=NULL;
	HBITMAP SourceBM=NULL;
	HBITMAP AlphaBM=NULL;
	HBITMAP OldBitmap = NULL;
	HBITMAP OldBitmap2 = NULL;
	HDC tmpDC;
	HDC tmpDC2;
	int FullWidth;	
	int Number;
	int tmpX;
	int ItemWidth, ItemHeight;

	if (m_Height > m_Width) 
	{
		// The items are stacked from top to bottom
		ItemWidth = m_Width;						
		ItemHeight = m_Height / m_NumOfComponents;
	} 
	else 
	{
		// The items are stacked from left to right
		ItemWidth = m_Width / m_NumOfComponents;	
		ItemHeight = m_Height;
	}


	FullWidth = NumOfNums * ItemWidth + (NumOfNums - 1 )* m_Separation;

	tmpDC = CreateCompatibleDC(dc);
	OldBitmap = (HBITMAP)SelectObject(tmpDC, m_Bitmap);

	// Create the alphablended bitmap (must be of the same size as the background)
	SourceBM = CreateCompatibleBitmap(tmpDC, FullWidth, ItemHeight);
	AlphaBM = CreateCompatibleBitmap(tmpDC, FullWidth, ItemHeight);

	tmpDC2 = CreateCompatibleDC(dc);
	OldBitmap2 = (HBITMAP)SelectObject(tmpDC2, SourceBM);

	// Hmm... probably should free allocated memory if it ran out. But in that
	// situation everything is fucked up anyway, so who cares.

	// If we have real alpha, we must fetch the background first
	BackgroundBM = GetBackground(dc, X, Y, FullWidth, ItemHeight);

	// Then we must get the correct component from the bitmap and alphamap

	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	RECT rect = { X, Y, FullWidth, ItemHeight };
	tmpX = 0;
	int power = pow(10, NumOfNums - 1);
	do 
	{
		if (m_NumOfComponents == 1)
		{
			Number = 0;
			power = 0;
		} 
		else
		{
			Number = Index / power;
			Index -= Number * power;
			power /= 10;
		}

		// Fetch the normal component
		SelectObject(tmpDC, m_Bitmap);
		SelectObject(tmpDC2, SourceBM);

		if(m_Height > m_Width) 
		{
			BitBlt(tmpDC2, tmpX, 0, ItemWidth, ItemHeight, tmpDC, 0, Number*ItemHeight, SRCCOPY);
		} else {
			BitBlt(tmpDC2, tmpX, 0, ItemWidth, ItemHeight, tmpDC, Number*ItemWidth, 0, SRCCOPY);
		}
			
		// Fetch the alpha component
		SelectObject(tmpDC, m_AlphaBitmap);
		SelectObject(tmpDC2, AlphaBM);

		// Set the alpha to full transparency first
		FillRect(tmpDC2, &rect, brush);

		if(m_Height > m_Width) 
		{
			BitBlt(tmpDC2, tmpX, 0, ItemWidth, ItemHeight, tmpDC, 0, Number*ItemHeight, SRCCOPY);
		} else {
			BitBlt(tmpDC2, tmpX, 0, ItemWidth, ItemHeight, tmpDC, Number*ItemWidth, 0, SRCCOPY);
		}

		tmpX += ItemWidth + m_Separation;

		SelectObject(tmpDC2, OldBitmap2);

		// None of the bitmaps mustn't be in any DC!
		CreateAlpha(SourceBM, AlphaBM, BackgroundBM);

	} while(power > 0 && m_NumOfComponents > 1);

	DeleteObject(brush);

	SelectObject(tmpDC, BackgroundBM);
	BitBlt(dc, X, Y, FullWidth, ItemHeight, tmpDC, 0, 0, SRCCOPY);

	SelectObject(tmpDC, OldBitmap);
	SelectObject(tmpDC2, OldBitmap2);

	// No need for these anymore
	DeleteObject(BackgroundBM);
	DeleteObject(AlphaBM);
	DeleteObject(SourceBM);
	DeleteObject(tmpDC);
	DeleteObject(tmpDC2);
}

/* 
** GetBackground
**
** Takes a copy of the given part of the dc and creates a bitmap out of it.
** The returned bitmap must be destroyed by the caller.
**
*/
HBITMAP CRasterizerBitmap::GetBackground(HDC dc, int X, int Y, int Width, int Height) 
{
	HDC tmpDC;
	HBITMAP OldBitmap;
	HBITMAP Bitmap = NULL;

	tmpDC = CreateCompatibleDC(dc);
	if(tmpDC != NULL) 
	{
		// Fetch the background
		Bitmap = CreateCompatibleBitmap(dc, Width, Height);
		OldBitmap = (HBITMAP)SelectObject(tmpDC, Bitmap);
		BitBlt(tmpDC, 0, 0, Width, Height, dc, X, Y, SRCCOPY);
		SelectObject(tmpDC, OldBitmap);
		DeleteDC(tmpDC);
	}

	return Bitmap;
}

/* 
** CreateAlpha
**
** Fills the Background-bitmap with new pixelvalues combined from Background, Source and Alpha.
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
	SetDIBits(dc, Background, 0, bm.bmHeight, BBuffer, &BI, DIB_RGB_COLORS);
	ReleaseDC(GetDesktopWindow(), dc);

	delete [] BBuffer;
	delete [] SBuffer;
	delete [] ABuffer;

	return true;
}

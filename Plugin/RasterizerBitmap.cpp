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
  $Header: /home/cvsroot/Rainlendar/Plugin/RasterizerBitmap.cpp,v 1.15 2004/11/06 13:38:59 rainy Exp $

  $Log: RasterizerBitmap.cpp,v $
  Revision 1.15  2004/11/06 13:38:59  rainy
  no message

  Revision 1.14  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.13  2003/10/27 17:40:01  Rainy
  Config is now singleton.

  Revision 1.12  2003/08/09 15:25:32  Rainy
  Added an error message if bitmap is not found.

  Revision 1.11  2003/03/22 20:29:33  rainy
  The size is taken from the image.

  Revision 1.10  2002/11/25 17:02:07  rainy
  Cleaned up the code.

  Revision 1.9  2002/11/12 18:10:24  rainy
  Added support for real alpha.

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
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "RasterizerBitmap.h"
#include "CalendarWindow.h"
#include "Error.h"
#include <math.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRasterizerBitmap::CRasterizerBitmap()
{
	m_NumOfComponents = 0;
	m_Separation = 0;
	m_Digits = 0;
}

CRasterizerBitmap::~CRasterizerBitmap()
{
}

bool CRasterizerBitmap::LoadImage(const std::string& filename, CImage* image)
{
	std::string name = filename;

	if (!name.empty())
	{
		CConfig::AddPath(name);

		if (!image->Load(name.c_str()))
		{
			char tmpSz[MAX_LINE_LENGTH];
			if (filename.empty())
			{
				strcpy(tmpSz, CCalendarWindow::c_Language.GetString("Error", 9));
			}
			else
			{
				sprintf(tmpSz, CCalendarWindow::c_Language.GetString("Error", 8), name.c_str());
			}
			MessageBox(NULL, tmpSz, "Rainlendar", MB_OK | MB_ICONERROR);
			return false;
		}
	}

	return true;
}

/* 
** Load
**
** Loads the given image. 
**
*/
void CRasterizerBitmap::Load(const std::string& filename)
{
	LoadImage(filename, &m_Image);

	m_Height = m_Image.GetHeight();
	m_Width = m_Image.GetWidth();
}

/* 
** Paint
**
** Paints the given part of the image. W & H are for align.
**
*/
void CRasterizerBitmap::Paint(CImage& background, int X, int Y, int W, int H, int Index)
{
	HBITMAP OldBitmap = NULL;
	HDC tmpDC=NULL;
	int Number, NumOfNums, ItemWidth, ItemHeight;

	const CImage* image = &m_Image;

	if (m_Profile && m_Profile->bitmap.GetBitmap() != NULL)
	{
		image = &(m_Profile->bitmap);
	}

	// Check the current profile and load the bitmap if necessary
	m_Height = image->GetHeight();
	m_Width = image->GetWidth();

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

	if (m_Digits == 0)
	{
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
	}
	else
	{
		NumOfNums = m_Digits;
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

	// Blit all necessary numbers
	int power = (int)pow(10.0, NumOfNums - 1);
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
			background.Blit(*image, X, Y, 0, Number * ItemHeight, ItemWidth, ItemHeight);
		} 
		else 
		{
			background.Blit(*image, X, Y, Number * ItemWidth, 0, ItemWidth, ItemHeight);
		}

		X += ItemWidth + m_Separation;

	} while(power > 0 && m_NumOfComponents > 1);
}



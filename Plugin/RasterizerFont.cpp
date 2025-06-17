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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/RasterizerFont.cpp,v 1.9 2002/11/25 17:01:29 rainy Exp $

  $Log: RasterizerFont.cpp,v $
  Revision 1.9  2002/11/25 17:01:29  rainy
  The buffers are now CImages instead of HBITMAPs.

  Revision 1.8  2002/11/12 18:10:18  rainy
  Added support for real alpha.

  Revision 1.7  2002/08/24 11:09:00  rainy
  Changed the error handling.

  Revision 1.6  2002/08/03 16:07:54  rainy
  Added CreateFont and modified SetFont to use that.

  Revision 1.5  2002/05/30 18:23:27  rainy
  Rect is now zeroed before used.

  Revision 1.4  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.3  2002/02/27 18:41:48  rainy
  Alignment takes now 9 different positions.

  Revision 1.2  2002/01/10 16:40:59  rainy
  Added vertical positioning.

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#pragma warning(disable: 4786)

#include "CalendarWindow.h"
#include "RasterizerFont.h"
#include "Error.h"
#include "Config.h"

CImage CRasterizerFont::m_ColorBuffer;
CImage CRasterizerFont::m_AlphaBuffer;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRasterizerFont::CRasterizerFont()
{
	m_Font = NULL;
}

CRasterizerFont::~CRasterizerFont()
{
	if (m_Font) DeleteObject(m_Font);
}

/* 
** CreateStringTable
**
** Tokenizes the given string for names
**
*/
void CRasterizerFont::CreateStringTable(const std::string& text, int count)
{
	std::string items = text;
	int pos = 0;
	int counter = 0;

	m_StringTable.clear();
	
	while(pos != -1)
	{
		pos = items.find('/');
		if (pos != -1)
		{
			m_StringTable.push_back(std::string(items.begin(), items.begin() + pos));
			items.erase(items.begin(), items.begin() + pos + 1);
		}
		else
		{
			m_StringTable.push_back(items);
		}
		counter++;
	}

	if (counter != count)
	{
		std::string err = "String: \"";
		err += text;
		err += "\" has incorrect number of items!";
	
		THROW(err);
	}
}

void CRasterizerFont::SetFont(const std::string& FontName)
{
	if (m_Font) DeleteObject(m_Font);
	m_Font = CreateFont(FontName);
}

/* 
** CreateFont
**
** Creates the font from given string. The string must have '/'-char as 
** separator for the fields.
**
*/
HFONT CRasterizerFont::CreateFont(const std::string& FontName)
{
	HFONT font;
	int nHeight;
	int nWidth;
	int nEscapement;
	int nOrientation; 
	int nWeight;
	BYTE bItalic; 
	BYTE bUnderline;
	BYTE cStrikeOut; 
	BYTE nCharSet;
	BYTE nOutPrecision;
	BYTE nClipPrecision; 
	BYTE nQuality;
	BYTE nPitchAndFamily;

	sscanf(FontName.c_str(), "%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i", 
					&nHeight, &nWidth, &nEscapement, &nOrientation, &nWeight,
					&bItalic, &bUnderline, &cStrikeOut, &nCharSet, &nOutPrecision, 
					&nClipPrecision, &nQuality, &nPitchAndFamily);

	int pos = FontName.rfind('/');
	std::string name(FontName.begin() + pos + 1, FontName.end());

	font = ::CreateFont( 
				nHeight, 
				nWidth, 
				nEscapement, 
				nOrientation, 
				nWeight, 
				bItalic, 
				bUnderline, 
				cStrikeOut, 
				nCharSet, 
				nOutPrecision, 
				nClipPrecision, 
				nQuality, 
				nPitchAndFamily, 
				name.c_str());

	if (font == NULL) THROW(ERR_CREATEFONT);

	return font;
}

/* 
** UpdateDimensions
**
** Calculates the maximum dimensions
**
*/
void CRasterizerFont::UpdateDimensions(const char* defaultString)
{
	HWND Desktop;
	HDC DDC;
	HDC tmpDC;
	HFONT OldFont;
	RECT Rect = {0, 0, 0, 0};

	Desktop=GetDesktopWindow();
	if(Desktop==NULL) THROW(ERR_TEXTDIMENSIONS);
	DDC=GetDC(Desktop);
	if(DDC==NULL) THROW(ERR_TEXTDIMENSIONS);

	tmpDC=CreateCompatibleDC(DDC);
	if(tmpDC==NULL) THROW(ERR_OUTOFMEM);

	ReleaseDC(Desktop, DDC);

	OldFont = (HFONT)SelectObject(tmpDC, m_Font);

	m_Width = 0;
	m_Height = 0;

	if (defaultString)
	{
		DrawText(tmpDC, defaultString, strlen(defaultString), &Rect, DT_CALCRECT | DT_SINGLELINE);
	
		m_Width = Rect.right - Rect.left;
		m_Height = Rect.bottom - Rect.top;
	}
	else
	{
		// Loop all strings and find the largest
		for (int i = 0; i < m_StringTable.size(); i++)
		{
			DrawText(tmpDC, m_StringTable[i].c_str(), m_StringTable[i].size(), &Rect, DT_CALCRECT | DT_SINGLELINE);
		
			m_Width = max(Rect.right - Rect.left, m_Width);
			m_Height = max(Rect.bottom - Rect.top, m_Height);
		}
	}

	SelectObject(tmpDC, OldFont);
	DeleteDC(tmpDC);
}

/* 
** Paint
**
** Paints the given part of the image. W & H are for align.
**
*/
void CRasterizerFont::Paint(CImage& background, int X, int Y, int W, int H, int Index)
{
	HFONT oldFont;
	HBITMAP oldBitmap;
	UINT format = 0;
	RECT rect = { X, Y, X + W, Y + H };
	char tmpSz[16];

	// Set the alignment
	switch (m_Align & 0x0F)
	{
	case CRasterizer::ALIGN_LEFT:
		format |= DT_LEFT;
		break;

	case CRasterizer::ALIGN_HCENTER:
		format |= DT_CENTER;
		break;

	case CRasterizer::ALIGN_RIGHT:
		format |= DT_RIGHT;
		break;
	};

	switch (m_Align & 0x0F0)
	{
	case CRasterizer::ALIGN_TOP:
		format |= DT_TOP;
		break;

	case CRasterizer::ALIGN_VCENTER:
		format |= DT_VCENTER;
		break;

	case CRasterizer::ALIGN_BOTTOM:
		format |= DT_BOTTOM;
		break;
	};

	format |= DT_NOCLIP | DT_SINGLELINE;

	HDC dc = CreateCompatibleDC(NULL);
	oldBitmap = (HBITMAP)SelectObject(dc, background.GetBitmap());
	oldFont = (HFONT)SelectObject(dc, m_Font);

	SetBkMode(dc, TRANSPARENT);
	if (m_Profile)
	{
		SetTextColor(dc, m_Profile->fontColor);
	}
	else
	{
		SetTextColor(dc, m_Color);
	}

	const char* text = NULL;
	if(m_StringTable.empty()) 
	{
		sprintf(tmpSz, "%i", Index);
		text = tmpSz;
	} 
	else 
	{
		text = m_StringTable[Index].c_str();
	}

	if (CCalendarWindow::Is2k() && CCalendarWindow::c_Config.GetNativeTransparency())
	{
		CreateBuffers(background.GetWidth(), background.GetHeight());

		SelectObject(dc, m_ColorBuffer.GetBitmap());
		DrawText(dc, text, strlen(text), &rect, format);
		
		SelectObject(dc, m_AlphaBuffer.GetBitmap());
		SetTextColor(dc, RGB(255, 255, 255));
		DrawText(dc, text, strlen(text), &rect, format);
	}
	else
	{
		// Just draw it normally
		DrawText(dc, text, strlen(text), &rect, format);
	}

	SelectObject(dc, oldBitmap);
	SelectObject(dc, oldFont);
	DeleteDC(dc);
}

void CRasterizerFont::CreateBuffers(int width, int height)
{
	// We need to draw the text to another bitmap for the alpha
	if (m_AlphaBuffer.GetBitmap() == NULL || m_ColorBuffer.GetBitmap() == NULL)
	{
		m_AlphaBuffer.Create(width, height, 255);
		m_ColorBuffer.Create(width, height, 255);

		HDC dc = CreateCompatibleDC(NULL);
		RECT r = { 0, 0, width, height };
		HBRUSH brush;

		brush = CreateSolidBrush(RGB(0, 0, 0));
		HBITMAP oldBitmap = (HBITMAP)SelectObject(dc, m_AlphaBuffer.GetBitmap());
		FillRect(dc, &r, brush);
		DeleteObject(brush);

		brush = CreateSolidBrush(RGB(128, 128, 128));
		SelectObject(dc, m_ColorBuffer.GetBitmap());
		FillRect(dc, &r, brush);
		DeleteObject(brush);

		SelectObject(dc, oldBitmap);
		DeleteDC(dc);
	}
}
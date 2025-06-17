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
  $Header: /home/cvsroot/Rainlendar/Plugin/RasterizerFont.cpp,v 1.16 2004/12/05 18:22:28 rainy Exp $

  $Log: RasterizerFont.cpp,v $
  Revision 1.16  2004/12/05 18:22:28  rainy
  Drawing uses always buffers.

  Revision 1.15  2004/11/06 13:38:59  rainy
  no message

  Revision 1.14  2004/06/10 16:20:41  rainy
  Fixed text drawing

  Revision 1.13  2004/01/28 18:07:03  rainy
  Multiline text is not clipped.

  Revision 1.12  2003/12/20 22:19:06  rainy
  Changed BYTEs to DWORDs.
  Prefix is not included in the rect calculations.

  Revision 1.11  2003/12/05 15:47:11  Rainy
  No prefix in todos

  Revision 1.10  2003/10/27 17:39:13  Rainy
  Separated drawing so that it can be used outside this class.

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
#pragma warning(disable: 4996)

#include "CalendarWindow.h"
#include "RasterizerFont.h"
#include "Error.h"
#include "Config.h"

CImage CRasterizerFont::c_ColorBuffer;
CImage CRasterizerFont::c_AlphaBuffer;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRasterizerFont::CRasterizerFont()
{
	m_Font = NULL;
	m_StrikeOutFont = NULL;
}

CRasterizerFont::~CRasterizerFont()
{
	if (m_Font) DeleteObject(m_Font);
	if (m_StrikeOutFont) DeleteObject(m_StrikeOutFont);
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

void CRasterizerFont::SetFont(const std::string& FontName, bool strikeOut)
{
	if (m_Font) DeleteObject(m_Font);
	if (m_StrikeOutFont) DeleteObject(m_StrikeOutFont);
	m_Font = CreateFont(FontName, false);
	if (strikeOut)
	{
		m_StrikeOutFont = CreateFont(FontName, true);
	}
}

/* 
** CreateFont
**
** Creates the font from given string. The string must have '/'-char as 
** separator for the fields.
**
*/
HFONT CRasterizerFont::CreateFont(const std::string& FontName, bool strikeOut)
{
	HFONT font;
	int nHeight;
	int nWidth;
	int nEscapement;
	int nOrientation; 
	int nWeight;
	DWORD bItalic; 
	DWORD bUnderline;
	DWORD cStrikeOut; 
	DWORD nCharSet;
	DWORD nOutPrecision;
	DWORD nClipPrecision; 
	DWORD nQuality;
	DWORD nPitchAndFamily;

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
				strikeOut ? TRUE : cStrikeOut, 
				CConfig::Instance().GetUseSkinCharSet() ? nCharSet : DEFAULT_CHARSET,
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
	RECT rect = {0, 0, 0, 0};

	m_Width = 0;
	m_Height = 0;

	if (defaultString)
	{
		GetTextSize(defaultString, &rect, false);
	
		m_Width = rect.right - rect.left;
		m_Height = rect.bottom - rect.top;
	}
	else
	{
		// Loop all strings and find the largest
		for (UINT i = 0; i < m_StringTable.size(); i++)
		{
			GetTextSize(m_StringTable[i].c_str(), &rect, false);

			m_Width = max(rect.right - rect.left, m_Width);
			m_Height = max(rect.bottom - rect.top, m_Height);
		}
	}
}

/* 
** GetTextSize
**
** Calculates the dimensions of the given text
**
*/
void CRasterizerFont::GetTextSize(const char* text, RECT* rect, bool multiline)
{
	HWND Desktop;
	HDC DDC;
	HDC tmpDC;
	HFONT OldFont;

	if (text == NULL || strlen(text) == 0)
	{
		rect->left = 0;
		rect->top = 0;
		rect->right = 0;
		rect->bottom = 0;
	}
	else
	{
		Desktop=GetDesktopWindow();
		if(Desktop==NULL) THROW(ERR_TEXTDIMENSIONS);
		DDC=GetDC(Desktop);
		if(DDC==NULL) THROW(ERR_TEXTDIMENSIONS);

		tmpDC=CreateCompatibleDC(DDC);
		if(tmpDC==NULL) THROW(ERR_OUTOFMEM);

		ReleaseDC(Desktop, DDC);

		OldFont = (HFONT)SelectObject(tmpDC, m_Font);

		UINT format = DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX;
		if (multiline)
		{
			format = DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX;
		}

		DrawText(tmpDC, text, strlen(text), rect, format);

		SelectObject(tmpDC, OldFont);
		DeleteDC(tmpDC);
	}
}

/* 
** Paint
**
** Paints the given part of the image. W & H are for align.
**
*/
void CRasterizerFont::Paint(CImage& background, int X, int Y, int W, int H, int Index)
{
	char tmpSz[16];
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

	WriteText(background, X, Y, W, H, text, false, false);
}

/* 
** DrawText
**
** Draws the actual text
**
*/
void CRasterizerFont::WriteText(CImage& background, int X, int Y, int W, int H, const char* text, bool multiline, bool strikeout)
{
	HFONT oldFont;
	HBITMAP oldBitmap;
	UINT format = 0;
	RECT rect = { 1, 1, W + 1, H + 1 };

	if (multiline)
	{
		format |= DT_NOCLIP | DT_WORDBREAK | DT_NOPREFIX;
	}
	else
	{
		format |= DT_NOCLIP | DT_SINGLELINE | DT_NOPREFIX; 
	}

	HDC dc = CreateCompatibleDC(NULL);
	oldBitmap = (HBITMAP)SelectObject(dc, background.GetBitmap());
	if (strikeout && m_StrikeOutFont)
	{
		oldFont = (HFONT)SelectObject(dc, m_StrikeOutFont);
	}
	else
	{
		oldFont = (HFONT)SelectObject(dc, m_Font);
	}

	SetBkMode(dc, TRANSPARENT);
	if (m_Profile)
	{
		SetTextColor(dc, m_Profile->fontColor);
	}
	else
	{
		SetTextColor(dc, m_Color);
	}

	RECT r = rect;
	GetTextSize(text, &r, multiline);

	CreateBuffers(r.right, r.bottom);

	SelectObject(dc, c_ColorBuffer.GetBitmap());
	DrawText(dc, text, strlen(text), &rect, format);
	
	SelectObject(dc, c_AlphaBuffer.GetBitmap());
	SetTextColor(dc, RGB(255, 255, 255));
	DrawText(dc, text, strlen(text), &rect, format);

	SelectObject(dc, oldBitmap);
	SelectObject(dc, oldFont);
	DeleteDC(dc);

	// Align the bitmap correctly
	switch (m_Align & 0x0F)
	{
	case CRasterizer::ALIGN_LEFT:
		X = X;
		break;

	case CRasterizer::ALIGN_HCENTER:
		X = X + (W - r.right) / 2;
		break;

	case CRasterizer::ALIGN_RIGHT:
		X = X + (W - r.right);
		break;
	};

	switch (m_Align & 0x0F0)
	{
	case CRasterizer::ALIGN_TOP:
		Y = Y;
		break;

	case CRasterizer::ALIGN_VCENTER:
		Y = Y + (H - r.bottom) / 2;
		break;

	case CRasterizer::ALIGN_BOTTOM:
		Y = Y + (H - r.bottom);
		break;
	};
	
	CRasterizerFont::DrawBuffers(background, X - 1, Y - 1);
}

void CRasterizerFont::CreateBuffers(int width, int height)
{
	// We need to draw the text to another bitmap for the alpha
	if (c_AlphaBuffer.GetBitmap() == NULL || c_ColorBuffer.GetBitmap() == NULL)
	{
		c_AlphaBuffer.Create(width, height, 255);
		c_ColorBuffer.Create(width, height, 255);

		HDC dc = CreateCompatibleDC(NULL);
		RECT r = { 0, 0, width, height };
		HBRUSH brush;

		brush = CreateSolidBrush(RGB(0, 0, 0));
		HBITMAP oldBitmap = (HBITMAP)SelectObject(dc, c_AlphaBuffer.GetBitmap());
		FillRect(dc, &r, brush);
		DeleteObject(brush);

		brush = CreateSolidBrush(RGB(128, 128, 128));
		SelectObject(dc, c_ColorBuffer.GetBitmap());
		FillRect(dc, &r, brush);
		DeleteObject(brush);

		SelectObject(dc, oldBitmap);
		DeleteDC(dc);
	}
}

void CRasterizerFont::DrawBuffers(CImage& doubleBuffer, int x, int y)
{
	// The text might be written to the buffers, so draw them
	if (c_ColorBuffer.GetBitmap() != NULL  && c_AlphaBuffer.GetBitmap() != NULL)
	{
		CImage textImage;
		textImage.Create(c_ColorBuffer.GetBitmap(), c_AlphaBuffer.GetBitmap(), 0);
		doubleBuffer.Blit(textImage, x, y, 0, 0, textImage.GetWidth(), textImage.GetHeight());
		ClearBuffers();
	}
}

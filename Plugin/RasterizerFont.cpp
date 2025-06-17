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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/RasterizerFont.cpp,v 1.2 2002/01/10 16:40:59 rainy Exp $

  $Log: RasterizerFont.cpp,v $
  Revision 1.2  2002/01/10 16:40:59  rainy
  Added vertical positioning.

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "RasterizerFont.h"
#include "Error.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRasterizerFont::CRasterizerFont()
{
	m_StringTable=NULL;
}

CRasterizerFont::~CRasterizerFont()
{
	if(m_StringTable!=NULL) {
		int i=0;
		while(m_StringTable[i]!=NULL) {
			delete [] m_StringTable[i++];
		}
		delete [] m_StringTable;
		m_StringTable=NULL;
	}
}

/* 
** CreateStringTable
**
** Tokenizes the given string for names
**
*/
void CRasterizerFont::CreateStringTable(CString& String, int Count)
{
	int Index, i, Start;

	m_StringTable=new char*[Count+1];
	if(m_StringTable==NULL) throw ERR_OUTOFMEM;

	Start=0;
	for(i=0; i<Count; i++) {
		Index=String.Find('/', Start);
		if(Index==-1) Index=String.GetLength();

		m_StringTable[i]=new char[Index-Start+1];
		if(m_StringTable[i]==NULL) throw ERR_OUTOFMEM;
		strcpy(m_StringTable[i], String.Mid(Start, Index-Start));

		Start=Index+1;
	}

	// Set the last one to NULL
	m_StringTable[i]=NULL;
}

/* 
** SetFont
**
** Creates the font from given string. The string must have '/'-char as 
** separator for the fields.
**
*/
void CRasterizerFont::SetFont(CString& FontName)
{
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

	sscanf(FontName, "%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i/%i", 
					&nHeight, &nWidth, &nEscapement, &nOrientation, &nWeight,
					&bItalic, &bUnderline, &cStrikeOut, &nCharSet, &nOutPrecision, 
					&nClipPrecision, &nQuality, &nPitchAndFamily);

	if(0==m_Font.CreateFont( 
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
		FontName.Mid(FontName.ReverseFind('/')+1)
		)) throw ERR_CREATEFONT;
}

/* 
** UpdateDimensions
**
** Calculates the maximum dimensions
**
*/
void CRasterizerFont::UpdateDimensions(char* DefaultString)
{
	char* String;
	int i=0;
	HWND Desktop;
	HDC DDC;
	HDC tmpDC;
	HFONT OldFont;
	RECT Rect;

	Desktop=GetDesktopWindow();
	if(Desktop==NULL) throw ERR_TEXTDIMENSIONS;
	DDC=GetDC(Desktop);
	if(DDC==NULL) throw ERR_TEXTDIMENSIONS;

	tmpDC=CreateCompatibleDC(DDC);
	if(tmpDC==NULL) throw ERR_OUTOFMEM;

	ReleaseDC(Desktop, DDC);

	OldFont=(HFONT)SelectObject(tmpDC, m_Font);

	m_Width=0;
	m_Height=0;

	// Loop all strings and find the largest

	if(m_StringTable==NULL) {
		String=DefaultString;
	} else {
		String=m_StringTable[i++];
	}

	while(String!=NULL) {
		DrawText(tmpDC, String, strlen(String), &Rect, DT_CALCRECT | DT_SINGLELINE);

		m_Width=max(Rect.right-Rect.left, m_Width);
		m_Height=max(Rect.bottom-Rect.top, m_Height);

		if(m_StringTable==NULL) {
			String=NULL;
		} else {
			String=m_StringTable[i++];
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
void CRasterizerFont::Paint(CDC& dc, int X, int Y, int W, int H, int Index)
{
	CFont* OldFont;
	UINT Format;
	CRect Rect(X, Y, X+W, Y+H);
	char tmpSz[16];
	char* String;

	// Set the alignment
	switch(m_Align) {
	case CRasterizer::ALIGN_LEFT:
		Format=DT_LEFT | DT_VCENTER;
		break;
	case CRasterizer::ALIGN_CENTER:
		Format=DT_CENTER | DT_VCENTER;
		break;
	case CRasterizer::ALIGN_RIGHT:
		Format=DT_RIGHT | DT_VCENTER;
		break;
	case CRasterizer::ALIGN_TOP:
		Format=DT_TOP | DT_CENTER;
		break;
	case CRasterizer::ALIGN_BOTTOM:
		Format=DT_BOTTOM | DT_CENTER;
		break;
	}	

	Format|=DT_NOCLIP | DT_SINGLELINE;

	if(m_StringTable==NULL) {
		sprintf(tmpSz, "%i", Index);
		String=tmpSz;
	} else {
		String=m_StringTable[Index];
	}

	OldFont=dc.SelectObject(&m_Font);
	
	dc.DrawText( String, strlen(String), Rect, Format );

	dc.SelectObject(OldFont);
}

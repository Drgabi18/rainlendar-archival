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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/RasterizerFont.h,v 1.1.1.1 2001/10/29 18:56:23 rainy Exp $

  $Log: RasterizerFont.h,v $
  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#if !defined(AFX_RASTERIZERFONT_H__132BB028_0E9C_4BC1_A3DA_A173056A3298__INCLUDED_)
#define AFX_RASTERIZERFONT_H__132BB028_0E9C_4BC1_A3DA_A173056A3298__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Rasterizer.h"
#include <afxtempl.h>

class CRasterizerFont : public CRasterizer  
{
public:
	CRasterizerFont();
	virtual ~CRasterizerFont();

	void SetFont(CString& FontName);
	void CreateStringTable(CString& String, int Count);

	void UpdateDimensions(char* DefaultString=NULL);

	void Paint(CDC& dc, int X, int Y, int W, int H, int Index);

protected:
	char** m_StringTable;
	CFont m_Font;
};

#endif // !defined(AFX_RASTERIZERFONT_H__132BB028_0E9C_4BC1_A3DA_A173056A3298__INCLUDED_)

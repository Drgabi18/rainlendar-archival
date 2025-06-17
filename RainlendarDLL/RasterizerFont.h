// RasterizerFont.h: interface for the CRasterizerFont class.
//
//////////////////////////////////////////////////////////////////////

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

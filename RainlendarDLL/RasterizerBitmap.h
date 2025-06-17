// RasterizerBitmap.h: interface for the CRasterizerBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RASTERIZERBITMAP_H__C7D0FDC2_F062_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_RASTERIZERBITMAP_H__C7D0FDC2_F062_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Rasterizer.h"
#include <afxtempl.h>

class CRasterizerBitmap : public CRasterizer  
{
public:
	CRasterizerBitmap();
	virtual ~CRasterizerBitmap();

	void Load(CString& Filename);
	void SetNumOfComponents(int Number) { m_NumOfComponents=Number; };
	void Paint(CDC& dc, int X, int Y, int W, int H, int Index);

	static bool CreateAlpha(HBITMAP Source, HBITMAP Alpha, HBITMAP Background);

protected:
	void GetBackground(CDC& dc, CBitmap& Bitmap, int X, int Y, int Width, int Height);
	void PaintAlpha(CDC& dc, int X, int Y, int NumOfNums, int Index);

	int m_NumOfComponents;

	CBitmap m_Bitmap;
	CBitmap m_AlphaBitmap;
	bool m_Alpha;
};

#endif // !defined(AFX_RASTERIZERBITMAP_H__C7D0FDC2_F062_11D3_92A3_0080AD90417B__INCLUDED_)

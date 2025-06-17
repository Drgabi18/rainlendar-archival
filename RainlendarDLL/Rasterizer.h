// Rasterizer.h: interface for the CRasterizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RASTERIZER_H__C7D0FDC1_F062_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_RASTERIZER_H__C7D0FDC1_F062_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRasterizer  
{
public:
	enum ALIGN {
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};

	enum TYPE {
		TYPE_NONE,
		TYPE_BITMAP,
		TYPE_FONT
	};

	CRasterizer();
	virtual ~CRasterizer();

	ALIGN GetAlign() { return m_Align; };
	void SetAlign(ALIGN Align ) { m_Align=Align; };

	virtual void Paint(CDC& dc, int X, int Y, int W, int H, int Index) = 0;

	int GetHeight() { return m_Height; };
	int GetWidth() { return m_Width; };

protected:
	ALIGN m_Align;

	int m_Width;
	int m_Height;
};

#endif // !defined(AFX_RASTERIZER_H__C7D0FDC1_F062_11D3_92A3_0080AD90417B__INCLUDED_)

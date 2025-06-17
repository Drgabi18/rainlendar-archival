// Background.h: interface for the CBackground class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACKGROUND_H__C7D0FDBE_F062_11D3_92A3_0080AD90417B__INCLUDED_)
#define AFX_BACKGROUND_H__C7D0FDBE_F062_11D3_92A3_0080AD90417B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBackground
{
public:
	CBackground();
	~CBackground();

	void Load(CString& Filename);
	void Create(int Width, int Height);
	void CopyBackground(int Width, int Height);
	void Paint(CDC& dc);

	void SetWindow(CWnd* CalendarWindow) { m_CalendarWindow=CalendarWindow; };

	int GetWidth() { return m_Width; };
	int GetHeight() { return m_Height; };

	void Initialize();

private:
	CString m_Filename;
	
	int m_Width;
	int m_Height;

	CWnd* m_CalendarWindow;

	bool m_Alpha;

	CBitmap m_AlphaImage;
	CBitmap m_Image;

	CBitmap m_Background;	// The background picture
};

#endif // !defined(AFX_BACKGROUND_H__C7D0FDBE_F062_11D3_92A3_0080AD90417B__INCLUDED_)

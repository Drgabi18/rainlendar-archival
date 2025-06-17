#if !defined(AFX_DIALOGTODAY_H__FD0FDD9C_7FFB_4CFB_B42B_A62024965CE8__INCLUDED_)
#define AFX_DIALOGTODAY_H__FD0FDD9C_7FFB_4CFB_B42B_A62024965CE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogToday.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogToday dialog

class CDialogToday : public CPropertyPage
{
	DECLARE_DYNCREATE(CDialogToday)

// Construction
public:
	CDialogToday();
	~CDialogToday();

// Dialog Data
	//{{AFX_DATA(CDialogToday)
	enum { IDD = IDD_TODAY };
	CComboBox	m_Rasterizer;
	CComboBox	m_Align;
	CString	m_Bitmap;
	BOOL	m_Enable;
	CString	m_Font;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogToday)
	public:
	virtual BOOL OnApply();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_RealFont;
	COLORREF m_FontColor;

	void UpdateConfig();

	// Generated message map functions
	//{{AFX_MSG(CDialogToday)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnTodayEnable();
	afx_msg void OnSelchangeTodayRasterizer();
	afx_msg void OnSelchangeTodayAlign();
	afx_msg void OnChangeTodayBitmap();
	afx_msg void OnTodayComponents1();
	afx_msg void OnTodayComponents10();
	afx_msg void OnTodayComponents32();
	afx_msg void OnTodayFontcolor();
	afx_msg void OnChangeTodayFontname();
	afx_msg void OnTodaySelect();
	afx_msg void OnTodayBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGTODAY_H__FD0FDD9C_7FFB_4CFB_B42B_A62024965CE8__INCLUDED_)

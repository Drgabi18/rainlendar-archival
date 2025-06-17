#if !defined(AFX_DIALOGYEAR_H__4CB8A636_3C71_439B_B00D_9CE45FE78928__INCLUDED_)
#define AFX_DIALOGYEAR_H__4CB8A636_3C71_439B_B00D_9CE45FE78928__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
// DialogYear.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogYear dialog

class CDialogYear : public CPropertyPage
{
	DECLARE_DYNCREATE(CDialogYear)

// Construction
public:
	CDialogYear();
	~CDialogYear();

// Dialog Data
	//{{AFX_DATA(CDialogYear)
	enum { IDD = IDD_YEAR };
	CComboBox	m_Rasterizer;
	CComboBox	m_Align;
	CString	m_Bitmap;
	BOOL	m_Enable;
	CString	m_Font;
	UINT	m_X;
	UINT	m_Y;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogYear)
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
	//{{AFX_MSG(CDialogYear)
	virtual BOOL OnInitDialog();
	afx_msg void OnYearEnable();
	afx_msg void OnSelchangeYearRasterizer();
	afx_msg void OnYearBrowse();
	afx_msg void OnYearSelect();
	afx_msg void OnYearFontcolor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelchangeYearAlign();
	afx_msg void OnChangeYearBitmap();
	afx_msg void OnChangeYearX();
	afx_msg void OnChangeYearY();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGYEAR_H__4CB8A636_3C71_439B_B00D_9CE45FE78928__INCLUDED_)

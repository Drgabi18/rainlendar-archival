#if !defined(AFX_DIALOGMONTH_H__9FCDC505_42C2_49DC_BF52_C1D066B2E21B__INCLUDED_)
#define AFX_DIALOGMONTH_H__9FCDC505_42C2_49DC_BF52_C1D066B2E21B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogMonth.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogMonth dialog

class CDialogMonth : public CPropertyPage
{
	DECLARE_DYNCREATE(CDialogMonth)

// Construction
public:
	CDialogMonth();
	~CDialogMonth();

// Dialog Data
	//{{AFX_DATA(CDialogMonth)
	enum { IDD = IDD_MONTH };
	CComboBox	m_Align;
	CComboBox	m_Rasterizer;
	CString	m_Bitmap;
	BOOL	m_Enable;
	CString	m_Font;
	UINT	m_X;
	UINT	m_Y;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogMonth)
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
	//{{AFX_MSG(CDialogMonth)
	virtual BOOL OnInitDialog();
	afx_msg void OnMonthEnable();
	afx_msg void OnSelchangeMonthRasterizer();
	afx_msg void OnMonthBrowse();
	afx_msg void OnMonthSelect();
	afx_msg void OnMonthFontcolor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelchangeMonthAlign();
	afx_msg void OnChangeMonthBitmap();
	afx_msg void OnChangeMonthX();
	afx_msg void OnChangeMonthY();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGMONTH_H__9FCDC505_42C2_49DC_BF52_C1D066B2E21B__INCLUDED_)

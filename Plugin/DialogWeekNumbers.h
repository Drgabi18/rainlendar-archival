#if !defined(AFX_DIALOGWEEKNUMBERS_H__FB74E3B3_2835_4388_A4B3_0BBB37042356__INCLUDED_)
#define AFX_DIALOGWEEKNUMBERS_H__FB74E3B3_2835_4388_A4B3_0BBB37042356__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogWeekNumbers.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogWeekNumbers dialog

class CDialogWeekNumbers : public CPropertyPage
{
	DECLARE_DYNCREATE(CDialogWeekNumbers)

// Construction
public:
	CDialogWeekNumbers();
	~CDialogWeekNumbers();

// Dialog Data
	//{{AFX_DATA(CDialogWeekNumbers)
	enum { IDD = IDD_WEEKNUMBERS };
	CComboBox	m_Rasterizer;
	CComboBox	m_Align;
	CString	m_Bitmap;
	BOOL	m_Enable;
	CString	m_Font;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogWeekNumbers)
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
	//{{AFX_MSG(CDialogWeekNumbers)
	afx_msg void OnChangeWeeknumBitmap();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeWeeknumAlign();
	afx_msg void OnSelchangeWeeknumRasterizer();
	afx_msg void OnWeeknumBrowse();
	afx_msg void OnWeeknumEnable();
	afx_msg void OnWeeknumFontcolor();
	afx_msg void OnWeeknumSelect();
	afx_msg void OnWeeknumComponents54();
	afx_msg void OnWeeknumComponents10();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGWEEKNUMBERS_H__FB74E3B3_2835_4388_A4B3_0BBB37042356__INCLUDED_)

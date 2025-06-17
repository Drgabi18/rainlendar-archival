#if !defined(AFX_DIALOGWEEKDAYS_H__B120CF32_B331_4DEB_A966_4346602B5ED0__INCLUDED_)
#define AFX_DIALOGWEEKDAYS_H__B120CF32_B331_4DEB_A966_4346602B5ED0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogWeekdays.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogWeekdays dialog

class CDialogWeekdays : public CPropertyPage
{
	DECLARE_DYNCREATE(CDialogWeekdays)

// Construction
public:
	CDialogWeekdays();
	~CDialogWeekdays();

// Dialog Data
	//{{AFX_DATA(CDialogWeekdays)
	enum { IDD = IDD_WEEKDAYS };
	CComboBox	m_Rasterizer;
	CComboBox	m_Align;
	BOOL	m_Enable;
	CString	m_Bitmap;
	CString	m_Font;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogWeekdays)
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
	//{{AFX_MSG(CDialogWeekdays)
	virtual BOOL OnInitDialog();
	afx_msg void OnWeekdaysEnable();
	afx_msg void OnSelchangeWeekdaysRasterizer();
	afx_msg void OnWeekdaysBrowse();
	afx_msg void OnWeekdaysSelect();
	afx_msg void OnWeekdaysFontcolor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelchangeWeekdaysAlign();
	afx_msg void OnChangeWeekdaysBitmap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGWEEKDAYS_H__B120CF32_B331_4DEB_A966_4346602B5ED0__INCLUDED_)

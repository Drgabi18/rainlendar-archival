#if !defined(AFX_DIALOGDAYS_H__8D4807E8_F381_40DB_A427_DEC4B20B6414__INCLUDED_)
#define AFX_DIALOGDAYS_H__8D4807E8_F381_40DB_A427_DEC4B20B6414__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogDays.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDays dialog

class CDialogDays : public CPropertyPage
{
	DECLARE_DYNCREATE(CDialogDays)

// Construction
public:
	CDialogDays();
	~CDialogDays();

// Dialog Data
	//{{AFX_DATA(CDialogDays)
	enum { IDD = IDD_DAYS };
	CComboBox	m_Align;
	CComboBox	m_Rasterizer;
	UINT	m_Y;
	UINT	m_X;
	UINT	m_W;
	UINT	m_H;
	BOOL	m_IgnoreToday;
	CString	m_Font;
	BOOL	m_Enable;
	CString	m_Bitmap;
	BOOL	m_IgnoreEvent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogDays)
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
	//{{AFX_MSG(CDialogDays)
	afx_msg void OnDaysEnable();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDaysRasterizer();
	afx_msg void OnDaysSelect();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDaysFontcolor();
	afx_msg void OnChangeDaysBitmap();
	afx_msg void OnDaysComponents10();
	afx_msg void OnDaysComponents32();
	afx_msg void OnChangeDaysH();
	afx_msg void OnDaysIgnoreToday();
	afx_msg void OnEditchangeDaysRasterizer();
	afx_msg void OnChangeDaysW();
	afx_msg void OnChangeDaysX();
	afx_msg void OnChangeDaysY();
	afx_msg void OnSelchangeDaysAlign();
	afx_msg void OnDaysBrowse();
	afx_msg void OnDaysIgnoreEvent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDAYS_H__8D4807E8_F381_40DB_A427_DEC4B20B6414__INCLUDED_)

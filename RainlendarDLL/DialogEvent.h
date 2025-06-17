#if !defined(AFX_DIALOGEVENT_H__A5328C5B_A5B5_41FE_807D_0D0DE46719BC__INCLUDED_)
#define AFX_DIALOGEVENT_H__A5328C5B_A5B5_41FE_807D_0D0DE46719BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogEvent.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogEvent dialog

class CDialogEvent : public CPropertyPage
{
	DECLARE_DYNCREATE(CDialogEvent)

// Construction
public:
	CDialogEvent();
	~CDialogEvent();

// Dialog Data
	//{{AFX_DATA(CDialogEvent)
	enum { IDD = IDD_EVENT };
	CComboBox	m_Rasterizer;
	CComboBox	m_Align;
	CString	m_Bitmap;
	BOOL	m_Enable;
	CString	m_Font;
	BOOL	m_ShowToolTips;
	BOOL	m_ShowMessageBox;
	CString	m_Execute;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogEvent)
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
	//{{AFX_MSG(CDialogEvent)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeEventAlign();
	afx_msg void OnChangeEventBitmap();
	afx_msg void OnEventBrowse();
	afx_msg void OnEventComponents1();
	afx_msg void OnEventComponents10();
	afx_msg void OnEventComponents32();
	afx_msg void OnEventEnable();
	afx_msg void OnEventFontcolor();
	afx_msg void OnChangeEventFontname();
	afx_msg void OnSelchangeEventRasterizer();
	afx_msg void OnEventSelect();
	afx_msg void OnEventTooltip();
	afx_msg void OnEventMessagebox();
	afx_msg void OnChangeEventExecute();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGEVENT_H__A5328C5B_A5B5_41FE_807D_0D0DE46719BC__INCLUDED_)

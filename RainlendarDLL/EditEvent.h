#if !defined(AFX_EDITEVENT_H__6E547BD3_BD94_4BEB_BB51_6B5D166C5DA5__INCLUDED_)
#define AFX_EDITEVENT_H__6E547BD3_BD94_4BEB_BB51_6B5D166C5DA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditEvent.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditEvent dialog

class CEditEvent : public CDialog
{
// Construction
public:
	CEditEvent(CWnd* pParent = NULL);   // standard constructor

	int GetDay() { return m_Day; };
	void SetDay(int Day ) { m_Day=Day; };

	void ReadEvent();
	void WriteEvent();

// Dialog Data

	//{{AFX_DATA(CEditEvent)
	enum { IDD = IDD_EDITEVENT };
	CString	m_Message;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditEvent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditEvent)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int m_Day;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITEVENT_H__6E547BD3_BD94_4BEB_BB51_6B5D166C5DA5__INCLUDED_)

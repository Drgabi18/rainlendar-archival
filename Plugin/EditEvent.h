/*
  Copyright (C) 2000 Kimmo Pekkola

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/EditEvent.h,v 1.1.1.1 2001/10/29 18:56:23 rainy Exp $

  $Log: EditEvent.h,v $
  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

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

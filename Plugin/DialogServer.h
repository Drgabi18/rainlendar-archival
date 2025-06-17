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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogServer.h,v 1.2 2002/01/27 16:20:45 rainy Exp $

  $Log: DialogServer.h,v $
  Revision 1.2  2002/01/27 16:20:45  rainy
  Added header comment

*/

#if !defined(AFX_DIALOGSERVER_H__6D83B014_98AB_4508_8BE4_6359FD1F7062__INCLUDED_)
#define AFX_DIALOGSERVER_H__6D83B014_98AB_4508_8BE4_6359FD1F7062__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogServer.h : header file
//

#include <afxtempl.h>
#include <afxmt.h>
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogServer dialog

class CDialogServer : public CPropertyPage
{
	DECLARE_DYNCREATE(CDialogServer)

// Construction
public:
	CDialogServer();
	~CDialogServer();

	void AddStatusString(const char* string);

// Dialog Data
	//{{AFX_DATA(CDialogServer)
	enum { IDD = IDD_SERVER };
	CListBox	m_Status;
	CString	m_Address;
	BOOL	m_Enable;
	UINT	m_Frequency;
	CString	m_ID;
	UINT	m_Port;
	BOOL	m_Startup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogServer)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateConfig();

	CCriticalSection m_CriticalSection;
	CList<CString, CString&> m_StatusStrings;
	bool m_InitWarning;

	// Generated message map functions
	//{{AFX_MSG(CDialogServer)
	afx_msg void OnServerSync();
	afx_msg void OnServerEnable();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeServerPort();
	afx_msg void OnChangeServerFrequency();
	afx_msg void OnChangeServerId();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGSERVER_H__6D83B014_98AB_4508_8BE4_6359FD1F7062__INCLUDED_)

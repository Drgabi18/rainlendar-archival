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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogGeneral.h,v 1.2 2002/01/15 17:58:42 rainy Exp $

  $Log: DialogGeneral.h,v $
  Revision 1.2  2002/01/15 17:58:42  rainy
  Removed the StartDelay

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#if !defined(AFX_DIALOGGENERAL_H__D4BD51F2_E848_4659_B582_1E8DB82B4F9C__INCLUDED_)
#define AFX_DIALOGGENERAL_H__D4BD51F2_E848_4659_B582_1E8DB82B4F9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogGeneral dialog

class CDialogGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CDialogGeneral)

// Construction
public:
	CDialogGeneral();
	~CDialogGeneral();


// Dialog Data
	//{{AFX_DATA(CDialogGeneral)
	enum { IDD = IDD_GENERAL };
	CString	m_Background;
	int		m_X;
	int		m_Y;
	BOOL	m_StartFromMonday;
	BOOL	m_StartHidden;
	BOOL	m_DisableHotkeys;
	BOOL	m_UseWindowName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogGeneral)
	public:
	virtual void OnOK();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateConfig();

	// Generated message map functions
	//{{AFX_MSG(CDialogGeneral)
	afx_msg void OnBackgroundBrowse();
	afx_msg void OnChangeX();
	afx_msg void OnChangeY();
	afx_msg void OnChangeBackground();
	afx_msg void OnStartFromMonday();
	virtual BOOL OnInitDialog();
	afx_msg void OnStartHidden();
	afx_msg void OnDisableHotkeys();
	afx_msg void OnUseWindowName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGGENERAL_H__D4BD51F2_E848_4659_B582_1E8DB82B4F9C__INCLUDED_)

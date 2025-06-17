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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogYear.h,v 1.1.1.1 2001/10/29 18:56:23 rainy Exp $

  $Log: DialogYear.h,v $
  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

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

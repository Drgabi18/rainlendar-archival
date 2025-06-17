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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogToday.h,v 1.1.1.1 2001/10/29 18:56:23 rainy Exp $

  $Log: DialogToday.h,v $
  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#if !defined(AFX_DIALOGTODAY_H__FD0FDD9C_7FFB_4CFB_B42B_A62024965CE8__INCLUDED_)
#define AFX_DIALOGTODAY_H__FD0FDD9C_7FFB_4CFB_B42B_A62024965CE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogToday.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogToday dialog

class CDialogToday : public CPropertyPage
{
	DECLARE_DYNCREATE(CDialogToday)

// Construction
public:
	CDialogToday();
	~CDialogToday();

// Dialog Data
	//{{AFX_DATA(CDialogToday)
	enum { IDD = IDD_TODAY };
	CComboBox	m_Rasterizer;
	CComboBox	m_Align;
	CString	m_Bitmap;
	BOOL	m_Enable;
	CString	m_Font;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogToday)
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
	//{{AFX_MSG(CDialogToday)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnTodayEnable();
	afx_msg void OnSelchangeTodayRasterizer();
	afx_msg void OnSelchangeTodayAlign();
	afx_msg void OnChangeTodayBitmap();
	afx_msg void OnTodayComponents1();
	afx_msg void OnTodayComponents10();
	afx_msg void OnTodayComponents32();
	afx_msg void OnTodayFontcolor();
	afx_msg void OnChangeTodayFontname();
	afx_msg void OnTodaySelect();
	afx_msg void OnTodayBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGTODAY_H__FD0FDD9C_7FFB_4CFB_B42B_A62024965CE8__INCLUDED_)

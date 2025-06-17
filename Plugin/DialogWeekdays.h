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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogWeekdays.h,v 1.1.1.1 2001/10/29 18:56:23 rainy Exp $

  $Log: DialogWeekdays.h,v $
  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

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

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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogEvent.h,v 1.1.1.1 2001/10/29 18:56:23 rainy Exp $

  $Log: DialogEvent.h,v $
  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

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

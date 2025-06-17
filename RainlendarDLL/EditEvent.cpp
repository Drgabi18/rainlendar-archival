// EditEvent.cpp : implementation file
//

#include "stdafx.h"
#include "rainlendardll.h"
#include "CalendarWindow.h"
#include "EditEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditEvent dialog


CEditEvent::CEditEvent(CWnd* pParent /*=NULL*/)
	: CDialog(CEditEvent::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditEvent)
	m_Message = _T("");
	//}}AFX_DATA_INIT
}


void CEditEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditEvent)
	DDX_Text(pDX, IDC_EDITEVENT_MESSAGE, m_Message);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditEvent, CDialog)
	//{{AFX_MSG_MAP(CEditEvent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditEvent message handlers

BOOL CEditEvent::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString Caption;
	Caption.Format("%i-%s-%i", m_Day, CCalendarWindow::C_CurrentDate.Format("%B"), CCalendarWindow::C_CurrentDate.GetYear());
	SetWindowText(Caption);

	ReadEvent();

	GetDlgItem(IDC_EDITEVENT_MESSAGE)->SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* 
** ReadEvent
**
** Reads the Event for selected day
**
*/
void CEditEvent::ReadEvent()
{
	char tmpSz[MAX_LINE_LENGTH];
	char IniPath[MAX_LINE_LENGTH];
	CTime Current=CCalendarWindow::C_CurrentDate;
	CString Date;
	char* Slash;

	// Get the DLL's directory
	GetModuleFileName(GetModuleHandle("Rainlendar.dll"), IniPath, MAX_LINE_LENGTH);

	Slash=strrchr(IniPath, '\\');
	if(Slash==NULL) {
		strcpy(IniPath, "Events.ini");
	} else {
		strcpy(Slash, "\\Events.ini");
	}

	Date.Format("%i-%i-%i", m_Day, Current.GetMonth(), Current.GetYear());

	if(GetPrivateProfileString( Date, "Message", "", tmpSz, 255, IniPath) > 0) {
		m_Message=tmpSz;
		m_Message.Replace("\\n", "\r\n");
		UpdateData(FALSE);
	}
} 

/* 
** WriteEvent
**
** Writes the Event for selected day
**
*/
void CEditEvent::WriteEvent()
{
	char IniPath[MAX_LINE_LENGTH];
	CTime Current=CCalendarWindow::C_CurrentDate;
	CString Date;
	char* Slash;

	// Get the DLL's directory
	GetModuleFileName(GetModuleHandle("Rainlendar.dll"), IniPath, MAX_LINE_LENGTH);

	Slash=strrchr(IniPath, '\\');
	if(Slash==NULL) {
		strcpy(IniPath, "Events.ini");
	} else {
		strcpy(Slash, "\\Events.ini");
	}

	Date.Format("%i-%i-%i", m_Day, Current.GetMonth(), Current.GetYear());

	m_Message.Replace("\n", "\\n");
	m_Message.Replace("\r", "");
	WritePrivateProfileString( Date, "Message", m_Message, IniPath );
} 

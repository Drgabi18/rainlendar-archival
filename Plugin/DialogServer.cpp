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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/DialogServer.cpp,v 1.3 2002/01/29 17:35:08 rainy Exp $

  $Log: DialogServer.cpp,v $
  Revision 1.3  2002/01/29 17:35:08  rainy
  The strings are added to the listbox only if the dialog exists.

  Revision 1.2  2002/01/27 16:20:45  rainy
  Added header comment

*/

#include "stdafx.h"
#include "rainlendardll.h"
#include "DialogServer.h"
#include "CalendarWindow.h"
#include "NetworkThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogServer property page

IMPLEMENT_DYNCREATE(CDialogServer, CPropertyPage)

CDialogServer::CDialogServer() : CPropertyPage(CDialogServer::IDD)
{
	//{{AFX_DATA_INIT(CDialogServer)
	m_Address = _T("");
	m_Enable = FALSE;
	m_Frequency = 0;
	m_ID = _T("");
	m_Port = 0;
	m_Startup = FALSE;
	//}}AFX_DATA_INIT

	m_InitWarning = true;
}

CDialogServer::~CDialogServer()
{
}

void CDialogServer::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogServer)
	DDX_Control(pDX, IDC_SERVER_STATUS, m_Status);
	DDX_Text(pDX, IDC_SERVER_ADDRESS, m_Address);
	DDX_Check(pDX, IDC_SERVER_ENABLE, m_Enable);
	DDX_Text(pDX, IDC_SERVER_FREQUENCY, m_Frequency);
	DDX_Text(pDX, IDC_SERVER_ID, m_ID);
	DDX_Text(pDX, IDC_SERVER_PORT, m_Port);
	DDX_Check(pDX, IDC_SERVER_STARTUP, m_Startup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogServer, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogServer)
	ON_BN_CLICKED(IDC_SERVER_SYNC, OnServerSync)
	ON_BN_CLICKED(IDC_SERVER_ENABLE, OnServerEnable)
	ON_EN_CHANGE(IDC_SERVER_PORT, OnChangeServerPort)
	ON_EN_CHANGE(IDC_SERVER_FREQUENCY, OnChangeServerFrequency)
	ON_EN_CHANGE(IDC_SERVER_ID, OnChangeServerId)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogServer message handlers

void CDialogServer::UpdateConfig()
{
	CCalendarWindow::c_Config.SetServerEnable(m_Enable==TRUE);
	CCalendarWindow::c_Config.SetServerAddress(m_Address);
	CCalendarWindow::c_Config.SetServerPort(m_Port);
	CCalendarWindow::c_Config.SetServerFrequency(m_Frequency);
	CCalendarWindow::c_Config.SetServerID(m_ID);
	CCalendarWindow::c_Config.SetServerStartup(m_Startup==TRUE);
}

void CDialogServer::OnServerSync() 
{
	static NetworkParams param;
	CString string;

	UpdateData();

	// Fetch the CCalendarWindow
	CWnd* Parent=GetParent();
	Parent = Parent->GetParent();

	// Here is a slight possibility of unsafe thread access
	// If the OnServerSync() is started multiple times before the
	// thread has a chance to copy the information, the data might
	// corrupt. Hopefully this doesn't occur too often :-)
	param.dialogServer = this;
	param.serverPort = m_Port;
	param.serverAddress = m_Address;
	param.userID = m_ID;
	param.requestType = REQUEST_SYNCRONIZATION;
	param.window = Parent->GetSafeHwnd();

	// Launch the network communication thread
	AfxBeginThread(NetworkThreadProc, &param);
}

void CDialogServer::OnServerEnable() 
{
	UpdateData();
	
	if(m_Enable) 
	{
		if(m_InitWarning)
		{
			// Show a warning first
			MessageBox("Warning!\n\nThis is a very experimental feature.\n"
						"You need to have some basic knowledge \n"
						"how client-server systems work and also\n"
						"how to set up a server in your computer.\n\n"
						"It is also possible that in some situations\n"
						"using this feature you might lose all your\n"
						"entries in the events.ini -file.\n\n"
						"So, consider yourself warned!\n"
						"And read the docs before trying this!",
						"Rainlendar", MB_OK);
		}

		GetDlgItem(IDC_SERVER_STATUS)->EnableWindow(true);
		GetDlgItem(IDC_SERVER_ADDRESS)->EnableWindow(true);
		GetDlgItem(IDC_SERVER_FREQUENCY)->EnableWindow(true);
		GetDlgItem(IDC_SERVER_ID)->EnableWindow(true);
		GetDlgItem(IDC_SERVER_PORT)->EnableWindow(true);
		GetDlgItem(IDC_SERVER_SYNC)->EnableWindow(true);
		GetDlgItem(IDC_SERVER_STARTUP)->EnableWindow(true);
	} 
	else 
	{
		GetDlgItem(IDC_SERVER_STATUS)->EnableWindow(false);
		GetDlgItem(IDC_SERVER_ADDRESS)->EnableWindow(false);
		GetDlgItem(IDC_SERVER_FREQUENCY)->EnableWindow(false);
		GetDlgItem(IDC_SERVER_ID)->EnableWindow(false);
		GetDlgItem(IDC_SERVER_PORT)->EnableWindow(false);
		GetDlgItem(IDC_SERVER_SYNC)->EnableWindow(false);
		GetDlgItem(IDC_SERVER_STARTUP)->EnableWindow(false);
	}
}

BOOL CDialogServer::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CSingleLock lock(&m_CriticalSection);
	lock.Lock();
	if(lock.IsLocked())
	{
		// We'll skip this if the critical section cannot be locked
		// Probably should just wait for it the get unlocked, but 
		// this is not so critical place.

		POSITION pos = m_StatusStrings.GetHeadPosition();
		for (int i = 0; i < m_StatusStrings.GetCount(); i++)
		{
			m_Status.AddString(m_StatusStrings.GetNext(pos));
		}
		lock.Unlock();
	}

	m_Enable = CCalendarWindow::c_Config.GetServerEnable();
	m_Address = CCalendarWindow::c_Config.GetServerAddress();
	m_Port = CCalendarWindow::c_Config.GetServerPort();
	m_Frequency =  CCalendarWindow::c_Config.GetServerFrequency();
	m_ID = CCalendarWindow::c_Config.GetServerID();
	m_Startup = CCalendarWindow::c_Config.GetServerStartup();

	UpdateData(FALSE);

	m_InitWarning = false;	// Disable the warning message
	OnServerEnable();
	m_InitWarning = true;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogServer::AddStatusString(const char* string) 
{
	CString text;
	CTime t = CTime::GetCurrentTime();
	text.Format("[%02i:%02i:%02i] ", t.GetHour(), t.GetMinute(), t.GetSecond());
	text += string;

	CSingleLock lock(&m_CriticalSection);
	lock.Lock();
	if(lock.IsLocked())
	{
		// We'll skip this if the critical section cannot be locked
		if (IsWindow(this->GetSafeHwnd()) && IsWindowVisible())
		{
			m_Status.AddString(text);
			int count = m_Status.GetCount();
			if (count > 0) m_Status.SetCurSel(count-1);
		}
		m_StatusStrings.AddTail(text);
		lock.Unlock();
	}
}

void CDialogServer::OnChangeServerPort() 
{
	UpdateData();
}

void CDialogServer::OnChangeServerFrequency() 
{
	UpdateData();
}

void CDialogServer::OnOK() 
{
	UpdateConfig();
	
	CPropertyPage::OnOK();
}

void CDialogServer::OnChangeServerId() 
{
	UpdateData();

	// Check the ID so that it doesn't contain illegal chars
	for (int i = 0; i < m_ID.GetLength(); i++)
	{
		if ((m_ID[i] < '0') || 
		    (m_ID[i] > '9' && m_ID[i] < 'A') ||
		    (m_ID[i] > 'Z' && m_ID[i] < 'a') ||
		    (m_ID[i] > 'z'))
		{
			MessageBox("Please use only numbers (0-9)\nand letters (A-Z, a-z)", "Rainlendar", MB_OK);
			m_ID = "";
			UpdateData(FALSE);
			return;
		}
	}
}

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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/NetworkThread.h,v 1.2 2002/01/27 16:20:46 rainy Exp $

  $Log: NetworkThread.h,v $
  Revision 1.2  2002/01/27 16:20:46  rainy
  Added header comment

*/

#if !defined(AFX_NETWORKTHREAD_H__2ABC4B7C_B95B_485E_AB90_A49741C7B4BD__INCLUDED_)
#define AFX_NETWORKTHREAD_H__2ABC4B7C_B95B_485E_AB90_A49741C7B4BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogServer.h"
#include <clientconnector.h>

// requestTypes
enum
{
	REQUEST_SYNCRONIZATION,		// Syncs the events with client and server
	REQUEST_SENDEVENTS,			// Sends events to server
	REQUEST_GETEVENTS			// Gets events from server
};

struct NetworkParams
{
	CString userID;
	CString serverAddress;
	UINT serverPort;
	CDialogServer* dialogServer;
	int requestType;
	HWND window;
};

UINT NetworkThreadProc(LPVOID pParam);

#endif // !defined(AFX_NETWORKTHREAD_H__2ABC4B7C_B95B_485E_AB90_A49741C7B4BD__INCLUDED_)

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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/NetworkThread.h,v 1.4 2002/05/30 18:26:03 rainy Exp $

  $Log: NetworkThread.h,v $
  Revision 1.4  2002/05/30 18:26:03  rainy
  The logging stuff is back.

  Revision 1.3  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.2  2002/01/27 16:20:46  rainy
  Added header comment

*/

#ifndef __NETWORKTHREAD_H__
#define __NETWORKTHREAD_H__

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
	std::string userID;
	std::string serverAddress;
	UINT serverPort;
	int requestType;
	HWND window;
};

DWORD WINAPI NetworkThreadProc(LPVOID pParam);

#endif

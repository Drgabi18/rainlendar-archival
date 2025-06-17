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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Server/RainlendarServer.h,v 1.1 2002/01/27 16:21:05 rainy Exp $

  $Log: RainlendarServer.h,v $
  Revision 1.1  2002/01/27 16:21:05  rainy
  Initial version

*/

#if !defined(AFX_RAINLENDARSERVER_H__3EB0EB20_FC36_4EFF_926E_2A98EFCD97BB__INCLUDED_)
#define AFX_RAINLENDARSERVER_H__3EB0EB20_FC36_4EFF_926E_2A98EFCD97BB__INCLUDED_

#include <simpleserver.h>
#include "EventCombiner.h"

class CRainlendarServer : public ssobjects::SimpleServer  
{
public:
	CRainlendarServer(ssobjects::SockAddr& saBind);
	virtual ~CRainlendarServer();

protected:
    virtual void processSingleMsg(ssobjects::PacketMessage* pmsg);
    virtual void idle(unsigned32 nTimer);
	void Reset();

	bool m_Busy;
	bool m_NewEventsFound;
	CEventCombiner* m_Combiner;
	ssobjects::CStr m_IniFile;
	unsigned32 m_EventCount;

	unsigned32 m_BusyTime;
};

#endif // !defined(AFX_RAINLENDARSERVER_H__3EB0EB20_FC36_4EFF_926E_2A98EFCD97BB__INCLUDED_)

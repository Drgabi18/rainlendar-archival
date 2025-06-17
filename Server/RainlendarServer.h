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
  $Header: /home/cvsroot/Rainlendar/Server/RainlendarServer.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: RainlendarServer.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.4  2005/03/01 17:02:52  rainy
  Path for the files is selectable.

  Revision 1.3  2004/11/14 11:38:13  rainy
  Changes for 0.20.

  Revision 1.2  2003/05/25 15:13:13  Rainy
  Added fltering support.

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
	CRainlendarServer(ssobjects::SockAddr& saBind, CIPFilter* filter, ssobjects::CStr& targetPath);
	virtual ~CRainlendarServer();

protected:
    virtual void processSingleMsg(ssobjects::PacketMessage* pmsg);
    virtual void idle(unsigned32 nTimer);
	void Reset();

	bool m_Busy;
	bool m_NewEventsFound;
	CEventCombiner* m_Combiner;
	ssobjects::CStr m_IniFile;
	ssobjects::CStr m_TargetPath;
	unsigned32 m_EventCount;

	unsigned32 m_BusyTime;
	
	CIPFilter* m_Filter;

	std::vector<GUID> m_RequestedEvents;
};

#endif // !defined(AFX_RAINLENDARSERVER_H__3EB0EB20_FC36_4EFF_926E_2A98EFCD97BB__INCLUDED_)

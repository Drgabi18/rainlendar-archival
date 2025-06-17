 /*
  Copyright (C) 2003 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Server/IPFilter.h,v 1.3 2004/11/14 11:37:46 rainy Exp $

  $Log: IPFilter.h,v $
  Revision 1.3  2004/11/14 11:37:46  rainy
  no message

  Revision 1.2  2003/05/25 15:12:20  Rainy
  Added possibility to get the last rule from the class.

  Revision 1.1  2003/05/24 20:06:44  Rainy
  Initial version.

*/

#if !defined(_IPFILTER_H_)
#define _IPFILTER_H_

#include "IPNumber.h"
#include <simpleserver.h>

struct FilterItem
{
	CIPNumber ip;
	ssobjects::CStr profiles;
};

class CIPFilter
{
public:
	CIPFilter();
	~CIPFilter();

	bool Parse(const ssobjects::CStr& filterFile);
	bool CheckIncoming(const CIPNumber& ip, const char* profile);
	bool CheckOutgoing(const CIPNumber& ip, const char* profile);
	const char* GetRule();

private:
	bool Check(const CIPNumber& ip, const char* profile, ssobjects::LinkedList<FilterItem>& accept, ssobjects::LinkedList<FilterItem>& deny);
	FilterItem* ParseItem(const char* buffer);

	ssobjects::CStr m_LastRule;
	ssobjects::LinkedList<FilterItem> m_AcceptIncoming;
	ssobjects::LinkedList<FilterItem> m_DenyIncoming;
	ssobjects::LinkedList<FilterItem> m_AcceptOutgoing;
	ssobjects::LinkedList<FilterItem> m_DenyOutgoing;
};

#endif // !defined(_IPFILTER_H_)

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
  $Header: /home/cvsroot/Rainlendar/Server/IPNumber.h,v 1.4 2004/12/05 18:17:13 rainy Exp $

  $Log: IPNumber.h,v $
  Revision 1.4  2004/12/05 18:17:13  rainy
  no message

  Revision 1.3  2003/08/09 15:22:42  Rainy
  Added string contructor.
  Mask is also stored as plain number.

  Revision 1.2  2003/05/25 15:12:41  Rainy
  Added GetString and comparison with the ip number.

  Revision 1.1  2003/05/24 20:06:44  Rainy
  Initial version.

*/

#if !defined(_IPNUMBER_H_)
#define _IPNUMBER_H_

#ifndef ULONG
typedef unsigned long ULONG;
#endif

class CIPNumber
{
public:
	CIPNumber();
	CIPNumber(ULONG ip, int mask);
	CIPNumber(const char* ipString);
	CIPNumber(unsigned char a, unsigned char b, unsigned char c, unsigned char d, int mask);
	~CIPNumber();

	bool Compare(const CIPNumber& ip);
	bool Compare(ULONG ip);
	const char* GetString();

private:
	ULONG m_MaskBits;
	ULONG m_Mask;
	ULONG m_IP;
};

#endif // !defined(_IPNUMBER_H_)

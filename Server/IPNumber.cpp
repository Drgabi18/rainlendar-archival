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
  $Header: /home/cvsroot/Rainlendar/Server/IPNumber.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: IPNumber.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.5  2004/12/05 18:17:13  rainy
  no message

  Revision 1.4  2003/08/09 15:22:42  Rainy
  Added string contructor.
  Mask is also stored as plain number.

  Revision 1.3  2003/05/25 18:12:45  Rainy
  The mask is set correctly now.

  Revision 1.2  2003/05/25 15:12:41  Rainy
  Added GetString and comparison with the ip number.

  Revision 1.1  2003/05/24 20:06:44  Rainy
  Initial version.

*/

#include "IPNumber.h"
#include <stdio.h>

CIPNumber::CIPNumber()
{
	m_IP = 0;
	m_Mask = 0;
	m_MaskBits = 0;
}

CIPNumber::CIPNumber(ULONG ip, int mask)
{
	if (mask == 0)
	{
		m_Mask = 0;
		m_MaskBits = 0;
	}
	else
	{
		m_Mask = mask;
		m_MaskBits = ~((1 << (32 - mask)) - 1);
	}
	m_IP = ip;
}

CIPNumber::CIPNumber(const char* ipString)
{
	if (ipString == NULL)
	{
		m_IP = 0;
		m_Mask = 0;
		m_MaskBits = 0;
	}
	else
	{
		int a, b, c, d, mask;
		sscanf(ipString, "%i.%i.%i.%i/%i", &a, &b, &c, &d, &mask);
		m_IP = (a << 24) | (b << 16) | (c << 8) | (d);
		m_Mask = mask;
		m_MaskBits = ~((1 << (32 - mask)) - 1);
	}
}

CIPNumber::CIPNumber(unsigned char a, unsigned char b, unsigned char c, unsigned char d, int mask)
{
	if (mask == 0)
	{
		m_Mask = 0;
		m_MaskBits = 0;
	}
	else
	{
		m_Mask = mask;
		m_MaskBits = ~((1 << (32 - mask)) - 1);
	}
	m_IP = (a << 24) | (b << 16) | (c << 8) | (d);
}

CIPNumber::~CIPNumber()
{
}

bool CIPNumber::Compare(const CIPNumber& ip)
{
	// Use this IP's mask for the comparison
	return (m_IP & m_MaskBits) == (ip.m_IP & m_MaskBits);
}

bool CIPNumber::Compare(ULONG ip)
{
	// Use this IP's mask for the comparison
	return (m_IP & m_MaskBits) == (ip & m_MaskBits);
}

const char* CIPNumber::GetString()
{
	static char buffer[256];
	sprintf(buffer, "%u.%u.%u.%u/%u", (m_IP >> 24) & 0xFF, (m_IP >> 16) & 0xFF, (m_IP >> 8) & 0xFF, m_IP & 0xFF, m_Mask);
	return buffer;
}

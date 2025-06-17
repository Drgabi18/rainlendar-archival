/*
  Copyright (C) 2001 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Library/Error.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: Error.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.4  2004/11/06 13:38:58  rainy
  no message

  Revision 1.3  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.2  2003/06/15 09:46:06  Rainy
  Strings are read from CLanguage class.

  Revision 1.1  2002/05/30 18:27:33  rainy
  Initial version

*/

#pragma warning(disable: 4786)
#pragma warning(disable: 4996)

#include "Error.h"
#include "CalendarWindow.h"
#include <stdio.h>

/* 
** GetString
**
** Returns the error string
**
*/
const std::string& CError::GetString()
{
	static char Buffer[16];

	if (m_Error != ERR_USER) 
	{
		m_String = CCalendarWindow::c_Language.GetString("Error", m_Error);
		if (m_File) 
		{
			sprintf(Buffer, "%i", m_Line);

			m_String += "\n(";
			m_String += m_File;
			m_String += " : ";
			m_String += Buffer;
			m_String += ")";
		}
	}

	return m_String; 
}

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
  $Header: /home/cvsroot/Rainlendar/Plugin/Error.h,v 1.7 2004/11/06 13:38:58 rainy Exp $

  $Log: Error.h,v $
  Revision 1.7  2004/11/06 13:38:58  rainy
  no message

  Revision 1.6  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.5  2003/06/15 09:46:06  Rainy
  Strings are read from CLanguage class.

  Revision 1.4  2002/08/24 11:12:32  rainy
  Errors are also written to the log file.

  Revision 1.3  2002/05/30 18:26:26  rainy
  Added WIN32_LEAN_AND_MEAN

  Revision 1.2  2002/05/23 17:33:41  rainy
  Removed all MFC stuff

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __ERROR_H__
#define __ERROR_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <string>
#include "Litestep.h"

#define THROW(err) \
	{ \
		CError error(err); \
		DebugLog(error.GetString().c_str()); \
		throw CError(err, __LINE__, __FILE__); \
	}

// Few predefined errors
enum RAINERROR 
{
	ERR_USER,
	ERR_OUTOFMEM,
	ERR_WINDOWCLASS,
	ERR_WINDOW,
	ERR_BACKGROUND,
	ERR_CREATEFONT,
	ERR_TEXTDIMENSIONS,
	ERR_NULLPARAMETER
};

class CError
{
public:

    CError(const std::string& String) { m_Error = ERR_USER; m_String = String; m_File = NULL; };
    CError(const char* String ) { m_Error = ERR_USER; m_String = String; m_File = NULL; };
    CError(const std::string& String, int Line, const char* File) { m_Error = ERR_USER; m_String = String; m_Line = Line; m_File = File; };
    CError(const char* String, int Line, const char* File) { m_Error = ERR_USER; m_String = String; m_Line = Line; m_File = File; };
    CError(RAINERROR Error) { m_Error = Error; m_File = NULL; };
    CError(RAINERROR Error, int Line, const char* File) { m_Error = Error; m_Line = Line; m_File = File; };

    const std::string& GetString(); 

private:
	std::string m_String;
	int m_Line;
	const char* m_File;
	RAINERROR m_Error;
};

#endif

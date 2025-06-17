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
  $Header: /home/cvsroot/Rainlendar/Library/Rasterizer.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: Rasterizer.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.6  2004/11/06 13:38:59  rainy
  no message

  Revision 1.5  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.4  2002/08/03 16:09:11  rainy
  Added support for profiles.

  Revision 1.3  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.2  2002/02/27 18:49:52  rainy
  Added mode alignments

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "Rasterizer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRasterizer::CRasterizer()
{
	m_Width=0;
	m_Height=0;
	m_Align = (ALIGN)(ALIGN_HCENTER + ALIGN_VCENTER);
	m_Profile = NULL;
}

CRasterizer::~CRasterizer()
{

}

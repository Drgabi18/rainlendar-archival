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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Rasterizer.cpp,v 1.2 2002/02/27 18:49:52 rainy Exp $

  $Log: Rasterizer.cpp,v $
  Revision 1.2  2002/02/27 18:49:52  rainy
  Added mode alignments

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "stdafx.h"
#include "RainlendarDLL.h"
#include "Rasterizer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRasterizer::CRasterizer()
{
	m_Width=0;
	m_Height=0;
	m_Align = (ALIGN)(ALIGN_HCENTER + ALIGN_VCENTER);
}

CRasterizer::~CRasterizer()
{

}

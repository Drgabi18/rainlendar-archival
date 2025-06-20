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
  $Header: /home/cvsroot/Rainlendar/Library/Rasterizer.h,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: Rasterizer.h,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.10  2004/11/06 13:38:59  rainy
  no message

  Revision 1.9  2004/06/10 16:22:25  rainy
  New address to repository.

  Revision 1.8  2003/08/09 15:27:21  Rainy
  Comment changed

  Revision 1.7  2002/11/12 18:10:37  rainy
  The interface of Paint changed a little.

  Revision 1.6  2002/08/03 16:09:10  rainy
  Added support for profiles.

  Revision 1.5  2002/05/30 18:24:17  rainy
  Added WIN32_LEAN_AND_MEAN

  Revision 1.4  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.3  2002/02/27 18:49:52  rainy
  Added mode alignments

  Revision 1.2  2002/01/10 16:41:17  rainy
  Added vertical positioning.

  Revision 1.1.1.1  2001/10/29 18:56:23  rainy
  Moved to CVS

*/

#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include "Image.h"

struct Profile;

class CRasterizer  
{
public:
	enum ALIGN
	{
		// The order (and values) must be like this so that it's backwards compatible
		ALIGN_LEFT    = 0x00,
		ALIGN_RIGHT   = 0x01,
		ALIGN_HCENTER = 0x02,
		ALIGN_TOP     = 0x00,
		ALIGN_VCENTER = 0x10,
		ALIGN_BOTTOM  = 0x20
	};

	enum TYPE 
	{
		TYPE_NONE,
		TYPE_BITMAP,
		TYPE_FONT
	};

	CRasterizer();
	virtual ~CRasterizer();

	virtual CRasterizer::TYPE GetType() = 0;

	int GetAlign() { return m_Align; };
	void SetAlign(int Align ) { m_Align=Align; };

	virtual void Paint(CImage& background, int X, int Y, int W, int H, int Index) = 0;

	int GetHeight() { return m_Height; };
	int GetWidth() { return m_Width; };

	void SetProfile(const Profile* profile) { m_Profile = profile; };
	const Profile* GetProfile() { return m_Profile; };
	
protected:
	int m_Align;
	int m_Width;
	int m_Height;
	const Profile* m_Profile;
};

#endif

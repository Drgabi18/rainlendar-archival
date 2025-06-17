/*
  Copyright (C) 2002 Kimmo Pekkola

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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/Image.cpp,v 1.1 2002/11/12 18:00:36 rainy Exp $

  $Log: Image.cpp,v $
  Revision 1.1  2002/11/12 18:00:36  rainy
  Initial version

*/

#include "Image.h"
#include "Litestep.h"
#include <png.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImage::CImage()
{
	m_Width = 0;
	m_Height = 0;
	m_Alpha = false;
	m_Image = NULL;
	m_Bits = NULL;
}

CImage::~CImage()
{
	Clear();
}

void CImage::Clear()
{
	if (m_Image) DeleteObject(m_Image);

	m_Width = 0;
	m_Height = 0;
	m_Alpha = false;
	m_Image = NULL;
	m_Bits = NULL;
}

/* 
** Load
**
** Loads the given image. This method supports BMPs and PNGs.
** Separate Alpha BMPs are supported as well.
**
*/
bool CImage::Load(const std::string& filename)
{
	HBITMAP alpha = NULL;
	HBITMAP m_Image = (HBITMAP)LoadImage(NULL, filename.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);

	if(m_Image == NULL) return LoadPNG(filename);

	BITMAP bm;
	GetObject(m_Image, sizeof(BITMAP), &bm);
	m_Width = bm.bmWidth;
	m_Height = bm.bmHeight;

	// Check for Alpha-map
	int pos = filename.rfind('.');
	if (pos != -1)
	{
		std::string alphaName;

		alphaName = filename;
		alphaName.insert(pos, "-Alpha");

		alpha = (HBITMAP)LoadImage(NULL, alphaName.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);

		if(alpha != NULL) 
		{
			// Check that it is correct size
			GetObject(alpha, sizeof(BITMAP), &bm);
			if (m_Width != bm.bmWidth || m_Height != bm.bmHeight)
			{
				DeleteObject(alpha);
				return true;
			}
		}
	}

	char logMsg[1024];
	sprintf(logMsg, "Loaded image: %s (W: %i, H: %i) %s", filename.c_str(), m_Width, m_Height, alpha?"Alpha":"No Alpha");
	LSLog(LOG_DEBUG, "Rainlendar", logMsg);

	Create(m_Image, alpha, 0x0ff);

	return true;
}

bool CImage::LoadPNG(const std::string& filename)
{
	png_struct    *png_ptr = NULL;
	png_info	  *info_ptr = NULL;
	png_byte      buf[8];
	png_byte      *png_pixels = NULL;
	png_byte      **row_pointers = NULL;
	png_byte      *pix_ptr = NULL;
	png_uint_32   row_bytes;
	
	png_uint_32   width;
	png_uint_32   height;
	int           bit_depth;
	int           channels;
	int           color_type;
	int           alpha_present;
	int           row, col;
	int           ret;
	int           i;
	
	FILE* png_file = fopen(filename.c_str(), "rb");

	/* read and check signature in PNG file */
	ret = fread (buf, 1, 8, png_file);
	if (ret != 8)
	{
		fclose(png_file);
		return false;
	}

	ret = png_check_sig (buf, 8);
	if (!ret)
	{
		fclose(png_file);
		return false;
	}
	
	/* create png and info structures */
	
	png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fclose(png_file);
		return false;
	}
	
	info_ptr = png_create_info_struct (png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct (&png_ptr, NULL, NULL);
		fclose(png_file);
		return false;
	}
	
	if (setjmp (png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
		fclose(png_file);
		return false;
	}
	
	/* set up the input control for C streams */
	png_init_io (png_ptr, png_file);
	png_set_sig_bytes (png_ptr, 8);  /* we already read the 8 signature bytes */
	
	/* read the file information */
	png_read_info (png_ptr, info_ptr);
	
	/* get size and bit-depth of the PNG-image */
	png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
	
	/* set-up the transformations */
	
	/* transform paletted images into full-color rgb */
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand (png_ptr);

	/* expand images to bit-depth 8 (only applicable for grayscale images) */
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand (png_ptr);

	/* transform transparency maps into full alpha-channel */
	if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_expand (png_ptr);

	/* downgrade 16-bit images to 8 bit */
	if (bit_depth == 16) png_set_strip_16 (png_ptr);

	/* Need to switch from RGB to BGR */
	png_set_bgr(png_ptr);

	/* all transformations have been registered; now update info_ptr data,
 	 * get rowbytes and channels, and allocate image memory */
	
	png_read_update_info (png_ptr, info_ptr);
	
	/* get the new color-type and bit-depth (after expansion/stripping) */
	png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
	
	/* calculate new number of channels and store alpha-presence */
	if (color_type == PNG_COLOR_TYPE_GRAY)
		channels = 1;
	else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		channels = 2;
	else if (color_type == PNG_COLOR_TYPE_RGB)
		channels = 3;
	else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		channels = 4;
	else
		channels = 0; /* should never happen */

	alpha_present = (channels - 1) % 2;
	
	/* row_bytes is the width x number of channels x (bit-depth / 8) */
	row_bytes = png_get_rowbytes (png_ptr, info_ptr);
	
	if ((png_pixels = (png_byte *) malloc (row_bytes * height * sizeof (png_byte))) == NULL) 
	{
		png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
		fclose(png_file);
		return false;
	}
	
	if ((row_pointers = (png_byte **) malloc (height * sizeof (png_bytep))) == NULL)
	{
		png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
		free (png_pixels);
		png_pixels = NULL;
		fclose(png_file);
		return false;
	}
	
	/* set the individual row_pointers to point at the correct offsets */
	for (i = 0; i < (height); i++)
	{
		row_pointers[i] = png_pixels + i * row_bytes;
	}

	/* now we can go ahead and just read the whole image */
	png_read_image (png_ptr, row_pointers);
	
	/* read rest of file, and get additional chunks in info_ptr - REQUIRED */
	png_read_end (png_ptr, info_ptr);
	
	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp) NULL);
	
	Create(width, height, 0x0);	// Create and empty bitmap

	pix_ptr = png_pixels;
	
	for (row = height - 1; row >= 0; row--)
	{
		for (col = 0; col < width; col++)
		{
			switch (channels)
			{
			case 1:		// Gray + no alpha
				m_Bits[row * width * 4 + col * 4 + 0] = *pix_ptr;
				m_Bits[row * width * 4 + col * 4 + 1] = *pix_ptr;
				m_Bits[row * width * 4 + col * 4 + 2] = *pix_ptr++;
				m_Bits[row * width * 4 + col * 4 + 3] = 255;
				break;
			case 2:		// Gray + alpha
				m_Bits[row * width * 4 + col * 4 + 0] = *pix_ptr;
				m_Bits[row * width * 4 + col * 4 + 1] = *pix_ptr;
				m_Bits[row * width * 4 + col * 4 + 2] = *pix_ptr++;
				m_Bits[row * width * 4 + col * 4 + 3] = *pix_ptr++;
				break;
			case 3:		// RGB + no alpha
				m_Bits[row * width * 4 + col * 4 + 0] = *pix_ptr++;
				m_Bits[row * width * 4 + col * 4 + 1] = *pix_ptr++;
				m_Bits[row * width * 4 + col * 4 + 2] = *pix_ptr++;
				m_Bits[row * width * 4 + col * 4 + 3] = 255;
				break;
			case 4:		// RGB + alpha
				m_Bits[row * width * 4 + col * 4 + 0] = *pix_ptr++;
				m_Bits[row * width * 4 + col * 4 + 1] = *pix_ptr++;
				m_Bits[row * width * 4 + col * 4 + 2] = *pix_ptr++;
				m_Bits[row * width * 4 + col * 4 + 3] = *pix_ptr++;
				break;
			}
		} /* end for col */
	} /* end for row */
	
	if (row_pointers != (unsigned char**) NULL)	free (row_pointers);
	if (png_pixels != (unsigned char*) NULL) free (png_pixels);

	fclose(png_file);

	if (alpha_present)
	{
		MultiplyAlpha();
	}
	else
	{
		m_Alpha = false;
	}
		
	char logMsg[1024];
	sprintf(logMsg, "Loaded image: %s (W: %i, H: %i) %s", filename.c_str(), m_Width, m_Height, m_Alpha?"Alpha":"No Alpha");
	LSLog(LOG_DEBUG, "Rainlendar", logMsg);

	return TRUE;
}

/*
** Resize
**
** Resizes the image
*/
void CImage::Resize(int w, int h, IMAGE_RESIZE_TYPE type)
{
	if (m_Image == NULL) return;

	HDC currentDC = CreateCompatibleDC(NULL);
	HDC newDC = CreateCompatibleDC(NULL);

	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = w * h * 4;
	
	// create our DIB section and select the bitmap into the dc
	VOID* bits;
	HBITMAP bitmap = CreateDIBSection(currentDC, &bmi, DIB_RGB_COLORS, &bits, NULL, 0x0);

	HBITMAP oldCurrentBitmap = (HBITMAP)SelectObject(currentDC, m_Image);
	HBITMAP oldNewBitmap = (HBITMAP)SelectObject(newDC, bitmap);

	if (type == IMAGE_RESIZE_TYPE_STRETCH)
	{
		if (m_Alpha)
		{
			BLENDFUNCTION blendPixelFunction= {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
			AlphaBlend(newDC, 0, 0, w, h, currentDC, 0, 0, m_Width, m_Height, blendPixelFunction);
		}
		else
		{
			SetStretchBltMode(currentDC, HALFTONE);
			SetStretchBltMode(newDC, HALFTONE);
			SetBrushOrgEx(currentDC, 0, 0, NULL);
			SetBrushOrgEx(newDC, 0, 0, NULL);

			StretchBlt(newDC, 0, 0, w, h, currentDC, 0, 0, m_Width, m_Height, SRCCOPY);
		}
	}
	else	// TILE
	{
		for(int j = 0; j < h; j += m_Height) 
		{
			for(int i = 0; i < w; i += m_Width) 
			{
				BitBlt(newDC, i, j, min(m_Width, w - i), min(m_Height, h - j), currentDC, 0, 0, SRCCOPY);
			}
		}
	}

	SelectObject(currentDC, oldCurrentBitmap);
	SelectObject(newDC, oldNewBitmap);
	DeleteDC(currentDC);
	DeleteDC(newDC);
	DeleteObject(m_Image);

	m_Image = bitmap;
	m_Bits = (BYTE*)bits;
}

/*
** Blit
**
** Blits the given image to this image.
**
*/
void CImage::Blit(const CImage& source, int dx, int dy, int sx, int sy, int w, int h)
{
	if (m_Image == NULL || source.m_Image == NULL) return;

	HDC dc = GetDC(GetDesktopWindow());
	HDC srcDC = CreateCompatibleDC(dc);
	HDC destDC = CreateCompatibleDC(dc);
	ReleaseDC(GetDesktopWindow(), dc);

	HBITMAP oldSrcBitmap = (HBITMAP)SelectObject(srcDC, source.m_Image);
	HBITMAP oldDestBitmap = (HBITMAP)SelectObject(destDC, m_Image);

	if (!source.HasAlpha())
	{
		BitBlt(destDC, dx, dy, w, h, srcDC, sx, sy, SRCCOPY);
	} 
	else
	{
		OSVERSIONINFO version;
		version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&version);
		
		if (version.dwPlatformId == VER_PLATFORM_WIN32_NT && version.dwMajorVersion > 4)
		{
			BLENDFUNCTION blendPixelFunction= {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
			AlphaBlend(destDC, dx, dy, w, h, srcDC, sx, sy, w, h, blendPixelFunction);
		}
		else
		{
			// Win98 does not alphablend very well, so we'll have to do everything manually (and slowly)
			AlphaBlit(source, dx, dy, sx, sy, w, h);
		}
	}

	SelectObject(srcDC, oldSrcBitmap);
	SelectObject(destDC, oldDestBitmap);
	DeleteDC(srcDC);
	DeleteDC(destDC);
}


void CImage::AlphaBlit(const CImage& source, int dx, int dy, int sx, int sy, int w, int h)
{
	int endW = min(m_Width, dx + w);
	int endH = min(m_Height, dy + h);

	// The images are upside down so we'll have to flip the y-coords

	for(int j = dy; j < endH ; j++) 
	{
		for(int i = dx; i < endW; i++) 
		{
			// r = sr * a + dr * (1 - a)

			int srcYIndex = (j - dy + source.m_Height - sy - h) * source.m_Width * 4 + (i - dx + sx) * 4;
			int destYIndex = (j + m_Height - 2 * dy - h) * m_Width * 4 + i * 4;

			int sa = source.m_Bits[srcYIndex + 3];
			int da = m_Bits[destYIndex + 3];
			da = max(da, 1);
			int r = source.m_Bits[srcYIndex + 0] + m_Bits[destYIndex + 0] * (0x0ff - sa) / da;
			int g = source.m_Bits[srcYIndex + 1] + m_Bits[destYIndex + 1] * (0x0ff - sa) / da;
			int b = source.m_Bits[srcYIndex + 2] + m_Bits[destYIndex + 2] * (0x0ff - sa) / da;
			int a = max(da, sa) + ((da * sa) >> 8);
			a = min(255, a);
			
			m_Bits[destYIndex + 0] = (BYTE)((r * a) >> 8);
			m_Bits[destYIndex + 1] = (BYTE)((g * a) >> 8);
			m_Bits[destYIndex + 2] = (BYTE)((b * a) >> 8);
			m_Bits[destYIndex + 3] = (BYTE)(a);

			assert(srcYIndex + 3 < source.m_Width * source.m_Height * 4);
			assert(destYIndex + 3 < m_Width * m_Height * 4);
		}
	}
}


void CImage::Create(int width, int height, BYTE alphaValue)
{
	HBITMAP bm = CreateBitmap(width, height, 1, 32, NULL);
	Create(bm, NULL, alphaValue);	// Fully transparent
	DeleteObject(bm);
}

/*
** Create
**
** Creates a 32 bit bitmap from the image and it's alpha channel
*/
void CImage::Create(HBITMAP image, HBITMAP alpha, BYTE alphaValue)
{
	LPBYTE SBuffer;
	LPBYTE ABuffer;
	BITMAP bm;
	BITMAPINFO bmi;

	Clear();

	GetObject(image, sizeof(BITMAP), &bm);
	m_Width = bm.bmWidth;
	m_Height = bm.bmHeight;

	// Join the image and alpha into one bitmap (if there is no alpha, make image opaque).
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = m_Width;
	bmi.bmiHeader.biHeight = m_Height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = m_Width * m_Height * 4;
	
	// create our DIB section and select the bitmap into the dc
	HDC dc = CreateCompatibleDC(NULL);
	VOID* bits;
	m_Image = CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0x0);
	m_Bits = (BYTE*)bits;

	if (alpha == NULL && alphaValue == 255)
	{
		m_Alpha = false;
	}
	else
	{
		m_Alpha = true;
	}

	// All the bitmaps are same size and same colordepth (I hope :)
	SBuffer = new BYTE[m_Width * m_Height * 4];
	ABuffer = new BYTE[m_Width * m_Height * 4];

	GetDIBits(dc, image, 0, m_Height, SBuffer, &bmi, DIB_RGB_COLORS);
	if (alpha)
	{
		GetDIBits(dc, alpha, 0, m_Height, ABuffer, &bmi, DIB_RGB_COLORS);
	}

	DeleteDC(dc);

	for(int i = 0; i < m_Height * m_Width * 4; i += 4) 
	{
		if (alpha)
		{
			m_Bits[i + 0] = SBuffer[i + 0] * ABuffer[i] >> 8;
			m_Bits[i + 1] = SBuffer[i + 1] * ABuffer[i] >> 8;
			m_Bits[i + 2] = SBuffer[i + 2] * ABuffer[i] >> 8;
			m_Bits[i + 3] = ABuffer[i];
		}
		else
		{
			m_Bits[i + 0] = SBuffer[i + 0] * alphaValue >> 8;
			m_Bits[i + 1] = SBuffer[i + 1] * alphaValue >> 8;
			m_Bits[i + 2] = SBuffer[i + 2] * alphaValue >> 8;
			m_Bits[i + 3] = alphaValue;
		}
	}

	delete [] SBuffer;
	delete [] ABuffer;
}

/*
** MultiplyAlpha
**
** Multiplies the alpha values with the RGB, just like Windows likes it.
*/
void CImage::MultiplyAlpha()
{
	if (m_Bits == NULL) return;

	for(int i = 0; i < m_Height * m_Width * 4; i += 4) 
	{
		m_Bits[i + 0] = m_Bits[i + 0] * m_Bits[i + 3] >> 8;
		m_Bits[i + 1] = m_Bits[i + 1] * m_Bits[i + 3] >> 8;
		m_Bits[i + 2] = m_Bits[i + 2] * m_Bits[i + 3] >> 8;
	}
}

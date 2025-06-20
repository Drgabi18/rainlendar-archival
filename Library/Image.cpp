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
  $Header: /home/cvsroot/Rainlendar/Library/Image.cpp,v 1.1.1.1 2005/07/10 18:48:07 rainy Exp $

  $Log: Image.cpp,v $
  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.10  2004/12/05 18:31:03  rainy
  Load image uses streams.

  Revision 1.9  2004/11/06 13:38:59  rainy
  no message

  Revision 1.8  2004/04/24 11:18:51  rainy
  Fixed alpha blend bug in WIn9x.

  Revision 1.7  2004/01/10 15:15:26  rainy
  Images can be loaded from resources too.

  Revision 1.6  2003/12/05 15:46:13  Rainy
  Changed image loading routines

  Revision 1.5  2003/10/27 17:37:31  Rainy
  Added new resize mode.

  Revision 1.4  2003/08/09 16:36:25  Rainy
  Added a check if the file exists.

  Revision 1.3  2003/06/15 09:48:45  Rainy
  Uses AlphaBlend for all stretching.

  Revision 1.2  2002/11/25 17:03:57  rainy
  Small fixes.

  Revision 1.1  2002/11/12 18:00:36  rainy
  Initial version

*/
#pragma warning(disable: 4996)

#include "Image.h"
#include "Litestep.h"
#include <png.h>
#include <assert.h>
#include <comutil.h>
#include <comdef.h>
#include <comutil.h>
#include <comdef.h>
#include <shlwapi.h>

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
	HBITMAP hBitmap = NULL;
	HBITMAP hBitmapAlpha = NULL;
	IPicture* pPicture = NULL;
	IPicture* pPictureAlpha = NULL;

	if (LoadPNG(filename))
	{
		return true;		// PNG loading succeeded
	}

	pPicture = LoadImage(filename.c_str());
	if (pPicture == NULL)
	{
		DebugLog("Unable to load the file: %s", filename.c_str());
		return false;
	}

	pPicture->get_Handle((OLE_HANDLE*)&hBitmap);

	BITMAP bm;
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	m_Width = bm.bmWidth;
	m_Height = bm.bmHeight;

	// Check for Alpha-map
	int pos = filename.rfind('.');
	if (pos != -1)
	{
		std::string alphaName;

		alphaName = filename;
		alphaName.insert(pos, "-Alpha");

		pPictureAlpha = LoadImage(alphaName.c_str());
		if (pPictureAlpha != NULL)
		{
			pPictureAlpha->get_Handle((OLE_HANDLE*)&hBitmapAlpha);

			if(hBitmapAlpha != NULL) 
			{
				// Check that it is correct size
				GetObject(hBitmapAlpha, sizeof(BITMAP), &bm);
				if (m_Width != bm.bmWidth || m_Height != bm.bmHeight)
				{
					DebugLog("The alpha-image has incorrect size.");
					hBitmapAlpha = NULL;
				}
			}
		}
	}

	Create(hBitmap, hBitmapAlpha, 0x0ff);

	if (pPicture) pPicture->Release();
	if (pPictureAlpha) pPictureAlpha->Release();

	DebugLog("Loaded normal image: %s (W: %i, H: %i) %s", filename.c_str(), m_Width, m_Height, hBitmapAlpha ? "Alpha" : "No Alpha");

	return true;
}

IPicture* CImage::LoadImage(const std::string& filename)
{
	HRESULT hr; 
	IUnknown* unk = NULL;
	IPersistStream* persistStream = NULL;
	IStream* stream = NULL;
	IPicture* picture = NULL;

	hr = SHCreateStreamOnFile(filename.c_str(), STGM_READ, &stream);
	if (SUCCEEDED(hr))
	{
		hr = OleCreatePictureIndirect(NULL, IID_IUnknown, TRUE, (LPVOID*)&unk);
		if (SUCCEEDED(hr))
		{
			hr = unk->QueryInterface(IID_IPersistStream, (LPVOID*)&persistStream );
			if (SUCCEEDED(hr))
            {
				hr = persistStream->Load(stream);
				if (SUCCEEDED(hr))
				{
                    hr = unk->QueryInterface(IID_IPicture, (LPVOID*)&picture);
				}
            }
		}
	}

	if (stream) stream->Release();
	if (persistStream) persistStream->Release();
	if (unk) unk->Release();

	return picture;
}

bool CImage::LoadResource(HINSTANCE instance, int resourceID)
{
	HBITMAP hBitmap = LoadBitmap(instance, MAKEINTRESOURCE(resourceID));

	if (hBitmap)
	{
		Create(hBitmap, NULL, 0x0ff);
		DeleteObject(hBitmap);
		return true;
	}
	return false;
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

	if (png_file == NULL) return false;

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
		
	DebugLog("Loaded png image: %s (W: %i, H: %i) %s", filename.c_str(), m_Width, m_Height, m_Alpha ? "Alpha" : "No Alpha");

	return TRUE;
}

/*
** Resize
**
** Resizes the image
*/
void CImage::Resize(int w, int h, IMAGE_RESIZE_TYPE type, RECT* margins)
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

	if (margins)
	{
		if (type == IMAGE_RESIZE_TYPE_STRETCH)
		{
			// Corners
			AlphaBlit(newDC, 0, 0, margins->left, margins->top, currentDC, 0, 0, margins->left, margins->top);
			AlphaBlit(newDC, w - margins->right, 0, margins->right, margins->top, currentDC, m_Width - margins->right, 0, margins->right, margins->top);
			AlphaBlit(newDC, w - margins->right, h - margins->bottom, margins->right, margins->bottom, currentDC, m_Width - margins->right, m_Height - margins->bottom, margins->right, margins->bottom);
			AlphaBlit(newDC, 0, h - margins->bottom, margins->left, margins->bottom, currentDC, 0, m_Height - margins->bottom, margins->left, margins->bottom);

			// Edges (N, E, S, W)
			AlphaBlit(newDC, margins->left, 0, w - margins->right - margins->left, margins->top, 
				       currentDC, margins->left, 0, m_Width - margins->right - margins->left, margins->top);
			AlphaBlit(newDC, w - margins->right, margins->top, margins->right, h - margins->bottom - margins->top, 
				       currentDC, m_Width - margins->right, margins->top, margins->right, m_Height - margins->bottom - margins->top);
			AlphaBlit(newDC, margins->left, h - margins->bottom, w - margins->right - margins->left, margins->bottom, 
				       currentDC, margins->left, m_Height - margins->bottom, m_Width - margins->right - margins->left, margins->bottom);
			AlphaBlit(newDC, 0, margins->top, margins->left, h - margins->bottom - margins->top, 
				       currentDC, 0, margins->top, margins->left, m_Height - margins->bottom - margins->top);

			// Center
			AlphaBlit(newDC, margins->left, margins->top, w - margins->right - margins->left, h - margins->bottom - margins->top, 
				       currentDC, margins->left, margins->top, m_Width - margins->right - margins->left, m_Height - margins->bottom - margins->top);
		}
		// TODO: add support for TILE
	}
	else
	{
		if (type == IMAGE_RESIZE_TYPE_STRETCH)
		{
			AlphaBlit(newDC, 0, 0, w, h, currentDC, 0, 0, m_Width, m_Height);
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
	}

	SelectObject(currentDC, oldCurrentBitmap);
	SelectObject(newDC, oldNewBitmap);
	DeleteDC(currentDC);
	DeleteDC(newDC);
	DeleteObject(m_Image);

	m_Image = bitmap;
	m_Bits = (BYTE*)bits;
	m_Height = h;
	m_Width = w;
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

void CImage::AlphaBlit(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
                       HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc)
{
	OSVERSIONINFO version;
	version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&version);

	if (version.dwPlatformId == VER_PLATFORM_WIN32_NT && version.dwMajorVersion > 4)
	{
		BLENDFUNCTION blendPixelFunction= {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
		AlphaBlend(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, 
			hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, blendPixelFunction);
	}
	else
	{
		// Win98 does not alphablend very well, so we'll have to do the blending with DDBs
		// msimg32.dll version 5.00.1693.1 crashes with AlphaBlend() if DIBs are used.
		// Note: This creates quite much color artifacts 

		HDC dc = CreateCompatibleDC(NULL);
		HDC dc2 = CreateCompatibleDC(NULL);
		HBITMAP bitmap = CreateBitmap(nWidthSrc, nHeightSrc, 1, 32, NULL);
		HBITMAP bitmap2 = CreateBitmap(nWidthDest, nHeightDest, 1, 32, NULL);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(dc, bitmap);
		HBITMAP oldBitmap2 = (HBITMAP)SelectObject(dc2, bitmap2);
		BitBlt(dc, 0, 0, nWidthSrc, nHeightSrc, hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
		BitBlt(dc2, 0, 0, nWidthDest, nHeightDest, hdcDest, nXOriginDest, nYOriginDest, SRCCOPY);

		BLENDFUNCTION blendPixelFunction= {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
		AlphaBlend(dc2, 0, 0, nWidthDest, nHeightDest, 
			dc, 0, 0, nWidthSrc, nHeightSrc, blendPixelFunction);

		BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, dc2, 0, 0, SRCCOPY);

		SelectObject(dc, oldBitmap);
		SelectObject(dc2, oldBitmap2);
		DeleteObject(bitmap);
		DeleteObject(bitmap2);
		DeleteDC(dc);
		DeleteDC(dc2);
	}
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

/*
** ERROR.H
**
** Created by: Kimmo Pekkola (2.3.2000)
**
** Version: 0.1
**
*/

#ifndef __ERROR_H__
#define __ERROR_H__

enum RAIN_ERROR {
	ERR_NONE,
	ERR_OUTOFMEM,
	ERR_WINDOWCLASS,
	ERR_WINDOW,
	ERR_BACKGROUND,
	ERR_BACKGROUNDALPHASIZE,
	ERR_ALPHASIZE,
	ERR_CREATEFONT,
	ERR_TEXTDIMENSIONS,
};

static const char* ErrorMessages[] = {
	"No Errors!",
	"Out of memory!",
	"Unable to create the window class!",
	"Unable to create the main window!",
	"Unable to fetch the desktop's background!",
	"Background and it's alpha-map must be same size!!",
	"Bitmap and it's alpha-map must be same size!!",
	"Unable to create font!",
	"Unable to calculate text dimensions!",
};

static const char* GetErrorMessage(RAIN_ERROR Error) { return ErrorMessages[Error]; };

#endif

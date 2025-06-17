/*
  Copyright (C) 2004 Kimmo Pekkola

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
  $Header: /home/cvsroot/Rainlendar/Library/RainlendarAPI.h,v 1.2 2005/10/12 15:47:02 rainy Exp $

  $Log: RainlendarAPI.h,v $
  Revision 1.2  2005/10/12 15:47:02  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.6  2005/03/25 13:58:43  rainy
  no message

  Revision 1.5  2005/03/01 18:25:29  rainy
  Updated the interface.

  Revision 1.4  2004/12/05 18:24:24  rainy
  Added todo's profile is copied too.

  Revision 1.3  2004/11/14 12:12:23  rainy
  Linux fixes

  Revision 1.2  2004/11/14 11:55:18  rainy
  Added profile to Todo & Rainlendar_Flushitems

  Revision 1.1  2004/11/06 13:38:59  rainy
  no message

*/

#ifndef _RAINLENDAR_API_H_
#define _RAINLENDAR_API_H_

#include <time.h>

#ifdef _WIN32
#include <guiddef.h>
#include <windows.h>
#else

#define LPCTSTR const char*
#define LPTSTR char*
#define INT int
#define HWND unsigned long
#define HINSTANCE unsigned long
#define BYTE unsigned char

typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID;


typedef struct _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME;


#endif

#ifdef _WIN32
#define PLUGIN_API extern "C" __declspec(dllexport) 
#ifdef PLUGIN_EXPORTS
#define RAINLENDAR_API extern "C" __declspec(dllexport) 
#else
#define RAINLENDAR_API extern "C" __declspec(dllimport) 
#endif
#else
#define RAINLENDAR_API
#define PLUGIN_API
#endif

//////////////////////////////////////////////////////////////////////////
//
// API provided by Rainlendar
//
//////////////////////////////////////////////////////////////////////////
enum TODO_TYPE
{	
	TODO_TYPE_TODO,
	TODO_TYPE_DONE,
	TODO_TYPE_NOTE
};

enum RAINLENDAR_TYPE
{
	RAINLENDAR_TYPE_EVENT,
	RAINLENDAR_TYPE_TODO
};

enum RECURRENCY_TYPE
{
	RECURRENCY_TYPE_SINGLE,
	RECURRENCY_TYPE_DAILY,
	RECURRENCY_TYPE_WEEKLY,
	RECURRENCY_TYPE_YEARLY,
	RECURRENCY_TYPE_MONTHLY_NTH_DAY,	// On the [Nth] day of the month
	RECURRENCY_TYPE_MONTHLY_NTH_WEEKDAY,	// On the [Nth] [weekday] of the month
	RECURRENCY_TYPE_MONTHLY_LASTWEEK_WEEKDAY,	// On the last [weekday] of the month
	RECURRENCY_TYPE_MONTHLY_LAST_DAY,	// On the last day of the month
	RECURRENCY_TYPE_MONTHLY_FIRST_WEEKDAY,	// First weekday of the month
	RECURRENCY_TYPE_MONTHLY_LAST_WEEKDAY,	// Last weekday of the month
};

enum RECURRENCY_REPEAT
{
	RECURRENCY_REPEAT_FOREVER,
	RECURRENCY_REPEAT_COUNT,
	RECURRENCY_REPEAT_UNTIL
};

struct RainlendarRecurrency
{
	DWORD size;
	RECURRENCY_TYPE type;
	UINT frequency;
	DWORD days;
	RECURRENCY_REPEAT repeatType;
	UINT count;
	FILETIME until;
};

struct RainlendarItem
{
	DWORD size;
	RAINLENDAR_TYPE type;
	GUID guid;
	BYTE deleted;
	BYTE readOnly;				// Items are not sent for saving
    FILETIME timeStamp;
};

struct RainlendarTodo : public RainlendarItem
{
	TODO_TYPE todoType;
	INT position;
	BYTE checked;
	LPTSTR message;
	LPTSTR profile;
};

struct RainlendarEvent : public RainlendarItem
{
	LPTSTR header;
	LPTSTR message;
	LPTSTR profile;
	FILETIME startTime;
	FILETIME endTime;			// 0 is single day
	BYTE allDayEvent;
	RainlendarRecurrency* recurrency;
	int alarm;
};

RAINLENDAR_API RainlendarItem* Rainlendar_GetItem(GUID* guid);
RAINLENDAR_API RainlendarItem** Rainlendar_GetAllItems(RAINLENDAR_TYPE type);
RAINLENDAR_API void Rainlendar_SetItem(RainlendarItem* item, UINT id);	// Thread safe
RAINLENDAR_API void Rainlendar_Log(LPCTSTR debugStr);
RAINLENDAR_API void Rainlendar_Redraw();		// redraws the window
RAINLENDAR_API void Rainlendar_Refresh(int delay);		// Refreshed everything (loades skin & events)
RAINLENDAR_API bool Rainlendar_EditItemDialog(HWND parent, RainlendarItem* item, RAINLENDAR_TYPE type);	// Default event/todo editor
RAINLENDAR_API LPCTSTR Rainlendar_GetString(LPCTSTR section, int index, LPCTSTR defaultString);
RAINLENDAR_API bool Rainlendar_CopyItem(RainlendarItem* from, RainlendarItem* to);
RAINLENDAR_API void Rainlendar_DeleteItem(RainlendarItem* item);
RAINLENDAR_API void Rainlendar_FlushItems();


//////////////////////////////////////////////////////////////////////////
//
// API provided by the plugins
//
//////////////////////////////////////////////////////////////////////////

PLUGIN_API void Plugin_Initialize(HINSTANCE dllInstance, LPCTSTR configFile, UINT id);	// Only called when Rainlendar starts (not during refresh)
PLUGIN_API void Plugin_Finalize();	// Called when Rainlendar exits

PLUGIN_API void Plugin_Enable();	// Called also on refresh
PLUGIN_API void Plugin_Disable();	// Called also on refresh

PLUGIN_API LPCTSTR Plugin_GetName();		// Must exist
PLUGIN_API LPCTSTR Plugin_GetAuthor();
PLUGIN_API UINT Plugin_GetVersion();

PLUGIN_API void Plugin_ReadItems();
PLUGIN_API void Plugin_ItemNotification(GUID** guid);

PLUGIN_API bool Plugin_EditItem(HWND parent, RainlendarItem* item, RAINLENDAR_TYPE type); // Optional

PLUGIN_API void Plugin_ExportItems(LPCTSTR eventFile, GUID** guids, RAINLENDAR_TYPE type); // Optional
PLUGIN_API bool Plugin_ImportItems(LPCTSTR eventFile, RAINLENDAR_TYPE type); // Optional

PLUGIN_API bool Plugin_ShowConfig(HWND hwndParent, LPCTSTR configFile);	// Optional


// NOTES:
// - To delete an event use Rainlendar_SetItem and set deleted to TRUE

#endif

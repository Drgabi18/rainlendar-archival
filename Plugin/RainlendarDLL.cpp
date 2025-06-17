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
  $Header: /home/cvsroot/Rainlendar/Plugin/RainlendarDLL.cpp,v 1.18 2004/12/05 18:23:13 rainy Exp $

  $Log: RainlendarDLL.cpp,v $
  Revision 1.18  2004/12/05 18:23:13  rainy
  Fixed the bangs.

  Revision 1.17  2004/11/06 13:38:59  rainy
  no message

  Revision 1.16  2004/06/10 16:17:14  rainy
  Added zpos

  Revision 1.15  2004/01/28 18:06:39  rainy
  Fixed few bangs.

  Revision 1.14  2004/01/25 10:01:47  rainy
  Added new bangs.

  Revision 1.13  2004/01/10 15:13:21  rainy
  Changed Toggle, Show and Hide to affect also the Todo window.

  Revision 1.12  2003/10/27 19:51:11  Rainy
  Fixed parameters to Initialize()

  Revision 1.11  2003/10/27 17:38:32  Rainy
  Removed wharf support.

  Revision 1.10  2003/08/09 15:26:26  Rainy
  DLL Instance is saved to global variable.

  Revision 1.9  2003/06/15 09:49:55  Rainy
  ShowMonth -> ChangeMonth

  Revision 1.8  2003/03/22 20:29:53  rainy
  Added Move and ZPos bangs

  Revision 1.7  2002/08/24 11:09:40  rainy
  Changed the error handling.

  Revision 1.6  2002/08/03 16:10:47  rainy
  Added interfaces for new bang commands.
  SetWharf is now just a boolean.

  Revision 1.5  2002/05/30 18:25:44  rainy
  Removed some Litestep related stuff.
  Rainlendar is now a static object.

  Revision 1.4  2002/05/23 17:33:40  rainy
  Removed all MFC stuff

  Revision 1.3  2002/01/10 16:41:35  rainy
  no message

  Revision 1.2  2001/12/23 10:01:30  rainy
  Rainlendar->dll uses a bit different interface.
  Added Wharf interface.

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/
#pragma warning(disable: 4996)

#include "RainlendarDLL.h"
#include "CalendarWindow.h"
#include "Error.h"
#include "Tooltip.h"
#include "EditEvent.h"

CRainlendar* Rainlendar = NULL; // The module
bool CRainlendar::c_DummyLitestep=false;
std::string CRainlendar::c_CmdLine;
HINSTANCE CRainlendar::c_Instance;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// Store the instance
		CRainlendar::SetInstance(hinstDLL);

		if (Rainlendar == NULL)
		{
			Rainlendar = new CRainlendar();
		}
	}
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

CRainlendar* GetRainlendar()
{ 
	return Rainlendar; 
};

/*
** initModuleEx
**
** This is called when the plugin is initialized
**
*/
int initModuleEx(HWND ParentWnd, HINSTANCE dllInst, LPCSTR szPath)
{
	int Result=1;
	
	try 
	{
		Result = Rainlendar->Initialize(ParentWnd, dllInst, szPath);
	} 
    catch(CError& error) 
    {
		MessageBox(NULL, error.GetString().c_str(), APPNAME, MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
	}

	return Result;
}

/*
** quitModule
**
** This is called when the plugin quits.
**
*/
void quitModule(HINSTANCE dllInst)
{
	Rainlendar->Quit(dllInst);
}

/*
** Initialize
**
** Init Rainlendar
**
*/
void Initialize(bool DummyLS, LPCSTR CmdLine)
{
	CRainlendar::SetDummyLitestep(DummyLS);
	
	if (CmdLine) 
	{
		CRainlendar::SetCommandLine(CmdLine);
	}
}

/*
** RainlendarToggle
**
** Callback for the !RainlendarToggle bang
**
*/
void RainlendarToggle(HWND, const char* arg)
{
	bool visible = false;

	if(arg!=NULL && arg[0]!='\0') 
	{
		if (0 == stricmp(arg, "visible"))
		{
			visible = true;
		}
	} 

	Rainlendar->ToggleWindow(visible);
}

/*
** RainlendarHide
**
** Callback for the !RainlendarHide bang
**
*/
void RainlendarHide(HWND, const char* arg)
{
	Rainlendar->HideWindow();
}

/*
** RainlendarShow
**
** Callback for the !RainlendarShow bang
**
*/
void RainlendarShow(HWND, const char* arg)
{
	bool activate = false;

	if(arg!=NULL && arg[0]!='\0') 
	{
		if (0 == stricmp(arg, "activate"))
		{
			activate = true;
		}
	} 
	Rainlendar->ShowWindow(activate);
}

/*
** RainlendarRefresh
**
** Callback for the !RainlendarRefresh bang
**
*/
void RainlendarRefresh(HWND, const char* arg)
{
	Rainlendar->RefreshWindow();
}

/*
** RainlendarRedraw
**
** Callback for the !RainlendarRedraw bang
**
*/
void RainlendarRedraw(HWND, const char* arg)
{
	Rainlendar->GetCalendarWindow().RedrawAll();
}

/*
** RainlendarConfig
**
** Callback for the !RainlendarConfig bang
**
*/
void RainlendarConfig(HWND, const char* arg)
{
	Rainlendar->ShowConfig();
}

/*
** RainlendarSkinConfig
**
** Callback for the !RainlendarConfig bang
**
*/
void RainlendarEditSkin(HWND, const char* arg)
{
	Rainlendar->ShowEditSkin();
}

/*
** RainlendarChangeSkin
**
** Callback for the !RainlendarChangeSkin bang
**
*/
void RainlendarChangeSkin(HWND, const char* arg)
{
	char* buffer;
	char* token;
	
	if (arg!=NULL && arg[0]!='\0') 
	{
		buffer = strdup(arg);
		token = strchr(buffer, '\\');
		
		if (token != NULL) 
		{
			*token = 0;
			CConfig::Instance().SetCurrentSkin(buffer);
			CConfig::Instance().SetCurrentSkinIni(token + 1);
			CConfig::Instance().WriteConfig(CConfig::WRITE_CONFIG);	// Write old config (happens only if Rainlendar.ini's not modified from outside)
			Rainlendar->RefreshWindow();
		}
		else
		{
			DebugLog("Invalid argument for !RainlendarChangeSkin");
		}
		free(buffer);
	} 
	else
	{
		DebugLog("No argument given for !RainlendarChangeSkin");
	}


}

/*
** RainlendarQuit
**
** Callback for the !RainlendarQuit bang
**
*/
void RainlendarQuit(HWND, const char* arg)
{
	// Delay quit
	PostMessage(Rainlendar->GetCalendarWindow().GetWindow(), WM_COMMAND, ID_QUIT, NULL);
}

/*
** RainlendarShowNext
**
** Callback for the !RainlendarShowNext bang
**
*/
void RainlendarShowNext(HWND, const char* arg)
{
	int num = 1;
	if (arg != NULL && arg[0] != '\0')
	{
		num = atoi(arg);
	}

	Rainlendar->ShowNextMonth(num);
}

/*
** RainlendarShowPrev
**
** Callback for the !RainlendarShowPrev bang
**
*/
void RainlendarShowPrev(HWND, const char* arg)
{
	int num = 1;
	if (arg != NULL && arg[0] != '\0')
	{
		num = atoi(arg);
	}

	Rainlendar->ShowPrevMonth(num);
}

/*
** RainlendarShowCurrent
**
** Callback for the !RainlendarShowCurrent bang
**
*/
void RainlendarShowCurrent(HWND, const char* arg)
{
	Rainlendar->ShowCurrentMonth();
}

/*
** RainlendarShowMonth
**
** Callback for the !RainlendarShowCurrent bang
**
*/
void RainlendarShowMonth(HWND, const char* arg)
{
	char Buffer[256];
	char* Token;
	int Month=0, Year=0;
	
	if(arg!=NULL && arg[0]!='\0') 
	{
		strncpy(Buffer, arg, 255);
		Buffer[255] = '\0';
		Token = strtok(Buffer, " ");
		
		if(Token!=NULL) 
		{
			Month = atoi(Token);
			Token = strtok(NULL, " ");
			
			if(Token != NULL) 
			{
				Year = atoi(Token);
				Rainlendar->ShowMonth(Month, Year);
				Rainlendar->GetCalendarWindow().Redraw();
			}
			else 
			{
				Rainlendar->ShowMonth(Month, -1);
				Rainlendar->GetCalendarWindow().Redraw();
			}
		}
	} 
	else 
	{
		Rainlendar->ShowMonth(-1, -1);
		Rainlendar->GetCalendarWindow().Redraw();
	}
}

/*
** RainlendarLsBoxHook
**
** Callback for the !RainlendarLsBoxHook bang
**
*/
void RainlendarLsBoxHook(HWND, const char* arg)
{
	const char* handle = strrchr(arg,' ');
	
	if (handle) 
	{
		HWND hWnd = (HWND)atoi(handle+1);
		HWND hWndCal = GetRainlendar()->GetCalendarWindow().GetWindow();
		if (hWnd && hWndCal) 
		{
			SetWindowLong(hWndCal, GWL_STYLE, (GetWindowLong(hWndCal, GWL_STYLE) &~ WS_POPUP) | WS_CHILD);
			SetParent(hWndCal, hWnd);
		}
	}
}

/*
** RainlendarMove
**
** Callback for the !RainlendarMove bang
**
*/
void RainlendarMove(HWND, const char* arg)
{
	if(arg!=NULL && arg[0]!='\0') 
	{
		int x = 0, y = 0, window = 0;
		sscanf(arg, "%i %i %i", &x, &y, &window);
		Rainlendar->MoveWindow(x, y, (RAINWINDOW_TYPE)window);
	} 
}

/*
** RainlendarZPos
**
** Callback for the !RainlendarZPos bang
**
*/
void RainlendarZPos(HWND, const char* arg)
{
	if(arg!=NULL && arg[0]!='\0') 
	{
		CConfig::WINDOWPOS pos = (CConfig::WINDOWPOS)atoi(arg);
		Rainlendar->SetWindowZPos(pos);
	} 
}

/*
** RainlendarEditTodo
**
** Callback for the !RainlendarEditTodo bang
**
*/
void RainlendarEditTodo(HWND, const char* arg)
{
	Rainlendar->EditTodo();
}

/*
** RainlendarShowTodo
**
** Callback for the !RainlendarShowTodo bang
**
*/
void RainlendarShowTodo(HWND, const char* arg)
{
	Rainlendar->GetCalendarWindow().GetTodoWindow().ShowWindow(false);
	CConfig::Instance().SetTodoEnable(false);
	CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
}

/*
** RainlendarHideTodo
**
** Callback for the !RainlendarHideTodo bang
**
*/
void RainlendarHideTodo(HWND, const char* arg)
{
	Rainlendar->GetCalendarWindow().GetTodoWindow().HideWindow();
	CConfig::Instance().SetTodoEnable(false);
	CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
}

/*
** RainlendarToggleTodo
**
** Callback for the !RainlendarToggleTodo bang
**
*/
void RainlendarToggleTodo(HWND, const char* arg)
{
	Rainlendar->GetCalendarWindow().GetTodoWindow().ToggleWindow();
	CConfig::Instance().SetTodoEnable(!CConfig::Instance().GetTodoEnable());
	CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
}

/*
** RainlendarShowEventList
**
** Callback for the !RainlendarShowEventList bang
**
*/
void RainlendarShowEventList(HWND, const char* arg)
{
	Rainlendar->GetCalendarWindow().GetEventListWindow().ShowWindow(false);
	CConfig::Instance().SetEventListEnable(false);
	CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
}

/*
** RainlendarHideEventList
**
** Callback for the !RainlendarHideEventList bang
**
*/
void RainlendarHideEventList(HWND, const char* arg)
{
	Rainlendar->GetCalendarWindow().GetEventListWindow().HideWindow();
	CConfig::Instance().SetEventListEnable(false);
	CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
}

/*
** RainlendarToggleEventList
**
** Callback for the !RainlendarToggleEventList bang
**
*/
void RainlendarToggleEventList(HWND, const char* arg)
{
	Rainlendar->GetCalendarWindow().GetEventListWindow().ToggleWindow();
	CConfig::Instance().SetEventListEnable(!CConfig::Instance().GetEventListEnable());
	CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
}

/*
** RainlendarShowCalendar
**
** Callback for the !RainlendarShowCalendar bang
**
*/
void RainlendarShowCalendar(HWND, const char* arg)
{
	Rainlendar->GetCalendarWindow().ShowWindow(false);
	CConfig::Instance().SetCalendarEnable(false);
	CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
}

/*
** RainlendarHideCalendar
**
** Callback for the !RainlendarHideCalendar bang
**
*/
void RainlendarHideCalendar(HWND, const char* arg)
{
	Rainlendar->GetCalendarWindow().HideWindow();
	CConfig::Instance().SetCalendarEnable(false);
	CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
}

/*
** RainlendarToggleCalendar
**
** Callback for the !RainlendarToggleCalendar bang
**
*/
void RainlendarToggleCalendar(HWND, const char* arg)
{
	Rainlendar->GetCalendarWindow().ToggleWindow();
	CConfig::Instance().SetCalendarEnable(!CConfig::Instance().GetCalendarEnable());
	CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
}

/*
** RainlendarShowItem
**
** Callback for the !RainlendarShowItem bang
**
*/
void RainlendarShowItem(HWND, const char* arg)
{
	CItem* item = Rainlendar->GetCalendarWindow().GetItem(arg);
	if (item)
	{
		item->Enable();
		item->WriteSettings();
	}
}

/*
** RainlendarHideItem
**
** Callback for the !RainlendarHideItem bang
**
*/
void RainlendarHideItem(HWND, const char* arg)
{
	CItem* item = Rainlendar->GetCalendarWindow().GetItem(arg);
	if (item)
	{
		item->Disable();
		item->WriteSettings();
	}
}

/*
** RainlendarToggleItem
**
** Callback for the !RainlendarToggleItem bang
**
*/
void RainlendarToggleItem(HWND, const char* arg)
{
	CItem* item = Rainlendar->GetCalendarWindow().GetItem(arg);
	if (item)
	{
		if (item->IsEnabled())
		{
			item->Disable();
		}
		else
		{
			item->Enable();
		}
		item->WriteSettings();
	}
}

/*
** RainlendarAddEvent
**
** Callback for the !RainlendarAddEvent bang
**
*/
void RainlendarAddEvent(HWND, const char* arg)
{
	int day = 0, month = 0, year = 0;

	if(arg!=NULL && arg[0]!='\0') 
	{
		sscanf(arg, "%i %i %i", &day, &month, &year);
	} 

	Rainlendar->AddEvent(day, month, year);
}

static _CrtMemState g_MemoryState;

/* 
** CRainlendar
**
** Constructor
**
*/
CRainlendar::CRainlendar()
{
#ifdef _DEBUG
	_CrtMemCheckpoint(&g_MemoryState);
//	_CrtSetBreakAlloc(966);
#endif
}

/* 
** ~CRainlendar
**
** Destructor
**
*/
CRainlendar::~CRainlendar()
{
}

/* 
** Initialize
**
** The main initialization function for the module.
** May throw CErrors !!!!
**
*/
int CRainlendar::Initialize(HWND Parent, HINSTANCE Instance, LPCSTR szPath)
{
	int Result=0;

	if (Parent==NULL || Instance==NULL) 
	{
		THROW(ERR_NULLPARAMETER);
	}	

	InitalizeLitestep(c_DummyLitestep);

	// Create the meter window and initialize it
	Result = m_Calendar.Initialize(Parent, Instance);

	// If we're running as Litestep's plugin, register the !bangs
	if(!c_DummyLitestep) 
	{
		int Msgs[] = { LM_GETREVID, 0 };
		// Register RevID message to Litestep
		::SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)m_Calendar.GetWindow(), (LPARAM)Msgs);

		AddBangCommand("!RainlendarToggle", RainlendarToggle);
		AddBangCommand("!RainlendarHide", RainlendarHide);
		AddBangCommand("!RainlendarShow", RainlendarShow);
		AddBangCommand("!RainlendarRefresh", RainlendarRefresh);
		AddBangCommand("!RainlendarRedraw", RainlendarRedraw);
		AddBangCommand("!RainlendarConfig", RainlendarConfig);
		AddBangCommand("!RainlendarEditSkin", RainlendarEditSkin);
		AddBangCommand("!RainlendarQuit", RainlendarQuit);
		AddBangCommand("!RainlendarShowNext", RainlendarShowNext);
		AddBangCommand("!RainlendarShowPrev", RainlendarShowPrev);
		AddBangCommand("!RainlendarShowCurrent", RainlendarShowCurrent);
		AddBangCommand("!RainlendarShowMonth", RainlendarShowMonth);
		AddBangCommand("!RainlendarLsBoxHook", RainlendarLsBoxHook);
		AddBangCommand("!RainlendarMove", RainlendarMove);
		AddBangCommand("!RainlendarZPos", RainlendarZPos);
		AddBangCommand("!RainlendarEditTodo", RainlendarEditTodo);
		AddBangCommand("!RainlendarShowTodo", RainlendarShowTodo);
		AddBangCommand("!RainlendarHideTodo", RainlendarHideTodo);
		AddBangCommand("!RainlendarToggleTodo", RainlendarToggleTodo);
		AddBangCommand("!RainlendarShowEventList", RainlendarShowEventList);
		AddBangCommand("!RainlendarHideEventList", RainlendarHideEventList);
		AddBangCommand("!RainlendarToggleEventList", RainlendarToggleEventList);
		AddBangCommand("!RainlendarShowCalendar", RainlendarShowCalendar);
		AddBangCommand("!RainlendarHideCalendar", RainlendarHideCalendar);
		AddBangCommand("!RainlendarToggleCalendar", RainlendarToggleCalendar);
		AddBangCommand("!RainlendarAddEvent", RainlendarAddEvent);
		AddBangCommand("!RainlendarShowItem", RainlendarShowItem);
		AddBangCommand("!RainlendarHideItem", RainlendarHideItem);
		AddBangCommand("!RainlendarToggleItem", RainlendarToggleItem);
		AddBangCommand("!RainlendarChangeSkin", RainlendarChangeSkin);
	}

	return Result;	// Alles OK
}

/* 
** Quit
**
** Called when the module quits
**
*/
void CRainlendar::Quit(HINSTANCE dllInst)
{
	CToolTip::Instance().Finalize();

	// If we're running as Litestep's plugin, unregister the !bangs
	if(!c_DummyLitestep) 
	{
		int Msgs[] = { LM_GETREVID, 0 };
		// Unregister RevID message
		if(m_Calendar.GetWindow()) ::SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)m_Calendar.GetWindow(), (LPARAM)Msgs);

		RemoveBangCommand("!RainlendarToggle");
		RemoveBangCommand("!RainlendarHide");
		RemoveBangCommand("!RainlendarShow");
		RemoveBangCommand("!RainlendarRefresh");
		RemoveBangCommand("!RainlendarRedraw");
		RemoveBangCommand("!RainlendarConfig");
		RemoveBangCommand("!RainlendarEditSkin");
		RemoveBangCommand("!RainlendarQuit");
		RemoveBangCommand("!RainlendarShowNext");
		RemoveBangCommand("!RainlendarShowPrev");
		RemoveBangCommand("!RainlendarShowCurrent");
		RemoveBangCommand("!RainlendarShowMonth");
		RemoveBangCommand("!RainlendarLsBoxHook");
		RemoveBangCommand("!RainlendarMove");
		RemoveBangCommand("!RainlendarZPos");
		RemoveBangCommand("!RainlendarEditTodo");
		RemoveBangCommand("!RainlendarShowTodo");
		RemoveBangCommand("!RainlendarHideTodo");
		RemoveBangCommand("!RainlendarToggleTodo");
		RemoveBangCommand("!RainlendarShowEventList");
		RemoveBangCommand("!RainlendarHideEventList");
		RemoveBangCommand("!RainlendarToggleEventList");
		RemoveBangCommand("!RainlendarShowCalendar");
		RemoveBangCommand("!RainlendarHideCalendar");
		RemoveBangCommand("!RainlendarToggleCalendar");
		RemoveBangCommand("!RainlendarAddEvent");
		RemoveBangCommand("!RainlendarShowItem");
		RemoveBangCommand("!RainlendarHideItem");
		RemoveBangCommand("!RainlendarToggleItem");
		RemoveBangCommand("!RainlendarChangeSkin");
	}

	delete this;

	CConfig::DeleteInstance();

	FinalizeLitestep();

#ifdef _DEBUG
	_CrtMemDumpAllObjectsSince(&g_MemoryState);
#endif
}

void CRainlendar::HideWindow()
{ 
	m_Calendar.HideWindow(); 
	m_Calendar.GetTodoWindow().HideWindow();
	m_Calendar.GetEventListWindow().HideWindow();

	CConfig::Instance().SetCalendarEnable(false);
	CConfig::Instance().SetTodoEnable(false);
	CConfig::Instance().SetEventListEnable(false);
	CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
}

void CRainlendar::ShowWindow(bool activate) 
{ 
	m_Calendar.ShowWindow(activate); 
	CConfig::Instance().SetCalendarEnable(true);

	if (!activate)
	{
		m_Calendar.GetTodoWindow().ShowWindow(false);
		CConfig::Instance().SetTodoEnable(true);

		m_Calendar.GetEventListWindow().ShowWindow(false);
		CConfig::Instance().SetEventListEnable(true);
	}

	CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
}

void CRainlendar::ToggleWindow(bool visible) 
{ 
	if (visible)
	{
		int state = 0;

		// If one of the windows is visible, remember it
		if (CConfig::Instance().GetCalendarEnable())
		{
			state |= (1 << (int)RAINWINDOW_TYPE_CALENDAR);
		}
		if (CConfig::Instance().GetTodoEnable())
		{
			state |= (1 << (int)RAINWINDOW_TYPE_TODO);
		}
		if (CConfig::Instance().GetEventListEnable())
		{
			state |= (1 << (int)RAINWINDOW_TYPE_EVENTLIST);
		}

		if (state != 0)
		{
			// Store the currently visible windows
			CConfig::Instance().SetVisibleWindows(state);
		}
		else
		{
			// Show the windows that were previously hidden
			state = CConfig::Instance().GetVisibleWindows();
		}

		if (state & (1 << (int)RAINWINDOW_TYPE_CALENDAR))
		{
			m_Calendar.ToggleWindow();
			CConfig::Instance().SetCalendarEnable(!CConfig::Instance().GetCalendarEnable());
		}
		if (state & (1 << (int)RAINWINDOW_TYPE_TODO))
		{
			m_Calendar.GetTodoWindow().ToggleWindow();
			CConfig::Instance().SetTodoEnable(!CConfig::Instance().GetTodoEnable());
		}
		if (state & (1 << (int)RAINWINDOW_TYPE_EVENTLIST))
		{
			m_Calendar.GetEventListWindow().ToggleWindow();
			CConfig::Instance().SetEventListEnable(!CConfig::Instance().GetEventListEnable());
		}
	}
	else
	{
		m_Calendar.ToggleWindow(); 
		m_Calendar.GetTodoWindow().ToggleWindow();
		m_Calendar.GetEventListWindow().ToggleWindow();

		CConfig::Instance().SetCalendarEnable(!CConfig::Instance().GetCalendarEnable());
		CConfig::Instance().SetTodoEnable(!CConfig::Instance().GetTodoEnable());
		CConfig::Instance().SetEventListEnable(!CConfig::Instance().GetEventListEnable());
	}

	CConfig::Instance().WriteConfig(CConfig::WRITE_WINDOW_STATE);
}

void CRainlendar::AddEvent(int day, int month, int year)
{
	CFileTime newEventStartTime = m_Calendar.c_TodaysDate;

	if (day > 0) 
	{
		newEventStartTime = CFileTime(day, month, year);
	}

	OpenEditEventDialog(m_Calendar.GetWindow(), CRainlendar::GetInstance(), NULL, &newEventStartTime);
}

void CRainlendar::SetWindowZPos(CConfig::WINDOWPOS pos)
{ 
	m_Calendar.SetWindowZPos(pos);

	if (CConfig::Instance().GetTodoEnable())
	{
		m_Calendar.GetTodoWindow().SetWindowZPos(pos);
	}
	if (CConfig::Instance().GetEventListEnable())
	{
		m_Calendar.GetEventListWindow().SetWindowZPos(pos);
	}
};

void CRainlendar::MoveWindow(int x, int y, RAINWINDOW_TYPE window)
{
	switch(window)
	{
	case RAINWINDOW_TYPE_TODO:
		m_Calendar.GetTodoWindow().MoveWindow(x, y);
		break;

	case RAINWINDOW_TYPE_MESSAGEBOX:
		m_Calendar.GetMessageWindow().MoveWindow(x, y);
		break;

	case RAINWINDOW_TYPE_EVENTLIST:
		m_Calendar.GetEventListWindow().MoveWindow(x, y);
		break;
	
	default:
		m_Calendar.MoveWindow(x, y);
		break;
	}
}
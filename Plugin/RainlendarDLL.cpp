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
  $Header: /home/cvsroot/Rainlendar/Plugin/RainlendarDLL.cpp,v 1.15 2004/01/28 18:06:39 rainy Exp $

  $Log: RainlendarDLL.cpp,v $
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
  Rainlendar.dll uses a bit different interface.
  Added Wharf interface.

  Revision 1.1.1.1  2001/10/29 18:56:24  rainy
  Moved to CVS

*/

#include "RainlendarDLL.h"
#include "CalendarWindow.h"
#include "Error.h"
#include "Tooltip.h"
#include "EditEvent.h"

CRainlendar Rainlendar; // The module
bool CRainlendar::c_DummyLitestep=false;
std::string CRainlendar::c_CmdLine;
HINSTANCE CRainlendar::c_Instance;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	// Store the instance
	CRainlendar::SetInstance(hinstDLL);

    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

CRainlendar* GetRainlendar()
{ 
	return &Rainlendar; 
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
		LSLog(LOG_DEBUG, "Rainlendar", "The initialization started.");
		Result = Rainlendar.Initialize(ParentWnd, dllInst, szPath);
		LSLog(LOG_NOTICE, "Rainlendar", "The initialization finished.");
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
	Rainlendar.Quit(dllInst);
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
	Rainlendar.ToggleWindow();
}

/*
** RainlendarHide
**
** Callback for the !RainlendarHide bang
**
*/
void RainlendarHide(HWND, const char* arg)
{
	Rainlendar.HideWindow();
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
	Rainlendar.ShowWindow(activate);
}

/*
** RainlendarRefresh
**
** Callback for the !RainlendarRefresh bang
**
*/
void RainlendarRefresh(HWND, const char* arg)
{
	Rainlendar.RefreshWindow();
}

/*
** RainlendarConfig
**
** Callback for the !RainlendarConfig bang
**
*/
void RainlendarConfig(HWND, const char* arg)
{
	Rainlendar.ShowConfig();
}

/*
** RainlendarSkinConfig
**
** Callback for the !RainlendarConfig bang
**
*/
void RainlendarEditSkin(HWND, const char* arg)
{
	Rainlendar.ShowEditSkin();
}

/*
** RainlendarQuit
**
** Callback for the !RainlendarQuit bang
**
*/
void RainlendarQuit(HWND, const char* arg)
{
	Rainlendar.QuitRainlendar();
}

/*
** RainlendarShowNext
**
** Callback for the !RainlendarShowNext bang
**
*/
void RainlendarShowNext(HWND, const char* arg)
{
	Rainlendar.ShowNextMonth();
}

/*
** RainlendarShowPrev
**
** Callback for the !RainlendarShowPrev bang
**
*/
void RainlendarShowPrev(HWND, const char* arg)
{
	Rainlendar.ShowPrevMonth();
}

/*
** RainlendarShowCurrent
**
** Callback for the !RainlendarShowCurrent bang
**
*/
void RainlendarShowCurrent(HWND, const char* arg)
{
	Rainlendar.ShowCurrentMonth();
}

/*
** RainlendarShowCurrent
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
				Rainlendar.ShowMonth(Month, Year);
				Rainlendar.RefreshWindow(true);
			}
			else 
			{
				Rainlendar.ShowMonth(Month, -1);
				Rainlendar.RefreshWindow(true);
			}
		}
	} 
	else 
	{
		Rainlendar.ShowMonth(-1, -1);
		Rainlendar.RefreshWindow(true);
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
	char* handle = strrchr(arg,' ');
	
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
		int x = 0, y = 0;
		sscanf(arg, "%i %i", &x, &y);
		Rainlendar.MoveWindow(x, y);
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
		Rainlendar.SetWindowZPos(pos);
	} 
}

/*
** RainlendarTodo
**
** Callback for the !RainlendarTodo bang
**
*/
void RainlendarTodo(HWND, const char* arg)
{
	Rainlendar.ShowTodo();
}

/*
** RainlendarEditTodo
**
** Callback for the !RainlendarEditTodo bang
**
*/
void RainlendarEditTodo(HWND, const char* arg)
{
	Rainlendar.EditTodo();
}

/*
** RainlendarShowTodo
**
** Callback for the !RainlendarShowTodo bang
**
*/
void RainlendarShowTodo(HWND, const char* arg)
{
	Rainlendar.ShowTodo();
}

/*
** RainlendarHideTodo
**
** Callback for the !RainlendarHideTodo bang
**
*/
void RainlendarHideTodo(HWND, const char* arg)
{
	Rainlendar.HideTodo();
}

/*
** RainlendarToggleTodo
**
** Callback for the !RainlendarToggleTodo bang
**
*/
void RainlendarToggleTodo(HWND, const char* arg)
{
	Rainlendar.ToggleTodo();
}

/*
** RainlendarAddEvent
**
** Callback for the !RainlendarAddEvent bang
**
*/
void RainlendarAddEvent(HWND, const char* arg)
{
	char Buffer[256];
	char* Token;
	int date;

	if(arg!=NULL && arg[0]!='\0') 
	{
		strncpy(Buffer, arg, 255);
		Buffer[255] = '\0';
		Token = strtok(Buffer, " ");
		
		if(Token!=NULL) 
		{
			date = atoi(Token);
			Token = strtok(NULL, " ");
			
			if(Token != NULL) 
			{
				Rainlendar.AddEvent(date, Token);
				return;
			}
			else 
			{
				Rainlendar.AddEvent(date, NULL);
				return;
			}
		}
	} 

	Rainlendar.AddEvent(0, NULL);
}

/*
** RainlendarShowEvents
**
** Callback for the !RainlendarShowEvents bang
**
*/
void RainlendarShowEvents(HWND, const char* arg)
{
	Rainlendar.ShowEvents();
}


/* 
** CRainlendar
**
** Constructor
**
*/
CRainlendar::CRainlendar()
{
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

	if(!c_DummyLitestep) InitalizeLitestep();

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
		AddBangCommand("!RainlendarAddEvent", RainlendarAddEvent);
		AddBangCommand("!RainlendarShowEvents", RainlendarShowEvents);
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
		RemoveBangCommand("!RainlendarAddEvent");
		RemoveBangCommand("!RainlendarShowEvents");
	}
}

void CRainlendar::HideWindow()
{ 
	m_Calendar.HideWindow(); 
	if (CConfig::Instance().GetTodoEnable()) 
	{
		m_Calendar.GetTodoWindow().HideWindow();
	}
}

void CRainlendar::ShowWindow(bool activate) 
{ 
	m_Calendar.ShowWindow(activate); 
	if (CConfig::Instance().GetTodoEnable()) 
	{
		m_Calendar.GetTodoWindow().ShowWindow(activate);
	}
}

void CRainlendar::ToggleWindow() 
{ 
	m_Calendar.ToggleWindow(); 
	if (CConfig::Instance().GetTodoEnable()) 
	{
		m_Calendar.GetTodoWindow().ToggleWindow();
	}
}

void CRainlendar::AddEvent(int date, const char* message)
{
	if (message && message[0] != 0) 
	{
		int day, month, year;
		CEventMessage event;
		event.SetDate(date);
		event.SetMessage(message);
		m_Calendar.GetEventManager()->AddEvent(event);
		CEventMessage::ValueToDate(date, &day, &month, &year);
		m_Calendar.GetEventManager()->WriteEvents(day, month, year);
		RefreshWindow();
	} 
	else
	{
		if (date == 0) 
		{
			SYSTEMTIME today;
			GetLocalTime(&today);
			date = CEventMessage::DateToValue(today.wDay, today.wMonth, today.wYear);
		}

		OpenEditEventDialog(m_Calendar.GetWindow(), CRainlendar::GetInstance(), date, 0);	
	}
}

void CRainlendar::ShowEvents()
{
	SYSTEMTIME today;
	GetLocalTime(&today);
	std::vector<CEventMessage*> eventList = m_Calendar.GetEventManager()->GetEvents(today.wDay, today.wMonth, today.wYear);
	
	m_Calendar.GetMessageWindow().ClearEvents();
	m_Calendar.ShowEventMessage(eventList, true);
}

void CRainlendar::SetWindowZPos(CConfig::WINDOWPOS pos)
{ 
	m_Calendar.SetWindowZPos(pos);

	if (CConfig::Instance().GetTodoEnable())
	{
		m_Calendar.GetTodoWindow().SetWindowZPos(pos);
	}
};

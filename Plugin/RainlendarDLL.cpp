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
  $Header: \\\\RAINBOX\\cvsroot/Rainlendar/Plugin/RainlendarDLL.cpp,v 1.5 2002/05/30 18:25:44 rainy Exp $

  $Log: RainlendarDLL.cpp,v $
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

CRainlendar Rainlendar; // The module
bool CRainlendar::c_DummyLitestep=false;
std::string CRainlendar::c_CmdLine;

CRainlendar* GetRainlendar()
{ 
	return &Rainlendar; 
};

/*
** initWharfModule
**
** This function is called when the plugin is initialized by wharf or lsbox.
**
*/
int initWharfModule(HWND ParentWnd, HINSTANCE dllInst, void* wd)
{
	int Result=1;
	
	try 
	{
		Result = Rainlendar.Initialize(ParentWnd, dllInst, wd != NULL, NULL);
	} 
    catch(CError& error) 
    {
		MessageBox(ParentWnd, error.GetString().c_str(), APPNAME, MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
	}

	return Result;
}

/*
** quitWharfModule
**
** This is called when the wharf plugin quits.
**
*/
void quitWharfModule(HINSTANCE dllInst)
{
	quitModule(dllInst);
}

/*
** getLSRegion
**
** Returns the window region for the wharf
**
*/
HRGN getLSRegion(int xOffset, int yOffset)
{
	return Rainlendar.GetRegion(xOffset, yOffset);

	return NULL;
}

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
		Result = Rainlendar.Initialize(ParentWnd, dllInst, NULL, szPath);
	} 
    catch(CError& error) 
    {
		MessageBox(ParentWnd, error.GetString().c_str(), APPNAME, MB_OK | MB_TOPMOST | MB_ICONEXCLAMATION);
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
	CRainlendar::SetCommandLine(CmdLine);
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
	Rainlendar.ShowWindow();
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
	
	
	{
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
				}
				else 
				{
					Rainlendar.ShowMonth(Month, -1);
				}
			}
		} 
		else 
		{
			Rainlendar.ShowMonth(-1, -1);
		}
	}
}

/* 
** CRainlendar
**
** Constructor
**
*/
CRainlendar::CRainlendar()
{
	m_Wharf = false;
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
int CRainlendar::Initialize(HWND Parent, HINSTANCE Instance, bool wd, LPCSTR szPath)
{
	int Result=0;

	if(Parent==NULL || Instance==NULL) 
	{
		throw CError(CError::ERR_NULLPARAMETER, __LINE__, __FILE__);
	}	

	m_Wharf = wd;

	if(!c_DummyLitestep) InitalizeLitestep();

	// Create the meter window and initialize it
	Result = m_Calendar.Initialize(*this, Parent, Instance);

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
	}
}

/* 
** GetRegion
**
** Function returns the window region for the Wharf. Probably should 
** add the implementation to here for better combatibility with wharf 
** and lsbox.
**
*/
HRGN CRainlendar::GetRegion(int xOffset, int yOffset)
{
	return NULL;
}


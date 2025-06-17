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
  $Header: /home/cvsroot/Rainlendar/Server/main.cpp,v 1.2 2005/07/21 15:01:10 rainy Exp $

  $Log: main.cpp,v $
  Revision 1.2  2005/07/21 15:01:10  rainy
  no message

  Revision 1.1.1.1  2005/07/10 18:48:07  rainy
  no message

  Revision 1.15  2005/03/01 19:56:58  rainy
  *** empty log message ***

  Revision 1.14  2005/03/01 19:46:32  rainy
  *** empty log message ***

  Revision 1.13  2005/03/01 19:33:51  rainy
  *** empty log message ***

  Revision 1.12  2005/03/01 17:43:20  rainy
  Linux fixes

  Revision 1.11  2005/03/01 17:02:29  rainy
  Added Windows service support.

  Revision 1.10  2004/11/14 12:12:10  rainy
  Linux fixes

  Revision 1.9  2004/11/14 11:38:07  rainy
  Version 0.6

  Revision 1.8  2004/04/24 11:45:29  rainy
  Added -m flag.

  Revision 1.7  2003/08/23 09:16:06  Rainy
  0.4.1

  Revision 1.6  2003/08/09 15:24:30  Rainy
  Version 0.4

  Revision 1.5  2003/05/25 18:12:58  Rainy
  The filter is deleter at the end.

  Revision 1.4  2003/05/25 15:13:02  Rainy
  Added fltering support.

  Revision 1.3  2003/05/24 13:51:51  Rainy
  Increased the version number.

  Revision 1.2  2002/08/03 16:13:48  rainy
  Added possiblity to log to a file.
  Added mutex to check if the server is already running.

  Revision 1.1  2002/01/27 16:21:04  rainy
  Initial version

*/

#include <stdio.h>
#ifndef _WIN32
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif 

#include <simpleserver.h>
#include "RainlendarServer.h"
#include "IPFilter.h"

using namespace ssobjects;

#define VERSION "0.8"
#define SERVICENAME "RainlendarServer"
#define SERVICEDISPLAYNAME "Rainlendar Server"

enum MANAGE_SERVICE
{
	MANAGE_SERVICE_REMOVE,
	MANAGE_SERVICE_START,
	MANAGE_SERVICE_STOP
};

void initDaemon();
void InstallService();
void ManageService(MANAGE_SERVICE task);
void RunService();
int RunServer();

static int g_Argc = 0;
static char** g_Argv = NULL;

int main(int argc, char* argv[])
{
	g_Argc = argc;
	g_Argv = argv;

#ifdef _WIN32
	// Parse command line
	for(int i = 1; i < argc; i++)
	{
		char* a = argv[i];

		if(!strcmp(a, "--runservice"))
		{
			RunService();
			return 0;
		}
		else if(!strcmp(a, "--install"))
		{
			InstallService();
			return 0;
		}
		else if(!strcmp(a, "--remove"))
		{
			ManageService(MANAGE_SERVICE_REMOVE);
			return 0;
		}
		else if(!strcmp(a, "--start"))
		{
			ManageService(MANAGE_SERVICE_START);
			return 0;
		}
		else if(!strcmp(a, "--stop"))
		{
			ManageService(MANAGE_SERVICE_STOP);
			return 0;
		}
	}
#endif

	return RunServer();
}

int RunServer()
{
	int port = 9999;
	bool daemonize = false;
	bool single = true;
	CStr logFile;
	CStr targetPath;
	CIPFilter* filter = NULL;

	printf("Rainlendar Server version %s\n", VERSION);

	// Parse command line
	for(int i = 1; i < g_Argc; i++)
	{
		char* a = g_Argv[i];

		if(!strcmp(a, "-h") || !strcmp(a, "--help") || !strcmp(a, "/?"))
		{
			printf("\nUsage: %s [options]\n", g_Argv[0]);
			puts  (" -h,--help                = this message");
			puts  (" -p,--port <port>         = run server on port <port>");
#ifndef _WIN32
			puts  (" -d,--daemonize           = daemonize server");
#endif
			puts  (" -t,--target <folder>     = The place where the database files are stored");
			puts  ("                            (it's your home folder by default)");
			puts  (" -l,--log <folder>        = write log to a file instead to console");
			puts  ("                            (don't use spaces in the folder name)");
			puts  (" -f,--filter <filterfile> = use the given file as the ip-filter");
			puts  ("                            (don't use spaces in the file name)");
#ifdef _WIN32
			puts  (" -m,--multi               = allow multiple instances");
			puts  (" --install                = install as service");
			puts  (" --remove                 = remove service");
			puts  (" --start                  = start service");
			puts  (" --stop                   = stop service");
#endif
			return 0;
		}
		else if(!strcmp(a, "-p") || !strcmp(a, "--port"))
		{
			if(++i >= g_Argc) 
			{
				printf("\n\"%s\" is invalid.\n", a);
				return 0;
			}
			port = atoi(g_Argv[i]);
		}
		else if(!strcmp(a, "-d") || !strcmp(a, "--daemonize"))
		{
			daemonize = true;
		}
		else if(!strcmp(a, "-m") || !strcmp(a, "--multi"))
		{
			single = false;
		}
		else if(!strcmp(a, "-l") || !strcmp(a, "--log"))
		{
			if(++i >= g_Argc) 
			{
				printf("\n\"%s\" is invalid.\n", a);
				return 0;
			}
			logFile = g_Argv[i];

			if(logFile.isNotEmpty() && (logFile.right(1) != "\\" || logFile.right(1) != "/"))
			{
				logFile += "/";
			}
		}
		else if(!strcmp(a, "-f") || !strcmp(a, "--filter"))
		{
			if(++i >= g_Argc) 
			{
				printf("\n\"%s\" is invalid.\n", a);
				return 0;
			}

			filter = new CIPFilter();
			if (!filter->Parse(g_Argv[i]))
			{
				delete filter;
				filter = NULL;
			}
		}
		else if(!strcmp(a, "-t") || !strcmp(a, "--target"))
		{
			if(++i >= g_Argc) 
			{
				printf("\n\"%s\" is invalid.\n", a);
				return 0;
			}
			targetPath = g_Argv[i];
			if(targetPath.isNotEmpty() && (targetPath.right(1) != "\\" || targetPath.right(1) != "/"))
			{
				targetPath += "/";
			}
		}
	}
	
#ifdef _WIN32
	HANDLE h = NULL;

	if (single)
	{
		// Check if RainlendarServer is already running
		h = CreateMutex(NULL, TRUE, "RAINLENDARSERVER_IS_RUNNING_MUTEX" );
		if (!h)
		{
			printf("Unable to create mutex.\n");
			// We'll continue anyway
		}
		else
		{
			if ( GetLastError() == ERROR_ALREADY_EXISTS )
			{
				// already an instance running.
				CloseHandle(h);
				printf("RainlendarServer is already running. Exiting.\n");
				return 0;
			}
		}
	}

	pthread_win32_process_attach_np();
#endif

	// Note that this SockAddr is okay here, as we are not specifying an ip address.
	// You should first construct the server before using SockAddr 
	// like "SockAddr("www.something.com",80)".
	SockAddr saBind((ULONG)INADDR_ANY, port);
	try
	{
		// All server output is routed through logs object. That way we can have output
		// sent to a file, console or both. Especially useful when you are running
		// in daemon mode, when you don't want any output on the console.
		if(logFile.isNotEmpty())
		{
			// Enable log file if we are running as a daemon.
			// Output goes to a file and to console by default.
			logFile += "RainlendarServer";
			logs::init(logFile);
			logs::enable(true);
			logs::set(logs::L_FILE, logs::L_ALL);  // output only log to a file now
		}

		if (targetPath.isEmpty())
		{
#ifdef _WIN32
			char buffer[MAX_PATH + 1];
			buffer[0] = 0;
			ExpandEnvironmentStrings("%APPDATA%", buffer, MAX_PATH);
			if (strcmp("%APPDATA%", buffer) == 0)
			{
				// No env var (Running as a service?) -> Use server's folder
				GetModuleFileName(NULL, buffer, MAX_PATH);
				char* pos = strrchr(buffer, '\\');
				if (pos)
				{
					pos++;
					*pos = 0;
				}
				else
				{
					*buffer = 0;
				}
			}
			else
			{
				strcat(buffer, "\\RainlendarServer\\");
				CreateDirectory(buffer, NULL);
			}
			targetPath = buffer;
#else
			targetPath = getenv("HOME");
			targetPath += "/.rainlendarserver/";
			mkdir(targetPath, 0700);
#endif
		}

		// In win32 it's important to construct the server BEFORE calling canBind, or any
		// other socket operation, as the server constructor contains a call to WSAStartup,
		// the required call to initialize windows socket layer.
		// Linux is okay either way.
		CRainlendarServer server(saBind, filter, targetPath);

		if(!SimpleServer::canBind(saBind))  // check if we can bind to this port
		{
			throwGeneralException("Can't bind");       // should not throw from main after server constructed
		}
		else
		{
			logs::log("Server on port %d\n", port);
			
#ifndef _WIN32
			if(daemonize)
			{
				logs::logln("Switching to daemon process...");
				initDaemon();
			}
#endif            
			server.startServer();               // server will now listen for connections
			
			logs::logln("server is finished.");
		}
	}
	catch(GeneralException& e)
	{
		// all errors use exceptions instead of return codes
		LOG("caught exception [%s]",e.getErrorMsg());
	}

	delete filter;

	filter = NULL;
#ifdef _WIN32
    if(h) CloseHandle(h);
	pthread_win32_process_detach_np();
#endif

	return 0;
}

#ifndef _WIN32

//From page 336 of 
//Unix Network Programming Network API's: Sockets and XTI by
//W. Richard Stevens:
//
//The purpose of the second fork is to guarantee that the daemon cannnot
//automatically aquire a controlling terminal should it open a terminal
//device in the future. Under SVR4, when a session leader without a
//controlling terminal opens a terminal device (that is not currently
//some other session's controlling terminal), the terminal becomes the controlling 
///terminal of the session leader. But by calling fork a second time,
//we guarantee that the second child is no longer a session leader, so it
//cannot acquire a contolling terminal. We must ignore SIGHUP because
//when a session leader terminates (the first child), all processes in the 
//session (our second child) are sent the SIGHUP signal. 

void initDaemon()
{
	pid_t pid;
	
	//  close(0);
	//  close(1);
	//  close(2);
	
	if((pid = fork())!=0)
		exit(0);    //parent terminates
	
	//1st child continues
	setsid();
	signal(SIGHUP,SIG_IGN);
    
	//see comment above for second for explaination
	if((pid = fork())!=0)
		exit(0);    //1st child terminates
	
	//2nd child continues
	umask(0);       //clear our file mode creation flag
	
}

#else	// _WIN32

// Service code from BareServ by R.M.Moore
// http://bobmoore.mvps.org

static SERVICE_STATUS_HANDLE g_hServiceStatus = NULL;
static HANDLE g_hTerminateEvent = NULL;
static HANDLE g_hThread = NULL;
static BOOL g_bServiceRunning = FALSE; 
static BOOL g_bServicePaused = FALSE;

void SendStatusToSCM(DWORD dwCurrentState,
                     DWORD dwWin32ExitCode, 
                     DWORD dwCheckPoint,
                     DWORD dwWaitHint)
{
	SERVICE_STATUS serviceStatus;
	
	serviceStatus.dwServiceType  = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = dwCurrentState;
	
	if (dwCurrentState == SERVICE_START_PENDING)
	{
		serviceStatus.dwControlsAccepted = 0;
	}
	else
	{
		serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;
	}

	serviceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = dwCheckPoint;
	serviceStatus.dwWaitHint = dwWaitHint;
	
	if (!SetServiceStatus (g_hServiceStatus, &serviceStatus))
	{
		ManageService(MANAGE_SERVICE_STOP);
	}
}

void InstallService()
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (hSCM)
	{
		std::string serviceName;
		char filename[MAX_PATH];
		GetModuleFileName(NULL, filename, sizeof(filename));

		serviceName = "\"";
		serviceName += filename;
		serviceName += "\"";
		serviceName += " --runservice";

		// Add the given arguments
		for(int i = 1; i < g_Argc; i++)
		{
			char* a = g_Argv[i];
			if (stricmp(a, "--install") != 0)
			{
				serviceName += " ";
				serviceName += a;
			}
		}

		SC_HANDLE hService = CreateService(hSCM,
			SERVICENAME,
			SERVICEDISPLAYNAME,
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			serviceName.c_str(),
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);

		if (hService)
		{
			printf("RainlendarServer was successfully installed as service.");
			CloseServiceHandle(hService);
		}
		else
		{
			printf("Unable to install the service [%u]", GetLastError());
		}

		CloseServiceHandle(hSCM);
	}
	else
	{
		printf("Cannot establish a connection to the service control manager [%u]", GetLastError());
	}
}

void ManageService(MANAGE_SERVICE task)
{
	SERVICE_STATUS ss;

	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (hSCM)
	{
		SC_HANDLE hService = OpenService(hSCM, SERVICENAME, SERVICE_ALL_ACCESS);
		if (hService)
		{
			switch (task) 
			{
			case MANAGE_SERVICE_REMOVE:
				if (DeleteService(hService))
				{
					printf("RainlendarServer was successfully removed from services.");
				}
				else
				{
					printf("Cannot remove the service [%u]", GetLastError());
				}
				break;

			case MANAGE_SERVICE_START:
				if (StartService(hService, 0, NULL))
				{
					printf("Service was successfully started.");
				}
				else
				{
					printf("Cannot start the service [%u]", GetLastError());
				}
				break;

			case MANAGE_SERVICE_STOP:
				if (ControlService(hService, SERVICE_CONTROL_STOP, &ss))
				{
					printf("Service was successfully stopped.");
				}
				else
				{
					printf("Cannot stop the service [%u]", GetLastError());
				}
				break;
			}

			CloseServiceHandle(hService);
		}
		else
		{
			printf("Cannot open the service [%u]", GetLastError());
		}
		CloseServiceHandle(hSCM);
	}
	else
	{
		printf("Cannot establish a connection to the service control manager [%u]", GetLastError());
	}
}

void CleanUp(DWORD error)
{
	if (g_hTerminateEvent) CloseHandle(g_hTerminateEvent);
	if (g_hServiceStatus) SendStatusToSCM(SERVICE_STOPPED, error, 0, 0);
	if (g_hThread) CloseHandle(g_hThread);
}

DWORD ServiceThread(LPDWORD param)
{
	RunServer();
	return 0;
}

bool InitService()
{
	DWORD id;
	
	g_hThread = CreateThread (0, 
		0,
		(LPTHREAD_START_ROUTINE)ServiceThread,
		0, 
		0, 
		&id);
	
	if (g_hThread)
	{
		g_bServiceRunning = TRUE;
		return true;
	}
	else
	{
		LOG("Unable create the service thread [%i]", GetLastError());
	}
	return false;
}

void PauseService()
{
   g_bServicePaused = TRUE;
   SuspendThread(g_hThread);
}

void ResumeService()
{
   g_bServicePaused = FALSE;
   ResumeThread(g_hThread);
}

void StopService() 
{
	g_bServiceRunning = FALSE;
	SetEvent(g_hTerminateEvent);
}

VOID ServiceCtrlHandler(DWORD controlCode) 
{
	DWORD currentState = 0;
	
	switch (controlCode)
	{
	case SERVICE_CONTROL_STOP:
		currentState = SERVICE_STOP_PENDING;
		SendStatusToSCM(SERVICE_STOP_PENDING, NO_ERROR, 1, 5000);
		StopService();
		return;
		
	case SERVICE_CONTROL_PAUSE:
		if (g_bServiceRunning && (!g_bServicePaused))
		{
			SendStatusToSCM(SERVICE_PAUSE_PENDING, NO_ERROR, 1, 5000);
			PauseService();
			currentState = SERVICE_PAUSED;
		}
		break;
		
	case SERVICE_CONTROL_CONTINUE:
		if (g_bServiceRunning && g_bServicePaused)
		{
			SendStatusToSCM(SERVICE_CONTINUE_PENDING, NO_ERROR, 1, 5000);
			ResumeService();
			currentState = SERVICE_RUNNING;
		}
		break;
	}
	SendStatusToSCM(currentState, NO_ERROR, 0, 0);
}

VOID ServiceMain(DWORD argc, LPTSTR *argv) 
{
	g_hServiceStatus = RegisterServiceCtrlHandler(SERVICENAME, (LPHANDLER_FUNCTION)ServiceCtrlHandler);
	if (g_hServiceStatus)
	{
		SendStatusToSCM(SERVICE_START_PENDING, NO_ERROR, 1, 5000);

		g_hTerminateEvent = CreateEvent (0, TRUE, FALSE, 0);
		if (g_hTerminateEvent)
		{
			SendStatusToSCM(SERVICE_START_PENDING, NO_ERROR, 2, 1000);
			SendStatusToSCM(SERVICE_START_PENDING, NO_ERROR, 3, 5000);

			if (InitService())
			{
				SendStatusToSCM(SERVICE_RUNNING,NO_ERROR, 0, 0);

				WaitForSingleObject (g_hTerminateEvent, INFINITE);
				CleanUp(0);
			}
			else
			{
				CleanUp(GetLastError());
			}
		}
		else
		{
			CleanUp(GetLastError());
		}
	}
	else
	{
		CleanUp(GetLastError());
	}
}

void RunService()
{
	SERVICE_TABLE_ENTRY srvTable[2]; 
	
	srvTable[0].lpServiceName = SERVICENAME;
	srvTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
	srvTable[1].lpServiceName = NULL;
	srvTable[1].lpServiceProc = NULL;

	if (!StartServiceCtrlDispatcher(srvTable))
	{
		printf("Unable to start the service control dispatcher [%u]", GetLastError());
	}
}

#endif

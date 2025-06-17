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
  $Header: //RAINBOX/cvsroot/Rainlendar/Server/main.cpp,v 1.6 2003/08/09 15:24:30 Rainy Exp $

  $Log: main.cpp,v $
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

#define VERSION "0.4"

void initDaemon();

int main(int argc, char* argv[])
{
	int port = 9999;
	bool daemonize = false;
	CStr logFile;
	CIPFilter* filter = NULL;

	printf("Rainlendar Server version %s\n", VERSION);

#ifdef _WIN32
	// Check if RainlendarServer is already running
	HANDLE h = CreateMutex(NULL, TRUE, "RAINLENDARSERVER_IS_RUNNING_MUTEX" );
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
#endif

	// Parse command line
	for(int i = 1; i < argc; i++)
	{
		char* a = argv[i];

		if(!strcmp(a, "-h") || !strcmp(a, "--help") || !strcmp(a, "/?"))
		{
			printf("\nUsage: %s [options]\n", argv[0]);
			puts  (" -h,--help                = this message");
			puts  (" -p,--port <port>         = run server on port <port>");
			puts  (" -d,--daemonize           = daemonize server (not available in win32)");
			puts  (" -l,--log <folder>        = write log to a file instead to console");
			puts  (" -f,--filter <filterfile> = use the given file as the ip-filter");

			return 0;
		}
		else if(!strcmp(a, "-p") || !strcmp(a, "--port"))
		{
			if(++i >= argc) 
			{
				printf("\n\"%s\" is invalid.\n", a);
				return 0;
			}
			port = atoi(argv[i]);
		}
		else if(!strcmp(a, "-d") || !strcmp(a, "--daemonize"))
		{
			daemonize = true;
		}
		else if(!strcmp(a, "-l") || !strcmp(a, "--log"))
		{
			if(++i >= argc) 
			{
				printf("\n\"%s\" is invalid.\n", a);
				return 0;
			}
			logFile = argv[i];

			if(logFile.isNotEmpty() && (logFile.right(1) != "\\" || logFile.right(1) != "/"))
			{
				logFile += "/";
			}
		}
		else if(!strcmp(a, "-f") || !strcmp(a, "--filter"))
		{
			if(++i >= argc) 
			{
				printf("\n\"%s\" is invalid.\n", a);
				return 0;
			}

			filter = new CIPFilter();
			if (!filter->Parse(argv[i]))
			{
				delete filter;
				filter = NULL;
			}
		}
	}
	
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
			logFile += "Rainlendar";
			logs::init(logFile);
			logs::enable(true);
			logs::set(logs::L_FILE, logs::L_ALL);  // output only log to a file now
		}
		
		// In win32 it's important to construct the server BEFORE calling canBind, or any
		// other socket operation, as the server constructor contains a call to WSAStartup,
		// the required call to initialize windows socket layer.
		// Linux is okay either way.
		CRainlendarServer server(saBind, filter);

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

#ifdef _WIN32
     if(h) CloseHandle(h);
#endif

	delete filter;
	filter = NULL;

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

#endif

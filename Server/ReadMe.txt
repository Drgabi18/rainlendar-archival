RainlendarServer 0.5
--------------------

This is the server part of the Rainlendar calendar. You only need this if you
are going to use the network distribution of the events.

There is some additional information in the Rainlendar's documentation, which
I suggest that you read before trying to run this server.

The server (and client) needs Simple Server Objects by Lee Patterson 
(http://ssobjects.sourceforge.net/). The server also need PThreads
(http://sources.redhat.com/pthreads-win32/). The linux version
links everything statically so you should need anything to run
the server (just to compile it). The Windows version needs 
pthreadVCE.dll.


Usage
-----

Just run RainlendarServer.exe (or ./rainlendarserver on Linux).

The server takes few optional command line arguments.

 -h,--help                = this message
 -p,--port <port>         = run server on port <port>
 -d,--daemonize           = daemonize server (not available in win32)
 -l,--log <folder>        = write log to a file instead to console
 -f,--filter <filterfile> = use the given file as the ip-filter
 -m,--multi               = allow multiple instances


If you want to run RainlendarServer as a Windows NT service, here is
how to do that:

- First get your hands on servany.exe and instsrv.exe
  If you have trouble finding those, try these links:
  ftp://ftp.microsoft.com/bussys/winnt/winnt-public/reskit/nt40/i386/
  http://www.swynk.com/trent/Sections/SMSInstKit.asp

- Copy the servany.exe to somewhere in your hard drive.

- Create a service from it with the instsrv.exe
  E.g. INSTSRV.EXE RainlendarServer D:\Tools\srvany.exe

- Run the registry editor and browse to
  HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\RainlendarServer

- Create 'Parameters' key

- Under Parameters key create an 'Application' value of type REG_SZ and 
  specify there the full path of RainlendarServer executable.
  E.g. Application: REG_SZ: D:\Tools\Rainlendar\RainlendarServer.exe

- Under Parameters key create an 'AppDirectory' value of type REG_SZ and 
  specify there the working path of the RainlendarServer.
  E.g. AppDirectory: REG_SZ: D:\Tools\Rainlendar

- If you want to give arguments to the RainlendarServer (e.g. -l to
  put the log into a file) create an 'AppParameters' value of type REG_SZ 
  and specify there the command line arguments.

- Open Control Panel -> Administrative Tools -> Services

- Open RainlendarServer services Properties 

- Set startup type to Automatic if you want to start the server every time
  you start your computer.



... or you can get Firedaemon (http://www.firedaemon.com) and use that.


History
-------

[21.4.2004] Version 0.5
- Added -m flag

[9.8.2003] Version 0.4
- Added CreatedBy and LastModified fields to the events-file.

[15.6.2003] Version 0.3
- Fixed a security invulnerability in the server.
- Added IP-filter to the server.
- The log contains a bit more information about the clients.

[10.8.2002] Version 0.2
- The server is not started if it's already running (on Windows).
- Added possibility to direct the log into a file.
- Changed to support the repeating events (the protocol is different now!)

[2.3.2002] Version 0.1
- Initial Version

RainlendarServer 0.1
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
Use -h to find out more.


History
-------

[2.3.2002]
- Initial Version

# Microsoft Developer Studio Project File - Name="RainlendarDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=RainlendarDLL - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RainlendarDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RainlendarDLL.mak" CFG="RainlendarDLL - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RainlendarDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RainlendarDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RainlendarDLL - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40b /d "NDEBUG"
# ADD RSC /l 0x40b /i "D:\Development\Microsoft Visual Studio\VC98\MFC\Include\l.kor\\" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 lsapi.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"../Release/Rainlendar.dll" /libpath:"e:\Projects\3rdparty\ls-b24\lsapi\release\\"

!ELSEIF  "$(CFG)" == "RainlendarDLL - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RainlendarDLL___Win32_Debug"
# PROP BASE Intermediate_Dir "RainlendarDLL___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /Gf /Gy /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40b /d "NDEBUG"
# ADD RSC /l 0x40b /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 lsapi.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"../Release/Rainlendar.dll" /libpath:"e:\Projects\ls-b24\lsapi\release\\"
# ADD LINK32 lsapi.lib /nologo /subsystem:windows /dll /incremental:yes /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"Debug/Rainlendar.dll" /libpath:"e:\Projects\3rdparty\ls-b24\lsapi\debug\\"
# SUBTRACT LINK32 /profile /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy            .\Debug\Rainlendar.dll            ..\Debug\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "RainlendarDLL - Win32 Release"
# Name "RainlendarDLL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Background.cpp
# End Source File
# Begin Source File

SOURCE=.\CalendarWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogDays.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogMonth.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogToday.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogWeekdays.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogYear.cpp
# End Source File
# Begin Source File

SOURCE=.\EditEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\Event.cpp
# End Source File
# Begin Source File

SOURCE=.\Item.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemDays.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemMonth.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemToday.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemWeekdays.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemYear.cpp
# End Source File
# Begin Source File

SOURCE=.\RainlendarDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\Rasterizer.cpp
# End Source File
# Begin Source File

SOURCE=.\RasterizerBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\RasterizerFont.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Background.h
# End Source File
# Begin Source File

SOURCE=.\CalendarWindow.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\DialogDays.h
# End Source File
# Begin Source File

SOURCE=.\DialogEvent.h
# End Source File
# Begin Source File

SOURCE=.\DialogGeneral.h
# End Source File
# Begin Source File

SOURCE=.\DialogMonth.h
# End Source File
# Begin Source File

SOURCE=.\DialogToday.h
# End Source File
# Begin Source File

SOURCE=.\DialogWeekdays.h
# End Source File
# Begin Source File

SOURCE=.\DialogYear.h
# End Source File
# Begin Source File

SOURCE=.\EditEvent.h
# End Source File
# Begin Source File

SOURCE=.\Error.h
# End Source File
# Begin Source File

SOURCE=.\Event.h
# End Source File
# Begin Source File

SOURCE=.\Item.h
# End Source File
# Begin Source File

SOURCE=.\ItemDays.h
# End Source File
# Begin Source File

SOURCE=.\ItemEvent.h
# End Source File
# Begin Source File

SOURCE=.\ItemMonth.h
# End Source File
# Begin Source File

SOURCE=.\ItemToday.h
# End Source File
# Begin Source File

SOURCE=.\ItemWeekdays.h
# End Source File
# Begin Source File

SOURCE=.\ItemYear.h
# End Source File
# Begin Source File

SOURCE=.\RainlendarDLL.h
# End Source File
# Begin Source File

SOURCE=.\Rasterizer.h
# End Source File
# Begin Source File

SOURCE=.\RasterizerBitmap.h
# End Source File
# Begin Source File

SOURCE=.\RasterizerFont.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RainlendarDLL.rc

!IF  "$(CFG)" == "RainlendarDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "RainlendarDLL - Win32 Debug"

# ADD BASE RSC /l 0x40b
# ADD RSC /l 0x40b /i "D:\Development\Microsoft Visual Studio\VC98\MFC\Include\l.kor\\"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\RainlendarDLL.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=..\Debug\Events.ini
# End Source File
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\Manual.html
# End Source File
# Begin Source File

SOURCE=.\ToDo.txt
# End Source File
# End Target
# End Project

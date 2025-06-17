# Microsoft Developer Studio Project File - Name="Plugin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Plugin - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Plugin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Plugin.mak" CFG="Plugin - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Plugin - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Plugin - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Plugin - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "..\..\..\3rdparty\ssobjects\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40b /d "NDEBUG"
# ADD RSC /l 0x40b /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib Advapi32.lib Ws2_32.lib ssobjects.lib comdlg32.lib comctl32.lib shell32.lib /nologo /dll /machine:I386 /nodefaultlib:"LIBCMT.lib" /out:"Release/Rainlendar.dll" /libpath:"..\..\..\3rdparty\ssobjects"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=Copy          Release\Rainlendar.dll          ..\TestBench\Rainlendar.dll         	Copy          Release\Rainlendar.dll               C:\Litestep\Themes\dArk\modules\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Plugin - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\3rdparty\ssobjects\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40b /d "_DEBUG"
# ADD RSC /l 0x40b /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib Advapi32.lib Ws2_32.lib ssobjects.lib comdlg32.lib comctl32.lib shell32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"LIBCMT.lib" /out:"Debug/Rainlendar.dll" /pdbtype:sept /libpath:"..\..\..\3rdparty\ssobjects"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=Copy          Debug\Rainlendar.dll          ..\TestBench\Rainlendar.dll         	Copy          Debug\Rainlendar.dll               C:\Litestep\Themes\dArk\modules\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Plugin - Win32 Release"
# Name "Plugin - Win32 Debug"
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

SOURCE=.\ConfigDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\EditEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\Error.cpp
# End Source File
# Begin Source File

SOURCE=.\Event.cpp
# End Source File
# Begin Source File

SOURCE=..\Server\EventCombiner.cpp
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

SOURCE=.\ItemWeekNumbers.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemYear.cpp
# End Source File
# Begin Source File

SOURCE=.\Litestep.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkThread.cpp
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

SOURCE=.\SkinDialog.cpp
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

SOURCE=.\ConfigDialog.h
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

SOURCE=.\ItemWeekNumbers.h
# End Source File
# Begin Source File

SOURCE=.\ItemYear.h
# End Source File
# Begin Source File

SOURCE=.\Litestep.h
# End Source File
# Begin Source File

SOURCE=.\NetworkThread.h
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

SOURCE=.\SkinDialog.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RainlendarDLL.rc
# End Source File
# End Group
# Begin Source File

SOURCE=..\Distribution.txt
# End Source File
# Begin Source File

SOURCE=..\Manual.html
# End Source File
# Begin Source File

SOURCE=..\ToDo.txt
# End Source File
# End Target
# End Project

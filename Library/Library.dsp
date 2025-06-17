# Microsoft Developer Studio Project File - Name="Library" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Library - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Library.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Library.mak" CFG="Library - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Library - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Library - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Library"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Library - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "x32\Release"
# PROP Intermediate_Dir "x32\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBRARY_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\3rdParty\lpng" /I "..\3rdParty\zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40b /d "NDEBUG"
# ADD RSC /l 0x40b /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 Msimg32.lib Oleacc.lib Winmm.lib Wininet.lib comctl32.lib shlwapi.lib libpng.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../TestBench/x32/Release/Rainlendar.dll" /libpath:"..\3rdParty\lpng\projects\visualc6\Win32_LIB_Release"

!ELSEIF  "$(CFG)" == "Library - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "x32\Debug"
# PROP Intermediate_Dir "x32\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBRARY_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\3rdParty\lpng" /I "..\3rdParty\zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40b /d "_DEBUG"
# ADD RSC /l 0x40b /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Msimg32.lib Oleacc.lib Winmm.lib Wininet.lib comctl32.lib shlwapi.lib libpngd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../TestBench/x32/Debug/Rainlendar.dll" /pdbtype:sept /libpath:"..\3rdParty\lpng\projects\visualc6\Win32_LIB_Debug"

!ENDIF 

# Begin Target

# Name "Library - Win32 Release"
# Name "Library - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AllEventsDialog.cpp
# End Source File
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

SOURCE=.\EntryEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\EntryItem.cpp
# End Source File
# Begin Source File

SOURCE=.\EntryTodo.cpp
# End Source File
# Begin Source File

SOURCE=.\Error.cpp
# End Source File
# Begin Source File

SOURCE=.\EventListWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\EventManager.cpp
# End Source File
# Begin Source File

SOURCE=.\FileTime.cpp
# End Source File
# Begin Source File

SOURCE=.\Image.cpp
# End Source File
# Begin Source File

SOURCE=.\Item.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemButton.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemDays.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemImage.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemMonth.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemTime.cpp
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

SOURCE=.\Language.cpp
# End Source File
# Begin Source File

SOURCE=.\Litestep.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Plugin.cpp
# End Source File
# Begin Source File

SOURCE=.\PluginManager.cpp
# End Source File
# Begin Source File

SOURCE=.\RainlendarAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\RainlendarDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\RainlendarDLL.rc
# End Source File
# Begin Source File

SOURCE=.\RainWindow.cpp
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
# Begin Source File

SOURCE=.\TodoDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TodoWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Tooltip.cpp
# End Source File
# Begin Source File

SOURCE=.\UpdateCheck.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AllEventsDialog.h
# End Source File
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

SOURCE=.\Date.h
# End Source File
# Begin Source File

SOURCE=.\EditEvent.h
# End Source File
# Begin Source File

SOURCE=.\EntryEvent.h
# End Source File
# Begin Source File

SOURCE=.\EntryItem.h
# End Source File
# Begin Source File

SOURCE=.\EntryTodo.h
# End Source File
# Begin Source File

SOURCE=.\Error.h
# End Source File
# Begin Source File

SOURCE=.\EventListWindow.h
# End Source File
# Begin Source File

SOURCE=.\EventManager.h
# End Source File
# Begin Source File

SOURCE=.\FileTime.h
# End Source File
# Begin Source File

SOURCE=.\Image.h
# End Source File
# Begin Source File

SOURCE=.\Item.h
# End Source File
# Begin Source File

SOURCE=.\ItemButton.h
# End Source File
# Begin Source File

SOURCE=.\ItemDays.h
# End Source File
# Begin Source File

SOURCE=.\ItemEvent.h
# End Source File
# Begin Source File

SOURCE=.\ItemImage.h
# End Source File
# Begin Source File

SOURCE=.\ItemMonth.h
# End Source File
# Begin Source File

SOURCE=.\ItemTime.h
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

SOURCE=.\Language.h
# End Source File
# Begin Source File

SOURCE=.\Litestep.h
# End Source File
# Begin Source File

SOURCE=.\MessageWindow.h
# End Source File
# Begin Source File

SOURCE=.\Plugin.h
# End Source File
# Begin Source File

SOURCE=.\PluginManager.h
# End Source File
# Begin Source File

SOURCE=.\RainlendarAPI.h
# End Source File
# Begin Source File

SOURCE=.\RainlendarDLL.h
# End Source File
# Begin Source File

SOURCE=.\RainWindow.h
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

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SkinDialog.h
# End Source File
# Begin Source File

SOURCE=.\TodoDialog.h
# End Source File
# Begin Source File

SOURCE=.\TodoWindow.h
# End Source File
# Begin Source File

SOURCE=.\Tooltip.h
# End Source File
# Begin Source File

SOURCE=.\UpdateCheck.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\default1.bin
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# End Group
# End Target
# End Project

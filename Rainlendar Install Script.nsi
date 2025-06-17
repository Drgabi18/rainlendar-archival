###############################################################################
#
#                 Rainlendar installer script for NSIS
#                           version 0.5
#
###############################################################################

#
# Variables
#
Name "Rainlendar 0.17"
OutFile "RainlendarInstaller.exe"
InstallDir "$PROGRAMFILES\Rainlendar"
InstallDirRegKey HKLM SOFTWARE\Rainlendar "Install_Dir"

DirShow show ; (make this hide to not let the user change it)
DirText "Select the directory to install Rainlendar in to:"

#
# Components
#
ComponentText "Select the installing options"

#
# Install
#
Section "Rainlendar main program" ; (default section)
  SetOutPath "$INSTDIR"
  SectionIn 1 RO

  IfFileExists $INSTDIR\Rainlendar.ini FileExistsLabel
  Goto OverWriteAllLabel
  
FileExistsLabel:
  MessageBox MB_YESNO|MB_ICONQUESTION "Do you want to keep your old settings?" IDNO OverWriteAllLabel
  
  Rename $INSTDIR\Rainlendar.ini $INSTDIR\Rainlendar.old
  File /r "Distrib\*.*"
  Delete $INSTDIR\Rainlendar.ini
  Rename $INSTDIR\Rainlendar.old $INSTDIR\Rainlendar.ini
  Goto DoneLabel

OverWriteAllLabel:
  File /r "Distrib\*.*"
  
DoneLabel:

  WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Rainlendar" "" "$INSTDIR"
  WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Rainlendar" "DisplayName" "Rainlendar (remove only)"
  WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Rainlendar" "UninstallString" '"$INSTDIR\uninst.exe"'
  
  WriteUninstaller "$INSTDIR\uninst.exe"

  # Shortcut on Start Menu (Directory and Uninstall)
  CreateDirectory "$SMPROGRAMS\Rainlendar"
  CreateShortCut  "$SMPROGRAMS\Rainlendar\Rainlendar.lnk" "$INSTDIR\rainlendar.exe" "" "$INSTDIR\rainlendar.exe" 0
  CreateShortCut  "$SMPROGRAMS\Rainlendar\Manual.lnk" "$INSTDIR\Manual.html" "" "$INSTDIR\Manual.html" 0
  CreateShortCut  "$SMPROGRAMS\Rainlendar\Remove Rainlendar.lnk" "$INSTDIR\uninst.exe" "" "$INSTDIR\uninst.exe" 0

SectionEnd ; end of default section

Section "Add shortcut to desktop"
  SectionIn 1
  CreateShortCut  "$DESKTOP\Rainlendar.lnk" "$INSTDIR\rainlendar.exe" "" "$INSTDIR\rainlendar.exe" 0
SectionEnd

Section "Add shortcut to startup folder"
  SectionIn 1
  CreateShortCut  "$SMSTARTUP\Rainlendar.lnk" "$INSTDIR\rainlendar.exe" "" "$INSTDIR\rainlendar.exe" 0
SectionEnd

#
# Uninstall
#
UninstallText "This will uninstall Rainlendar from your system.$\nMake sure that Rainlendar is not running before proceeding.$\nNOTE: This will also delete all your Rainlendar events and languages!"

Section Uninstall

  FindWindow $0 "Rainlendar"
  IsWindow $0 0 RainlendarDoesntExistLabel
  MessageBox MB_OK "Rainlendar is running. Please close it before trying to uninstall the application."
  Quit

RainlendarDoesntExistLabel:
  Delete "$INSTDIR\Skins\Candy\*.*"
  Delete "$INSTDIR\Skins\Default\*.*"
  Delete "$INSTDIR\Skins\Shadow3\Icons\*.*"
  Delete "$INSTDIR\Skins\Shadow3\*.*"
  Delete "$INSTDIR\Languages\*.*"
  Delete "$INSTDIR\*.*"
  RMDir "$INSTDIR\Skins\Candy"
  RMDir "$INSTDIR\Skins\Default"
  RMDir "$INSTDIR\Skins\Shadow3\Icons"
  RMDir "$INSTDIR\Skins\Shadow3"
  RMDir "$INSTDIR\Languages"
  RMDir "$INSTDIR\Skins"
  DeleteRegKey HKLM SOFTWARE\Rainlendar
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Rainlendar"
  RMDir "$INSTDIR"
  Delete "$SMSTARTUP\Rainlendar.lnk"
  Delete "$DESKTOP\Rainlendar.lnk"
  Delete "$SMPROGRAMS\Rainlendar\*.*"
  RMDir "$SMPROGRAMS\Rainlendar"

  IfFileExists "$INSTDIR" 0 NoErrorMsg
    MessageBox MB_OK "Note: $INSTDIR could not be removed!" IDOK 0 ; skipped if file doesn't exist
  NoErrorMsg:
  
SectionEnd

; eof

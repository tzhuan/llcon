; llcon NSIS installer script

!define APP_NAME          "Llcon"
!define APP_EXE			  "llcon.exe"
!define AUTORUN_NAME      "llcon server"
!define UNINSTALL_EXE     "Uninstall.exe"
!define INSTALLER_NAME    "llconinstaller.exe"
!define VS_REDIST_PATH    "C:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\BootStrapper\Packages\vcredist_x86\"
;!define VS_REDIST_PATH   "C:\Programme\Microsoft Visual Studio 8\SDK\v2.0\BootStrapper\Packages\vcredist_x86\"
!define VS_REDIST_EXE     "vcredist_x86.exe"
!define UNINST_KEY        "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
!define AUTORUN_KEY       "Software\Microsoft\Windows\CurrentVersion\Run"


SetCompressor lzma
Name          "${APP_NAME}"
Caption       "${APP_NAME}"
OutFile       "${INSTALLER_NAME}"
InstallDir    "$PROGRAMFILES\${APP_NAME}"

LicenseText   "License"
LicenseData   "..\COPYING"

Page license
Page directory
Page instfiles


Section

  ; add reg keys so that software appears in Windows "Add/Remove Software"
  WriteRegStr HKLM "${UNINST_KEY}" "DisplayName" "${APP_NAME} (remove only)"
  WriteRegStr HKLM "${UNINST_KEY}" "UninstallString" '"$INSTDIR\${UNINSTALL_EXE}"'

  SetOutPath       $INSTDIR

  ; main application
  File             "Release\${APP_EXE}"

  ; QT dlls
  File             "$%QTDIR%\bin\QtCore4.dll"
  File             "$%QTDIR%\bin\QtGui4.dll"
  File             "$%QTDIR%\bin\QtNetwork4.dll"
  File             "$%QTDIR%\bin\QtXml4.dll"

  ; other files
  File             "..\COPYING"

  ; temporarily create Microsoft Visual Studio redistributable,
  File             "${VS_REDIST_PATH}${VS_REDIST_EXE}"
  ExecWait         '"$INSTDIR\${VS_REDIST_EXE}" /Q'

  ; uninstaller
  WriteUninstaller $INSTDIR\${UNINSTALL_EXE}

  ; shortcuts
  CreateShortCut   "$DESKTOP\${APP_NAME}.lnk" "$OUTDIR\${APP_EXE}"

  CreateDirectory  "$SMPROGRAMS\${APP_NAME}"
  CreateShortCut   "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}"
  CreateShortCut   "$SMPROGRAMS\${APP_NAME}\${APP_NAME} server.lnk" "$INSTDIR\${APP_EXE}" "-s"
  CreateShortCut   "$SMPROGRAMS\${APP_NAME}\${UNINSTALL_EXE}.lnk" "$INSTDIR\${UNINSTALL_EXE}"

  ; cleanup: remove temporary Microsoft Visual Studio redistributable executable
  Delete           $INSTDIR\${VS_REDIST_EXE}

  ; accessible qt plugin
  SetOutPath       $INSTDIR\accessible
  File             "$%QTDIR%\plugins\accessible\qtaccessiblewidgets4.dll"

SectionEnd


Section "Uninstall"

DeleteRegKey HKLM "${UNINST_KEY}"

; the software may have written an auto run entry in the registry, remove it
DeleteRegValue HKCU "${AUTORUN_KEY}" "${AUTORUN_NAME}"

Delete "$DESKTOP\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME} server.lnk"
Delete "$SMPROGRAMS\${APP_NAME}\${UNINSTALL_EXE}.lnk"
RMDIR  "$SMPROGRAMS\${APP_NAME}"

Delete $INSTDIR\${UNINSTALL_EXE}
Delete $INSTDIR\${APP_EXE}
Delete $INSTDIR\QtCore4.dll
Delete $INSTDIR\QtGui4.dll
Delete $INSTDIR\QtNetwork4.dll
Delete $INSTDIR\QtXml4.dll
Delete $INSTDIR\COPYING
Delete $INSTDIR\accessible\qtaccessiblewidgets4.dll
RMDir  $INSTDIR\accessible
RMDir  $INSTDIR

SectionEnd

; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "MADARA"

!define PRODUCT_PUBLISHER "James Edmondson"
!define PRODUCT_WEB_SITE "http://madara.googlecode.com"
;!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\kats_batch.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define env_hklm 'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
!define env_hkcu 'HKCU "Environment"'

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include "winmessages.nsh"
!include "WordFunc.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "..\..\LICENSE.txt"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.txt"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro WordAdd
!insertmacro un.WordAdd
; MUI end ------




; Get the MADARA version
!system "get_version.exe"
!include "VERSION.txt"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "madara_python_${PRODUCT_VERSION}.exe"
InstallDir "C:\MADARA"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails hide
ShowUnInstDetails hide

Section "debug-libs" SEC01
  SetOutPath "$INSTDIR\lib"
  SetOverwrite ifnewer
;  File "..\..\lib\KATSd.dll"
  File "..\..\lib\MADARAd.dll"
;  File "..\..\lib\KATSd.lib"
  File "..\..\lib\MADARAd.lib"
SectionEnd

Section "-release-libs" SEC02
;  File "..\..\lib\KATS.dll"
  File "..\..\lib\MADARA.dll"
  File "..\..\lib\madara.pyd"
  File "$%BOOST_ROOT%\stage\lib\libboost_python3-vc100-mt-$%BOOST_VERSION%.lib"
  File "$%BOOST_ROOT%\stage\lib\boost_python-vc100-mt-$%BOOST_VERSION%.lib"
  File "..\..\lib\MADARA.lib"
  File "..\..\lib\maal.py"
  File "..\..\lib\maml.py"
SectionEnd



;Section "docs" SEC03
;  SetOutPath "$INSTDIR\docs"
;  File "..\..\docs\Doxyfile_KaRL.dxy"
;  File "..\..\docs\Doxyfile_KATS"
;  File "..\..\docs\Doxyfile_MAML.dxy"
;  File "..\..\docs\Doxyfile_MAAL.dxy"
;  File "..\..\docs\Doxyfile_CID.dxy"
;  File "..\..\docs\README.txt"

;  SetOutPath "$INSTDIR\docs\cid"
;  File /r ..\..\docs\cid\html\*.*

;  SetOutPath "$INSTDIR\docs\kats"
;  File /r ..\..\docs\kats\html\*.*

;  SetOutPath "$INSTDIR\docs\karl"
;  File /r ..\..\docs\karl\html\*.*
  
;  SetOutPath "$INSTDIR\docs\maml"
;  File /r ..\..\docs\maml\html\*.*

;  SetOutPath "$INSTDIR\docs\maal"
;  File /r ..\..\docs\maal\html\*.*

;  CreateDirectory "$SMPROGRAMS\MADARA"
;  CreateShortCut "$SMPROGRAMS\MADARA\README.lnk" "$INSTDIR\docs\README.txt"
;SectionEnd

Section "tests" SEC04
  SetOutPath "$INSTDIR\bin"
  File "..\..\bin\profile_architecture.exe"
  File /r "..\..\bin\test*.exe"
  File /r "..\..\bin\tutorial*.exe"
  File "..\..\bin\system_calls.exe"
SectionEnd

;Section "-exes" SEC09
;  SetOutPath "$INSTDIR\bin"
;  File "..\..\bin\kats_batch.exe"
;  File "..\..\bin\kats_observer.exe"
;  File "..\..\bin\kats_process.exe"
;  File "..\..\bin\kats_sleep.exe"
;SectionEnd

Section "gme" SEC05
  SetOutPath "$INSTDIR\gme"
  File "..\..\modeling\KATS.mta"
  File "..\..\modeling\KATS.xmp"
  File "..\..\modeling\interpreters\GenerateXML\Release\GenerateXML.dll"
  SetOutPath "$INSTDIR\gme\Icons"
  File "..\..\modeling\Icons\barrier.gif"
  File "..\..\modeling\Icons\Host.gif"
  File "..\..\modeling\Icons\kill.gif"
  File "..\..\modeling\Icons\binoculars.jpg"
  File "..\..\modeling\Icons\blue_play.jpg"
  File "..\..\modeling\Icons\blue_round_play_button_4180.jpg"
  File "..\..\modeling\Icons\buttongroup.jpg"
  File "..\..\modeling\Icons\domain.jpg"
  File "..\..\modeling\Icons\host.jpg"
  File "..\..\modeling\Icons\sleep.jpg"
  File "..\..\modeling\Icons\XMLGenerator.png"
  File "..\..\modeling\Icons\sources.txt"
SectionEnd

Section "configs" SEC06
  SetOutPath "$INSTDIR\configs\cid\deployments\test_cid"
  File "..\..\configs\cid\deployments\test_cid\1_even_fans_disjoint.template"
  File "..\..\configs\cid\deployments\test_cid\2_even_fans_disjoint.template"
  File "..\..\configs\cid\deployments\test_cid\3_deep_tree.template"
  File "..\..\configs\cid\deployments\test_cid\3_even_fans_disjoint.template"
  File "..\..\configs\cid\deployments\test_cid\3waytree.template"
  File "..\..\configs\cid\deployments\test_cid\3waytree_strict.template"
  File "..\..\configs\cid\deployments\test_cid\4_even_fans_disjoint.template"
  File "..\..\configs\cid\deployments\test_cid\mwaytree.template"
  File "..\..\configs\cid\deployments\test_cid\specialized.template"
  SetOutPath "$INSTDIR\configs\kats"
;  File "..\..\configs\kats\FourSleeps.xml"
;  File "..\..\configs\kats\Generics.xml"
;  File "..\..\configs\kats\ospl_start.xml"
;  File "..\..\configs\kats\Timeout.xml"
;  File "..\..\configs\kats\TwoSleeps.xml"
;  File /r "..\..\configs\kats\transports"
SectionEnd

Section "ace" SEC10

  SetOutPath "$INSTDIR"
  
  File /oname=$INSTDIR\ACE-LICENSE.txt "$%ACE_ROOT%\COPYING"
  CreateShortCut "$SMPROGRAMS\MADARA\ACE-LICENSE.lnk" "$INSTDIR\ACE-LICENSE.txt"

  SetOutPath "$INSTDIR\lib"
  File "$%ACE_ROOT%\lib\ACE.dll"
  File "$%ACE_ROOT%\lib\ACEd.dll"
  File "$%ACE_ROOT%\lib\ACE.lib"
  File "$%ACE_ROOT%\lib\ACEd.lib"
  
  SetOutPath "$INSTDIR\include"

  File /r $%ACE_ROOT%\ace

SectionEnd

Section "-include" SEC07
  SetOutPath "$INSTDIR\include\madara"
  File "..\..\include\madara\MADARA_export.h"
  
  ; copy the madara directories
  File /r "..\..\include\madara\utility"
  File /r "..\..\include\madara\transport"
  File /r "..\..\include\madara\knowledge_engine"
  File /r "..\..\include\madara\kats"
  File /r "..\..\include\madara\expression_tree"
  File /r "..\..\include\madara\cid"
  File /r "..\..\include\madara\maml"
  File /r "..\..\include\madara\maal"
  File /r "..\..\include\madara\python"
  File /r "..\..\port"
  
SectionEnd

Section "-vcredist" SEC11

  SetOutPath "$INSTDIR\vcredist"
  File "redist\vcredist_x64.exe"
  File "redist\vcredist_x86.exe"

  # 64-bit Installs

  # Make sure the variable is clear
  StrCpy $1 ""
  SetRegview 64

  # Detect if it is installed already
  ReadRegStr $1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\{1D8E6291-B0D5-35EC-8441-6616F567A0F7}" DisplayName
  
  
  StrCmp $1 "" sp164_not_exists sp164_exists

  sp164_not_exists:
    # From http://blogs.msdn.com/astebner/archive/2007/02/07/update-regarding-silent-install-of-the-vc-8-0-runtime-vcredist-packages.aspx
    # �qb!� for progress with no cancel, �qb� for progress and cancel, �qn� for no interaction

    DetailPrint "Installing VC 10 64-bit Redistributable."  

    ExecWait '$INSTDIR\vcredist\vcredist_x64.exe /q' $0 # Only progress bar
    DetailPrint "vcredist_x64 SP1 Update returned $0"
    

  sp164_exists:
  # Nothing to do

  # 32-bit Installs
  # Make sure the variable is clear
  StrCpy $1 ""
  SetRegview 32

  # Detect if it is installed already
  ReadRegStr $1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\{F0C3E5D1-1ADE-321E-8167-68EF0DE699A5}" DisplayName
  StrCmp $1 "" sp1_not_exists sp1_exists

  sp1_not_exists:
    # From http://blogs.msdn.com/astebner/archive/2007/02/07/update-regarding-silent-install-of-the-vc-8-0-runtime-vcredist-packages.aspx
    # �qb!� for progress with no cancel, �qb� for progress and cancel, �qn� for no interaction
    DetailPrint "Installing VC 10 32-bit Redistributable."  
    ExecWait '$INSTDIR\vcredist\vcredist_x86.exe /q' $0 # Only progress bar
    DetailPrint "vcredist_x86 SP1 Update returned $0"

  sp1_exists:
  # Nothing to do

SectionEnd

Section "-tests"
  SetOutPath "$INSTDIR"
  File /r "..\..\tests"
SectionEnd

Section "-tutorials"
  SetOutPath "$INSTDIR"
  File /r "..\..\tutorials"
SectionEnd

Section "-requiredtools"
  SetOutPath "$INSTDIR\bin"
  File /r "..\..\bin\madara_version.exe"
SectionEnd

Section "-basic" SEC08
  SetOutPath "$INSTDIR"
  File "..\..\VERSION.txt"
  CreateShortCut "$SMPROGRAMS\MADARA\VERSION.lnk" "$INSTDIR\VERSION.txt"
  File "..\..\LICENSE.txt"
  CreateShortCut "$SMPROGRAMS\MADARA\LICENSE.lnk" "$INSTDIR\LICENSE.txt"
  File "..\..\README.txt"
  CreateShortCut "$SMPROGRAMS\MADARA\README.lnk" "$INSTDIR\README.txt"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\MADARA\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\MADARA\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Pre
  
  ; Ensure that only one installer is running for MADARA
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t "MadaraIntallMutex") i .r1 ?e'
  Pop $R0
 
  StrCmp $R0 0 +3
  MessageBox MB_OK|MB_ICONEXCLAMATION "The MADARA installer is already running."
  Abort

SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\bin\kats_batch.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\bin\kats_batch.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"

  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "OldMadaraRoot" "$%MADARA_ROOT%"

; set environment variables
   ; include for some of the windows messages defines
   ; HKLM (all users) vs HKCU (current user) defines
   ; set variable
   WriteRegExpandStr ${env_hklm} MADARA_ROOT $INSTDIR
   
   ; read the path variable
   ReadRegStr $1 ${env_hklm} PYTHONPATH

   ; remove references to MADARA in the PATH variable
   ${WordAdd} $1 ";" "+%MADARA_ROOT%\lib" $2
   
   ; now add them back
   WriteRegExpandStr ${env_hklm} PYTHONPATH $2

   ; read the path variable
   ReadRegStr $1 ${env_hklm} PATH

   ; remove references to MADARA in the PATH variable
   ${WordAdd} $1 ";" "+%MADARA_ROOT%\lib;%MADARA_ROOT%\bin" $2
   
   ; now add them back
   WriteRegExpandStr ${env_hklm} PATH $2

   ; make sure windows knows about the change
   SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000

SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "MADARA libraries compiled in debug mode (optional)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} "Should always be included"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC03} "Help files and API documentation"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC04} "Tests for KaRL, KATS, etc."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC06} "Example configuration files"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC07} "Include files for MADARA projects"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC08} "Version, License, etc."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC10} "Install ACE v6.0.1 (required if not already installed)."
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "MADARA was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove MADARA and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\LICENSE.txt"
  Delete "$INSTDIR\VERSION.txt"

  Delete "$SMPROGRAMS\MADARA\Uninstall.lnk"
  Delete "$SMPROGRAMS\MADARA\Website.lnk"
  Delete "$SMPROGRAMS\MADARA\LICENSE.lnk"
  Delete "$SMPROGRAMS\MADARA\VERSION.lnk"
  Delete "$SMPROGRAMS\MADARA\README.lnk"

  RMDir "$SMPROGRAMS\MADARA"
  RMDir /r "$INSTDIR\lib"
  RMDir /r "$INSTDIR\include"
  RMDir /r "$INSTDIR\port"
  RMDir /r "$INSTDIR\gme"
  RMDir /r "$INSTDIR\docs"
  RMDir /r "$INSTDIR\configs"
  RMDir /r "$INSTDIR\modeling"
  RMDir /r "$INSTDIR\tests"
  RMDir /r "$INSTDIR\bin"
  RMDir /r "$INSTDIR"

  ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "OldMadaraRoot"

  ;ReadRegStr $0 HKLM "SOFTWARE\Microsoft" "Bob"

  StrCmp $0 '' 0 revert
   ReadRegStr $1 ${env_hklm} PATH
   
   ; remove references to MADARA in the PATH variable
   ${WordAdd} $1 ";" "-%MADARA_ROOT%\lib;%MADARA_ROOT%\bin" $2

   WriteRegExpandStr ${env_hklm} PATH $2
   
   ReadRegStr $1 ${env_hklm} PYTHONPATH
   
   ; remove references to MADARA in the PATH variable
   ${WordAdd} $1 ";" "-%MADARA_ROOT%\lib" $2

   WriteRegExpandStr ${env_hklm} PYTHONPATH $2
   
   ; delete variable
   DeleteRegValue ${env_hklm} MADARA_ROOT
   
   Goto done
   ; revert to the old MADARA variable
   revert:
   WriteRegExpandStr ${env_hklm} MADARA_ROOT $0
   ; make sure windows knows about the change

   SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
   done:
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
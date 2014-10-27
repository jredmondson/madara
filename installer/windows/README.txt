MADARA WINDOWS INSTALLER INFORMATION

0. INTRODUCTION

  CONTACT: James Edmondson <jedmondson@gmail.com>
  WEBSITE: http://madara.googlecode.com

  This file provides guidance on how to use the Nullsoft Installer scripts
  to create self-installing Windows executables for the current MADARA version.

1. OBTAINING PREREQUISITES

  Project: Nullsoft Scriptable Install System
  URL: http://nsis.sourceforge.net

    Click on Download and pick up the latest release (alpha is recommended).

2. GENERATING SELF-INSTALLER EXECUTABLES

  2.1 Get Version

    In order for the installer to automatically grab the version number from
    VERSION.txt in MADARA_ROOT, we first need to compile a separate executable
    for the main installation script to use. To do this, right click on
    get_version.nsi and select "Compile NSIS Script". This will create get_version.exe

  2.2 Create the MADARA installer

    There are several MADARA installer scripts available, but the one we're likely to
    use is the madara_base_nokats_nohelp.nsi one. To compile the installer, right click
    on this nsi file and select "Compile NSIS Script". The resulting output should be
    a file of the form "madara_{VERSION}.exe". This file can be used by any Windows user
    to install MADARA natively on their machine.

  2.3 64 bit installs

    Convention is to rename the generated file to "madara_x64_{VERSION}.exe"
-----------------------------------------------------------------------------
Office_ClamWin
ClamWin Antivirus plugin for Office Documents
Copyright Dark Sylinc (c) 2005-2006
[darksylinc@users.sourceforge.net]

This product is part of ClamWin, maintined by alch [alch at users dot sourceforge dot net]

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
-----------------------------------------------------------------------------

Office_ClamWin is a COM module used by MS Office 2000 (= v9.0) to scan
office documents (i.e. *.doc; *.xls) for viruses. This module receives
the document location before it is opened by the program and launches
ClamWin minimized. After 4 seconds it restores the window because it surely
found a virus, otherwise ClamWin closes automatically. The program that
called this module stays frozen until ClamWin is closed.
Office_ClamWin supports multi-language for it's messages and warnings.
It doesn't mean that ClamWin supports this feature or not.

NOTE: YOU NEED CLAMWIN 0.86.2 TO USE THE OLE SUPPORT

Compilation
===========
Well, you need MS VC++ 6.0 to compile (Just wait for vctoolkit2003 scripts to be ready...
hey! you can do it yourself!)

What's New!
===========

Jun 27 2006
===========
* Fixed crash when releasing the COM module

Sep 16 2005
===========
+ Verbose logging (helps finding bugs)
* Log is in "C:\OCW.log", cannot be changed by user yet

Not Dated
=========
+ Many bug fixes
+ OLE support (needs ClamWin 0.86.2)

Jun 12 2005
===========
* First Release!!!

Installation
============
Compile the dll.
The module supports automatic registering (installation)
Run the scripts to launch them (choose only one according your compilation):
* %Office_ClamWin_path%\Install\Debug\Reg_Debug.bat
* %Office_ClamWin_path%\Install\Debug\UnReg_Debug.bat
* %Office_ClamWin_path%\Install\Release\Reg_Release.bat
* %Office_ClamWin_path%\Install\Release\UnReg_Release.bat
The Dll will ask you some questions and then you are finished!!!

Depending on ClamWin's installation, Office_ClamWin (OCW) may use HKLM or HKCU as registry keys in:
[HKXX\Software\ClamWin\Office Plugin\]
[HKXX\Software\ClamWin\Office Plugin\Language]
[HKXX\Software\ClamWin\Office Plugin\Settings]

What DOES work and what DOESN'T
===============================
Platform SDK says that Internet Explorer 5.0 uses this module to scan automatic
downloaded file (such as ActiveX components) Untested.
So far I have tested MS Word XP, Excel XP, and Power Point XP

Known Issues
============
Previsous versions crashed OLEView when realeasing this module. It could also crash MS Office
in some cases. Fixed since June, 27th, 2006

Configuration
=============
"rundll32.exe Office_ClamWin.dll,Configure" will launch the configuration dialog
     Enable OLE Scan (may be slow)  (needs ClamWin 0.86.2)
     -----------------------------
      Scans OLE content resident in memory for viruses. Note that it first copies the data
      to a temp file. If this temp file is deleted, it doesn't mean that the virus was removed.
      You have to choose "Yes" when the _plugin_ asks you to delete the virus.
      Note: The plugin deletes all the given OLE files, you might loose data.

     ClamWin restore delay time
     --------------------------
      Specifies the time, in milliseconds, that the plugin will wait to restore the ClamWin Window


Troubleshooting
===============
Q: I can't compile!!!
A: It should if you are using MS VC++ 6.0. Try downloading the Platform SDK:
   http://www.microsoft.com/msdownload/platformsdk/sdkupdate/

Q: When I open a file, it says "ClamWin was not found"
A: You need ClamWin to make this plugin work.
   Make sure it's been correctly installed.
   www.clamwin.com

Q: How do I know if a virus was found?
A: The module waits 4 seconds. If after that the ClamWin Window still
   exists, it is restored because surely an error happened (or your PC is too slow)
   Note that the application that launched the module will be freezed until
   the ClamWin window is closed.

Q: Can I remove the module after changing it from it's registered place?
A: Yes. But don't expect the module to be loaded by Office or other apps.
   I strongly recommend to register the dll again.

Q: Is there any configuration dialog?
A: Yes: rundll32.exe Office_ClamWin.dll,Configure

Q: Hey, where is the multi-language support?
A: Check that language dll files are in the same folder where the plugin is.
   Also check that the file extensions are *.dll
   Launch the Language configuration dialog: rundll32.exe Office_ClamWin.dll,ChangeLanguage
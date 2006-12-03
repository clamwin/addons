-----------------------------------------------------------------------------
Clamssenger
ClamWin MSN Messenger Plug-in
Copyright Dark Sylinc (c) 2006
[darksylinc at users dot sourceforge dot net]

This product is part of the ClamWin project

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

I. INTRODUCTION
II. COMPILING THE SOURCE CODE
III. TROUBLESHOOTING
III LINKS

I. INTRODUCTION
===============
Clamssenger is a simple program that scans files received through MSN Messenger (once the download
is completed).
When finished downloading, MSN launches Clamssenger and then it launches ClamWin to scan the file.
That simple.
The only little problem is that currently we don't know how to activate the plug-in automatically.
You have to do it yourself.
1. Open MSN Messenger
2. Go to the Tools menu
3. Click in Options
4. Go to "File transfers"
5. Enable "Analyze files looking for viruses using:" or similar
6. Write the path to Clamssenger.exe and accept


II. COMPILING THE SOURCE CODE
=============================
I'm using Microsoft Visual C++ Toolkit 2003 and Plattform SDK 2003 to compile the source.
You need the first one to compile the code and the second to compile the resources. To the date,
you can modify the makefile to avoid compiling the resources, in case you can't download Plattform SDK
We only use resources to include the nice Icon and the "Version" tab in the propierties dialog
To compile Clamssenger just type:
   nmake -f Makefile.vc
in the command line.
If you have Microsoft Visual C++ 6.0 or +, you may prefer the project workspace "Clamssenger.dsw"
and compile the release configuration.
See the LINKS section to see where to download Microsoft Visual C++ Toolkit and Plattform SDK

III. TROUBLESHOOTING
====================
Q: MSN Messenger doesn't launch Clamssenger
A: Check that you have correctly configured MSN Messenger. Look INTRODUCTION for more details.
   Check the log. It is stored in C:\Clamssenger.log and send it to darksylinc at users dot sourceforge dot net

LINKS
=====
Microsoft Visual C++ Toolkit 2003: (note that I've heard of a new 2005 release)
----------------------------------
http://msdn.microsoft.com/visualc/vctoolkit2003/

Plattform SDK:
--------------
http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
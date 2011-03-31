//-----------------------------------------------------------------------------
// Name:        Office_ClamWin.cpp
// Product:     ClamWin Antivirus plugin for Office Documents
//
// Author:      Dark Sylinc [darksylinc at users dot sourceforge dot net]
//
//				ClamWin programmer: alch [alch at users dot sourceforge dot net]
//
// Created:     2006/30/07
// Copyright:   Copyright Dark Sylinc (c) 2006
// Licence:
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <windows.h>
#include <fstream.h>

ofstream of;

#define log(text) { of.open("C:\\Clamssenger.log", ios::app); of << text << "\n"; of.close(); }

#define CLAMSSENGER_VERSION		"1.0"

//int main( unsigned int numargs, char *args[] )
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	HKEY hKey;
	TCHAR *ClamAVPath;
	TCHAR *tmp;
	DWORD size, dwType;
	FILE *file; //Tmp, to check that clamscan.exe exists

	STARTUPINFO Start_info;
    PROCESS_INFORMATION Proc_info;

	log( "\nClamssenger v" << CLAMSSENGER_VERSION );
	log( "Build date: " << __DATE__ );

	if( *strCmdLine == '\0' )
	{
		log( "Error: Not enough arguments" );
		log( "Arguments: " << strCmdLine );
		return -1;
	}

	//LoadSettings();

	log( "This program is free software; you can redistribute it and/or modify\n"
	   "it under the terms of the GNU General Public License as published by\n"
	   "the Free Software Foundation; either version 2 of the License, or\n"
	   "(at your option) any later version.\n\n"
	   "This program is distributed in the hope that it will be useful,\n"
	   "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	   "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	   "GNU General Public License for more details.\n"
	   "You should have received a copy of the GNU General Public License\n"
	   "along with this program; if not, write to the Free Software\n"
	   "Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA." );
	log( "Scanning file: " << strCmdLine );

	ClamAVPath	= (TCHAR*)malloc( 512 );
	tmp			= (TCHAR*)malloc( 1152 );
	memset( ClamAVPath, 0, 512 );
	memset( tmp, 0, 1152 );

	memset( &Start_info, 0, sizeof(STARTUPINFO) );
    Start_info.cb = sizeof(STARTUPINFO);
    memset( &Proc_info, 0, sizeof(PROCESS_INFORMATION) );

	//Check ClamAV path in HKEY_CURRENT_USER
	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\ClamWin\0", 0, KEY_READ, &hKey ) )
	{
		//Now try the HKEY_LOCAL_MACHINE
		if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, "Software\\ClamWin\0", 0, KEY_READ, &hKey ) )
		{
			log( "Error: Couldn't Open registry key to find ClamWin's folder" );
			MessageBox( 0, "Error: ClaWin was not found. Please install it.\nWarning: Your file was not scanned. Therefore it may still contain viruses.", "ClamWin not installed!", MB_ICONERROR|MB_OK );
			free(ClamAVPath);
			free(tmp);
			return -1;
		}
	}

	//Retrieve the path
	size = 512;
	RegQueryValueEx(hKey, "Path", NULL, &dwType, (BYTE*)ClamAVPath, &size);

	RegCloseKey( hKey );

	//Check we've got the right values
	if( *ClamAVPath == '\0' )
	{
		log("Error: Path to ClamWin's folder is corrupt");
		MessageBox( 0, "Error: ClaWin was not found. Please install it.\nWarning: Your file was not scanned. Therefore it may still contain viruses.", "ClamWin not installed!", MB_ICONERROR|MB_OK );
		free(ClamAVPath);
		free(tmp);
		return -1;
	}
	strcat( ClamAVPath, "\\Clamwin.exe\0" );
	if( !(file = fopen( ClamAVPath, "rb" )) )
	{
		log("Error: Error checking" << ClamAVPath );
		MessageBox( 0, "Error: ClaWin was not found. Please install it.\nWarning: Your file was not scanned. Therefore it may still contain viruses.", "ClamWin not installed!", MB_ICONERROR|MB_OK );
		free(ClamAVPath);
		free(tmp);
		return -1;
	}
	fclose(file);

	sprintf( tmp, "\"%s\" --mode=scanner --close --path=\"%s\"\0", ClamAVPath, strCmdLine );
	//Run ClamWin, minimized
	if( CreateProcess( NULL, tmp, NULL, NULL, FALSE, 0, NULL, NULL, &Start_info, &Proc_info ) )
	{
		DWORD ExitCode;
		GUITHREADINFO gui;
		gui.cbSize = sizeof(GUITHREADINFO);

		log("Scan: Running ClamWin...");
		log("File: " << '\"' << strCmdLine << '\"' );
		log("Process ID: 0x" << hex << Proc_info.dwProcessId);
		log("Thread ID: 0x" << hex << Proc_info.dwThreadId);

		WaitForSingleObject( Proc_info.hProcess, INFINITE );

		GetExitCodeProcess( Proc_info.hProcess, &ExitCode );

		log( "Scan: exit code = 0x" << hex << ExitCode);

	    //Close process and thread handles.
		CloseHandle( Proc_info.hProcess );
		CloseHandle( Proc_info.hThread );
	}

	free(ClamAVPath);
	free(tmp);

	return 0;
}

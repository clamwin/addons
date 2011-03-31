//-----------------------------------------------------------------------------
// Name:        Languages.cpp
// Product:     ClamWin Antivirus plugin for Office Documents
//
// Author:      Dark Sylinc [darksylinc@users.sourceforge.net]
//
//				ClamWin programmer: alch [alch at users dot sourceforge dot net]
//
// Created:     2005/12/06
// Copyright:   Copyright Dark Sylinc (c) 2005
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

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "Office_ClamWin.h"
#include "resource.h"
#include "languages.h"

void LoadLanguage();
int LoadSymLGN( HMODULE module );
int EnumerateAllLNG_Modules( HWND hDlg );
int GetLNG_Path_by_Name( TCHAR *LNG_Name, TCHAR *OutPath );
int CheckLNGSettings( HWND hDlg, BYTE Mode );

LRESULT CALLBACK LangChooseProc( HWND hDlg, UINT message, WPARAM wParam,
                                 LPARAM lParam );

RUNT_INT GetLanguageSize;
RUNT_LNGNAME GetLanguageName;
RUNT_DWORD GetDllVersion;

//---------------------------------------------------------------------------
// Function: LoadLanguage()
// --------
//	Main function called by the dll to fill the buffer with
//	the entries in a specific language
//---------------------------------------------------------------------------
void LoadLanguage()
{
	HKEY hKey;
	DWORD dwType, dwSize;
	BOOL bAreOpt;

	HMODULE module;
	BOOL UseDefault;
	TCHAR *Path;

	Path = (TCHAR*)malloc(512);
	memset( Path, 0, 512 );

	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\ClamWin\\Office Plugin\\Language\0", 0, KEY_READ, &hKey ) )
	{
		if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, "Software\\ClamWin\\Office Plugin\\Language\0", 0, KEY_READ, &hKey ) )
			bAreOpt = 0;
		else
			bAreOpt = 1;
	}
	else
		bAreOpt = 1;

	if( bAreOpt )
	{
		//Get the options and the language file
		dwSize = 512;
		RegQueryValueEx(hKey, "Path", NULL, &dwType, (BYTE*)Path, &dwSize);
		dwSize = sizeof(BOOL);
		RegQueryValueEx(hKey, "UseDefault", NULL, &dwType, (BYTE*)&UseDefault, &dwSize);
		RegCloseKey( hKey );

		if( !UseDefault && *Path != '\0' )
		{
			module = LoadLibrary( Path );
			if( !module )
				goto use_default;
			if( LoadSymLGN( module ) )
			{
				FreeLibrary( module );
				goto use_default;
			}
			if( Language_strings )
				free( Language_strings );
			Language_strings = (sLanguage_strings*)malloc(sizeof(sLanguage_strings));
			memset( Language_strings, 0, sizeof(sLanguage_strings) );
			LoadString( module, IDS_VIRUSFOUND    ,   Language_strings->VIRUSFOUND,       196 );
			LoadString( module, IDS_ACCOUNTENABLEQ,   Language_strings->ACCOUNTENABLEQ,   196 );
			LoadString( module, IDS_CONFIRMINSTALL,   Language_strings->CONFIRMINSTALL,   196 );
			LoadString( module, IDS_CONFIRMUNINSTALL, Language_strings->CONFIRMUNINSTALL, 196 );
			LoadString( module, IDS_CLAMWNOTINSTALL,  Language_strings->CLAMWNOTINSTALL,  196 );
			LoadString( module, IDS_EN_OLEMAYBESLOW,  Language_strings->EN_OLEMAYBESLOW,  196 );
			LoadString( module, IDS_CONFIRMCRASH,     Language_strings->CONFIRMCRASH,     196 );
			LoadString( module, IDS_EN_OLEMAYBESLOW,  Language_strings->EN_OLEMAYBESLOW,  196 );
			LoadString( module, IDS_OLEVIRUSFOUND,    Language_strings->OLEVIRUSFOUND,    196 );
			LoadString( module, IDS_LOGGING,		  Language_strings->LOGGING,		   64 );
			return;
		}
	}

use_default:
	if( Language_strings )
		free( Language_strings );
	Language_strings = (sLanguage_strings*)malloc(sizeof(sLanguage_strings));
	memset( Language_strings, 0, sizeof(sLanguage_strings) );
	LoadString( DllInstance, IDS_VIRUSFOUND    ,   Language_strings->VIRUSFOUND,       196 );
	LoadString( DllInstance, IDS_ACCOUNTENABLEQ,   Language_strings->ACCOUNTENABLEQ,   196 );
	LoadString( DllInstance, IDS_CONFIRMINSTALL,   Language_strings->CONFIRMINSTALL,   196 );
	LoadString( DllInstance, IDS_CONFIRMUNINSTALL, Language_strings->CONFIRMUNINSTALL, 196 );
	LoadString( DllInstance, IDS_CLAMWNOTINSTALL,  Language_strings->CLAMWNOTINSTALL,  196 );
	LoadString( DllInstance, IDS_CONFIRMCRASH,     Language_strings->CONFIRMCRASH,     196 );
	LoadString( DllInstance, IDS_EN_OLEMAYBESLOW,  Language_strings->EN_OLEMAYBESLOW,  196 );
	LoadString( DllInstance, IDS_OLEVIRUSFOUND,	   Language_strings->OLEVIRUSFOUND,    196 );
	LoadString( DllInstance, IDS_LOGGING,		    Language_strings->LOGGING,		    64 );
}

//---------------------------------------------------------------------------
// Function: LoadSymLGN()
// --------
//	Check given module for being a compatible Language file
//	Also loads symbols
//
//	Input:
//	-----
//	1) Pointer to the module
//
//	Returns:
//	-------
//	1) NULL if OK
//	2) -1 if Error
//---------------------------------------------------------------------------
int LoadSymLGN( HMODULE module )
{
	GetLanguageSize = (RUNT_INT)GetProcAddress( module, "GetLanguageSize" );
	GetLanguageName = (RUNT_LNGNAME)GetProcAddress( module, "GetLanguageName" );
	GetDllVersion = (RUNT_DWORD)GetProcAddress( module, "GetDllVersion" );
	if( !GetLanguageSize || !GetLanguageName || !GetDllVersion )
		return -1;
	//Check that this Module is a compatible version
	if( GetDllVersion() < CLAMWIN_LNG_VERSION )
		return -1;

	return 0;
}

//---------------------------------------------------------------------------
// Function: EnumerateAllLNG_Modules()
// --------
//	Enumerates all the language files in the current folder
//	and puts them into the dlg
//
//	Input:
//	-----
//	1) Dialog's HWND
//
//	Returns:
//	-------
//	1) NULL if OK
//	2) -1 if Error
//---------------------------------------------------------------------------
int EnumerateAllLNG_Modules( HWND hDlg )
{
	int ReturnValue = -1;
	TCHAR			drive[_MAX_DRIVE], dir[_MAX_DIR];
	TCHAR			searchpath[_MAX_PATH];
	HANDLE			findfirst;
	WIN32_FIND_DATA libaa;

	HWND hLngChsComboBox, hDefault;
	hLngChsComboBox = GetDlgItem( hDlg, IDC_LANGCHOOSE );
	hDefault		= GetDlgItem( hDlg, IDC_USEDEFAULT );
	//Reset the content in the combo box
	ComboBox_ResetContent( hLngChsComboBox );

	TCHAR *DllName = (TCHAR*) malloc( 512 );
	TCHAR *path    = (TCHAR*) malloc( 512 );
	GetModuleFileName( DllInstance, DllName, 512 );
	GetModuleFileName( DllInstance, path,    512 );

	//Prepare directory
	memset( drive, 0, _MAX_DRIVE );
	memset( dir, 0, _MAX_DIR );
	memset( searchpath, 0, _MAX_PATH );
	_splitpath(DllName, drive, dir, 0, 0);
	memset( DllName, 0, 512 );
	strcpy( searchpath, drive );
	strcat( searchpath, dir );
	strcpy( path, searchpath );
	strcat( searchpath, "*.dll" );

	findfirst = FindFirstFile(searchpath, &libaa);
	if( findfirst == INVALID_HANDLE_VALUE )
	{
		//No FILE in the directory (the dir is empty)
		return -1;
	}

	findfirst = FindFirstFile(searchpath, &libaa);
	do
	{
		strcpy( DllName, path );
		strcat( DllName, libaa.cFileName );
		HMODULE module = LoadLibrary( DllName );
		if( !module )
			continue; //Skip this file, it is not a valid lng module

		//Safely load the symbols
		if( LoadSymLGN( module ) )
			goto end;
		//Language's name size exceeds our buffer size. (we should create a
		if( GetLanguageSize() >= 512 ) //buffer according each name)
			goto end;
		GetLanguageName( DllName ); //Use DllName as tmp buffer

		ReturnValue = 0; //Return OK
		//Add dimensions to the combo box
		ComboBox_AddString( hLngChsComboBox, DllName );
end:
		FreeLibrary( module );
	} while(FindNextFile(findfirst, &libaa));

	//Set the combobox selection on the first option
	ComboBox_SetCurSel( hLngChsComboBox, 0 );

	free(DllName);
	free(path);

	if( !ReturnValue )
	{
		//Enable the combo box
		Button_SetCheck( hDefault, 0 );
		EnableWindow( hDefault, 1 );
		EnableWindow( hLngChsComboBox, 1 );
	}
	else
	{
		//Disable the combo box in case there's no language dll
		Button_SetCheck( hDefault, 1 );
		EnableWindow( hDefault, 0 );
		EnableWindow( hLngChsComboBox, 0 );
	}
	return ReturnValue;
}

//---------------------------------------------------------------------------
// Function: GetLNG_Path_by_Name()
// --------
//	Look's for a module with a specific language name (i.e. "Spanish")
//	and retrieves it's path
//
//	Input:
//	-----
//	1) Language's name
//
//	Output:
//	------
//	1) [in]
//	2) The path from the DLL
//
//	Returns:
//	-------
//	1) NULL if OK
//	2( 1 if not found
//---------------------------------------------------------------------------
int GetLNG_Path_by_Name( TCHAR *LNG_Name, TCHAR *OutPath )
{
	TCHAR			drive[_MAX_DRIVE], dir[_MAX_DIR];
	TCHAR			searchpath[_MAX_PATH];
	HANDLE			findfirst;
	WIN32_FIND_DATA libaa;

	int ReturnValue = 1;
	TCHAR *DllName = (TCHAR*) malloc( 512 );
	TCHAR *path    = (TCHAR*) malloc( 512 );
	TCHAR *tmp;
	GetModuleFileName( DllInstance, DllName, 512 );
	GetModuleFileName( DllInstance, path,    512 );

	//Prepare directory
	memset( drive, 0, _MAX_DRIVE );
	memset( dir, 0, _MAX_DIR );
	memset( searchpath, 0, _MAX_PATH );
	_splitpath(DllName, drive, dir, 0, 0);
	memset( DllName, 0, 512 );
	strcpy( searchpath, drive );
	strcat( searchpath, dir );
	strcpy( path, searchpath );
	strcat( searchpath, "*.dll" );

	findfirst = FindFirstFile(searchpath, &libaa);
	if( findfirst == INVALID_HANDLE_VALUE )
	{
		//No FILE in the directory (the dir is empty)
		return 1;
	}

	findfirst = FindFirstFile(searchpath, &libaa);
	do
	{
		strcpy( DllName, path );
		strcat( DllName, libaa.cFileName );
		HMODULE module = LoadLibrary( DllName );
		if( !module )
			continue; //Skip this file, it is not a valid lng module

		//Safely load the symbols
		if( LoadSymLGN( module ) )
			goto end;
		tmp = (TCHAR*)malloc( GetLanguageSize() );
		GetLanguageName( tmp );
		if( !strcmp( tmp, LNG_Name ) )
		{//We've found the DLL!
			ReturnValue = 0;
			strcpy( OutPath, DllName );
			free(tmp);
			FreeLibrary( module );
			break;
		}
		free(tmp);
end:
		FreeLibrary( module );
	} while(FindNextFile(findfirst, &libaa));

	free(DllName);
	free(path);
	return ReturnValue;
}

//---------------------------------------------------------------------------
// Function: CheckLNGSettings()
// --------
//	Check system registry and sets the options to the dlg
//	If there were no entries. The Dlg is set to default values.
//
//	Input:
//	-----
//	1) Dialog's HWND
//	2) Byte value:
//		0 = Read only. Sets values to the Dlg
//		1 = Write mode. Gets the values from Dlg and stores them in the reg.
//
//	Returns:
//	-------
//	1) NULL if OK
//	2) -1 if Error
//---------------------------------------------------------------------------
int CheckLNGSettings( HWND hDlg, BYTE Mode )
{
	HKEY hKey;
	DWORD dwSize, dwType;
	HWND hLngChsComboBox, hDefault;
	hLngChsComboBox = GetDlgItem( hDlg, IDC_LANGCHOOSE );
	hDefault		= GetDlgItem( hDlg, IDC_USEDEFAULT );

	HMODULE module;

	BOOL bAreOpt;
	BOOL UseDefault = 0;
	TCHAR *Path;
	BYTE *tmp;

	Path = (TCHAR*)malloc(512);
	memset( Path, 0, 512 );
	switch( Mode )
	{
	case 0: //Read only
		//Check that there are already options
		if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\ClamWin\\Office Plugin\\Language\0", 0, KEY_READ, &hKey ) )
		{
			if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, "Software\\ClamWin\\Office Plugin\\Language\0", 0, KEY_READ, &hKey ) )
				bAreOpt = 0;
			else
				bAreOpt = 1;
		}
		else
			bAreOpt = 1;

		if( bAreOpt )
		{
			//Get the options
			dwSize = 512;
			RegQueryValueEx(hKey, "Path", NULL, &dwType, (BYTE*)Path, &dwSize);
			dwSize = sizeof(BOOL);
			RegQueryValueEx(hKey, "UseDefault", NULL, &dwType, (BYTE*)&UseDefault, &dwSize);
			RegCloseKey( hKey );
		}

		//Set the combobox selection on the first option
		ComboBox_SetCurSel( hLngChsComboBox, 0 );
		if( *Path != '\0' )
		{//Now try to select the saved option
			module = LoadLibrary( Path );
			if( module )
			{
				if( !LoadSymLGN( module ) )
				{
					tmp = (BYTE*)malloc( GetLanguageSize() );
					GetLanguageName( (TCHAR*)tmp );
					int index = ComboBox_FindStringExact( hLngChsComboBox, 0, tmp );
					ComboBox_SetCurSel( hLngChsComboBox, index );
					free(tmp);
				}
				FreeLibrary( module );
			}
		}
		Button_SetCheck( hDefault, UseDefault );
		EnableWindow( hLngChsComboBox, !UseDefault );
	break;
	case 1: //Write mode
		if( ERROR_SUCCESS != RegCreateKeyEx( HKEY_LOCAL_MACHINE, "Software\\ClamWin\\Office Plugin\\Language\0",
											0, 0, REG_OPTION_NON_VOLATILE,
											KEY_ALL_ACCESS, NULL, &hKey, NULL ) )
			goto error;

		//Save the checkbox status
		UseDefault = Button_GetCheck( hDefault );
		RegSetValueEx( hKey, "UseDefault", 0, REG_DWORD, (UCHAR*)&UseDefault, sizeof(BOOL) );

		//Save the DLL path
		ComboBox_GetText( hLngChsComboBox, Path, 512 );
		GetLNG_Path_by_Name( Path, Path );
		RegSetValueEx( hKey, "Path", 0, REG_SZ, (UCHAR*)Path, strlen(Path)+1 );
		RegCloseKey( hKey );
	break;
	}

	free(Path);
	return 0;

error:
	free(Path);
	return -1;
}

//---------------------------------------------------------------------------
// Function: LangChooseProc()
// --------
//	Dialog proccedure for IDD_LANGUAGECHOOSE
//---------------------------------------------------------------------------
LRESULT CALLBACK LangChooseProc( HWND hDlg, UINT message, WPARAM wParam,
                                 LPARAM lParam )
{
	BOOL UseDefault;
	switch (message)
    {
        case WM_INITDIALOG:
			//Set the icon for this dialog
			PostMessage( hDlg, WM_SETICON, ICON_BIG,   (LPARAM) LoadIcon( DllInstance, MAKEINTRESOURCE(IDI_CLAMICON)) );  // Set big icon
		    PostMessage( hDlg, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon( DllInstance, MAKEINTRESOURCE(IDI_CLAMICON)) );  // Set small icon
			if( EnumerateAllLNG_Modules( hDlg ) )
				SendMessage( hDlg, WM_COMMAND, IDOK, 0 );
			CheckLNGSettings( hDlg, 0 );
			break;

        case WM_COMMAND:
			switch ( LOWORD (wParam))
			{
			case IDOK:
				CheckLNGSettings( hDlg, 1 );
				EndDialog( hDlg, TRUE );
				break;
			case IDC_USEDEFAULT:
				UseDefault = Button_GetCheck( GetDlgItem( hDlg, IDC_USEDEFAULT ) );
				EnableWindow( GetDlgItem( hDlg, IDC_LANGCHOOSE ), !UseDefault );
				break;
			}
            break;

        case WM_CLOSE:
            EndDialog( hDlg, TRUE );

            return TRUE;
            break;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Name:        Office_ClamWin.cpp
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
//
#include <windows.h>
#include <olectl.h>
#include <stdio.h>
#include <direct.h>
#include "Office_ClamWin.h"
#include "languages.h"
#include "resource.h"


ULONG Dll_Ref=0; //Reference count to this DLL
//ULONG p_Ref=0; //Reference count
//ULONG Of_Ref=0; //Reference count

sLanguage_strings *Language_strings = 0L; //Language
HINSTANCE DllInstance = 0;

ofstream of;

// {56FFCC30-D398-11d0-B2AE-00A0C908FA49}
const GUID FAR IID_IOfficeAntiVirus =
{ 0x56ffcc30, 0xd398, 0x11d0, { 0xb2, 0xae, 0x00, 0xa0, 0xc9, 0x08, 0xfa, 0x49 } };

// {DE1F7EEF-1851-11D3-939E-0004AC1ABE1F}
static const GUID CLSID_OfficeClamWin =
{ 0xde1f7eef, 0x1851, 0x11d3, { 0x93, 0x9e, 0x00, 0x04, 0xac, 0x1a, 0xbe, 0x1f } };

// {CLSID_ClamWin OfficeAntiVirus Class}
static const GUID CLSID_OfficeClamWin_Class =
{ 0x00b73327, 0x7348, 0x4ef0, { 0x9d, 0x81, 0x0c, 0x5c, 0xc6, 0xb6, 0x75, 0x8b } };

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	DllInstance = (HINSTANCE)hModule;

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			LoadSettings();
		case DLL_THREAD_ATTACH:
			LoadLanguage();
			break;
			/*if( !Language_strings )
			{
				Language_strings = (sLanguage_strings*)malloc(sizeof(sLanguage_strings));
				LoadString( DllInstance, IDS_VIRUSFOUND    ,   Language_strings->VIRUSFOUND,       196 );
				LoadString( DllInstance, IDS_ACCOUNTENABLEQ,   Language_strings->ACCOUNTENABLEQ,   196 );
				LoadString( DllInstance, IDS_CONFIRMINSTALL,   Language_strings->CONFIRMINSTALL,   196 );
				LoadString( DllInstance, IDS_CONFIRMUNINSTALL, Language_strings->CONFIRMUNINSTALL, 196 );
				LoadString( DllInstance, IDS_CLAMWNOTINSTALL,  Language_strings->CLAMWNOTINSTALL,  196 );
				LoadString( DllInstance, IDS_OLENOTSUPPORTED,  Language_strings->OLENOTSUPPORTED,  640 );
				LoadString( DllInstance, IDS_CONFIRMCRASH,     Language_strings->CONFIRMCRASH,     196 );
				break;
			}*/
		case DLL_THREAD_DETACH:
			//log( "*Called DLL_THREAD_DETACH (IOffAVFactory: Ref=" << dec << p_Ref << " IOfficeAntiVirus: Ref=" << dec << Of_Ref << ")*" );
			log( "*Called DLL_THREAD_DETACH (DLL: Ref = " << dec << Dll_Ref << ")*" );
			break;
		case DLL_PROCESS_DETACH:
			//log( "***Called DLL_PROCESS_DETACH (IOffAVFactory: Ref=" << dec << p_Ref << " IOfficeAntiVirus: Ref=" << dec << Of_Ref << ")***" );
			log( "***Called DLL_PROCESS_DETACH (DLL: Ref = " << dec << Dll_Ref << ")***" );
			log( "------------------------------------------------------------------------------\n" );
			if( Language_strings )
			{
				free(Language_strings);
				Language_strings = 0L;
			}
			break;
    }
	return TRUE;
}

//---------------------------------------------------------------------------
// IOffAVFactory::Constructor
//---------------------------------------------------------------------------
IOffAVFactory::IOffAVFactory()
{
	p_Ref = 0;
	Dll_Ref++;
}

//---------------------------------------------------------------------------
// IOffAVFactory::Destructor
//---------------------------------------------------------------------------
IOffAVFactory::~IOffAVFactory()
{
	Dll_Ref++;
}

//---------------------------------------------------------------------------
// IOffAVFactory::CreateInstance
//	Creates the IOfficeAntiVirus interface for the app
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE IOffAVFactory::CreateInstance(LPUNKNOWN pUnkOuter,
															REFIID riid,
															LPVOID *ppvObj)
{
	*ppvObj = NULL;

	log( "IOffAVFactory::CreateInstance() called");

	if( pUnkOuter )
	{
		log( "Warning: returning CLASS_E_NOAGGREGATION");
		return CLASS_E_NOAGGREGATION;
	}

	IOfficeAntiVirus *OffAV = new IOfficeAntiVirus(); //Create the IOfficeAntiVirus class

	if( !OffAV )
		return E_OUTOFMEMORY;

	return OffAV->QueryInterface(riid, ppvObj);
}

//---------------------------------------------------------------------------
// IOffAVFactory::QueryInterface
//---------------------------------------------------------------------------
STDMETHODIMP IOffAVFactory::QueryInterface(THIS_ REFIID riid, LPVOID FAR* ppvObj)
{
	*ppvObj = NULL;

	if( IsEqualIID(riid, IID_IUnknown) ||
		IsEqualIID(riid, IID_IClassFactory))
	{
		log( "IOffAVFactory::QueryInterface() called" );
		*ppvObj = (LPCLASSFACTORY)this;

		AddRef();

		return NOERROR;
	}

	return E_NOINTERFACE;
}

//---------------------------------------------------------------------------
// IOffAVFactory::AddRef
//---------------------------------------------------------------------------
STDMETHODIMP_ (ULONG) IOffAVFactory::AddRef()
{
	log( "IOffAVFactory::AddRef()" )
	return p_Ref++;
}

//---------------------------------------------------------------------------
// IOffAVFactory::Release
//---------------------------------------------------------------------------
STDMETHODIMP_ (ULONG) IOffAVFactory::Release()
{
	if( p_Ref ) //Shouldn't check... but could cause a crash anyway
		p_Ref--;
	log( "IOffAVFactory::Release() (Reference: " << p_Ref << ")" );
	if( !p_Ref )
		delete this;
	return p_Ref;
}

//---------------------------------------------------------------------------
// IOfficeAntiVirus::Constructor
//---------------------------------------------------------------------------
IOfficeAntiVirus::IOfficeAntiVirus()
{
	Of_Ref = 0;
	Dll_Ref++;
}

//---------------------------------------------------------------------------
// IOfficeAntiVirus::Destructor
//---------------------------------------------------------------------------
IOfficeAntiVirus::~IOfficeAntiVirus()
{
	Dll_Ref--;
}

//---------------------------------------------------------------------------
// IOfficeAntiVirus::QueryInterface
//---------------------------------------------------------------------------
STDMETHODIMP IOfficeAntiVirus::QueryInterface(THIS_ REFIID riid, LPVOID FAR* ppvObj)
{
	*ppvObj = NULL;

	if( IsEqualIID(riid, IID_IUnknown) ||
		IsEqualIID(riid, IID_IOfficeAntiVirus))
	{
		log( "IOfficeAntiVirus::QueryInterface() called" );
		*ppvObj = (IOfficeAntiVirus*)this;

		AddRef();

		return NOERROR;
	}

	return E_NOINTERFACE;
}

//---------------------------------------------------------------------------
// IOfficeAntiVirus::AddRef
//---------------------------------------------------------------------------
STDMETHODIMP_ (ULONG) IOfficeAntiVirus::AddRef()
{
	log("IOfficeAntiVirus::AddRef()");
	return Of_Ref++;
}

//---------------------------------------------------------------------------
// IOfficeAntiVirus::Release
//---------------------------------------------------------------------------
STDMETHODIMP_ (ULONG) IOfficeAntiVirus::Release()
{
	if( Of_Ref ) //Shouldn't check... but could cause a crash anyway
		Of_Ref--;
	log( "IOfficeAntiVirus::Release() (Reference: " << Of_Ref << ")" );

	if( !Of_Ref )
		delete this;
	return Of_Ref;
}

//---------------------------------------------------------------------------
// IOfficeAntiVirus::Scan
//	Scans given file
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE IOfficeAntiVirus::Scan( MSOAVINFO *pmsoavinfo )
{
	HKEY hKey;
	TCHAR *ClamAVPath;
	TCHAR *cmd;
	TCHAR *tmp;
	DWORD size, dwType;
	FILE *file; //Tmp, to check that clamscan.exe exists

	STARTUPINFO Start_info;
    PROCESS_INFORMATION Proc_info;

	LoadSettings();

	log( "IOfficeAntiVirus::Scan() called" );
	if( !pmsoavinfo )
	{
		log("Error: pmsoavinfo points to NULL");
		return E_FAIL;
	}
	if( pmsoavinfo->cbsize != sizeof(MSOAVINFO) )
	{
		log("Error: pmsoavinfo->cbsize is not" << sizeof(MSOAVINFO) << "Current value:" << pmsoavinfo->cbsize);
		return E_FAIL;
	}

	if( !Language_strings ) //Sometimes it is not loaded
		LoadLanguage();

	ClamAVPath	= (TCHAR*)malloc( 512 );
	cmd			= (TCHAR*)malloc( 512 );
	tmp			= (TCHAR*)malloc( 1152 );
	memset( ClamAVPath, 0, 512 );
	memset( cmd, 0, 512 );
	memset( tmp, 0, 1152 );

	memset( &Start_info, 0, sizeof(STARTUPINFO) );
    Start_info.cb = sizeof(STARTUPINFO);
	Start_info.dwFlags = STARTF_USESHOWWINDOW; //wShowWindow is valid (see below)
	Start_info.wShowWindow = SW_MINIMIZE; //Start ClamWin minimized
    memset( &Proc_info, 0, sizeof(PROCESS_INFORMATION) );

	//Check ClamAV path in HKEY_CURRENT_USER
	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\ClamWin\0", 0, KEY_READ, &hKey ) )
	{
		//Now try the HKEY_LOCAL_MACHINE
		if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, "Software\\ClamWin\0", 0, KEY_READ, &hKey ) )
		{
			log( "Error: Couldn't Open registry key to find ClamWin's folder" );
			MessageBox( pmsoavinfo->hwnd, Language_strings->CLAMWNOTINSTALL, "ClamWin not installed!", MB_ICONERROR|MB_OK );
			free(ClamAVPath);
			free(cmd);
			free(tmp);
			return E_FAIL;
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
		MessageBox( pmsoavinfo->hwnd, Language_strings->CLAMWNOTINSTALL, "ClamWin not installed!", MB_ICONERROR|MB_OK );
		free(ClamAVPath);
		free(cmd);
		free(tmp);
		return E_FAIL;
	}
	strcat( ClamAVPath, "\\Clamwin.exe\0" );
	if( !(file = fopen( ClamAVPath, "rb" )) )
	{
		log("Error: Error checking" << ClamAVPath );
		MessageBox( pmsoavinfo->hwnd, Language_strings->CLAMWNOTINSTALL, "ClamWin not installed!", MB_ICONERROR|MB_OK );
		free(ClamAVPath);
		free(cmd);
		free(tmp);
		return E_FAIL;
	}
	fclose(file);

	if( pmsoavinfo->fPath )
	{
		log("Scan: Single file mode")
		//Get Windows version for whether checking using unicode
		DWORD dwVersion = GetVersion();

		//Convert the given string from UNICODE to ANSI
		WideCharToMultiByte( CP_ACP, 0, pmsoavinfo->u.pwzFullPath, -1,
							cmd, 512, NULL, NULL );

		if (dwVersion < 0x80000000)		//Windows NT family, use unicode
		{
			log("Windows NT family detected, using UNICODE");
			//Check that the file exists (UNICODE CHECKING, SINCE ANSI CAN GIVE WRONG VALUES)
			if( !(file = _wfopen( pmsoavinfo->u.pwzFullPath, L"rb" )) )
			{
				free(ClamAVPath);
				free(cmd);
				free(tmp);
				return ERROR_FILE_NOT_FOUND;
			}
		}
		else  //Windows 9x family (Unlike NTFS, FAT32 only allows ASCII chars)
		{
			log("Windows 9x family detected, using ASCII");
			if( !(file = fopen( cmd, "rb" )) )
			{
				free(ClamAVPath);
				free(cmd);
				free(tmp);
				return ERROR_FILE_NOT_FOUND;
			}
		}
		fclose(file);
	}
	else
	{
		unsigned int u = 0,  //ClamWin$.ole where $ contains u.
					cmdlen;
		HRESULT retvalue = S_OK;
		DWORD ExitCode;

		//OLE Scanning was disabled
		if( !OCW_Settings.OLE_Scan )
			return retvalue;

		log("Scan: OLE mode")

		//Get Windows' temp folder
		if( !GetTempPath( 512, cmd ) )
			//strcpy( cmd, "C:\\ClamWin.ole\0" );
			strcpy( cmd, "C:\\OCW\\\0" );
		else
			strcat( cmd, "OCW\\\0" );
			//strcat( cmd, "ClamWin.ole\0" );

		log("Scan: OLEs tmp folder:" << cmd );
		mkdir( cmd );
		cmdlen = strlen(cmd);
		//Prepare the command line to launch ClamWin
		sprintf( tmp, "\"%s\" --mode=scanner --close \"--path=%s\0", ClamAVPath, cmd );

		IStorage *IStg = pmsoavinfo->u.lpstg;
		IEnumSTATSTG *Enum_StatSTG = 0L;

		STATSTG statstg;
		memset( &statstg, 0, sizeof(statstg) );

		IStg->EnumElements( 0, 0, 0, &Enum_StatSTG );
		HRESULT hr = Enum_StatSTG->Next( 1, &statstg, 0 );
		while( hr == S_OK )
		{
			if( statstg.type == STGTY_STREAM )
			{
				IStream *IStr = 0;
				LARGE_INTEGER large_i;
				ULARGE_INTEGER FileSize;
				BYTE t;
				large_i.QuadPart = 0;

				//Open the file
				if( !(hr = IStg->OpenStream( statstg.pwcsName, 0, STGM_SHARE_EXCLUSIVE, 0, &IStr )) )
				{
					//Get the file size
					IStr->Seek( large_i, STREAM_SEEK_END, &FileSize );
					IStr->Seek( large_i, STREAM_SEEK_SET, 0 );
					sprintf( cmd, "%s\\ClamWin%d.ole", cmd, u );
					u++;
					file = fopen( cmd, "wb" );
					cmd[cmdlen] = 0x00;
					//for( unsigned __int64 i=0; i<FileSize.QuadPart; i++ )
					while( FileSize.QuadPart-- )
					{
						IStr->Read( &t, 1, 0 );
						putc( t, file );
					}
					fclose(file);
					IStr->Release();
				}
			}
			if( statstg.pwcsName )
				CoTaskMemFree( statstg.pwcsName );
			hr = Enum_StatSTG->Next( 1, &statstg, 0 );
		}

		log("Scan: Number of OLE files:" << dec << u );

		//Run ClamWin, minimized
		if( CreateProcess( NULL, tmp, NULL, NULL, FALSE, 0, NULL, NULL, &Start_info, &Proc_info ) )
		{
			GUITHREADINFO gui;
			gui.cbSize = sizeof(GUITHREADINFO);

			log("Scan: Running ClamWin...");
			log("Process ID: 0x" << hex << Proc_info.dwProcessId);
			log("Thread ID: 0x" << hex << Proc_info.dwThreadId);

			WaitForSingleObject( Proc_info.hProcess, INFINITE );

			GetExitCodeProcess( Proc_info.hProcess, &ExitCode );

	    	//Close process and thread handles.
			CloseHandle( Proc_info.hProcess );
			CloseHandle( Proc_info.hThread );
		}
		if( ExitCode )
		{
			log( "Scan: exit code = 0x" << hex << ExitCode);
			log("Warning: Exit code not Zero, probably a virus was found!!!");
			if( IDOK == MessageBox( 0, Language_strings->OLEVIRUSFOUND, "Virus found!!!", MB_ICONEXCLAMATION|MB_OKCANCEL ) )
			{
				Enum_StatSTG->Reset();
				HRESULT hr = Enum_StatSTG->Next( 1, &statstg, 0 );
				while( hr == S_OK )
				{
					if( statstg.type == STGTY_STREAM )
					{
						IStg->DestroyElement( statstg.pwcsName );
						IStg->Commit(STGC_OVERWRITE);
						retvalue = S_FALSE;
					}
					if( statstg.pwcsName )
						CoTaskMemFree( statstg.pwcsName );
					hr = Enum_StatSTG->Next( 1, &statstg, 0 );
				}
			}
			else
				retvalue = E_FAIL;
		}

		while( u-- )
		{
			sprintf( cmd, "%s\\ClamWin%d.ole", cmd, u );
			hr = remove(cmd);
			cmd[cmdlen] = 0x00;
		}
		rmdir(cmd);

		if( Enum_StatSTG )
			Enum_StatSTG->Release();

		free(ClamAVPath);
		free(cmd);
		free(tmp);
		return retvalue;
	}

	sprintf( tmp, "\"%s\" --mode=scanner --close --path=\"%s\"\0", ClamAVPath, cmd );
	//Run ClamWin, minimized
	if( CreateProcess( NULL, tmp, NULL, NULL, FALSE, 0, NULL, NULL, &Start_info, &Proc_info ) )
	{
		DWORD ExitCode;
		GUITHREADINFO gui;
		gui.cbSize = sizeof(GUITHREADINFO);

		log("Scan: Running ClamWin...");
		log("File: " << '\"' << cmd << '\"' );
		log("Process ID: 0x" << hex << Proc_info.dwProcessId);
		log("Thread ID: 0x" << hex << Proc_info.dwThreadId);

		//Wait ClamWin to be finished. We don't want to detect a virus and open it anyway
		WaitForSingleObject( Proc_info.hProcess, OCW_Settings.DelayTime ); //First wait X seconds.
		if( GetGUIThreadInfo( Proc_info.dwThreadId, &gui ) )
			ShowWindow( gui.hwndActive, SW_RESTORE ); //Surely, an error happened, bring ClamWin
													  //window to the front
		WaitForSingleObject( Proc_info.hProcess, INFINITE );

		GetExitCodeProcess( Proc_info.hProcess, &ExitCode );

		log( "Scan: exit code = 0x" << hex << ExitCode);

	    //Close process and thread handles.
		CloseHandle( Proc_info.hProcess );
		CloseHandle( Proc_info.hThread );
	}

	free(ClamAVPath);
	free(cmd);
	free(tmp);

	return S_OK;
}


//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------
STDAPI DllCanUnloadNow( void )
{
	//mmmhh.. not sure if this is right.
	return (Dll_Ref == 0 ? S_OK : S_FALSE);
}

//---------------------------------------------------------------------------
// DllGetClassObject
//	Main entry point for COM interfaces
//---------------------------------------------------------------------------
STDAPI DllGetClassObject( REFCLSID rclsid, REFIID riid, LPVOID *ppvOut )
{
	*ppvOut = NULL;

	if (IsEqualIID(rclsid, CLSID_OfficeClamWin) ||
		IsEqualIID(rclsid, CLSID_OfficeClamWin_Class))
	{
		IOffAVFactory *OffAVFact = new IOffAVFactory();

		return OffAVFact->QueryInterface(riid, ppvOut);
	}

	return CLASS_E_CLASSNOTAVAILABLE;
}


//---------------------------------------------------------------------------
// DllRegisterServer
//	Called by regsvr32.exe to tell the DLL to register the class
//	(install the module)
//---------------------------------------------------------------------------
STDAPI DllRegisterServer(void)
{
	HKEY hKey;
	HKEY Root;
	BOOL bAll_Users;
	TCHAR SubKey[128];
	TCHAR *DllName;

	flog( "\n---------------------------------------------\n" );
	flog( "DllRegisterServer() started" );
	flog( "Office_ClamWin Copyright Dark Sylinc (c) 2005\nUnder GPL license" );
	flog( "Build date: " << __DATE__ );
	flog( "Language spec revision: " << dec << CLAMWIN_LNG_VERSION );
	flog( "\n---------------------------------------------\n" );

	//Display the language choose dialog
	DialogBox( DllInstance,
			MAKEINTRESOURCE(IDD_LANGUAGECHOOSE),
			NULL,
			(DLGPROC) LangChooseProc );
	//Reload language buffer
	LoadLanguage();

	//Ask the user whether he wants to install the Plugin
	if( MessageBox( NULL, Language_strings->CONFIRMINSTALL, "ClamWin MS Office Document virus scanner", MB_OKCANCEL ) == IDCANCEL )
		return SELFREG_E_CLASS;

	//Ask the user whether the plugin will be enabled for All accounts or just one
	if( MessageBox( NULL, Language_strings->ACCOUNTENABLEQ, "Account configuration", MB_ICONQUESTION|MB_YESNO ) == IDYES )
	{
		bAll_Users = 1;
		Root = HKEY_CLASSES_ROOT;
		strcpy( SubKey, "CLSID\\{00B73327-7348-4ef0-9D81-0C5CC6B6758B}\0" );
		flog( "Install: Plugin enabled for all accounts" );
	}
	else
	{
		bAll_Users = 0;
		Root = HKEY_CURRENT_USER;
		strcpy( SubKey, "Classes\\CLSID\\{00B73327-7348-4ef0-9D81-0C5CC6B6758B}\0" );
		flog( "Install: Plugin enabled for single account" );
	}

	//Create the Class key
	if( ERROR_SUCCESS != RegCreateKeyEx( Root, SubKey, 0, 0, REG_OPTION_NON_VOLATILE,
										KEY_ALL_ACCESS, NULL, &hKey, NULL ) )
			return SELFREG_E_CLASS;

	RegSetValueEx( hKey, "", 0, REG_SZ, (UCHAR*)"ClamWin OfficeAntiVirus Class\0", strlen("ClamWin OfficeAntiVirus Class\0")+1 );

	RegCloseKey( hKey );

	//Register the Class as supporting the IOfficeAntiVirus feature
	strcat( SubKey, "\\Implemented Categories\\{56FFCC30-D398-11D0-B2AE-00A0C908FA49}\0" );
	if( ERROR_SUCCESS != RegCreateKeyEx( Root, SubKey, 0, 0, REG_OPTION_NON_VOLATILE,
										KEY_ALL_ACCESS, NULL, &hKey, NULL ) )
			return SELFREG_E_CLASS;
	RegCloseKey( hKey );

	//Register the location of the DLL
	if( bAll_Users )
		strcpy( SubKey, "CLSID\\{00B73327-7348-4ef0-9D81-0C5CC6B6758B}\\InProcServer32\0" );
	else
		strcpy( SubKey, "Classes\\CLSID\\{00B73327-7348-4ef0-9D81-0C5CC6B6758B}\\InProcServer32\0" );

	if( ERROR_SUCCESS != RegCreateKeyEx( Root, SubKey, 0, 0, REG_OPTION_NON_VOLATILE,
										KEY_ALL_ACCESS, NULL, &hKey, NULL ) )
			return SELFREG_E_CLASS;

	DllName = (TCHAR*) malloc( 512 );
	*DllName = '\"'; //Place between brackets (Security issue)
	GetModuleFileName( DllInstance, DllName+1, 511 );
	strcat( DllName, "\"" );
	RegSetValueEx( hKey, "", 0, REG_SZ, (UCHAR*)DllName, strlen(DllName)+1 );
	RegSetValueEx( hKey, "ThreadingModel\0", 0, REG_SZ, (UCHAR*)"Apartment\0", strlen("Apartment\0")+1 );
	free(DllName);

	RegCloseKey( hKey );

	//Misc registration
	if( bAll_Users )
		strcpy( SubKey, "CLSID\\{00B73327-7348-4ef0-9D81-0C5CC6B6758B}\\TypeLib\0" );
	else
		strcpy( SubKey, "Classes\\CLSID\\{00B73327-7348-4ef0-9D81-0C5CC6B6758B}\\TypeLib\0" );
	if( ERROR_SUCCESS != RegCreateKeyEx( Root, SubKey, 0, 0, REG_OPTION_NON_VOLATILE,
										KEY_ALL_ACCESS, NULL, &hKey, NULL ) )
			return SELFREG_E_CLASS;

	RegSetValueEx( hKey, "", 0, REG_SZ, (UCHAR*)"{00B73327-7348-4ef0-9D81-0C5CC6B6758B}\0", strlen("{00B73327-7348-4ef0-9D81-0C5CC6B6758B}\0")+1 );

	RegCloseKey( hKey );

	//Enable the IOfficeAntiVirus Feature
	if( ERROR_SUCCESS != RegCreateKeyEx( HKEY_CLASSES_ROOT, "Component Categories\\{56FFCC30-D398-11D0-B2AE-00A0C908FA49}\0",
										0, 0, REG_OPTION_NON_VOLATILE,
										KEY_ALL_ACCESS, NULL, &hKey, NULL ) )
			return SELFREG_E_CLASS;
	RegSetValueEx( hKey, "409", 0, REG_SZ, (UCHAR*)"\0", 1 );

	RegCloseKey( hKey );

	flog( "DllRegisterServer() completed successfully" );

	return S_OK;
}

//---------------------------------------------------------------------------
// DllUnregisterServer
//	Called by regsvr32.exe to tell the DLL to unregister the class
//	(uninstall the module)
//---------------------------------------------------------------------------
STDAPI DllUnregisterServer(void)
{
	HKEY hKey;
	BOOL bInstalled;

	//Ask the user whether he wants to uninstall the Plugin
	if( MessageBox( NULL, Language_strings->CONFIRMUNINSTALL, "ClamWin MS Office Document virus scanner", MB_OKCANCEL ) == IDCANCEL )
		return SELFREG_E_CLASS;

	flog( "DllUnregisterServer() started" );

	//Check if we are not uninstalling something that hasn't been ever installed...
	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CLASSES_ROOT, "CLSID\\{00B73327-7348-4ef0-9D81-0C5CC6B6758B}\0", 0, KEY_READ, &hKey ) )
	{
		bInstalled = 1;
		RegCloseKey( hKey );
	}
	else if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, "Classes\\CLSID\\{00B73327-7348-4ef0-9D81-0C5CC6B6758B}\0", 0, KEY_READ, &hKey ) )
	{
		bInstalled = 1;
		RegCloseKey( hKey );
	}
	else
		bInstalled = 0;

	//Delete the keys
	RegDeleteKey( HKEY_CLASSES_ROOT, "CLSID\\{00B73327-7348-4ef0-9D81-0C5CC6B6758B}\0" );
	RegDeleteKey( HKEY_CURRENT_USER, "Classes\\CLSID\\{00B73327-7348-4ef0-9D81-0C5CC6B6758B}\0" );

	//Disable the IOfficeAntiVirus Feature ONLY if there was a previous installation:
	//we could be disabling another AV engine
	if( bInstalled )
		RegDeleteKey( HKEY_CLASSES_ROOT, "Component Categories\\{56FFCC30-D398-11D0-B2AE-00A0C908FA49}\0" );

	RegDeleteKey( HKEY_LOCAL_MACHINE, "Software\\ClamWin\\Office Plugin\\Language\0" );
	RegDeleteKey( HKEY_LOCAL_MACHINE, "Software\\ClamWin\\Office Plugin\\Settings\0" );
	RegDeleteKey( HKEY_LOCAL_MACHINE, "Software\\ClamWin\\Office Plugin\0" );

	flog( "DllUnregisterServer() completed successfully" );

	return S_OK;
}

//---------------------------------------------------------------------------
// ChangeLanguage
//	Displays the dialog to change language.
//---------------------------------------------------------------------------
void CALLBACK ChangeLanguage(
  HWND hwnd,		// handle to owner window
  HINSTANCE hinst,	// instance handle for the DLL
  LPTSTR lpCmdLine,	// string the DLL will parse
  int nCmdShow )	// show state
{
	//Display the language choose dialog
	DialogBox( DllInstance,
			MAKEINTRESOURCE(IDD_LANGUAGECHOOSE),
			NULL,
			(DLGPROC) LangChooseProc );
	//Reload language buffer
	LoadLanguage();
}

//---------------------------------------------------------------------------
// Configure
//	Displays the dialog to change settings
//---------------------------------------------------------------------------
void CALLBACK Configure(
  HWND hwnd,		// handle to owner window
  HINSTANCE hinst,	// instance handle for the DLL
  LPTSTR lpCmdLine,	// string the DLL will parse
  int nCmdShow )	// show state
{
	//Display the language choose dialog
	DialogBox( DllInstance,
			MAKEINTRESOURCE(IDD_CONFIG),
			NULL,
			(DLGPROC) ConfigProc );
}

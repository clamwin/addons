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
#include "Office_ClamWin.h"

sOCW_Settings OCW_Settings;

typedef struct _REG_INT
{
	char *Reg_name;
	void *point;
	int   Default_value;
}REG_INT;

const REG_INT reg_ints[] =
{
	{"OLE Scan\0",		&OCW_Settings.OLE_Scan,		1},
	{"Delay time\0",	&OCW_Settings.DelayTime,	4000},
	{"Enable logging\0",&OCW_Settings.Logging,		0}
};

//---------------------------------------------------------------------------
// LoadSettings()
//	Loads most settings from registry
//---------------------------------------------------------------------------
void LoadSettings()
{
	HKEY hKey;
	DWORD dwCreateNew; //Were the settings saved at least once to the reg?
	DWORD dwType, dwSize;

	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\ClamWin\0", 0, KEY_READ, &hKey ) )
	{
		RegCloseKey( hKey );
		RegCreateKeyEx( HKEY_CURRENT_USER, "Software\\ClamWin\\Office Plugin\\Settings\0", 0, 0,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, &dwCreateNew );
	}
	else
		RegCreateKeyEx( HKEY_LOCAL_MACHINE, "Software\\ClamWin\\Office Plugin\\Settings\0", 0, 0,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, &dwCreateNew );
createkey:
	//Save the default values if we are creating the reg key
	if( dwCreateNew == REG_CREATED_NEW_KEY )
		for( unsigned int i=0; i<sizeof(reg_ints)/sizeof(REG_INT); i++ )
			RegSetValueEx( hKey, reg_ints[i].Reg_name, 0, REG_DWORD, (unsigned char*)&reg_ints[i].Default_value, 4 );;

	//Load the values
	for( unsigned int i=0; i<sizeof(reg_ints)/sizeof(REG_INT); i++ )
	{
		dwType = REG_DWORD;
		dwSize = 4;
		if( ERROR_SUCCESS !=
			RegQueryValueEx( hKey, reg_ints[i].Reg_name, 0, &dwType,
				(unsigned char*)reg_ints[i].point, &dwSize ) )
		{
			dwCreateNew = REG_CREATED_NEW_KEY;
			goto createkey;
		}
	}

	RegCloseKey( hKey );
}

//---------------------------------------------------------------------------
// SaveSettings()
//	Saves most settings from registry. Note that if OCW_Settings should have
//	been initialized with at least default values.
//---------------------------------------------------------------------------
void SaveSettings()
{
	HKEY hKey;

	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\ClamWin\\Office Plugin\0", 0, KEY_READ, &hKey ) )
	{
		RegCloseKey( hKey );
		RegCreateKeyEx( HKEY_CURRENT_USER, "Software\\ClamWin\\Office Plugin\\Settings\0", 0, 0,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, 0 );
	}
	else
		RegCreateKeyEx( HKEY_LOCAL_MACHINE, "Software\\ClamWin\\Office Plugin\\Settings\0", 0, 0,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, 0 );

	//Save the values
	for( unsigned int i=0; i<sizeof(reg_ints)/sizeof(REG_INT); i++ )
		RegSetValueEx( hKey, reg_ints[i].Reg_name, 0, REG_DWORD,
				(unsigned char*)reg_ints[i].point, 4 );

	RegCloseKey( hKey );
}

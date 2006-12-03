//-----------------------------------------------------------------------------
// Name:        Clam_es.cpp
// Product:     Office ClamWin Antivirus plugin language module
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

#define BUILDING_LNG //Don't forget to define this if you're creating a Language module

#include <windows.h>
#include <stdio.h>
#include "Languages.h"

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	return TRUE;
}

//---------------------------------------------------------------------------
// GetLanguageSize()
//	Gets the number of strings from the language name+1
//	i.e. "Spanish" will return 7+1 = 8
//---------------------------------------------------------------------------
int GetLanguageSize()
{
	return strlen("Español\0")+1;
}

//---------------------------------------------------------------------------
// GetLanguageName()
//	Gets the name of this language
//	i.e. "Spanish"
//---------------------------------------------------------------------------
void GetLanguageName( char *name )
{
	memset( name, 0, strlen("Español\0")+1 );
	strcpy( name, "Español\0" );
}

//---------------------------------------------------------------------------
// GetDllVersion()
//	Gets the version of this language DLL (specs)
//---------------------------------------------------------------------------
unsigned long GetDllVersion()
{
	return CLAMWIN_LNG_VERSION;
}
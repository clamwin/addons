//-----------------------------------------------------------------------------
// Name:        Languages.h
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

#ifndef CLAMWIN_LNG
#define CLAMWIN_LNG

#define CLAMWIN_LNG_VERSION 0x00000003

#ifdef BUILDING_LNG
//---------------------------------------------------------------------------
// GetLanguageSize()
//	Gets the number of strings from the language name+1
//	i.e. "Spanish" will return 7+1 = 8
//---------------------------------------------------------------------------
int GetLanguageSize();

//---------------------------------------------------------------------------
// GetLanguageName()
//	Gets the name of this language
//	i.e. "Spanish"
//---------------------------------------------------------------------------
void GetLanguageName( char *name );

//---------------------------------------------------------------------------
// GetDllVersion()
//	Gets the version of this language DLL (specs)
//---------------------------------------------------------------------------
unsigned long GetDllVersion();

#else

#undef IDS_OLENOTSUPPORTED //don't allow for this version in read mode

typedef int (*RUNT_INT)();
typedef void (*RUNT_LNGNAME) (char*);
typedef unsigned long (*RUNT_DWORD)();

extern RUNT_INT GetLanguageSize;
extern RUNT_LNGNAME GetLanguageName;
extern RUNT_DWORD GetDllVersion;

#endif

#endif //!CLAMWIN_LNG
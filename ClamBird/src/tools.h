//-----------------------------------------------------------------------------
// Name:        tools.h
// Product:     ClamWin Antivirus Thunderbird Extension
//
// Author(s):      budtse [budtse at users dot sourceforge dot net]
//
// Created:     2006/11/07
// Copyright:   Copyright ClamWin Pty Ltd (c) 2005-2006
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
//   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef __tools_h__
#define __tools_h__

#ifndef NULL
#define NULL 0;
#endif

bool CheckFileExists(const char* aPath);
bool GetTempDirectory(char* aOutput, size_t aLength);
void mprintf(const char *str, ...);
char* TranslateClamReturnCode(int aCode, char* aBuffer, size_t aSize);


#endif // __tools_h__
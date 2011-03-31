//-----------------------------------------------------------------------------
// Name:        tools.cpp
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

#include <stdio.h>
#include <windows.h>
#include "tools.h"

bool mprintf_disabled = false;
bool mprintf_stdout = true;
bool mprintf_quiet = false;
bool mprintf_verbose = true;

bool CheckFileExists(const char* aPath) {
	WIN32_FIND_DATA findData;
	HANDLE hFile = FindFirstFile(aPath, &findData);
	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}
	FindClose(hFile);
	return true;
}

bool GetTempDirectory(char* aOutput, size_t aSize) {
	if (!aOutput) return false;
	if (aSize <= 0) return false;

    char *shortTempPath = new char[aSize];

	// Get the short name for the temporary path
	if (GetTempPath(aSize, shortTempPath) == 0) {
		delete shortTempPath;
		return false;
	}
	if (strlen(shortTempPath) == 0) {
		delete shortTempPath;
		return false;
	}
	// Convert the shortname to the longname
	if (GetLongPathName(shortTempPath, aOutput, aSize) == 0) {
		int ret = GetLastError();
		delete shortTempPath;
		return false;
	}
	delete shortTempPath;
	shortTempPath = NULL;

	// Remove the trailing \ if it is there
	if (*(aOutput + strlen(aOutput) - 1) == '\\')
		*(aOutput + strlen(aOutput) - 1) = 0;

	return true;
}

char* TranslateClamReturnCode(int aCode, char* aBuffer, size_t aSize) {

	if (aBuffer == NULL || aSize == 0)
		return aBuffer;

	switch(aCode) {
		case -50:
			strncpy(aBuffer, "CL_EUSERABORT (-50) : interrupted by callback", aSize);
			break;
		case -13:
			strncpy(aBuffer, "CL_EFORMAT (-13) : bad format or broken file", aSize);
			break;
		case -12:
			strncpy(aBuffer, "CL_EIO (-12) : general I/O error", aSize);
			break;
		case -11:
			strncpy(aBuffer, "CL_EDSIG (-11) : digital signature verification error", aSize);
			break;
		case -10:
			strncpy(aBuffer, "CL_EMD5 (-10) : MD5 verification error", aSize);
			break;
		case -9:
			strncpy(aBuffer, "CL_ECVDEXTR (-9) : CVD extraction failure", aSize);
			break;
		case -8:
			strncpy(aBuffer, "CL_ECVD (-8) : not a CVD file (or broken)", aSize);
			break;
		case -7:
			strncpy(aBuffer, "CL_ETMPDIR (-7) : mkdir() failed", aSize);
			break;
		case -6:
			strncpy(aBuffer, "CL_EPATSHORT (-6) : pattern too short", aSize);
			break;
		case -5:
			strncpy(aBuffer, "CL_EMALFDB (-5) : malformed database", aSize);
			break;
		case -4:
			strncpy(aBuffer, "CL_EOPEN (-4) : file open error", aSize);
			break;
		case -3:
			strncpy(aBuffer, "CL_EMEM (-3) : memory allocation error", aSize);
			break;
		case -2:
			strncpy(aBuffer, "CL_EFSYNC (-2) : fsync() failed", aSize);
			break;
		case -1:
			strncpy(aBuffer, "CL_ETMPFILE (-1) : tmpfile() failed", aSize);
			break;
		case 0:
			strncpy(aBuffer, "CL_CLEAN (0) : virus not found", aSize);
			break;
		case 1:
			strncpy(aBuffer, "CL_VIRUS (1) : virus found", aSize);
			break;
		case 10:
			strncpy(aBuffer, "CL_EMAXREC (10) : virus found", aSize);
			break;
		case 11:
			strncpy(aBuffer, "CL_EMAXSIZE (11) : size limit exceeded", aSize);
			break;
		case 12:
			strncpy(aBuffer, "CL_EMAXFILES (12) : files limit exceeded", aSize);
			break;
		case 100:
			strncpy(aBuffer, "CL_ERAR (100) : rar handler error", aSize);
			break;
		case 101:
			strncpy(aBuffer, "CL_EZIP (101) : zip handler error", aSize);
			break;
		case 102:
			strncpy(aBuffer, "CL_EMALFZIP (102) : malformed zip", aSize);
			break;
		case 103:
			strncpy(aBuffer, "CL_EGZIP (103) : gzip handler error", aSize);
			break;
		case 104:
			strncpy(aBuffer, "CL_EBZIP (104) : bzip2 handler error", aSize);
			break;
		case 105:
			strncpy(aBuffer, "CL_EOLE2 (105) : OLE2 handler error", aSize);
			break;
		case 106:
			strncpy(aBuffer, "CL_EMSCOMP (106) : compress.exe handler error", aSize);
			break;
		case 107:
			strncpy(aBuffer, "CL_EMSCAB (107) : MS CAB module error", aSize);
			break;
		case 108:
			strncpy(aBuffer, "CL_EM7ZIP (108) : 7zip module error", aSize);
			break;
		case 200:
			strncpy(aBuffer, "CL_EACCES (200) : access denied", aSize);
			break;
		case 300:
			strncpy(aBuffer, "CL_ENULLARG (300) : null argument error", aSize);
			break;
		default:
			char buf[100];
			sprintf(buf, "%d is UNKNOWN CLAMAV RETURN CODE", aCode);
			strncpy(aBuffer, buf, aSize);
			break;
	}
	return aBuffer;
}

// mprintf : simply use stdout, which can be redirected to file output
// the command syntax of mprintf was copied from the clam project
void mprintf(const char *str, ...)
{
	va_list args, argscpy;

	if(mprintf_disabled) {
		return;
	}

/* legend:
 * ! - ERROR
 * $ - WARNING
 * ...
 */

/*
 *             ERROR    WARNING    STANDARD
 * normal      stderr   stderr     stdout
 * 
 * verbose     stderr   stderr     stdout
 * 
 * quiet       stderr     no         no
 */

	va_start(args, str);
	/* va_copy is less portable so we just use va_start once more */
	va_start(argscpy, str);

	if(*str == '!') {
		printf("ERROR: ");
		vprintf(++str, args);
		printf("\n");
	} else if(*str == '@') {
		printf("ERROR: ");
		vprintf(++str, args);
		printf("\n");
	} else if(!mprintf_quiet) {
		if(*str == '$') {
			printf("WARNING: ");
			vprintf(++str, args);
			printf("\n");
		} else if(*str == '*') {
			if(mprintf_verbose) {
				vprintf(++str, args);
				printf("\n");
			}
		} else {
			vprintf(str, args);
			printf("\n");
		}
	}

	va_end(args);
	va_end(argscpy);

	fflush(stdout);
}
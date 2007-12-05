/*
 * Win32 Process Helpers
 *
 * Copyright (c) 2007 Gianluigi Tiesi <sherpya@netfarm.it>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this software; if not, write to the
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _PYHELPERS_H
#define _PYHELPERS_H
#include <windows.h>
#include <Tlhelp32.h>
#include <Python.h>

#define PYHELPERS_VERSION "1.0"

#ifndef TH32CS_SNAPMODULE32
#define TH32CS_SNAPMODULE32 0x00000010
#endif

/* kernel32 */
typedef HANDLE (WINAPI *imp_CreateToolhelp32Snapshot)(DWORD, DWORD);
typedef BOOL (WINAPI *imp_Process32First)(HANDLE, PROCESSENTRY32 *);
typedef BOOL (WINAPI *imp_Process32Next)(HANDLE, PROCESSENTRY32 *);
typedef BOOL (WINAPI *imp_Module32First)(HANDLE, MODULEENTRY32 *);
typedef BOOL (WINAPI *imp_Module32Next)(HANDLE, MODULEENTRY32 *);

/* advapi32 */
typedef BOOL (WINAPI *imp_OpenProcessToken)(HANDLE, DWORD, PHANDLE);
typedef BOOL (WINAPI *imp_LookupPrivilegeValueA)(LPCSTR, LPCSTR, PLUID);
typedef BOOL (WINAPI *imp_AdjustTokenPrivileges)(HANDLE, BOOL, PTOKEN_PRIVILEGES, DWORD, PTOKEN_PRIVILEGES, PDWORD);

typedef HANDLE (WINAPI *imp_CreateRemoteThread)(HANDLE,
                LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);

typedef struct _memhelpers_t
{
    BOOL ok;
    imp_CreateToolhelp32Snapshot CreateToolhelp32Snapshot;
    imp_Process32First Process32First;
    imp_Process32Next Process32Next;
    imp_Module32First Module32First;
    imp_Module32Next Module32Next;
    imp_CreateRemoteThread CreateRemoteThread;

    imp_OpenProcessToken OpenProcessToken;
    imp_LookupPrivilegeValueA LookupPrivilegeValueA;
    imp_AdjustTokenPrivileges AdjustTokenPrivileges;

} memhelpers_t;

extern PyObject *PyhelpersError;

extern memhelpers_t pyhi_helpers;
extern int EnablePrivilege(LPCSTR PrivilegeName, DWORD yesno);
extern int pyhi_dynlink(void);
extern int initToolHelp(PyObject *module);

extern int killProcess(DWORD pid);
extern int unloadModule(DWORD pid, HANDLE hModule);

#endif /* _PYHELPERS_H */
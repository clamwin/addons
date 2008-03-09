/*
 * Win32 Process Helpers
 *
 * Copyright (c) 2008 Gianluigi Tiesi <sherpya@netfarm.it>
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

#include <windows.h>
#include "pyhelpers.h"

memhelpers_t pyhi_helpers;

#define TIMEOUT_MODULE  30000

/* Needed to Scan System Processes */
int EnablePrivilege(LPCSTR PrivilegeName, DWORD yesno)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if(!pyhi_helpers.OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY | TOKEN_READ, &hToken))
        return 0;

    if(!pyhi_helpers.LookupPrivilegeValueA(NULL, PrivilegeName, &luid))
        return 0;

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = yesno;

    pyhi_helpers.AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);
    CloseHandle(hToken);
    return (GetLastError() == ERROR_SUCCESS) ? 1 : 0;
}

int killProcess(DWORD pid)
{
    HANDLE hProc;
    if ((hProc = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, pid)))
    {
        TerminateProcess(hProc, 0);
        if (WaitForSingleObject(hProc, TIMEOUT_MODULE) != WAIT_OBJECT_0)
            fprintf(stderr, "Unable to unload process from memory\n");
        CloseHandle(hProc);
    }
    else
        fprintf(stderr, "OpenProcess() failed %d\n", GetLastError());
    return 1; /* Skip to next process anyway */
}

/* Not so safe ;) */
int unloadModule(DWORD pid, HANDLE hModule)
{
    DWORD rc = 1;
    HANDLE ht;
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);

    if (!hProcess)
    {
        fprintf(stderr, "OpenProcess() failed %d\n", GetLastError());
        return 1; /* Skip to next process */
    }

    if ((ht = pyhi_helpers.CreateRemoteThread(hProcess, 0, 0, FreeLibrary, hModule, 0, &rc)))
    {
        if(WaitForSingleObject(ht, TIMEOUT_MODULE) == WAIT_TIMEOUT)
        {
            CloseHandle(ht);
            CloseHandle(hProcess);
            fprintf(stderr, "The module may trying to trick us, killing the process, please rescan\n");
            return killProcess(pid);
        }
        CloseHandle(ht);
        rc = 0; /* Continue scanning this process */
    }
    else
    {
        DWORD res = GetLastError();
        if (res == ERROR_CALL_NOT_IMPLEMENTED)
        {
            fprintf(stderr, "Module unloading is not supported on this OS\n");
            rc = -1; /* Don't complain about removing/moving the file */
        }
        else
        {
            fprintf(stderr, "CreateRemoteThread() failed %d\n", res);
            rc = 1; /* Skip to next process */
        }
    }

    CloseHandle(hProcess);
    return rc;
}


/* ---- */
#define Q(string) # string
#define IMPORT_FUNC(hLib, x) \
    pyhi_helpers.x = ( imp_##x ) GetProcAddress(hLib, Q(x)); \
    if (!pyhi_helpers.x) return 0;

int pyhi_dynlink(void)
{
    HINSTANCE hKernel = GetModuleHandleA("kernel32.dll");
    HINSTANCE hAdvapi32 = GetModuleHandleA("advapi32.dll");

    if (!hKernel || !hAdvapi32) return 0;
    memset(&pyhi_helpers, 0, sizeof(pyhi_helpers));

    /* kernel32 */
    IMPORT_FUNC(hKernel, CreateToolhelp32Snapshot);
    IMPORT_FUNC(hKernel, Process32First);
    IMPORT_FUNC(hKernel, Process32Next);
    IMPORT_FUNC(hKernel, Module32First);
    IMPORT_FUNC(hKernel, Module32Next);
    IMPORT_FUNC(hKernel, CreateRemoteThread);

    /* advapi32 */
    IMPORT_FUNC(hAdvapi32, OpenProcessToken);
    IMPORT_FUNC(hAdvapi32, LookupPrivilegeValueA);
    IMPORT_FUNC(hAdvapi32, AdjustTokenPrivileges);

    EnablePrivilege(SE_DEBUG_NAME, SE_PRIVILEGE_ENABLED);
    return 1;
}

#ifdef XP_WIN
#include <windows.h>

#define UNEXPECTED_ERROR 8
#define NEED_REBOOT 100

int RenameFileOnReboot(const char *source, const char *dest)
{
    OSVERSIONINFO version;
    char src[MAX_PATH], dst[MAX_PATH] = "NUL";

    version.dwOSVersionInfoSize = sizeof(version);
    GetVersionEx(&version);
    if ((version.dwPlatformId == VER_PLATFORM_WIN32_NT) || (version.dwMajorVersion != 4))
    {
        if (!GetFullPathName(source, MAX_PATH, src, NULL)) return -1;
        if (dest && !GetFullPathName(dest, MAX_PATH, dst, NULL)) return -1;
        return MoveFileEx(src, dest ? dst : NULL, MOVEFILE_DELAY_UNTIL_REBOOT) ? NEED_REBOOT : -1;
    }
    else
    {
        char wininit[MAX_PATH];
        if (!GetWindowsDirectory(wininit, MAX_PATH)) return -1;
        strncat(wininit, "\\wininit.ini", MAX_PATH);

        if (!GetShortPathName(source, src, MAX_PATH)) return -1;
        if (dest && !GetShortPathName(dest, dst, MAX_PATH)) return -1;
        return WritePrivateProfileString("rename", dest, src, wininit) ? NEED_REBOOT : -1;
    }
}
#endif
//-----------------------------------------------------------------------------
// Name:        nsClamWinService.cpp
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
//   along with this program; if not, write to the 
//      Free Software Foundation, Inc., 
//      51 Franklin Street, Fifth Floor, 
//      Boston, MA 02110-1301 USA
//
//   Parts of the code are based on or inspired by:
//     - pyclamav (http://xael.org/norman/python/pyclamav/) 
//       by Alexandre Norman, released under GPL
//     - oladdin (http://www.clamwin.com) by Alch, released under GPL
//

#include "nsClamWinService.h"
#include <nsStringApi.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include "tools.h"
#include <process.h>
#include <shlobj.h>

NS_IMPL_ISUPPORTS1(nsClamwinService, nsIClamwinService)


nsClamwinService::nsClamwinService() {
  /* member initializers and constructor code */
	mInitialized = false;
	mTempDirectory = NULL;
	mQuarantineDirectory = NULL;
	mDatabasePath = NULL;
	mRoot = NULL;
	mDatabaseOpen = false;
    mDatabaseVersionMain = 0;
    mDatabaseVersionDaily = 0;
}

nsClamwinService::~nsClamwinService() {
  /* destructor code */
	if (mTempDirectory != NULL) {
		delete mTempDirectory;
		mTempDirectory = NULL;
	}
	if (mQuarantineDirectory != NULL) {
		delete mQuarantineDirectory;
		mQuarantineDirectory = NULL;
	}
	if (mDatabasePath != NULL) {
		delete mDatabasePath;
		mDatabasePath = NULL;
	}
}

/* void Initialize (); */
NS_IMETHODIMP nsClamwinService::Initialize() {

	if (mInitialized) {
		// Already initialized
		mprintf("!nsClamwinService::Initialize : already initialized");
		return NS_OK;
	}

	mClamWinConfigFile = new char[MAX_PATH + 1];
	if (GetClamWinConfigFile(mClamWinConfigFile, MAX_PATH) == 0)
	{
		delete mClamWinConfigFile;
		mClamWinConfigFile = NULL;
		return NS_ERROR_UNEXPECTED;
	}

	// Get the temporary file path
	mTempDirectory = new char[MAX_PATH + 1];
	if (!GetTempDirectory(mTempDirectory, MAX_PATH))
	{
		delete mTempDirectory;
		mTempDirectory = NULL;
		return NS_ERROR_UNEXPECTED;
	}

	// Get the ClamWin path from the registry
	// We need it to add the bin directory to the path, so we
	// always use the latest version of libclamav available.
	mClamWinPath = new char[MAX_PATH + 1];
	if (GetClamWinPath(mClamWinPath, MAX_PATH) == 0)
	{
		delete mClamWinPath;
		mClamWinPath = NULL;
		return NS_ERROR_UNEXPECTED;
	}

	// Initialize logging to nsClamWinService.log
	char* logFile = new char[MAX_PATH + 1];
	if (strlen(mTempDirectory) + 21 >= MAX_PATH) {
		return NS_ERROR_UNEXPECTED;
	}
	sprintf(logFile, "%s\\nsClamWinService.log",
		mTempDirectory);
	
	FILE* hFile = freopen(logFile, "w", stdout);
	delete logFile;
	if (hFile)
	{
		mprintf("Initializing nsClamWinService.cpp");
	}

	// Read Quarantine dir from the configuration file
	mQuarantineDirectory = new char[MAX_PATH + 1];
	if (GetPrivateProfileString("ClamAV", "quarantinedir", "", mQuarantineDirectory, MAX_PATH, mClamWinConfigFile) == 0)
	{
		delete mQuarantineDirectory;
		mQuarantineDirectory = NULL;
		return NS_ERROR_UNEXPECTED;
	}
	mprintf("Quarantine Directory = '%s'", mQuarantineDirectory);

	// Read database path from the configuration file
	mDatabasePath = new char[MAX_PATH + 1];
	if (GetPrivateProfileString("ClamAV", "database", "", mDatabasePath, MAX_PATH, mClamWinConfigFile) == 0)
	{
		delete mDatabasePath;
		mDatabasePath = NULL;
		return NS_ERROR_UNEXPECTED;
	}
	mprintf("Database Path = '%s'", mDatabasePath);

	// Prepare the mLimits structure for the calls to libclamav
	memset(&mLimits, 0, sizeof(struct cl_limits));
	mLimits.maxfiles = 1000;                /* max files */
	mLimits.maxfilesize = 1 * 1048576;    /* maximal archived file size == 5 Mb */
	mLimits.maxreclevel = 5;             /* maximal recursion level */
	mLimits.maxratio = 200;             /* maximal compression ratio */
	mLimits.archivememlim = 0;           /* disable memory limit for bzip2 scanner */

	LoadDatabase();

	mInitialized = true;
	mprintf("Out nsClamWinService::Initialize");

	return NS_OK;
}


int nsClamwinService::LoadDatabase(void)
{
    int returnValue = 0;

    // so loaddb can reused
    if (mRoot)
    {
        cl_free(mRoot);
        mRoot = NULL;
    }

    if((returnValue = cl_load(mDatabasePath ? mDatabasePath : cl_retdbdir(), &mRoot, &mNumberOfSignatures, CL_DB_STDOPT)))
        return returnValue;

    // build the final tree
    if((returnValue = cl_build(mRoot)))
    {
        // free the partial tree
        cl_free(mRoot);
        return returnValue;
    }

    GetDatabaseVersion(&mDatabaseVersionMain, &mDatabaseVersionDaily);
    return 0;
}

void nsClamwinService::GetDatabaseVersion(int *pMainVersion, 
										  int *pDailyVersion)
{
    const char *databaseDirectory;
    char *path = NULL;
    struct cl_cvd *cvd;
    size_t length = 0;

    // Database directory
    databaseDirectory = mDatabasePath[0] ? mDatabasePath : cl_retdbdir();

    // Main
    length = strlen(databaseDirectory) + 22;
    path = (char *) malloc(length);
    _snprintf(path, length, "%s/main.cvd", databaseDirectory);
    if(_access(path, 0) < 0)
        _snprintf(path, length, "%s/main.inc/main.info", databaseDirectory);
    if((cvd = cl_cvdhead(path)))
    {
        *pMainVersion = cvd->version;
        cl_cvdfree(cvd);
    }
    free(path);

    // Daily
    length = strlen(databaseDirectory) + 22;
    path = (char *) malloc(length);
    _snprintf(path, length, "%s/daily.cvd", databaseDirectory);
    if(_access(path, 0) < 0)
        _snprintf(path, length, "%s/daily.inc/daily.info", databaseDirectory);
    if((cvd = cl_cvdhead(path)))
    {
        *pDailyVersion = cvd->version;
        cl_cvdfree(cvd);
    }
    free(path);
}

int nsClamwinService::CheckDatabaseChangesAndReload()
{
	int mainVersion = 0;
    int dailyVersion = 0;

    // Test if the database have changed or not yet loaded
    GetDatabaseVersion(&mainVersion, &dailyVersion);

    // If database changed : Reload
    if (!mRoot || (mainVersion != mDatabaseVersionMain) || 
			(dailyVersion != mDatabaseVersionDaily))
        return LoadDatabase();
    return 0;
}

int nsClamwinService::ScanFile(const char *aPath, char **aVirusName)
{
    unsigned long int size = 0;
    const char *virusName;
    int returnValue = 0;

    if ((returnValue = CheckDatabaseChangesAndReload()))
    {
		mprintf("Reloading database : %s", cl_strerror(returnValue));
        return -1;
    }

    returnValue = cl_scanfile(aPath, &virusName, &size, mRoot, &mLimits, CL_SCAN_STDOPT);

    // Test return code
    switch (returnValue)
    {
        case CL_VIRUS : // File contains a virus
			strcpy(*aVirusName, virusName);
            return CL_VIRUS;
        case CL_CLEAN : // File is clean
            return CL_CLEAN;
        default: // Abnormal value
			mprintf("cl_scanfile returned %d : %s", returnValue, cl_strerror(returnValue));
        return -1;
    }
}



// void Log (in string aMessage)
NS_IMETHODIMP nsClamwinService::Log(const char *aMessage)
{
	if (!mInitialized) return NS_ERROR_NOT_INITIALIZED;
	mprintf("JS : %s", aMessage);
    return NS_OK;
}

// readonly attribute ACString Version
NS_IMETHODIMP nsClamwinService::GetVersion(nsACString & aVersion) {
	aVersion.Assign(NSCLAMWINSERVICE_VERSION);
	mprintf("nsClamWinService::GetVersion() : Version = %s", NSCLAMWINSERVICE_VERSION);
    return NS_OK;
}

// short scanFile (in string path)
NS_IMETHODIMP nsClamwinService::ScanFile(const char *aPath, 
										 char **aReportFile, 
										 PRInt16 *_retval){

int size = MAX_PATH;
int returnValue;
bool isQuarantined = false;
char* virusName = new char[MAX_PATH];

	*aReportFile = new char[MAX_PATH + 1];
	if (!aPath) {
		mprintf("!nsClamwinService::ScanFile( NULL, *, *)");
		return NS_ERROR_NULL_POINTER;
	}
	if (!_retval) {
		mprintf("!nsClamwinService::ScanFile( '%s', NULL, *)", aPath);
		return NS_ERROR_NULL_POINTER;
	}
	mprintf("IN nsClamwinService::ScanFile('%s', *, *)", aPath);

	// First check if the file is already saved.  If not, exit.
	// This function will be called again until timeout
	if (!CheckFileExists(aPath)) {
		mprintf("File doesn't exist yet, return CL_FILESTATUS_SAVING...");
		*(_retval) = CL_FILESTATUS_SAVING;
		return NS_OK;
	}

	// Scan the file using libClamAv
	returnValue = ScanFile(aPath, &virusName);

	if (returnValue == CL_VIRUS) {

		// Virus detected
		mprintf("$FOUND VIRUS in '%s' : '%s'\n", aPath, virusName);

		// move the file to the quarantine folder here,
		// so the path can be included in a generated report
		char quarantinedFilename[MAX_PATH + 1];
		mprintf("Moving file to quarantine...");
		isQuarantined = MoveFileToQuarantine(aPath, quarantinedFilename, MAX_PATH);
		mprintf("Quarantined file name = '%s'", quarantinedFilename);

		// TODO: first we need a way to remove the report automatically,
		// for instance showing it and asking the user to save or delete.
		//
		// Generate a report text file in the temporary folder
		//mprintf("Generating report...");
		//if (!GenerateReport(aPath, quarantinedFilename, virusName, isQuarantined, 
		//	*aReportFile, size)) {
		//		*aReportFile = NULL;
		//}
		//mprintf("Report file name = '%s'", aReportFile);
	} else {
		if (returnValue != CL_CLEAN) {
			mprintf("ERROR OCCURRED SCANNING THE FILE : %d, %s", 
				returnValue, cl_strerror(returnValue));
		}
		mprintf("Deleting temporary copy...");
		DeleteFile(aPath);
		*aReportFile = NULL;
	}

	*(_retval) = returnValue;
    mprintf("OUT nsClamwinService::ScanFile. Return = %d", returnValue);

	return NS_OK;
}

// ACString getTempFilename (in string originalFilename);
NS_IMETHODIMP nsClamwinService::GetTempFilename(const char *aOriginalFilename, 
												nsACString & _retval) {
	if (!aOriginalFilename) {
		mprintf("!nsClamWinService::GetTempFilename( NULL, *)");
		return NS_ERROR_NULL_POINTER;
	}
	mprintf("IN nsClamwinService::GetTempFilename('%s', *)", aOriginalFilename);

	char* tempFileName = new char[MAX_PATH];
	
	if (mTempDirectory == NULL) {
		mprintf("!mTempDirectory = NULL");
		return NS_ERROR_UNEXPECTED;
	}

	// Try to use the original filename
	if (strlen(mTempDirectory) + 1 + strlen(aOriginalFilename) > MAX_PATH) {
		// The name will be too long, so just return an error for now
		mprintf("!Temp File Name too long");
		return NS_ERROR_UNEXPECTED;
	}
	sprintf(tempFileName, "%s\\%s", mTempDirectory, aOriginalFilename);

	// See if this file already exists
	WIN32_FIND_DATA findData;
	HANDLE hFile = FindFirstFile(tempFileName, &findData);
	int iExtra = 0;
	while (hFile != INVALID_HANDLE_VALUE) {
		// Close the file handle
		FindClose(hFile);

		iExtra++;
		// Stop after MAX_TEMP tries
		if (iExtra > MAX_TEMP) {
			mprintf("!Cannot generate unique filename for temp file");
			return NS_ERROR_UNEXPECTED;
		}
		// try another extension
		if (strlen(mTempDirectory) + strlen(aOriginalFilename) + 5 
			>= MAX_PATH) {
				mprintf("!Unique filename for tempfile too long");
				return NS_ERROR_UNEXPECTED;
		}
		sprintf(tempFileName, "%s\\%s.%03d", mTempDirectory, 
			aOriginalFilename, iExtra);
		// See if this file exists
		hFile = FindFirstFile(tempFileName, &findData);
	}
	mprintf("Temp File Name = '%s'", tempFileName);

	// Set the return value
	_retval.Assign(tempFileName);

	mprintf("OUT nsClamWinService::GetTempFilename");
    return NS_OK;
}

bool nsClamwinService::GetQuarantineFilename(const char *aOriginalFilename, 
											 char* aQuarantineFilename, 
											 size_t aSize) {

	if (aQuarantineFilename == NULL) {
		mprintf("!nsClamWinService::GetQuarantineFileName( NULL, *, *)");
		return false;
	}

	if (mQuarantineDirectory == NULL) {
		mprintf("!nsClamWinService::GetQuarantineFileName( '%s', NULL, *)", aOriginalFilename);
		return false;
	}
	mprintf("nsClamWinService::GetQuarantineFileName( '%s', '%s', *)", aOriginalFilename, mQuarantineDirectory);

	// Try to use the original filename
	if (strlen(mQuarantineDirectory) + 1 + strlen(aOriginalFilename) > aSize) {
		// The name will be too long, so just return an error for now
		mprintf("!Quarantine file name too long");
		return false;
	}

	sprintf(aQuarantineFilename, "%s\\%s", mQuarantineDirectory, 
		aOriginalFilename);
	
	// See if this file already exists
	WIN32_FIND_DATA findData;
	HANDLE hFile = FindFirstFile(aQuarantineFilename, &findData);
	int iExtra = 0;
	while (hFile != INVALID_HANDLE_VALUE) {
		// Close the file handle
		FindClose(hFile);

		iExtra++;
		// Stop after MAX_TEMP tries
		if (iExtra > MAX_TEMP) {
			mprintf("!Can't generate unique quarantine file name");
			return false;
		}
		// try another extension
		if (strlen(mQuarantineDirectory) + strlen(aOriginalFilename) + 5 >= MAX_PATH) {
			mprintf("!Unique quarantine file name too long");
			return false;
		}
		sprintf(aQuarantineFilename, "%s\\%s.%03d", 
			mQuarantineDirectory, aOriginalFilename, iExtra);
		// See if this file exists
		hFile = FindFirstFile(aQuarantineFilename, &findData);
	}
	mprintf("Quarantine file name = '%s'", aQuarantineFilename);
	mprintf("OUT nsClamWinService::GetQuarantineFilename");
    return true;
}

// ACString getTempFilename (in string originalFilename);
NS_IMETHODIMP nsClamwinService::GetQuarantineFilename(const char *aOriginalFilename, 
												nsACString & _retval) {

	if (!aOriginalFilename) {
		mprintf("!nsClamWinService::GetQuarantineFilename( NULL, *)");
		return NS_ERROR_NULL_POINTER;
	}
	mprintf("IN nsClamWinService::GetQuarantineFilename( '%s', *)", aOriginalFilename);

	char* quarantineFilename = new char[MAX_PATH];

	if (!GetQuarantineFilename(aOriginalFilename, quarantineFilename, 
		MAX_PATH)) {
			delete quarantineFilename;
			mprintf("!Can't get unique Quarantine file name");
			return NS_ERROR_UNEXPECTED;
	} 
	mprintf("OUT nsClamWinService::GetQuarantineFilename");
	return NS_OK;
}

bool nsClamwinService::MoveFileToQuarantine(const char* aFilename, 
											char* aQuarantinedFilename,
											const size_t aSize) {

	if (aQuarantinedFilename == NULL) {
		mprintf("nsClamWinService::MoveFileToQuarantine( NULL, *, *)");
		return false;
	}
	mprintf("IN nsClamWinService::MoveFileToQuarantine( '%s', *, *)", aFilename);

	char *filename;
	char *leafname;
	char *p;

	filename = new char[strlen(aFilename) + 1];
	strcpy(filename, aFilename);

	leafname = filename;
	p = strstr(leafname, "\\");
	while (p != NULL) {
		leafname = p + 1;
		p = strstr(leafname, "\\");
	}

	if (!GetQuarantineFilename(leafname, aQuarantinedFilename, 
		aSize)) {
			delete filename;
			mprintf("!Can't create unique Quarantine File name");
			return false;
	} 
	delete filename;
	filename = NULL;

	if (rename(aFilename, aQuarantinedFilename) != 0) {
		mprintf("!Failed to move file");
		return false;
	}
	mprintf("OUT nsClamWinService::MoveFileToQuarantine");
	return true;
}

bool nsClamwinService::GenerateReport(const char* aOriginalFilename, 
									  const char* aQuarantinedFilename, 
									  const char* aVirusname,
									  const bool aIsQuarantined,
									  char* aReportFilename,
									  size_t aSize) {
	if (aOriginalFilename == NULL) {
			mprintf("!nsClamwinService::GenerateReport( NULL, *, *, *, *, *, *)");
			return false;
	}
	if (aQuarantinedFilename == NULL) {
			mprintf("!nsClamwinService::GenerateReport( '%s', NULL, *, *, *, *, *)", aOriginalFilename);
			return false;
	}
	if (aReportFilename == NULL) {
			mprintf("!nsClamwinService::GenerateReport( '%s', '%s', *, *, *, *, *)", aOriginalFilename, aQuarantinedFilename);
			return false;
	}
	mprintf("IN nsClamwinService::GenerateReport( '%s', '%s', '%s', *, *, *, *)", aOriginalFilename, aQuarantinedFilename, aReportFilename);

	// Get a filename for the report in the temporary directory
	if (strlen(mTempDirectory) >= aSize) {
		mprintf("!Report File name buffer too small");
		return false;
	}
	if (strlen(mTempDirectory) + strlen(aOriginalFilename) + 24 >= aSize) {
		mprintf("!Report File name too long");
		return false;
	}
	sprintf(aReportFilename, "%s\\ClamWin Scan Report.txt", mTempDirectory);
	// See if this file already exists
	WIN32_FIND_DATA findData;
	HANDLE hFile = FindFirstFile(aReportFilename, &findData);
	int iExtra = 0;
	while (hFile != INVALID_HANDLE_VALUE) {
		// Close the file handle
		FindClose(hFile);

		iExtra++;
		// Stop after MAX_TEMP tries
		if (iExtra > MAX_TEMP) {
			mprintf("!Can't generate unique filename for report file");
			return false;
		}
		// try another extension
		if (strlen(mTempDirectory) + strlen(aOriginalFilename) + 28 
			>= aSize) {
			mprintf("!Unique filename for report file too long");
			return false;
		}
		sprintf(aReportFilename, "%s\\ClamWin Scan Report.%03d.txt", 
			mTempDirectory, iExtra);
		// See if this file exists
		hFile = FindFirstFile(aReportFilename, &findData);
	}
	mprintf("Report file name = '%s'", aReportFilename);

	// open the file
	FILE* fp = fopen(aReportFilename, "w");
	if (fp == NULL) {
		mprintf("!Failed to open report file for writing");
		return false;
	}

	// Write report
	fprintf(fp, "==================================================\n");
	fprintf(fp, " ClamWin Free Antivirus extension for Thunderbird \n");
	fprintf(fp, "==================================================\n\n");
	fprintf(fp, "The virus %s was found in attachment %s\n\n", aVirusname, 
		aOriginalFilename);
	if (aIsQuarantined) {
		fprintf(fp, "This file was moved to %s.\n", aQuarantinedFilename);
	} else {
		fprintf(fp, "The file could not be moved and is deleted.\n");
	}

	// close file
	fclose(fp);

	mprintf("OUT nsClamwinService::GenerateReport");
	return true;
}

// void CloseDatabase ();
NS_IMETHODIMP nsClamwinService::CloseDatabase()
{
	mprintf("nsClamWinService::CloseDatabase()");
	mDatabaseOpen = false;
	cl_free(mRoot);
	mRoot = NULL;
    return NS_OK;
}

size_t nsClamwinService::GetClamWinPath(char* aPath, size_t aSize)
{
    DWORD dwType;
    HKEY hKey;

	if (!aPath) return 0;
    if (aSize == 0) return 0;
    *aPath = '\0';

	// try in hkey_current_user
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, 
        "Software\\ClamWin", 0, KEY_READ, &hKey))
    {
        RegQueryValueEx(hKey, "Path", NULL, &dwType, (PBYTE)aPath, 
            (LPDWORD)&aSize);
        CloseHandle(hKey);
    }
    // try in hkey_local_machine if failed
    if (!strlen(aPath) && (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
        "Software\\ClamWin", 0, KEY_READ, &hKey)))

    {
        RegQueryValueEx(hKey, "Path", NULL, &dwType, (PBYTE)aPath, 
            (LPDWORD)&aSize);
        CloseHandle(hKey);
    }
	return strlen(aPath);
}

size_t nsClamwinService::GetClamWinConfigFile(char* aConfigFile, size_t aSize)
{
	char profileDir[MAX_PATH + 1];

	if (aConfigFile == NULL) return 0;
    if (aSize == 0) return 0;
	*aConfigFile = '\0';

	if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 
        SHGFP_TYPE_CURRENT, &profileDir[0])))
    {
		return 0;
    }

	strncpy(aConfigFile, &profileDir[0], aSize);
	strncat(aConfigFile, "\\.clamwin\\clamwin.conf", 
        aSize - strlen(aConfigFile));

	// check if this file exists
	if (!FileExists(aConfigFile))
	{
		char defaultConfigFile[MAX_PATH];
		// copy the standard clamwin.conf file from the clamwin folder if not
		sprintf(defaultConfigFile, "%s\\ClamWin.conf", mClamWinPath);
		if (!FileExists(defaultConfigFile))
		{
			strcpy(aConfigFile, "");
			return 0;
		}
		if (CopyFile(defaultConfigFile, aConfigFile, FALSE) == FALSE)
		{
			strcpy(aConfigFile, "");
			return 0;
		}
	}
	return strlen(aConfigFile);
}

bool nsClamwinService::FileExists(const char* aPath)
{
    if (!aPath) return false;
    if (strlen(aPath) == 0) return false;

    WIN32_FIND_DATA findData;
	HANDLE hFile = FindFirstFile(aPath, &findData);
	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}
	FindClose(hFile);
	return true;
}

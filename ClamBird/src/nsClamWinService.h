//-----------------------------------------------------------------------------
// Name:        nsClamWinService.h
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

#ifndef __nsClamWinService_h__
#define __nsClamWinService_h__


#include "nsIClamWinService.h"
#include <clamav.h>
#include <stdio.h>


#define NSCLAMWINSERVICE_CONTRACTID "@clamwin.com/clambird/nsClamWinService;1"
#define NSCLAMWINSERVICE_CLASSNAME "ClamWin XPCom Service"

// {4D598E8A-B69C-4826-B40B-90FE8F80286C}
#define NSCLAMWINSERVICE_CID			\
{ /* D0836FE0-4ACB-4c02-8614-08A2120196B9 */	\
  0xd0836fe0, 0x4acb, 0x4c02,			\
{ 0x86, 0x14, 0x8, 0xa2, 0x12, 0x1, 0x96, 0xb9 } }

#define NSCLAMWINSERVICE_VERSION "0.0.1"

const int MAX_PATH = 260;	// Maximum number of characters in a file path
const int MAX_TEMP = 100;	// Maximum number of temporary file combinations
						// before failure

// Extra return values for nsClamwinService::ScanFile()
// Other return values are the standard return values from libClamAV.
const int CL_FILESTATUS_SAVING 		= 20000;	// File is being saved
const int CL_FILESTATUS_SCANNING 	= 20001;	// File is being scanned
const int CL_FILESTATUS_CLEAR 		= 20003;	// File was scanned and not infected
const int CL_FILESTATUS_INFECTED 	= 20004;	// File was scanned and infected
const int CL_ERROR					= 20005;	// A fatal error occurred, file is not scanned

static struct cl_limits mLimits;

class nsClamwinService : public nsIClamwinService {
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICLAMWINSERVICE

  nsClamwinService();

private:
  ~nsClamwinService();

  size_t GetClamWinConfigFile(char* aConfigFile, size_t aSize);
  bool GetQuarantineFilename(const char *aOriginalFilename,
					char* aQuarantineFilename, size_t aSize);
  bool MoveFileToQuarantine(const char* aFilename,
	  char* aQuarantinedFile,
	  const size_t aSize);
  bool GenerateReport(const char* path, const char *quarantine,
	  const char* aVirusName, const bool isQuarantined, char* aReportFilename,
	  size_t aSize);
  bool ParseScanLog(const char* aScanLog, char* aVirusName, const size_t aSize);
  int LoadDatabase();
  void GetDatabaseVersion(int *pMainVersion, int *pDailyVersion);
  int CheckDatabaseChangesAndReload();
  int ScanFile(const char *aPath, char** aVirusName);
  size_t GetClamWinPath(char* aPath, size_t aSize);
  bool FileExists(const char* aPath);

  // private data members
  char* mClamWinPath;
  char* mClamWinConfigFile;
  char* mTempDirectory;
  char* mQuarantineDirectory;
  char* mDatabasePath;
  bool mInitialized;
  bool mDatabaseOpen;

  FILE *stdoutStream;

  // Structures for clamav
  struct cl_node *mRoot;
  unsigned int mNumberOfSignatures;
  int mDatabaseVersionMain;
  int mDatabaseVersionDaily;

};



#endif //__nsClamWinService_h__
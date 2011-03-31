//-----------------------------------------------------------------------------
// Name:        Office_ClamWin.h
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

#ifdef OFFICE_CLAMWIN_EXPORTS
#define OFFICE_CLAMWIN_API __declspec(dllexport)
#else
#define OFFICE_CLAMWIN_API __declspec(dllimport)
#endif

#define OCW_LOG_ENABLE

//These definitions were taken (and some were modified a bit)
//from <msoav.h> to fix a few compilations problems with some programs
typedef struct _msoavinfo
{
int cbsize;			//size of this struct
struct {
	ULONG fPath:1;			//when true use pwzFullPath else use lpstg
	ULONG fReadOnlyRequest:1;	//user requests file to be opened read/only
	ULONG fInstalled:1;	//the file at pwzFullPath is an installed file
	ULONG fHttpDownload:1;	//the file at pwzFullPath is a temp file downloaded from http/ftp
	};
HWND hwnd;			//parent window of the Office9 app
union {
	WCHAR *pwzFullPath;	//full path to the file about to be opened
	LPSTORAGE lpstg;	//OLE Storage of the doc about to be opened
	}u;
WCHAR *pwzHostName;	 // Host Office 9 apps name
WCHAR *pwzOrigURL;	 		// URL of the origin of this downloaded file.
}MSOAVINFO;

class IOffAVFactory : public IClassFactory
{
protected:
	ULONG p_Ref;
public:
	IOffAVFactory();
	~IOffAVFactory();

    // *** IUnknown methods ***
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID riid, LPVOID FAR* ppvObj);
    virtual ULONG STDMETHODCALLTYPE AddRef(THIS);
    virtual ULONG STDMETHODCALLTYPE Release(THIS);

    // *** IClassFactory methods ***
	virtual HRESULT STDMETHODCALLTYPE CreateInstance(LPUNKNOWN pUnkOuter,
															REFIID riid,
															LPVOID *ppvObj);
	virtual HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock)
	{ return NOERROR; }
};

class IOfficeAntiVirus : public IUnknown
{
protected:
	ULONG Of_Ref;
public:
	IOfficeAntiVirus();
	~IOfficeAntiVirus();

	// *** IUnknown methods ***
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(THIS_ REFIID riid, LPVOID FAR* ppvObj);
    virtual ULONG STDMETHODCALLTYPE AddRef(THIS);
    virtual ULONG STDMETHODCALLTYPE Release(THIS);

    // *** IOfficeAntiVirus methods ***
	virtual HRESULT STDMETHODCALLTYPE Scan( MSOAVINFO *pmsoavinfo );
};

typedef struct _sLanguage_strings
{
	TCHAR VIRUSFOUND[196];
	TCHAR ACCOUNTENABLEQ[196];
	TCHAR CONFIRMINSTALL[196];
	TCHAR CONFIRMUNINSTALL[196];
	TCHAR CLAMWNOTINSTALL[196];
	TCHAR CONFIRMCRASH[196];
	TCHAR EN_OLEMAYBESLOW[196];
	TCHAR OLEVIRUSFOUND[196];
	TCHAR LOGGING[64];
}sLanguage_strings;

typedef struct _sOCW_Settings
{
	unsigned long OLE_Scan; //Whether perform a scan to OLE objects
	unsigned long DelayTime; //Response time to restore the ClamWin window
	unsigned long Logging; //Log (Helps finding bugs)
}sOCW_Settings;

//Office_ClamWin.cpp
extern HINSTANCE DllInstance;
extern sLanguage_strings *Language_strings;

//Settings.cpp
extern sOCW_Settings OCW_Settings;
void LoadSettings();
void SaveSettings();

//Languages.cpp
void LoadLanguage();

LRESULT CALLBACK LangChooseProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK ConfigProc( HWND hDlg, UINT message, WPARAM wParam,LPARAM lParam );

//For logging purposes
#include <fstream.h>
extern ofstream of;

#ifdef OCW_LOG_ENABLE
#define log(text) { if (OCW_Settings.Logging) { of.open("C:\\OCW.log", ios::app); of << text << "\n"; of.close(); } }
//Force logging (no matter what OCW_Settings.Logging says)
#define flog(text) { of.open("C:\\OCW.log", ios::app); of << text << "\n"; of.close(); }
#else
#define log(test) {}
#define flog(test) {}
#endif

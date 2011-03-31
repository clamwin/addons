//-----------------------------------------------------------------------------
// Name:        Config.cpp
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
//

#include <windows.h>
#include <windowsx.h>
#include "Office_ClamWin.h"
#include "resource.h"

LRESULT CALLBACK ConfigProc( HWND hDlg, UINT message, WPARAM wParam,
                                 LPARAM lParam );

//---------------------------------------------------------------------------
// Function: ConfigProc()
// --------
//	Dialog proccedure for IDD_CONFIG
//---------------------------------------------------------------------------
LRESULT CALLBACK ConfigProc( HWND hDlg, UINT message, WPARAM wParam,
                                 LPARAM lParam )
{
	TCHAR string[24];
	switch (message)
    {
        case WM_INITDIALOG:
			//Set the icon for this dialog
			PostMessage( hDlg, WM_SETICON, ICON_BIG,   (LPARAM) LoadIcon( DllInstance, MAKEINTRESOURCE(IDI_CLAMICON)) );  // Set big icon
		    PostMessage( hDlg, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon( DllInstance, MAKEINTRESOURCE(IDI_CLAMICON)) );  // Set small icon

			//Display the settings
			LoadSettings();
			Button_SetCheck( GetDlgItem( hDlg, IDC_OLESCANENABLE ), OCW_Settings.OLE_Scan );
			SetDlgItemInt( hDlg, IDC_DELAY, OCW_Settings.DelayTime, 0 );

			//'Logging' checkbox
			Button_SetCheck( GetDlgItem( hDlg, IDC_LOGGING ), OCW_Settings.Logging );

			//Language specific
			LoadLanguage();
			SetDlgItemText( hDlg, IDC_OLESCANENABLE, Language_strings->EN_OLEMAYBESLOW );
			SetDlgItemText( hDlg, IDC_LOGGING,		 Language_strings->LOGGING );
			break;

        case WM_COMMAND:
			switch ( LOWORD (wParam))
			{
			case IDOK:
				OCW_Settings.OLE_Scan = Button_GetCheck( GetDlgItem( hDlg, IDC_OLESCANENABLE ) );
				OCW_Settings.Logging = Button_GetCheck( GetDlgItem( hDlg, IDC_LOGGING ) );
				GetDlgItemText( hDlg, IDC_DELAY, string, 24 );
				OCW_Settings.DelayTime = atoi(string);
				SaveSettings();
				EndDialog( hDlg, TRUE );
				break;
			case IDCANCEL:
				EndDialog( hDlg, IDCANCEL );
				break;
			}
            break;

        case WM_CLOSE:
            EndDialog( hDlg, IDCANCEL );
            return TRUE;
            break;
    }

    return 0;
}
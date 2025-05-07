/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#include "launcher.hxx"

#ifndef _WINDOWS_
#   define WIN32_LEAN_AND_MEAN
#if defined _MSC_VER
#pragma warning(push, 1)
#endif
#   include <windows.h>
#   include <shellapi.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif
#endif

#include <stdlib.h>
#include <malloc.h>

#define PACKVERSION(major,minor) MAKELONG(minor,major)


#ifdef __MINGW32__
extern "C" int APIENTRY WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
#else
extern "C" int APIENTRY _tWinMain( HINSTANCE, HINSTANCE, LPTSTR, int )
#endif
{
    // Set an explicit Application User Model ID for the process
    
    WCHAR szShell32[MAX_PATH];
    GetSystemDirectoryW(szShell32, MAX_PATH);
    wcscat(szShell32, L"\\Shell32.dll");

    HINSTANCE hinstDll = LoadLibraryW(szShell32);

    if(hinstDll)
    {
        DLLVERSIONINFO dvi;
        ZeroMemory(&dvi, sizeof(dvi));
        dvi.cbSize = sizeof(dvi);

        DLLGETVERSIONPROC pDllGetVersion;
        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");
        HRESULT hr = (*pDllGetVersion)(&dvi);

        if(SUCCEEDED(hr))
        {
            DWORD dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
            if(dwVersion >= PACKVERSION(6,1)) // Shell32 version in Windows 7
            {
                typedef HRESULT (WINAPI *SETCURRENTPROCESSEXPLICITAPPUSERMODELID)(PCWSTR);
                SETCURRENTPROCESSEXPLICITAPPUSERMODELID pSetCurrentProcessExplicitAppUserModelID;
                pSetCurrentProcessExplicitAppUserModelID =
                    (SETCURRENTPROCESSEXPLICITAPPUSERMODELID)GetProcAddress(hinstDll, "SetCurrentProcessExplicitAppUserModelID");

                if(pSetCurrentProcessExplicitAppUserModelID)
                    (*pSetCurrentProcessExplicitAppUserModelID) (APPUSERMODELID);
            }
        }
    }
    FreeLibrary(hinstDll);    

    // Retreive startup info

    STARTUPINFO aStartupInfo;

    ZeroMemory( &aStartupInfo, sizeof(aStartupInfo) );
    aStartupInfo.cb = sizeof( aStartupInfo );
    GetStartupInfo( &aStartupInfo );

    // Retrieve command line

    LPTSTR  lpCommandLine = GetCommandLine();

    {
        lpCommandLine = (LPTSTR)_alloca( sizeof(_TCHAR) * (_tcslen(lpCommandLine) + _tcslen(APPLICATION_SWITCH) + 2) );

        _tcscpy( lpCommandLine, GetCommandLine() );
        _tcscat( lpCommandLine, _T(" ") );
        _tcscat( lpCommandLine, APPLICATION_SWITCH );
    }


    // Calculate application name

    TCHAR   szApplicationName[MAX_PATH];
    TCHAR   szDrive[MAX_PATH];
    TCHAR   szDir[MAX_PATH];
    TCHAR   szFileName[MAX_PATH];
    TCHAR   szExt[MAX_PATH];

    GetModuleFileName( NULL, szApplicationName, MAX_PATH );
    _tsplitpath( szApplicationName, szDrive, szDir, szFileName, szExt );
    _tmakepath( szApplicationName, szDrive, szDir, OFFICE_IMAGE_NAME, _T(".exe") );


    PROCESS_INFORMATION aProcessInfo;

    BOOL    fSuccess = CreateProcess(
        szApplicationName,
        lpCommandLine,
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &aStartupInfo,
        &aProcessInfo );

    if ( fSuccess )
    {
        // Wait for soffice process to be terminated to allow other applications
        // to wait for termination of started process

        WaitForSingleObject( aProcessInfo.hProcess, INFINITE );

        CloseHandle( aProcessInfo.hProcess );
        CloseHandle( aProcessInfo.hThread );

        return 0;
    }

    DWORD   dwError = GetLastError();

    LPVOID lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dwError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR)&lpMsgBuf,
        0,
        NULL
    );

    // Display the string.
    MessageBox( NULL, (LPCTSTR)lpMsgBuf, NULL, MB_OK | MB_ICONERROR );

    // Free the buffer.
    LocalFree( lpMsgBuf );

    return GetLastError();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

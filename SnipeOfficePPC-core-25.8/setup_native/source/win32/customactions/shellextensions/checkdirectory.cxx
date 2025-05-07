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

#define _WIN32_WINNT 0x0401

#ifdef _MSC_VER
#pragma warning(push, 1) /* disable warnings within system headers */
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msiquery.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <malloc.h>
#include <assert.h>

#ifdef UNICODE
#define _UNICODE
#define _tstring    wstring
#else
#define _tstring    string
#endif
#include <tchar.h>
#include <string>
#include <queue>
#include <stdio.h>

#include <systools/win32/uwinapi.h>
#include <../tools/seterror.hxx>

static std::_tstring GetMsiProperty( MSIHANDLE handle, const std::_tstring& sProperty )
{
    std::_tstring result;
    TCHAR szDummy[1] = TEXT("");
    DWORD nChars = 0;

    if ( MsiGetProperty( handle, sProperty.c_str(), szDummy, &nChars ) == ERROR_MORE_DATA )
    {
        DWORD nBytes = ++nChars * sizeof(TCHAR);
        LPTSTR buffer = reinterpret_cast<LPTSTR>(_alloca(nBytes));
        ZeroMemory( buffer, nBytes );
        MsiGetProperty(handle, sProperty.c_str(), buffer, &nChars);
        result = buffer;
    }

    return result;
}

static void UnsetMsiProperty(MSIHANDLE handle, const std::_tstring& sProperty)
{
    MsiSetProperty(handle, sProperty.c_str(), NULL);
}

static void SetMsiProperty(MSIHANDLE handle, const std::_tstring& sProperty, const std::_tstring&)
{
    MsiSetProperty(handle, sProperty.c_str(), TEXT("1"));
}

extern "C" UINT __stdcall CheckInstallDirectory(MSIHANDLE handle)
{
    std::_tstring sInstallPath = GetMsiProperty(handle, TEXT("INSTALLLOCATION"));
    std::_tstring sOfficeHostnamePath = GetMsiProperty(handle, TEXT("OFFICEDIRHOSTNAME"));

    // MessageBox(NULL, sInstallPath.c_str(), "DEBUG", MB_OK);

    // unsetting all properties

    UnsetMsiProperty( handle, TEXT("DIRECTORY_NOT_EMPTY") );

    // 1. Searching for file setup.ini

    std::_tstring sSetupIniPath = sInstallPath + sOfficeHostnamePath + TEXT("\\program\\setup.ini");

    WIN32_FIND_DATA data;
    HANDLE hdl = FindFirstFile(sSetupIniPath.c_str(), &data);

    // std::_tstring mystr = "Searching for " + sSetupIniPath;
    // MessageBox(NULL, mystr.c_str(), "DEBUG", MB_OK);

    if ( IsValidHandle(hdl) )
    {
        // setup.ini found -> directory cannot be used for installation.
        SetMsiProperty( handle, TEXT("DIRECTORY_NOT_EMPTY"), TEXT("1") );
        SetMsiErrorCode( MSI_ERROR_DIRECTORY_NOT_EMPTY );
        // std::_tstring notEmptyStr = "Directory is not empty. Please choose another installation directory.";
        // std::_tstring notEmptyTitle = "Directory not empty";
        // MessageBox(NULL, notEmptyStr.c_str(), notEmptyTitle.c_str(), MB_OK);
    }

    return ERROR_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

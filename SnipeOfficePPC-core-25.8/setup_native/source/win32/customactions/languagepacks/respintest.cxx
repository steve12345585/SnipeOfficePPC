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

#undef UNICODE
#undef _UNICODE

#define _WIN32_WINDOWS 0x0410

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

#include <tchar.h>
#include <string>
#include <sal/macros.h>

#include <systools/win32/uwinapi.h>
#include <../tools/seterror.hxx>

using namespace std;

namespace
{
    string GetMsiProperty(MSIHANDLE handle, const string& sProperty)
    {
        string  result;
        TCHAR   szDummy[1] = TEXT("");
        DWORD   nChars = 0;

        if (MsiGetProperty(handle, sProperty.c_str(), szDummy, &nChars) == ERROR_MORE_DATA)
        {
            DWORD nBytes = ++nChars * sizeof(TCHAR);
            LPTSTR buffer = reinterpret_cast<LPTSTR>(_alloca(nBytes));
            ZeroMemory( buffer, nBytes );
            MsiGetProperty(handle, sProperty.c_str(), buffer, &nChars);
            result = buffer;
        }
        return result;
    }

    inline bool IsSetMsiProperty(MSIHANDLE handle, const string& sProperty)
    {
        return (GetMsiProperty(handle, sProperty).length() > 0);
    }

    inline void UnsetMsiProperty(MSIHANDLE handle, const string& sProperty)
    {
        MsiSetProperty(handle, sProperty.c_str(), NULL);
    }

    inline void SetMsiProperty(MSIHANDLE handle, const string& sProperty, const string&)
    {
        MsiSetProperty(handle, sProperty.c_str(), TEXT("1"));
    }
} // namespace

extern "C" UINT __stdcall GetUserInstallMode(MSIHANDLE handle)
{
    string sOfficeInstallPath = GetMsiProperty(handle, TEXT("INSTALLLOCATION"));

    // MessageBox(NULL, sOfficeInstallPath.c_str(), "DEBUG", MB_OK);

    // unsetting all properties

    UnsetMsiProperty( handle, TEXT("INVALIDDIRECTORY") );
    UnsetMsiProperty( handle, TEXT("ISWRONGPRODUCT") );
    UnsetMsiProperty( handle, TEXT("PATCHISOLDER") );
    UnsetMsiProperty( handle, TEXT("ALLUSERS") );

    // 1. Searching for "ProductCode" in setup.ini

    string sSetupiniPath = sOfficeInstallPath + TEXT("program\\setup.ini");

    TCHAR szValue[32767];

    GetPrivateProfileString(
        TEXT("Bootstrap"),
        TEXT("ProductCode"),
        TEXT("INVALIDDIRECTORY"),
        szValue,
        SAL_N_ELEMENTS(szValue),
        sSetupiniPath.c_str()
        );

    if ( !_tcsicmp( szValue, TEXT("INVALIDDIRECTORY") ) )
    {
        // No setup.ini or no "ProductCode" in setup.ini. This is an invalid directory.
        SetMsiProperty( handle, TEXT("INVALIDDIRECTORY"), TEXT("YES") );
        // MessageBox(NULL, "INVALIDDIRECTORY set, no setup.ini or ProductCode in setup.ini.", "DEBUG", MB_OK);
        SetMsiErrorCode( MSI_ERROR_INVALIDDIRECTORY );
        return ERROR_SUCCESS;
    }

    // 2. Comparing first three characters of "PRODUCTMAJOR" from property table and "buildid" from InfoFile

    szValue[0] = '\0';

    GetPrivateProfileString(
        TEXT("Bootstrap"),
        TEXT("buildid"),
        TEXT("ISWRONGPRODUCT"),
        szValue,
        SAL_N_ELEMENTS(szValue),
        sSetupiniPath.c_str()
        );

    if ( !_tcsicmp( szValue, TEXT("ISWRONGPRODUCT") ) )
    {
        SetMsiProperty( handle, TEXT("ISWRONGPRODUCT"), TEXT("YES") );
        // MessageBox(NULL, "ISWRONGPRODUCT 1 set after searching buildid", "DEBUG", MB_OK);
        SetMsiErrorCode( MSI_ERROR_ISWRONGPRODUCT );
        return ERROR_SUCCESS;
    }

    string ProductMajor = GetMsiProperty(handle, TEXT("PRODUCTMAJOR"));

    // Comparing the first three characters, for example "680"
    // If not equal, this version is not suited for patch or language pack

    if (_tcsnicmp(ProductMajor.c_str(), szValue, 3))
    {
        SetMsiProperty( handle, TEXT("ISWRONGPRODUCT"), TEXT("YES") );
        // MessageBox(NULL, "ISWRONGPRODUCT 2 set after searching PRODUCTMAJOR", "DEBUG", MB_OK);
        SetMsiErrorCode( MSI_ERROR_ISWRONGPRODUCT );
        return ERROR_SUCCESS;
    }

    // 3. Only for patch: Comparing "PRODUCTMINOR from property table and "ProductMinor" from InfoFile

    string isPatch = GetMsiProperty(handle, TEXT("ISPATCH"));

    if (isPatch=="1")
    {
        string ProductMinor = GetMsiProperty(handle, TEXT("PRODUCTBUILDID"));
        int PatchProductMinor = atoi(ProductMinor.c_str());

        szValue[0] = '\0';

        GetPrivateProfileString(
            TEXT("Bootstrap"),
            TEXT("ProductBuildid"),
            TEXT("8918"),
            szValue,
            SAL_N_ELEMENTS(szValue),
            sSetupiniPath.c_str()
            );

        int InstalledProductMinor = atoi(szValue);

        if ( InstalledProductMinor >= PatchProductMinor )
        {
            SetMsiProperty( handle, TEXT("PATCHISOLDER"), TEXT("YES") );
            // MessageBox(NULL, "PATCHISOLDER set", "DEBUG", MB_OK);
            SetMsiErrorCode( MSI_ERROR_PATCHISOLDER );
            return ERROR_SUCCESS;
        }
    }

    // 4. Setting property ALLUSERS with value from "setup.ini"

    szValue[0] = '\0';

    GetPrivateProfileString(
        TEXT("Bootstrap"),
        TEXT("ALLUSERS"),
        TEXT(""),
        szValue,
        SAL_N_ELEMENTS(szValue),
        sSetupiniPath.c_str()
        );

    if ( szValue[0] )
    {
        SetMsiProperty( handle, TEXT("ALLUSERS"), szValue );
        // MessageBox(NULL, "ALLUSERS set", "DEBUG", MB_OK);
    }

    return ERROR_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#define UNICODE

#ifdef _MSC_VER
#pragma warning(push,1) // disable warnings within system headers
#endif
#include <windows.h>
#include <msiquery.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include "strsafe.h"

#include <seterror.hxx>

//----------------------------------------------------------
BOOL GetMsiProp( MSIHANDLE hMSI, const wchar_t* pPropName, wchar_t** ppValue )
{
    DWORD sz = 0;
       if ( MsiGetProperty( hMSI, pPropName, L"", &sz ) == ERROR_MORE_DATA )
       {
           sz++;
           DWORD nbytes = sz * sizeof( wchar_t );
           wchar_t* buff = reinterpret_cast<wchar_t*>( malloc( nbytes ) );
           ZeroMemory( buff, nbytes );
           MsiGetProperty( hMSI, pPropName, buff, &sz );
           *ppValue = buff;

        return TRUE;
    }

    return FALSE;
}

//----------------------------------------------------------
#ifdef DEBUG
inline void OutputDebugStringFormat( LPCTSTR pFormat, ... )
{
    TCHAR    buffer[1024];
    va_list  args;

    va_start( args, pFormat );
    StringCchVPrintf( buffer, sizeof(buffer), pFormat, args );
    OutputDebugString( buffer );
}
#else
static inline void OutputDebugStringFormat( LPCTSTR, ... )
{
}
#endif

//----------------------------------------------------------
extern "C" UINT __stdcall CheckVersions( MSIHANDLE hMSI )
{
    // MessageBox(NULL, L"CheckVersions", L"Information", MB_OK | MB_ICONINFORMATION);

    wchar_t* pVal = NULL;

    if ( GetMsiProp( hMSI, L"NEWPRODUCTS", &pVal ) && pVal )
    {
        OutputDebugStringFormat( TEXT("DEBUG: NEWPRODUCTS found [%s]"), pVal );
        if ( *pVal != 0 )
            SetMsiErrorCode( MSI_ERROR_NEW_VERSION_FOUND );
        free( pVal );
    }
    pVal = NULL;
    if ( GetMsiProp( hMSI, L"SAMEPRODUCTS", &pVal ) && pVal )
    {
        OutputDebugStringFormat( TEXT("DEBUG: SAMEPRODUCTS found [%s]"), pVal );
        if ( *pVal != 0 )
            SetMsiErrorCode( MSI_ERROR_SAME_VERSION_FOUND );
        free( pVal );
    }
    pVal = NULL;
    if ( GetMsiProp( hMSI, L"OLDPRODUCTS", &pVal ) && pVal )
    {
        OutputDebugStringFormat( TEXT("DEBUG: OLDPRODUCTS found [%s]"), pVal );
        if ( *pVal != 0 )
            SetMsiErrorCode( MSI_ERROR_OLD_VERSION_FOUND );
        free( pVal );
    }
    pVal = NULL;
    if ( GetMsiProp( hMSI, L"BETAPRODUCTS", &pVal ) && pVal )
    {
        OutputDebugStringFormat( TEXT("DEBUG: BETAPRODUCTS found [%s]"), pVal );
        if ( *pVal != 0 )
            SetMsiErrorCode( MSI_ERROR_OLD_VERSION_FOUND );
        free( pVal );
    }

    pVal = NULL;
    if ( GetMsiProp( hMSI, L"NEWPRODUCTSPATCH", &pVal ) && pVal )
    {
        OutputDebugStringFormat( TEXT("DEBUG: NEWPRODUCTSPATCH found [%s]"), pVal );
        if ( *pVal != 0 )
            SetMsiErrorCode( MSI_ERROR_NEW_PATCH_FOUND );
        free( pVal );
    }
    pVal = NULL;
    if ( GetMsiProp( hMSI, L"SAMEPRODUCTSPATCH", &pVal ) && pVal )
    {
        OutputDebugStringFormat( TEXT("DEBUG: SAMEPRODUCTSPATCH found [%s]"), pVal );
        if ( *pVal != 0 )
            SetMsiErrorCode( MSI_ERROR_SAME_PATCH_FOUND );
        free( pVal );
    }
    pVal = NULL;
    if ( GetMsiProp( hMSI, L"OLDPRODUCTSPATCH", &pVal ) && pVal )
    {
        OutputDebugStringFormat( TEXT("DEBUG: OLDPRODUCTSPATCH found [%s]"), pVal );
        if ( *pVal != 0 )
            SetMsiErrorCode( MSI_ERROR_OLD_PATCH_FOUND );
        free( pVal );
    }

    return ERROR_SUCCESS;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

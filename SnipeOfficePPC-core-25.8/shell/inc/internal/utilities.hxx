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

#ifndef UTILITIES_HXX_INCLUDED
#define UTILITIES_HXX_INCLUDED

#include <malloc.h>
#if defined _MSC_VER
#pragma warning(push, 1)
#endif
#include <windows.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include "internal/types.hxx"

#include <string>
#define STRSAFE_NO_DEPRECATE
#ifdef __MINGW32__

// Work around lack of strsafe library in mingw-w64, do let their
// strsafe.h provide inlines of StringCchVPrintfA etc, avoid linking
// errors in a debug build.
#ifdef __CRT__NO_INLINE
#undef __CRT__NO_INLINE
#define DID_UNDEFINE__CRT__NO_INLINE
#endif

extern "C" {

#endif

#include <strsafe.h>

#ifdef __MINGW32__
}

#ifdef DID_UNDEFINE__CRT__NO_INLINE
#define __CRT__NO_INLINE
#endif

#endif

//---------------------------------
/** Convert a string to a wstring
    using CP_ACP
*/
std::wstring StringToWString(const std::string& String);

//---------------------------------
/** Convert a wstring to a string
    using CP_ACP
*/
std::string WStringToString(const std::wstring& String);

//---------------------------------
/** Retrieve a string from the
    resources of this module
*/
std::wstring GetResString(int ResId);

//---------------------------------
/** Returns whether we are running
    on Windows XP or not
*/
bool is_windows_xp_or_above();

//---------------------------------
/** helper function to judge if the string is only has spaces.
    @returns
        <TRUE>if the provided string contains only but at least one space
        character else <FALSE/>.
*/
bool HasOnlySpaces(const std::wstring& String);

/** convert LocaleSet pair into Windows LCID identifier.
    @returns
        Windows Locale Identifier corresponding to input LocaleSet.
*/

//---------------------------------
/** Convert a long path name using Windows api call GetShortPathName
*/
std::wstring getShortPathName( const std::wstring& aLongName );


LCID LocaleSetToLCID( const LocaleSet_t & Locale );

//----------------------------------------------------------
#ifdef DEBUG
inline void OutputDebugStringFormat( LPCSTR pFormat, ... )
{
    CHAR    buffer[1024];
    va_list args;

    va_start( args, pFormat );
    StringCchVPrintfA( buffer, sizeof(buffer), pFormat, args );
    OutputDebugStringA( buffer );
}
#else
static inline void OutputDebugStringFormat( LPCSTR, ... )
{
}
#endif
//----------------------------------------------------------


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

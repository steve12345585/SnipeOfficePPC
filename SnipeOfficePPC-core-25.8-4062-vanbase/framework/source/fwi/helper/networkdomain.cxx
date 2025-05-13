/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#include <helper/networkdomain.hxx>

#ifdef WNT
//_________________________________________________________________________________________________________________
//  Windows
//_________________________________________________________________________________________________________________

#define UNICODE
#if defined _MSC_VER
#pragma warning(push, 1)
#endif
#include <windows.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif

static DWORD WINAPI GetUserDomainW_NT( LPWSTR lpBuffer, DWORD nSize )
{
    return GetEnvironmentVariable( TEXT("USERDOMAIN"), lpBuffer, nSize );
}

static rtl::OUString GetUserDomain()
{
    sal_Unicode aBuffer[256];
    DWORD   nResult;

    nResult = GetUserDomainW_NT( reinterpret_cast<LPWSTR>(aBuffer), sizeof( aBuffer ) );

    if ( nResult > 0 )
        return rtl::OUString( aBuffer );
    else
        return rtl::OUString();
}

//_________________________________________________________________________________________________________________
//  Windows
//_________________________________________________________________________________________________________________

namespace framework
{

rtl::OUString NetworkDomain::GetYPDomainName()
{
    return ::rtl::OUString();
}

rtl::OUString NetworkDomain::GetNTDomainName()
{
    return GetUserDomain();
}

}

#elif defined( UNIX )

#include <rtl/ustring.h>
#include <stdlib.h>
#include <errno.h>
#include <osl/thread.h>

//_________________________________________________________________________________________________________________
//  Unix
//_________________________________________________________________________________________________________________

#if defined( SOLARIS )

//_________________________________________________________________________________________________________________
//  Solaris
//_________________________________________________________________________________________________________________

#include <sys/systeminfo.h>
#include <sal/alloca.h>

static rtl_uString *getDomainName()
{
    /* Initialize and assume failure */
    rtl_uString *ustrDomainName = NULL;

    char    szBuffer[256];

    long    nCopied = sizeof(szBuffer);
    char    *pBuffer = szBuffer;
    long    nBufSize;

    do
    {
        nBufSize = nCopied;
        nCopied = sysinfo( SI_SRPC_DOMAIN, pBuffer, nBufSize );

        /*  If nCopied is greater than buffersize we need to allocate
            a buffer with suitable size */

        if ( nCopied > nBufSize )
            pBuffer = (char *)alloca( nCopied );

    } while ( nCopied > nBufSize );

    if ( -1 != nCopied  )
    {
        rtl_string2UString(
            &ustrDomainName,
            pBuffer,
            nCopied - 1,
            osl_getThreadTextEncoding(),
            OSTRING_TO_OUSTRING_CVTFLAGS );
    }

    return ustrDomainName;
}

#elif defined( LINUX ) /* endif SOLARIS */

//_________________________________________________________________________________________________________________
//  Linux
//_________________________________________________________________________________________________________________

#include <unistd.h>
#include <string.h>

static rtl_uString *getDomainName()
{
    /* Initialize and assume failure */
    rtl_uString *ustrDomainName = NULL;

    char    *pBuffer;
    int     result;
    size_t  nBufSize = 0;

    do
    {
        nBufSize += 256; /* Increase buffer size by steps of 256 bytes */
        pBuffer = (char *)alloca( nBufSize );
        result = getdomainname( pBuffer, nBufSize );
        /* If buffersize in not large enough -1 is returned and errno
        is set to EINVAL. This only applies to libc. With glibc the name
        is truncated. */
    } while ( -1 == result && EINVAL == errno );

    if ( 0 == result )
    {
        rtl_string2UString(
            &ustrDomainName,
            pBuffer,
            strlen( pBuffer ),
            osl_getThreadTextEncoding(),
            OSTRING_TO_OUSTRING_CVTFLAGS );
    }

    return ustrDomainName;
}

#else /* LINUX */

//_________________________________________________________________________________________________________________
//  Other Unix
//_________________________________________________________________________________________________________________

static rtl_uString *getDomainName()
{
    return NULL;
}

#endif

//_________________________________________________________________________________________________________________
//  Unix
//_________________________________________________________________________________________________________________

namespace framework
{

rtl::OUString NetworkDomain::GetYPDomainName()
{
    rtl_uString* pResult = getDomainName();
    if ( pResult )
        return rtl::OUString( pResult );
    else
        return rtl::OUString();
}

rtl::OUString NetworkDomain::GetNTDomainName()
{
    return ::rtl::OUString();
}

}

#else /* UNIX */

//_________________________________________________________________________________________________________________
//  Other operating systems (non-Windows and non-Unix)
//_________________________________________________________________________________________________________________

namespace framework
{

rtl::OUString NetworkDomain::GetYPDomainName()
{
    return rtl::OUString();
}

rtl::OUString NetworkDomain::GetNTDomainName()
{
    return rtl::OUString();
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

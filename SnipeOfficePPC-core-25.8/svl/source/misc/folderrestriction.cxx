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


#include <svl/folderrestriction.hxx>
#include "osl/process.h"
#include "tools/urlobj.hxx"
#include "unotools/localfilehelper.hxx"

//-----------------------------------------------------------------------------

static void convertStringListToUrls (
    const rtl::OUString& _rColonSeparatedList, ::std::vector< String >& _rTokens, bool _bFinalSlash )
{
    const sal_Unicode cSeparator =
#if defined(WNT)
        ';'
#else
        ':'
#endif
            ;

    sal_Int32 nIndex = 0;
    do
    {
        // the current token in the list
        rtl::OUString sCurrentToken = _rColonSeparatedList.getToken( 0, cSeparator, nIndex );
        if (!sCurrentToken.isEmpty())
        {
            INetURLObject aCurrentURL;

            rtl::OUString sURL;
            if ( ::utl::LocalFileHelper::ConvertPhysicalNameToURL( sCurrentToken, sURL ) )
                aCurrentURL = INetURLObject( sURL );
            else
            {
                // smart URL parsing, assuming FILE protocol
                aCurrentURL = INetURLObject( sCurrentToken, INET_PROT_FILE );
            }

            if ( _bFinalSlash )
                aCurrentURL.setFinalSlash( );
            else
                aCurrentURL.removeFinalSlash( );
            _rTokens.push_back( aCurrentURL.GetMainURL( INetURLObject::NO_DECODE ) );
        }
    }
    while ( nIndex >= 0 );
}

/** retrieves the value of an environment variable
    @return <TRUE/> if and only if the retrieved string value is not empty
*/
static bool getEnvironmentValue( const sal_Char* _pAsciiEnvName, ::rtl::OUString& _rValue )
{
    _rValue = ::rtl::OUString();
    ::rtl::OUString sEnvName = ::rtl::OUString::createFromAscii( _pAsciiEnvName );
    osl_getEnvironment( sEnvName.pData, &_rValue.pData );
    return !_rValue.isEmpty();
}

//-----------------------------------------------------------------------------

namespace svt
{

    void getUnrestrictedFolders( ::std::vector< String >& _rFolders )
    {
        _rFolders.resize( 0 );
        ::rtl::OUString sRestrictedPathList;
        if ( getEnvironmentValue( "RestrictedPath", sRestrictedPathList ) )
        {
            // append a final slash. This ensures that when we later on check
            // for unrestricted paths, we don't allow paths like "/home/user35" just because
            // "/home/user3" is allowed - with the final slash, we make it "/home/user3/".
            convertStringListToUrls( sRestrictedPathList, _rFolders, true );
        }
    }

} // namespace svt

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

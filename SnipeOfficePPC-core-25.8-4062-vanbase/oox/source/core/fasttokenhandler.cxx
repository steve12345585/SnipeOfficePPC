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

#include "oox/core/fasttokenhandler.hxx"

#include <com/sun/star/uno/XComponentContext.hpp>
#include "oox/helper/helper.hxx"
#include "oox/token/tokenmap.hxx"

namespace oox {
namespace core {

// ============================================================================

using namespace ::com::sun::star::uno;

using ::rtl::OUString;

// ============================================================================

OUString SAL_CALL FastTokenHandler_getImplementationName()
{
    return CREATE_OUSTRING( "com.sun.star.comp.oox.core.FastTokenHandler" );
}

Sequence< OUString > SAL_CALL FastTokenHandler_getSupportedServiceNames()
{
    Sequence< OUString > aServiceNames( 1 );
    aServiceNames[ 0 ] = CREATE_OUSTRING( "com.sun.star.xml.sax.FastTokenHandler" );
    return aServiceNames;
}

Reference< XInterface > SAL_CALL FastTokenHandler_createInstance( const Reference< XComponentContext >& /*rxContext*/ ) throw (Exception)
{
    return static_cast< ::cppu::OWeakObject* >( new FastTokenHandler );
}

// ============================================================================

FastTokenHandler::FastTokenHandler() :
    mrTokenMap( StaticTokenMap::get() )
{
}

FastTokenHandler::~FastTokenHandler()
{
}

// XServiceInfo

OUString SAL_CALL FastTokenHandler::getImplementationName() throw (RuntimeException)
{
    return FastTokenHandler_getImplementationName();
}

sal_Bool SAL_CALL FastTokenHandler::supportsService( const OUString& rServiceName ) throw (RuntimeException)
{
    Sequence< OUString > aServiceNames = FastTokenHandler_getSupportedServiceNames();
    for( sal_Int32 nIndex = 0, nLength = aServiceNames.getLength(); nIndex < nLength; ++nIndex )
        if( aServiceNames[ nIndex ] == rServiceName )
            return sal_True;
    return sal_False;
}

Sequence< OUString > SAL_CALL FastTokenHandler::getSupportedServiceNames() throw (RuntimeException)
{
    return FastTokenHandler_getSupportedServiceNames();
}

// XFastTokenHandler

sal_Int32 FastTokenHandler::getToken( const OUString& rIdentifier ) throw( RuntimeException )
{
    return mrTokenMap.getTokenFromUnicode( rIdentifier );
}

OUString FastTokenHandler::getIdentifier( sal_Int32 nToken ) throw( RuntimeException )
{
    return mrTokenMap.getUnicodeTokenName( nToken );
}

Sequence< sal_Int8 > FastTokenHandler::getUTF8Identifier( sal_Int32 nToken ) throw( RuntimeException )
{
    return mrTokenMap.getUtf8TokenName( nToken );
}

sal_Int32 FastTokenHandler::getTokenFromUTF8( const Sequence< sal_Int8 >& rIdentifier ) throw( RuntimeException )
{
    return mrTokenMap.getTokenFromUtf8( rIdentifier );
}

// ============================================================================

} // namespace core
} // namespace oox

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

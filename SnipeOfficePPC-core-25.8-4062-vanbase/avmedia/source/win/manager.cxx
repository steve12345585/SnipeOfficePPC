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

#include "manager.hxx"
#include "player.hxx"

#include <tools/urlobj.hxx>

#define AVMEDIA_WIN_MANAGER_IMPLEMENTATIONNAME "com.sun.star.comp.avmedia.Manager_DirectX"
#define AVMEDIA_WIN_MANAGER_SERVICENAME "com.sun.star.media.Manager"

using namespace ::com::sun::star;

namespace avmedia { namespace win {
// ----------------
// - Manager -
// ----------------

Manager::Manager( const uno::Reference< lang::XMultiServiceFactory >& rxMgr ) :
    mxMgr( rxMgr )
{
}

// ------------------------------------------------------------------------------

Manager::~Manager()
{
}

// ------------------------------------------------------------------------------

uno::Reference< media::XPlayer > SAL_CALL Manager::createPlayer( const OUString& rURL )
    throw (uno::RuntimeException)
{
    Player*                             pPlayer( new Player( mxMgr ) );
    uno::Reference< media::XPlayer >    xRet( pPlayer );
    const INetURLObject                 aURL( rURL );

    if( !pPlayer->create( aURL.GetMainURL( INetURLObject::DECODE_UNAMBIGUOUS ) )  )
        xRet = uno::Reference< media::XPlayer >();

    return xRet;
}

// ------------------------------------------------------------------------------

OUString SAL_CALL Manager::getImplementationName(  )
    throw (uno::RuntimeException)
{
    return OUString( AVMEDIA_WIN_MANAGER_IMPLEMENTATIONNAME );
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL Manager::supportsService( const OUString& ServiceName )
    throw (uno::RuntimeException)
{
    return ServiceName == AVMEDIA_WIN_MANAGER_SERVICENAME;
}

// ------------------------------------------------------------------------------

uno::Sequence< OUString > SAL_CALL Manager::getSupportedServiceNames(  )
    throw (uno::RuntimeException)
{
    uno::Sequence< OUString > aRet(1);
    aRet[0] = AVMEDIA_WIN_MANAGER_SERVICENAME ;

    return aRet;
}

} // namespace win
} // namespace avmedia

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

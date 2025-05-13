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

#include <com/sun/star/task/XMasterPasswordHandling.hpp>
#include <com/sun/star/ucb/URLAuthenticationRequest.hpp>
#include <ucbhelper/simpleauthenticationrequest.hxx>

using namespace com::sun::star;
using namespace ucbhelper;

//=========================================================================
SimpleAuthenticationRequest::SimpleAuthenticationRequest(
                                      const rtl::OUString & rURL,
                                      const rtl::OUString & rServerName,
                                      const rtl::OUString & rRealm,
                                      const rtl::OUString & rUserName,
                                      const rtl::OUString & rPassword,
                                      const rtl::OUString & rAccount,
                                      sal_Bool bAllowPersistentStoring,
                                      sal_Bool bAllowUseSystemCredentials )
{

    // Fill request...
    ucb::URLAuthenticationRequest aRequest;
//    aRequest.Message        = // OUString
//    aRequest.Context        = // XInterface
    aRequest.Classification = task::InteractionClassification_ERROR;
    aRequest.ServerName     = rServerName;
//    aRequest.Diagnostic     = // OUString
    aRequest.HasRealm       = !rRealm.isEmpty();
    if ( aRequest.HasRealm )
        aRequest.Realm = rRealm;
    aRequest.HasUserName    = sal_True;
    aRequest.UserName       = rUserName;
    aRequest.HasPassword    = sal_True;
    aRequest.Password       = rPassword;
    aRequest.HasAccount     = !rAccount.isEmpty();
    if ( aRequest.HasAccount )
        aRequest.Account = rAccount;
    aRequest.URL = rURL;

    initialize(aRequest,
       sal_False,
       sal_True,
       sal_True,
       aRequest.HasAccount,
       bAllowPersistentStoring,
       bAllowUseSystemCredentials );
}

//=========================================================================
SimpleAuthenticationRequest::SimpleAuthenticationRequest(
                                      const rtl::OUString & rURL,
                                      const rtl::OUString & rServerName,
                                      EntityType eRealmType,
                                      const rtl::OUString & rRealm,
                                      EntityType eUserNameType,
                                      const rtl::OUString & rUserName,
                                      EntityType ePasswordType,
                                      const rtl::OUString & rPassword,
                                      EntityType eAccountType,
                                      const rtl::OUString & rAccount )
{
    // Fill request...
    ucb::URLAuthenticationRequest aRequest;
//    aRequest.Message        = // OUString
//    aRequest.Context        = // XInterface
    aRequest.Classification = task::InteractionClassification_ERROR;
    aRequest.ServerName     = rServerName;
//    aRequest.Diagnostic     = // OUString
    aRequest.HasRealm       = eRealmType != ENTITY_NA;
    if ( aRequest.HasRealm )
        aRequest.Realm = rRealm;
    aRequest.HasUserName    = eUserNameType != ENTITY_NA;
    if ( aRequest.HasUserName )
        aRequest.UserName = rUserName;
    aRequest.HasPassword    = ePasswordType != ENTITY_NA;
    if ( aRequest.HasPassword )
        aRequest.Password = rPassword;
    aRequest.HasAccount     = eAccountType != ENTITY_NA;
    if ( aRequest.HasAccount )
        aRequest.Account = rAccount;
    aRequest.URL = rURL;

    initialize(aRequest,
       eRealmType == ENTITY_MODIFY,
       eUserNameType == ENTITY_MODIFY,
       ePasswordType == ENTITY_MODIFY,
       eAccountType == ENTITY_MODIFY,
       sal_True,
       sal_False );
}

//=========================================================================
void SimpleAuthenticationRequest::initialize(
      const ucb::URLAuthenticationRequest & rRequest,
      sal_Bool bCanSetRealm,
      sal_Bool bCanSetUserName,
      sal_Bool bCanSetPassword,
      sal_Bool bCanSetAccount,
      sal_Bool bAllowPersistentStoring,
      sal_Bool bAllowUseSystemCredentials )
{
    setRequest( uno::makeAny( rRequest ) );

    // Fill continuations...
    uno::Sequence< ucb::RememberAuthentication > aRememberModes(
        bAllowPersistentStoring ? 3 : 2 );
    aRememberModes[ 0 ] = ucb::RememberAuthentication_NO;
    aRememberModes[ 1 ] = ucb::RememberAuthentication_SESSION;
    if (bAllowPersistentStoring)
        aRememberModes[ 2 ] = ucb::RememberAuthentication_PERSISTENT;

    m_xAuthSupplier
        = new InteractionSupplyAuthentication(
                this,
                bCanSetRealm,
                bCanSetUserName,
                bCanSetPassword,
                bCanSetAccount,
                aRememberModes, // rRememberPasswordModes
                ucb::RememberAuthentication_SESSION, // eDefaultRememberPasswordMode
                aRememberModes, // rRememberAccountModes
                ucb::RememberAuthentication_SESSION, // eDefaultRememberAccountMode
                bAllowUseSystemCredentials, // bCanUseSystemCredentials,
                false // bDefaultUseSystemCredentials
            );

    uno::Sequence<
        uno::Reference< task::XInteractionContinuation > > aContinuations( 3 );
    aContinuations[ 0 ] = new InteractionAbort( this );
    aContinuations[ 1 ] = new InteractionRetry( this );
    aContinuations[ 2 ] = m_xAuthSupplier.get();

    setContinuations( aContinuations );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#ifndef _UCBHELPER_SIMPLEAUTHENTICATIONREQUEST_HXX
#define _UCBHELPER_SIMPLEAUTHENTICATIONREQUEST_HXX

#include <rtl/ref.hxx>
#include <ucbhelper/interactionrequest.hxx>
#include "ucbhelper/ucbhelperdllapi.h"

namespace com { namespace sun { namespace star { namespace ucb {
  class URLAuthenticationRequest;
} } } }

namespace ucbhelper {

/**
  * This class implements a simple authentication interaction request.
  * Instances can be passed directly to XInteractionHandler::handle(...). Each
  * instance contains an AuthenticationRequest and three interaction
  * continuations: "Abort", "Retry" and "SupplyAuthentication". The parameters
  * for the AuthenticationRequest and the InteractionSupplyAuthentication
  * objects are partly taken from contructors parameters and partly defaulted
  * as follows:
  *
  * Read-only values : servername, realm
  * Read-write values: username, password, account
  * All remember-authentication values: RememberAuthentication_NO
  *
  * @see com::sun::star::ucb::AuthenticationRequest
  * @see com::sun::star::ucb::RememberAuthentication
  * @see InteractionAbort
  * @see InteractionRetry
  * @see InteractionSupplyAuthentication
  */
class UCBHELPER_DLLPUBLIC SimpleAuthenticationRequest : public ucbhelper::InteractionRequest
{
    rtl::Reference<
        ucbhelper::InteractionSupplyAuthentication > m_xAuthSupplier;

private:
    void initialize( const ::com::sun::star::ucb::URLAuthenticationRequest & rRequest,
                     sal_Bool bCanSetRealm,
                     sal_Bool bCanSetUserName,
                     sal_Bool bCanSetPassword,
                     sal_Bool bCanSetAccount,
                      sal_Bool bAllowPersistentStoring,
                     sal_Bool bAllowUseSystemCredentials );

public:
    /** Specification whether some entity (realm, username, password, account)
        is either not applicable at all, has a fixed value, or is modifiable.
     */
    enum EntityType
    {
        ENTITY_NA,
        ENTITY_FIXED,
        ENTITY_MODIFY
    };

    /**
      * Constructor.
      *
      * @param rURL contains a URL for which authentication is requested.
      * @param rServerName contains a server name.
      * @param rRealm contains a realm, if applicable.
      * @param rUserName contains a username, if available (for instance from
      *        a previous try).
      * @param rPassword contains a password, if available (for instance from
      *        a previous try).
      * @param rAccount contains an account, if applicable.
      * @param bAllowPersistentStoring specifies if the credentials should
      *        be stored in the passowrd container persistently
      * @param bAllowUseSystemCredntials specifies if requesting client is
      *        able to obtain and use system credentials for authentication
      */
    SimpleAuthenticationRequest( const rtl::OUString & rURL,
                                 const rtl::OUString & rServerName,
                                 const rtl::OUString & rRealm,
                                 const rtl::OUString & rUserName,
                                 const rtl::OUString & rPassword,
                                 const rtl::OUString & rAccount,
                                 sal_Bool bAllowPersistentStoring,
                                 sal_Bool bAllowUseSystemCredentials );


    /**
      * Constructor.
      *
      * @param rServerName contains a server name.
      * @param eRealmType specifies whether a realm is applicable and
               modifiable.
      * @param rRealm contains a realm, if applicable.
      * @param eUserNameType specifies whether a username is applicable and
               modifiable.
      * @param rUserName contains a username, if available (for instance from
      *        a previous try).
      * @param ePasswordType specifies whether a password is applicable and
               modifiable.
      * @param rPassword contains a password, if available (for instance from
      *        a previous try).
      * @param eAccountType specifies whether an account is applicable and
               modifiable.
      * @param rAccount contains an account, if applicable.
      */
    SimpleAuthenticationRequest( const rtl::OUString & rURL,
                                 const rtl::OUString & rServerName,
                                 EntityType eRealmType,
                                 const rtl::OUString & rRealm,
                                 EntityType eUserNameType,
                                 const rtl::OUString & rUserName,
                                 EntityType ePasswordType,
                                 const rtl::OUString & rPassword,
                                 EntityType eAccountType = ENTITY_NA,
                                 const rtl::OUString & rAccount
                                    = rtl::OUString() );

    /**
      * This method returns the supplier for the missing authentication data,
      * that, for instance can be used to query the password supplied by the
      * interaction handler.
      *
      * @return the supplier for the missing authentication data.
      */
    const rtl::Reference< ucbhelper::InteractionSupplyAuthentication > &
    getAuthenticationSupplier() const { return m_xAuthSupplier; }
};

} // namespace ucbhelper

#endif /* !_UCBHELPER_SIMPLEAUTHENTICATIONREQUEST_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

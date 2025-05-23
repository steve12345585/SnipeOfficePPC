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

#include <com/sun/star/ucb/CertificateValidationRequest.hpp>
#include <ucbhelper/simplecertificatevalidationrequest.hxx>

using namespace com::sun::star;
using namespace ucbhelper;

//=========================================================================
SimpleCertificateValidationRequest::SimpleCertificateValidationRequest( const sal_Int32 & lCertificateValidity,
                                                                        const com::sun::star::uno::Reference<com::sun::star::security::XCertificate> pCertificate,
                                                                        const rtl::OUString & hostname)
{
    // Fill request...
    ucb::CertificateValidationRequest aRequest;
    aRequest.CertificateValidity = lCertificateValidity;
    aRequest.Certificate = pCertificate;
    aRequest.HostName = hostname;

    setRequest( uno::makeAny( aRequest ) );

    uno::Sequence< uno::Reference< task::XInteractionContinuation > > aContinuations( 2 );
    aContinuations[ 0 ] = new InteractionAbort( this );
    aContinuations[ 1 ] = new InteractionApprove( this );

    setContinuations( aContinuations );
    pCertificate.get();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

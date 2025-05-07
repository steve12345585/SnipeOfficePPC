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

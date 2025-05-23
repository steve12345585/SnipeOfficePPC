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


#include "cppuhelper/factory.hxx"
#include "com/sun/star/lang/IllegalArgumentException.hpp"

#include "cmdenv.hxx"

/**************************************************************************
                                TODO
 **************************************************************************

 *************************************************************************/
using namespace com::sun::star;
using namespace ucb_cmdenv;

//=========================================================================
//=========================================================================
//
// UcbCommandEnvironment Implementation.
//
//=========================================================================
//=========================================================================

UcbCommandEnvironment::UcbCommandEnvironment(
    const uno::Reference< lang::XMultiServiceFactory >& /*xSMgr*/ )
//: m_xSMgr( xSMgr )
{
}

//=========================================================================
// virtual
UcbCommandEnvironment::~UcbCommandEnvironment()
{
}

//=========================================================================
//
// XInitialization methods.
//
//=========================================================================

// virtual
void SAL_CALL UcbCommandEnvironment::initialize(
        const uno::Sequence< uno::Any >& aArguments )
    throw( uno::Exception,
           uno::RuntimeException )
{
    if ( ( aArguments.getLength() < 2 ) ||
         !( aArguments[ 0 ] >>= m_xIH ) ||
         !( aArguments[ 1 ] >>= m_xPH ))
        throw lang::IllegalArgumentException();
}

//=========================================================================
//
// XServiceInfo methods.
//
//=========================================================================

// virtual
::rtl::OUString SAL_CALL UcbCommandEnvironment::getImplementationName()
    throw ( uno::RuntimeException )
{
    return getImplementationName_Static();
}

//=========================================================================
// virtual
sal_Bool SAL_CALL
UcbCommandEnvironment::supportsService( const ::rtl::OUString& ServiceName )
    throw ( uno::RuntimeException )
{
    uno::Sequence< rtl::OUString > aSNL = getSupportedServiceNames();
    const rtl::OUString * pArray = aSNL.getConstArray();
    for ( sal_Int32 i = 0; i < aSNL.getLength(); i++ )
    {
        if ( pArray[ i ] == ServiceName )
            return sal_True;
    }
    return sal_False;
}

//=========================================================================
// virtual
uno::Sequence< ::rtl::OUString > SAL_CALL
UcbCommandEnvironment::getSupportedServiceNames()
    throw ( uno::RuntimeException )
{
    return getSupportedServiceNames_Static();
}

//=========================================================================
// static
rtl::OUString UcbCommandEnvironment::getImplementationName_Static()
{
    return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
        "com.sun.star.comp.ucb.CommandEnvironment" ) );
}

//=========================================================================
// static
uno::Sequence< rtl::OUString >
UcbCommandEnvironment::getSupportedServiceNames_Static()
{
    uno::Sequence< rtl::OUString > aSNS( 1 );
    aSNS.getArray()[ 0 ]
        = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
            "com.sun.star.ucb.CommandEnvironment" ) );
    return aSNS;
}

//=========================================================================
//
// XCommandInfo methods.
//
//=========================================================================

// virtual
uno::Reference< task::XInteractionHandler > SAL_CALL
UcbCommandEnvironment::getInteractionHandler()
    throw ( uno::RuntimeException )
{
    return m_xIH;
}

//=========================================================================
// virtual
uno::Reference< ucb::XProgressHandler > SAL_CALL
UcbCommandEnvironment::getProgressHandler()
    throw ( uno::RuntimeException )
{
    return m_xPH;
}

//=========================================================================
//
// Service factory implementation.
//
//=========================================================================

static uno::Reference< uno::XInterface > SAL_CALL
UcbCommandEnvironment_CreateInstance(
    const uno::Reference< lang::XMultiServiceFactory> & rSMgr )
    throw( uno::Exception )
{
    lang::XServiceInfo * pX = static_cast< lang::XServiceInfo * >(
        new UcbCommandEnvironment( rSMgr ) );
    return uno::Reference< uno::XInterface >::query( pX );
}

//=========================================================================
// static
uno::Reference< lang::XSingleServiceFactory >
UcbCommandEnvironment::createServiceFactory(
    const uno::Reference< lang::XMultiServiceFactory >& rxServiceMgr )
{
    return uno::Reference< lang::XSingleServiceFactory >(
            cppu::createSingleFactory(
                rxServiceMgr,
                UcbCommandEnvironment::getImplementationName_Static(),
                UcbCommandEnvironment_CreateInstance,
                UcbCommandEnvironment::getSupportedServiceNames_Static() ) );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

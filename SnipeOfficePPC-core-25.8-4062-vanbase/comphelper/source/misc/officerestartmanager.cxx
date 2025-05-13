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


#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/awt/XRequestCallback.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include <comphelper_module.hxx>
#include "officerestartmanager.hxx"

using namespace ::com::sun::star;

namespace comphelper
{

// ----------------------------------------------------------
uno::Sequence< ::rtl::OUString > SAL_CALL OOfficeRestartManager::getSupportedServiceNames_static()
{
    uno::Sequence< rtl::OUString > aResult( 1 );
    aResult[0] = getServiceName_static();
    return aResult;
}

// ----------------------------------------------------------
::rtl::OUString SAL_CALL OOfficeRestartManager::getImplementationName_static()
{
    return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.task.OfficeRestartManager" ) );
}

// ----------------------------------------------------------
::rtl::OUString SAL_CALL OOfficeRestartManager::getSingletonName_static()
{
    return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.task.OfficeRestartManager" ) );
}

// ----------------------------------------------------------
::rtl::OUString SAL_CALL OOfficeRestartManager::getServiceName_static()
{
    return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.task.OfficeRestartManager" ) );
}

// ----------------------------------------------------------
uno::Reference< uno::XInterface > SAL_CALL OOfficeRestartManager::Create( const uno::Reference< uno::XComponentContext >& rxContext )
{
    return static_cast< cppu::OWeakObject* >( new OOfficeRestartManager( rxContext ) );
}

// XRestartManager
// ----------------------------------------------------------
void SAL_CALL OOfficeRestartManager::requestRestart( const uno::Reference< task::XInteractionHandler >& /* xInteractionHandler */ )
    throw (uno::Exception, uno::RuntimeException)
{
    if ( !m_xContext.is() )
        throw uno::RuntimeException();

    {
        ::osl::MutexGuard aGuard( m_aMutex );

        // if the restart already running there is no need to trigger it again
        if ( m_bRestartRequested )
            return;

        m_bRestartRequested = sal_True;

        // the office is still not initialized, no need to terminate, changing the state is enough
        if ( !m_bOfficeInitialized )
            return;
    }

    // TODO: use InteractionHandler to report errors
    try
    {
        // register itself as a job that should be executed asynchronously
        uno::Reference< lang::XMultiComponentFactory > xFactory( m_xContext->getServiceManager(), uno::UNO_SET_THROW );

        uno::Reference< awt::XRequestCallback > xRequestCallback(
            xFactory->createInstanceWithContext(
                 ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.awt.AsyncCallback" )),
                 m_xContext ),
             uno::UNO_QUERY_THROW );

        xRequestCallback->addCallback( this, uno::Any() );
    }
    catch ( uno::Exception& )
    {
        // the try to request restart has failed
        m_bRestartRequested = sal_False;
    }
}

// ----------------------------------------------------------
::sal_Bool SAL_CALL OOfficeRestartManager::isRestartRequested( ::sal_Bool bOfficeInitialized )
    throw (uno::Exception, uno::RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );

    if ( bOfficeInitialized && !m_bOfficeInitialized )
        m_bOfficeInitialized = bOfficeInitialized;

    return m_bRestartRequested;
}

// XCallback
// ----------------------------------------------------------
void SAL_CALL OOfficeRestartManager::notify( const uno::Any& /* aData */ )
    throw ( uno::RuntimeException )
{
    try
    {
        sal_Bool bSuccess = sal_False;

        if ( m_xContext.is() )
        {
            uno::Reference< lang::XMultiComponentFactory > xFactory( m_xContext->getServiceManager(), uno::UNO_SET_THROW );
            uno::Reference< frame::XDesktop > xDesktop(
                xFactory->createInstanceWithContext(
                    ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.frame.Desktop" ) ), m_xContext ),
                uno::UNO_QUERY_THROW );

            // Turn Quickstarter veto off
            uno::Reference< beans::XPropertySet > xPropertySet( xDesktop, uno::UNO_QUERY_THROW );
            ::rtl::OUString aVetoPropName( RTL_CONSTASCII_USTRINGPARAM( "SuspendQuickstartVeto" ) );
            uno::Any aValue;
            aValue <<= (sal_Bool)sal_True;
            xPropertySet->setPropertyValue( aVetoPropName, aValue );

            try
            {
                bSuccess = xDesktop->terminate();
            } catch( uno::Exception& )
            {}

            if ( !bSuccess )
            {
                aValue <<= (sal_Bool)sal_False;
                xPropertySet->setPropertyValue( aVetoPropName, aValue );
            }
        }

        if ( !bSuccess )
            m_bRestartRequested = sal_False;
    }
    catch( uno::Exception& )
    {
        // the try to restart has failed
        m_bRestartRequested = sal_False;
    }
}

// XServiceInfo
// ----------------------------------------------------------
::rtl::OUString SAL_CALL OOfficeRestartManager::getImplementationName() throw (uno::RuntimeException)
{
    return getImplementationName_static();
}

// ----------------------------------------------------------
::sal_Bool SAL_CALL OOfficeRestartManager::supportsService( const ::rtl::OUString& aServiceName ) throw (uno::RuntimeException)
{
    const uno::Sequence< rtl::OUString > & aSupportedNames = getSupportedServiceNames_static();
    for ( sal_Int32 nInd = 0; nInd < aSupportedNames.getLength(); nInd++ )
    {
        if ( aSupportedNames[ nInd ].equals( aServiceName ) )
            return sal_True;
    }

    return sal_False;
}

// ----------------------------------------------------------
uno::Sequence< ::rtl::OUString > SAL_CALL OOfficeRestartManager::getSupportedServiceNames() throw (uno::RuntimeException)
{
    return getSupportedServiceNames_static();
}

} // namespace comphelper

void createRegistryInfo_OOfficeRestartManager()
{
    static ::comphelper::module::OAutoRegistration< ::comphelper::OOfficeRestartManager > aAutoRegistration;
    static ::comphelper::module::OSingletonRegistration< ::comphelper::OOfficeRestartManager > aSingletonRegistration;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

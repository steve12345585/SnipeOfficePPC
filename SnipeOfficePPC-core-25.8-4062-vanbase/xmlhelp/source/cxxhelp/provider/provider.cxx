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


/**************************************************************************
                                TODO
 **************************************************************************

 *************************************************************************/

#include <stdio.h>
#include <osl/file.hxx>
#include <osl/diagnose.h>
#include <ucbhelper/contentidentifier.hxx>
#include <com/sun/star/frame/XConfigManager.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/configuration/theDefaultProvider.hpp>
#include <com/sun/star/container/XContainer.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/container/XNameReplace.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <comphelper/processfactory.hxx>
#include <unotools/configmgr.hxx>
#include <rtl/bootstrap.hxx>

#include "databases.hxx"
#include "provider.hxx"
#include "content.hxx"

using namespace com::sun::star;
using namespace chelp;

//=========================================================================
//=========================================================================
//
// ContentProvider Implementation.
//
//=========================================================================
//=========================================================================

ContentProvider::ContentProvider(
    const uno::Reference< uno::XComponentContext >& rxContext )
    : ::ucbhelper::ContentProviderImplHelper( rxContext ),
        isInitialized( false ),
        m_aScheme(MYUCP_URL_SCHEME),
        m_pDatabases( 0 )
{
}

//=========================================================================
// virtual
ContentProvider::~ContentProvider()
{
    delete m_pDatabases;
}

//=========================================================================
//
// XInterface methods.
//
//=========================================================================

XINTERFACE_IMPL_6( ContentProvider,
                   lang::XTypeProvider,
                   lang::XServiceInfo,
                   ucb::XContentProvider,
                   lang::XComponent,
                   lang::XEventListener, /* base of XContainerListener */
                   container::XContainerListener);

//=========================================================================
//
// XTypeProvider methods.
//
//=========================================================================

XTYPEPROVIDER_IMPL_5( ContentProvider,
                         lang::XTypeProvider,
                         lang::XServiceInfo,
                         ucb::XContentProvider,
                      lang::XComponent,
                      container::XContainerListener);

//=========================================================================
//
// XServiceInfo methods.
//
//=========================================================================

rtl::OUString SAL_CALL ContentProvider::getImplementationName()
    throw( uno::RuntimeException )
{
    return getImplementationName_Static();
}

rtl::OUString ContentProvider::getImplementationName_Static()
{
    return rtl::OUString("CHelpContentProvider" );
}

sal_Bool SAL_CALL
ContentProvider::supportsService(const rtl::OUString& ServiceName )
    throw( uno::RuntimeException )
{
    uno::Sequence< rtl::OUString > aSNL = getSupportedServiceNames();
    const rtl::OUString* pArray = aSNL.getArray();
    for( sal_Int32 i = 0; i < aSNL.getLength(); i++ )
    {
        if( pArray[ i ] == ServiceName )
            return sal_True;
    }

    return sal_False;
}

uno::Sequence< rtl::OUString > SAL_CALL
ContentProvider::getSupportedServiceNames()
    throw( uno::RuntimeException )
{
    return getSupportedServiceNames_Static();
}

static uno::Reference< uno::XInterface > SAL_CALL
ContentProvider_CreateInstance(
         const uno::Reference< lang::XMultiServiceFactory> & rSMgr )
    throw( uno::Exception )
{
    lang::XServiceInfo * pX = static_cast< lang::XServiceInfo * >(
        new ContentProvider( comphelper::getComponentContext(rSMgr) ) );
    return uno::Reference< uno::XInterface >::query( pX );
}

uno::Sequence< rtl::OUString >
ContentProvider::getSupportedServiceNames_Static()
{
    uno::Sequence< rtl::OUString > aSNS( 2 );
    aSNS.getArray()[ 0 ] =
        rtl::OUString(
            MYUCP_CONTENT_PROVIDER_SERVICE_NAME1 );
    aSNS.getArray()[ 1 ] =
        rtl::OUString(
            MYUCP_CONTENT_PROVIDER_SERVICE_NAME2 );

    return aSNS;
}

//=========================================================================
//
// Service factory implementation.
//
//=========================================================================

ONE_INSTANCE_SERVICE_FACTORY_IMPL( ContentProvider );

//=========================================================================
//
// XContentProvider methods.
//
//=========================================================================

// virtual
uno::Reference< ucb::XContent > SAL_CALL
ContentProvider::queryContent(
        const uno::Reference< ucb::XContentIdentifier >& xCanonicId )
    throw( ucb::IllegalIdentifierException, uno::RuntimeException )
{
    if ( !xCanonicId->getContentProviderScheme()
             .equalsIgnoreAsciiCase( m_aScheme ) )
    {   // Wrong URL-scheme
        throw ucb::IllegalIdentifierException();
    }

    {
        osl::MutexGuard aGuard( m_aMutex );
        if( !isInitialized )
            init();
    }

    if( !m_pDatabases )
        throw uno::RuntimeException();

    rtl::OUString aOUString( m_pDatabases->getInstallPathAsURL() );
    rtl::OString aOString( aOUString.getStr(),
                           aOUString.getLength(),
                           RTL_TEXTENCODING_UTF8 );

    // Check, if a content with given id already exists...
    uno::Reference< ucb::XContent > xContent
        = queryExistingContent( xCanonicId ).get();
    if ( xContent.is() )
        return xContent;

    xContent = new Content( m_xContext, this, xCanonicId, m_pDatabases );

    // register new content
    registerNewContent( xContent );

    // Further checks

    if ( !xContent->getIdentifier().is() )
        throw ucb::IllegalIdentifierException();

    return xContent;
}

void SAL_CALL
ContentProvider::dispose()
    throw ( uno::RuntimeException)
{
    if(m_xContainer.is())
    {
        m_xContainer->removeContainerListener(this);
        m_xContainer.clear();
    }
}

void SAL_CALL
ContentProvider::elementReplaced(const container::ContainerEvent& Event)
    throw (uno::RuntimeException)
{
    if(!m_pDatabases)
        return;

    rtl::OUString accessor;
    Event.Accessor >>= accessor;
    if(accessor.compareToAscii("HelpStyleSheet"))
        return;

    rtl::OUString replacedElement,element;
    Event.ReplacedElement >>= replacedElement;
    Event.Element >>= element;

    if(replacedElement == element)
        return;

    m_pDatabases->changeCSS(element);
}

void ContentProvider::init()
{
    osl::MutexGuard aGuard( m_aMutex );

    isInitialized = true;
    uno::Reference< lang::XMultiServiceFactory > sProvider(
        getConfiguration() );
    uno::Reference< container::XHierarchicalNameAccess > xHierAccess(
        getHierAccess( sProvider,
                       "org.openoffice.Office.Common" ) );

    rtl::OUString instPath( getKey( xHierAccess,"Path/Current/Help" ) );
    if( instPath.isEmpty() )
        // try to determine path from default
        instPath = rtl::OUString( "$(instpath)/help" );
    // replace anything like $(instpath);
    subst( instPath );

    rtl::OUString stylesheet( getKey( xHierAccess,"Help/HelpStyleSheet" ) );
    try
    {
        // now adding as configuration change listener for the stylesheet
        uno::Reference< container::XNameAccess> xAccess(
            xHierAccess, uno::UNO_QUERY );
        if( xAccess.is() )
        {
            uno::Any aAny =
                xAccess->getByName( rtl::OUString( "Help" ) );
            aAny >>= m_xContainer;
            if( m_xContainer.is() )
                m_xContainer->addContainerListener( this );
        }
    }
    catch( uno::Exception const & )
    {
    }

    xHierAccess = getHierAccess( sProvider, "org.openoffice.Setup" );

    rtl::OUString setupversion(
        getKey( xHierAccess,"Product/ooSetupVersion" ) );
    rtl::OUString setupextension;

    try
    {
        uno::Reference< lang::XMultiServiceFactory > xConfigProvider =
              configuration::theDefaultProvider::get( m_xContext );

        uno::Sequence < uno::Any > lParams(1);
        beans::PropertyValue                       aParam ;
        aParam.Name    = ::rtl::OUString("nodepath");
        aParam.Value <<= ::rtl::OUString("/org.openoffice.Setup/Product");
        lParams[0] = uno::makeAny(aParam);

        // open it
        uno::Reference< uno::XInterface > xCFG( xConfigProvider->createInstanceWithArguments(
                    ::rtl::OUString("com.sun.star.configuration.ConfigurationAccess"),
                    lParams) );

        uno::Reference< container::XNameAccess > xDirectAccess(xCFG, uno::UNO_QUERY);
        uno::Any aRet = xDirectAccess->getByName(::rtl::OUString("ooSetupExtension"));

        aRet >>= setupextension;
    }
    catch ( uno::Exception& )
    {
    }

    rtl::OUString productversion(
        setupversion +
        rtl::OUString( " " ) +
        setupextension );

    uno::Sequence< rtl::OUString > aImagesZipPaths( 2 );
    xHierAccess = getHierAccess( sProvider,  "org.openoffice.Office.Common" );

    rtl::OUString aPath( getKey( xHierAccess, "Path/Current/UserConfig" ) );
    subst( aPath );
    aImagesZipPaths[ 0 ] = aPath;

    aPath = rtl::OUString("$BRAND_BASE_DIR/share/config");
    rtl::Bootstrap::expandMacros(aPath);
    aImagesZipPaths[ 1 ] = aPath;

    sal_Bool showBasic = getBooleanKey(xHierAccess,"Help/ShowBasic");
    m_pDatabases = new Databases( showBasic,
                                  instPath,
                                  aImagesZipPaths,
                                  utl::ConfigManager::getProductName(),
                                  productversion,
                                  stylesheet,
                                  m_xContext );
}

uno::Reference< lang::XMultiServiceFactory >
ContentProvider::getConfiguration() const
{
    uno::Reference< lang::XMultiServiceFactory > xProvider;
    if( m_xContext.is() )
    {
        try
        {
            xProvider = configuration::theDefaultProvider::get( m_xContext );
        }
        catch( const uno::Exception& )
        {
            OSL_ENSURE( xProvider.is(), "cant instantiate configuration" );
        }
    }

    return xProvider;
}

uno::Reference< container::XHierarchicalNameAccess >
ContentProvider::getHierAccess(
    const uno::Reference< lang::XMultiServiceFactory >& sProvider,
    const char* file ) const
{
    uno::Reference< container::XHierarchicalNameAccess > xHierAccess;

    if( sProvider.is() )
    {
        uno::Sequence< uno::Any > seq( 1 );
        rtl::OUString sReaderService(
            rtl::OUString(
                "com.sun.star.configuration.ConfigurationAccess" ) );

        seq[ 0 ] <<= rtl::OUString::createFromAscii( file );

        try
        {
            xHierAccess =
                uno::Reference< container::XHierarchicalNameAccess >(
                    sProvider->createInstanceWithArguments(
                        sReaderService, seq ),
                    uno::UNO_QUERY );
        }
        catch( const uno::Exception& )
        {
        }
    }
    return xHierAccess;
}



rtl::OUString
ContentProvider::getKey(
    const uno::Reference< container::XHierarchicalNameAccess >& xHierAccess,
    const char* key ) const
{
    rtl::OUString instPath;
    if( xHierAccess.is() )
    {
        uno::Any aAny;
        try
        {
            aAny =
                xHierAccess->getByHierarchicalName(
                    rtl::OUString::createFromAscii( key ) );
        }
        catch( const container::NoSuchElementException& )
        {
        }
        aAny >>= instPath;
    }
    return instPath;
}

sal_Bool
ContentProvider::getBooleanKey(
    const uno::Reference< container::XHierarchicalNameAccess >& xHierAccess,
    const char* key ) const
{
  sal_Bool ret = sal_False;
  if( xHierAccess.is() )
  {
      uno::Any aAny;
      try
      {
          aAny =
            xHierAccess->getByHierarchicalName(
                rtl::OUString::createFromAscii( key ) );
      }
      catch( const container::NoSuchElementException& )
      {
      }
      aAny >>= ret;
  }
  return ret;
}

void ContentProvider::subst( rtl::OUString& instpath ) const
{
    uno::Reference< frame::XConfigManager > xCfgMgr;
    if( m_xContext.is() )
    {
        try
        {
            xCfgMgr =
                uno::Reference< frame::XConfigManager >(
                    m_xContext->getServiceManager()->createInstanceWithContext("com.sun.star.config.SpecialConfigManager", m_xContext),
                    uno::UNO_QUERY );
        }
        catch( const uno::Exception&)
        {
            OSL_ENSURE( xCfgMgr.is(),
                        "cant instantiate the special config manager " );
        }
    }

    OSL_ENSURE( xCfgMgr.is(), "specialconfigmanager not found\n" );

    if( xCfgMgr.is() )
        instpath = xCfgMgr->substituteVariables( instpath );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

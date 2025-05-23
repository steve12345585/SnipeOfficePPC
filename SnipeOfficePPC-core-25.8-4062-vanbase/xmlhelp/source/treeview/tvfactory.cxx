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


#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include "tvfactory.hxx"
#include "tvread.hxx"


using namespace treeview;
using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;



TVFactory::TVFactory( const uno::Reference< XMultiServiceFactory >& xMSF )
    : m_xMSF( xMSF )
{
}


TVFactory::~TVFactory()
{
}


//////////////////////////////////////////////////////////////////////////
// XInterface
//////////////////////////////////////////////////////////////////////////

void SAL_CALL
TVFactory::acquire(
    void )
    throw()
{
  OWeakObject::acquire();
}


void SAL_CALL
TVFactory::release(
              void )
  throw()
{
  OWeakObject::release();
}


Any SAL_CALL
TVFactory::queryInterface(
    const Type& rType )
    throw( RuntimeException )
{
    Any aRet = cppu::queryInterface( rType,
                                     (static_cast< XServiceInfo* >(this)),
                                     (static_cast< XTypeProvider* >(this)),
                                     (static_cast< XMultiServiceFactory* >(this)) );

    return aRet.hasValue() ? aRet : OWeakObject::queryInterface( rType );
}


////////////////////////////////////////////////////////////////////////////////
//
// XTypeProvider methods.

XTYPEPROVIDER_IMPL_3( TVFactory,
                         XServiceInfo,
                         XTypeProvider,
                         XMultiServiceFactory );



////////////////////////////////////////////////////////////////////////////////

// XServiceInfo methods.

rtl::OUString SAL_CALL
TVFactory::getImplementationName()
    throw( RuntimeException )
{
    return TVFactory::getImplementationName_static();
}


sal_Bool SAL_CALL
TVFactory::supportsService(
    const rtl::OUString& ServiceName )
    throw( RuntimeException )
{
    return
        ServiceName.compareToAscii( "com.sun.star.help.TreeView" ) == 0 ||
        ServiceName.compareToAscii( "com.sun.star.ucb.HiearchyDataSource" ) == 0;
}


Sequence< rtl::OUString > SAL_CALL
TVFactory::getSupportedServiceNames( void )
    throw( RuntimeException )
{
    return TVFactory::getSupportedServiceNames_static();
}



// XMultiServiceFactory

Reference< XInterface > SAL_CALL
TVFactory::createInstance(
    const rtl::OUString& aServiceSpecifier )
    throw( Exception,
           RuntimeException )
{
    Any aAny;
    aAny <<= rtl::OUString();
    Sequence< Any > seq( 1 );
    seq[0] <<= PropertyValue(
        rtl::OUString( "nodepath" ),
        -1,
        aAny,
        PropertyState_DIRECT_VALUE );

    return createInstanceWithArguments( aServiceSpecifier,
                                        seq );
}


Reference< XInterface > SAL_CALL
TVFactory::createInstanceWithArguments(
    const rtl::OUString& ServiceSpecifier,
    const Sequence< Any >& Arguments )
    throw( Exception,
           RuntimeException )
{
    (void)ServiceSpecifier;

    if( ! m_xHDS.is() )
    {
        cppu::OWeakObject* p = new TVChildTarget( m_xMSF );
        m_xHDS = Reference< XInterface >( p );
    }

    Reference< XInterface > ret = m_xHDS;

    rtl::OUString hierview;
    for( int i = 0; i < Arguments.getLength(); ++i )
    {
        PropertyValue pV;
        if( ! ( Arguments[i] >>= pV ) )
            continue;

        if( pV.Name.compareToAscii( "nodepath" ) )
            continue;

        if( ! ( pV.Value >>= hierview ) )
            continue;

        break;
    }

    if( !hierview.isEmpty() )
    {
        Reference< XHierarchicalNameAccess > xhieraccess( m_xHDS,UNO_QUERY );
        Any aAny = xhieraccess->getByHierarchicalName( hierview );
        Reference< XInterface > xInterface;
        aAny >>= xInterface;
        return xInterface;
    }
    else
        return m_xHDS;
}


Sequence< rtl::OUString > SAL_CALL
TVFactory::getAvailableServiceNames( )
    throw( RuntimeException )
{
    Sequence< rtl::OUString > seq( 1 );
    seq[0] = rtl::OUString( "com.sun.star.ucb.HierarchyDataReadAccess" );
    return seq;
}



// static


rtl::OUString SAL_CALL
TVFactory::getImplementationName_static()
{
    return rtl::OUString( "com.sun.star.help.TreeViewImpl" );
}


Sequence< rtl::OUString > SAL_CALL
TVFactory::getSupportedServiceNames_static()
{
    Sequence< rtl::OUString > seq( 2 );
    seq[0] = rtl::OUString( "com.sun.star.help.TreeView" );
    seq[1] = rtl::OUString( "com.sun.star.ucb.HiearchyDataSource" );
    return seq;
}


Reference< XSingleServiceFactory > SAL_CALL
TVFactory::createServiceFactory(
    const Reference< XMultiServiceFactory >& rxServiceMgr )
{
    return Reference< XSingleServiceFactory > (
        cppu::createSingleFactory(
            rxServiceMgr,
            TVFactory::getImplementationName_static(),
            TVFactory::CreateInstance,
            TVFactory::getSupportedServiceNames_static() ) );
}



Reference< XInterface > SAL_CALL
TVFactory::CreateInstance(
    const Reference< XMultiServiceFactory >& xMultiServiceFactory )
{
    XServiceInfo* xP = (XServiceInfo*) new TVFactory( xMultiServiceFactory );
    return Reference< XInterface >::query( xP );
}

//=========================================================================
extern "C" SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory(
    const sal_Char * pImplName,void * pServiceManager,void * pRegistryKey )
{
    (void)pRegistryKey;

    void * pRet = 0;

    Reference< XMultiServiceFactory > xSMgr(
        reinterpret_cast< XMultiServiceFactory * >( pServiceManager ) );

    Reference< XSingleServiceFactory > xFactory;

    //////////////////////////////////////////////////////////////////////
    // File Content Provider.
    //////////////////////////////////////////////////////////////////////

    if ( TVFactory::getImplementationName_static().compareToAscii( pImplName ) == 0 )
    {
        xFactory = TVFactory::createServiceFactory( xSMgr );
    }

    //////////////////////////////////////////////////////////////////////

    if ( xFactory.is() )
    {
        xFactory->acquire();
        pRet = xFactory.get();
    }

    return pRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

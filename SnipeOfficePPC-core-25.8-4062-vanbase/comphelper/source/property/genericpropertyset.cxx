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

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <cppuhelper/weakagg.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <comphelper/propertysethelper.hxx>
#include <osl/mutex.hxx>
#include <comphelper/genericpropertyset.hxx>
#include <comphelper/propertysetinfo.hxx>
#include <comphelper/stl_types.hxx>
#include <comphelper/servicehelper.hxx>
#include <rtl/uuid.h>

///////////////////////////////////////////////////////////////////////

using namespace ::rtl;
using namespace ::osl;
using namespace ::cppu;
using namespace ::comphelper;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;

DECLARE_STL_USTRINGACCESS_MAP( Any, GenericAnyMapImpl );

namespace comphelper
{
    struct IMPL_GenericPropertySet_MutexContainer
    {
        Mutex maMutex ;
    } ;

    class GenericPropertySet :  public OWeakAggObject,
                                public XServiceInfo,
                                public XTypeProvider,
                                public PropertySetHelper,
                                private IMPL_GenericPropertySet_MutexContainer
    {
    private:
        GenericAnyMapImpl   maAnyMap;
        ::cppu::OMultiTypeInterfaceContainerHelperVar< ::rtl::OUString, ::rtl::OUStringHash,UStringEqual> m_aListener;

    protected:
        virtual void _setPropertyValues( const PropertyMapEntry** ppEntries, const  Any* pValues ) throw( UnknownPropertyException,  PropertyVetoException,  IllegalArgumentException,  WrappedTargetException );
        virtual void _getPropertyValues( const PropertyMapEntry** ppEntries,  Any* pValue ) throw( UnknownPropertyException,  WrappedTargetException );

    public:
        GenericPropertySet( PropertySetInfo* pInfo ) throw();
        virtual ~GenericPropertySet() throw();

        // XInterface
        virtual  Any SAL_CALL queryAggregation( const  Type & rType ) throw( RuntimeException);
        virtual  Any SAL_CALL queryInterface( const  Type & rType ) throw( RuntimeException);
        virtual void SAL_CALL acquire() throw();
        virtual void SAL_CALL release() throw();

        // XTypeProvider
        virtual  Sequence<  Type > SAL_CALL getTypes(  ) throw( RuntimeException);
        virtual  Sequence< sal_Int8 > SAL_CALL getImplementationId(  ) throw( RuntimeException);

        // XServiceInfo
        virtual rtl::OUString SAL_CALL getImplementationName() throw(  RuntimeException );
        virtual sal_Bool SAL_CALL supportsService( const rtl::OUString& ServiceName ) throw(  RuntimeException );
        virtual  Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames() throw(  RuntimeException );

        // XPropertySet
        virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    };

}

///////////////////////////////////////////////////////////////////////

GenericPropertySet::GenericPropertySet( PropertySetInfo* pInfo ) throw()
: PropertySetHelper( pInfo )
,m_aListener(maMutex)
{
}

GenericPropertySet::~GenericPropertySet() throw()
{
}
void SAL_CALL GenericPropertySet::addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const Reference< XPropertyChangeListener >& xListener ) throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    Reference < XPropertySetInfo > xInfo = getPropertySetInfo(  );
    if ( xInfo.is() )
    {
        if ( aPropertyName.isEmpty() )
        {
            Sequence< Property> aSeq = xInfo->getProperties();
            const Property* pIter = aSeq.getConstArray();
            const Property* pEnd  = pIter + aSeq.getLength();
            for( ; pIter != pEnd ; ++pIter)
            {
                m_aListener.addInterface(pIter->Name,xListener);
            }
        }
        else if ( xInfo->hasPropertyByName(aPropertyName) )
            m_aListener.addInterface(aPropertyName,xListener);
        else
            throw UnknownPropertyException( aPropertyName, *this );
    }
}

void SAL_CALL GenericPropertySet::removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const Reference< XPropertyChangeListener >& xListener ) throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    ResettableMutexGuard aGuard( maMutex );
    Reference < XPropertySetInfo > xInfo = getPropertySetInfo(  );
    aGuard.clear();
    if ( xInfo.is() )
    {
        if ( aPropertyName.isEmpty() )
        {
            Sequence< Property> aSeq = xInfo->getProperties();
            const Property* pIter = aSeq.getConstArray();
            const Property* pEnd  = pIter + aSeq.getLength();
            for( ; pIter != pEnd ; ++pIter)
            {
                m_aListener.removeInterface(pIter->Name,xListener);
            }
        }
        else if ( xInfo->hasPropertyByName(aPropertyName) )
            m_aListener.removeInterface(aPropertyName,xListener);
        else
            throw UnknownPropertyException( aPropertyName, *this );
    }
}

void GenericPropertySet::_setPropertyValues( const PropertyMapEntry** ppEntries, const Any* pValues )
    throw(UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException )
{
    ResettableMutexGuard aGuard( maMutex );

    while( *ppEntries )
    {
        const OUString aPropertyName( (*ppEntries)->mpName, (*ppEntries)->mnNameLen, RTL_TEXTENCODING_ASCII_US );
        OInterfaceContainerHelper * pHelper = m_aListener.getContainer(aPropertyName);

        maAnyMap[ aPropertyName ] = *pValues;

        if ( pHelper )
        {
            PropertyChangeEvent aEvt;
            aEvt.PropertyName = aPropertyName;
            aEvt.NewValue = *pValues;
            aGuard.clear();
            pHelper->notifyEach( &XPropertyChangeListener::propertyChange, aEvt );
            aGuard.reset();
        }

        ppEntries++;
        pValues++;
    }
}

void GenericPropertySet::_getPropertyValues( const comphelper::PropertyMapEntry** ppEntries, Any* pValue )
    throw( UnknownPropertyException, WrappedTargetException )
{
    MutexGuard aGuard( maMutex );

    while( *ppEntries )
    {
        const OUString aPropertyName( (*ppEntries)->mpName, (*ppEntries)->mnNameLen, RTL_TEXTENCODING_ASCII_US );
        *pValue = maAnyMap[ aPropertyName ];

        ppEntries++;
        pValue++;
    }
}

// XInterface

Any SAL_CALL GenericPropertySet::queryInterface( const Type & rType )
    throw( RuntimeException )
{
    return OWeakAggObject::queryInterface( rType );
}

Any SAL_CALL GenericPropertySet::queryAggregation( const Type & rType )
    throw(RuntimeException)
{
    Any aAny;

    if( rType == ::getCppuType((const Reference< XServiceInfo >*)0) )
        aAny <<= Reference< XServiceInfo >(this);
    else if( rType == ::getCppuType((const Reference< XTypeProvider >*)0) )
        aAny <<= Reference< XTypeProvider >(this);
    else if( rType == ::getCppuType((const Reference< XPropertySet >*)0) )
        aAny <<= Reference< XPropertySet >(this);
    else if( rType == ::getCppuType((const Reference< XMultiPropertySet >*)0) )
        aAny <<= Reference< XMultiPropertySet >(this);
    else
        aAny <<= OWeakAggObject::queryAggregation( rType );

    return aAny;
}

void SAL_CALL GenericPropertySet::acquire() throw()
{
    OWeakAggObject::acquire();
}

void SAL_CALL GenericPropertySet::release() throw()
{
    OWeakAggObject::release();
}

uno::Sequence< uno::Type > SAL_CALL GenericPropertySet::getTypes()
    throw (uno::RuntimeException)
{
    uno::Sequence< uno::Type > aTypes( 5 );
    uno::Type* pTypes = aTypes.getArray();

    *pTypes++ = ::getCppuType((const uno::Reference< XAggregation>*)0);
    *pTypes++ = ::getCppuType((const uno::Reference< XServiceInfo>*)0);
    *pTypes++ = ::getCppuType((const uno::Reference< XTypeProvider>*)0);
    *pTypes++ = ::getCppuType((const uno::Reference< XPropertySet>*)0);
    *pTypes++ = ::getCppuType((const uno::Reference< XMultiPropertySet>*)0);

    return aTypes;
}

namespace
{
    class theGenericPropertySetImplmentationId : public rtl::Static< UnoTunnelIdInit, theGenericPropertySetImplmentationId > {};
}

uno::Sequence< sal_Int8 > SAL_CALL GenericPropertySet::getImplementationId()
    throw (uno::RuntimeException)
{
    return theGenericPropertySetImplmentationId::get().getSeq();
}

// XServiceInfo

sal_Bool SAL_CALL GenericPropertySet::supportsService( const  OUString& ServiceName ) throw(RuntimeException)
{
    Sequence< OUString > aSNL( getSupportedServiceNames() );
    const OUString * pArray = aSNL.getConstArray();

    for( sal_Int32 i = 0; i < aSNL.getLength(); ++i )
        if( pArray[i] == ServiceName )
            return sal_True;

    return sal_False;
}

OUString SAL_CALL GenericPropertySet::getImplementationName() throw( RuntimeException )
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.comphelper.GenericPropertySet") );
}

Sequence< OUString > SAL_CALL GenericPropertySet::getSupportedServiceNames(  )
    throw( RuntimeException )
{
    Sequence< OUString > aSNS( 1 );
    aSNS.getArray()[0] = OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.beans.XPropertySet" ));
    return aSNS;
}

::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > comphelper::GenericPropertySet_CreateInstance( comphelper::PropertySetInfo* pInfo )
{
    return (XPropertySet*)new GenericPropertySet( pInfo );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

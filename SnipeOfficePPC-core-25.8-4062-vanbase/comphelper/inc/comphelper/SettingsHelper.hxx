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

#ifndef _COMPHELPER_SETTINGSHELPER_HXX_
#define _COMPHELPER_SETTINGSHELPER_HXX_
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase3.hxx>

namespace comphelper
{
    class MasterPropertySet;
    class MasterPropertySetInfo;
    class ChainablePropertySet;
    class ChainablePropertySetInfo;

    typedef  cppu::WeakImplHelper3
    <
        ::com::sun::star::beans::XPropertySet,
        ::com::sun::star::beans::XMultiPropertySet,
        ::com::sun::star::lang::XServiceInfo
    >
    HelperBaseNoState;
    template < class ComphelperBase, class ComphelperBaseInfo > class SettingsHelperNoState :
        public HelperBaseNoState,
        public ComphelperBase
    {
    public:
        SettingsHelperNoState ( ComphelperBaseInfo *pInfo, ::osl::SolarMutex* pMutex = NULL)
        : ComphelperBase ( pInfo, pMutex )
        {}
        virtual ~SettingsHelperNoState () throw( ) {}
        com::sun::star::uno::Any SAL_CALL queryInterface( const com::sun::star::uno::Type& aType ) throw (com::sun::star::uno::RuntimeException)
        { return HelperBaseNoState::queryInterface( aType ); }
        void SAL_CALL acquire(  ) throw ()
        { HelperBaseNoState::acquire( ); }
        void SAL_CALL release(  ) throw ()
        { HelperBaseNoState::release( ); }

        // XPropertySet
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  )
            throw(::com::sun::star::uno::RuntimeException)
        { return ComphelperBase::getPropertySetInfo(); }
        virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue )
            throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
        { ComphelperBase::setPropertyValue ( aPropertyName, aValue ); }
        virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName )
            throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
        { return ComphelperBase::getPropertyValue ( PropertyName ); }
        virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener )
            throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
        { ComphelperBase::addPropertyChangeListener ( aPropertyName, xListener ); }
        virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener )
            throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
        { ComphelperBase::removePropertyChangeListener ( aPropertyName, aListener ); }
        virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener )
            throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
        { ComphelperBase::addVetoableChangeListener ( PropertyName, aListener ); }
        virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener )
            throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
        { ComphelperBase::removeVetoableChangeListener ( PropertyName, aListener ); }

        // XMultiPropertySet
        virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues )
            throw(::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
        { ComphelperBase::setPropertyValues ( aPropertyNames, aValues ); }
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames )
            throw(::com::sun::star::uno::RuntimeException)
        { return ComphelperBase::getPropertyValues ( aPropertyNames ); }
        virtual void SAL_CALL addPropertiesChangeListener( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener )
            throw(::com::sun::star::uno::RuntimeException)
        { ComphelperBase::addPropertiesChangeListener ( aPropertyNames, xListener ); }
        virtual void SAL_CALL removePropertiesChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener )
            throw(::com::sun::star::uno::RuntimeException)
        { ComphelperBase::removePropertiesChangeListener ( xListener ); }
        virtual void SAL_CALL firePropertiesChangeEvent( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener )
            throw(::com::sun::star::uno::RuntimeException)
        { ComphelperBase::firePropertiesChangeEvent ( aPropertyNames, xListener ); }
    };
    typedef comphelper::SettingsHelperNoState
    <
        ::comphelper::MasterPropertySet,
        ::comphelper::MasterPropertySetInfo
    >
    MasterHelperNoState;
    typedef comphelper::SettingsHelperNoState
    <
        ::comphelper::ChainablePropertySet,
        ::comphelper::ChainablePropertySetInfo
    >
    ChainableHelperNoState;
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

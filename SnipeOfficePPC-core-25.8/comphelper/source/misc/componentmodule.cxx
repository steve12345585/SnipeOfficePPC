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

#include <comphelper/componentmodule.hxx>

/** === begin UNO includes === **/
/** === end UNO includes === **/
#include <comphelper/sequence.hxx>
#include <osl/diagnose.h>

#include <vector>

//........................................................................
namespace comphelper
{
//........................................................................

    using namespace ::cppu;
    /** === being UNO using === **/
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::lang::XMultiServiceFactory;
    using ::com::sun::star::registry::XRegistryKey;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::uno::XInterface;
    /** === end UNO using === **/

    typedef ::std::vector< ComponentDescription >   ComponentDescriptions;

    //=========================================================================
    //= OModuleImpl
    //=========================================================================
    /** implementation for <type>OModule</type>. not threadsafe, has to be guarded by it's owner
    */
    class OModuleImpl
    {
    public:
        ComponentDescriptions                           m_aRegisteredComponents;

        OModuleImpl();
        ~OModuleImpl();
    };

    //-------------------------------------------------------------------------
    OModuleImpl::OModuleImpl()
    {
    }

    //-------------------------------------------------------------------------
    OModuleImpl::~OModuleImpl()
    {
    }

    //=========================================================================
    //= OModule
    //=========================================================================
    //-------------------------------------------------------------------------
    OModule::OModule()
        : m_nClients(0)
        , m_pImpl(new OModuleImpl)
    {
    }

    OModule::~OModule()
    {
        delete m_pImpl;
    }

    //-------------------------------------------------------------------------
    void OModule::registerClient( OModule::ClientAccess )
    {
        ::osl::MutexGuard aGuard(m_aMutex);
        if ( 1 == osl_incrementInterlockedCount( &m_nClients ) )
            onFirstClient();
    }

    //-------------------------------------------------------------------------
    void OModule::revokeClient( OModule::ClientAccess )
    {
        ::osl::MutexGuard aGuard(m_aMutex);
        if ( 0 == osl_decrementInterlockedCount( &m_nClients ) )
            onLastClient();
    }

    //--------------------------------------------------------------------------
    void OModule::onFirstClient()
    {
    }

    //--------------------------------------------------------------------------
    void OModule::onLastClient()
    {
    }

    //--------------------------------------------------------------------------
    void OModule::registerImplementation( const ComponentDescription& _rComp )
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        if ( !m_pImpl )
            throw RuntimeException();

        m_pImpl->m_aRegisteredComponents.push_back( _rComp );
    }

    //--------------------------------------------------------------------------
    void OModule::registerImplementation( const ::rtl::OUString& _rImplementationName, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& _rServiceNames,
        ::cppu::ComponentFactoryFunc _pCreateFunction, FactoryInstantiation _pFactoryFunction )
    {
        ComponentDescription aComponent( _rImplementationName, _rServiceNames, ::rtl::OUString(), _pCreateFunction, _pFactoryFunction );
        registerImplementation( aComponent );
    }

    //--------------------------------------------------------------------------
    void* OModule::getComponentFactory( const sal_Char* _pImplementationName )
    {
        Reference< XInterface > xFactory( getComponentFactory(
            ::rtl::OUString::createFromAscii( _pImplementationName ) ) );
        return xFactory.get();
    }

    //--------------------------------------------------------------------------
    Reference< XInterface > OModule::getComponentFactory( const ::rtl::OUString& _rImplementationName )
    {
        Reference< XInterface > xReturn;

        for (   ComponentDescriptions::const_iterator component = m_pImpl->m_aRegisteredComponents.begin();
                component != m_pImpl->m_aRegisteredComponents.end();
                ++component
            )
        {
            if ( component->sImplementationName == _rImplementationName )
            {
                xReturn = component->pFactoryCreationFunc(
                    component->pComponentCreationFunc,
                    component->sImplementationName,
                    component->aSupportedServices,
                    NULL
                );
                if ( xReturn.is() )
                {
                    xReturn->acquire();
                    return xReturn.get();
                }
            }
        }

        return NULL;
    }

//........................................................................
} // namespace comphelper
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

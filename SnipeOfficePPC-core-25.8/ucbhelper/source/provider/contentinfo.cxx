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

/**************************************************************************
                                TODO
 **************************************************************************

 *************************************************************************/
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/ucb/XPropertySetRegistry.hpp>

#include "osl/diagnose.h"
#include "osl/mutex.hxx"
#include <ucbhelper/contenthelper.hxx>
#include <ucbhelper/contentinfo.hxx>

using namespace com::sun::star;

//=========================================================================
//=========================================================================
//
// PropertySetInfo Implementation.
//
//=========================================================================
//=========================================================================

namespace ucbhelper {

PropertySetInfo::PropertySetInfo(
    const uno::Reference< lang::XMultiServiceFactory >& rxSMgr,
    const uno::Reference< com::sun::star::ucb::XCommandEnvironment >& rxEnv,
    ContentImplHelper* pContent )
: m_xSMgr( rxSMgr ),
  m_xEnv( rxEnv ),
  m_pProps( 0 ),
  m_pContent( pContent )
{
}

//=========================================================================
// virtual
PropertySetInfo::~PropertySetInfo()
{
    delete m_pProps;
}

//=========================================================================
//
// XInterface methods.
//
//=========================================================================

XINTERFACE_IMPL_2( PropertySetInfo,
                   lang::XTypeProvider,
                   beans::XPropertySetInfo );

//=========================================================================
//
// XTypeProvider methods.
//
//=========================================================================

XTYPEPROVIDER_IMPL_2( PropertySetInfo,
                          lang::XTypeProvider,
                          beans::XPropertySetInfo );

//=========================================================================
//
// XPropertySetInfo methods.
//
//=========================================================================

// virtual
uno::Sequence< beans::Property > SAL_CALL PropertySetInfo::getProperties()
    throw( uno::RuntimeException )
{
    if ( !m_pProps )
    {
        osl::MutexGuard aGuard( m_aMutex );
        if ( !m_pProps )
        {
            //////////////////////////////////////////////////////////////
            // Get info for core ( native) properties.
            //////////////////////////////////////////////////////////////

            try
            {
                uno::Sequence< beans::Property > aProps
                    = m_pContent->getProperties( m_xEnv );
                m_pProps = new uno::Sequence< beans::Property >( aProps );
            }
            catch ( uno::RuntimeException const & )
            {
                throw;
            }
            catch ( uno::Exception const & )
            {
                m_pProps = new uno::Sequence< beans::Property >( 0 );
            }

            //////////////////////////////////////////////////////////////
            // Get info for additional properties.
            //////////////////////////////////////////////////////////////

            uno::Reference< com::sun::star::ucb::XPersistentPropertySet >
                xSet ( m_pContent->getAdditionalPropertySet( sal_False ) );

            if ( xSet.is() )
            {
                // Get property set info.
                uno::Reference< beans::XPropertySetInfo > xInfo(
                    xSet->getPropertySetInfo() );
                if ( xInfo.is() )
                {
                    const uno::Sequence< beans::Property >& rAddProps
                        = xInfo->getProperties();
                    sal_Int32 nAddProps = rAddProps.getLength();
                    if ( nAddProps > 0 )
                    {
                        sal_Int32 nPos = m_pProps->getLength();
                        m_pProps->realloc( nPos + nAddProps );

                        beans::Property* pProps = m_pProps->getArray();
                        const beans::Property* pAddProps
                            = rAddProps.getConstArray();

                        for ( sal_Int32 n = 0; n < nAddProps; ++n, ++nPos )
                            pProps[ nPos ] = pAddProps[ n ];
                    }
                }
            }
        }
    }
    return *m_pProps;
}

//=========================================================================
// virtual
beans::Property SAL_CALL PropertySetInfo::getPropertyByName(
        const rtl::OUString& aName )
    throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    beans::Property aProp;
    if ( queryProperty( aName, aProp ) )
        return aProp;

    throw beans::UnknownPropertyException();
}

//=========================================================================
// virtual
sal_Bool SAL_CALL PropertySetInfo::hasPropertyByName(
        const rtl::OUString& Name )
    throw( uno::RuntimeException )
{
    beans::Property aProp;
    return queryProperty( Name, aProp );
}

//=========================================================================
//
// Non-Interface methods.
//
//=========================================================================

void PropertySetInfo::reset()
{
    osl::MutexGuard aGuard( m_aMutex );
    delete m_pProps;
    m_pProps = 0;
}

//=========================================================================
sal_Bool PropertySetInfo::queryProperty(
    const rtl::OUString& rName, beans::Property& rProp )
{
    osl::MutexGuard aGuard( m_aMutex );

    getProperties();

    const beans::Property* pProps = m_pProps->getConstArray();
    sal_Int32 nCount = m_pProps->getLength();
    for ( sal_Int32 n = 0; n < nCount; ++n )
    {
        const beans::Property& rCurrProp = pProps[ n ];
        if ( rCurrProp.Name == rName )
        {
            rProp = rCurrProp;
            return sal_True;
        }
    }

    return sal_False;
}

//=========================================================================
//=========================================================================
//
// CommandProcessorInfo Implementation.
//
//=========================================================================
//=========================================================================

CommandProcessorInfo::CommandProcessorInfo(
    const uno::Reference< lang::XMultiServiceFactory >& rxSMgr,
    const uno::Reference< com::sun::star::ucb::XCommandEnvironment >& rxEnv,
    ContentImplHelper* pContent )
: m_xSMgr( rxSMgr ),
  m_xEnv( rxEnv ),
  m_pCommands( 0 ),
  m_pContent( pContent )
{
}

//=========================================================================
// virtual
CommandProcessorInfo::~CommandProcessorInfo()
{
    delete m_pCommands;
}

//=========================================================================
//
// XInterface methods.
//
//=========================================================================

XINTERFACE_IMPL_2( CommandProcessorInfo,
                   lang::XTypeProvider,
                   com::sun::star::ucb::XCommandInfo );

//=========================================================================
//
// XTypeProvider methods.
//
//=========================================================================

XTYPEPROVIDER_IMPL_2( CommandProcessorInfo,
                         lang::XTypeProvider,
                         com::sun::star::ucb::XCommandInfo );

//=========================================================================
//
// XCommandInfo methods.
//
//=========================================================================

// virtual
uno::Sequence< com::sun::star::ucb::CommandInfo > SAL_CALL
CommandProcessorInfo::getCommands()
    throw( uno::RuntimeException )
{
    if ( !m_pCommands )
    {
        osl::MutexGuard aGuard( m_aMutex );
        if ( !m_pCommands )
        {
            //////////////////////////////////////////////////////////////
            // Get info for commands.
            //////////////////////////////////////////////////////////////

            try
            {
                uno::Sequence< com::sun::star::ucb::CommandInfo > aCmds
                    = m_pContent->getCommands( m_xEnv );
                m_pCommands
                    = new uno::Sequence< com::sun::star::ucb::CommandInfo >(
                        aCmds );
            }
            catch ( uno::RuntimeException const & )
            {
                throw;
            }
            catch ( uno::Exception const & )
            {
                m_pCommands
                    = new uno::Sequence< com::sun::star::ucb::CommandInfo >(
                        0 );
            }
        }
    }
    return *m_pCommands;
}

//=========================================================================
// virtual
com::sun::star::ucb::CommandInfo SAL_CALL
CommandProcessorInfo::getCommandInfoByName(
        const rtl::OUString& Name )
    throw( com::sun::star::ucb::UnsupportedCommandException,
           uno::RuntimeException )
{
    com::sun::star::ucb::CommandInfo aInfo;
    if ( queryCommand( Name, aInfo ) )
        return aInfo;

    throw com::sun::star::ucb::UnsupportedCommandException();
}

//=========================================================================
// virtual
com::sun::star::ucb::CommandInfo SAL_CALL
CommandProcessorInfo::getCommandInfoByHandle( sal_Int32 Handle )
    throw( com::sun::star::ucb::UnsupportedCommandException,
           uno::RuntimeException )
{
    com::sun::star::ucb::CommandInfo aInfo;
    if ( queryCommand( Handle, aInfo ) )
        return aInfo;

    throw com::sun::star::ucb::UnsupportedCommandException();
}

//=========================================================================
// virtual
sal_Bool SAL_CALL CommandProcessorInfo::hasCommandByName(
       const rtl::OUString& Name )
    throw( uno::RuntimeException )
{
    com::sun::star::ucb::CommandInfo aInfo;
    return queryCommand( Name, aInfo );
}

//=========================================================================
// virtual
sal_Bool SAL_CALL CommandProcessorInfo::hasCommandByHandle( sal_Int32 Handle )
    throw( uno::RuntimeException )
{
    com::sun::star::ucb::CommandInfo aInfo;
    return queryCommand( Handle, aInfo );
}

//=========================================================================
//
// Non-Interface methods.
//
//=========================================================================

void CommandProcessorInfo::reset()
{
    osl::MutexGuard aGuard( m_aMutex );
    delete m_pCommands;
    m_pCommands = 0;
}


//=========================================================================
sal_Bool CommandProcessorInfo::queryCommand(
    const rtl::OUString& rName,
    com::sun::star::ucb::CommandInfo& rCommand )
{
    osl::MutexGuard aGuard( m_aMutex );

    getCommands();

    const com::sun::star::ucb::CommandInfo* pCommands
        = m_pCommands->getConstArray();
    sal_Int32 nCount = m_pCommands->getLength();
    for ( sal_Int32 n = 0; n < nCount; ++n )
    {
        const com::sun::star::ucb::CommandInfo& rCurrCommand = pCommands[ n ];
        if ( rCurrCommand.Name == rName )
        {
            rCommand = rCurrCommand;
            return sal_True;
        }
    }

    return sal_False;
}

//=========================================================================
sal_Bool CommandProcessorInfo::queryCommand(
    sal_Int32 nHandle,
    com::sun::star::ucb::CommandInfo& rCommand )
{
    osl::MutexGuard aGuard( m_aMutex );

    getCommands();

    const com::sun::star::ucb::CommandInfo* pCommands
        = m_pCommands->getConstArray();
    sal_Int32 nCount = m_pCommands->getLength();
    for ( sal_Int32 n = 0; n < nCount; ++n )
    {
        const com::sun::star::ucb::CommandInfo& rCurrCommand = pCommands[ n ];
        if ( rCurrCommand.Handle == nHandle )
        {
            rCommand = rCurrCommand;
            return sal_True;
        }
    }

    return sal_False;
}

} // namespace ucbhelper

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

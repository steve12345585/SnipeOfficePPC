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


#include "log_module.hxx"
#include "methodguard.hxx"
#include "loghandler.hxx"

#include <com/sun/star/logging/XConsoleHandler.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/logging/LogLevel.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/ucb/AlreadyInitializedException.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/beans/NamedValue.hpp>

#include <comphelper/componentcontext.hxx>

#include <cppuhelper/compbase3.hxx>
#include <cppuhelper/basemutex.hxx>

#include <stdio.h>

//........................................................................
namespace logging
{
//........................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::logging::XConsoleHandler;
    using ::com::sun::star::lang::XServiceInfo;
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::XComponentContext;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::logging::XLogFormatter;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::logging::LogRecord;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::uno::XInterface;
    using ::com::sun::star::lang::XInitialization;
    using ::com::sun::star::ucb::AlreadyInitializedException;
    using ::com::sun::star::lang::IllegalArgumentException;
    using ::com::sun::star::beans::NamedValue;
    /** === end UNO using === **/
    namespace LogLevel = ::com::sun::star::logging::LogLevel;

    //====================================================================
    //= ConsoleHandler - declaration
    //====================================================================
    //--------------------------------------------------------------------
    typedef ::cppu::WeakComponentImplHelper3    <   XConsoleHandler
                                                ,   XServiceInfo
                                                ,   XInitialization
                                                >   ConsoleHandler_Base;
    class ConsoleHandler    :public ::cppu::BaseMutex
                            ,public ConsoleHandler_Base
    {
    private:
        ::comphelper::ComponentContext  m_aContext;
        LogHandlerHelper                m_aHandlerHelper;
        sal_Int32                       m_nThreshold;

    protected:
        ConsoleHandler( const Reference< XComponentContext >& _rxContext );
        virtual ~ConsoleHandler();

        // XConsoleHandler
        virtual ::sal_Int32 SAL_CALL getThreshold() throw (RuntimeException);
        virtual void SAL_CALL setThreshold( ::sal_Int32 _threshold ) throw (RuntimeException);

        // XLogHandler
        virtual ::rtl::OUString SAL_CALL getEncoding() throw (RuntimeException);
        virtual void SAL_CALL setEncoding( const ::rtl::OUString& _encoding ) throw (RuntimeException);
        virtual Reference< XLogFormatter > SAL_CALL getFormatter() throw (RuntimeException);
        virtual void SAL_CALL setFormatter( const Reference< XLogFormatter >& _formatter ) throw (RuntimeException);
        virtual ::sal_Int32 SAL_CALL getLevel() throw (RuntimeException);
        virtual void SAL_CALL setLevel( ::sal_Int32 _level ) throw (RuntimeException);
        virtual void SAL_CALL flush(  ) throw (RuntimeException);
        virtual ::sal_Bool SAL_CALL publish( const LogRecord& Record ) throw (RuntimeException);

        // XInitialization
        virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName() throw(RuntimeException);
        virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& _rServiceName ) throw(RuntimeException);
        virtual Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw(RuntimeException);

        // OComponentHelper
        virtual void SAL_CALL disposing();

    public:
        // XServiceInfo - static version
        static ::rtl::OUString SAL_CALL getImplementationName_static();
        static Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_static();
        static Reference< XInterface > Create( const Reference< XComponentContext >& _rxContext );

    public:
        typedef ComponentMethodGuard< ConsoleHandler > MethodGuard;
        void    enterMethod( MethodGuard::Access );
        void    leaveMethod( MethodGuard::Access );
    };

    //====================================================================
    //= ConsoleHandler - implementation
    //====================================================================
    //--------------------------------------------------------------------
    ConsoleHandler::ConsoleHandler( const Reference< XComponentContext >& _rxContext )
        :ConsoleHandler_Base( m_aMutex )
        ,m_aContext( _rxContext )
        ,m_aHandlerHelper( _rxContext, m_aMutex, rBHelper )
        ,m_nThreshold( LogLevel::SEVERE )
    {
    }

    //--------------------------------------------------------------------
    ConsoleHandler::~ConsoleHandler()
    {
        if ( !rBHelper.bDisposed )
        {
            acquire();
            dispose();
        }
    }

    //--------------------------------------------------------------------
    void SAL_CALL ConsoleHandler::disposing()
    {
        m_aHandlerHelper.setFormatter( NULL );
    }

    //--------------------------------------------------------------------
    void ConsoleHandler::enterMethod( MethodGuard::Access )
    {
        m_aHandlerHelper.enterMethod();
    }

    //--------------------------------------------------------------------
    void ConsoleHandler::leaveMethod( MethodGuard::Access )
    {
        m_aMutex.release();
    }

    //--------------------------------------------------------------------
    ::sal_Int32 SAL_CALL ConsoleHandler::getThreshold() throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        return m_nThreshold;
    }

    //--------------------------------------------------------------------
    void SAL_CALL ConsoleHandler::setThreshold( ::sal_Int32 _threshold ) throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        m_nThreshold = _threshold;
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL ConsoleHandler::getEncoding() throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        ::rtl::OUString sEncoding;
        OSL_VERIFY( m_aHandlerHelper.getEncoding( sEncoding ) );
        return sEncoding;
    }

    //--------------------------------------------------------------------
    void SAL_CALL ConsoleHandler::setEncoding( const ::rtl::OUString& _rEncoding ) throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        OSL_VERIFY( m_aHandlerHelper.setEncoding( _rEncoding ) );
    }

    //--------------------------------------------------------------------
    Reference< XLogFormatter > SAL_CALL ConsoleHandler::getFormatter() throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        return m_aHandlerHelper.getFormatter();
    }

    //--------------------------------------------------------------------
    void SAL_CALL ConsoleHandler::setFormatter( const Reference< XLogFormatter >& _rxFormatter ) throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        m_aHandlerHelper.setFormatter( _rxFormatter );
    }

    //--------------------------------------------------------------------
    ::sal_Int32 SAL_CALL ConsoleHandler::getLevel() throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        return m_aHandlerHelper.getLevel();
    }

    //--------------------------------------------------------------------
    void SAL_CALL ConsoleHandler::setLevel( ::sal_Int32 _nLevel ) throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        m_aHandlerHelper.setLevel( _nLevel );
    }

    //--------------------------------------------------------------------
    void SAL_CALL ConsoleHandler::flush(  ) throw (RuntimeException)
    {
        MethodGuard aGuard( *this );
        fflush( stdout );
        fflush( stderr );
    }

    //--------------------------------------------------------------------
    ::sal_Bool SAL_CALL ConsoleHandler::publish( const LogRecord& _rRecord ) throw (RuntimeException)
    {
        MethodGuard aGuard( *this );

        ::rtl::OString sEntry;
        if ( !m_aHandlerHelper.formatForPublishing( _rRecord, sEntry ) )
            return sal_False;

        if ( _rRecord.Level >= m_nThreshold )
            fprintf( stderr, "%s\n", sEntry.getStr() );
        else
            fprintf( stdout, "%s\n", sEntry.getStr() );

        return sal_True;
    }

    //--------------------------------------------------------------------
    void SAL_CALL ConsoleHandler::initialize( const Sequence< Any >& _rArguments ) throw (Exception, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if ( m_aHandlerHelper.getIsInitialized() )
            throw AlreadyInitializedException();

        if ( _rArguments.getLength() == 0 )
        {   // create() - nothing to init
            m_aHandlerHelper.setIsInitialized();
            return;
        }

        if ( _rArguments.getLength() != 1 )
            throw IllegalArgumentException( ::rtl::OUString(), *this, 1 );

        Sequence< NamedValue > aSettings;
        if ( !( _rArguments[0] >>= aSettings ) )
            throw IllegalArgumentException( ::rtl::OUString(), *this, 1 );

        // createWithSettings( [in] sequence< ::com::sun::star::beans::NamedValue > Settings )
        ::comphelper::NamedValueCollection aTypedSettings( aSettings );
        m_aHandlerHelper.initFromSettings( aTypedSettings );

        aTypedSettings.get_ensureType( "Threshold", m_nThreshold );

        m_aHandlerHelper.setIsInitialized();
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL ConsoleHandler::getImplementationName() throw(RuntimeException)
    {
        return getImplementationName_static();
    }

    //--------------------------------------------------------------------
    ::sal_Bool SAL_CALL ConsoleHandler::supportsService( const ::rtl::OUString& _rServiceName ) throw(RuntimeException)
    {
        const Sequence< ::rtl::OUString > aServiceNames( getSupportedServiceNames() );
        for (   const ::rtl::OUString* pServiceNames = aServiceNames.getConstArray();
                pServiceNames != aServiceNames.getConstArray() + aServiceNames.getLength();
                ++pServiceNames
            )
            if ( _rServiceName == *pServiceNames )
                return sal_True;
        return sal_False;
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL ConsoleHandler::getSupportedServiceNames() throw(RuntimeException)
    {
        return getSupportedServiceNames_static();
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL ConsoleHandler::getImplementationName_static()
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.extensions.ConsoleHandler" ) );
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL ConsoleHandler::getSupportedServiceNames_static()
    {
        Sequence< ::rtl::OUString > aServiceNames(1);
        aServiceNames[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.logging.ConsoleHandler" ) );
        return aServiceNames;
    }

    //--------------------------------------------------------------------
    Reference< XInterface > ConsoleHandler::Create( const Reference< XComponentContext >& _rxContext )
    {
        return *( new ConsoleHandler( _rxContext ) );
    }

    //--------------------------------------------------------------------
    void createRegistryInfo_ConsoleHandler()
    {
        static OAutoRegistration< ConsoleHandler > aAutoRegistration;
    }

//........................................................................
} // namespace logging
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

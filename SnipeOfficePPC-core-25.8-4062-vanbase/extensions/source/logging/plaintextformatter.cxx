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

#include <stdio.h>

#include <com/sun/star/logging/XLogFormatter.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>

#include <comphelper/componentcontext.hxx>

#include <cppuhelper/implbase2.hxx>

#include <rtl/ustrbuf.hxx>

#include <osl/thread.h>

//........................................................................
namespace logging
{
//........................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::logging::XLogFormatter;
    using ::com::sun::star::uno::XComponentContext;
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::lang::XServiceInfo;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::logging::LogRecord;
    using ::com::sun::star::uno::XInterface;
    /** === end UNO using === **/

    //====================================================================
    //= PlainTextFormatter - declaration
    //====================================================================
    typedef ::cppu::WeakImplHelper2 <   XLogFormatter
                                    ,   XServiceInfo
                                    >   PlainTextFormatter_Base;
    class PlainTextFormatter : public PlainTextFormatter_Base
    {
    private:
        ::comphelper::ComponentContext  m_aContext;

    protected:
        PlainTextFormatter( const Reference< XComponentContext >& _rxContext );
        virtual ~PlainTextFormatter();

        // XLogFormatter
        virtual ::rtl::OUString SAL_CALL getHead(  ) throw (RuntimeException);
        virtual ::rtl::OUString SAL_CALL format( const LogRecord& Record ) throw (RuntimeException);
        virtual ::rtl::OUString SAL_CALL getTail(  ) throw (RuntimeException);

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName() throw(RuntimeException);
        virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& _rServiceName ) throw(RuntimeException);
        virtual Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw(RuntimeException);

    public:
        // XServiceInfo - static version
        static ::rtl::OUString SAL_CALL getImplementationName_static();
        static Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_static();
        static Reference< XInterface > Create( const Reference< XComponentContext >& _rxContext );
    };

    //====================================================================
    //= PlainTextFormatter - implementation
    //====================================================================
    //--------------------------------------------------------------------
    PlainTextFormatter::PlainTextFormatter( const Reference< XComponentContext >& _rxContext )
        :m_aContext( _rxContext )
    {
    }

    //--------------------------------------------------------------------
    PlainTextFormatter::~PlainTextFormatter()
    {
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL PlainTextFormatter::getHead(  ) throw (RuntimeException)
    {
        ::rtl::OUStringBuffer aHeader;
        aHeader.appendAscii( "  event no" );                 // column 1: the event number
        aHeader.appendAscii( " " );
        aHeader.appendAscii( "thread  " );                   // column 2: the thread ID
        aHeader.appendAscii( " " );
        aHeader.appendAscii( "date      " );                 // column 3: date
        aHeader.appendAscii( " " );
        aHeader.appendAscii( "time       " );         // column 4: time
        aHeader.appendAscii( " " );
        aHeader.appendAscii( "(class/method:) message" );    // column 5: class/method/message
        aHeader.appendAscii( "\n" );
        return aHeader.makeStringAndClear();
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL PlainTextFormatter::format( const LogRecord& _rRecord ) throw (RuntimeException)
    {
        char buffer[ 30 ];
        const int buffer_size = sizeof( buffer );
        int used = snprintf( buffer, buffer_size, "%10i", (int)_rRecord.SequenceNumber );
        if ( used >= buffer_size || used < 0 )
            buffer[ buffer_size - 1 ] = 0;

        ::rtl::OUStringBuffer aLogEntry;
        aLogEntry.appendAscii( buffer );
        aLogEntry.appendAscii( " " );

        ::rtl::OString sThreadID( ::rtl::OUStringToOString( _rRecord.ThreadID, osl_getThreadTextEncoding() ) );
        snprintf( buffer, buffer_size, "%8s", sThreadID.getStr() );
        aLogEntry.appendAscii( buffer );
        aLogEntry.appendAscii( " " );

        snprintf( buffer, buffer_size, "%04i-%02i-%02i %02i:%02i:%02i.%02i",
            (int)_rRecord.LogTime.Year, (int)_rRecord.LogTime.Month, (int)_rRecord.LogTime.Day,
            (int)_rRecord.LogTime.Hours, (int)_rRecord.LogTime.Minutes, (int)_rRecord.LogTime.Seconds, (int)_rRecord.LogTime.HundredthSeconds );
        aLogEntry.appendAscii( buffer );
        aLogEntry.appendAscii( " " );

        if ( !(_rRecord.SourceClassName.isEmpty() || _rRecord.SourceMethodName.isEmpty()) )
        {
            aLogEntry.append( _rRecord.SourceClassName );
            aLogEntry.appendAscii( "::" );
            aLogEntry.append( _rRecord.SourceMethodName );
            aLogEntry.appendAscii( ": " );
        }

        aLogEntry.append( _rRecord.Message );
        aLogEntry.appendAscii( "\n" );

        return aLogEntry.makeStringAndClear();
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL PlainTextFormatter::getTail(  ) throw (RuntimeException)
    {
        // no tail
        return ::rtl::OUString();
    }

    //--------------------------------------------------------------------
    ::sal_Bool SAL_CALL PlainTextFormatter::supportsService( const ::rtl::OUString& _rServiceName ) throw(RuntimeException)
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
    ::rtl::OUString SAL_CALL PlainTextFormatter::getImplementationName() throw(RuntimeException)
    {
        return getImplementationName_static();
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL PlainTextFormatter::getSupportedServiceNames() throw(RuntimeException)
    {
        return getSupportedServiceNames_static();
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL PlainTextFormatter::getImplementationName_static()
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.extensions.PlainTextFormatter" ) );
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL PlainTextFormatter::getSupportedServiceNames_static()
    {
        Sequence< ::rtl::OUString > aServiceNames(1);
        aServiceNames[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.logging.PlainTextFormatter" ) );
        return aServiceNames;
    }

    //--------------------------------------------------------------------
    Reference< XInterface > PlainTextFormatter::Create( const Reference< XComponentContext >& _rxContext )
    {
        return *( new PlainTextFormatter( _rxContext ) );
    }

    //--------------------------------------------------------------------
    void createRegistryInfo_PlainTextFormatter()
    {
        static OAutoRegistration< PlainTextFormatter > aAutoRegistration;
    }

//........................................................................
} // namespace logging
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

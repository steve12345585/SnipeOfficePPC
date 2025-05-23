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


#include "loghandler.hxx"

#include <com/sun/star/logging/LogLevel.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/logging/PlainTextFormatter.hpp>

#include <tools/diagnose_ex.h>
#include <comphelper/componentcontext.hxx>
#include <rtl/tencinfo.h>

//........................................................................
namespace logging
{
//........................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::XComponentContext;
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::logging::LogRecord;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::logging::XLogFormatter;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::lang::IllegalArgumentException;
    using ::com::sun::star::lang::DisposedException;
    using ::com::sun::star::logging::PlainTextFormatter;
    /** === end UNO using === **/
    namespace LogLevel = ::com::sun::star::logging::LogLevel;

    //====================================================================
    //= LogHandlerHelper
    //====================================================================
    //--------------------------------------------------------------------
    LogHandlerHelper::LogHandlerHelper( const Reference< XComponentContext >& _rxContext, ::osl::Mutex& _rMutex, ::cppu::OBroadcastHelper& _rBHelper )
        :m_eEncoding( RTL_TEXTENCODING_UTF8 )
        ,m_nLevel( LogLevel::SEVERE )
        ,m_xFormatter( NULL )
        ,m_xContext( _rxContext )
        ,m_rMutex( _rMutex )
        ,m_rBHelper( _rBHelper )
        ,m_bInitialized( false )
    {
    }

    //--------------------------------------------------------------------
    void LogHandlerHelper::initFromSettings( const ::comphelper::NamedValueCollection& _rSettings )
    {
        ::rtl::OUString sEncoding;
        if ( _rSettings.get_ensureType( "Encoding", sEncoding ) )
        {
            if ( !setEncoding( sEncoding ) )
                throw IllegalArgumentException();
        }

        _rSettings.get_ensureType( "Formatter", m_xFormatter );
        _rSettings.get_ensureType( "Level", m_nLevel );
    }

    //--------------------------------------------------------------------
    void LogHandlerHelper::enterMethod()
    {
        m_rMutex.acquire();

        if ( !getIsInitialized() )
            throw DisposedException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "component not initialized" ) ), NULL );

        if ( m_rBHelper.bDisposed )
            throw DisposedException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "component already disposed" ) ), NULL );

        // fallback settings, in case they weren't passed at construction time
        if ( !getFormatter().is() )
        {
            try
            {
                Reference< XLogFormatter > xFormatter( PlainTextFormatter::create( m_xContext ), UNO_QUERY_THROW );
                setFormatter( xFormatter );
            }
            catch( const Exception& )
            {
                DBG_UNHANDLED_EXCEPTION();
            }
        }
    }

    //--------------------------------------------------------------------
    bool LogHandlerHelper::getEncoding( ::rtl::OUString& _out_rEncoding ) const
    {
        const char* pMimeCharset = rtl_getMimeCharsetFromTextEncoding( m_eEncoding );
        if ( pMimeCharset )
        {
            _out_rEncoding = ::rtl::OUString::createFromAscii( pMimeCharset );
            return true;
        }
        _out_rEncoding = ::rtl::OUString();
        return false;
    }

    //--------------------------------------------------------------------
    bool LogHandlerHelper::setEncoding( const ::rtl::OUString& _rEncoding )
    {
        ::rtl::OString sAsciiEncoding( ::rtl::OUStringToOString( _rEncoding, RTL_TEXTENCODING_ASCII_US ) );
        rtl_TextEncoding eEncoding = rtl_getTextEncodingFromMimeCharset( sAsciiEncoding.getStr() );
        if ( eEncoding != RTL_TEXTENCODING_DONTKNOW )
        {
            m_eEncoding = eEncoding;
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------
    bool LogHandlerHelper::formatForPublishing( const LogRecord& _rRecord, ::rtl::OString& _out_rEntry ) const
    {
        if ( _rRecord.Level < getLevel() )
            // not to be published due to low level
            return false;

        try
        {
            Reference< XLogFormatter > xFormatter( getFormatter(), UNO_QUERY_THROW );
            ::rtl::OUString sEntry( xFormatter->format( _rRecord ) );
            _out_rEntry = ::rtl::OUStringToOString( sEntry, getTextEncoding() );
            return true;
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        return false;
    }

    //--------------------------------------------------------------------
    bool LogHandlerHelper::getEncodedHead( ::rtl::OString& _out_rHead ) const
    {
        try
        {
            Reference< XLogFormatter > xFormatter( getFormatter(), UNO_QUERY_THROW );
            ::rtl::OUString sHead( xFormatter->getHead() );
            _out_rHead = ::rtl::OUStringToOString( sHead, getTextEncoding() );
            return true;
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        return false;
    }

    //--------------------------------------------------------------------
    bool LogHandlerHelper::getEncodedTail( ::rtl::OString& _out_rTail ) const
    {
        try
        {
            Reference< XLogFormatter > xFormatter( getFormatter(), UNO_QUERY_THROW );
            ::rtl::OUString sTail( xFormatter->getTail() );
            _out_rTail = ::rtl::OUStringToOString( sTail, getTextEncoding() );
            return true;
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        return false;
    }

//........................................................................
} // namespace logging
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

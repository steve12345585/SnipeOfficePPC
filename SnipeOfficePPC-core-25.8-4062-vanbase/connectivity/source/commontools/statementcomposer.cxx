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

#include <connectivity/statementcomposer.hxx>

#include <connectivity/dbtools.hxx>

#include <com/sun/star/sdb/CommandType.hpp>
#include <com/sun/star/lang/NullPointerException.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/sdb/XQueriesSupplier.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

#include <unotools/sharedunocomponent.hxx>
#include <tools/diagnose_ex.h>
#include <comphelper/property.hxx>

//........................................................................
namespace dbtools
{
//........................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::sdbc::XConnection;
    using ::com::sun::star::sdb::XSingleSelectQueryComposer;
    using ::com::sun::star::lang::NullPointerException;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::lang::XComponent;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::sdb::XQueriesSupplier;
    using ::com::sun::star::container::XNameAccess;
    using ::com::sun::star::uno::UNO_QUERY;
    using ::com::sun::star::beans::XPropertySet;
    using ::com::sun::star::lang::XMultiServiceFactory;
    using ::com::sun::star::sdbc::SQLException;
    /** === end UNO using === **/
    namespace CommandType = ::com::sun::star::sdb::CommandType;

    //====================================================================
    //= StatementComposer_Data
    //====================================================================
    struct StatementComposer_Data
    {
        const Reference< XConnection >          xConnection;
        Reference< XSingleSelectQueryComposer > xComposer;
        ::rtl::OUString                         sCommand;
        ::rtl::OUString                         sFilter;
        ::rtl::OUString                         sOrder;
        sal_Int32                               nCommandType;
        sal_Bool                                bEscapeProcessing;
        bool                                    bComposerDirty;
        bool                                    bDisposeComposer;

        StatementComposer_Data( const Reference< XConnection >& _rxConnection )
            :xConnection( _rxConnection )
            ,sCommand()
            ,sFilter()
            ,sOrder()
            ,nCommandType( CommandType::COMMAND )
            ,bEscapeProcessing( sal_True )
            ,bComposerDirty( true )
            ,bDisposeComposer( true )
        {
            if ( !_rxConnection.is() )
                throw NullPointerException();
        }
    };

    //--------------------------------------------------------------------
    namespace
    {
        //----------------------------------------------------------------
        void    lcl_resetComposer( StatementComposer_Data& _rData )
        {
            if ( _rData.bDisposeComposer && _rData.xComposer.is() )
            {
                try
                {
                    Reference< XComponent > xComposerComponent( _rData.xComposer, UNO_QUERY_THROW );
                    xComposerComponent->dispose();
                }
                catch( const Exception& )
                {
                    DBG_UNHANDLED_EXCEPTION();
                }
            }
            _rData.xComposer.clear();
        }

        //----------------------------------------------------------------
        bool    lcl_ensureUpToDateComposer_nothrow( StatementComposer_Data& _rData )
        {
            if ( !_rData.bComposerDirty )
                return _rData.xComposer.is();
            lcl_resetComposer( _rData );

            try
            {
                ::rtl::OUString sStatement;
                switch ( _rData.nCommandType )
                {
                    case CommandType::COMMAND:
                        if ( _rData.bEscapeProcessing )
                            sStatement = _rData.sCommand;
                        // (in case of no escape processing  we assume a not parseable statement)
                        break;

                    case CommandType::TABLE:
                    {
                        if ( _rData.sCommand.isEmpty() )
                            break;

                        sStatement = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SELECT * FROM " ) );

                        ::rtl::OUString sCatalog, sSchema, sTable;
                        qualifiedNameComponents( _rData.xConnection->getMetaData(), _rData.sCommand, sCatalog, sSchema, sTable, eInDataManipulation );

                        sStatement += composeTableNameForSelect( _rData.xConnection, sCatalog, sSchema, sTable );
                    }
                    break;

                    case CommandType::QUERY:
                    {
                        // ask the connection for the query
                        Reference< XQueriesSupplier > xSupplyQueries( _rData.xConnection, UNO_QUERY_THROW );
                        Reference< XNameAccess >      xQueries( xSupplyQueries->getQueries(), UNO_QUERY_THROW );

                        if ( !xQueries->hasByName( _rData.sCommand ) )
                            break;

                        Reference< XPropertySet > xQuery( xQueries->getByName( _rData.sCommand ), UNO_QUERY_THROW );

                        //  a native query ?
                        sal_Bool bQueryEscapeProcessing = sal_False;
                        xQuery->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "EscapeProcessing" ) ) ) >>= bQueryEscapeProcessing;
                        if ( !bQueryEscapeProcessing )
                            break;

                        // the command used by the query
                        xQuery->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Command" ) ) ) >>= sStatement;
                        if ( sStatement.isEmpty() )
                            break;

                        // use a composer to build a statement from the query filter/order props
                        Reference< XMultiServiceFactory > xFactory( _rData.xConnection, UNO_QUERY_THROW );
                        ::utl::SharedUNOComponent< XSingleSelectQueryComposer > xComposer;
                        xComposer.set(
                            xFactory->createInstance( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sdb.SingleSelectQueryComposer" ) ) ),
                            UNO_QUERY_THROW
                        );

                        // the "basic" statement
                        xComposer->setElementaryQuery( sStatement );

                        // the sort order
                        const ::rtl::OUString sPropOrder( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Order" ) ) );
                        if ( ::comphelper::hasProperty( sPropOrder, xQuery ) )
                        {
                            ::rtl::OUString sOrder;
                            OSL_VERIFY( xQuery->getPropertyValue( sPropOrder ) >>= sOrder );
                            xComposer->setOrder( sOrder );
                        }

                        // the filter
                        sal_Bool bApplyFilter = sal_True;
                        const ::rtl::OUString sPropApply( RTL_CONSTASCII_USTRINGPARAM( "ApplyFilter" ));
                        if ( ::comphelper::hasProperty( sPropApply, xQuery ) )
                        {
                            OSL_VERIFY( xQuery->getPropertyValue( sPropApply ) >>= bApplyFilter );
                        }

                        if ( bApplyFilter )
                        {
                            ::rtl::OUString sFilter;
                            OSL_VERIFY( xQuery->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Filter" ) ) ) >>= sFilter );
                            xComposer->setFilter( sFilter );
                        }

                        // the composed statement
                        sStatement = xComposer->getQuery();
                    }
                    break;

                    default:
                        OSL_FAIL("lcl_ensureUpToDateComposer_nothrow: no table, no query, no statement - what else ?!");
                        break;
                }

                if ( !sStatement.isEmpty() )
                {
                    // create an composer
                    Reference< XMultiServiceFactory > xFactory( _rData.xConnection, UNO_QUERY_THROW );
                    Reference< XSingleSelectQueryComposer > xComposer( xFactory->createInstance( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sdb.SingleSelectQueryComposer" ) ) ),
                        UNO_QUERY_THROW );
                    xComposer->setElementaryQuery( sStatement );

                    // append sort/filter
                    xComposer->setOrder( _rData.sOrder );
                    xComposer->setFilter( _rData.sFilter );

                    sStatement = xComposer->getQuery();

                    _rData.xComposer = xComposer;
                    _rData.bComposerDirty = false;
                }
            }
            catch( const SQLException& )
            {
                // allowed to leave here
            }
            catch( const Exception& )
            {
                DBG_UNHANDLED_EXCEPTION();
            }

            return _rData.xComposer.is();
        }
    }

    //====================================================================
    //= StatementComposer
    //====================================================================
    //--------------------------------------------------------------------
    StatementComposer::StatementComposer( const Reference< XConnection >& _rxConnection,
        const ::rtl::OUString&  _rCommand, const sal_Int32 _nCommandType, const sal_Bool _bEscapeProcessing )
        :m_pData( new StatementComposer_Data( _rxConnection ) )
    {
        OSL_PRECOND( _rxConnection.is(), "StatementComposer::StatementComposer: illegal connection!" );
        m_pData->sCommand = _rCommand;
        m_pData->nCommandType = _nCommandType;
        m_pData->bEscapeProcessing = _bEscapeProcessing;
    }

    //--------------------------------------------------------------------
    StatementComposer::~StatementComposer()
    {
        lcl_resetComposer( *m_pData );
    }

    //--------------------------------------------------------------------
    void StatementComposer::setDisposeComposer( bool _bDoDispose )
    {
        m_pData->bDisposeComposer = _bDoDispose;
    }

    //--------------------------------------------------------------------
    void StatementComposer::setFilter( const ::rtl::OUString& _rFilter )
    {
        m_pData->sFilter = _rFilter;
        m_pData->bComposerDirty = true;
    }

    //--------------------------------------------------------------------
    void StatementComposer::setOrder( const ::rtl::OUString& _rOrder )
    {
        m_pData->sOrder = _rOrder;
        m_pData->bComposerDirty = true;
    }

    //--------------------------------------------------------------------
    Reference< XSingleSelectQueryComposer > StatementComposer::getComposer()
    {
        lcl_ensureUpToDateComposer_nothrow( *m_pData );
        return m_pData->xComposer;
    }

    //--------------------------------------------------------------------
    ::rtl::OUString StatementComposer::getQuery()
    {
        if ( lcl_ensureUpToDateComposer_nothrow( *m_pData ) )
        {
            return m_pData->xComposer->getQuery();
        }

        return ::rtl::OUString();
    }

//........................................................................
} // namespace dbtools
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

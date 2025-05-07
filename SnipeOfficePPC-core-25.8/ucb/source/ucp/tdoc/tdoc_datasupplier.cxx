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

#include <vector>

#include "osl/diagnose.h"
#include "ucbhelper/contentidentifier.hxx"

#include "tdoc_datasupplier.hxx"
#include "tdoc_content.hxx"

using namespace com::sun::star;
using namespace tdoc_ucp;

namespace tdoc_ucp
{

//=========================================================================
//
// struct ResultListEntry.
//
//=========================================================================

struct ResultListEntry
{
    rtl::OUString                             aURL;
    uno::Reference< ucb::XContentIdentifier > xId;
    uno::Reference< ucb::XContent >           xContent;
    uno::Reference< sdbc::XRow >              xRow;

    ResultListEntry( const rtl::OUString& rURL ) : aURL( rURL ) {}
};

//=========================================================================
//
// ResultList.
//
//=========================================================================

typedef std::vector< ResultListEntry* > ResultList;

//=========================================================================
//
// struct DataSupplier_Impl.
//
//=========================================================================

struct DataSupplier_Impl
{
    osl::Mutex                                   m_aMutex;
    ResultList                                   m_aResults;
    rtl::Reference< Content >                    m_xContent;
    uno::Reference< lang::XMultiServiceFactory > m_xSMgr;
    uno::Sequence< rtl::OUString > *             m_pNamesOfChildren;
    sal_Int32                                    m_nOpenMode;
    bool                                         m_bCountFinal;
    bool                                         m_bThrowException;

    DataSupplier_Impl(
            const uno::Reference< lang::XMultiServiceFactory >& rxSMgr,
            const rtl::Reference< Content >& rContent,
            sal_Int32 nOpenMode )
    : m_xContent( rContent ), m_xSMgr( rxSMgr ),
      m_pNamesOfChildren( 0 ), m_nOpenMode( nOpenMode ),
      m_bCountFinal( false ), m_bThrowException( false )
    {}
    ~DataSupplier_Impl();
};

//=========================================================================
DataSupplier_Impl::~DataSupplier_Impl()
{
    ResultList::const_iterator it  = m_aResults.begin();
    ResultList::const_iterator end = m_aResults.end();

    while ( it != end )
    {
        delete (*it);
        ++it;
    }

    delete m_pNamesOfChildren;
}

}

//=========================================================================
//=========================================================================
//
// DataSupplier Implementation.
//
//=========================================================================
//=========================================================================

ResultSetDataSupplier::ResultSetDataSupplier(
                const uno::Reference< lang::XMultiServiceFactory >& rxSMgr,
                const rtl::Reference< Content >& rContent,
                sal_Int32 nOpenMode )
: m_pImpl( new DataSupplier_Impl( rxSMgr, rContent, nOpenMode ) )
{
}

//=========================================================================
// virtual
ResultSetDataSupplier::~ResultSetDataSupplier()
{
    delete m_pImpl;
}

//=========================================================================
// virtual
rtl::OUString
ResultSetDataSupplier::queryContentIdentifierString( sal_uInt32 nIndex )
{
    osl::Guard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( nIndex < m_pImpl->m_aResults.size() )
    {
        rtl::OUString aId = m_pImpl->m_aResults[ nIndex ]->aURL;
        if ( !aId.isEmpty() )
        {
            // Already cached.
            return aId;
        }
    }

    if ( getResult( nIndex ) )
    {
        // Note: getResult fills m_pImpl->m_aResults[ nIndex ]->aURL.
        return m_pImpl->m_aResults[ nIndex ]->aURL;
    }
    return rtl::OUString();
}

//=========================================================================
// virtual
uno::Reference< ucb::XContentIdentifier >
ResultSetDataSupplier::queryContentIdentifier( sal_uInt32 nIndex )
{
    osl::Guard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( nIndex < m_pImpl->m_aResults.size() )
    {
        uno::Reference< ucb::XContentIdentifier > xId
                                = m_pImpl->m_aResults[ nIndex ]->xId;
        if ( xId.is() )
        {
            // Already cached.
            return xId;
        }
    }

    rtl::OUString aId = queryContentIdentifierString( nIndex );
    if ( !aId.isEmpty() )
    {
        uno::Reference< ucb::XContentIdentifier > xId
            = new ::ucbhelper::ContentIdentifier( aId );
        m_pImpl->m_aResults[ nIndex ]->xId = xId;
        return xId;
    }
    return uno::Reference< ucb::XContentIdentifier >();
}

//=========================================================================
// virtual
uno::Reference< ucb::XContent >
ResultSetDataSupplier::queryContent( sal_uInt32 nIndex )
{
    osl::Guard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( nIndex < m_pImpl->m_aResults.size() )
    {
        uno::Reference< ucb::XContent > xContent
                                = m_pImpl->m_aResults[ nIndex ]->xContent;
        if ( xContent.is() )
        {
            // Already cached.
            return xContent;
        }
    }

    uno::Reference< ucb::XContentIdentifier > xId
        = queryContentIdentifier( nIndex );
    if ( xId.is() )
    {
        try
        {
            uno::Reference< ucb::XContent > xContent
                = m_pImpl->m_xContent->getProvider()->queryContent( xId );
            m_pImpl->m_aResults[ nIndex ]->xContent = xContent;
            return xContent;

        }
        catch ( ucb::IllegalIdentifierException const & )
        {
        }
    }
    return uno::Reference< ucb::XContent >();
}

//=========================================================================
// virtual
sal_Bool ResultSetDataSupplier::getResult( sal_uInt32 nIndex )
{
    osl::ClearableGuard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( m_pImpl->m_aResults.size() > nIndex )
    {
        // Result already present.
        return sal_True;
    }

    // Result not (yet) present.

    if ( m_pImpl->m_bCountFinal )
        return sal_False;

    // Try to obtain result...

    sal_uInt32 nOldCount = m_pImpl->m_aResults.size();
    bool bFound = false;

    if ( queryNamesOfChildren() )
    {
        for ( sal_uInt32 n = nOldCount;
              n < sal::static_int_cast<sal_uInt32>(
                      m_pImpl->m_pNamesOfChildren->getLength());
              ++n )
        {
            const rtl::OUString & rName
                = m_pImpl->m_pNamesOfChildren->getConstArray()[ n ];

            if ( rName.isEmpty() )
            {
                OSL_FAIL( "ResultDataSupplier::getResult - Empty name!" );
                break;
            }

            // Assemble URL for child.
            rtl::OUString aURL = assembleChildURL( rName );

            m_pImpl->m_aResults.push_back( new ResultListEntry( aURL ) );

            if ( n == nIndex )
            {
                // Result obtained.
                bFound = true;
                break;
            }
        }
    }

    if ( !bFound )
        m_pImpl->m_bCountFinal = sal_True;

    rtl::Reference< ::ucbhelper::ResultSet > xResultSet = getResultSet().get();
    if ( xResultSet.is() )
    {
        // Callbacks follow!
        aGuard.clear();

        if ( nOldCount < m_pImpl->m_aResults.size() )
            xResultSet->rowCountChanged( nOldCount, m_pImpl->m_aResults.size() );

        if ( m_pImpl->m_bCountFinal )
            xResultSet->rowCountFinal();
    }

    return bFound;
}

//=========================================================================
// virtual
sal_uInt32 ResultSetDataSupplier::totalCount()
{
    osl::ClearableGuard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( m_pImpl->m_bCountFinal )
        return m_pImpl->m_aResults.size();

    sal_uInt32 nOldCount = m_pImpl->m_aResults.size();

    if ( queryNamesOfChildren() )
    {
        for ( sal_uInt32 n = nOldCount;
              n < sal::static_int_cast<sal_uInt32>(
                      m_pImpl->m_pNamesOfChildren->getLength());
              ++n )
        {
            const rtl::OUString & rName
                = m_pImpl->m_pNamesOfChildren->getConstArray()[ n ];

            if ( rName.isEmpty() )
            {
                OSL_FAIL( "ResultDataSupplier::getResult - Empty name!" );
                break;
            }

            // Assemble URL for child.
            rtl::OUString aURL = assembleChildURL( rName );

            m_pImpl->m_aResults.push_back( new ResultListEntry( aURL ) );
        }
    }

    m_pImpl->m_bCountFinal = sal_True;

    rtl::Reference< ::ucbhelper::ResultSet > xResultSet = getResultSet().get();
    if ( xResultSet.is() )
    {
        // Callbacks follow!
        aGuard.clear();

        if ( nOldCount < m_pImpl->m_aResults.size() )
            xResultSet->rowCountChanged( nOldCount, m_pImpl->m_aResults.size() );

        xResultSet->rowCountFinal();
    }

    return m_pImpl->m_aResults.size();
}

//=========================================================================
// virtual
sal_uInt32 ResultSetDataSupplier::currentCount()
{
    return m_pImpl->m_aResults.size();
}

//=========================================================================
// virtual
sal_Bool ResultSetDataSupplier::isCountFinal()
{
    return m_pImpl->m_bCountFinal;
}

//=========================================================================
// virtual
uno::Reference< sdbc::XRow >
ResultSetDataSupplier::queryPropertyValues( sal_uInt32 nIndex  )
{
    osl::Guard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( nIndex < m_pImpl->m_aResults.size() )
    {
        uno::Reference< sdbc::XRow > xRow = m_pImpl->m_aResults[ nIndex ]->xRow;
        if ( xRow.is() )
        {
            // Already cached.
            return xRow;
        }
    }

    if ( getResult( nIndex ) )
    {
        uno::Reference< sdbc::XRow > xRow = Content::getPropertyValues(
                        m_pImpl->m_xSMgr,
                        getResultSet()->getProperties(),
                        m_pImpl->m_xContent->getContentProvider().get(),
                        queryContentIdentifierString( nIndex ) );
        m_pImpl->m_aResults[ nIndex ]->xRow = xRow;
        return xRow;
    }

    return uno::Reference< sdbc::XRow >();
}

//=========================================================================
// virtual
void ResultSetDataSupplier::releasePropertyValues( sal_uInt32 nIndex )
{
    osl::Guard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( nIndex < m_pImpl->m_aResults.size() )
        m_pImpl->m_aResults[ nIndex ]->xRow = uno::Reference< sdbc::XRow >();
}

//=========================================================================
// virtual
void ResultSetDataSupplier::close()
{
}

//=========================================================================
// virtual
void ResultSetDataSupplier::validate()
    throw( ucb::ResultSetException )
{
    if ( m_pImpl->m_bThrowException )
        throw ucb::ResultSetException();
}

//=========================================================================
bool ResultSetDataSupplier::queryNamesOfChildren()
{
    osl::Guard< osl::Mutex > aGuard( m_pImpl->m_aMutex );

    if ( m_pImpl->m_pNamesOfChildren == 0 )
    {
        uno::Sequence< rtl::OUString > * pNamesOfChildren
            = new uno::Sequence< rtl::OUString >();

        if ( !m_pImpl->m_xContent->getContentProvider()->queryNamesOfChildren(
                m_pImpl->m_xContent->getIdentifier()->getContentIdentifier(),
                *pNamesOfChildren ) )
        {
            OSL_FAIL( "Got no list of children!" );
            m_pImpl->m_bThrowException = sal_True;
            return false;
        }
        else
        {
            m_pImpl->m_pNamesOfChildren = pNamesOfChildren;
        }
    }
    return true;
}

//=========================================================================
::rtl::OUString
ResultSetDataSupplier::assembleChildURL( const ::rtl::OUString& aName )
{
    rtl::OUString aContURL
        = m_pImpl->m_xContent->getIdentifier()->getContentIdentifier();
    rtl::OUString aURL( aContURL );

    sal_Int32 nUrlEnd = aURL.lastIndexOf( '/' );
    if ( nUrlEnd != aURL.getLength() - 1 )
        aURL += rtl::OUString("/");

    aURL += aName;
    return aURL;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

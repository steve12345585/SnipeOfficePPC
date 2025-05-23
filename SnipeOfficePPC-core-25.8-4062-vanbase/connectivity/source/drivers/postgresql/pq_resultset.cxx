/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 *  Effective License of whole file:
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 *  Parts "Copyright by Sun Microsystems, Inc" prior to August 2011:
 *
 *    The Contents of this file are made available subject to the terms of
 *    the GNU Lesser General Public License Version 2.1
 *
 *    Copyright: 2000 by Sun Microsystems, Inc.
 *
 *    Contributor(s): Joerg Budischewski
 *
 *  All parts contributed on or after August 2011:
 *
 *    Version: MPL 1.1 / GPLv3+ / LGPLv2.1+
 *
 *    The contents of this file are subject to the Mozilla Public License Version
 *    1.1 (the "License"); you may not use this file except in compliance with
 *    the License or as specified alternatively below. You may obtain a copy of
 *    the License at http://www.mozilla.org/MPL/
 *
 *    Software distributed under the License is distributed on an "AS IS" basis,
 *    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 *    for the specific language governing rights and limitations under the
 *    License.
 *
 *    Major Contributor(s):
 *    [ Copyright (C) 2011 Lionel Elie Mamane <lionel@mamane.lu> ]
 *
 *    All Rights Reserved.
 *
 *    For minor contributions see the git repository.
 *
 *    Alternatively, the contents of this file may be used under the terms of
 *    either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 *    the GNU Lesser General Public License Version 2.1 or later (the "LGPLv2.1+"),
 *    in which case the provisions of the GPLv3+ or the LGPLv2.1+ are applicable
 *    instead of those above.
 *
 ************************************************************************/

#include "pq_resultset.hxx"
#include "pq_resultsetmetadata.hxx"

#include <com/sun/star/sdbc/FetchDirection.hpp>
#include <com/sun/star/sdbc/ResultSetConcurrency.hpp>
#include <com/sun/star/sdbc/ResultSetType.hpp>
#include <com/sun/star/sdbc/DataType.hpp>

using rtl::OUString;
using rtl::OUStringToOString;

using osl::MutexGuard;

using com::sun::star::uno::RuntimeException;
using com::sun::star::uno::Any;
using com::sun::star::uno::makeAny;
using com::sun::star::uno::Reference;
using com::sun::star::uno::XInterface;

using com::sun::star::sdbc::SQLException;
using com::sun::star::sdbc::XResultSetMetaData;


namespace pq_sdbc_driver
{
#define ASCII_STR(x) OUString( RTL_CONSTASCII_USTRINGPARAM( x ) )

void ResultSet::checkClosed()
    throw ( com::sun::star::sdbc::SQLException, com::sun::star::uno::RuntimeException )
{
    if( ! m_result )
    {
        throw SQLException( ASCII_STR( "pq_resultset: already closed" ),
                            *this,  OUString(), 1, Any() );
    }

    if( ! m_ppSettings || ! *m_ppSettings || ! (*m_ppSettings)->pConnection )
    {
        throw SQLException( ASCII_STR( "pq_resultset: statement has been closed already" ),
                            *this, OUString(), 1, Any() );
    }

}

ResultSet::ResultSet( const ::rtl::Reference< RefCountedMutex > & refMutex,
                      const Reference< XInterface > & owner,
                      ConnectionSettings **ppSettings,
                      PGresult * result,
                      const rtl::OUString &schema,
                      const rtl::OUString &table)
    : BaseResultSet(
        refMutex, owner, PQntuples( result ),
        PQnfields( result ),(*ppSettings)->tc ),
      m_result( result ),
      m_schema( schema ),
      m_table( table ),
      m_ppSettings( ppSettings )
{
    // LEM TODO: shouldn't these things be inherited from the statement or something like that?
    sal_Bool b = sal_False;
    // Positioned update/delete not supported, so no cursor name
    // Fetch direction and size are cursor-specific things, so not used now.
    // Fetch size not set
    m_props[ BASERESULTSET_FETCH_DIRECTION ] = makeAny(
        com::sun::star::sdbc::FetchDirection::UNKNOWN);
    // No escape processing for now
    m_props[ BASERESULTSET_ESCAPE_PROCESSING ] = Any( &b, getBooleanCppuType() );
    // Bookmarks not implemented for now
    m_props[ BASERESULTSET_IS_BOOKMARKABLE ] = Any( &b, getBooleanCppuType() );
    m_props[ BASERESULTSET_RESULT_SET_CONCURRENCY ] = makeAny(
        com::sun::star::sdbc::ResultSetConcurrency::READ_ONLY );
    m_props[ BASERESULTSET_RESULT_SET_TYPE ] = makeAny(
        com::sun::star::sdbc::ResultSetType::SCROLL_INSENSITIVE );
}


Any ResultSet::getValue( sal_Int32 columnIndex )
{
    Any ret;
    if( PQgetisnull( m_result, m_row, columnIndex -1 ) )
    {
        m_wasNull = true;
    }
    else
    {
        m_wasNull = false;
        ret <<= ::rtl::OUString(
            PQgetvalue( m_result, m_row , columnIndex -1 ) ,
            PQgetlength( m_result, m_row , columnIndex -1 ) ,
            (*m_ppSettings)->encoding );

    }
    return ret;
}

ResultSet::~ResultSet()
{}

void ResultSet::close(  ) throw (SQLException, RuntimeException)
{
    Reference< XInterface > owner;
    {
        MutexGuard guard( m_refMutex->mutex );
        if( m_result )
        {
            PQclear(m_result );
            m_result = 0;
            m_row = -1;
        }
        owner = m_owner;
        m_owner.clear();
    }
}

Reference< XResultSetMetaData > ResultSet::getMetaData(  ) throw (SQLException, RuntimeException)
{
    MutexGuard guard( m_refMutex->mutex );
    checkClosed();
    return new ResultSetMetaData(
        m_refMutex, this, this, m_ppSettings, m_result, m_schema, m_table );
}

sal_Int32 ResultSet::findColumn( const ::rtl::OUString& columnName )
        throw (SQLException, RuntimeException)
{
    MutexGuard guard( m_refMutex->mutex );
    checkClosed();
    return PQfnumber(
        m_result,
        OUStringToOString( columnName, (*m_ppSettings)->encoding ).getStr())
        +1;
}

static bool isNumber( const char * data, sal_Int32 len )
{
    bool ret = false;
    if( len )
    {
        ret = true;
        for( int i = 0 ; i < len ; i ++ )
        {
            if( ( data[i] >= '0' && data[i] <= '9' ) ||
                data[i] == '-' || data[i] == '+' || data[i] == '.' || data[i] == ',' )
            {
                if( data[i] == '-' && i != 0 && i != len-1 )
                {
                    // no number, maybe a date
                    ret = false;
                    break;
                }
            }
            else
            {
                ret = false;
                break;
            }
        }
    }
    return ret;
}

static bool isInteger( const char * data, sal_Int32 len )
{
    bool ret = false;
    if( len )
    {
        ret = true;
        for( int i = 0 ; i < len ; i ++ )
        {
            if( ( data[i] >= '0' && data[i] <= '9' ) ||
                data[i] == '-' || data[i] == '+' )
            {
                if( data[i] == '-' && i != 0 && i != len-1 )
                {
                    // no number, maybe a date
                    ret = false;
                    break;
                }
            }
            else
            {
                ret = false;
                break;
            }
        }
    }
    return ret;
}

static bool isDate( const char * data, sal_Int32 len )
{
    return 10 == len &&
        '-' == data[4] &&
        '-' == data[7] &&
        isInteger( &(data[0]),4 ) &&
        isInteger( &(data[5]),2) &&
        isInteger( &(data[8]),2 );
}

static bool isTime( const char * data, sal_Int32 len )
{
    return 8 == len &&
        ':' == data[2] &&
        ':' == data[5] &&
        isInteger( &(data[0]),2 ) &&
        isInteger( &(data[3]),2) &&
        isInteger( &(data[6]),2 );

}

static bool isTimestamp( const char * data, sal_Int32 len )
{
    return len == 19 && isDate( data, 10) && isTime( &(data[11]),8 );
}

sal_Int32 ResultSet::guessDataType( sal_Int32 column )
{
    // we don't look into more than 100 rows ...
    sal_Int32 ret = com::sun::star::sdbc::DataType::INTEGER;

    int maxRows = ( m_rowCount > 100 ? 100 : m_rowCount );
    for( int i = 0 ; i < maxRows ; i ++ )
    {
        if( ! PQgetisnull( m_result, i , column-1  ) )
        {
            const char * p = PQgetvalue( m_result, i , column -1 );
            int len = PQgetlength( m_result, i , column -1 );

            if( com::sun::star::sdbc::DataType::INTEGER == ret )
            {
                if( ! isInteger( p,len ) )
                    ret = com::sun::star::sdbc::DataType::NUMERIC;
            }
            if( com::sun::star::sdbc::DataType::NUMERIC == ret )
            {
                if( ! isNumber( p,len ) )
                {
                    ret = com::sun::star::sdbc::DataType::DATE;
                }
            }
            if( com::sun::star::sdbc::DataType::DATE == ret )
            {
                if( ! isDate( p,len ) )
                {
                    ret = com::sun::star::sdbc::DataType::TIME;
                }
            }
            if( com::sun::star::sdbc::DataType::TIME == ret )
            {
                if( ! isTime( p,len ) )
                {
                    ret = com::sun::star::sdbc::DataType::TIMESTAMP;
                }
            }
            if( com::sun::star::sdbc::DataType::TIMESTAMP == ret )
            {
                if( ! isTimestamp( p,len ) )
                {
                    ret = com::sun::star::sdbc::DataType::LONGVARCHAR;
                    break;
                }
            }
        }
    }
    return ret;
}

}

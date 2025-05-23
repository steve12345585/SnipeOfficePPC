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

#include <rtl/ustrbuf.hxx>

#include <cppuhelper/typeprovider.hxx>
#include <cppuhelper/queryinterface.hxx>

#include <com/sun/star/beans/PropertyAttribute.hpp>

#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/XParameters.hpp>

#include "pq_xtable.hxx"
#include "pq_xtables.hxx"
#include "pq_xviews.hxx"
#include "pq_xindexes.hxx"
#include "pq_xkeys.hxx"
#include "pq_xcolumns.hxx"
#include "pq_tools.hxx"
#include "pq_statics.hxx"

using osl::MutexGuard;
using osl::Mutex;

using rtl::OUString;
using rtl::OUStringBuffer;
using rtl::OUStringToOString;

using com::sun::star::container::XNameAccess;
using com::sun::star::container::XIndexAccess;
using com::sun::star::container::ElementExistException;
using com::sun::star::container::NoSuchElementException;

using com::sun::star::uno::Reference;
using com::sun::star::uno::Exception;
using com::sun::star::uno::UNO_QUERY;
using com::sun::star::uno::XInterface;
using com::sun::star::uno::Sequence;
using com::sun::star::uno::Any;
using com::sun::star::uno::makeAny;
using com::sun::star::uno::Type;
using com::sun::star::uno::RuntimeException;

using com::sun::star::lang::IllegalArgumentException;
using com::sun::star::lang::IndexOutOfBoundsException;

using com::sun::star::beans::XPropertySetInfo;
using com::sun::star::beans::XFastPropertySet;
using com::sun::star::beans::XMultiPropertySet;
using com::sun::star::beans::XPropertySet;
using com::sun::star::beans::Property;

using com::sun::star::sdbc::XResultSet;
using com::sun::star::sdbc::XPreparedStatement;
using com::sun::star::sdbc::XStatement;
using com::sun::star::sdbc::XParameters;
using com::sun::star::sdbc::XRow;
using com::sun::star::sdbc::SQLException;

namespace pq_sdbc_driver
{
Table::Table( const ::rtl::Reference< RefCountedMutex > & refMutex,
              const Reference< com::sun::star::sdbc::XConnection > & connection,
              ConnectionSettings *pSettings)
    : ReflectionBase(
        getStatics().refl.table.implName,
        getStatics().refl.table.serviceNames,
        refMutex,
        connection,
        pSettings,
        * getStatics().refl.table.pProps ),
      m_pColumns( 0 )
{}

Reference< XPropertySet > Table::createDataDescriptor(  ) throw (RuntimeException)
{
    TableDescriptor * pTable = new TableDescriptor(
        m_refMutex, m_conn, m_pSettings );
    pTable->copyValuesFrom( this );

    return Reference< XPropertySet > ( pTable );
}

Reference< XNameAccess > Table::getColumns(  ) throw (::com::sun::star::uno::RuntimeException)
{
    if( ! m_columns.is() )
    {
        m_columns = Columns::create(
            m_refMutex,
            m_conn,
            m_pSettings,
            extractStringProperty( this, getStatics().SCHEMA_NAME ),
            extractStringProperty( this, getStatics().NAME ),
            &m_pColumns);
    }
    return m_columns;
}

Reference< XNameAccess > Table::getIndexes() throw (::com::sun::star::uno::RuntimeException)
{
    if( ! m_indexes.is() )
    {
        m_indexes = ::pq_sdbc_driver::Indexes::create(
            m_refMutex,
            m_conn,
            m_pSettings,
            extractStringProperty( this, getStatics().SCHEMA_NAME ),
            extractStringProperty( this, getStatics().NAME ) );
    }
    return m_indexes;
}

Reference< XIndexAccess > Table::getKeys(  ) throw (::com::sun::star::uno::RuntimeException)
{
    if( ! m_keys.is() )
    {
        m_keys = ::pq_sdbc_driver::Keys::create(
            m_refMutex,
            m_conn,
            m_pSettings,
            extractStringProperty( this, getStatics().SCHEMA_NAME ),
            extractStringProperty( this, getStatics().NAME ) );
    }
    return m_keys;
}

void Table::rename( const ::rtl::OUString& newName )
        throw (::com::sun::star::sdbc::SQLException,
               ::com::sun::star::container::ElementExistException,
               ::com::sun::star::uno::RuntimeException)
{
    MutexGuard guard( m_refMutex->mutex );
    Statics & st = getStatics();

    ::rtl::OUString oldName = extractStringProperty(this,st.NAME );
    ::rtl::OUString schema = extractStringProperty(this,st.SCHEMA_NAME );
    ::rtl::OUString fullOldName = concatQualified( schema, oldName );

    OUString newTableName;
    OUString newSchemaName;
    // OOo2.0 passes schema + dot + new-table-name while
    // OO1.1.x passes new Name without schema
    // in case name contains a dot, it is interpreted as schema.tablename
    if( newName.indexOf( '.' ) >= 0 )
    {
        splitConcatenatedIdentifier( newName, &newSchemaName, &newTableName );
    }
    else
    {
        newTableName = newName;
        newSchemaName = schema;
    }
    ::rtl::OUString fullNewName = concatQualified( newSchemaName, newTableName );

    if( extractStringProperty( this, st.TYPE ).equals( st.VIEW ) && m_pSettings->views.is() )
    {
        // maintain view list (really strange API !)
        Any a = m_pSettings->pViewsImpl->getByName( fullOldName );
        Reference< com::sun::star::sdbcx::XRename > Xrename;
        a >>= Xrename;
        if( Xrename.is() )
        {
            Xrename->rename( newName );
            setPropertyValue_NoBroadcast_public( st.SCHEMA_NAME, makeAny(newSchemaName) );
        }
    }
    else
    {
        if( ! newSchemaName.equals(schema) )
        {
            // try new schema name first
            try
            {
                OUStringBuffer buf(128);
                buf.appendAscii( RTL_CONSTASCII_STRINGPARAM( "ALTER TABLE" ) );
                bufferQuoteQualifiedIdentifier(buf, schema, oldName, m_pSettings );
                buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("SET SCHEMA" ) );
                bufferQuoteIdentifier( buf, newSchemaName, m_pSettings );
                Reference< XStatement > statement = m_conn->createStatement();
                statement->executeUpdate( buf.makeStringAndClear() );
                setPropertyValue_NoBroadcast_public( st.SCHEMA_NAME, makeAny(newSchemaName) );
                disposeNoThrow( statement );
                schema = newSchemaName;
            }
            catch( com::sun::star::sdbc::SQLException &e )
            {
                OUStringBuffer buf( e.Message );
                buf.appendAscii( RTL_CONSTASCII_STRINGPARAM( "(NOTE: Only postgresql server >= V8.1 support changing a table's schema)" ) );
                e.Message = buf.makeStringAndClear();
                throw;
            }

        }
        if( ! newTableName.equals( oldName ) ) // might also be just the change of a schema name
        {
            OUStringBuffer buf(128);
            buf.appendAscii( RTL_CONSTASCII_STRINGPARAM( "ALTER TABLE" ) );
            bufferQuoteQualifiedIdentifier(buf, schema, oldName, m_pSettings );
            buf.appendAscii( RTL_CONSTASCII_STRINGPARAM("RENAME TO" ) );
            bufferQuoteIdentifier( buf, newTableName, m_pSettings );
            Reference< XStatement > statement = m_conn->createStatement();
            statement->executeUpdate( buf.makeStringAndClear() );
            disposeNoThrow( statement );
        }
    }
    setPropertyValue_NoBroadcast_public( st.NAME, makeAny(newTableName) );
    // inform the container of the name change !
    if( m_pSettings->tables.is() )
    {
        m_pSettings->pTablesImpl->rename( fullOldName, fullNewName );
    }
}

void Table::alterColumnByName(
    const ::rtl::OUString& colName,
    const Reference< XPropertySet >& descriptor )
    throw (SQLException,NoSuchElementException,RuntimeException)
{
    Reference< com::sun::star::container::XNameAccess > colums =
        Reference< com::sun::star::container::XNameAccess > ( getColumns(), UNO_QUERY );

    OUString newName = extractStringProperty(descriptor, getStatics().NAME );
    ::pq_sdbc_driver::alterColumnByDescriptor(
        extractStringProperty( this, getStatics().SCHEMA_NAME ),
        extractStringProperty( this, getStatics().NAME ),
        m_pSettings,
        m_conn->createStatement(),
        Reference< com::sun::star::beans::XPropertySet>( colums->getByName( colName ), UNO_QUERY) ,
        descriptor );

    if( colName !=  newName )
    {
//         m_pColumns->rename( colName, newName );
        m_pColumns->refresh();
    }
}

void Table::alterColumnByIndex(
    sal_Int32 index,
    const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor )
    throw (SQLException,IndexOutOfBoundsException,RuntimeException)
{
    Reference< com::sun::star::container::XIndexAccess > colums =
        Reference< com::sun::star::container::XIndexAccess>( getColumns(), UNO_QUERY );
    Reference< com::sun::star::beans::XPropertySet> column(colums->getByIndex( index ), UNO_QUERY );
    OUString oldName = extractStringProperty( column, getStatics().NAME );
    OUString newName = extractStringProperty( descriptor, getStatics().NAME );
    ::pq_sdbc_driver::alterColumnByDescriptor(
        extractStringProperty( this, getStatics().SCHEMA_NAME ),
        extractStringProperty( this, getStatics().NAME ),
        m_pSettings,
        m_conn->createStatement(),
        column,
        descriptor );
//     m_pColumns->rename( oldName, newName );
    m_pColumns->refresh();
}

Sequence<Type > Table::getTypes() throw( RuntimeException )
{
    static cppu::OTypeCollection *pCollection;
    if( ! pCollection )
    {
        MutexGuard guard( osl::Mutex::getGlobalMutex() );
        if( !pCollection )
        {
            static cppu::OTypeCollection collection(
                getCppuType( (Reference< com::sun::star::sdbcx::XIndexesSupplier> *) 0 ),
                getCppuType( (Reference< com::sun::star::sdbcx::XKeysSupplier> *) 0 ),
                getCppuType( (Reference< com::sun::star::sdbcx::XColumnsSupplier> *) 0 ),
                getCppuType( (Reference< com::sun::star::sdbcx::XRename> *) 0 ),
                getCppuType( (Reference< com::sun::star::sdbcx::XAlterTable> *) 0 ),
                ReflectionBase::getTypes());
            pCollection = &collection;
        }
    }
    return pCollection->getTypes();
}

Sequence< sal_Int8> Table::getImplementationId() throw( RuntimeException )
{
    return getStatics().refl.table.implementationId;
}

Any Table::queryInterface( const Type & reqType ) throw (RuntimeException)
{
    Any ret;

    ret = ReflectionBase::queryInterface( reqType );
    if( ! ret.hasValue() )
        ret = ::cppu::queryInterface(
            reqType,
            static_cast< com::sun::star::sdbcx::XIndexesSupplier * > ( this ),
            static_cast< com::sun::star::sdbcx::XKeysSupplier * > ( this ),
            static_cast< com::sun::star::sdbcx::XColumnsSupplier * > ( this ),
            static_cast< com::sun::star::sdbcx::XRename * > ( this ),
            static_cast< com::sun::star::sdbcx::XAlterTable * > ( this )
            );
    return ret;
}

::com::sun::star::uno::Any Table::getPropertyValue(const ::rtl::OUString& aPropertyName)
        throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    return ReflectionBase::getPropertyValue( aPropertyName );
}


::rtl::OUString Table::getName(  ) throw (::com::sun::star::uno::RuntimeException)
{
    Statics & st = getStatics();
    return concatQualified(
        extractStringProperty( this, st.SCHEMA_NAME ),
        extractStringProperty( this, st.NAME ) );
}

void Table::setName( const ::rtl::OUString& aName ) throw (::com::sun::star::uno::RuntimeException)
{
    rename( aName );
}



//________________________________________________________________________
TableDescriptor::TableDescriptor(
    const ::rtl::Reference< RefCountedMutex > & refMutex,
    const Reference< com::sun::star::sdbc::XConnection > & connection,
    ConnectionSettings *pSettings)
    : ReflectionBase(
        getStatics().refl.tableDescriptor.implName,
        getStatics().refl.tableDescriptor.serviceNames,
        refMutex,
        connection,
        pSettings,
        * getStatics().refl.tableDescriptor.pProps )
{
}

Reference< XNameAccess > TableDescriptor::getColumns(  ) throw (::com::sun::star::uno::RuntimeException)
{
    if( ! m_columns.is() )
    {
        m_columns = new ColumnDescriptors(m_refMutex, m_conn, m_pSettings );
    }
    return m_columns;
}

Reference< XNameAccess > TableDescriptor::getIndexes() throw (::com::sun::star::uno::RuntimeException)
{
    if( ! m_indexes.is() )
    {
        m_indexes = ::pq_sdbc_driver::IndexDescriptors::create(
            m_refMutex,
            m_conn,
            m_pSettings);
    }
    return m_indexes;
}

Reference< XIndexAccess > TableDescriptor::getKeys(  ) throw (::com::sun::star::uno::RuntimeException)
{
    if( ! m_keys.is() )
    {
        m_keys = ::pq_sdbc_driver::KeyDescriptors::create(
            m_refMutex,
            m_conn,
            m_pSettings );
    }
    return m_keys;
}


Sequence<Type > TableDescriptor::getTypes() throw( RuntimeException )
{
    static cppu::OTypeCollection *pCollection;
    if( ! pCollection )
    {
        MutexGuard guard( osl::Mutex::getGlobalMutex() );
        if( !pCollection )
        {
            static cppu::OTypeCollection collection(
                getCppuType( (Reference< com::sun::star::sdbcx::XIndexesSupplier> *) 0 ),
                getCppuType( (Reference< com::sun::star::sdbcx::XKeysSupplier> *) 0 ),
                getCppuType( (Reference< com::sun::star::sdbcx::XColumnsSupplier> *) 0 ),
                ReflectionBase::getTypes());
            pCollection = &collection;
        }
    }
    return pCollection->getTypes();
}

Sequence< sal_Int8> TableDescriptor::getImplementationId() throw( RuntimeException )
{
    return getStatics().refl.tableDescriptor.implementationId;
}

Any TableDescriptor::queryInterface( const Type & reqType ) throw (RuntimeException)
{
    Any ret;

    ret = ReflectionBase::queryInterface( reqType );
    if( ! ret.hasValue() )
        ret = ::cppu::queryInterface(
            reqType,
            static_cast< com::sun::star::sdbcx::XIndexesSupplier * > ( this ),
            static_cast< com::sun::star::sdbcx::XKeysSupplier * > ( this ),
            static_cast< com::sun::star::sdbcx::XColumnsSupplier * > ( this ));
    return ret;
}


Reference< XPropertySet > TableDescriptor::createDataDescriptor(  ) throw (RuntimeException)
{
    TableDescriptor * pTable = new TableDescriptor(
        m_refMutex, m_conn, m_pSettings );

    // TODO: deep copies
    pTable->m_values = m_values;

    return Reference< XPropertySet > ( pTable );
}

}

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

#include <resultset.hxx>
#include "dbastrings.hrc"
#include "apitools.hxx"
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/sdbc/ResultSetType.hpp>
#include <cppuhelper/typeprovider.hxx>
#include <comphelper/property.hxx>
#include <comphelper/sequence.hxx>
#include <comphelper/types.hxx>
#include <tools/debug.hxx>
#include <tools/diagnose_ex.h>
#include <datacolumn.hxx>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <connectivity/dbexception.hxx>
#include <connectivity/dbtools.hxx>
#include <cppuhelper/exc_hlp.hxx>
#include <osl/thread.h>
#include <rtl/logfile.hxx>


using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::container;
using namespace ::cppu;
using namespace ::osl;
using namespace dbaccess;
using namespace dbtools;

DBG_NAME(OResultSet)

OResultSet::OResultSet(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet >& _xResultSet,
                       const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _xStatement,
                       sal_Bool _bCaseSensitive)
           :OResultSetBase(m_aMutex)
           ,OPropertySetHelper(OResultSetBase::rBHelper)
           ,m_xDelegatorResultSet(_xResultSet)
           ,m_aWarnings( Reference< XWarningsSupplier >( _xResultSet, UNO_QUERY ) )
           ,m_bIsBookmarkable(sal_False)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::OResultSet" );
    DBG_CTOR(OResultSet, NULL);

    m_pColumns = new OColumns(*this, m_aMutex, _bCaseSensitive, ::std::vector< ::rtl::OUString>(), NULL,NULL);

    try
    {
        m_aStatement = _xStatement;
        m_xDelegatorResultSetUpdate = m_xDelegatorResultSetUpdate.query( m_xDelegatorResultSet );
        m_xDelegatorRow = m_xDelegatorRow.query( m_xDelegatorResultSet );
        m_xDelegatorRowUpdate = m_xDelegatorRowUpdate.query( m_xDelegatorResultSet );

        Reference< XPropertySet > xSet(m_xDelegatorResultSet, UNO_QUERY);
        xSet->getPropertyValue(PROPERTY_RESULTSETTYPE) >>= m_nResultSetType;
        xSet->getPropertyValue(PROPERTY_RESULTSETCONCURRENCY) >>= m_nResultSetConcurrency;

        // test for Bookmarks
        if (ResultSetType::FORWARD_ONLY != m_nResultSetType)
        {
            Reference <XPropertySetInfo > xInfo(xSet->getPropertySetInfo());
            if (xInfo->hasPropertyByName(PROPERTY_ISBOOKMARKABLE))
            {
                m_bIsBookmarkable = ::comphelper::getBOOL(xSet->getPropertyValue(PROPERTY_ISBOOKMARKABLE));
                OSL_ENSURE( !m_bIsBookmarkable || Reference< XRowLocate >(m_xDelegatorResultSet, UNO_QUERY).is(),
                    "OResultSet::OResultSet: aggregate is inconsistent in it's bookmarkable attribute!" );
                m_bIsBookmarkable = m_bIsBookmarkable && Reference< XRowLocate >(m_xDelegatorResultSet, UNO_QUERY).is();
            }
        }
    }
    catch(Exception&)
    {
    }
}

OResultSet::~OResultSet()
{
    m_pColumns->acquire();
    m_pColumns->disposing();
    delete m_pColumns;

    DBG_DTOR(OResultSet, NULL);
}

// com::sun::star::lang::XTypeProvider
Sequence< Type > OResultSet::getTypes() throw (RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getTypes" );
    OTypeCollection aTypes(::getCppuType( (const Reference< XPropertySet > *)0 ),
                           OResultSetBase::getTypes());

    return aTypes.getTypes();
}

Sequence< sal_Int8 > OResultSet::getImplementationId() throw (RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getImplementationId" );
    static OImplementationId * pId = 0;
    if (! pId)
    {
        MutexGuard aGuard( Mutex::getGlobalMutex() );
        if (! pId)
        {
            static OImplementationId aId;
            pId = &aId;
        }
    }
    return pId->getImplementationId();
}

// com::sun::star::uno::XInterface
Any OResultSet::queryInterface( const Type & rType ) throw (RuntimeException)
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::queryInterface" );
    Any aIface = OResultSetBase::queryInterface( rType );
    if (!aIface.hasValue())
        aIface = ::cppu::queryInterface(
                    rType,
                    static_cast< XPropertySet * >( this ));

    return aIface;
}

void OResultSet::acquire() throw ()
{
    OResultSetBase::acquire();
}

void OResultSet::release() throw ()
{
    OResultSetBase::release();
}


// OResultSetBase
void OResultSet::disposing()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::disposing" );
    OPropertySetHelper::disposing();

    MutexGuard aGuard(m_aMutex);

    // free the columns
    m_pColumns->disposing();

    // close the pending result set
    Reference< XCloseable > (m_xDelegatorResultSet, UNO_QUERY)->close();

    m_xDelegatorResultSet = NULL;
    m_xDelegatorRow = NULL;
    m_xDelegatorRowUpdate = NULL;

    m_aStatement = Reference< XInterface >();
}

// XCloseable
void OResultSet::close(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::close" );
    {
        MutexGuard aGuard( m_aMutex );
        ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);
    }
    dispose();
}

// XServiceInfo
rtl::OUString OResultSet::getImplementationName(  ) throw(RuntimeException)
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getImplementationName" );
    return rtl::OUString("com.sun.star.sdb.OResultSet");
}

sal_Bool OResultSet::supportsService( const ::rtl::OUString& _rServiceName ) throw (RuntimeException)
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::supportsService" );
    return ::comphelper::findValue(getSupportedServiceNames(), _rServiceName, sal_True).getLength() != 0;
}

Sequence< ::rtl::OUString > OResultSet::getSupportedServiceNames(  ) throw (RuntimeException)
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getSupportedServiceNames" );
    Sequence< ::rtl::OUString > aSNS( 2 );
    aSNS[0] = SERVICE_SDBC_RESULTSET;
    aSNS[1] = SERVICE_SDB_RESULTSET;
    return aSNS;
}

// com::sun::star::beans::XPropertySet
Reference< XPropertySetInfo > OResultSet::getPropertySetInfo() throw (RuntimeException)
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getPropertySetInfo" );
    return createPropertySetInfo( getInfoHelper() ) ;
}

// comphelper::OPropertyArrayUsageHelper
::cppu::IPropertyArrayHelper* OResultSet::createArrayHelper( ) const
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::createArrayHelper" );
    BEGIN_PROPERTY_HELPER(6)
        DECL_PROP1(CURSORNAME,              ::rtl::OUString,    READONLY);
        DECL_PROP0(FETCHDIRECTION,          sal_Int32);
        DECL_PROP0(FETCHSIZE,               sal_Int32);
        DECL_PROP1_BOOL(ISBOOKMARKABLE,         READONLY);
        DECL_PROP1(RESULTSETCONCURRENCY,    sal_Int32,      READONLY);
        DECL_PROP1(RESULTSETTYPE,           sal_Int32,      READONLY);
    END_PROPERTY_HELPER();
}

// cppu::OPropertySetHelper
::cppu::IPropertyArrayHelper& OResultSet::getInfoHelper()
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getInfoHelper" );
    return *getArrayHelper();
}

sal_Bool OResultSet::convertFastPropertyValue(Any & rConvertedValue, Any & rOldValue, sal_Int32 nHandle, const Any& rValue ) throw( IllegalArgumentException  )
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::convertFastPropertyValue" );
    // be lazy ...
    rConvertedValue = rValue;
    getFastPropertyValue( rOldValue, nHandle );
    return sal_True;
}

void OResultSet::setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const Any& rValue ) throw (Exception)
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::setFastPropertyValue_NoBroadcast" );
    // set it for the driver result set
    Reference< XPropertySet > xSet(m_xDelegatorResultSet, UNO_QUERY);
    switch (nHandle)
    {
        case PROPERTY_ID_FETCHDIRECTION:
            xSet->setPropertyValue(PROPERTY_FETCHDIRECTION, rValue);
            break;
        case PROPERTY_ID_FETCHSIZE:
            xSet->setPropertyValue(PROPERTY_FETCHSIZE, rValue);
            break;
        default:
            OSL_FAIL("unknown Property");
    }
}

void OResultSet::getFastPropertyValue( Any& rValue, sal_Int32 nHandle ) const
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getFastPropertyValue" );
    switch (nHandle)
    {
        case PROPERTY_ID_ISBOOKMARKABLE:
        {
            sal_Bool bVal = m_bIsBookmarkable;
            rValue.setValue(&bVal, getBooleanCppuType());
        }   break;
        default:
        {
            // get the property name
            ::rtl::OUString aPropName;
            sal_Int16 nAttributes;
            const_cast<OResultSet*>(this)->getInfoHelper().
                fillPropertyMembersByHandle(&aPropName, &nAttributes, nHandle);
            OSL_ENSURE(!aPropName.isEmpty(), "property not found?");

            // now read the value
            rValue = Reference< XPropertySet >(m_xDelegatorResultSet, UNO_QUERY)->getPropertyValue(aPropName);
        }
    }
}

// XWarningsSupplier
Any OResultSet::getWarnings(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getWarnings" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);
    return m_aWarnings.getWarnings();
}

void OResultSet::clearWarnings(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::clearWarnings" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);
    m_aWarnings.clearWarnings();
}

// ::com::sun::star::sdbc::XResultSetMetaDataSupplier
Reference< XResultSetMetaData > OResultSet::getMetaData(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getMetaData" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return Reference< XResultSetMetaDataSupplier >(m_xDelegatorResultSet, UNO_QUERY)->getMetaData();
}

// ::com::sun::star::sdbc::XColumnLocate
sal_Int32 OResultSet::findColumn(const rtl::OUString& columnName) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::findColumn" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return Reference< XColumnLocate >(m_xDelegatorResultSet, UNO_QUERY)->findColumn(columnName);
}

namespace
{
    static Reference< XDatabaseMetaData > lcl_getDBMetaDataFromStatement_nothrow( const Reference< XInterface >& _rxStatement )
    {
        Reference< XDatabaseMetaData > xDBMetaData;
        try
        {
            Reference< XStatement > xStatement( _rxStatement, UNO_QUERY );
            Reference< XPreparedStatement > xPreparedStatement( _rxStatement, UNO_QUERY );
            Reference< XConnection > xConn;
            if ( xStatement.is() )
                xConn = xStatement->getConnection();
            else if ( xPreparedStatement.is() )
                xConn = xPreparedStatement->getConnection();
            if ( xConn.is() )
                xDBMetaData = xConn->getMetaData();
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        return xDBMetaData;
    }
}

// ::com::sun::star::sdbcx::XColumnsSupplier
Reference< ::com::sun::star::container::XNameAccess > OResultSet::getColumns(void) throw( RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getColumns" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    // do we have to populate the columns
    if (!m_pColumns->isInitialized())
    {
        // get the metadata
        Reference< XResultSetMetaData > xMetaData = Reference< XResultSetMetaDataSupplier >(m_xDelegatorResultSet, UNO_QUERY)->getMetaData();

        sal_Int32 nColCount = 0;
        // do we have columns
        try
        {
            Reference< XDatabaseMetaData > xDBMetaData( lcl_getDBMetaDataFromStatement_nothrow( getStatement() ) );
            nColCount = xMetaData->getColumnCount();

            for ( sal_Int32 i = 0; i < nColCount; ++i)
            {
                // retrieve the name of the column
                rtl::OUString sName = xMetaData->getColumnName(i + 1);
                ODataColumn* pColumn = new ODataColumn(xMetaData, m_xDelegatorRow, m_xDelegatorRowUpdate, i + 1, xDBMetaData);

                // don't silently assume that the name is unique - result set implementations
                // are allowed to return duplicate names, but we are required to have
                // unique column names
                if ( m_pColumns->hasByName( sName ) )
                    sName = ::dbtools::createUniqueName( m_pColumns, sName );

                m_pColumns->append( sName, pColumn );
            }
        }
        catch ( const SQLException& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
        m_pColumns->setInitialized();

    #if OSL_DEBUG_LEVEL > 0
        // some sanity checks. Especially in case we auto-adjusted the column names above,
        // this might be reasonable
        try
        {
            const Reference< XNameAccess > xColNames( static_cast< XNameAccess* >( m_pColumns ), UNO_SET_THROW );
            const Sequence< ::rtl::OUString > aNames( xColNames->getElementNames() );
            OSL_POSTCOND( aNames.getLength() == nColCount,
                "OResultSet::getColumns: invalid column count!" );
            for (   const ::rtl::OUString* pName = aNames.getConstArray();
                    pName != aNames.getConstArray() + aNames.getLength();
                    ++pName
                )
            {
                Reference< XPropertySet > xColProps( xColNames->getByName( *pName ), UNO_QUERY_THROW );
                ::rtl::OUString sName;
                OSL_VERIFY( xColProps->getPropertyValue( PROPERTY_NAME ) >>= sName );
                OSL_POSTCOND( sName == *pName, "OResultSet::getColumns: invalid column name!" );
            }

        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    #endif
    }
    return m_pColumns;
}

// ::com::sun::star::sdbc::XRow
sal_Bool OResultSet::wasNull(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::wasNull" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->wasNull();
}

rtl::OUString OResultSet::getString(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getString" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getString(columnIndex);
}

sal_Bool OResultSet::getBoolean(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getBoolean" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getBoolean(columnIndex);
}

sal_Int8 OResultSet::getByte(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getByte" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getByte(columnIndex);
}

sal_Int16 OResultSet::getShort(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getShort" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getShort(columnIndex);
}

sal_Int32 OResultSet::getInt(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getInt" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getInt(columnIndex);
}

sal_Int64 OResultSet::getLong(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getLong" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getLong(columnIndex);
}

float OResultSet::getFloat(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getFloat" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getFloat(columnIndex);
}

double OResultSet::getDouble(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getDouble" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getDouble(columnIndex);
}

Sequence< sal_Int8 > OResultSet::getBytes(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getBytes" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getBytes(columnIndex);
}

::com::sun::star::util::Date OResultSet::getDate(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getDate" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getDate(columnIndex);
}

::com::sun::star::util::Time OResultSet::getTime(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getTime" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getTime(columnIndex);
}

::com::sun::star::util::DateTime OResultSet::getTimestamp(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getTimestamp" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getTimestamp(columnIndex);
}

Reference< ::com::sun::star::io::XInputStream >  OResultSet::getBinaryStream(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getBinaryStream" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getBinaryStream(columnIndex);
}

Reference< ::com::sun::star::io::XInputStream >  OResultSet::getCharacterStream(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getCharacterStream" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getCharacterStream(columnIndex);
}

Any OResultSet::getObject(sal_Int32 columnIndex, const Reference< ::com::sun::star::container::XNameAccess > & typeMap) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getObject" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getObject(columnIndex, typeMap);
}

Reference< XRef >  OResultSet::getRef(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getRef" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getRef(columnIndex);
}

Reference< XBlob >  OResultSet::getBlob(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getBlob" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getBlob(columnIndex);
}

Reference< XClob >  OResultSet::getClob(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getClob" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getClob(columnIndex);
}

Reference< XArray >  OResultSet::getArray(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getArray" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorRow->getArray(columnIndex);
}

// ::com::sun::star::sdbc::XRowUpdate
void OResultSet::updateNull(sal_Int32 columnIndex) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateNull" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateNull(columnIndex);
}

void OResultSet::updateBoolean(sal_Int32 columnIndex, sal_Bool x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateBoolean" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateBoolean(columnIndex, x);
}

void OResultSet::updateByte(sal_Int32 columnIndex, sal_Int8 x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateByte" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateByte(columnIndex, x);
}

void OResultSet::updateShort(sal_Int32 columnIndex, sal_Int16 x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateShort" );
        MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateShort(columnIndex, x);
}

void OResultSet::updateInt(sal_Int32 columnIndex, sal_Int32 x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateInt" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateInt(columnIndex, x);
}

void OResultSet::updateLong(sal_Int32 columnIndex, sal_Int64 x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateLong" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateLong(columnIndex, x);
}

void OResultSet::updateFloat(sal_Int32 columnIndex, float x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateFloat" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateFloat(columnIndex, x);
}

void OResultSet::updateDouble(sal_Int32 columnIndex, double x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateDouble" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateDouble(columnIndex, x);
}

void OResultSet::updateString(sal_Int32 columnIndex, const rtl::OUString& x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateString" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateString(columnIndex, x);
}

void OResultSet::updateBytes(sal_Int32 columnIndex, const Sequence< sal_Int8 >& x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateBytes" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateBytes(columnIndex, x);
}

void OResultSet::updateDate(sal_Int32 columnIndex, const ::com::sun::star::util::Date& x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateDate" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateDate(columnIndex, x);
}

void OResultSet::updateTime(sal_Int32 columnIndex, const ::com::sun::star::util::Time& x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateTime" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateTime(columnIndex, x);
}

void OResultSet::updateTimestamp(sal_Int32 columnIndex, const ::com::sun::star::util::DateTime& x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateTimestamp" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateTimestamp(columnIndex, x);
}

void OResultSet::updateBinaryStream(sal_Int32 columnIndex, const Reference< ::com::sun::star::io::XInputStream > & x, sal_Int32 length) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateBinaryStream" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateBinaryStream(columnIndex, x, length);
}

void OResultSet::updateCharacterStream(sal_Int32 columnIndex, const Reference< ::com::sun::star::io::XInputStream > & x, sal_Int32 length) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateCharacterStream" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateCharacterStream(columnIndex, x, length);
}

void OResultSet::updateNumericObject(sal_Int32 columnIndex, const Any& x, sal_Int32 scale) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateNumericObject" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateNumericObject(columnIndex, x, scale);
}

void OResultSet::updateObject(sal_Int32 columnIndex, const Any& x) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateObject" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorRowUpdate->updateObject(columnIndex, x);
}

// ::com::sun::star::sdbc::XResultSet
sal_Bool OResultSet::next(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::next" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->next();
}

sal_Bool OResultSet::isBeforeFirst(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::isBeforeFirst" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->isBeforeFirst();
}

sal_Bool OResultSet::isAfterLast(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::isAfterLast" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->isAfterLast();
}

sal_Bool OResultSet::isFirst(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::isFirst" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->isFirst();
}

sal_Bool OResultSet::isLast(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::isLast" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->isLast();
}

void OResultSet::beforeFirst(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::beforeFirst" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    m_xDelegatorResultSet->beforeFirst();
}

void OResultSet::afterLast(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::afterLast" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    m_xDelegatorResultSet->afterLast();
}

sal_Bool OResultSet::first(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::first" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->first();
}

sal_Bool OResultSet::last(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::last" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->last();
}

sal_Int32 OResultSet::getRow(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getRow" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->getRow();
}

sal_Bool OResultSet::absolute(sal_Int32 row) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::absolute" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->absolute(row);
}

sal_Bool OResultSet::relative(sal_Int32 rows) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::relative" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->relative(rows);
}

sal_Bool OResultSet::previous(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::previous" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->previous();
}

void OResultSet::refreshRow(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::refreshRow" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    m_xDelegatorResultSet->refreshRow();
}

sal_Bool OResultSet::rowUpdated(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::rowUpdated" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->rowUpdated();
}

sal_Bool OResultSet::rowInserted(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::rowInserted" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->rowInserted();
}

sal_Bool OResultSet::rowDeleted(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::rowDeleted" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_xDelegatorResultSet->rowDeleted();
}

Reference< XInterface > OResultSet::getStatement(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getStatement" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    return m_aStatement;
}

// ::com::sun::star::sdbcx::XRowLocate
Any OResultSet::getBookmark(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::getBookmark" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkBookmarkable();

    return Reference< XRowLocate >(m_xDelegatorResultSet, UNO_QUERY)->getBookmark();
}

sal_Bool OResultSet::moveToBookmark(const Any& bookmark) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::moveToBookmark" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkBookmarkable();

    return Reference< XRowLocate >(m_xDelegatorResultSet, UNO_QUERY)->moveToBookmark(bookmark);
}

sal_Bool OResultSet::moveRelativeToBookmark(const Any& bookmark, sal_Int32 rows) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::moveRelativeToBookmark" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkBookmarkable();

    return Reference< XRowLocate >(m_xDelegatorResultSet, UNO_QUERY)->moveRelativeToBookmark(bookmark, rows);
}

sal_Int32 OResultSet::compareBookmarks(const Any& _first, const Any& _second) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::compareBookmarks" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkBookmarkable();

    return Reference< XRowLocate >(m_xDelegatorResultSet, UNO_QUERY)->compareBookmarks(_first, _second);
}

sal_Bool OResultSet::hasOrderedBookmarks(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::hasOrderedBookmarks" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkBookmarkable();

    return Reference< XRowLocate >(m_xDelegatorResultSet, UNO_QUERY)->hasOrderedBookmarks();
}

sal_Int32 OResultSet::hashBookmark(const Any& bookmark) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::hashBookmark" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkBookmarkable();

    return Reference< XRowLocate >(m_xDelegatorResultSet, UNO_QUERY)->hashBookmark(bookmark);
}

// ::com::sun::star::sdbc::XResultSetUpdate
void OResultSet::insertRow(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::insertRow" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorResultSetUpdate->insertRow();
}

void OResultSet::updateRow(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::updateRow" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorResultSetUpdate->updateRow();
}

void OResultSet::deleteRow(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::deleteRow" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorResultSetUpdate->deleteRow();
}

void OResultSet::cancelRowUpdates(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::cancelRowUpdates" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorResultSetUpdate->cancelRowUpdates();
}

void OResultSet::moveToInsertRow(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::moveToInsertRow" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorResultSetUpdate->moveToInsertRow();
}

void OResultSet::moveToCurrentRow(void) throw( SQLException, RuntimeException )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::moveToCurrentRow" );
    MutexGuard aGuard(m_aMutex);
    ::connectivity::checkDisposed(OResultSetBase::rBHelper.bDisposed);

    checkReadOnly();

    m_xDelegatorResultSetUpdate->moveToCurrentRow();
}

void OResultSet::checkReadOnly() const
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::checkReadOnly" );
    if  (   ( m_nResultSetConcurrency == ResultSetConcurrency::READ_ONLY )
        ||  !m_xDelegatorResultSetUpdate.is()
        )
        throwSQLException( "The result set is read-only.", SQL_GENERAL_ERROR, *const_cast< OResultSet* >( this ) );
}

void OResultSet::checkBookmarkable() const
{
    //RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbaccess", "Ocke.Janssen@sun.com", "OResultSet::checkBookmarkable" );
    if ( !m_bIsBookmarkable )
        throwSQLException( "The result set does not have bookmark support.", SQL_GENERAL_ERROR, *const_cast< OResultSet* >( this ) );
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

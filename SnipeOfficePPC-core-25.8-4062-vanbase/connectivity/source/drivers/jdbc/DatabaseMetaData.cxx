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

#include <sal/macros.h>
#include "java/sql/DatabaseMetaData.hxx"
#include "java/sql/Connection.hxx"
#include "java/sql/ResultSet.hxx"
#include "java/tools.hxx"
#include "java/lang/String.hxx"
#include "connectivity/CommonTools.hxx"
#include "FDatabaseMetaDataResultSet.hxx"
#include <comphelper/types.hxx>
#include "TPrivilegesResultSet.hxx"
#include "diagnose_ex.h"
#include "resource/jdbc_log.hrc"

using namespace ::comphelper;

using namespace connectivity;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;

//**************************************************************
//************ Class: java.sql.DatabaseMetaData
//**************************************************************

jclass java_sql_DatabaseMetaData::theClass              = 0;

java_sql_DatabaseMetaData::~java_sql_DatabaseMetaData()
{
    SDBThreadAttach::releaseRef();
}

jclass java_sql_DatabaseMetaData::getMyClass() const
{
    // the class must be fetched only once, therefore static
    if( !theClass )
        theClass = findMyClass("java/sql/DatabaseMetaData");
    return theClass;
}
// -----------------------------------------------------------------------------
java_sql_DatabaseMetaData::java_sql_DatabaseMetaData( JNIEnv * pEnv, jobject myObj, java_sql_Connection& _rConnection )
    :ODatabaseMetaDataBase( &_rConnection,_rConnection.getConnectionInfo() )
    ,java_lang_Object( pEnv, myObj )
    ,m_pConnection( &_rConnection )
    ,m_aLogger( _rConnection.getLogger() )
{
    SDBThreadAttach::addRef();
}

// -------------------------------------------------------------------------
Reference< XResultSet > java_sql_DatabaseMetaData::impl_getTypeInfo_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callResultSetMethod( "getTypeInfo", mID );
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getCatalogs(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callResultSetMethod( "getCatalogs", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString java_sql_DatabaseMetaData::impl_getCatalogSeparator_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getCatalogSeparator", mID );
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getSchemas(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callResultSetMethod( "getSchemas", mID );
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getColumnPrivileges(
        const Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table, const ::rtl::OUString& columnNamePattern ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callResultSetMethodWithStrings( "getColumnPrivileges", mID, catalog, schema, table, &columnNamePattern );
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getColumns(
        const Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& tableNamePattern, const ::rtl::OUString& columnNamePattern ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callResultSetMethodWithStrings( "getColumns", mID, catalog, schemaPattern, tableNamePattern, &columnNamePattern );
}

// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getTables(
        const Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& tableNamePattern, const Sequence< ::rtl::OUString >& _types ) throw(SQLException, RuntimeException)
{
    static const char * cSignature = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;)Ljava/sql/ResultSet;";
    static const char * cMethodName = "getTables";

    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD, cMethodName );

    jobject out(0);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment geloescht worden!");

    {
        // execute Java-Call
        static jmethodID mID(NULL);
        obtainMethodId(t.pEnv, cMethodName,cSignature, mID);
        OSL_VERIFY_RES( !isExceptionOccurred(t.pEnv,sal_True),"Exception occurred!");
        jvalue args[4];

        args[3].l = 0;
        sal_Int32 typeFilterCount = _types.getLength();
        if ( typeFilterCount )
        {
            jobjectArray pObjArray = static_cast< jobjectArray >( t.pEnv->NewObjectArray( (jsize)typeFilterCount, java_lang_String::st_getMyClass(), 0 ) );
            OSL_VERIFY_RES( !isExceptionOccurred( t.pEnv, sal_True ), "Exception occurred!" );
            const ::rtl::OUString* typeFilter = _types.getConstArray();
            bool bIncludeAllTypes = false;
            for ( sal_Int32 i=0; i<typeFilterCount; ++i, ++typeFilter )
            {
                if ( typeFilter->equalsAsciiL( "%", 1 ) )
                {
                    bIncludeAllTypes = true;
                    break;
                }
                jstring aT = convertwchar_tToJavaString( t.pEnv, *typeFilter );
                t.pEnv->SetObjectArrayElement( pObjArray, (jsize)i, aT );
                OSL_VERIFY_RES( !isExceptionOccurred( t.pEnv, sal_True ), "Exception occurred!" );
            }

            if ( bIncludeAllTypes )
            {
                // the SDBC API allows to pass "%" as table type filter, but in JDBC, "all table types"
                // is represented by the table type being <null/>
                t.pEnv->DeleteLocalRef( pObjArray );
                OSL_VERIFY_RES( !isExceptionOccurred( t.pEnv, sal_True ), "Exception occurred!" );
            }
            else
            {
                args[3].l = pObjArray;
            }
        }
        // if we are to display "all catalogs", then respect m_aCatalogRestriction
        Any aCatalogFilter( catalog );
        if ( !aCatalogFilter.hasValue() )
            aCatalogFilter = m_pConnection->getCatalogRestriction();
        // similar for schema
        Any aSchemaFilter;
        if ( schemaPattern.equalsAsciiL( "%", 1 ) )
            aSchemaFilter = m_pConnection->getSchemaRestriction();
        else
            aSchemaFilter <<= schemaPattern;

        args[0].l = aCatalogFilter.hasValue() ? convertwchar_tToJavaString( t.pEnv, ::comphelper::getString( aCatalogFilter ) ) : NULL;
        args[1].l = aSchemaFilter.hasValue() ? convertwchar_tToJavaString( t.pEnv, ::comphelper::getString( aSchemaFilter ) ) : NULL;
        args[2].l = convertwchar_tToJavaString(t.pEnv,tableNamePattern);
        out = t.pEnv->CallObjectMethod( object, mID, args[0].l, args[1].l,args[2].l,args[3].l);
        jthrowable jThrow = t.pEnv->ExceptionOccurred();
        if ( jThrow )
            t.pEnv->ExceptionClear();// we have to clear the exception here because we want to handle it below
        if ( aCatalogFilter.hasValue() )
        {
            t.pEnv->DeleteLocalRef((jstring)args[0].l);
            OSL_VERIFY_RES( !isExceptionOccurred( t.pEnv, sal_True ), "Exception occurred!" );
        }
        if(args[1].l)
        {
            t.pEnv->DeleteLocalRef((jstring)args[1].l);
            OSL_VERIFY_RES( !isExceptionOccurred( t.pEnv, sal_True ), "Exception occurred!" );
        }
        if(!tableNamePattern.isEmpty())
        {
            t.pEnv->DeleteLocalRef((jstring)args[2].l);
            OSL_VERIFY_RES( !isExceptionOccurred( t.pEnv, sal_True ), "Exception occurred!" );
        }
        //for(INT16 i=0;i<len;i++)
        if ( args[3].l )
        {
            t.pEnv->DeleteLocalRef( (jobjectArray)args[3].l );
            OSL_VERIFY_RES( !isExceptionOccurred( t.pEnv, sal_True ), "Exception occurred!" );
        }

        if ( jThrow )
        {
            if ( t.pEnv->IsInstanceOf( jThrow,java_sql_SQLException_BASE::st_getMyClass() ) )
            {
                java_sql_SQLException_BASE* pException = new java_sql_SQLException_BASE( t.pEnv, jThrow );
                SQLException e( pException->getMessage(),
                                    *this,
                                    pException->getSQLState(),
                                    pException->getErrorCode(),
                                    Any()
                                );
                delete pException;
                throw  e;
            }
        }
    }

    if ( !out )
        return NULL;

    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_SUCCESS, cMethodName );
    return new java_sql_ResultSet( t.pEnv, out, m_aLogger,*m_pConnection);
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getProcedureColumns(
        const Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& procedureNamePattern, const ::rtl::OUString& columnNamePattern ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callResultSetMethodWithStrings( "getProcedureColumns", mID, catalog, schemaPattern, procedureNamePattern, &columnNamePattern );
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getProcedures( const Any&
                catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& procedureNamePattern ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callResultSetMethodWithStrings( "getProcedures", mID, catalog, schemaPattern, procedureNamePattern );
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getVersionColumns(
        const Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callResultSetMethodWithStrings( "getVersionColumns", mID, catalog, schema, table );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxBinaryLiteralLength(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxBinaryLiteralLength", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxRowSize(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxRowSize", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxCatalogNameLength(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxCatalogNameLength", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxCharLiteralLength(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxCharLiteralLength", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxColumnNameLength(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxColumnNameLength", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxColumnsInIndex(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxColumnsInIndex", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxCursorNameLength(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxCursorNameLength", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxConnections(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxConnections", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxColumnsInTable(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxColumnsInTable", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxStatementLength(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxStatementLength", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxTableNameLength(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxTableNameLength", mID );
}
// -------------------------------------------------------------------------
sal_Int32 java_sql_DatabaseMetaData::impl_getMaxTablesInSelect_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxTablesInSelect", mID );
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getExportedKeys(
        const Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callResultSetMethodWithStrings( "getExportedKeys", mID, catalog, schema, table );
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getImportedKeys(
        const Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callResultSetMethodWithStrings( "getImportedKeys", mID, catalog, schema, table );
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getPrimaryKeys(
        const Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callResultSetMethodWithStrings( "getPrimaryKeys", mID, catalog, schema, table );
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getIndexInfo(
        const Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table,
        sal_Bool unique, sal_Bool approximate ) throw(SQLException, RuntimeException)
{
    static const char * cSignature = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZ)Ljava/sql/ResultSet;";
    static const char * cMethodName = "getIndexInfo";

    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD, cMethodName );

    jobject out(0);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment geloescht worden!");

    {
        // execute Java-Call
        static jmethodID mID(NULL);
        obtainMethodId(t.pEnv, cMethodName,cSignature, mID);
        jvalue args[5];
        // convert Parameter
        args[0].l = catalog.hasValue() ? convertwchar_tToJavaString(t.pEnv,comphelper::getString(catalog)) : 0;
        args[1].l = schema.toChar() == '%' ? NULL : convertwchar_tToJavaString(t.pEnv,schema);
        args[2].l = convertwchar_tToJavaString(t.pEnv,table);
        args[3].z = unique;
        args[4].z = approximate;
        out = t.pEnv->CallObjectMethod( object, mID, args[0].l,args[1].l,args[2].l,args[3].z,args[4].z );

        // and clean up
        if(catalog.hasValue())
            t.pEnv->DeleteLocalRef((jstring)args[0].l);
        if(args[1].l)
            t.pEnv->DeleteLocalRef((jstring)args[1].l);
        if(!table.isEmpty())
            t.pEnv->DeleteLocalRef((jstring)args[2].l);
        ThrowLoggedSQLException( m_aLogger, t.pEnv, *this );
    }
    if ( !out )
        return NULL;

    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_SUCCESS, cMethodName );
    return new java_sql_ResultSet( t.pEnv, out, m_aLogger,*m_pConnection);
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getBestRowIdentifier(
        const Any& catalog, const ::rtl::OUString& schema, const ::rtl::OUString& table, sal_Int32 scope,
        sal_Bool nullable ) throw(SQLException, RuntimeException)
{
    static const char * cSignature = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IZ)Ljava/sql/ResultSet;";
    static const char * cMethodName = "getBestRowIdentifier";

    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD, cMethodName );

    jobject out(0);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment geloescht worden!");

    {
        // execute Java-Call
        static jmethodID mID(NULL);
        obtainMethodId(t.pEnv, cMethodName,cSignature, mID);
        jvalue args[3];
        // convert Parameter
        args[0].l = catalog.hasValue() ? convertwchar_tToJavaString(t.pEnv,comphelper::getString(catalog)) : 0;
        args[1].l = schema.toChar() == '%' ? NULL : convertwchar_tToJavaString(t.pEnv,schema);
        args[2].l = convertwchar_tToJavaString(t.pEnv,table);
        out = t.pEnv->CallObjectMethod( object, mID, args[0].l,args[1].l,args[2].l,scope,nullable);

        // and cleanup
        if(catalog.hasValue())
            t.pEnv->DeleteLocalRef((jstring)args[0].l);
        if(args[1].l)
            t.pEnv->DeleteLocalRef((jstring)args[1].l);
        if(!table.isEmpty())
            t.pEnv->DeleteLocalRef((jstring)args[2].l);
        ThrowLoggedSQLException( m_aLogger, t.pEnv, *this );
    }

    if ( !out )
        return NULL;

    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_SUCCESS, cMethodName );
    return new java_sql_ResultSet( t.pEnv, out, m_aLogger,*m_pConnection);
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getTablePrivileges(
        const Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& tableNamePattern ) throw(SQLException, RuntimeException)
{
    if ( m_pConnection->isIgnoreDriverPrivilegesEnabled() )
        return new OResultSetPrivileges(this,catalog,schemaPattern,tableNamePattern);

    static jmethodID mID(NULL);
    Reference< XResultSet > xReturn( impl_callResultSetMethodWithStrings( "getTablePrivileges", mID, catalog, schemaPattern, tableNamePattern ) );

    if ( xReturn.is() )
    {
        // we have to check the result columns for the tables privleges
        Reference< XResultSetMetaDataSupplier > xMetaSup(xReturn,UNO_QUERY);
        if ( xMetaSup.is() )
        {
            Reference< XResultSetMetaData> xMeta = xMetaSup->getMetaData();
            if ( xMeta.is() && xMeta->getColumnCount() != 7 )
            {
                // here we know that the count of column doesn't match
                ::std::map<sal_Int32,sal_Int32> aColumnMatching;
                static const ::rtl::OUString sPrivs[] = {
                                            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TABLE_CAT")),
                                            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TABLE_SCHEM")),
                                            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TABLE_NAME")),
                                            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("GRANTOR")),
                                            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("GRANTEE")),
                                            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PRIVILEGE")),
                                            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("IS_GRANTABLE"))
                                        };

                ::rtl::OUString sColumnName;
                sal_Int32 nCount = xMeta->getColumnCount();
                for (sal_Int32 i = 1 ; i <= nCount ; ++i)
                {
                    sColumnName = xMeta->getColumnName(i);
                    for (sal_uInt32 j = 0 ; j < sizeof(sPrivs)/sizeof(sPrivs[0]); ++j)
                    {
                        if ( sPrivs[j] == sColumnName )
                        {
                            aColumnMatching.insert( ::std::map<sal_Int32,sal_Int32>::value_type(i,j+1) );
                            break;
                        }
                    }

                }
                // fill our own resultset
                ODatabaseMetaDataResultSet* pNewPrivRes = new ODatabaseMetaDataResultSet( ODatabaseMetaDataResultSet::eTablePrivileges );
                Reference< XResultSet > xTemp = xReturn;
                xReturn = pNewPrivRes;
                ODatabaseMetaDataResultSet::ORows aRows;
                Reference< XRow > xRow(xTemp,UNO_QUERY);
                ::rtl::OUString sValue;

                ODatabaseMetaDataResultSet::ORow aRow(8);
                while ( xRow.is() && xTemp->next() )
                {
                    ::std::map<sal_Int32,sal_Int32>::iterator aIter = aColumnMatching.begin();
                    ::std::map<sal_Int32,sal_Int32>::iterator aEnd  = aColumnMatching.end();
                    for (;aIter != aEnd ; ++aIter)
                    {
                        sValue = xRow->getString(aIter->first);
                        if ( xRow->wasNull() )
                            aRow[aIter->second] = ODatabaseMetaDataResultSet::getEmptyValue();
                        else
                            aRow[aIter->second] = new ORowSetValueDecorator(sValue);
                    }

                    aRows.push_back(aRow);
                }
                pNewPrivRes->setRows(aRows);
            }
        }
    }
    return xReturn;
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getCrossReference(
        const Any& primaryCatalog, const ::rtl::OUString& primarySchema,
        const ::rtl::OUString& primaryTable, const Any& foreignCatalog,
        const ::rtl::OUString& foreignSchema, const ::rtl::OUString& foreignTable ) throw(SQLException, RuntimeException)
{
    static const char * cSignature = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/sql/ResultSet;";
    static const char * cMethodName = "getCrossReference";
    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD, cMethodName );

    jobject out(0);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment geloescht worden!");
    {

        // execute Java-Call
        static jmethodID mID(NULL);
        obtainMethodId(t.pEnv, cMethodName,cSignature, mID);
        jvalue args[6];
        // convert Parameter
        args[0].l = primaryCatalog.hasValue() ? convertwchar_tToJavaString(t.pEnv,comphelper::getString(primaryCatalog)) : 0;
        args[1].l = primarySchema.toChar() == '%' ? NULL : convertwchar_tToJavaString(t.pEnv,primarySchema);
        args[2].l = convertwchar_tToJavaString(t.pEnv,primaryTable);
        args[3].l = foreignCatalog.hasValue() ? convertwchar_tToJavaString(t.pEnv,comphelper::getString(foreignCatalog)) : 0;
        args[4].l = foreignSchema.toChar() == '%' ? NULL : convertwchar_tToJavaString(t.pEnv,foreignSchema);
        args[5].l = convertwchar_tToJavaString(t.pEnv,foreignTable);
        out = t.pEnv->CallObjectMethod( object, mID, args[0].l,args[2].l,args[2].l,args[3].l,args[4].l,args[5].l );

        // and clean up
        if(primaryCatalog.hasValue())
            t.pEnv->DeleteLocalRef((jstring)args[0].l);
        if(args[1].l)
            t.pEnv->DeleteLocalRef((jstring)args[1].l);
        if(!primaryTable.isEmpty())
            t.pEnv->DeleteLocalRef((jstring)args[2].l);
        if(foreignCatalog.hasValue())
            t.pEnv->DeleteLocalRef((jstring)args[3].l);
        if(args[4].l)
            t.pEnv->DeleteLocalRef((jstring)args[4].l);
        if(!foreignTable.isEmpty())
            t.pEnv->DeleteLocalRef((jstring)args[5].l);
        ThrowLoggedSQLException( m_aLogger, t.pEnv, *this );
    }

    if ( !out )
        return NULL;

    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_SUCCESS, cMethodName );
    return new java_sql_ResultSet( t.pEnv, out, m_aLogger,*m_pConnection);
}

// -------------------------------------------------------------------------
sal_Bool java_sql_DatabaseMetaData::impl_callBooleanMethod( const char* _pMethodName, jmethodID& _inout_MethodID )
{
    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD, _pMethodName );
    jboolean out( java_lang_Object::callBooleanMethod(_pMethodName,_inout_MethodID) );
    m_aLogger.log< const sal_Char*, sal_Int16>( LogLevel::FINEST, STR_LOG_META_DATA_RESULT, _pMethodName, out );
    return out;
}

// -------------------------------------------------------------------------
::rtl::OUString java_sql_DatabaseMetaData::impl_callStringMethod( const char* _pMethodName, jmethodID& _inout_MethodID )
{
    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD, _pMethodName );

    const ::rtl::OUString sReturn( callStringMethod(_pMethodName,_inout_MethodID) );
    if ( m_aLogger.isLoggable( LogLevel::FINEST ) )
    {
        ::rtl::OUString sLoggedResult( sReturn );
        if ( sLoggedResult.isEmpty() )
            sLoggedResult = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "<empty string>" ) );
        m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_RESULT, _pMethodName, sLoggedResult );
    }

    return sReturn;
}

// -------------------------------------------------------------------------
sal_Int32 java_sql_DatabaseMetaData::impl_callIntMethod( const char* _pMethodName, jmethodID& _inout_MethodID )
{
    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD, _pMethodName );
    sal_Int32 out( (sal_Int32)callIntMethod(_pMethodName,_inout_MethodID) );
    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_RESULT, _pMethodName, (sal_Int32)out );
    return out;
}

// -------------------------------------------------------------------------
sal_Bool java_sql_DatabaseMetaData::impl_callBooleanMethodWithIntArg( const char* _pMethodName, jmethodID& _inout_MethodID, sal_Int32 _nArgument )
{
    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD_ARG1, _pMethodName, _nArgument );

    jboolean out( callBooleanMethodWithIntArg(_pMethodName,_inout_MethodID,_nArgument) );

    m_aLogger.log< const sal_Char*, sal_Int16 >( LogLevel::FINEST, STR_LOG_META_DATA_RESULT, _pMethodName, out );
    return out;
}

// -------------------------------------------------------------------------
Reference< XResultSet > java_sql_DatabaseMetaData::impl_callResultSetMethod( const char* _pMethodName, jmethodID& _inout_MethodID )
{
    SDBThreadAttach t;
    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD, _pMethodName );
    jobject out(callResultSetMethod(t.env(),_pMethodName,_inout_MethodID));
    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_SUCCESS, _pMethodName );
    return new java_sql_ResultSet( t.pEnv, out, m_aLogger,*m_pConnection);
}

// -------------------------------------------------------------------------
Reference< XResultSet > java_sql_DatabaseMetaData::impl_callResultSetMethodWithStrings( const char* _pMethodName, jmethodID& _inout_MethodID,
    const Any& _rCatalog, const ::rtl::OUString& _rSchemaPattern, const ::rtl::OUString& _rLeastPattern,
    const ::rtl::OUString* _pOptionalAdditionalString )
{
    bool bCatalog = _rCatalog.hasValue();
    ::rtl::OUString sCatalog;
    _rCatalog >>= sCatalog;

    bool bSchema = _rSchemaPattern.toChar() != '%';

    // log the call
    if ( m_aLogger.isLoggable( LogLevel::FINEST ) )
    {
        ::rtl::OUString sCatalogLog = bCatalog ? sCatalog : ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "null" ) );
        ::rtl::OUString sSchemaLog = bSchema ? _rSchemaPattern : ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "null" ) );
        if ( _pOptionalAdditionalString )
            m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD_ARG4, _pMethodName, sCatalogLog, sSchemaLog, _rLeastPattern, *_pOptionalAdditionalString );
        else
            m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD_ARG3, _pMethodName, sCatalogLog, sSchemaLog, _rLeastPattern );
    }

    jobject out(0);

    SDBThreadAttach t;
    OSL_ENSURE( t.pEnv, "java_sql_DatabaseMetaData::impl_callResultSetMethodWithStrings: no Java enviroment anymore!" );

    {
        const  char* pSignature = _pOptionalAdditionalString
            ?   "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/sql/ResultSet;"
            :   "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/sql/ResultSet;";
        // obtain method ID
        obtainMethodId(t.pEnv, _pMethodName,pSignature, _inout_MethodID);

        // call method

        {
            jvalue args[4];
            // convert parameters
            args[0].l = bCatalog ? convertwchar_tToJavaString( t.pEnv, sCatalog ) : NULL;
            args[1].l = bSchema ? convertwchar_tToJavaString( t.pEnv, _rSchemaPattern ) : NULL;
            args[2].l = convertwchar_tToJavaString( t.pEnv, _rLeastPattern );
            args[3].l = _pOptionalAdditionalString ? convertwchar_tToJavaString( t.pEnv, *_pOptionalAdditionalString ) : NULL;

            // actually do the call
            if ( _pOptionalAdditionalString )
                out = t.pEnv->CallObjectMethod( object, _inout_MethodID, args[0].l, args[1].l, args[2].l, args[3].l );
            else
                out = t.pEnv->CallObjectMethod( object, _inout_MethodID, args[0].l, args[1].l, args[2].l );

            // clean up
            if ( args[0].l )
                t.pEnv->DeleteLocalRef( (jstring)args[0].l );
            if ( args[1].l )
                t.pEnv->DeleteLocalRef( (jstring)args[1].l );
            if ( args[2].l )
                t.pEnv->DeleteLocalRef( (jstring)args[2].l );
            if ( args[3].l )
                t.pEnv->DeleteLocalRef( (jstring)args[3].l );

            ThrowLoggedSQLException( m_aLogger, t.pEnv, *this );
        }
    }

    if ( !out )
        return NULL;

    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_SUCCESS, _pMethodName );
    return new java_sql_ResultSet( t.pEnv, out, m_aLogger,*m_pConnection);
}

// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::doesMaxRowSizeIncludeBlobs(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "doesMaxRowSizeIncludeBlobs", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::storesLowerCaseQuotedIdentifiers(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "storesLowerCaseQuotedIdentifiers", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::storesLowerCaseIdentifiers(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "storesLowerCaseIdentifiers", mID );
}
// -------------------------------------------------------------------------
sal_Bool java_sql_DatabaseMetaData::impl_storesMixedCaseQuotedIdentifiers_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "storesMixedCaseQuotedIdentifiers", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::storesMixedCaseIdentifiers(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "storesMixedCaseIdentifiers", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::storesUpperCaseQuotedIdentifiers(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "storesUpperCaseQuotedIdentifiers", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::storesUpperCaseIdentifiers(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "storesUpperCaseIdentifiers", mID );
}
// -------------------------------------------------------------------------
sal_Bool java_sql_DatabaseMetaData::impl_supportsAlterTableWithAddColumn_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsAlterTableWithAddColumn", mID );
}
// -------------------------------------------------------------------------
sal_Bool java_sql_DatabaseMetaData::impl_supportsAlterTableWithDropColumn_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsAlterTableWithDropColumn", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxIndexLength(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxIndexLength", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsNonNullableColumns(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsNonNullableColumns", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getCatalogTerm(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getCatalogTerm", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString java_sql_DatabaseMetaData::impl_getIdentifierQuoteString_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getIdentifierQuoteString", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getExtraNameCharacters(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getExtraNameCharacters", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsDifferentTableCorrelationNames(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsDifferentTableCorrelationNames", mID );
}
// -------------------------------------------------------------------------
sal_Bool java_sql_DatabaseMetaData::impl_isCatalogAtStart_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "isCatalogAtStart", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::dataDefinitionIgnoredInTransactions(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "dataDefinitionIgnoredInTransactions", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::dataDefinitionCausesTransactionCommit(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "dataDefinitionCausesTransactionCommit", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsDataManipulationTransactionsOnly(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsDataManipulationTransactionsOnly", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsDataDefinitionAndDataManipulationTransactions(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsDataDefinitionAndDataManipulationTransactions", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsPositionedDelete(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsPositionedDelete", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsPositionedUpdate(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsPositionedUpdate", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsOpenStatementsAcrossRollback(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsOpenStatementsAcrossRollback", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsOpenStatementsAcrossCommit(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsOpenStatementsAcrossCommit", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsOpenCursorsAcrossCommit(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsOpenCursorsAcrossCommit", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsOpenCursorsAcrossRollback(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsOpenCursorsAcrossRollback", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsTransactionIsolationLevel( sal_Int32 level ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethodWithIntArg( "supportsTransactionIsolationLevel", mID, level );
}
// -------------------------------------------------------------------------
sal_Bool java_sql_DatabaseMetaData::impl_supportsSchemasInDataManipulation_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsSchemasInDataManipulation", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsANSI92FullSQL(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsANSI92FullSQL", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsANSI92EntryLevelSQL(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsANSI92EntryLevelSQL", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsIntegrityEnhancementFacility(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsIntegrityEnhancementFacility", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsSchemasInIndexDefinitions(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsSchemasInIndexDefinitions", mID );
}
// -------------------------------------------------------------------------
sal_Bool java_sql_DatabaseMetaData::impl_supportsSchemasInTableDefinitions_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsSchemasInTableDefinitions", mID );
}
// -------------------------------------------------------------------------
sal_Bool java_sql_DatabaseMetaData::impl_supportsCatalogsInTableDefinitions_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsCatalogsInTableDefinitions", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsCatalogsInIndexDefinitions(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsCatalogsInIndexDefinitions", mID );
}
// -------------------------------------------------------------------------
sal_Bool java_sql_DatabaseMetaData::impl_supportsCatalogsInDataManipulation_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsCatalogsInDataManipulation", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsOuterJoins(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsOuterJoins", mID );
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getTableTypes(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callResultSetMethod( "getTableTypes", mID );
}
// -------------------------------------------------------------------------
sal_Int32 java_sql_DatabaseMetaData::impl_getMaxStatements_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxStatements", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxProcedureNameLength(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxProcedureNameLength", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxSchemaNameLength(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxSchemaNameLength", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsTransactions(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsTransactions", mID );
}

// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::allProceduresAreCallable(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "allProceduresAreCallable", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsStoredProcedures(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsStoredProcedures", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsSelectForUpdate(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsSelectForUpdate", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::allTablesAreSelectable(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "allTablesAreSelectable", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::isReadOnly(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "isReadOnly", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::usesLocalFiles(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "usesLocalFiles", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::usesLocalFilePerTable(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "usesLocalFilePerTable", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsTypeConversion(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsTypeConversion", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::nullPlusNonNullIsNull(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "nullPlusNonNullIsNull", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsColumnAliasing(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsColumnAliasing", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsTableCorrelationNames(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsTableCorrelationNames", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsConvert( sal_Int32 fromType, sal_Int32 toType ) throw(SQLException, RuntimeException)
{
    static const char* pMethodName = "supportsConvert";
    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD_ARG2, pMethodName, fromType, toType );

    jboolean out( sal_False );
    SDBThreadAttach t;

    {
        static jmethodID mID(NULL);
        obtainMethodId(t.pEnv, pMethodName,"(II)Z", mID);
        out = t.pEnv->CallBooleanMethod( object, mID, fromType, toType );
        ThrowLoggedSQLException( m_aLogger, t.pEnv, *this );
    }

    m_aLogger.log< const sal_Char*, sal_Int16 >( LogLevel::FINEST, STR_LOG_META_DATA_RESULT, pMethodName, out );
    return out;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsExpressionsInOrderBy(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsExpressionsInOrderBy", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsGroupBy(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsGroupBy", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsGroupByBeyondSelect(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsGroupByBeyondSelect", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsGroupByUnrelated(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsGroupByUnrelated", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsMultipleTransactions(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsMultipleTransactions", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsMultipleResultSets(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsMultipleResultSets", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsLikeEscapeClause(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsLikeEscapeClause", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsOrderByUnrelated(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsOrderByUnrelated", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsUnion(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsUnion", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsUnionAll(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsUnionAll", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsMixedCaseIdentifiers(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsMixedCaseIdentifiers", mID );
}
// -------------------------------------------------------------------------
sal_Bool java_sql_DatabaseMetaData::impl_supportsMixedCaseQuotedIdentifiers_throw(  )
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsMixedCaseQuotedIdentifiers", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::nullsAreSortedAtEnd(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "nullsAreSortedAtEnd", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::nullsAreSortedAtStart(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "nullsAreSortedAtStart", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::nullsAreSortedHigh(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "nullsAreSortedHigh", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::nullsAreSortedLow(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "nullsAreSortedLow", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsSchemasInProcedureCalls(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsSchemasInProcedureCalls", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsSchemasInPrivilegeDefinitions(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsSchemasInPrivilegeDefinitions", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsCatalogsInProcedureCalls(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsCatalogsInProcedureCalls", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsCatalogsInPrivilegeDefinitions(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsCatalogsInPrivilegeDefinitions", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsCorrelatedSubqueries(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsCorrelatedSubqueries", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsSubqueriesInComparisons(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsSubqueriesInComparisons", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsSubqueriesInExists(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsSubqueriesInExists", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsSubqueriesInIns(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsSubqueriesInIns", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsSubqueriesInQuantifieds(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsSubqueriesInQuantifieds", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsANSI92IntermediateSQL(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsANSI92IntermediateSQL", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getURL(  ) throw(SQLException, RuntimeException)
{
    ::rtl::OUString sURL = m_pConnection->getURL();
    if ( sURL.isEmpty() )
    {
        static jmethodID mID(NULL);
        sURL = impl_callStringMethod( "getURL", mID );
    }
    return sURL;
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getUserName(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getUserName", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getDriverName(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getDriverName", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getDriverVersion(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getDriverVersion", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getDatabaseProductVersion(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getDatabaseProductVersion", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getDatabaseProductName(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getDatabaseProductName", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getProcedureTerm(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getProcedureTerm", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getSchemaTerm(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getSchemaTerm", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getDriverMajorVersion(  ) throw(RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getDriverMajorVersion", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getDefaultTransactionIsolation(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getDefaultTransactionIsolation", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getDriverMinorVersion(  ) throw(RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getDriverMinorVersion", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getSQLKeywords(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getSQLKeywords", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getSearchStringEscape(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getSearchStringEscape", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getStringFunctions(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getStringFunctions", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getTimeDateFunctions(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getTimeDateFunctions", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getSystemFunctions(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getSystemFunctions", mID );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL java_sql_DatabaseMetaData::getNumericFunctions(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callStringMethod( "getNumericFunctions", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsExtendedSQLGrammar(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsExtendedSQLGrammar", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsCoreSQLGrammar(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsCoreSQLGrammar", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsMinimumSQLGrammar(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsMinimumSQLGrammar", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsFullOuterJoins(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsFullOuterJoins", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsLimitedOuterJoins(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsLimitedOuterJoins", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxColumnsInGroupBy(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxColumnsInGroupBy", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxColumnsInOrderBy(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxColumnsInOrderBy", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxColumnsInSelect(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxColumnsInSelect", mID );
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL java_sql_DatabaseMetaData::getMaxUserNameLength(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callIntMethod( "getMaxUserNameLength", mID );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsResultSetType( sal_Int32 setType ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethodWithIntArg( "supportsResultSetType", mID, setType );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsResultSetConcurrency( sal_Int32 setType, sal_Int32 concurrency ) throw(SQLException, RuntimeException)
{
    static const char* pMethodName = "supportsResultSetConcurrency";
    m_aLogger.log( LogLevel::FINEST, STR_LOG_META_DATA_METHOD_ARG2, pMethodName, setType, concurrency );

    jboolean out( sal_False );
    SDBThreadAttach t;

    {
        static jmethodID mID(NULL);
        obtainMethodId(t.pEnv, pMethodName,"(II)Z", mID);
        out =   t.pEnv->CallBooleanMethod( object, mID, setType, concurrency);
        ThrowLoggedSQLException( m_aLogger, t.pEnv, *this );
    }

    m_aLogger.log< const sal_Char*, sal_Int16 >( LogLevel::FINEST, STR_LOG_META_DATA_RESULT, pMethodName, out );
    return out;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::ownUpdatesAreVisible( sal_Int32 setType ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethodWithIntArg( "ownUpdatesAreVisible", mID, setType );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::ownDeletesAreVisible( sal_Int32 setType ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethodWithIntArg( "ownDeletesAreVisible", mID, setType );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::ownInsertsAreVisible( sal_Int32 setType ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethodWithIntArg( "ownInsertsAreVisible", mID, setType );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::othersUpdatesAreVisible( sal_Int32 setType ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethodWithIntArg( "othersUpdatesAreVisible", mID, setType );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::othersDeletesAreVisible( sal_Int32 setType ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethodWithIntArg( "othersDeletesAreVisible", mID, setType );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::othersInsertsAreVisible( sal_Int32 setType ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethodWithIntArg( "othersInsertsAreVisible", mID, setType );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::updatesAreDetected( sal_Int32 setType ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethodWithIntArg( "updatesAreDetected", mID, setType );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::deletesAreDetected( sal_Int32 setType ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethodWithIntArg( "deletesAreDetected", mID, setType );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::insertsAreDetected( sal_Int32 setType ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethodWithIntArg( "insertsAreDetected", mID, setType );
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL java_sql_DatabaseMetaData::supportsBatchUpdates(  ) throw(SQLException, RuntimeException)
{
    static jmethodID mID(NULL);
    return impl_callBooleanMethod( "supportsBatchUpdates", mID );
}
// -------------------------------------------------------------------------
Reference< XResultSet > SAL_CALL java_sql_DatabaseMetaData::getUDTs(
        const Any& catalog, const ::rtl::OUString& schemaPattern, const ::rtl::OUString& typeNamePattern,
        const Sequence< sal_Int32 >& types ) throw(SQLException, RuntimeException)
{
    jobject out(0);
    SDBThreadAttach t; OSL_ENSURE(t.pEnv,"Java Enviroment geloescht worden!");
    {


        static const char * cSignature = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[I)Ljava/sql/ResultSet;";
        static const char * cMethodName = "getUDTs";
        // dismiss Java-Call
        static jmethodID mID(NULL);
        obtainMethodId(t.pEnv, cMethodName,cSignature, mID);
        {
            jvalue args[4];
            // initialize temporary Variable
            args[0].l = catalog.hasValue() ? convertwchar_tToJavaString(t.pEnv,comphelper::getString(catalog)) : 0;
            args[1].l = schemaPattern.toChar() == '%' ? NULL : convertwchar_tToJavaString(t.pEnv,schemaPattern);
            args[2].l = convertwchar_tToJavaString(t.pEnv,typeNamePattern);
            jintArray pArray = t.pEnv->NewIntArray(types.getLength());
            t.pEnv->SetIntArrayRegion(pArray,0,types.getLength(),(jint*)types.getConstArray());
            args[3].l = pArray;

            out = t.pEnv->CallObjectMethod( object, mID, args[0].l, args[1].l,args[2].l,args[3].l);

            if(catalog.hasValue())
                t.pEnv->DeleteLocalRef((jstring)args[0].l);
            if(!schemaPattern.isEmpty())
                t.pEnv->DeleteLocalRef((jstring)args[1].l);
            if(!typeNamePattern.isEmpty())
                t.pEnv->DeleteLocalRef((jstring)args[2].l);
            if(args[3].l)
                t.pEnv->DeleteLocalRef((jintArray)args[3].l);
            ThrowLoggedSQLException( m_aLogger, t.pEnv, *this );
        }
    }

    return out ? new java_sql_ResultSet( t.pEnv, out, m_aLogger,*m_pConnection ) : 0;
}
// -------------------------------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

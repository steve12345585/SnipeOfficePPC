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

#include "connectivity/dbtools.hxx"
#include "connectivity/dbconversion.hxx"
#include "connectivity/dbcharset.hxx"
#include "connectivity/SQLStatementHelper.hxx"
#include <unotools/confignode.hxx>
#include "resource/sharedresources.hxx"
#include "resource/common_res.hrc"
#include <com/sun/star/sdbc/XConnection.hpp>
#include <com/sun/star/sdbc/ColumnValue.hpp>
#include <com/sun/star/sdbc/DataType.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#include <com/sun/star/sdbcx/XKeysSupplier.hpp>
#include <com/sun/star/sdbc/XDriverAccess.hpp>
#include <com/sun/star/sdbcx/XDataDefinitionSupplier.hpp>
#include <com/sun/star/sdbcx/Privilege.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/sdbc/KeyRule.hpp>
#include <com/sun/star/sdbcx/KeyType.hpp>
#include "TConnection.hxx"
#include "connectivity/sdbcx/VColumn.hxx"
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/container/XChild.hpp>

#include <tools/diagnose_ex.h>
#include <unotools/sharedunocomponent.hxx>

//.........................................................................
namespace dbtools
{
//.........................................................................
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::sdbc;
    using namespace ::com::sun::star::sdbcx;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::container;
    using namespace ::com::sun::star::frame;
    using namespace connectivity;
    using namespace comphelper;

::rtl::OUString createStandardColumnPart(const Reference< XPropertySet >& xColProp,const Reference< XConnection>& _xConnection,ISQLStatementHelper* _pHelper,const ::rtl::OUString& _sCreatePattern)
{

    Reference<XDatabaseMetaData> xMetaData = _xConnection->getMetaData();

    ::dbtools::OPropertyMap& rPropMap = OMetaConnection::getPropMap();

    ::rtl::OUString sTypeName;
    sal_Int32       nDataType   = 0;
    sal_Int32       nPrecision  = 0;
    sal_Int32       nScale      = 0;

    const ::rtl::OUString sQuoteString = xMetaData->getIdentifierQuoteString();
    ::rtl::OUStringBuffer aSql = ::dbtools::quoteName(sQuoteString,::comphelper::getString(xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_NAME))));

    aSql.appendAscii(" ");

    nDataType = nPrecision = nScale = 0;
    sal_Bool bIsAutoIncrement = sal_False;
    xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_TYPENAME))           >>= sTypeName;
    xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_TYPE))               >>= nDataType;
    xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_PRECISION))          >>= nPrecision;
    xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_SCALE))              >>= nScale;
    xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_ISAUTOINCREMENT))    >>= bIsAutoIncrement;

    // check if the user enter a specific string to create autoincrement values
    ::rtl::OUString sAutoIncrementValue;
    Reference<XPropertySetInfo> xPropInfo = xColProp->getPropertySetInfo();
    if ( xPropInfo.is() && xPropInfo->hasPropertyByName(rPropMap.getNameByIndex(PROPERTY_ID_AUTOINCREMENTCREATION)) )
        xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_AUTOINCREMENTCREATION)) >>= sAutoIncrementValue;
    // look if we have to use precisions
    sal_Bool bUseLiteral = sal_False;
    ::rtl::OUString sPreFix,sPostFix,sCreateParams;
    {
        Reference<XResultSet> xRes = xMetaData->getTypeInfo();
        if(xRes.is())
        {
            Reference<XRow> xRow(xRes,UNO_QUERY);
            while(xRes->next())
            {
                ::rtl::OUString sTypeName2Cmp = xRow->getString(1);
                sal_Int32 nType = xRow->getShort(2);
                sPreFix = xRow->getString (4);
                sPostFix = xRow->getString (5);
                sCreateParams = xRow->getString(6);
                // first identical type will be used if typename is empty
                if ( sTypeName.isEmpty() && nType == nDataType )
                    sTypeName = sTypeName2Cmp;

                if( sTypeName.equalsIgnoreAsciiCase(sTypeName2Cmp) && nType == nDataType && !sCreateParams.isEmpty() && !xRow->wasNull())
                {
                    bUseLiteral = sal_True;
                    break;
                }
            }
        }
    }

    sal_Int32 nIndex = 0;
    if ( !sAutoIncrementValue.isEmpty() && (nIndex = sTypeName.indexOf(sAutoIncrementValue)) != -1 )
    {
        sTypeName = sTypeName.replaceAt(nIndex,sTypeName.getLength() - nIndex,::rtl::OUString());
    }

    if ( (nPrecision > 0 || nScale > 0) && bUseLiteral )
    {
        sal_Int32 nParenPos = sTypeName.indexOf('(');
        if ( nParenPos == -1 )
        {
            aSql.append(sTypeName);
            aSql.appendAscii("(");
        }
        else
        {
            aSql.append(sTypeName.copy(0,++nParenPos));
        }

        if ( nPrecision > 0 && nDataType != DataType::TIMESTAMP )
        {
            aSql.append(nPrecision);
            if ( (nScale > 0) || (!_sCreatePattern.isEmpty() && sCreateParams.indexOf(_sCreatePattern) != -1) )
                aSql.appendAscii(",");
        }
        if ( (nScale > 0) || ( !_sCreatePattern.isEmpty() && sCreateParams.indexOf(_sCreatePattern) != -1 ) || nDataType == DataType::TIMESTAMP )
            aSql.append(nScale);

        if ( nParenPos == -1 )
            aSql.appendAscii(")");
        else
        {
            nParenPos = sTypeName.indexOf(')',nParenPos);
            aSql.append(sTypeName.copy(nParenPos));
        }
    }
    else
        aSql.append(sTypeName); // simply add the type name

    ::rtl::OUString aDefault = ::comphelper::getString(xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_DEFAULTVALUE)));
    if ( !aDefault.isEmpty() )
    {
        aSql.append(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" DEFAULT ")));
        aSql.append(sPreFix);
        aSql.append(aDefault);
        aSql.append(sPostFix);
    } // if ( aDefault.getLength() )

    if(::comphelper::getINT32(xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_ISNULLABLE))) == ColumnValue::NO_NULLS)
        aSql.append(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" NOT NULL")));

    if ( bIsAutoIncrement && !sAutoIncrementValue.isEmpty())
    {
        aSql.appendAscii(" ");
        aSql.append(sAutoIncrementValue);
    }

    if ( _pHelper )
        _pHelper->addComment(xColProp,aSql);

    return aSql.makeStringAndClear();
}
// -----------------------------------------------------------------------------

::rtl::OUString createStandardCreateStatement(const Reference< XPropertySet >& descriptor,const Reference< XConnection>& _xConnection,ISQLStatementHelper* _pHelper,const ::rtl::OUString& _sCreatePattern)
{
    ::rtl::OUStringBuffer aSql(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CREATE TABLE ")));
    ::rtl::OUString sCatalog,sSchema,sTable,sComposedName;

    Reference<XDatabaseMetaData> xMetaData = _xConnection->getMetaData();
    ::dbtools::OPropertyMap& rPropMap = OMetaConnection::getPropMap();

    descriptor->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_CATALOGNAME))  >>= sCatalog;
    descriptor->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_SCHEMANAME))   >>= sSchema;
    descriptor->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_NAME))         >>= sTable;

    sComposedName = ::dbtools::composeTableName( xMetaData, sCatalog, sSchema, sTable, sal_True, ::dbtools::eInTableDefinitions );
    if ( sComposedName.isEmpty() )
        ::dbtools::throwFunctionSequenceException(_xConnection);

    aSql.append(sComposedName);
    aSql.append(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" (")));

    // columns
    Reference<XColumnsSupplier> xColumnSup(descriptor,UNO_QUERY);
    Reference<XIndexAccess> xColumns(xColumnSup->getColumns(),UNO_QUERY);
    // check if there are columns
    if(!xColumns.is() || !xColumns->getCount())
        ::dbtools::throwFunctionSequenceException(_xConnection);

    Reference< XPropertySet > xColProp;

    sal_Int32 nCount = xColumns->getCount();
    for(sal_Int32 i=0;i<nCount;++i)
    {
        if ( (xColumns->getByIndex(i) >>= xColProp) && xColProp.is() )
        {
            aSql.append(createStandardColumnPart(xColProp,_xConnection,_pHelper,_sCreatePattern));
            aSql.appendAscii(",");
        }
    }
    return aSql.makeStringAndClear();
}
namespace
{
    ::rtl::OUString generateColumnNames(const Reference<XIndexAccess>& _xColumns,const Reference<XDatabaseMetaData>& _xMetaData)
    {
        ::dbtools::OPropertyMap& rPropMap = OMetaConnection::getPropMap();
        static const ::rtl::OUString sComma(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(",")));

        const ::rtl::OUString sQuote(_xMetaData->getIdentifierQuoteString());
        ::rtl::OUString sSql( RTL_CONSTASCII_USTRINGPARAM( " (" ));
        Reference< XPropertySet > xColProp;

        sal_Int32 nColCount  = _xColumns->getCount();
        for(sal_Int32 i=0;i<nColCount;++i)
        {
            if ( (_xColumns->getByIndex(i) >>= xColProp) && xColProp.is() )
                sSql += ::dbtools::quoteName(sQuote,::comphelper::getString(xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_NAME))))
                        + sComma;
        }

        if ( nColCount )
            sSql = sSql.replaceAt(sSql.getLength()-1,1,::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(")")));
        return sSql;
    }
}
// -----------------------------------------------------------------------------
::rtl::OUString createStandardKeyStatement(const Reference< XPropertySet >& descriptor,const Reference< XConnection>& _xConnection)
{
    Reference<XDatabaseMetaData> xMetaData = _xConnection->getMetaData();
    ::dbtools::OPropertyMap& rPropMap = OMetaConnection::getPropMap();

    ::rtl::OUStringBuffer aSql;
    // keys
    Reference<XKeysSupplier> xKeySup(descriptor,UNO_QUERY);
    Reference<XIndexAccess> xKeys = xKeySup->getKeys();
    if ( xKeys.is() )
    {
        Reference< XPropertySet > xColProp;
        Reference<XIndexAccess> xColumns;
        Reference<XColumnsSupplier> xColumnSup;
        ::rtl::OUString sCatalog,sSchema,sTable,sComposedName;
        sal_Bool bPKey = sal_False;
        for(sal_Int32 i=0;i<xKeys->getCount();++i)
        {
            if ( (xKeys->getByIndex(i) >>= xColProp) && xColProp.is() )
            {

                sal_Int32 nKeyType      = ::comphelper::getINT32(xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_TYPE)));

                if ( nKeyType == KeyType::PRIMARY )
                {
                    if(bPKey)
                        ::dbtools::throwFunctionSequenceException(_xConnection);

                    bPKey = sal_True;
                    xColumnSup = Reference<XColumnsSupplier>(xColProp,UNO_QUERY);
                    xColumns = Reference<XIndexAccess>(xColumnSup->getColumns(),UNO_QUERY);
                    if(!xColumns.is() || !xColumns->getCount())
                        ::dbtools::throwFunctionSequenceException(_xConnection);

                    const ::rtl::OUString sQuote     = xMetaData->getIdentifierQuoteString();
                    aSql.append(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" PRIMARY KEY ")));
                    aSql.append(generateColumnNames(xColumns,xMetaData));
                }
                else if(nKeyType == KeyType::UNIQUE)
                {
                    xColumnSup = Reference<XColumnsSupplier>(xColProp,UNO_QUERY);
                    xColumns = Reference<XIndexAccess>(xColumnSup->getColumns(),UNO_QUERY);
                    if(!xColumns.is() || !xColumns->getCount())
                        ::dbtools::throwFunctionSequenceException(_xConnection);

                    const ::rtl::OUString sQuote     = xMetaData->getIdentifierQuoteString();
                    aSql.append(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" UNIQUE ")));
                    aSql.append(generateColumnNames(xColumns,xMetaData));
                }
                else if(nKeyType == KeyType::FOREIGN)
                {
                    sal_Int32 nDeleteRule   = getINT32(xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_DELETERULE)));

                    xColumnSup = Reference<XColumnsSupplier>(xColProp,UNO_QUERY);
                    xColumns = Reference<XIndexAccess>(xColumnSup->getColumns(),UNO_QUERY);
                    if(!xColumns.is() || !xColumns->getCount())
                        ::dbtools::throwFunctionSequenceException(_xConnection);

                    aSql.append(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" FOREIGN KEY ")));
                    ::rtl::OUString sRefTable = getString(xColProp->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_REFERENCEDTABLE)));
                    ::dbtools::qualifiedNameComponents(xMetaData,
                                                        sRefTable,
                                                        sCatalog,
                                                        sSchema,
                                                        sTable,
                                                        ::dbtools::eInDataManipulation);
                    sComposedName = ::dbtools::composeTableName( xMetaData, sCatalog, sSchema, sTable, sal_True, ::dbtools::eInTableDefinitions );


                    if ( sComposedName.isEmpty() )
                        ::dbtools::throwFunctionSequenceException(_xConnection);

                    aSql.append(generateColumnNames(xColumns,xMetaData));

                    switch(nDeleteRule)
                    {
                        case KeyRule::CASCADE:
                            aSql.append(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" ON DELETE CASCADE ")));
                            break;
                        case KeyRule::RESTRICT:
                            aSql.append(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" ON DELETE RESTRICT ")));
                            break;
                        case KeyRule::SET_NULL:
                            aSql.append(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" ON DELETE SET NULL ")));
                            break;
                        case KeyRule::SET_DEFAULT:
                            aSql.append(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" ON DELETE SET DEFAULT ")));
                            break;
                        default:
                            ;
                    }
                }
            }
        }
    }

    if ( aSql.getLength() )
    {
        if ( aSql[aSql.getLength() - 1] == ',' )
            aSql[aSql.getLength() - 1] = ')';
        else
            aSql.appendAscii(")");
    }

    return aSql.makeStringAndClear();

}
// -----------------------------------------------------------------------------
::rtl::OUString createSqlCreateTableStatement(  const Reference< XPropertySet >& descriptor,
                                                const Reference< XConnection>& _xConnection,
                                                ISQLStatementHelper* _pHelper,
                                                const ::rtl::OUString& _sCreatePattern)
{
    ::rtl::OUString aSql = ::dbtools::createStandardCreateStatement(descriptor,_xConnection,_pHelper,_sCreatePattern);
    const ::rtl::OUString sKeyStmt = ::dbtools::createStandardKeyStatement(descriptor,_xConnection);
    if ( !sKeyStmt.isEmpty() )
        aSql += sKeyStmt;
    else
    {
        if ( aSql.lastIndexOf(',') == (aSql.getLength()-1) )
            aSql = aSql.replaceAt(aSql.getLength()-1,1,::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(")")));
        else
            aSql += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(")"));
    }
    return aSql;
}
namespace
{
    Reference<XPropertySet> lcl_createSDBCXColumn(const Reference<XNameAccess>& _xPrimaryKeyColumns,
                                          const Reference<XConnection>& _xConnection,
                                          const Any& _aCatalog,
                                          const ::rtl::OUString& _aSchema,
                                          const ::rtl::OUString& _aTable,
                                          const ::rtl::OUString& _rQueryName,
                                          const ::rtl::OUString& _rName,
                                          sal_Bool _bCase,
                                          sal_Bool _bQueryForInfo,
                                          sal_Bool _bIsAutoIncrement,
                                          sal_Bool _bIsCurrency,
                                          sal_Int32 _nDataType)
    {
        Reference<XPropertySet> xProp;
        Reference<XDatabaseMetaData> xMetaData = _xConnection->getMetaData();
        Reference< XResultSet > xResult = xMetaData->getColumns(_aCatalog, _aSchema, _aTable, _rQueryName);
        ::rtl::OUString sCatalog;
        _aCatalog >>= sCatalog;

        if ( xResult.is() )
        {
            UStringMixEqual aMixCompare(_bCase);
            Reference< XRow > xRow(xResult,UNO_QUERY);
            while( xResult->next() )
            {
                if ( aMixCompare(xRow->getString(4),_rName) )
                {
                    sal_Int32       nField5 = xRow->getInt(5);
                    ::rtl::OUString aField6 = xRow->getString(6);
                    sal_Int32       nField7 = xRow->getInt(7)
                                ,   nField9 = xRow->getInt(9)
                                ,   nField11= xRow->getInt(11);
                    ::rtl::OUString sField12 = xRow->getString(12),
                                    sField13 = xRow->getString(13);
                    ::comphelper::disposeComponent(xRow);

                    sal_Bool bAutoIncrement = _bIsAutoIncrement
                            ,bIsCurrency    = _bIsCurrency;
                    if ( _bQueryForInfo )
                    {
                        const ::rtl::OUString sQuote = xMetaData->getIdentifierQuoteString();
                        ::rtl::OUString sQuotedName  = ::dbtools::quoteName(sQuote,_rName);
                        ::rtl::OUString sComposedName;
                        sComposedName = composeTableNameForSelect(_xConnection, getString( _aCatalog ), _aSchema, _aTable );

                        ColumnInformationMap aInfo(_bCase);
                        collectColumnInformation(_xConnection,sComposedName,sQuotedName,aInfo);
                        ColumnInformationMap::iterator aIter = aInfo.begin();
                        if ( aIter != aInfo.end() )
                        {
                            bAutoIncrement  = aIter->second.first.first;
                            bIsCurrency     = aIter->second.first.second;
                            if ( DataType::OTHER == nField5 )
                                nField5     = aIter->second.second;
                        }
                    }
                    else if ( DataType::OTHER == nField5 )
                        nField5 = _nDataType;

                    if ( nField11 != ColumnValue::NO_NULLS )
                    {
                        try
                        {
                            if ( _xPrimaryKeyColumns.is() )
                            {
                                if ( _xPrimaryKeyColumns->hasByName(_rName) )
                                    nField11 = ColumnValue::NO_NULLS;

                            }
                            else
                            {
                                Reference< XResultSet > xPKeys = xMetaData->getPrimaryKeys( _aCatalog, _aSchema, _aTable );
                                Reference< XRow > xPKeyRow( xPKeys, UNO_QUERY_THROW );
                                while( xPKeys->next() ) // there can be only one primary key
                                {
                                    ::rtl::OUString sKeyColumn = xPKeyRow->getString(4);
                                    if ( aMixCompare(_rName,sKeyColumn) )
                                    {
                                        nField11 = ColumnValue::NO_NULLS;
                                        break;
                                    }
                                }
                            }
                        }
                        catch(SQLException&)
                        {
                            OSL_FAIL( "lcl_createSDBCXColumn: caught an exception!" );
                        }
                    }

                    connectivity::sdbcx::OColumn* pRet = new connectivity::sdbcx::OColumn(_rName,
                                                aField6,
                                                sField13,
                                                sField12,
                                                nField11,
                                                nField7,
                                                nField9,
                                                nField5,
                                                bAutoIncrement,
                                                sal_False,
                                                bIsCurrency,
                                                _bCase,
                                                sCatalog,
                                                _aSchema,
                                                _aTable);

                    xProp = pRet;
                    break;
                }
            }
        }

        return xProp;
    }
    //------------------------------------------------------------------
    Reference< XModel> lcl_getXModel(const Reference< XInterface>& _xIface)
    {
        Reference< XInterface > xParent = _xIface;
        Reference< XModel > xModel(xParent,UNO_QUERY);;
        while( xParent.is() && !xModel.is() )
        {
            Reference<XChild> xChild(xParent,UNO_QUERY);
            xParent.set(xChild.is() ? xChild->getParent() : Reference< XInterface >(),UNO_QUERY);
            xModel.set(xParent,UNO_QUERY);
        }
        return xModel;
    }
}
// -----------------------------------------------------------------------------
Reference<XPropertySet> createSDBCXColumn(const Reference<XPropertySet>& _xTable,
                                          const Reference<XConnection>& _xConnection,
                                          const ::rtl::OUString& _rName,
                                          sal_Bool _bCase,
                                          sal_Bool _bQueryForInfo,
                                          sal_Bool _bIsAutoIncrement,
                                          sal_Bool _bIsCurrency,
                                          sal_Int32 _nDataType)
{
    Reference<XPropertySet> xProp;
    OSL_ENSURE(_xTable.is(),"Table is NULL!");
    if ( !_xTable.is() )
        return xProp;

    ::dbtools::OPropertyMap& rPropMap = OMetaConnection::getPropMap();
    Reference<XDatabaseMetaData> xMetaData = _xConnection->getMetaData();
    Any aCatalog;
    aCatalog = _xTable->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_CATALOGNAME));
    ::rtl::OUString sCatalog;
    aCatalog >>= sCatalog;

    ::rtl::OUString aSchema, aTable;
    _xTable->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_SCHEMANAME))  >>= aSchema;
    _xTable->getPropertyValue(rPropMap.getNameByIndex(PROPERTY_ID_NAME))        >>= aTable;

    Reference<XNameAccess> xPrimaryKeyColumns = getPrimaryKeyColumns_throw(_xTable);

    xProp = lcl_createSDBCXColumn(xPrimaryKeyColumns,_xConnection,aCatalog, aSchema, aTable, _rName,_rName,_bCase,_bQueryForInfo,_bIsAutoIncrement,_bIsCurrency,_nDataType);
    if ( !xProp.is() )
    {
        xProp = lcl_createSDBCXColumn(xPrimaryKeyColumns,_xConnection,aCatalog, aSchema, aTable, ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("%")),_rName,_bCase,_bQueryForInfo,_bIsAutoIncrement,_bIsCurrency,_nDataType);
        if ( !xProp.is() )
            xProp = new connectivity::sdbcx::OColumn(_rName,
                                                ::rtl::OUString(),::rtl::OUString(),::rtl::OUString(),
                                                ColumnValue::NULLABLE_UNKNOWN,
                                                0,
                                                0,
                                                DataType::VARCHAR,
                                                _bIsAutoIncrement,
                                                sal_False,
                                                _bIsCurrency,
                                                _bCase,
                                                sCatalog,
                                                aSchema,
                                                aTable);

    }

    return xProp;
}

// -----------------------------------------------------------------------------
bool getBooleanDataSourceSetting( const Reference< XConnection >& _rxConnection, const sal_Char* _pAsciiSettingName )
{
    bool bValue( false );
    try
    {
        Reference< XPropertySet> xDataSourceProperties( findDataSource( _rxConnection ), UNO_QUERY );
        OSL_ENSURE( xDataSourceProperties.is(), "::dbtools::getBooleanDataSourceSetting: somebody is using this with a non-SDB-level connection!" );
        if ( xDataSourceProperties.is() )
        {
            Reference< XPropertySet > xSettings(
                xDataSourceProperties->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Settings") ) ),
                UNO_QUERY_THROW
            );
            OSL_VERIFY( xSettings->getPropertyValue( ::rtl::OUString::createFromAscii( _pAsciiSettingName ) ) >>= bValue );
        }
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }
    return bValue;
}
// -------------------------------------------------------------------------
bool getDataSourceSetting( const Reference< XInterface >& _xChild, const ::rtl::OUString& _sAsciiSettingsName,
    Any& /* [out] */ _rSettingsValue )
{
    bool bIsPresent = false;
    try
    {
        const Reference< XPropertySet> xDataSourceProperties( findDataSource( _xChild ), UNO_QUERY );
        if ( !xDataSourceProperties.is() )
            return false;

        const Reference< XPropertySet > xSettings(
                xDataSourceProperties->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Settings") ) ),
                UNO_QUERY_THROW
            );

        _rSettingsValue = xSettings->getPropertyValue( _sAsciiSettingsName );
        bIsPresent = true;
    }
    catch( const Exception& )
    {
        bIsPresent = false;
    }
    return bIsPresent;
}
// -------------------------------------------------------------------------
bool getDataSourceSetting( const Reference< XInterface >& _rxDataSource, const sal_Char* _pAsciiSettingsName,
    Any& /* [out] */ _rSettingsValue )
{
    ::rtl::OUString sAsciiSettingsName = ::rtl::OUString::createFromAscii(_pAsciiSettingsName);
    return getDataSourceSetting( _rxDataSource, sAsciiSettingsName,_rSettingsValue );
}
// -----------------------------------------------------------------------------
sal_Bool isDataSourcePropertyEnabled(const Reference<XInterface>& _xProp,const ::rtl::OUString& _sProperty,sal_Bool _bDefault)
{
    sal_Bool bEnabled = _bDefault;
    try
    {
        Reference< XPropertySet> xProp(findDataSource(_xProp),UNO_QUERY);
        if ( xProp.is() )
        {
            Sequence< PropertyValue > aInfo;
            xProp->getPropertyValue(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Info"))) >>= aInfo;
            const PropertyValue* pValue =::std::find_if(aInfo.getConstArray(),
                                                aInfo.getConstArray() + aInfo.getLength(),
                                                ::std::bind2nd(TPropertyValueEqualFunctor(),_sProperty));
            if ( pValue && pValue != (aInfo.getConstArray() + aInfo.getLength()) )
                pValue->Value >>= bEnabled;
        }
    }
    catch(SQLException&)
    {
        DBG_UNHANDLED_EXCEPTION();
    }
    return bEnabled;
}
// -----------------------------------------------------------------------------
Reference< XTablesSupplier> getDataDefinitionByURLAndConnection(
            const ::rtl::OUString& _rsUrl,
            const Reference< XConnection>& _xConnection,
            const Reference< XMultiServiceFactory>& _rxFactory)
{
    Reference< XTablesSupplier> xTablesSup;
    try
    {
        Reference< XDriverAccess> xManager(
            _rxFactory->createInstance( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.sdbc.DriverManager")) ),
            UNO_QUERY_THROW );
        Reference< XDataDefinitionSupplier > xSupp( xManager->getDriverByURL( _rsUrl ), UNO_QUERY );

        if ( xSupp.is() )
        {
            xTablesSup = xSupp->getDataDefinitionByConnection( _xConnection );
            OSL_ENSURE(xTablesSup.is(),"No table supplier!");
        }
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }
    return xTablesSup;
}

// -----------------------------------------------------------------------------
sal_Int32 getTablePrivileges(const Reference< XDatabaseMetaData>& _xMetaData,
                             const ::rtl::OUString& _sCatalog,
                             const ::rtl::OUString& _sSchema,
                             const ::rtl::OUString& _sTable)
{
    OSL_ENSURE(_xMetaData.is(),"Invalid metadata!");
    sal_Int32 nPrivileges = 0;
    try
    {
        Any aVal;
        if(!_sCatalog.isEmpty())
            aVal <<= _sCatalog;
        Reference< XResultSet > xPrivileges = _xMetaData->getTablePrivileges(aVal, _sSchema, _sTable);
        Reference< XRow > xCurrentRow(xPrivileges, UNO_QUERY);

        const ::rtl::OUString sUserWorkingFor = _xMetaData->getUserName();
        static const ::rtl::OUString sSELECT( RTL_CONSTASCII_USTRINGPARAM( "SELECT" ));
        static const ::rtl::OUString sINSERT( RTL_CONSTASCII_USTRINGPARAM( "INSERT" ));
        static const ::rtl::OUString sUPDATE( RTL_CONSTASCII_USTRINGPARAM( "UPDATE" ));
        static const ::rtl::OUString sDELETE( RTL_CONSTASCII_USTRINGPARAM( "DELETE" ));
        static const ::rtl::OUString sREAD( RTL_CONSTASCII_USTRINGPARAM( "READ" ));
        static const ::rtl::OUString sCREATE( RTL_CONSTASCII_USTRINGPARAM( "CREATE" ));
        static const ::rtl::OUString sALTER( RTL_CONSTASCII_USTRINGPARAM( "ALTER" ));
        static const ::rtl::OUString sREFERENCE( RTL_CONSTASCII_USTRINGPARAM( "REFERENCE" ));
        static const ::rtl::OUString sDROP( RTL_CONSTASCII_USTRINGPARAM( "DROP" ));

        if ( xCurrentRow.is() )
        {
            // after creation the set is positioned before the first record, per definition
#ifdef DBG_UTIL
            Reference< XResultSetMetaDataSupplier > xSup(xPrivileges,UNO_QUERY);
            if ( xSup.is() )
            {
                Reference< XResultSetMetaData > xRsMetaData = xSup->getMetaData();
                if ( xRsMetaData.is() )
                {
                    sal_Int32 nCount = xRsMetaData->getColumnCount();
                    for (sal_Int32 i=1; i<=nCount; ++i)
                    {
                        ::rtl::OUString sColumnName = xRsMetaData->getColumnName(i);
                    }
                }
            }
#endif

            ::rtl::OUString sPrivilege, sGrantee;
            while ( xPrivileges->next() )
            {
#ifdef DBG_UTIL
                ::rtl::OUString sCat, sSchema, sName, sGrantor, sGrantable;
                sCat        = xCurrentRow->getString(1);
                sSchema     = xCurrentRow->getString(2);
                sName       = xCurrentRow->getString(3);
                sGrantor    = xCurrentRow->getString(4);
#endif
                sGrantee    = xCurrentRow->getString(5);
                sPrivilege  = xCurrentRow->getString(6);
#ifdef DBG_UTIL
                sGrantable  = xCurrentRow->getString(7);
#endif

                if (!sUserWorkingFor.equalsIgnoreAsciiCase(sGrantee))
                    continue;

                if (sPrivilege.equalsIgnoreAsciiCase(sSELECT))
                    nPrivileges |= Privilege::SELECT;
                else if (sPrivilege.equalsIgnoreAsciiCase(sINSERT))
                    nPrivileges |= Privilege::INSERT;
                else if (sPrivilege.equalsIgnoreAsciiCase(sUPDATE))
                    nPrivileges |= Privilege::UPDATE;
                else if (sPrivilege.equalsIgnoreAsciiCase(sDELETE))
                    nPrivileges |= Privilege::DELETE;
                else if (sPrivilege.equalsIgnoreAsciiCase(sREAD))
                    nPrivileges |= Privilege::READ;
                else if (sPrivilege.equalsIgnoreAsciiCase(sCREATE))
                    nPrivileges |= Privilege::CREATE;
                else if (sPrivilege.equalsIgnoreAsciiCase(sALTER))
                    nPrivileges |= Privilege::ALTER;
                else if (sPrivilege.equalsIgnoreAsciiCase(sREFERENCE))
                    nPrivileges |= Privilege::REFERENCE;
                else if (sPrivilege.equalsIgnoreAsciiCase(sDROP))
                    nPrivileges |= Privilege::DROP;
            }
        }
        disposeComponent(xPrivileges);

        // Some drivers put a table privilege as soon as any column has the privilege,
        // some drivers only if all columns have the privilege.
        // To unifiy the situation, collect column privileges here, too.
        Reference< XResultSet > xColumnPrivileges = _xMetaData->getColumnPrivileges(aVal, _sSchema, _sTable, ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("%")));
        Reference< XRow > xColumnCurrentRow(xColumnPrivileges, UNO_QUERY);
        if ( xColumnCurrentRow.is() )
        {
            // after creation the set is positioned before the first record, per definition
            ::rtl::OUString sPrivilege, sGrantee;
            while ( xColumnPrivileges->next() )
            {
#ifdef DBG_UTIL
                ::rtl::OUString sCat, sSchema, sTableName, sColumnName, sGrantor, sGrantable;
                sCat        = xColumnCurrentRow->getString(1);
                sSchema     = xColumnCurrentRow->getString(2);
                sTableName  = xColumnCurrentRow->getString(3);
                sColumnName = xColumnCurrentRow->getString(4);
                sGrantor    = xColumnCurrentRow->getString(5);
#endif
                sGrantee    = xColumnCurrentRow->getString(6);
                sPrivilege  = xColumnCurrentRow->getString(7);
#ifdef DBG_UTIL
                sGrantable  = xColumnCurrentRow->getString(8);
#endif

                if (!sUserWorkingFor.equalsIgnoreAsciiCase(sGrantee))
                    continue;

                if (sPrivilege.equalsIgnoreAsciiCase(sSELECT))
                    nPrivileges |= Privilege::SELECT;
                else if (sPrivilege.equalsIgnoreAsciiCase(sINSERT))
                    nPrivileges |= Privilege::INSERT;
                else if (sPrivilege.equalsIgnoreAsciiCase(sUPDATE))
                    nPrivileges |= Privilege::UPDATE;
                else if (sPrivilege.equalsIgnoreAsciiCase(sDELETE))
                    nPrivileges |= Privilege::DELETE;
                else if (sPrivilege.equalsIgnoreAsciiCase(sREAD))
                    nPrivileges |= Privilege::READ;
                else if (sPrivilege.equalsIgnoreAsciiCase(sCREATE))
                    nPrivileges |= Privilege::CREATE;
                else if (sPrivilege.equalsIgnoreAsciiCase(sALTER))
                    nPrivileges |= Privilege::ALTER;
                else if (sPrivilege.equalsIgnoreAsciiCase(sREFERENCE))
                    nPrivileges |= Privilege::REFERENCE;
                else if (sPrivilege.equalsIgnoreAsciiCase(sDROP))
                    nPrivileges |= Privilege::DROP;
            }
        }
        disposeComponent(xColumnPrivileges);
    }
    catch(const SQLException& e)
    {
        static ::rtl::OUString sNotSupportedState( RTL_CONSTASCII_USTRINGPARAM( "IM001" ));
        // some drivers don't support any privileges so we assume that we are allowed to do all we want :-)
        if(e.SQLState == sNotSupportedState)
            nPrivileges |=  Privilege::DROP         |
                            Privilege::REFERENCE    |
                            Privilege::ALTER        |
                            Privilege::CREATE       |
                            Privilege::READ         |
                            Privilege::DELETE       |
                            Privilege::UPDATE       |
                            Privilege::INSERT       |
                            Privilege::SELECT;
        else
            OSL_FAIL("Could not collect the privileges !");
    }
    return nPrivileges;
}
// -----------------------------------------------------------------------------
// we need some more information about the column
void collectColumnInformation(const Reference< XConnection>& _xConnection,
                              const ::rtl::OUString& _sComposedName,
                              const ::rtl::OUString& _rName,
                              ColumnInformationMap& _rInfo)
{
    static ::rtl::OUString STR_WHERE = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" WHERE "));

    ::rtl::OUString sSelect = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SELECT "));
    sSelect += _rName;
    sSelect += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" FROM "));
    sSelect += _sComposedName;
    sSelect += STR_WHERE;
    sSelect += ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("0 = 1"));

    try
    {
        ::utl::SharedUNOComponent< XStatement > xStmt( _xConnection->createStatement() );
        Reference< XPropertySet > xStatementProps( xStmt, UNO_QUERY_THROW );
        xStatementProps->setPropertyValue( OMetaConnection::getPropMap().getNameByIndex( PROPERTY_ID_ESCAPEPROCESSING ), makeAny( (sal_Bool)sal_False ) );
        Reference< XResultSet > xResult( xStmt->executeQuery( sSelect ), UNO_QUERY_THROW );
        Reference< XResultSetMetaDataSupplier > xSuppMeta( xResult, UNO_QUERY_THROW );
        Reference< XResultSetMetaData > xMeta( xSuppMeta->getMetaData(), UNO_QUERY_THROW );

        sal_Int32 nCount = xMeta->getColumnCount();
        OSL_ENSURE( nCount != 0, "::dbtools::collectColumnInformation: result set has empty (column-less) meta data!" );
        for (sal_Int32 i=1; i <= nCount ; ++i)
        {
            _rInfo.insert(ColumnInformationMap::value_type(xMeta->getColumnName(i),
                ColumnInformation(TBoolPair(xMeta->isAutoIncrement(i),xMeta->isCurrency(i)),xMeta->getColumnType(i))));
        }
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }
}

// -----------------------------------------------------------------------------
bool isEmbeddedInDatabase( const Reference< XInterface >& _rxComponent, Reference< XConnection >& _rxActualConnection )
{
    bool bIsEmbedded = false;
    try
    {
        Reference< XModel > xModel = lcl_getXModel( _rxComponent );

        if ( xModel.is() )
        {
            Sequence< PropertyValue > aArgs = xModel->getArgs();
            const PropertyValue* pIter = aArgs.getConstArray();
            const PropertyValue* pEnd  = pIter + aArgs.getLength();
            for(;pIter != pEnd;++pIter)
            {
                if ( pIter->Name == "ComponentData" )
                {
                    Sequence<PropertyValue> aDocumentContext;
                    pIter->Value >>= aDocumentContext;
                    const PropertyValue* pContextIter = aDocumentContext.getConstArray();
                    const PropertyValue* pContextEnd  = pContextIter + aDocumentContext.getLength();
                    for(;pContextIter != pContextEnd;++pContextIter)
                    {
                        if (  pContextIter->Name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "ActiveConnection" ) )
                        && ( pContextIter->Value >>= _rxActualConnection )
                        )
                        {
                            bIsEmbedded = true;
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    catch(Exception&)
    {
        // not intereseted in
    }
    return bIsEmbedded;
}
// -----------------------------------------------------------------------------
namespace
{
    ::rtl::OUString lcl_getEncodingName( rtl_TextEncoding _eEncoding )
    {
        ::rtl::OUString sEncodingName;

        OCharsetMap aCharsets;
        OCharsetMap::CharsetIterator aEncodingPos = aCharsets.find( _eEncoding );
        OSL_ENSURE( aEncodingPos != aCharsets.end(), "lcl_getEncodingName: *which* encoding?" );
        if ( aEncodingPos != aCharsets.end() )
            sEncodingName = (*aEncodingPos).getIanaName();

        return sEncodingName;
    }
}

// -----------------------------------------------------------------------------
sal_Int32 DBTypeConversion::convertUnicodeString( const ::rtl::OUString& _rSource, ::rtl::OString& _rDest, rtl_TextEncoding _eEncoding ) SAL_THROW((com::sun::star::sdbc::SQLException))
{
    if ( !rtl_convertUStringToString( &_rDest.pData, _rSource.getStr(), _rSource.getLength(),
            _eEncoding,
            RTL_UNICODETOTEXT_FLAGS_UNDEFINED_ERROR |
            RTL_UNICODETOTEXT_FLAGS_UNDEFINED_REPLACE |
            RTL_UNICODETOTEXT_FLAGS_PRIVATE_MAPTO0 |
            RTL_UNICODETOTEXT_FLAGS_NOCOMPOSITE )
        )
    {
        SharedResources aResources;
        ::rtl::OUString sMessage = aResources.getResourceStringWithSubstitution( STR_CANNOT_CONVERT_STRING,
            "$string$", _rSource,
            "$charset$",  lcl_getEncodingName( _eEncoding )
        );

        throw SQLException(
            sMessage,
            NULL,
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "22018" ) ),
            22018,
            Any()
        );
    }

    return _rDest.getLength();
}

// -----------------------------------------------------------------------------
sal_Int32 DBTypeConversion::convertUnicodeStringToLength( const ::rtl::OUString& _rSource, ::rtl::OString&  _rDest,
   sal_Int32 _nMaxLen, rtl_TextEncoding _eEncoding ) SAL_THROW((SQLException))
{
    sal_Int32 nLen = convertUnicodeString( _rSource, _rDest, _eEncoding );
    if ( nLen > _nMaxLen )
    {
        SharedResources aResources;
        ::rtl::OUString sMessage = aResources.getResourceStringWithSubstitution( STR_STRING_LENGTH_EXCEEDED,
            "$string$", _rSource,
            "$maxlen$", ::rtl::OUString::valueOf( _nMaxLen ),
            "$charset$", lcl_getEncodingName( _eEncoding )
        );

        throw SQLException(
            sMessage,
            NULL,
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "22001" ) ),
            22001,
            Any()
        );
    }

   return nLen;
}
::rtl::OUString lcl_getReportEngines()
{
    static ::rtl::OUString s_sNodeName(RTL_CONSTASCII_USTRINGPARAM("org.openoffice.Office.DataAccess/ReportEngines"));
    return s_sNodeName;
}
// -----------------------------------------------------------------------------
::rtl::OUString lcl_getDefaultReportEngine()
{
    static ::rtl::OUString s_sNodeName(RTL_CONSTASCII_USTRINGPARAM("DefaultReportEngine"));
    return s_sNodeName;
}
// -----------------------------------------------------------------------------
::rtl::OUString lcl_getReportEngineNames()
{
    static ::rtl::OUString s_sNodeName(RTL_CONSTASCII_USTRINGPARAM("ReportEngineNames"));
    return s_sNodeName;
}
// -----------------------------------------------------------------------------
::rtl::OUString getDefaultReportEngineServiceName(const Reference< XMultiServiceFactory >& _rxORB)
{
    ::utl::OConfigurationTreeRoot aReportEngines = ::utl::OConfigurationTreeRoot::createWithServiceFactory(
        _rxORB, lcl_getReportEngines(), -1, ::utl::OConfigurationTreeRoot::CM_READONLY);

    if ( aReportEngines.isValid() )
    {
        ::rtl::OUString sDefaultReportEngineName;
        aReportEngines.getNodeValue(lcl_getDefaultReportEngine()) >>= sDefaultReportEngineName;
        if ( !sDefaultReportEngineName.isEmpty() )
        {
            ::utl::OConfigurationNode aReportEngineNames = aReportEngines.openNode(lcl_getReportEngineNames());
            if ( aReportEngineNames.isValid() )
            {
                ::utl::OConfigurationNode aReportEngine = aReportEngineNames.openNode(sDefaultReportEngineName);
                if ( aReportEngine.isValid() )
                {
                    ::rtl::OUString sRet;
                    const static ::rtl::OUString s_sService(RTL_CONSTASCII_USTRINGPARAM("ServiceName"));
                    aReportEngine.getNodeValue(s_sService) >>= sRet;
                    return sRet;
                }
            }
        }
        else
            return ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.report.pentaho.SOReportJobFactory"));
    }
    else
        return ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.report.pentaho.SOReportJobFactory"));
    return ::rtl::OUString();
}
// -----------------------------------------------------------------------------
//.........................................................................
}   // namespace dbtools
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

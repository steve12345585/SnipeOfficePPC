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

#include "NResultSetMetaData.hxx"
#include "NDatabaseMetaData.hxx"
#include "connectivity/dbexception.hxx"
#include <com/sun/star/sdbc/DataType.hpp>
#include "resource/evoab2_res.hrc"

using namespace connectivity::evoab;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::sdbc;

OEvoabResultSetMetaData::OEvoabResultSetMetaData(const ::rtl::OUString& _aTableName)
    : m_aTableName(_aTableName),
      m_aEvoabFields()
{

}
// -------------------------------------------------------------------------
OEvoabResultSetMetaData::~OEvoabResultSetMetaData()
{
}
// -------------------------------------------------------------------------
void OEvoabResultSetMetaData::setEvoabFields(const ::rtl::Reference<connectivity::OSQLColumns> &xColumns) throw(SQLException)
{
        OSQLColumns::Vector::const_iterator aIter;
        static const ::rtl::OUString aName("Name");

        for (aIter = xColumns->get().begin(); aIter != xColumns->get().end(); ++aIter)
        {
                ::rtl::OUString aFieldName;

                (*aIter)->getPropertyValue(aName) >>= aFieldName;
                guint nFieldNumber = findEvoabField(aFieldName);
                if (nFieldNumber == (guint)-1)
                {
                    connectivity::SharedResources aResource;
                    const ::rtl::OUString sError( aResource.getResourceStringWithSubstitution(
                            STR_INVALID_COLUMNNAME,
                            "$columnname$", aFieldName
                         ) );
                    ::dbtools::throwGenericSQLException( sError, *this );
                }
                m_aEvoabFields.push_back(nFieldNumber);
        }
}

// -------------------------------------------------------------------------
sal_Int32 SAL_CALL OEvoabResultSetMetaData::getColumnDisplaySize( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return 50;
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL OEvoabResultSetMetaData::getColumnType( sal_Int32 nColumnNum ) throw(SQLException, RuntimeException)
{
        sal_uInt32 nField = m_aEvoabFields[nColumnNum - 1];
    return evoab::getFieldType (nField);
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL OEvoabResultSetMetaData::getColumnCount(  ) throw(SQLException, RuntimeException)
{
    return m_aEvoabFields.size();
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OEvoabResultSetMetaData::isCaseSensitive( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return sal_True;
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OEvoabResultSetMetaData::getSchemaName( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return ::rtl::OUString();
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OEvoabResultSetMetaData::getColumnName( sal_Int32 nColumnNum ) throw(SQLException, RuntimeException)
{
    sal_uInt32 nField = m_aEvoabFields[nColumnNum - 1];
    return evoab::getFieldName( nField );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OEvoabResultSetMetaData::getColumnTypeName( sal_Int32 nColumnNum ) throw(SQLException, RuntimeException)
{
    sal_uInt32 nField = m_aEvoabFields[nColumnNum - 1];
    return evoab::getFieldTypeName( nField );
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OEvoabResultSetMetaData::getColumnLabel( sal_Int32 nColumnNum ) throw(SQLException, RuntimeException)
{
    sal_uInt32 nField = m_aEvoabFields[nColumnNum - 1];
    const ColumnProperty *pSpecs = getField(nField);
    GParamSpec *pSpec = pSpecs->pField;
    rtl::OUString aLabel;

    if( pSpec )
            aLabel = rtl::OStringToOUString( g_param_spec_get_nick( (GParamSpec *) pSpec ),
                                             RTL_TEXTENCODING_UTF8 );
    return aLabel;
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OEvoabResultSetMetaData::getColumnServiceName( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return ::rtl::OUString();
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OEvoabResultSetMetaData::getTableName( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return m_aTableName;//::rtl::OUString("TABLE");
}
// -------------------------------------------------------------------------
::rtl::OUString SAL_CALL OEvoabResultSetMetaData::getCatalogName( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return ::rtl::OUString();
}
// -------------------------------------------------------------------------

sal_Bool SAL_CALL OEvoabResultSetMetaData::isCurrency( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return sal_False;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OEvoabResultSetMetaData::isAutoIncrement( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return sal_False;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OEvoabResultSetMetaData::isSigned( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return sal_False;
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL OEvoabResultSetMetaData::getPrecision( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return 0;
}
// -----------------------------------------------------------------------------
sal_Int32 SAL_CALL OEvoabResultSetMetaData::getScale( sal_Int32 /*nColumnNum*/ ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException)
{
    return 0;
}
// -------------------------------------------------------------------------
sal_Int32 SAL_CALL OEvoabResultSetMetaData::isNullable( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return 0;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OEvoabResultSetMetaData::isSearchable( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return sal_True;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OEvoabResultSetMetaData::isReadOnly( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return sal_True;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OEvoabResultSetMetaData::isDefinitelyWritable( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return sal_False;
}
// -------------------------------------------------------------------------
sal_Bool SAL_CALL OEvoabResultSetMetaData::isWritable( sal_Int32 /*nColumnNum*/ ) throw(SQLException, RuntimeException)
{
    return sal_False;
}
// -------------------------------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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
#include "mysqlc_general.hxx"
#include "mysqlc_resultsetmetadata.hxx"

#include <cppconn/exception.h>
#include <cppconn/datatype.h>

using com::sun::star::sdbc::SQLException;

using com::sun::star::uno::UNO_QUERY;
using com::sun::star::uno::Reference;
using com::sun::star::uno::XInterface;
using com::sun::star::uno::Any;
using ::rtl::OUString;

namespace mysqlc_sdbc_driver
{
// -----------------------------------------------------------------------------
void throwFeatureNotImplementedException( const sal_Char* _pAsciiFeatureName, const Reference< XInterface >& _rxContext, const Any* _pNextException )
    throw (SQLException)
{
    const ::rtl::OUString sMessage = ::rtl::OUString::createFromAscii( _pAsciiFeatureName ) + ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ": feature not implemented." ) );
    throw SQLException(
        sMessage,
        _rxContext,
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("HYC00")),
        0,
        _pNextException ? *_pNextException : Any()
    );
}


void throwInvalidArgumentException( const sal_Char* _pAsciiFeatureName, const Reference< XInterface >& _rxContext, const Any* _pNextException )
    throw (SQLException)
{
    const ::rtl::OUString sMessage = ::rtl::OUString::createFromAscii( _pAsciiFeatureName ) + ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ": invalid arguments." ) );
    throw SQLException(
        sMessage,
        _rxContext,
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("HYC00")),
        0,
        _pNextException ? *_pNextException : Any()
    );
}

void translateAndThrow(const ::sql::SQLException& _error, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _context, const rtl_TextEncoding encoding)
{
    throw SQLException(
            convert(_error.what(), encoding),
            _context,
            convert(_error.getSQLState(), encoding),
            _error.getErrorCode(),
            Any()
        );
}


OUString getStringFromAny(const Any& _rAny)
{
    OUString nReturn;
    OSL_VERIFY( _rAny >>= nReturn );
    return nReturn;
}


int mysqlToOOOType(int cppConnType)
    throw ()
{
    switch (cppConnType) {
        case sql::DataType::BIT:
            return com::sun::star::sdbc::DataType::VARCHAR;

        case sql::DataType::TINYINT:
            return com::sun::star::sdbc::DataType::TINYINT;

        case sql::DataType::SMALLINT:
            return com::sun::star::sdbc::DataType::SMALLINT;

        case sql::DataType::INTEGER:
            return com::sun::star::sdbc::DataType::INTEGER;

        case sql::DataType::BIGINT:
            return com::sun::star::sdbc::DataType::BIGINT;

        case sql::DataType::REAL:
            return com::sun::star::sdbc::DataType::REAL;

        case sql::DataType::DOUBLE:
            return com::sun::star::sdbc::DataType::DOUBLE;

        case sql::DataType::DECIMAL:
            return com::sun::star::sdbc::DataType::DECIMAL;

        case sql::DataType::CHAR:
            return com::sun::star::sdbc::DataType::CHAR;

        case sql::DataType::BINARY:
            return com::sun::star::sdbc::DataType::BINARY;

        case sql::DataType::ENUM:
        case sql::DataType::SET:
        case sql::DataType::VARCHAR:
            return com::sun::star::sdbc::DataType::VARCHAR;

        case sql::DataType::VARBINARY:
            return com::sun::star::sdbc::DataType::VARBINARY;

        case sql::DataType::LONGVARCHAR:
            return com::sun::star::sdbc::DataType::LONGVARCHAR;

        case sql::DataType::LONGVARBINARY:
            return com::sun::star::sdbc::DataType::LONGVARBINARY;

        case sql::DataType::TIMESTAMP:
            return com::sun::star::sdbc::DataType::TIMESTAMP;

        case sql::DataType::DATE:
            return com::sun::star::sdbc::DataType::DATE;

        case sql::DataType::TIME:
            return com::sun::star::sdbc::DataType::TIME;

        case sql::DataType::GEOMETRY:
            return com::sun::star::sdbc::DataType::VARCHAR;

        case sql::DataType::SQLNULL:
            return com::sun::star::sdbc::DataType::SQLNULL;

        case sql::DataType::UNKNOWN:
            return com::sun::star::sdbc::DataType::VARCHAR;
    }

    OSL_FAIL( "mysqlToOOOType: unhandled case, falling back to VARCHAR" );
    return com::sun::star::sdbc::DataType::VARCHAR;
}


::rtl::OUString convert(const ::std::string& _string, const rtl_TextEncoding encoding)
{
    return ::rtl::OUString( _string.c_str(), _string.size(), encoding );
}

::std::string convert(const ::rtl::OUString& _string, const rtl_TextEncoding encoding)
{
    return ::std::string( ::rtl::OUStringToOString( _string, encoding ).getStr() );
}


} /* namespace */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

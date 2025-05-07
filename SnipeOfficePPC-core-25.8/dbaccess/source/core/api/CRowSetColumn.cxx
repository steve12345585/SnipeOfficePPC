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


#include "dbastrings.hrc"
#include "apitools.hxx"
#include "CRowSetColumn.hxx"

#include <com/sun/star/sdb/XColumn.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>

#include <comphelper/types.hxx>
#include <cppuhelper/typeprovider.hxx>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::sdb;
using namespace ::com::sun::star::beans;

namespace dbaccess
{

ORowSetColumn::ORowSetColumn(   const Reference < XResultSetMetaData >& _xMetaData, const Reference < XRow >& _xRow, sal_Int32 _nPos,
                const Reference< XDatabaseMetaData >& _rxDBMeta, const ::rtl::OUString& _rDescription, const ::rtl::OUString& i_sLabel,ORowSetCacheIterator& _rColumnValue )
    :ORowSetDataColumn( _xMetaData, _xRow, NULL, _nPos, _rxDBMeta, _rDescription, i_sLabel,_rColumnValue )
{
}

::cppu::IPropertyArrayHelper* ORowSetColumn::createArrayHelper( ) const
{
    BEGIN_PROPERTY_SEQUENCE(21)

    DECL_PROP1( CATALOGNAME,                ::rtl::OUString,    READONLY );
    DECL_PROP1( DISPLAYSIZE,                sal_Int32,          READONLY );
    DECL_PROP1_BOOL( ISAUTOINCREMENT,                           READONLY );
    DECL_PROP1_BOOL( ISCASESENSITIVE,                           READONLY );
    DECL_PROP1_BOOL( ISCURRENCY,                                READONLY );
    DECL_PROP1_BOOL( ISDEFINITELYWRITABLE,                      READONLY );
    DECL_PROP1( ISNULLABLE,                 sal_Int32,          READONLY );
    DECL_PROP1_BOOL( ISREADONLY,                                BOUND );
    DECL_PROP1_BOOL( ISROWVERSION,                              READONLY );
    DECL_PROP1_BOOL( ISSEARCHABLE,                              READONLY );
    DECL_PROP1_BOOL( ISSIGNED,                                  READONLY );
    DECL_PROP1_BOOL( ISWRITABLE,                                READONLY );
    DECL_PROP1( LABEL,                      ::rtl::OUString,    READONLY );
    DECL_PROP1( PRECISION,                  sal_Int32,          READONLY );
    DECL_PROP1( SCALE,                      sal_Int32,          READONLY );
    DECL_PROP1( SCHEMANAME,                 ::rtl::OUString,    READONLY );
    DECL_PROP1( SERVICENAME,                ::rtl::OUString,    READONLY );
    DECL_PROP1( TABLENAME,                  ::rtl::OUString,    READONLY );
    DECL_PROP1( TYPE,                       sal_Int32,          READONLY );
    DECL_PROP1( TYPENAME,                   ::rtl::OUString,    READONLY );
    DECL_PROP2( VALUE,                      Any,                READONLY, BOUND );

    END_PROPERTY_SEQUENCE()

    Sequence< Property > aRegisteredProperties;
    describeProperties( aRegisteredProperties );

    return new ::cppu::OPropertyArrayHelper( ::comphelper::concatSequences( aDescriptor, aRegisteredProperties ), sal_False );
}

::cppu::IPropertyArrayHelper& ORowSetColumn::getInfoHelper()
{
    return *static_cast< ::comphelper::OPropertyArrayUsageHelper< ORowSetColumn >* >(this)->getArrayHelper();
}

void SAL_CALL ORowSetColumn::setFastPropertyValue_NoBroadcast(sal_Int32 nHandle,const Any& rValue )throw (Exception)
{
    OSL_ENSURE( nHandle != PROPERTY_ID_VALUE, "ORowSetColumn::setFastPropertyValue_NoBroadcast: hmm? This property is marked as READONLY!" );
    if ( nHandle != PROPERTY_ID_VALUE )
        ORowSetDataColumn::setFastPropertyValue_NoBroadcast( nHandle, rValue );
}

}   // namespace dbaccess
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

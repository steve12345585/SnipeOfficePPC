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

#include "mysql/YCatalog.hxx"
#include "mysql/YUsers.hxx"
#include "mysql/YTables.hxx"
#include "mysql/YViews.hxx"
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbc/XResultSet.hpp>
#include <comphelper/types.hxx>


// -------------------------------------------------------------------------
using namespace connectivity;
using namespace connectivity::mysql;
using namespace connectivity::sdbcx;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
// -------------------------------------------------------------------------
OMySQLCatalog::OMySQLCatalog(const Reference< XConnection >& _xConnection) : OCatalog(_xConnection)
                ,m_xConnection(_xConnection)
{
}
// -----------------------------------------------------------------------------
void OMySQLCatalog::refreshObjects(const Sequence< ::rtl::OUString >& _sKindOfObject,TStringVector& _rNames)
{
    Reference< XResultSet > xResult = m_xMetaData->getTables(Any(),
                                                            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("%")),
                                                            ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("%")),
                                                            _sKindOfObject);
    fillNames(xResult,_rNames);
}
// -------------------------------------------------------------------------
void OMySQLCatalog::refreshTables()
{
    TStringVector aVector;
    static const ::rtl::OUString s_sTableTypeView(RTL_CONSTASCII_USTRINGPARAM("VIEW"));
    static const ::rtl::OUString s_sTableTypeTable(RTL_CONSTASCII_USTRINGPARAM("TABLE"));
    static const ::rtl::OUString s_sAll(RTL_CONSTASCII_USTRINGPARAM("%"));

    Sequence< ::rtl::OUString > sTableTypes(3);
    sTableTypes[0] = s_sTableTypeView;
    sTableTypes[1] = s_sTableTypeTable;
    sTableTypes[2] = s_sAll;    // just to be sure to include anything else ....

    refreshObjects(sTableTypes,aVector);

    if ( m_pTables )
        m_pTables->reFill(aVector);
    else
        m_pTables = new OTables(m_xMetaData,*this,m_aMutex,aVector);
}
// -------------------------------------------------------------------------
void OMySQLCatalog::refreshViews()
{
    Sequence< ::rtl::OUString > aTypes(1);
    aTypes[0] = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("VIEW"));

    // let's simply assume the server is new enough to support views. Current drivers
    // as of this writing might not return the proper information in getTableTypes, so
    // don't rely on it.
    // during #73245# / 2007-10-26 / frank.schoenheit@sun.com
    bool bSupportsViews = sal_True;

    TStringVector aVector;
    if ( bSupportsViews )
        refreshObjects(aTypes,aVector);

    if ( m_pViews )
        m_pViews->reFill(aVector);
    else
        m_pViews = new OViews(m_xMetaData,*this,m_aMutex,aVector);
}
// -------------------------------------------------------------------------
void OMySQLCatalog::refreshGroups()
{
}
// -------------------------------------------------------------------------
void OMySQLCatalog::refreshUsers()
{
    TStringVector aVector;
    Reference< XStatement > xStmt = m_xConnection->createStatement(  );
    Reference< XResultSet >  xResult = xStmt->executeQuery(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("select User from mysql.user group by User")));
    if ( xResult.is() )
    {
        Reference< XRow > xRow(xResult,UNO_QUERY);
        TString2IntMap aMap;
        while( xResult->next() )
            aVector.push_back(xRow->getString(1));
        ::comphelper::disposeComponent(xResult);
    }
    ::comphelper::disposeComponent(xStmt);

    if(m_pUsers)
        m_pUsers->reFill(aVector);
    else
        m_pUsers = new OUsers(*this,m_aMutex,aVector,m_xConnection,this);
}
// -----------------------------------------------------------------------------
Any SAL_CALL OMySQLCatalog::queryInterface( const Type & rType ) throw(RuntimeException)
{
    if ( rType == ::getCppuType((const Reference<XGroupsSupplier>*)0) )
        return Any();


    return OCatalog::queryInterface(rType);
}
// -----------------------------------------------------------------------------
Sequence< Type > SAL_CALL OMySQLCatalog::getTypes(  ) throw(RuntimeException)
{
    Sequence< Type > aTypes = OCatalog::getTypes();
    ::std::vector<Type> aOwnTypes;
    aOwnTypes.reserve(aTypes.getLength());
    const Type* pBegin = aTypes.getConstArray();
    const Type* pEnd = pBegin + aTypes.getLength();
    for(;pBegin != pEnd;++pBegin)
    {
        if ( !(*pBegin == ::getCppuType((const Reference<XGroupsSupplier>*)0)))
        {
            aOwnTypes.push_back(*pBegin);
        }
    }
    const Type* pTypes = aOwnTypes.empty() ? 0 : &aOwnTypes[0];
    return Sequence< Type >(pTypes, aOwnTypes.size());
}
// -----------------------------------------------------------------------------


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

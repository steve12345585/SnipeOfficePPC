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


#include "KTables.hxx"
#include "KTable.hxx"
#include "KCatalog.hxx"
#include "KConnection.hxx"
#include <comphelper/types.hxx>

using namespace connectivity::kab;
using namespace connectivity;
using namespace ::comphelper;
using namespace ::cppu;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;

sdbcx::ObjectType KabTables::createObject(const ::rtl::OUString& _rName)
{
    ::rtl::OUString aName,aSchema;
    aSchema = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("%"));
    aName = _rName;

    Sequence< ::rtl::OUString > aTypes(1);
    aTypes[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("%"));
    ::rtl::OUString sEmpty;

    Reference< XResultSet > xResult = m_xMetaData->getTables(Any(), aSchema, aName, aTypes);

    sdbcx::ObjectType xRet = NULL;
    if (xResult.is())
    {
        Reference< XRow > xRow(xResult, UNO_QUERY);
        if (xResult->next()) // there can be only one table with this name
        {
            KabTable* pRet = new KabTable(
                    this,
                    static_cast<KabCatalog&>(m_rParent).getConnection(),
                    aName,
                    xRow->getString(4),
                    xRow->getString(5),
                    sEmpty);
            xRet = pRet;
        }
    }
    ::comphelper::disposeComponent(xResult);

    return xRet;
}
// -------------------------------------------------------------------------
void KabTables::impl_refresh(  ) throw(RuntimeException)
{
    static_cast<KabCatalog&>(m_rParent).refreshTables();
}
// -------------------------------------------------------------------------
void KabTables::disposing(void)
{
m_xMetaData.clear();
    OCollection::disposing();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

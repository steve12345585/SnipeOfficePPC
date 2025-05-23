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

#ifndef CONNECTIVITY_TKEY_HXX
#define CONNECTIVITY_TKEY_HXX

#include "connectivity/dbtoolsdllapi.hxx"
#include <connectivity/sdbcx/VKey.hxx>
#include <com/sun/star/sdbc/XDatabaseMetaData.hpp>

namespace connectivity
{
    class OTableHelper;
    class OOO_DLLPUBLIC_DBTOOLS OTableKeyHelper : public connectivity::sdbcx::OKey
    {
        OTableHelper*   m_pTable;
    public:
        virtual void refreshColumns();
    public:
        OTableKeyHelper(    OTableHelper* _pTable);
        OTableKeyHelper(    OTableHelper* _pTable
                ,const ::rtl::OUString& _Name
                ,const sdbcx::TKeyProperties& _rProps
            );
        inline OTableHelper* getTable() const { return m_pTable; }
    };
}
#endif // CONNECTIVITY_TKEY_HXX


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

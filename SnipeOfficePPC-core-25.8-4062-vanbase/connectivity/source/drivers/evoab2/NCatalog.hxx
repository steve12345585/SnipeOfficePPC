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

#ifndef _CONNECTIVITY_EVOAB_CATALOG_HXX_
#define _CONNECTIVITY_EVOAB_CATALOG_HXX_

#include "connectivity/sdbcx/VCatalog.hxx"

namespace connectivity
{
    namespace evoab
    {
        class OEvoabConnection;
        class OEvoabCatalog : public connectivity::sdbcx::OCatalog
        {
            OEvoabConnection *m_pConnection;
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData > m_xMetaData;
        public:
            OEvoabCatalog(OEvoabConnection *_pCon);
            inline OEvoabConnection* getConnection() const { return m_pConnection; }
            virtual void refreshTables();
            virtual void refreshViews() {}
            virtual void refreshGroups() {}
            virtual void refreshUsers() {}
 // XTablesSupplier
                        virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > SAL_CALL getTables(
                                        ) throw(::com::sun::star::uno::RuntimeException);
        };
    }
}
#endif // _CONNECTIVITY_MOZAB_CATALOG_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

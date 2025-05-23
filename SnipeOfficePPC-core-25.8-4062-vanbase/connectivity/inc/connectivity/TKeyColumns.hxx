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

#ifndef CONNECTIVITY_TKEYCOLUMNS_HXX
#define CONNECTIVITY_TKEYCOLUMNS_HXX

#include "connectivity/sdbcx/VCollection.hxx"
#include "connectivity/TKey.hxx"
#include "connectivity/dbtoolsdllapi.hxx"

namespace connectivity
{
    class OOO_DLLPUBLIC_DBTOOLS OKeyColumnsHelper : public connectivity::sdbcx::OCollection
    {
        OTableKeyHelper* m_pKey;
    protected:
        virtual sdbcx::ObjectType createObject(const ::rtl::OUString& _rName);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > createDescriptor();
        virtual void impl_refresh() throw(::com::sun::star::uno::RuntimeException);
    public:
        OKeyColumnsHelper(  OTableKeyHelper* _pKey,
                        ::osl::Mutex& _rMutex,
                        const ::std::vector< ::rtl::OUString> &_rVector);
    };
}
#endif // CONNECTIVITY_TKEYCOLUMNS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

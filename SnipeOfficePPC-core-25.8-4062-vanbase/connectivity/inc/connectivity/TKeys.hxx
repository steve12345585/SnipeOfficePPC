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

#ifndef CONNECTIVITY_TKEYS_HXX
#define CONNECTIVITY_TKEYS_HXX

#include "connectivity/sdbcx/VCollection.hxx"
#include <com/sun/star/sdbc/XDatabaseMetaData.hpp>
#include "connectivity/TTableHelper.hxx"
#include "connectivity/dbtoolsdllapi.hxx"

namespace connectivity
{
    typedef sdbcx::OCollection OKeys_BASE;
    class OOO_DLLPUBLIC_DBTOOLS OKeysHelper : public OKeys_BASE
    {
        OTableHelper*       m_pTable;
    protected:
        virtual sdbcx::ObjectType createObject(const ::rtl::OUString& _rName);
        virtual void impl_refresh() throw(::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > createDescriptor();
        virtual sdbcx::ObjectType appendObject( const ::rtl::OUString& _rForName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor );
        virtual void dropObject(sal_Int32 _nPos,const ::rtl::OUString _sElementName);

        virtual ::rtl::OUString getDropForeignKey() const;

    public:
        OKeysHelper(    OTableHelper* _pTable,
                ::osl::Mutex& _rMutex,
                const TStringVector& _rVector
                );
        inline OTableHelper* getTable() const { return m_pTable; }

        static void cloneDescriptorColumns(
            const sdbcx::ObjectType& _rSourceDescriptor,
            const sdbcx::ObjectType& _rDestDescriptor
        );
    };
}

#endif // CONNECTIVITY_TKEYS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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
#ifndef CONNECTIVITY_MYSQL_COLUMN_HXX
#define CONNECTIVITY_MYSQL_COLUMN_HXX
#include "connectivity/TColumnsHelper.hxx"
#include "connectivity/sdbcx/VColumn.hxx"

namespace connectivity
{
    namespace mysql
    {
        class OMySQLColumns : public OColumnsHelper
        {
        protected:
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > createDescriptor();
        public:
            OMySQLColumns(  ::cppu::OWeakObject& _rParent
                            ,sal_Bool _bCase
                            ,::osl::Mutex& _rMutex
                            ,const TStringVector &_rVector
                            ,sal_Bool _bUseHardRef = sal_True
                        );
        };

        class OMySQLColumn;
        typedef sdbcx::OColumn OMySQLColumn_BASE;
        typedef ::comphelper::OIdPropertyArrayUsageHelper<OMySQLColumn> OMySQLColumn_PROP;

        class OMySQLColumn :    public OMySQLColumn_BASE,
                                public OMySQLColumn_PROP
        {
            ::rtl::OUString m_sAutoIncrement;
        protected:
            virtual ::cppu::IPropertyArrayHelper* createArrayHelper( sal_Int32 _nId) const;
            virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();

        public:
            OMySQLColumn(sal_Bool _bCase);
            virtual void construct();

            virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);
        };
    }
}
#endif // CONNECTIVITY_MYSQL_COLUMN_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

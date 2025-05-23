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

#ifndef _CONNECTIVITY_SDBCX_VIEW_HXX_
#define _CONNECTIVITY_SDBCX_VIEW_HXX_

#include <osl/diagnose.h>
#include <com/sun/star/sdbcx/XDataDescriptorFactory.hpp>
#include <com/sun/star/sdbc/XDatabaseMetaData.hpp>
#include <comphelper/proparrhlp.hxx>
#include <cppuhelper/compbase1.hxx>
#include "connectivity/CommonTools.hxx"
#include <cppuhelper/interfacecontainer.h>
#include <com/sun/star/container/XNamed.hpp>
#include "connectivity/sdbcx/VDescriptor.hxx"
#include "connectivity/dbtoolsdllapi.hxx"
#include <comphelper/IdPropArrayHelper.hxx>
#include <cppuhelper/implbase2.hxx>

namespace connectivity
{
    namespace sdbcx
    {

        typedef ::cppu::WeakImplHelper2<    ::com::sun::star::lang::XServiceInfo,
                                        ::com::sun::star::container::XNamed> OView_BASE;



        class OOO_DLLPUBLIC_DBTOOLS OView :
                        public ::comphelper::OMutexAndBroadcastHelper,
                        public OView_BASE,
                        public ::comphelper::OIdPropertyArrayUsageHelper<OView>,
                        public ODescriptor
        {
        protected:
            ::rtl::OUString m_CatalogName;
            ::rtl::OUString m_SchemaName;
            ::rtl::OUString m_Command;
            sal_Int32       m_CheckOption;
            // need for the getName method
            ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData >       m_xMetaData;

            // OPropertyArrayUsageHelper
            virtual ::cppu::IPropertyArrayHelper* createArrayHelper( sal_Int32 _nId) const;
            // OPropertySetHelper
            virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();

        public:
            DECLARE_SERVICE_INFO();

            OView(sal_Bool _bCase,const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData >& _xMetaData);
            OView(  sal_Bool _bCase,
                    const ::rtl::OUString& _rName,
                    const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData >& _xMetaData,
                    sal_Int32 _nCheckOption = 0,
                    const ::rtl::OUString& _rCommand = ::rtl::OUString(),
                    const ::rtl::OUString& _rSchemaName = ::rtl::OUString(),
                    const ::rtl::OUString& _rCatalogName = ::rtl::OUString());
            virtual ~OView();

            // ODescriptor
            virtual void construct();

            // ::cppu::OComponentHelper
            virtual void SAL_CALL disposing(void);

            // XInterface
            virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL acquire() throw();
            virtual void SAL_CALL release() throw();
            //XTypeProvider
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);
            // XPropertySet
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);
            // XNamed
            virtual ::rtl::OUString SAL_CALL getName(  ) throw(::com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setName( const ::rtl::OUString& ) throw(::com::sun::star::uno::RuntimeException);
        };
    }
}

#endif // _CONNECTIVITY_SDBCX_VIEW_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

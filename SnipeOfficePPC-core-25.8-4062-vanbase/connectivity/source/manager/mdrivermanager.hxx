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

#ifndef _CONNECTIVITY_DRIVERMANAGER_HXX_
#define _CONNECTIVITY_DRIVERMANAGER_HXX_

#include <com/sun/star/sdbc/XDriverManager2.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/XNamingService.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>

#include <cppuhelper/implbase3.hxx>
#include <comphelper/stl_types.hxx>
#include <comphelper/logging.hxx>
#include <comphelper/componentcontext.hxx>
#include <osl/mutex.hxx>
#include "connectivity/DriversConfig.hxx"

namespace drivermanager
{

    //======================================================================
    //= various
    //======================================================================
    typedef ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDriver >                     SdbcDriver;
    DECLARE_STL_USTRINGACCESS_MAP( SdbcDriver, DriverCollection );

    typedef ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory >       DriverFactory;
    struct DriverAccess
    {
        ::rtl::OUString     sImplementationName;        /// the implementation name of the driver
        DriverFactory       xComponentFactory;          /// the factory to create the driver component (if not already done so)
        SdbcDriver          xDriver;                    /// the driver itself
    };

    //==========================================================================
    //= OSDBCDriverManager - the one-instance service for managing SDBC drivers
    //==========================================================================
    typedef ::cppu::WeakImplHelper3 <   ::com::sun::star::sdbc::XDriverManager2
                                    ,   ::com::sun::star::lang::XServiceInfo
                                    ,   ::com::sun::star::uno::XNamingService
                                    >   OSDBCDriverManager_Base;

    class OSDBCDriverManager : public OSDBCDriverManager_Base
    {
        friend class ODriverEnumeration;

        ::osl::Mutex                    m_aMutex;
        ::comphelper::ComponentContext  m_aContext;
        ::comphelper::EventLogger       m_aEventLogger;

        DECLARE_STL_VECTOR(DriverAccess, DriverAccessArray);
        DriverAccessArray               m_aDriversBS;

        // for drivers registered at runtime (not bootstrapped) we don't require an XServiceInfo interface,
        // so we have to remember their impl-name in another way
        DECLARE_STL_USTRINGACCESS_MAP(SdbcDriver, DriverCollection);
        DriverCollection                m_aDriversRT;

        ::connectivity::DriversConfig   m_aDriverConfig;
        sal_Int32                       m_nLoginTimeout;

    private:
        OSDBCDriverManager(
            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& _rxContext );
        ~OSDBCDriverManager();

    public:

    // XDriverManager
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getConnection( const ::rtl::OUString& url ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getConnectionWithInfo( const ::rtl::OUString& url, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& info ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setLoginTimeout( sal_Int32 seconds ) throw(::com::sun::star::uno::RuntimeException);
        virtual sal_Int32 SAL_CALL getLoginTimeout(  ) throw(::com::sun::star::uno::RuntimeException);

    // XDriverAccess
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDriver > SAL_CALL getDriverByURL( const ::rtl::OUString& url ) throw(::com::sun::star::uno::RuntimeException);

    // XEnumerationAccess
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL createEnumeration(  ) throw(::com::sun::star::uno::RuntimeException);

    // XElementAccess
        virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  ) throw(::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL hasElements(  ) throw(::com::sun::star::uno::RuntimeException);

    // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);

    // XServiceInfo - static methods
        static ::rtl::OUString SAL_CALL getImplementationName_static(  ) throw(::com::sun::star::uno::RuntimeException);
        static ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_static(  ) throw(::com::sun::star::uno::RuntimeException);
        static ::rtl::OUString SAL_CALL getSingletonName_static(  ) throw(::com::sun::star::uno::RuntimeException);
        static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL Create( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxContext );

    // XNamingService
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getRegisteredObject( const ::rtl::OUString& Name ) throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL registerObject( const ::rtl::OUString& Name, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& Object ) throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL revokeObject( const ::rtl::OUString& Name ) throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

    protected:
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDriver > implGetDriverForURL(const ::rtl::OUString& _rURL);

        /** retrieve the driver order preferences from the configuration and
            sort m_aDriversBS accordingly.
        */
        void initializeDriverPrecedence();

        void bootstrapDrivers();
    };

}   // namespace drivermanager

#endif // _CONNECTIVITY_DRIVERMANAGER_HXX_


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

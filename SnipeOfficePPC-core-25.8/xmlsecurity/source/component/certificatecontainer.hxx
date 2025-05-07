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

#ifndef _XCERTIFICATECONTAINER_HXX_
#define _XCERTIFICATECONTAINER_HXX_

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <cppuhelper/factory.hxx>
#include <cppuhelper/implbase2.hxx>

#include <com/sun/star/security/XCertificateContainer.hpp>

#include <com/sun/star/security/CertificateContainerStatus.hpp>


#include <vector>
#include <map>

using namespace com::sun::star;
using namespace cppu;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;

class CertificateContainer : public ::cppu::WeakImplHelper2< ::com::sun::star::lang::XServiceInfo, ::com::sun::star::security::XCertificateContainer >
{
    private:
        typedef std::map< ::rtl::OUString, ::rtl::OUString > Map;
        Map certMap;
        Map certTrustMap;

        ::sal_Bool SAL_CALL searchMap( const ::rtl::OUString & url, const ::rtl::OUString & certificate_name, Map &_certMap  );
        virtual ::sal_Bool SAL_CALL isTemporaryCertificate( const ::rtl::OUString & url, const ::rtl::OUString & certificate_name ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::sal_Bool SAL_CALL isCertificateTrust( const ::rtl::OUString & url, const ::rtl::OUString & certificate_name ) throw(::com::sun::star::uno::RuntimeException);

    public:

        CertificateContainer(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& ) {};
        virtual ~CertificateContainer(){};

        virtual ::sal_Bool SAL_CALL addCertificate( const ::rtl::OUString & url, const ::rtl::OUString & certificate_name, ::sal_Bool trust ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::security::CertificateContainerStatus SAL_CALL hasCertificate( const ::rtl::OUString & url, const ::rtl::OUString & certificate_name ) throw(::com::sun::star::uno::RuntimeException);
        // provide factory
    static ::rtl::OUString SAL_CALL
                                        impl_getStaticImplementationName( ) throw(::com::sun::star::uno::RuntimeException);

    static ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
                    impl_getStaticSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);

    static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
                    impl_createInstance( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager ) throw( ::com::sun::star::uno::RuntimeException );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL
                                        getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::sal_Bool SAL_CALL
                                        supportsService( const ::rtl::OUString& ServiceName ) throw(::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
                                        getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);

};



#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

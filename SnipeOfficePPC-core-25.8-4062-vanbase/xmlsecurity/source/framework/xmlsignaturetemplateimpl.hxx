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

#ifndef _XMLSIGNATURETEMPLATEIMPL_HXX_
#define _XMLSIGNATURETEMPLATEIMPL_HXX_

#include <sal/config.h>
#include <rtl/ustring.hxx>
#include <cppuhelper/factory.hxx>
#include <cppuhelper/implbase3.hxx>
#include <com/sun/star/uno/Exception.hpp>

#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/xml/crypto/XXMLSignatureTemplate.hpp>
#include <com/sun/star/xml/wrapper/XXMLElementWrapper.hpp>

#include <vector>

class XMLSignatureTemplateImpl : public ::cppu::WeakImplHelper3<
    ::com::sun::star::xml::crypto::XXMLSignatureTemplate ,
    ::com::sun::star::lang::XInitialization ,
    ::com::sun::star::lang::XServiceInfo >
{
    private :
        ::com::sun::star::uno::Reference< ::com::sun::star::xml::wrapper::XXMLElementWrapper > m_xTemplate ;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xServiceManager ;
        std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::xml::wrapper::XXMLElementWrapper > > targets;
        ::com::sun::star::uno::Reference< ::com::sun::star::xml::crypto::XUriBinding > m_xUriBinding;
        ::com::sun::star::xml::crypto::SecurityOperationStatus m_nStatus;

    public :
        XMLSignatureTemplateImpl( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& aFactory ) ;
        virtual ~XMLSignatureTemplateImpl() ;

        //Methods from XXMLSignatureTemplate
        virtual void SAL_CALL setTemplate(
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::wrapper::XXMLElementWrapper >& aXmlElement
            ) throw( com::sun::star::uno::RuntimeException, com::sun::star::lang::IllegalArgumentException);

        virtual ::com::sun::star::uno::Reference< ::com::sun::star::xml::wrapper::XXMLElementWrapper > SAL_CALL getTemplate(
        ) throw (com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL setTarget(
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::wrapper::XXMLElementWrapper >& aXmlElement
        ) throw( com::sun::star::uno::RuntimeException, com::sun::star::lang::IllegalArgumentException);

        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::xml::wrapper::XXMLElementWrapper > > SAL_CALL getTargets(
        ) throw (com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL setBinding(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::xml::crypto::XUriBinding >& aUriBinding )
            throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::crypto::XUriBinding >
            SAL_CALL getBinding(  )
            throw (::com::sun::star::uno::RuntimeException);

        virtual void SAL_CALL setStatus(
            ::com::sun::star::xml::crypto::SecurityOperationStatus status )
            throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::xml::crypto::SecurityOperationStatus
            SAL_CALL getStatus(  )
            throw (::com::sun::star::uno::RuntimeException);

        //Methods from XInitialization
        virtual void SAL_CALL initialize(
            const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments
        ) throw( ::com::sun::star::uno::Exception , ::com::sun::star::uno::RuntimeException ) ;

        //Methods from XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException ) ;

        virtual sal_Bool SAL_CALL supportsService(
            const ::rtl::OUString& ServiceName
        ) throw( ::com::sun::star::uno::RuntimeException ) ;

        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException ) ;

        //Helper for XServiceInfo
        static ::com::sun::star::uno::Sequence< ::rtl::OUString > impl_getSupportedServiceNames() ;

        static ::rtl::OUString impl_getImplementationName() throw( ::com::sun::star::uno::RuntimeException ) ;

        //Helper for registry
        static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL impl_createInstance( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& aServiceManager ) throw( ::com::sun::star::uno::RuntimeException ) ;

        static ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > impl_createFactory( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& aServiceManager ) ;
} ;

#endif  // _XMLSIGNATURETEMPLATE_XMLSECIMPL_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

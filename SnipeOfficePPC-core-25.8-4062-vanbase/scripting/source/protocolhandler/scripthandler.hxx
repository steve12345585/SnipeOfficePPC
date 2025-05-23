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

#ifndef _FRAMEWORK_SCRIPT_SCRIPTHANDLER_HXX
#define _FRAMEWORK_SCRIPT_SCRIPTHANDLER_HXX

#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XNotifyingDispatch.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <cppuhelper/implbase4.hxx>
#include <com/sun/star/script/provider/XScriptProvider.hpp>

namespace rtl
{
class OUString;
}

namespace com { namespace sun { namespace star {

    namespace document {
        class XScriptInvocationContext;
    }
    namespace uno {
        class Any;
    }
    namespace lang {
        class XMultiServiceFactory;
        class XSingleServiceFactory;
    }
    namespace frame {
        class XFrame;
        class XDispatch;
        class XNotifyingDispatch;
        class XDispatchResultListener;
        struct DispatchDescriptor;
    }
    namespace beans {
        struct PropertyValue;
    }
    namespace util {
        struct URL;
    }
} } }

namespace scripting_protocolhandler
{

class ScriptProtocolHandler :
public ::cppu::WeakImplHelper4< css::frame::XDispatchProvider,
    css::frame::XNotifyingDispatch, css::lang::XServiceInfo, css::lang::XInitialization >
{
private:
    bool m_bInitialised;
    css::uno::Reference < css::lang::XMultiServiceFactory >         m_xFactory;
    css::uno::Reference < css::frame::XFrame >                      m_xFrame;
    css::uno::Reference < css::script::provider::XScriptProvider >  m_xScriptProvider;
    css::uno::Reference< css::document::XScriptInvocationContext >  m_xScriptInvocation;

    void createScriptProvider();
    bool getScriptInvocation();

public:
    ScriptProtocolHandler( const css::uno::Reference <
        css::lang::XMultiServiceFactory >& xFactory );
    virtual ~ScriptProtocolHandler();

    /* XServiceInfo */
    virtual ::rtl::OUString SAL_CALL getImplementationName()
        throw( css::uno::RuntimeException );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& sServiceName )
        throw( css::uno::RuntimeException );
    virtual css::uno::Sequence < ::rtl::OUString > SAL_CALL getSupportedServiceNames()
        throw( css::uno::RuntimeException );

    /* Helper for XServiceInfo */
    static css::uno::Sequence < ::rtl::OUString > impl_getStaticSupportedServiceNames();
    static ::rtl::OUString impl_getStaticImplementationName();

    /* Helper for registry */
    static css::uno::Reference < css::uno::XInterface > SAL_CALL
    impl_createInstance(
        const css::uno::Reference< css::lang::XMultiServiceFactory >& xServiceManager )
    throw( css::uno::RuntimeException );
    static css::uno::Reference < css::lang::XSingleServiceFactory > impl_createFactory(
        const css::uno::Reference< css::lang::XMultiServiceFactory >& xServiceManager );

    /* Implementation for XDispatchProvider */
    virtual css::uno::Reference < css::frame::XDispatch > SAL_CALL
    queryDispatch( const css::util::URL& aURL, const ::rtl::OUString& sTargetFrameName,
                   sal_Int32 eSearchFlags ) throw( css::uno::RuntimeException ) ;
    virtual css::uno::Sequence< css::uno::Reference < css::frame::XDispatch > > SAL_CALL
    queryDispatches(
        const css::uno::Sequence < css::frame::DispatchDescriptor >& seqDescriptor )
    throw( css::uno::RuntimeException );

    /* Implementation for X(Notifying)Dispatch */
    virtual void SAL_CALL dispatchWithNotification(
    const css::util::URL& aURL,
    const css::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArgs,
    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchResultListener >& Listener )
    throw ( css::uno::RuntimeException );
    virtual void SAL_CALL dispatch(
        const css::util::URL& aURL,
        const css::uno::Sequence< css::beans::PropertyValue >& lArgs )
        throw ( css::uno::RuntimeException );
    virtual void SAL_CALL addStatusListener(
        const css::uno::Reference< css::frame::XStatusListener >& xControl,
        const css::util::URL& aURL )
        throw ( css::uno::RuntimeException );
    virtual void SAL_CALL removeStatusListener(
        const css::uno::Reference< css::frame::XStatusListener >& xControl,
        const css::util::URL& aURL )
        throw ( css::uno::RuntimeException );

    /* Implementation for XInitialization */
    virtual void SAL_CALL initialize(
        const css::uno::Sequence < css::uno::Any >& aArguments )
        throw ( css::uno::Exception );
};

}
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

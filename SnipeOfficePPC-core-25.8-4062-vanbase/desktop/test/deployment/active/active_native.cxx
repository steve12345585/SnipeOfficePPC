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

#include "sal/config.h"

#include <cassert>

#include "boost/noncopyable.hpp"
#include "com/sun/star/awt/MessageBoxButtons.hpp"
#include "com/sun/star/awt/Rectangle.hpp"
#include "com/sun/star/awt/Toolkit.hpp"
#include "com/sun/star/awt/XMessageBox.hpp"
#include "com/sun/star/awt/XWindowPeer.hpp"
#include "com/sun/star/beans/PropertyValue.hpp"
#include "com/sun/star/frame/DispatchDescriptor.hpp"
#include "com/sun/star/frame/XDesktop.hpp"
#include "com/sun/star/frame/XDispatch.hpp"
#include "com/sun/star/frame/XDispatchProvider.hpp"
#include "com/sun/star/frame/XFrame.hpp"
#include "com/sun/star/frame/XStatusListener.hpp"
#include "com/sun/star/lang/XComponent.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/registry/XRegistryKey.hpp"
#include "com/sun/star/uno/DeploymentException.hpp"
#include "com/sun/star/uno/Exception.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "com/sun/star/util/URL.hpp"
#include "cppuhelper/factory.hxx"
#include "cppuhelper/implbase2.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "cppuhelper/supportsservice.hxx"
#include "cppuhelper/weak.hxx"
#include "rtl/textenc.h"
#include "rtl/ustring.hxx"
#include "sal/log.hxx"
#include "sal/types.h"
#include "uno/lbnames.h"

namespace {

class Provider:
    public cppu::WeakImplHelper2<
        css::lang::XServiceInfo, css::frame::XDispatchProvider >,
    private boost::noncopyable
{
public:
    static css::uno::Reference< css::uno::XInterface > SAL_CALL static_create(
        css::uno::Reference< css::uno::XComponentContext > const & xContext)
        SAL_THROW((css::uno::Exception))
    { return static_cast< cppu::OWeakObject * >(new Provider(xContext)); }

    static OUString SAL_CALL static_getImplementationName();

    static css::uno::Sequence< OUString > SAL_CALL
    static_getSupportedServiceNames();

private:
    Provider(
        css::uno::Reference< css::uno::XComponentContext > const & context):
        context_(context) { assert(context.is()); }

    virtual ~Provider() {}

    virtual OUString SAL_CALL getImplementationName()
        throw (css::uno::RuntimeException)
    { return static_getImplementationName(); }

    virtual sal_Bool SAL_CALL supportsService(OUString const & ServiceName)
        throw (css::uno::RuntimeException)
    { return cppu::supportsService(this, ServiceName); }

    virtual css::uno::Sequence< OUString > SAL_CALL
    getSupportedServiceNames() throw (css::uno::RuntimeException)
    { return static_getSupportedServiceNames(); }

    virtual css::uno::Reference< css::frame::XDispatch > SAL_CALL queryDispatch(
        css::util::URL const &, OUString const &, sal_Int32)
        throw (css::uno::RuntimeException);

    virtual css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > >
    SAL_CALL queryDispatches(
        css::uno::Sequence< css::frame::DispatchDescriptor > const & Requests)
        throw (css::uno::RuntimeException);

    css::uno::Reference< css::uno::XComponentContext > context_;
};

OUString Provider::static_getImplementationName() {
    return rtl::OUString("com.sun.star.comp.test.deployment.active_native");
}

css::uno::Sequence< OUString > Provider::static_getSupportedServiceNames()
{
    OUString name("com.sun.star.test.deployment.active_native");
    return css::uno::Sequence< OUString >(&name, 1);
}

css::uno::Reference< css::frame::XDispatch > Provider::queryDispatch(
    css::util::URL const &, OUString const &, sal_Int32)
    throw (css::uno::RuntimeException)
{
    css::uno::Reference< css::frame::XDispatch > dispatch;
    if (!(context_->getValueByName(
              "/singletons/com.sun.star.test.deployment."
              "active_native_singleton") >>=
          dispatch) ||
        !dispatch.is())
    {
        throw css::uno::DeploymentException(
            "component context fails to supply singleton"
            " com.sun.star.test.deployment.active_native_singleton of type"
            " com.sun.star.frame.XDispatch",
            context_);
    }
    return dispatch;
}

css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > >
Provider::queryDispatches(
    css::uno::Sequence< css::frame::DispatchDescriptor > const & Requests)
    throw (css::uno::RuntimeException)
{
    css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > > s(
        Requests.getLength());
    for (sal_Int32 i = 0; i < s.getLength(); ++i) {
        s[i] = queryDispatch(
            Requests[i].FeatureURL, Requests[i].FrameName,
            Requests[i].SearchFlags);
    }
    return s;
}

class Dispatch:
    public cppu::WeakImplHelper2<
        css::lang::XServiceInfo, css::frame::XDispatch >,
    private boost::noncopyable
{
public:
    static css::uno::Reference< css::uno::XInterface > SAL_CALL static_create(
        css::uno::Reference< css::uno::XComponentContext > const & xContext)
        SAL_THROW((css::uno::Exception))
    { return static_cast< cppu::OWeakObject * >(new Dispatch(xContext)); }

    static OUString SAL_CALL static_getImplementationName();

    static css::uno::Sequence< OUString > SAL_CALL
    static_getSupportedServiceNames()
    { return css::uno::Sequence< OUString >(); }

private:
    Dispatch(
        css::uno::Reference< css::uno::XComponentContext > const & context):
        context_(context) { assert(context.is()); }

    virtual ~Dispatch() {}

    virtual OUString SAL_CALL getImplementationName()
        throw (css::uno::RuntimeException)
    { return static_getImplementationName(); }

    virtual sal_Bool SAL_CALL supportsService(OUString const & ServiceName)
        throw (css::uno::RuntimeException)
    { return cppu::supportsService(this, ServiceName); }

    virtual css::uno::Sequence< OUString > SAL_CALL
    getSupportedServiceNames() throw (css::uno::RuntimeException)
    { return static_getSupportedServiceNames(); }

    virtual void SAL_CALL dispatch(
        css::util::URL const &,
        css::uno::Sequence< css::beans::PropertyValue > const &)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL addStatusListener(
        css::uno::Reference< css::frame::XStatusListener > const &,
        css::util::URL const &)
        throw (css::uno::RuntimeException)
    {}

    virtual void SAL_CALL removeStatusListener(
        css::uno::Reference< css::frame::XStatusListener > const &,
        css::util::URL const &)
        throw (css::uno::RuntimeException)
    {}

    css::uno::Reference< css::uno::XComponentContext > context_;
};

OUString Dispatch::static_getImplementationName() {
    return rtl::OUString(
        "com.sun.star.comp.test.deployment.active_native_singleton");
}

void Dispatch::dispatch(
    css::util::URL const &,
    css::uno::Sequence< css::beans::PropertyValue > const &)
    throw (css::uno::RuntimeException)
{
    css::uno::Reference< css::awt::XMessageBox > box(
        css::awt::Toolkit::create(context_)->createMessageBox(
            css::uno::Reference< css::awt::XWindowPeer >(
                css::uno::Reference< css::frame::XFrame >(
                    css::uno::Reference< css::frame::XDesktop >(
                        (context_->getServiceManager()->
                         createInstanceWithContext(
                             "com.sun.star.frame.Desktop", context_)),
                        css::uno::UNO_QUERY_THROW)->getCurrentFrame(),
                    css::uno::UNO_SET_THROW)->getComponentWindow(),
                css::uno::UNO_QUERY_THROW),
            css::awt::Rectangle(), "infobox",
            css::awt::MessageBoxButtons::BUTTONS_OK, "active", "native"),
        css::uno::UNO_SET_THROW);
    box->execute();
    css::uno::Reference< css::lang::XComponent >(
        box, css::uno::UNO_QUERY_THROW)->dispose();
}

static cppu::ImplementationEntry const services[] = {
    { &Provider::static_create, &Provider::static_getImplementationName,
      &Provider::static_getSupportedServiceNames,
      &cppu::createSingleComponentFactory, 0, 0 },
    { &Dispatch::static_create, &Dispatch::static_getImplementationName,
      &Dispatch::static_getSupportedServiceNames,
      &cppu::createSingleComponentFactory, 0, 0 },
    { 0, 0, 0, 0, 0, 0 }
};

}

extern "C" SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory(
    char const * pImplName, void * pServiceManager, void * pRegistryKey)
{
    return cppu::component_getFactoryHelper(
        pImplName, pServiceManager, pRegistryKey, services);
}

extern "C" sal_Bool SAL_CALL component_writeInfo(
    void * pServiceManager, void * pRegistryKey)
{
    if (!component_writeInfoHelper(pServiceManager, pRegistryKey, services)) {
        return false;
    }
    try {
        css::uno::Reference< css::registry::XRegistryKey >(
            (css::uno::Reference< css::registry::XRegistryKey >(
                static_cast< css::registry::XRegistryKey * >(pRegistryKey))->
             createKey(
                 "/" + Dispatch::static_getImplementationName() +
                 ("/UNO/SINGLETONS/com.sun.star.test.deployment."
                  "active_native_singleton"))),
            css::uno::UNO_SET_THROW)->
            setStringValue(Dispatch::static_getImplementationName());
    } catch (const css::uno::Exception & e) {
        SAL_INFO(
            "desktop.test",
            "active_native component_writeInfo exception: " << e.Message);
        return false;
    }
    return true;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

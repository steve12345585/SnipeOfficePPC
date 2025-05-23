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


#include "com/sun/star/bridge/XBridge.hpp"
#include "com/sun/star/bridge/XBridgeFactory.hpp"
#include "com/sun/star/connection/Connector.hpp"
#include "com/sun/star/connection/XConnection.hpp"
#include "com/sun/star/connection/XConnector.hpp"
#include "com/sun/star/lang/XMultiComponentFactory.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/lang/XSingleComponentFactory.hpp"
#include "com/sun/star/registry/InvalidRegistryException.hpp"
#include "com/sun/star/registry/XRegistryKey.hpp"
#include "com/sun/star/uno/Exception.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "cppuhelper/factory.hxx"
#include "cppuhelper/implbase2.hxx"
#include "cppuhelper/weak.hxx"
#include "rtl/string.h"
#include "rtl/ustring.hxx"
#include "sal/types.h"
#include "test/java_uno/equals/XBase.hpp"
#include "test/java_uno/equals/XDerived.hpp"
#include "test/java_uno/equals/XTestInterface.hpp"
#include "uno/environment.h"
#include "uno/lbnames.h"

namespace {

class Service: public cppu::WeakImplHelper2<
    css::lang::XServiceInfo, test::java_uno::equals::XTestInterface >
{
public:
    virtual inline rtl::OUString SAL_CALL getImplementationName()
        throw (css::uno::RuntimeException)
    { return rtl::OUString::createFromAscii(getImplementationName_static()); }

    virtual sal_Bool SAL_CALL supportsService(
        rtl::OUString const & rServiceName) throw (css::uno::RuntimeException);

    virtual inline css::uno::Sequence< rtl::OUString > SAL_CALL
    getSupportedServiceNames()  throw (css::uno::RuntimeException)
    { return getSupportedServiceNames_static(); }

    virtual void SAL_CALL connect(rtl::OUString const & rConnection,
                                  rtl::OUString const & rProtocol)
        throw (css::uno::Exception);

    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL get(
        rtl::OUString const & rName) throw (css::uno::RuntimeException);

    static inline sal_Char const * getImplementationName_static()
    { return "com.sun.star.test.bridges.testequals.impl"; }

    static css::uno::Sequence< rtl::OUString >
    getSupportedServiceNames_static();

    static css::uno::Reference< css::uno::XInterface > SAL_CALL createInstance(
        css::uno::Reference< css::uno::XComponentContext > const & rContext)
        throw (css::uno::Exception);

private:
    explicit inline Service(
        css::uno::Reference< css::uno::XComponentContext > const & rContext):
        m_xContext(rContext) {}

    css::uno::Reference< css::uno::XComponentContext > m_xContext;
    css::uno::Reference< css::bridge::XBridge > m_xBridge;
};

}

sal_Bool Service::supportsService(rtl::OUString const & rServiceName)
    throw (css::uno::RuntimeException)
{
    css::uno::Sequence< rtl::OUString > aNames(
        getSupportedServiceNames_static());
    for (sal_Int32 i = 0; i< aNames.getLength(); ++i)
        if (aNames[i] == rServiceName)
            return true;
    return false;
}

void Service::connect(rtl::OUString const & rConnection,
                      rtl::OUString const & rProtocol)
    throw (css::uno::Exception)
{
    css::uno::Reference< css::connection::XConnection > xConnection(
        css::connection::Connector::create(m_xContext)->connect(rConnection));
    css::uno::Reference< css::bridge::XBridgeFactory > xBridgeFactory(
        m_xContext->getServiceManager()->createInstanceWithContext(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.bridge.BridgeFactory" )),
            m_xContext),
        css::uno::UNO_QUERY);
    m_xBridge = xBridgeFactory->createBridge(rtl::OUString(), rProtocol,
                                             xConnection, 0);
}

css::uno::Reference< css::uno::XInterface >
Service::get(rtl::OUString const & rName) throw (css::uno::RuntimeException)
{
    return m_xBridge->getInstance(rName);
}

css::uno::Sequence< rtl::OUString > Service::getSupportedServiceNames_static()
{
    css::uno::Sequence< rtl::OUString > aNames(1);
    aNames[0] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.test.bridges.testequals" ));
    return aNames;
}

css::uno::Reference< css::uno::XInterface > Service::createInstance(
    css::uno::Reference< css::uno::XComponentContext > const & rContext)
    throw (css::uno::Exception)
{
    // Make types known:
    getCppuType(
        static_cast<
        css::uno::Reference< test::java_uno::equals::XBase > const * >(0));
    getCppuType(
        static_cast<
        css::uno::Reference< test::java_uno::equals::XDerived > const * >(0));
    getCppuType(
        static_cast<
        css::uno::Reference< test::java_uno::equals::XTestInterface > const * >(
            0));

    return static_cast< cppu::OWeakObject * >(new Service(rContext));
}

extern "C" SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory(sal_Char const * pImplName,
                                                void * pServiceManager, void *)
{
    void * pFactory = 0;
    if (pServiceManager)
        if (rtl_str_compare(pImplName, Service::getImplementationName_static())
            == 0)
        {
            css::uno::Reference< css::lang::XSingleComponentFactory >
                xFactory(cppu::createSingleComponentFactory(
                             &Service::createInstance,
                             rtl::OUString::createFromAscii(
                                 Service::getImplementationName_static()),
                             Service::getSupportedServiceNames_static()));
            if (xFactory.is())
            {
                xFactory->acquire();
                pFactory = xFactory.get();
            }
        }
    return pFactory;
}

namespace {

bool writeInfo(void * pRegistryKey, sal_Char const * pImplementationName,
               css::uno::Sequence< rtl::OUString > const & rServiceNames)
{
    rtl::OUString aKeyName( RTL_CONSTASCII_USTRINGPARAM( "/" ));
    aKeyName += rtl::OUString::createFromAscii(pImplementationName);
    aKeyName += rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/UNO/SERVICES" ));
    css::uno::Reference< css::registry::XRegistryKey > xKey;
    try
    {
        xKey = static_cast< css::registry::XRegistryKey * >(pRegistryKey)->
            createKey(aKeyName);
    }
    catch (css::registry::InvalidRegistryException &) {}
    if (!xKey.is())
        return false;
    bool bSuccess = true;
    for (sal_Int32 i = 0; i < rServiceNames.getLength(); ++i)
        try
        {
            xKey->createKey(rServiceNames[i]);
        }
        catch (css::registry::InvalidRegistryException &)
        {
            bSuccess = false;
            break;
        }
    return bSuccess;
}

}

extern "C" sal_Bool SAL_CALL component_writeInfo(void *, void * pRegistryKey)
{
    return pRegistryKey
        && writeInfo(pRegistryKey, Service::getImplementationName_static(),
                     Service::getSupportedServiceNames_static());
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

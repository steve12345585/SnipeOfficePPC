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

#include "sal/config.h"

#include <cassert>
#include <vector>

#include "boost/noncopyable.hpp"
#include "com/sun/star/beans/NamedValue.hpp"
#include "com/sun/star/beans/PropertyValue.hpp"
#include "com/sun/star/lang/EventObject.hpp"
#include "com/sun/star/lang/Locale.hpp"
#include "com/sun/star/lang/XLocalizable.hpp"
#include "com/sun/star/lang/XMultiServiceFactory.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/lang/XSingleComponentFactory.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/DeploymentException.hpp"
#include "com/sun/star/uno/Exception.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "com/sun/star/util/XFlushListener.hpp"
#include "com/sun/star/util/XFlushable.hpp"
#include "com/sun/star/util/XRefreshListener.hpp"
#include "com/sun/star/util/XRefreshable.hpp"
#include "comphelper/locale.hxx"
#include "cppu/unotype.hxx"
#include "cppuhelper/compbase5.hxx"
#include "cppuhelper/factory.hxx"
#include "cppuhelper/implbase2.hxx"
#include "cppuhelper/interfacecontainer.hxx"
#include "cppuhelper/weak.hxx"
#include "osl/mutex.hxx"
#include "sal/types.h"
#include "rtl/ref.hxx"
#include "rtl/unload.h"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"

#include "components.hxx"
#include "configurationprovider.hxx"
#include "lock.hxx"
#include "rootaccess.hxx"

namespace configmgr { namespace configuration_provider {

namespace {

namespace css = com::sun::star;

char const accessServiceName[] =
    "com.sun.star.configuration.ConfigurationAccess";
char const updateAccessServiceName[] =
    "com.sun.star.configuration.ConfigurationUpdateAccess";

void badNodePath() {
    throw css::uno::Exception(
        rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.configuration.ConfigurationProvider expects a"
                " single, non-empty, string nodepath argument")),
        0);
}

typedef
    cppu::WeakComponentImplHelper5<
        css::lang::XServiceInfo, css::lang::XMultiServiceFactory,
        css::util::XRefreshable, css::util::XFlushable,
        css::lang::XLocalizable >
    ServiceBase;

class Service:
    private osl::Mutex, public ServiceBase, private boost::noncopyable
{
public:
    Service(
        css::uno::Reference< css::uno::XComponentContext > const context,
        rtl::OUString const & locale):
        ServiceBase(*static_cast< osl::Mutex * >(this)), context_(context),
        locale_(locale)
    {
        lock_ = lock();
        assert(context.is());
    }

private:
    virtual ~Service() {}

    virtual void SAL_CALL disposing() { flushModifications(); }

    virtual rtl::OUString SAL_CALL getImplementationName()
        throw (css::uno::RuntimeException)
    { return configuration_provider::getImplementationName(); }

    virtual sal_Bool SAL_CALL supportsService(rtl::OUString const & ServiceName)
        throw (css::uno::RuntimeException)
    { return ServiceName == getSupportedServiceNames()[0]; } //TODO

    virtual css::uno::Sequence< rtl::OUString > SAL_CALL
    getSupportedServiceNames() throw (css::uno::RuntimeException)
    { return configuration_provider::getSupportedServiceNames(); }

    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstance(
        rtl::OUString const & aServiceSpecifier)
        throw (css::uno::Exception, css::uno::RuntimeException);

    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL
    createInstanceWithArguments(
        rtl::OUString const & ServiceSpecifier,
        css::uno::Sequence< css::uno::Any > const & Arguments)
        throw (css::uno::Exception, css::uno::RuntimeException);

    virtual css::uno::Sequence< rtl::OUString > SAL_CALL
    getAvailableServiceNames() throw (css::uno::RuntimeException);

    virtual void SAL_CALL refresh() throw (css::uno::RuntimeException);

    virtual void SAL_CALL addRefreshListener(
        css::uno::Reference< css::util::XRefreshListener > const & l)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL removeRefreshListener(
        css::uno::Reference< css::util::XRefreshListener > const & l)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL flush() throw (css::uno::RuntimeException);

    virtual void SAL_CALL addFlushListener(
        css::uno::Reference< css::util::XFlushListener > const & l)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL removeFlushListener(
        css::uno::Reference< css::util::XFlushListener > const & l)
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL setLocale(css::lang::Locale const & eLocale)
        throw (css::uno::RuntimeException);

    virtual css::lang::Locale SAL_CALL getLocale()
        throw (css::uno::RuntimeException);

    void flushModifications() const;

    css::uno::Reference< css::uno::XComponentContext > context_;
    rtl::OUString locale_;
    boost::shared_ptr<osl::Mutex> lock_;
};

css::uno::Reference< css::uno::XInterface > Service::createInstance(
    rtl::OUString const & aServiceSpecifier)
    throw (css::uno::Exception, css::uno::RuntimeException)
{
    return createInstanceWithArguments(
        aServiceSpecifier, css::uno::Sequence< css::uno::Any >());
}

css::uno::Reference< css::uno::XInterface >
Service::createInstanceWithArguments(
    rtl::OUString const & ServiceSpecifier,
    css::uno::Sequence< css::uno::Any > const & Arguments)
    throw (css::uno::Exception, css::uno::RuntimeException)
{
    rtl::OUString nodepath;
    rtl::OUString locale;
    for (sal_Int32 i = 0; i < Arguments.getLength(); ++i) {
        css::beans::NamedValue v1;
        css::beans::PropertyValue v2;
        rtl::OUString name;
        css::uno::Any value;
        if (Arguments[i] >>= v1) {
            name = v1.Name;
            value = v1.Value;
        } else if (Arguments[i] >>= v2) {
            name = v2.Name;
            value = v2.Value;
        } else if (Arguments.getLength() == 1 && (Arguments[i] >>= nodepath)) {
            // For backwards compatibility, allow a single string argument that
            // denotes nodepath.
            if (nodepath.isEmpty()) {
                badNodePath();
            }
            break;
        } else {
            throw css::uno::Exception(
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM(
                        "com.sun.star.configuration.ConfigurationProvider"
                        " expects NamedValue or PropertyValue arguments")),
                0);
        }
        // For backwards compatibility, allow "nodepath" and "Locale" in any
        // case:
        if (name.equalsIgnoreAsciiCaseAsciiL(
                RTL_CONSTASCII_STRINGPARAM("nodepath")))
        {
            if (!nodepath.isEmpty() || !(value >>= nodepath) ||
                nodepath.isEmpty())
            {
                badNodePath();
            }
        } else if (name.equalsIgnoreAsciiCaseAsciiL(
                       RTL_CONSTASCII_STRINGPARAM("locale")))
        {
            if (!locale.isEmpty() || !(value >>= locale) ||
                locale.isEmpty())
            {
                throw css::uno::Exception(
                    rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM(
                            "com.sun.star.configuration.ConfigurationProvider"
                            " expects at most one, non-empty, string Locale"
                            " argument")),
                    0);
            }
        }
    }
    if (nodepath.isEmpty()) {
        badNodePath();
    }
    // For backwards compatibility, allow a nodepath that misses the leading
    // slash:
    if (nodepath[0] != '/') {
        nodepath = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/")) + nodepath;
    }
    if (locale.isEmpty()) {
        //TODO: should the Access use the dynamically changing locale_ instead?
        locale = locale_;
        if (locale.isEmpty()) {
            locale = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("en-US"));
        }
    }
    bool update;
    if ( ServiceSpecifier == accessServiceName )
    {
        update = false;
    } else if ( ServiceSpecifier == updateAccessServiceName )
    {
        update = true;
    } else {
        throw css::uno::Exception(
            (rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM(
                    "com.sun.star.configuration.ConfigurationProvider does not"
                    " support service ")) +
             ServiceSpecifier),
            static_cast< cppu::OWeakObject * >(this));
    }
    osl::MutexGuard guard(*lock_);
    Components & components = Components::getSingleton(context_);
    rtl::Reference< RootAccess > root(
        new RootAccess(components, nodepath, locale, update));
    if (root->isValue()) {
        throw css::uno::Exception(
            (rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM(
                    "com.sun.star.configuration.ConfigurationProvider: there is"
                    " a leaf value at nodepath ")) +
             nodepath),
            static_cast< cppu::OWeakObject * >(this));
    }
    components.addRootAccess(root);
    return static_cast< cppu::OWeakObject * >(root.get());
}

css::uno::Sequence< rtl::OUString > Service::getAvailableServiceNames()
    throw (css::uno::RuntimeException)
{
    css::uno::Sequence< rtl::OUString > names(2);
    names[0] = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(accessServiceName));
    names[1] = rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM(updateAccessServiceName));
    return names;
}

void Service::refresh() throw (css::uno::RuntimeException) {
    //TODO
    cppu::OInterfaceContainerHelper * cont = rBHelper.getContainer(
        cppu::UnoType< css::util::XRefreshListener >::get());
    if (cont != 0) {
        css::lang::EventObject ev(static_cast< cppu::OWeakObject * >(this));
        cont->notifyEach(&css::util::XRefreshListener::refreshed, ev);
    }
}

void Service::addRefreshListener(
    css::uno::Reference< css::util::XRefreshListener > const & l)
    throw (css::uno::RuntimeException)
{
    rBHelper.addListener(
        cppu::UnoType< css::util::XRefreshListener >::get(), l);
}

void Service::removeRefreshListener(
    css::uno::Reference< css::util::XRefreshListener > const & l)
    throw (css::uno::RuntimeException)
{
    rBHelper.removeListener(
        cppu::UnoType< css::util::XRefreshListener >::get(), l);
}

void Service::flush() throw (css::uno::RuntimeException) {
    flushModifications();
    cppu::OInterfaceContainerHelper * cont = rBHelper.getContainer(
        cppu::UnoType< css::util::XFlushListener >::get());
    if (cont != 0) {
        css::lang::EventObject ev(static_cast< cppu::OWeakObject * >(this));
        cont->notifyEach(&css::util::XFlushListener::flushed, ev);
    }
}

void Service::addFlushListener(
    css::uno::Reference< css::util::XFlushListener > const & l)
    throw (css::uno::RuntimeException)
{
    rBHelper.addListener(cppu::UnoType< css::util::XFlushListener >::get(), l);
}

void Service::removeFlushListener(
    css::uno::Reference< css::util::XFlushListener > const & l)
    throw (css::uno::RuntimeException)
{
    rBHelper.removeListener(
        cppu::UnoType< css::util::XFlushListener >::get(), l);
}

void Service::setLocale(css::lang::Locale const & eLocale)
    throw (css::uno::RuntimeException)
{
    osl::MutexGuard guard(*lock_);
    locale_ = comphelper::Locale(
        eLocale.Language, eLocale.Country, eLocale.Variant).toISO();
}

css::lang::Locale Service::getLocale() throw (css::uno::RuntimeException) {
    osl::MutexGuard guard(*lock_);
    css::lang::Locale loc;
    if ( locale_ == "*" ) {
        loc.Language = locale_;
    } else if (! locale_.isEmpty()) {
        try {
            comphelper::Locale l(locale_);
            loc.Language = l.getLanguage();
            loc.Country = l.getCountry();
            loc.Variant = l.getVariant();
        } catch (comphelper::Locale::MalFormedLocaleException & e) {
            throw css::uno::RuntimeException(
                (rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM("MalformedLocaleException: ")) +
                 e.Message),
                static_cast< cppu::OWeakObject * >(this));
        }
    }
    return loc;
}

void Service::flushModifications() const {
    Components * components;
    {
        osl::MutexGuard guard(*lock_);
        components = &Components::getSingleton(context_);
    }
    components->flushModifications();
}

class Factory:
    public cppu::WeakImplHelper2<
        css::lang::XSingleComponentFactory, css::lang::XServiceInfo >,
    private boost::noncopyable
{
public:
    Factory() {}

private:
    virtual ~Factory() {}

    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL
    createInstanceWithContext(
        css::uno::Reference< css::uno::XComponentContext > const & Context)
        throw (css::uno::Exception, css::uno::RuntimeException);

    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL
    createInstanceWithArgumentsAndContext(
        css::uno::Sequence< css::uno::Any > const & Arguments,
        css::uno::Reference< css::uno::XComponentContext > const & Context)
        throw (css::uno::Exception, css::uno::RuntimeException);

    virtual rtl::OUString SAL_CALL getImplementationName()
        throw (css::uno::RuntimeException)
    { return configuration_provider::getImplementationName(); }

    virtual sal_Bool SAL_CALL supportsService(rtl::OUString const & ServiceName)
        throw (css::uno::RuntimeException)
    { return ServiceName == getSupportedServiceNames()[0]; } //TODO

    virtual css::uno::Sequence< rtl::OUString > SAL_CALL
    getSupportedServiceNames() throw (css::uno::RuntimeException)
    { return configuration_provider::getSupportedServiceNames(); }
};

css::uno::Reference< css::uno::XInterface > Factory::createInstanceWithContext(
    css::uno::Reference< css::uno::XComponentContext > const & Context)
    throw (css::uno::Exception, css::uno::RuntimeException)
{
    return createInstanceWithArgumentsAndContext(
        css::uno::Sequence< css::uno::Any >(), Context);
}

css::uno::Reference< css::uno::XInterface >
Factory::createInstanceWithArgumentsAndContext(
    css::uno::Sequence< css::uno::Any > const & Arguments,
    css::uno::Reference< css::uno::XComponentContext > const & Context)
    throw (css::uno::Exception, css::uno::RuntimeException)
{
    if (Arguments.getLength() == 0) {
        css::uno::Reference< css::uno::XInterface > instance;
        if (!(Context->getValueByName(
                  rtl::OUString(
                      RTL_CONSTASCII_USTRINGPARAM(
                          "/singletons/"
                          "com.sun.star.configuration.theDefaultProvider")))
              >>= instance) ||
            !instance.is())
        {
            throw css::uno::DeploymentException(
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM(
                        "component context fails to supply singleton"
                        " com.sun.star.configuration.theDefaultProvider")),
                Context);
        }
        return instance;
    } else {
        rtl::OUString locale;
        for (sal_Int32 i = 0; i < Arguments.getLength(); ++i) {
            css::beans::NamedValue v1;
            css::beans::PropertyValue v2;
            rtl::OUString name;
            css::uno::Any value;
            if (Arguments[i] >>= v1) {
                name = v1.Name;
                value = v1.Value;
            } else if (Arguments[i] >>= v2) {
                name = v2.Name;
                value = v2.Value;
            } else {
                throw css::uno::Exception(
                    rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM(
                            "com.sun.star.configuration.ConfigurationProvider"
                            " factory expects NamedValue or PropertyValue"
                            " arguments")),
                    0);
            }
            // For backwards compatibility, allow "Locale" and (ignored)
            // "EnableAsync" in any case:
            if (name.equalsIgnoreAsciiCaseAsciiL(
                    RTL_CONSTASCII_STRINGPARAM("locale")))
            {
                if (!locale.isEmpty() || !(value >>= locale) ||
                    locale.isEmpty())
                {
                    throw css::uno::Exception(
                        rtl::OUString(
                            RTL_CONSTASCII_USTRINGPARAM(
                                "com.sun.star.configuration."
                                "ConfigurationProvider factory expects at most"
                                " one, non-empty, string Locale argument")),
                        0);
                }
            } else if (!name.equalsIgnoreAsciiCaseAsciiL(
                           RTL_CONSTASCII_STRINGPARAM("enableasync")))
            {
                throw css::uno::Exception(
                    rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM(
                            "com.sun.star.configuration.ConfigurationProvider"
                            " factory: unknown argument ")) + name,
                    0);
            }
        }
        return static_cast< cppu::OWeakObject * >(new Service(Context, locale));
    }
}

}

css::uno::Reference< css::uno::XInterface > createDefault(
    css::uno::Reference< css::uno::XComponentContext > const & context)
{
    return static_cast< cppu::OWeakObject * >(
        new Service(context, rtl::OUString()));
}

rtl::OUString getImplementationName() {
    return rtl::OUString(
        RTL_CONSTASCII_USTRINGPARAM(
            "com.sun.star.comp.configuration.ConfigurationProvider"));
}

css::uno::Sequence< rtl::OUString > getSupportedServiceNames() {
    rtl::OUString name(
        RTL_CONSTASCII_USTRINGPARAM(
            "com.sun.star.configuration.ConfigurationProvider"));
    return css::uno::Sequence< rtl::OUString >(&name, 1);
}

css::uno::Reference< css::lang::XSingleComponentFactory >
createFactory(
    SAL_UNUSED_PARAMETER cppu::ComponentFactoryFunc,
    SAL_UNUSED_PARAMETER rtl::OUString const &,
    SAL_UNUSED_PARAMETER css::uno::Sequence< rtl::OUString > const &,
    SAL_UNUSED_PARAMETER rtl_ModuleCount *)
    SAL_THROW(())
{
    return new Factory;
}

} }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

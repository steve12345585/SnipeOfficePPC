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

#include <list>

#include "boost/noncopyable.hpp"
#include "com/sun/star/beans/NamedValue.hpp"
#include "com/sun/star/container/XHierarchicalNameAccess.hpp"
#include "com/sun/star/lang/XMultiServiceFactory.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Sequence.hxx"
#include "osl/diagnose.h"
#include "rtl/instance.hxx"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"
#include "unotools/configitem.hxx"
#include "unotools/configmgr.hxx"
#include "comphelper/processfactory.hxx"

namespace {

namespace css = com::sun::star;

class RegisterConfigItemHelper: private boost::noncopyable {
public:
    RegisterConfigItemHelper(
        utl::ConfigManager & manager, utl::ConfigItem & item):
            manager_(manager), item_(&item)
    {
        manager.registerConfigItem(item_);
    }

    ~RegisterConfigItemHelper() {
        if (item_ != 0) {
            manager_.removeConfigItem(*item_);
        }
    }

    void keep() { item_ = 0; }

private:
    utl::ConfigManager & manager_;
    utl::ConfigItem * item_;
};

css::uno::Reference< css::lang::XMultiServiceFactory >
getConfigurationProvider() {
    return css::uno::Reference< css::lang::XMultiServiceFactory >(
        (css::uno::Reference< css::lang::XMultiServiceFactory >(
            comphelper::getProcessServiceFactory(), css::uno::UNO_SET_THROW)->
         createInstance(
             rtl::OUString(
                 RTL_CONSTASCII_USTRINGPARAM(
                     "com.sun.star.configuration.ConfigurationProvider")))),
        css::uno::UNO_QUERY_THROW);
}

rtl::OUString getConfigurationString(
    rtl::OUString const & module, rtl::OUString const & path)
{
    css::uno::Sequence< css::uno::Any > args(1);
    args[0] <<= css::beans::NamedValue(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("nodepath")),
        css::uno::makeAny(module));
    return
        css::uno::Reference< css::container::XHierarchicalNameAccess >(
            getConfigurationProvider()->createInstanceWithArguments(
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM(
                        "com.sun.star.configuration.ConfigurationAccess")),
                args),
            css::uno::UNO_QUERY_THROW)->
        getByHierarchicalName(path).get< rtl::OUString >();
}

struct theConfigManager:
    public rtl::Static< utl::ConfigManager, theConfigManager >
{};

}

rtl::OUString utl::ConfigManager::getAboutBoxProductVersion() {
    return getConfigurationString(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.Setup")),
        rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("Product/ooSetupVersionAboutBox")));
}

rtl::OUString utl::ConfigManager::getAboutBoxProductVersionSuffix() {
    return getConfigurationString(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.Setup")),
        rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("Product/ooSetupVersionAboutBoxSuffix")));
}

rtl::OUString utl::ConfigManager::getDefaultCurrency() {
    return getConfigurationString(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.Setup")),
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("L10N/ooSetupCurrency")));
}

rtl::OUString utl::ConfigManager::getLocale() {
    return getConfigurationString(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.Setup")),
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("L10N/ooLocale")));
}

rtl::OUString utl::ConfigManager::getProductExtension() {
    return getConfigurationString(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.Setup")),
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Product/ooSetupExtension")));
}

rtl::OUString utl::ConfigManager::getProductName() {
    return getConfigurationString(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.Setup")),
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Product/ooName")));
}

rtl::OUString utl::ConfigManager::getProductVersion() {
    return getConfigurationString(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.Setup")),
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Product/ooSetupVersion")));
}

rtl::OUString utl::ConfigManager::getProductXmlFileFormat() {
    return getConfigurationString(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.Setup")),
        rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("Product/ooXMLFileFormatName")));
}

rtl::OUString utl::ConfigManager::getProductXmlFileFormatVersion() {
    return getConfigurationString(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.Setup")),
        rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("Product/ooXMLFileFormatVersion")));
}

rtl::OUString utl::ConfigManager::getVendor() {
    return getConfigurationString(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.Setup")),
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Product/ooVendor")));
}

rtl::OUString utl::ConfigManager::getWriterCompatibilityVersionOOo_1_1() {
    return getConfigurationString(
        rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM(
                "/org.openoffice.Office.Compatibility")),
        rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("WriterCompatibilityVersion/OOo11")));
}

void utl::ConfigManager::storeConfigItems() {
    getConfigManager().doStoreConfigItems();
}

utl::ConfigManager & utl::ConfigManager::getConfigManager() {
    return theConfigManager::get();
}

css::uno::Reference< css::container::XHierarchicalNameAccess >
utl::ConfigManager::acquireTree(utl::ConfigItem & item) {
    css::uno::Sequence< css::uno::Any > args(1);
    args[0] <<= css::beans::NamedValue(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("nodepath")),
        css::uno::makeAny(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/org.openoffice.")) +
            item.GetSubTreeName()));
    if ((item.GetMode() & CONFIG_MODE_ALL_LOCALES) != 0) {
        args.realloc(2);
        args[1] <<= css::beans::NamedValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("locale")),
            css::uno::makeAny(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("*"))));
    }
    return css::uno::Reference< css::container::XHierarchicalNameAccess >(
        getConfigurationProvider()->createInstanceWithArguments(
            rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM(
                    "com.sun.star.configuration.ConfigurationUpdateAccess")),
            args),
        css::uno::UNO_QUERY_THROW);
}

utl::ConfigManager::ConfigManager() {}

utl::ConfigManager::~ConfigManager() {
    OSL_ASSERT(items_.empty());
}

css::uno::Reference< css::container::XHierarchicalNameAccess >
utl::ConfigManager::addConfigItem(utl::ConfigItem & item) {
    RegisterConfigItemHelper reg(*this, item);
    css::uno::Reference< css::container::XHierarchicalNameAccess > tree(
        acquireTree(item));
    reg.keep();
    return tree;
}

void utl::ConfigManager::removeConfigItem(utl::ConfigItem & item) {
    for (std::list< ConfigItem * >::iterator i(items_.begin());
         i != items_.end(); ++i)
    {
        if (*i == &item) {
            items_.erase(i);
            break;
        }
    }
}

void utl::ConfigManager::registerConfigItem(utl::ConfigItem * item) {
    OSL_ASSERT(item != 0);
    items_.push_back(item);
}

void utl::ConfigManager::doStoreConfigItems() {
    for (std::list< ConfigItem * >::iterator i(items_.begin());
         i != items_.end(); ++i)
    {
        if ((*i)->IsModified()) {
            (*i)->Commit();
            (*i)->ClearModified();
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

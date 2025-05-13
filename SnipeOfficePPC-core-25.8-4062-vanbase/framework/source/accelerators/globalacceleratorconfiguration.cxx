/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#include <accelerators/globalacceleratorconfiguration.hxx>

#include <threadhelp/readguard.hxx>
#include <threadhelp/writeguard.hxx>
#include "helper/mischelper.hxx"

#include <acceleratorconst.h>
#include <services.h>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/util/XChangesNotifier.hpp>

#include <vcl/svapp.hxx>
#include <comphelper/locale.hxx>
#include <comphelper/configurationhelper.hxx>


namespace framework
{

//-----------------------------------------------
// XInterface, XTypeProvider, XServiceInfo
DEFINE_XINTERFACE_2(GlobalAcceleratorConfiguration           ,
                    XCUBasedAcceleratorConfiguration                 ,
                    DIRECT_INTERFACE(css::lang::XServiceInfo),
                    DIRECT_INTERFACE(css::lang::XInitialization))
DEFINE_XTYPEPROVIDER_2_WITH_BASECLASS(GlobalAcceleratorConfiguration,
                                      XCUBasedAcceleratorConfiguration      ,
                                      css::lang::XServiceInfo       ,
                                      css::lang::XInitialization)

DEFINE_XSERVICEINFO_MULTISERVICE(GlobalAcceleratorConfiguration                   ,
                                 ::cppu::OWeakObject                              ,
                                 SERVICENAME_GLOBALACCELERATORCONFIGURATION       ,
                                 IMPLEMENTATIONNAME_GLOBALACCELERATORCONFIGURATION)

DEFINE_INIT_SERVICE(GlobalAcceleratorConfiguration,
                    {
                        /*Attention
                        I think we don't need any mutex or lock here ... because we are called by our own static method impl_createInstance()
                        to create a new instance of this class by our own supported service factory.
                        see macro DEFINE_XSERVICEINFO_MULTISERVICE and "impl_initService()" for further informations!
                        */
                        impl_ts_fillCache();
                    }
                   )

//-----------------------------------------------
GlobalAcceleratorConfiguration::GlobalAcceleratorConfiguration(const css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR)
    : XCUBasedAcceleratorConfiguration(xSMGR)
{
}

//-----------------------------------------------
GlobalAcceleratorConfiguration::~GlobalAcceleratorConfiguration()
{
}

void SAL_CALL GlobalAcceleratorConfiguration::initialize(const css::uno::Sequence< css::uno::Any >& /*lArguments*/)
    throw(css::uno::Exception       ,
          css::uno::RuntimeException)
{
}

//-----------------------------------------------
void GlobalAcceleratorConfiguration::impl_ts_fillCache()
{
    // get current office locale ... but dont cache it.
    // Otherwise we must be listener on the configuration layer
    // which seems to superflous for this small implementation .-)
    ::comphelper::Locale aLocale = ::comphelper::Locale(m_sLocale);

    // May be there exists no accelerator config? Handle it gracefully :-)
    try
    {
        m_sGlobalOrModules = CFG_ENTRY_GLOBAL;
        XCUBasedAcceleratorConfiguration::reload();

        css::uno::Reference< css::util::XChangesNotifier > xBroadcaster(m_xCfg, css::uno::UNO_QUERY_THROW);
        m_xCfgListener = new WeakChangesListener(this);
        xBroadcaster->addChangesListener(m_xCfgListener);
    }
    catch(const css::uno::RuntimeException&)
        { throw; }
    catch(const css::uno::Exception&)
        {}
}

} // namespace framework

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

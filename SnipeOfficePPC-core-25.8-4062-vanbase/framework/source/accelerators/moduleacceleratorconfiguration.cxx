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

#include <accelerators/moduleacceleratorconfiguration.hxx>

#include <threadhelp/readguard.hxx>
#include <threadhelp/writeguard.hxx>
#include "helper/mischelper.hxx"

#include <acceleratorconst.h>
#include <services.h>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/embed/ElementModes.hpp>

#include <comphelper/sequenceashashmap.hxx>
#include <vcl/svapp.hxx>

#include <comphelper/configurationhelper.hxx>

#include <com/sun/star/util/XChangesNotifier.hpp>

#include <rtl/logfile.hxx>

#include <rtl/logfile.h>


namespace framework
{

//-----------------------------------------------
// XInterface, XTypeProvider, XServiceInfo
DEFINE_XINTERFACE_2(ModuleAcceleratorConfiguration              ,
                    XCUBasedAcceleratorConfiguration                    ,
                    DIRECT_INTERFACE(css::lang::XServiceInfo)   ,
                    DIRECT_INTERFACE(css::lang::XInitialization))

DEFINE_XTYPEPROVIDER_2_WITH_BASECLASS(ModuleAcceleratorConfiguration,
                                      XCUBasedAcceleratorConfiguration      ,
                                      css::lang::XServiceInfo       ,
                                      css::lang::XInitialization    )

DEFINE_XSERVICEINFO_MULTISERVICE(ModuleAcceleratorConfiguration                   ,
                                 ::cppu::OWeakObject                              ,
                                 SERVICENAME_MODULEACCELERATORCONFIGURATION       ,
                                 IMPLEMENTATIONNAME_MODULEACCELERATORCONFIGURATION)

DEFINE_INIT_SERVICE(ModuleAcceleratorConfiguration,
                    {
                        /*Attention
                        I think we don't need any mutex or lock here ... because we are called by our own static method impl_createInstance()
                        to create a new instance of this class by our own supported service factory.
                        see macro DEFINE_XSERVICEINFO_MULTISERVICE and "impl_initService()" for further informations!
                        */
                    }
                   )

//-----------------------------------------------
ModuleAcceleratorConfiguration::ModuleAcceleratorConfiguration(const css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR)
    : XCUBasedAcceleratorConfiguration(xSMGR)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "framework", "Ocke.Janssen@sun.com", "ModuleAcceleratorConfiguration::ModuleAcceleratorConfiguration" );
}

//-----------------------------------------------
ModuleAcceleratorConfiguration::~ModuleAcceleratorConfiguration()
{
}

//-----------------------------------------------
void SAL_CALL ModuleAcceleratorConfiguration::initialize(const css::uno::Sequence< css::uno::Any >& lArguments)
    throw(css::uno::Exception       ,
          css::uno::RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "framework", "Ocke.Janssen@sun.com", "ModuleAcceleratorConfiguration::initialize" );
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    ::comphelper::SequenceAsHashMap lArgs(lArguments);
    m_sModule = lArgs.getUnpackedValueOrDefault(::rtl::OUString("ModuleIdentifier"), ::rtl::OUString());
    m_sLocale = lArgs.getUnpackedValueOrDefault(::rtl::OUString("Locale")          , ::rtl::OUString("x-default"));

    if (m_sModule.isEmpty())
        throw css::uno::RuntimeException(
                ::rtl::OUString("The module dependend accelerator configuration service was initialized with an empty module identifier!"),
                static_cast< ::cppu::OWeakObject* >(this));

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    impl_ts_fillCache();
}

//-----------------------------------------------
void ModuleAcceleratorConfiguration::impl_ts_fillCache()
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "framework", "Ocke.Janssen@sun.com", "ModuleAcceleratorConfiguration::impl_ts_fillCache" );
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    m_sModuleCFG = m_sModule;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // get current office locale ... but dont cache it.
    // Otherwise we must be listener on the configuration layer
    // which seems to superflous for this small implementation .-)
    ::comphelper::Locale aLocale = ::comphelper::Locale(m_sLocale);

    // May be the current app module does not have any
    // accelerator config? Handle it gracefully :-)
    try
    {
        m_sGlobalOrModules = CFG_ENTRY_MODULES;
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

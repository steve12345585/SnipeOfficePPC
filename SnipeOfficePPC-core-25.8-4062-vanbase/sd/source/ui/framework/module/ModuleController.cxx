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


#include "framework/ModuleController.hxx"

#include "tools/ConfigurationAccess.hxx"
#include <comphelper/processfactory.hxx>
#include <comphelper/stl_types.hxx>
#include <boost/bind.hpp>
#include <boost/unordered_map.hpp>

#include <tools/diagnose_ex.h>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing::framework;
using ::rtl::OUString;
using ::sd::tools::ConfigurationAccess;

namespace sd { namespace framework {

static const sal_uInt32 snFactoryPropertyCount (2);
static const sal_uInt32 snStartupPropertyCount (1);




class ModuleController::ResourceToFactoryMap
    : public ::boost::unordered_map<
    rtl::OUString,
    rtl::OUString,
    ::rtl::OUStringHash,
    ::comphelper::UStringEqual>
{
public:
    ResourceToFactoryMap (void) {}
};


class ModuleController::LoadedFactoryContainer
    : public ::boost::unordered_map<
    rtl::OUString,
    WeakReference<XInterface>,
    ::rtl::OUStringHash,
    ::comphelper::UStringEqual>
{
public:
    LoadedFactoryContainer (void) {}
};





Reference<XInterface> SAL_CALL ModuleController_createInstance (
    const Reference<XComponentContext>& rxContext)
{
    return Reference<XInterface>(ModuleController::CreateInstance(rxContext), UNO_QUERY);
}




::rtl::OUString ModuleController_getImplementationName (void) throw(RuntimeException)
{
    return ::rtl::OUString("com.sun.star.comp.Draw.framework.module.ModuleController");
}




Sequence<rtl::OUString> SAL_CALL ModuleController_getSupportedServiceNames (void)
    throw (RuntimeException)
{
    static const ::rtl::OUString sServiceName("com.sun.star.drawing.framework.ModuleController");
    return Sequence<rtl::OUString>(&sServiceName, 1);
}




//===== ModuleController ======================================================

Reference<XModuleController> ModuleController::CreateInstance (
    const Reference<XComponentContext>& rxContext)
{
    return new ModuleController(rxContext);
}




ModuleController::ModuleController (const Reference<XComponentContext>& rxContext) throw()
    : ModuleControllerInterfaceBase(MutexOwner::maMutex),
      mxController(),
      mpResourceToFactoryMap(new ResourceToFactoryMap()),
      mpLoadedFactories(new LoadedFactoryContainer())
{
    (void)rxContext;
    LoadFactories(rxContext);
}




ModuleController::~ModuleController (void) throw()
{
}




void SAL_CALL ModuleController::disposing (void)
{
    // Break the cyclic reference back to DrawController object
    mpLoadedFactories.reset();
    mpResourceToFactoryMap.reset();
    mxController.clear();
}




void ModuleController::LoadFactories (const Reference<XComponentContext>& rxContext)
{
    try
    {
        ConfigurationAccess aConfiguration (
            rxContext,
            "/org.openoffice.Office.Impress/",
            ConfigurationAccess::READ_ONLY);
        Reference<container::XNameAccess> xFactories (
            aConfiguration.GetConfigurationNode("MultiPaneGUI/Framework/ResourceFactories"),
            UNO_QUERY);
        ::std::vector<rtl::OUString> aProperties (snFactoryPropertyCount);
        aProperties[0] = "ServiceName";
        aProperties[1] = "ResourceList";
        ConfigurationAccess::ForAll(
            xFactories,
            aProperties,
            ::boost::bind(&ModuleController::ProcessFactory, this, _2));
    }
    catch (Exception&)
    {
        DBG_UNHANDLED_EXCEPTION();
    }
}




void ModuleController::ProcessFactory (const ::std::vector<Any>& rValues)
{
    OSL_ASSERT(rValues.size() == snFactoryPropertyCount);

    // Get the service name of the factory.
    rtl::OUString sServiceName;
    rValues[0] >>= sServiceName;

    // Get all resource URLs that are created by the factory.
    Reference<container::XNameAccess> xResources (rValues[1], UNO_QUERY);
    ::std::vector<rtl::OUString> aURLs;
    tools::ConfigurationAccess::FillList(
        xResources,
        "URL",
        aURLs);

    SAL_INFO("sd.fwk", OSL_THIS_FUNC << ": ModuleController::adding factory " <<
        OUStringToOString(sServiceName, RTL_TEXTENCODING_UTF8).getStr());

    // Add the resource URLs to the map.
    ::std::vector<rtl::OUString>::const_iterator iResource;
    for (iResource=aURLs.begin(); iResource!=aURLs.end(); ++iResource)
    {
        (*mpResourceToFactoryMap)[*iResource] = sServiceName;
        SAL_INFO("sd.fwk", OSL_THIS_FUNC << ":    " <<
            OUStringToOString(*iResource, RTL_TEXTENCODING_UTF8).getStr());
    }
}




void ModuleController::InstantiateStartupServices (void)
{
    try
    {
        tools::ConfigurationAccess aConfiguration (
            "/org.openoffice.Office.Impress/",
            tools::ConfigurationAccess::READ_ONLY);
        Reference<container::XNameAccess> xFactories (
            aConfiguration.GetConfigurationNode("MultiPaneGUI/Framework/StartupServices"),
            UNO_QUERY);
        ::std::vector<rtl::OUString> aProperties (snStartupPropertyCount);
        aProperties[0] = "ServiceName";
        tools::ConfigurationAccess::ForAll(
            xFactories,
            aProperties,
            ::boost::bind(&ModuleController::ProcessStartupService, this, _2));
    }
    catch (Exception&)
    {
        OSL_TRACE("ERROR in ModuleController::InstantiateStartupServices");
    }
}




void ModuleController::ProcessStartupService (const ::std::vector<Any>& rValues)
{
    OSL_ASSERT(rValues.size() == snStartupPropertyCount);

    try
    {
        // Get the service name of the startup service.
        rtl::OUString sServiceName;
        rValues[0] >>= sServiceName;

        // Instantiate service.
        Reference<lang::XMultiServiceFactory> xGlobalFactory (
            ::comphelper::getProcessServiceFactory(), UNO_QUERY);
        if (xGlobalFactory.is())
        {
            // Create the startup service.
            Sequence<Any> aArguments(1);
            aArguments[0] <<= mxController;
            // Note that when the new object will be destroyed at the end of
            // this scope when it does not register itself anywhere.
            // Typically it will add itself as ConfigurationChangeListener
            // at the configuration controller.
            xGlobalFactory->createInstanceWithArguments(sServiceName, aArguments);

            SAL_INFO("sd.fwk", OSL_THIS_FUNC << ": ModuleController::created startup service " <<
                OUStringToOString(sServiceName, RTL_TEXTENCODING_UTF8).getStr());
        }
    }
    catch (Exception&)
    {
        OSL_TRACE("ERROR in ModuleController::ProcessStartupServices");
    }
}




//----- XModuleController -----------------------------------------------------

void SAL_CALL ModuleController::requestResource (const OUString& rsResourceURL)
    throw (RuntimeException)
{
    ResourceToFactoryMap::const_iterator iFactory (mpResourceToFactoryMap->find(rsResourceURL));
    if (iFactory != mpResourceToFactoryMap->end())
    {
        // Check that the factory has already been loaded and not been
        // destroyed in the meantime.
        Reference<XInterface> xFactory;
        LoadedFactoryContainer::const_iterator iLoadedFactory (
            mpLoadedFactories->find(iFactory->second));
        if (iLoadedFactory != mpLoadedFactories->end())
            xFactory = Reference<XInterface>(iLoadedFactory->second, UNO_QUERY);
        if ( ! xFactory.is())
        {
            // Create a new instance of the factory.
            Reference<lang::XMultiServiceFactory> xGlobalFactory (
                ::comphelper::getProcessServiceFactory(), UNO_QUERY);
            if (xGlobalFactory.is())
            {
                // Create the factory service.
                Sequence<Any> aArguments(1);
                aArguments[0] <<= mxController;
                xFactory = xGlobalFactory->createInstanceWithArguments(
                    iFactory->second,
                    aArguments);

                // Remember that this factory has been instanced.
                (*mpLoadedFactories)[iFactory->second] = xFactory;
            }
        }
    }
}




//----- XInitialization -------------------------------------------------------

void SAL_CALL ModuleController::initialize (const Sequence<Any>& aArguments)
    throw (Exception, RuntimeException)
{
    if (aArguments.getLength() > 0)
    {
        try
        {
            // Get the XController from the first argument.
            mxController = Reference<frame::XController>(aArguments[0], UNO_QUERY_THROW);

            InstantiateStartupServices();
        }
        catch (RuntimeException&)
        {}
    }
}


} } // end of namespace sd::framework

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

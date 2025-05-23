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

#include <accelerators/documentacceleratorconfiguration.hxx>

#include <xml/acceleratorconfigurationreader.hxx>

#include <xml/acceleratorconfigurationwriter.hxx>

#include <xml/saxnamespacefilter.hxx>

#include <threadhelp/readguard.hxx>
#include <threadhelp/writeguard.hxx>

#include <acceleratorconst.h>
#include <services.h>

#include <com/sun/star/io/XActiveDataSource.hpp>

#include <com/sun/star/io/XSeekable.hpp>

#include <com/sun/star/io/XTruncate.hpp>

#include <com/sun/star/embed/ElementModes.hpp>

#include <com/sun/star/xml/sax/InputSource.hpp>

#include <com/sun/star/xml/sax/XParser.hpp>

#include <comphelper/sequenceashashmap.hxx>


namespace framework
{

//-----------------------------------------------
// XInterface, XTypeProvider, XServiceInfo
DEFINE_XINTERFACE_2(DocumentAcceleratorConfiguration                   ,
                    XMLBasedAcceleratorConfiguration                           ,
                    DIRECT_INTERFACE(css::lang::XServiceInfo)          ,
                    DIRECT_INTERFACE(css::lang::XInitialization))
//                    DIRECT_INTERFACE(css::ui::XUIConfigurationStorage))

DEFINE_XTYPEPROVIDER_2_WITH_BASECLASS(DocumentAcceleratorConfiguration ,
                                      XMLBasedAcceleratorConfiguration         ,
                                      css::lang::XServiceInfo          ,
                                      css::lang::XInitialization)
//                                      css::ui::XUIConfigurationStorage)

DEFINE_XSERVICEINFO_MULTISERVICE(DocumentAcceleratorConfiguration                   ,
                                 ::cppu::OWeakObject                                ,
                                 SERVICENAME_DOCUMENTACCELERATORCONFIGURATION       ,
                                 IMPLEMENTATIONNAME_DOCUMENTACCELERATORCONFIGURATION)

DEFINE_INIT_SERVICE(DocumentAcceleratorConfiguration,
                    {
                        /*Attention
                        I think we don't need any mutex or lock here ... because we are called by our own static method impl_createInstance()
                        to create a new instance of this class by our own supported service factory.
                        see macro DEFINE_XSERVICEINFO_MULTISERVICE and "impl_initService()" for further informations!
                        */
                    }
                   )

//-----------------------------------------------
DocumentAcceleratorConfiguration::DocumentAcceleratorConfiguration(const css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR)
    : XMLBasedAcceleratorConfiguration(xSMGR)
{
}

//-----------------------------------------------
DocumentAcceleratorConfiguration::~DocumentAcceleratorConfiguration()
{
    m_aPresetHandler.removeStorageListener(this);
}

//-----------------------------------------------
void SAL_CALL DocumentAcceleratorConfiguration::initialize(const css::uno::Sequence< css::uno::Any >& lArguments)
    throw(css::uno::Exception       ,
          css::uno::RuntimeException)
{
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    ::comphelper::SequenceAsHashMap lArgs(lArguments);
    m_xDocumentRoot = lArgs.getUnpackedValueOrDefault(
                        ::rtl::OUString("DocumentRoot"),
                        css::uno::Reference< css::embed::XStorage >());

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    impl_ts_fillCache();
}

//-----------------------------------------------
void SAL_CALL DocumentAcceleratorConfiguration::setStorage(const css::uno::Reference< css::embed::XStorage >& xStorage)
    throw(css::uno::RuntimeException)
{
    // Attention! xStorage must be accepted too, if it's NULL !

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);
    sal_Bool bForgetOldStorages = m_xDocumentRoot.is();
    m_xDocumentRoot = xStorage;
    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    if (bForgetOldStorages)
        impl_ts_clearCache();

    if (xStorage.is())
        impl_ts_fillCache();
}

//-----------------------------------------------
sal_Bool SAL_CALL DocumentAcceleratorConfiguration::hasStorage()
    throw(css::uno::RuntimeException)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    return m_xDocumentRoot.is();
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
void DocumentAcceleratorConfiguration::impl_ts_fillCache()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xDocumentRoot = m_xDocumentRoot;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // Sometimes we must live without a document root.
    // E.g. if the document is readonly ...
    if (!xDocumentRoot.is())
        return;

    // get current office locale ... but dont cache it.
    // Otherwise we must be listener on the configuration layer
    // which seems to superflous for this small implementation .-)
    ::comphelper::Locale aLocale = impl_ts_getLocale();

    // May be the current document does not contain any
    // accelerator config? Handle it gracefully :-)
    try
    {
        // Note: The used preset class is threadsafe by itself ... and live if we live!
        // We do not need any mutex here.

        // open the folder, where the configuration exists
        m_aPresetHandler.connectToResource(
            PresetHandler::E_DOCUMENT,
            PresetHandler::RESOURCETYPE_ACCELERATOR(),
            ::rtl::OUString(),
            xDocumentRoot,
            aLocale);

        DocumentAcceleratorConfiguration::reload();
        m_aPresetHandler.addStorageListener(this);
    }
    catch(const css::uno::Exception&)
    {}
}

//-----------------------------------------------
void DocumentAcceleratorConfiguration::impl_ts_clearCache()
{
    m_aPresetHandler.forgetCachedStorages();
}

} // namespace framework

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

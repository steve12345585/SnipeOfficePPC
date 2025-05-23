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


#include "BasicToolBarFactory.hxx"

#include "ViewTabBar.hxx"
#include "framework/FrameworkHelper.hxx"
#include <comphelper/mediadescriptor.hxx>

#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include "DrawController.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::drawing::framework;

namespace sd { namespace framework {


Reference<XInterface> SAL_CALL BasicToolBarFactory_createInstance (
    const Reference<XComponentContext>& rxContext)
{
    return static_cast<XWeak*>(new BasicToolBarFactory(rxContext));
}




::rtl::OUString BasicToolBarFactory_getImplementationName (void) throw(RuntimeException)
{
    return ::rtl::OUString("com.sun.star.comp.Draw.framework.BasicToolBarFactory");
}




Sequence<rtl::OUString> SAL_CALL BasicToolBarFactory_getSupportedServiceNames (void)
    throw (RuntimeException)
{
    const ::rtl::OUString sServiceName("com.sun.star.drawing.framework.BasicToolBarFactory");
    return Sequence<rtl::OUString>(&sServiceName, 1);
}





//===== BasicToolBarFactory ===================================================

BasicToolBarFactory::BasicToolBarFactory (
    const Reference<XComponentContext>& rxContext)
    : BasicToolBarFactoryInterfaceBase(m_aMutex),
      mxConfigurationController(),
      mxController(),
      mpViewShellBase(NULL)
{
    (void)rxContext;
}




BasicToolBarFactory::~BasicToolBarFactory (void)
{
}




void SAL_CALL BasicToolBarFactory::disposing (void)
{
    Shutdown();
}




void BasicToolBarFactory::Shutdown (void)
{
    mpViewShellBase = NULL;
    Reference<lang::XComponent> xComponent (mxConfigurationController, UNO_QUERY);
    if (xComponent.is())
        xComponent->removeEventListener(static_cast<lang::XEventListener*>(this));
    if (mxConfigurationController.is())
    {
        mxConfigurationController->removeResourceFactoryForReference(this);
        mxConfigurationController = NULL;
    }
}




//----- XInitialization -------------------------------------------------------

void SAL_CALL BasicToolBarFactory::initialize (const Sequence<Any>& aArguments)
    throw (Exception, RuntimeException)
{
    if (aArguments.getLength() > 0)
    {
        try
        {
            // Get the XController from the first argument.
            mxController = Reference<frame::XController>(aArguments[0], UNO_QUERY_THROW);

            // Tunnel through the controller to obtain a ViewShellBase.
            Reference<lang::XUnoTunnel> xTunnel (mxController, UNO_QUERY_THROW);
            ::sd::DrawController* pController = reinterpret_cast<sd::DrawController*>(
                xTunnel->getSomething(sd::DrawController::getUnoTunnelId()));
            if (pController != NULL)
                mpViewShellBase = pController->GetViewShellBase();

            ::comphelper::MediaDescriptor aDescriptor (mxController->getModel()->getArgs());
            if ( ! aDescriptor.getUnpackedValueOrDefault(
                ::comphelper::MediaDescriptor::PROP_PREVIEW(),
                sal_False))
            {
                // Register the factory for its supported tool bars.
                Reference<XControllerManager> xControllerManager(mxController, UNO_QUERY_THROW);
                mxConfigurationController = xControllerManager->getConfigurationController();
                if (mxConfigurationController.is())
                {
                    mxConfigurationController->addResourceFactory(
                        FrameworkHelper::msViewTabBarURL, this);
                }

                Reference<lang::XComponent> xComponent (mxConfigurationController, UNO_QUERY);
                if (xComponent.is())
                    xComponent->addEventListener(static_cast<lang::XEventListener*>(this));
            }
            else
            {
                // The view shell is in preview mode and thus does not need
                // the view tab bar.
                mxConfigurationController = NULL;
            }
        }
        catch (RuntimeException&)
        {
            Shutdown();
            throw;
        }
    }
}




//----- lang::XEventListener --------------------------------------------------

void SAL_CALL BasicToolBarFactory::disposing (
    const lang::EventObject& rEventObject)
    throw (RuntimeException)
{
    if (rEventObject.Source == mxConfigurationController)
        mxConfigurationController = NULL;
}




//===== XPaneFactory ==========================================================

Reference<XResource> SAL_CALL BasicToolBarFactory::createResource (
    const Reference<XResourceId>& rxToolBarId)
    throw (RuntimeException, IllegalArgumentException, WrappedTargetException)
{
    ThrowIfDisposed();

    Reference<XResource> xToolBar;

    if (rxToolBarId->getResourceURL().equals(FrameworkHelper::msViewTabBarURL))
    {
        xToolBar = new ViewTabBar(rxToolBarId, mxController);
    }
    else
        throw lang::IllegalArgumentException();


    return xToolBar;
}





void SAL_CALL BasicToolBarFactory::releaseResource (
    const Reference<XResource>& rxToolBar)
    throw (RuntimeException)
{
    ThrowIfDisposed();

    Reference<XComponent> xComponent (rxToolBar, UNO_QUERY);
    if (xComponent.is())
        xComponent->dispose();
}




void BasicToolBarFactory::ThrowIfDisposed (void) const
    throw (lang::DisposedException)
{
    if (rBHelper.bDisposed || rBHelper.bInDispose)
    {
        throw lang::DisposedException ("BasicToolBarFactory object has already been disposed",
            const_cast<uno::XWeak*>(static_cast<const uno::XWeak*>(this)));
    }
}




} } // end of namespace sd::framework

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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
    static const ::rtl::OUString sServiceName("com.sun.star.drawing.framework.BasicToolBarFactory");
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

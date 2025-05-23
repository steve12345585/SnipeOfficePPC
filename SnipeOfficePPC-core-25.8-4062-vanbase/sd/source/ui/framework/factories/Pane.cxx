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


#include "framework/Pane.hxx"

#include <vcl/svapp.hxx>
#include <osl/mutex.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <vcl/window.hxx>
#include <cppcanvas/vclfactory.hxx>
#include <comphelper/servicehelper.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing::framework;

using ::rtl::OUString;

namespace sd { namespace framework {

Pane::Pane (
    const Reference<XResourceId>& rxPaneId,
    ::Window* pWindow)
    throw ()
    : PaneInterfaceBase(MutexOwner::maMutex),
      mxPaneId(rxPaneId),
      mpWindow(pWindow),
      mxWindow(VCLUnoHelper::GetInterface(pWindow))
{
}




Pane::~Pane (void)
{
}




void Pane::disposing (void)
{
    mxWindow = NULL;
    mpWindow = NULL;
}




::Window* Pane::GetWindow (void)
{
    if (mxWindow.is())
        return mpWindow;
    else
        return NULL;
}




//----- XPane -----------------------------------------------------------------

Reference<awt::XWindow> SAL_CALL Pane::getWindow (void)
    throw (RuntimeException)
{
    ThrowIfDisposed();

    return mxWindow;
}




Reference<rendering::XCanvas> SAL_CALL Pane::getCanvas (void)
    throw (RuntimeException)
{
    ::osl::MutexGuard aGuard (maMutex);
    ThrowIfDisposed();

    if ( ! mxCanvas.is())
        mxCanvas = CreateCanvas();

    return mxCanvas;
}




//----- XPane2 ----------------------------------------------------------------

sal_Bool SAL_CALL Pane::isVisible (void)
    throw (RuntimeException)
{
    ThrowIfDisposed();

    const ::Window* pWindow = GetWindow();
    if (pWindow != NULL)
        return pWindow->IsVisible();
    else
        return false;
}




void SAL_CALL Pane::setVisible (sal_Bool bIsVisible)
    throw (RuntimeException)
{
    ThrowIfDisposed();

    ::Window* pWindow = GetWindow();
    if (pWindow != NULL)
        pWindow->Show(bIsVisible);
}




Reference<accessibility::XAccessible> SAL_CALL Pane::getAccessible (void)
    throw (RuntimeException)
{
    ThrowIfDisposed();
    ::Window* pWindow = GetWindow();
    if (pWindow != NULL)
        return pWindow->GetAccessible(sal_False);
    else
        return NULL;
}




void SAL_CALL Pane::setAccessible (
    const Reference<accessibility::XAccessible>& rxAccessible)
    throw (RuntimeException)
{
    ThrowIfDisposed();
    ::Window* pWindow = GetWindow();
    if (pWindow != NULL)
        pWindow->SetAccessible(rxAccessible);
}




//----- XResource -------------------------------------------------------------

Reference<XResourceId> SAL_CALL Pane::getResourceId (void)
    throw (RuntimeException)
{
    ThrowIfDisposed();

    return mxPaneId;
}




sal_Bool SAL_CALL Pane::isAnchorOnly (void)
    throw (RuntimeException)
{
    return true;
}




//----- XUnoTunnel ------------------------------------------------------------

namespace
{
    class thePaneUnoTunnelId : public rtl::Static< UnoTunnelIdInit, thePaneUnoTunnelId > {};
}

const Sequence<sal_Int8>& Pane::getUnoTunnelId (void)
{
    return thePaneUnoTunnelId::get().getSeq();
}

sal_Int64 SAL_CALL Pane::getSomething (const Sequence<sal_Int8>& rId)
    throw (RuntimeException)
{
    sal_Int64 nResult = 0;

    if (rId.getLength() == 16
        && memcmp(getUnoTunnelId().getConstArray(), rId.getConstArray(), 16) == 0)
    {
        nResult = reinterpret_cast<sal_Int64>(this);
    }

    return nResult;
}




//-----------------------------------------------------------------------------

Reference<rendering::XCanvas> Pane::CreateCanvas (void)
    throw (RuntimeException)
{
    Reference<rendering::XCanvas> xCanvas;

    if (mpWindow != NULL)
    {
        ::cppcanvas::SpriteCanvasSharedPtr pCanvas (
            ::cppcanvas::VCLFactory::getInstance().createSpriteCanvas(*mpWindow));
        if (pCanvas.get() != NULL)
            xCanvas = Reference<rendering::XCanvas>(pCanvas->getUNOSpriteCanvas(), UNO_QUERY);
    }

    return xCanvas;
}




void Pane::ThrowIfDisposed (void) const
    throw (lang::DisposedException)
{
    if (rBHelper.bDisposed || rBHelper.bInDispose)
    {
        throw lang::DisposedException ("Pane object has already been disposed",
            const_cast<uno::XWeak*>(static_cast<const uno::XWeak*>(this)));
    }
}


} } // end of namespace sd::framework

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

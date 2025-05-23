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

#ifndef __FRAMEWORK_PATTERN_WINDOW_HXX_
#define __FRAMEWORK_PATTERN_WINDOW_HXX_

#include <general.h>

#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XTopWindow.hpp>

#include <toolkit/unohlp.hxx>
#include <vcl/window.hxx>
#include <vcl/syswin.hxx>
#include <vcl/wrkwin.hxx>
#include <vcl/svapp.hxx>
#include <osl/mutex.hxx>
#include <rtl/ustring.hxx>

//_______________________________________________
// namespaces

namespace framework{


class WindowHelper
{
    public:

//-----------------------------------------------
static ::rtl::OUString getWindowState(const css::uno::Reference< css::awt::XWindow >& xWindow)
{
    if (!xWindow.is())
        return ::rtl::OUString();

    rtl::OString sWindowState;
    // SOLAR SAFE -> ----------------------------
    {
        SolarMutexGuard aSolarGuard;

        Window*    pWindow     = VCLUnoHelper::GetWindow(xWindow);
        // check for system window is neccessary to guarantee correct pointer cast!
        if (pWindow!=NULL && pWindow->IsSystemWindow())
        {
            sal_uLong nMask  = WINDOWSTATE_MASK_ALL;
            nMask &= ~(WINDOWSTATE_MASK_MINIMIZED);
            sWindowState = ((SystemWindow*)pWindow)->GetWindowState(nMask);
        }
    }
    // <- SOLAR SAFE ----------------------------

    return rtl::OStringToOUString(sWindowState,RTL_TEXTENCODING_UTF8);
}

//-----------------------------------------------
static void setWindowState(const css::uno::Reference< css::awt::XWindow >& xWindow     ,
                           const ::rtl::OUString&                          sWindowState)
{
    if (
        (!xWindow.is()            ) ||
        (!sWindowState.getLength())
       )
        return;

    // SOLAR SAFE -> ----------------------------
    SolarMutexGuard aSolarGuard;

    Window* pWindow = VCLUnoHelper::GetWindow(xWindow);
    // check for system window is neccessary to guarantee correct pointer cast!
    if (
        (pWindow                  ) &&
        (pWindow->IsSystemWindow()) &&
        (
            // dont overwrite a might existing minimized mode!
            (pWindow->GetType() != WINDOW_WORKWINDOW) ||
            (!((WorkWindow*)pWindow)->IsMinimized() )
        )
       )
    {
        ((SystemWindow*)pWindow)->SetWindowState(OUStringToOString(sWindowState,RTL_TEXTENCODING_UTF8));
    }

    // <- SOLAR SAFE ----------------------------
}

//-----------------------------------------------
static ::sal_Bool isTopWindow(const css::uno::Reference< css::awt::XWindow >& xWindow)
{
    // even child frame containing top level windows (e.g. query designer of database) will be closed
    css::uno::Reference< css::awt::XTopWindow > xTopWindowCheck(xWindow, css::uno::UNO_QUERY);
    if (xTopWindowCheck.is())
    {
        // Note: Toolkit interface XTopWindow sometimes is used by real VCL-child-windows also .-)
        // Be sure that these window is realy a "top system window".
        // Attention ! Checking Window->GetParent() isnt the right approach here.
        // Because sometimes VCL create "implicit border windows" as parents even we created
        // a simple XWindow using the toolkit only .-(
        SolarMutexGuard aSolarGuard;
        Window* pWindow = VCLUnoHelper::GetWindow( xWindow );
        if (
            (pWindow                  ) &&
            (pWindow->IsSystemWindow())
           )
            return sal_True;
    }

    return sal_False;
}

};

} // namespace framework

#endif // __FRAMEWORK_PATTERN_WINDOW_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

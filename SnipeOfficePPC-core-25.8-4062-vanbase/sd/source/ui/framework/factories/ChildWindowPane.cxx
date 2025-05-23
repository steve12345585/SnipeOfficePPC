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


#include "ChildWindowPane.hxx"

#include "PaneDockingWindow.hxx"
#include "ViewShellBase.hxx"
#include "ViewShellManager.hxx"
#include "framework/FrameworkHelper.hxx"
#include <toolkit/helper/vclunohelper.hxx>
#include <vcl/svapp.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing::framework;

namespace sd { namespace framework {


SAL_WNODEPRECATED_DECLARATIONS_PUSH
ChildWindowPane::ChildWindowPane (
    const Reference<XResourceId>& rxPaneId,
    sal_uInt16 nChildWindowId,
    ViewShellBase& rViewShellBase,
    ::std::auto_ptr<SfxShell> pShell)
    : ChildWindowPaneInterfaceBase(rxPaneId,(::Window*)NULL),
      mnChildWindowId(nChildWindowId),
      mrViewShellBase(rViewShellBase),
      mpShell(pShell),
      mbHasBeenActivated(false)
{
    mrViewShellBase.GetViewShellManager()->ActivateShell(mpShell.get());

    SfxViewFrame* pViewFrame = mrViewShellBase.GetViewFrame();
    if (pViewFrame != NULL)
    {
        if (mrViewShellBase.IsActive())
        {
            if (pViewFrame->KnowsChildWindow(mnChildWindowId))
            {
                if (pViewFrame->HasChildWindow(mnChildWindowId))
                {
                    // The ViewShellBase has already been activated.  Make
                    // the child window visible as soon as possible.
                    pViewFrame->SetChildWindow(mnChildWindowId, sal_True);
                    OSL_TRACE("ChildWindowPane:activating now");
                }
                else
                {
                    // The window is created asynchronously.  Rely on the
                    // ConfigurationUpdater to try another update, and with
                    // that another request for this window, in a short
                    // time.
                    OSL_TRACE("ChildWindowPane:activated asynchronously");
                }
            }
            else
            {
                OSL_TRACE("ChildWindowPane:not known");
            }
        }
        else
        {
            // The ViewShellBase has not yet been activated.  Hide the
            // window and wait a little before it is made visible.  See
            // comments in the GetWindow() method for an explanation.
            pViewFrame->SetChildWindow(mnChildWindowId, sal_False);
            OSL_TRACE("ChildWindowPane:base not active");
        }
    }
}
SAL_WNODEPRECATED_DECLARATIONS_POP



ChildWindowPane::~ChildWindowPane (void)
{
}




void ChildWindowPane::Hide (void)
{
    SfxViewFrame* pViewFrame = mrViewShellBase.GetViewFrame();
    if (pViewFrame != NULL)
        if (pViewFrame->KnowsChildWindow(mnChildWindowId))
            if (pViewFrame->HasChildWindow(mnChildWindowId))
                pViewFrame->SetChildWindow(mnChildWindowId, sal_False);

    // Release the window because when the child window is shown again it
    // may use a different window.
    mxWindow = NULL;
}




void SAL_CALL ChildWindowPane::disposing (void)
{
    ::osl::MutexGuard aGuard (maMutex);

    mrViewShellBase.GetViewShellManager()->DeactivateShell(mpShell.get());
    mpShell.reset();

    if (mxWindow.is())
    {
        mxWindow->removeEventListener(this);
    }

    Pane::disposing();
}




::Window* ChildWindowPane::GetWindow (void)
{
    do
    {
        if (mxWindow.is())
            // Window already exists => nothing to do.
            break;

        // When the window is not yet present then obtain it only when the
        // shell has already been activated.  The activation is not
        // necessary for the code to work properly but is used to optimize
        // the layouting and displaying of the window.  When it is made
        // visible to early then some layouting seems to be made twice or at
        // an inconvenient time and the overall process of initializing the
        // Impress takes longer.
        if ( ! mbHasBeenActivated && mpShell.get()!=NULL && ! mpShell->IsActive())
            break;

        mbHasBeenActivated = true;
        SfxViewFrame* pViewFrame = mrViewShellBase.GetViewFrame();
        if (pViewFrame == NULL)
            break;
        // The view frame has to know the child window.  This can be the
        // case, when for example the document is in read-only mode:  the
        // task pane is then not available.
        if ( ! pViewFrame->KnowsChildWindow(mnChildWindowId))
            break;

        pViewFrame->SetChildWindow(mnChildWindowId, sal_True);
        SfxChildWindow* pChildWindow = pViewFrame->GetChildWindow(mnChildWindowId);
        if (pChildWindow == NULL)
            if (pViewFrame->HasChildWindow(mnChildWindowId))
            {
                // The child window is not yet visible.  Ask the view frame
                // to show it and try again to get access to the child
                // window.
                pViewFrame->ShowChildWindow(mnChildWindowId, sal_True);
                pChildWindow = pViewFrame->GetChildWindow(mnChildWindowId);
            }

        // When the child window is still not visible then we have to try later.
        if (pChildWindow == NULL)
            break;

        // From the child window get the docking window and from that the
        // content window that is the container for the actual content.
        PaneDockingWindow* pDockingWindow = dynamic_cast<PaneDockingWindow*>(
            pChildWindow->GetWindow());
        if (pDockingWindow == NULL)
            break;

        // At last, we have access to the window and its UNO wrapper.
        mpWindow = &pDockingWindow->GetContentWindow();
        mxWindow = VCLUnoHelper::GetInterface(mpWindow);

        // Register as window listener to be informed when the child window
        // is hidden.
        if (mxWindow.is())
            mxWindow->addEventListener(this);
    }
    while (false);

    return mpWindow;
}




Reference<awt::XWindow> SAL_CALL ChildWindowPane::getWindow (void)
    throw (RuntimeException)
{
    if (mpWindow == NULL || ! mxWindow.is())
        GetWindow();
    return Pane::getWindow();
}



IMPLEMENT_FORWARD_XINTERFACE2(
    ChildWindowPane,
    ChildWindowPaneInterfaceBase,
    Pane);
IMPLEMENT_FORWARD_XTYPEPROVIDER2(
    ChildWindowPane,
    ChildWindowPaneInterfaceBase,
    Pane);




//----- XEventListener --------------------------------------------------------

void SAL_CALL ChildWindowPane::disposing (const lang::EventObject& rEvent)
    throw (RuntimeException)
{
    ThrowIfDisposed();

    if (rEvent.Source == mxWindow)
    {
        // The window is gone but the pane remains alive.  The next call to
        // GetWindow() may create the window anew.
        mxWindow = NULL;
        mpWindow = NULL;
    }
}




} } // end of namespace sd::framework

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

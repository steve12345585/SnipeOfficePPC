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


#include "PaneHider.hxx"

#include "ViewShell.hxx"
#include "ViewShellBase.hxx"
#include "slideshow.hxx"
#include "slideshowimpl.hxx"
#include "framework/FrameworkHelper.hxx"
#include "framework/ConfigurationController.hxx"

#include <com/sun/star/drawing/framework/XControllerManager.hpp>
#include <com/sun/star/drawing/framework/XConfigurationController.hpp>
#include <com/sun/star/drawing/framework/XConfiguration.hpp>
#include <com/sun/star/lang/DisposedException.hpp>

#include <tools/diagnose_ex.h>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing::framework;
using ::sd::framework::FrameworkHelper;
using ::com::sun::star::lang::DisposedException;

namespace sd {

PaneHider::PaneHider (const ViewShell& rViewShell, SlideshowImpl* pSlideShow)
    : mrViewShell(rViewShell),
      mbWindowVisibilitySaved(false),
      mbOriginalLeftPaneWindowVisibility(false),
      mbOriginalRightPaneWindowVisibility(false)
{
     // Hide the left and right pane windows when a slideshow exists and is
    // not full screen.
    if (pSlideShow!=NULL && !pSlideShow->isFullScreen()) try
    {
        Reference<XControllerManager> xControllerManager (
            mrViewShell.GetViewShellBase().GetController(), UNO_QUERY_THROW);
        mxConfigurationController = xControllerManager->getConfigurationController();
        if (mxConfigurationController.is())
        {
            // Get and save the current configuration.
            mxConfiguration = mxConfigurationController->getRequestedConfiguration();
            if (mxConfiguration.is())
            {
                // Iterate over the resources and deactivate the panes.
                Sequence<Reference<XResourceId> > aResources (
                    mxConfiguration->getResources(
                        NULL,
                        framework::FrameworkHelper::msPaneURLPrefix,
                        AnchorBindingMode_DIRECT));
                for (sal_Int32 nIndex=0; nIndex<aResources.getLength(); ++nIndex)
                {
                    Reference<XResourceId> xPaneId (aResources[nIndex]);
                    if ( ! xPaneId->getResourceURL().equals(FrameworkHelper::msCenterPaneURL))
                    {
                        mxConfigurationController->requestResourceDeactivation(xPaneId);
                    }
                }
            }
        }
        FrameworkHelper::Instance(mrViewShell.GetViewShellBase())->WaitForUpdate();
    }
    catch (RuntimeException&)
    {
        DBG_UNHANDLED_EXCEPTION();
    }
}




PaneHider::~PaneHider (void)
{
    if (mxConfiguration.is() && mxConfigurationController.is())
    {
        try
        {
            mxConfigurationController->restoreConfiguration(mxConfiguration);
        }
        catch (DisposedException&)
        {
            // When the configuration controller is already disposed then
            // there is no point in restoring the configuration.
        }
    }
}


} // end of namespace sd

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

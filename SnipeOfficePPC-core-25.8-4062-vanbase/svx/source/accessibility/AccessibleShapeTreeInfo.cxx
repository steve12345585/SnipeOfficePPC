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


#include <svx/AccessibleShapeTreeInfo.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::accessibility;
using ::com::sun::star::uno::Reference;

namespace accessibility {

AccessibleShapeTreeInfo::AccessibleShapeTreeInfo (void)
    : mpView (NULL),
      mpWindow (NULL),
      mpViewForwarder (NULL)
{
    // Empty.
}




AccessibleShapeTreeInfo::AccessibleShapeTreeInfo (const AccessibleShapeTreeInfo& rInfo)
    : mxDocumentWindow (rInfo.mxDocumentWindow),
      mxModelBroadcaster (rInfo.mxModelBroadcaster),
      mpView (rInfo.mpView),
      mxController (rInfo.mxController),
      mpWindow (rInfo.mpWindow),
      mpViewForwarder (rInfo.mpViewForwarder)
{
    // Empty.
}




AccessibleShapeTreeInfo& AccessibleShapeTreeInfo::operator= (const AccessibleShapeTreeInfo& rInfo)
{
    if ( this != &rInfo )
    {
        mxDocumentWindow = rInfo.mxDocumentWindow;
        mxModelBroadcaster = rInfo.mxModelBroadcaster;
        mpView = rInfo.mpView;
        mxController = rInfo.mxController,
        mpWindow = rInfo.mpWindow;
        mpViewForwarder = rInfo.mpViewForwarder;
    }
    return *this;
}




AccessibleShapeTreeInfo::~AccessibleShapeTreeInfo (void)
{
    //empty
}




void AccessibleShapeTreeInfo::SetDocumentWindow (
    const Reference<XAccessibleComponent>& rxDocumentWindow)
{
    if (mxDocumentWindow != rxDocumentWindow)
        mxDocumentWindow = rxDocumentWindow;
}




void AccessibleShapeTreeInfo::SetControllerBroadcaster (
    const uno::Reference<document::XEventBroadcaster>& rxControllerBroadcaster)
{
    mxModelBroadcaster = rxControllerBroadcaster;
}




uno::Reference<document::XEventBroadcaster>
    AccessibleShapeTreeInfo::GetControllerBroadcaster (void) const
{
    return mxModelBroadcaster;
}




void AccessibleShapeTreeInfo::SetModelBroadcaster (
    const Reference<document::XEventBroadcaster>& rxModelBroadcaster)
{
    mxModelBroadcaster = rxModelBroadcaster;
}




Reference<document::XEventBroadcaster>
        AccessibleShapeTreeInfo::GetModelBroadcaster (void) const
{
    return mxModelBroadcaster;
}




void AccessibleShapeTreeInfo::SetSdrView (SdrView* pView)
{
    mpView = pView;
}




SdrView* AccessibleShapeTreeInfo::GetSdrView (void) const
{
    return mpView;
}




void AccessibleShapeTreeInfo::SetController (
    const Reference<frame::XController>& rxController)
{
    mxController = rxController;
}




Reference<frame::XController>
    AccessibleShapeTreeInfo::GetController (void) const
{
    return mxController;
}




void AccessibleShapeTreeInfo::SetWindow (Window* pWindow)
{
    mpWindow = pWindow;
}




Window* AccessibleShapeTreeInfo::GetWindow (void) const
{
    return mpWindow;
}




void AccessibleShapeTreeInfo::SetViewForwarder (const IAccessibleViewForwarder* pViewForwarder)
{
    mpViewForwarder = pViewForwarder;
}




const IAccessibleViewForwarder* AccessibleShapeTreeInfo::GetViewForwarder (void) const
{
    return mpViewForwarder;
}


} // end of namespace accessibility

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

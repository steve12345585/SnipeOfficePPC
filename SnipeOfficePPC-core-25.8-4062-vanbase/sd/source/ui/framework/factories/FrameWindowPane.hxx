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

#ifndef SD_FRAMEWORK_FRAME_WINDOW_PANE_HXX
#define SD_FRAMEWORK_FRAME_WINDOW_PANE_HXX

#include "framework/Pane.hxx"

#include <com/sun/star/drawing/framework/XResourceId.hpp>


namespace sd { namespace framework {

/** This subclass is not necessary anymore.  We can remove it if that
    remains so.
*/
class FrameWindowPane
    : public Pane
{
public:
    FrameWindowPane (
        const ::com::sun::star::uno::Reference<
            com::sun::star::drawing::framework::XResourceId>& rxPaneId,
        ::Window* pWindow);
    virtual ~FrameWindowPane (void) throw();

    /** A frame window typically can (and should) exists on its own without
        children, if only to visualize that something (a view) is missing.
        Therefore this method always returns <FALSE/>.
    */
    virtual sal_Bool SAL_CALL isAnchorOnly (void)
        throw (com::sun::star::uno::RuntimeException);
};

} } // end of namespace sd::framework

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

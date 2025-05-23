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

#ifndef _SIDEBARWINACC_HXX
#define _SIDEBARWINACC_HXX

#include <toolkit/awt/vclxwindow.hxx>

class ViewShell;
class SwSidebarItem;
class SwFrm;

namespace sw { namespace sidebarwindows {

class SwSidebarWin;

class SidebarWinAccessible : public VCLXWindow
{
    public:
        explicit SidebarWinAccessible( SwSidebarWin& rSidebarWin,
                                       ViewShell& rViewShell,
                                       const SwSidebarItem& rSidebarItem );
        virtual ~SidebarWinAccessible();

        virtual com::sun::star::uno::Reference< com::sun::star::accessibility::XAccessibleContext >
                CreateAccessibleContext();

        void ChangeSidebarItem( const SwSidebarItem& rSidebarItem );

    private:
        SwSidebarWin& mrSidebarWin;
        ViewShell& mrViewShell;
        const SwFrm* mpAnchorFrm;
        bool bAccContextCreated;
};

} } // end of namespace sw::sidebarwindows

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

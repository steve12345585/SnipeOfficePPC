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

#ifndef SVT_PANELTABBARPEER_HXX
#define SVT_PANELTABBARPEER_HXX

#include "svtaccessiblefactory.hxx"

#include <toolkit/awt/vclxwindow.hxx>

//......................................................................................................................
namespace svt
{
//......................................................................................................................

    class PanelTabBar;
    //====================================================================
    //= PanelTabBarPeer
    //====================================================================
    class PanelTabBarPeer : public VCLXWindow
    {
    public:
        PanelTabBarPeer( PanelTabBar& i_rTabBar );

    protected:
        ~PanelTabBarPeer();

        // VCLXWindow overridables
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > CreateAccessibleContext();

        // XComponent
        void SAL_CALL dispose() throw(::com::sun::star::uno::RuntimeException);

    private:
        AccessibleFactoryAccess m_aAccessibleFactory;
        PanelTabBar*            m_pTabBar;
    };

//........................................................................
} // namespace svt
//........................................................................

#endif // SVT_PANELTABBARPEER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

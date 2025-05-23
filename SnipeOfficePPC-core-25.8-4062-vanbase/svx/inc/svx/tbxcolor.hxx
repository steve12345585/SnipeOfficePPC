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
#ifndef _SVX_TBXCOLOR_HXX
#define _SVX_TBXCOLOR_HXX

#include <com/sun/star/frame/XLayoutManager.hpp>
#include "svx/svxdllapi.h"

//........................................................................
namespace svx
{
//........................................................................

    //====================================================================
    //= ToolboxAccess
    //====================================================================
    class SVX_DLLPUBLIC ToolboxAccess
    {
    private:
        bool                                                    m_bDocking;
        ::rtl::OUString                                         m_sToolboxResName;
        ::com::sun::star::uno::Reference<
            ::com::sun::star::frame::XLayoutManager >   m_xLayouter;

    public:
        ToolboxAccess( const ::rtl::OUString& rToolboxName );

    public:
        /** toggles the toolbox
        */
        void        toggleToolbox() const;

        /** determines whether the toolbox is currently visible
        */
        bool        isToolboxVisible() const;

        /** forces that the toolbox is docked
        */
        inline void forceDocking() { m_bDocking = true; }
    };

//........................................................................
} // namespace svx
//........................................................................

#endif // #ifndef _SVX_TBXCOLOR_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

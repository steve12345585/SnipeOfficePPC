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

#ifndef INCLUDED_DP_GUI_SHARED_HXX
#define INCLUDED_DP_GUI_SHARED_HXX

#include "rtl/instance.hxx"
#include "tools/resmgr.hxx"


namespace dp_gui {

struct DeploymentGuiResMgr :
    public ::rtl::StaticWithInit< ResMgr *, DeploymentGuiResMgr > {
        ResMgr * operator () () {
            return ResMgr::CreateResMgr( "deploymentgui" );
    }
};

class DpGuiResId : public ResId
{
public:
    DpGuiResId( sal_uInt16 nId ):ResId( nId, *DeploymentGuiResMgr::get() ) {}
};

#define DPGUI_RESSTR(i)   DpGuiResId(i).toString()

} // namespace dp_gui

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

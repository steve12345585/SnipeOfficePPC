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

#include "navicfg.hxx"

//------------------------------------------------------------------

//! #define CFGPATH_NAVIPI          "Office.Calc/Navigator"

//------------------------------------------------------------------

ScNavipiCfg::ScNavipiCfg() :
//! ConfigItem( OUString(RTL_CONSTASCII_USTRINGPARAM( CFGPATH_NAVIPI )) ),
    nListMode(0),
    nDragMode(0),
    nRootType(0)
{
}

//------------------------------------------------------------------------

void ScNavipiCfg::SetListMode(sal_uInt16 nNew)
{
    if ( nListMode != nNew )
    {
        nListMode = nNew;
//!     SetModified();
    }
}

void ScNavipiCfg::SetDragMode(sal_uInt16 nNew)
{
    if ( nDragMode != nNew )
    {
        nDragMode = nNew;
//!     SetModified();
    }
}

void ScNavipiCfg::SetRootType(sal_uInt16 nNew)
{
    if ( nRootType != nNew )
    {
        nRootType = nNew;
//!     SetModified();
    }
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#ifndef _SV_SALVD_H
#define _SV_SALVD_H

#include <salvd.hxx>

class WinSalGraphics;

// -----------------
// - SalVirDevData -
// -----------------

class WinSalVirtualDevice : public SalVirtualDevice
{
public:
    HDC                     mhDC;                   // HDC or 0 for Cache Device
    HBITMAP                 mhBmp;                  // Memory Bitmap
    HBITMAP                 mhDefBmp;               // Default Bitmap
    WinSalGraphics*         mpGraphics;             // current VirDev graphics
    WinSalVirtualDevice*    mpNext;                 // next VirDev
    sal_uInt16                  mnBitCount;             // BitCount (0 or 1)
    sal_Bool                    mbGraphics;             // is Graphics used
    sal_Bool                    mbForeignDC;            // uses a foreign DC instead of a bitmap

    WinSalVirtualDevice();
    virtual ~WinSalVirtualDevice();

    virtual SalGraphics*            GetGraphics();
    virtual void                    ReleaseGraphics( SalGraphics* pGraphics );
    virtual sal_Bool                    SetSize( long nNewDX, long nNewDY );
    virtual void                    GetSize( long& rWidth, long& rHeight );
};

#endif // _SV_SALVD_H

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

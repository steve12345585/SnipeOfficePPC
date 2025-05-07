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

#ifndef _SV_SALOBJ_H
#define _SV_SALOBJ_H

#include <salobj.hxx>

// -----------------
// - SalObjectData -
// -----------------

class WinSalObject : public SalObject
{
public:
    HWND                    mhWnd;                  // Window handle
    HWND                    mhWndChild;             // Child Window handle
    HWND                    mhLastFocusWnd;         // Child-Window, welches als letztes den Focus hatte
    SystemChildData         maSysData;              // SystemEnvData
    RGNDATA*                mpClipRgnData;          // ClipRegion-Data
    RGNDATA*                mpStdClipRgnData;       // Cache Standard-ClipRegion-Data
    RECT*                   mpNextClipRect;         // Naechstes ClipRegion-Rect
    sal_Bool                mbFirstClipRect;        // Flag for first cliprect to insert
    sal_Bool                mbInterceptChildWindowKeyDown; // Intercept the KeyDown event sent to system child window
    WinSalObject*               mpNextObject;           // pointer to next object


    WinSalObject();
    virtual ~WinSalObject();

    virtual void                    ResetClipRegion();
    virtual sal_uInt16                  GetClipRegionType();
    virtual void                    BeginSetClipRegion( sal_uIntPtr nRects );
    virtual void                    UnionClipRegion( long nX, long nY, long nWidth, long nHeight );
    virtual void                    EndSetClipRegion();
    virtual void                    SetPosSize( long nX, long nY, long nWidth, long nHeight );
    virtual void                    Show( sal_Bool bVisible );
    virtual void                    Enable( sal_Bool nEnable );
    virtual void                    GrabFocus();
    virtual const SystemEnvData*    GetSystemData() const;
    virtual void InterceptChildWindowKeyDown( sal_Bool bIntercept );
};

#endif // _SV_SALOBJ_H

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#ifndef _BMPWIN_HXX
#define _BMPWIN_HXX

#include <vcl/graph.hxx>
#include <vcl/bitmapex.hxx>
#include <vcl/window.hxx>

/*--------------------------------------------------------------------
    Description:    extended page for graphics
 --------------------------------------------------------------------*/
class BmpWindow : public Window
{
private:
    Graphic     aGraphic;
    BitmapEx    aBmp;

    sal_Bool        bHorz : 1;
    sal_Bool        bVert : 1;
    sal_Bool        bGraphic : 1;
    sal_Bool        bLeftAlign : 1;

    void Paint(const Rectangle& rRect);

public:
    BmpWindow(Window* pPar, sal_uInt16 nId,
                const Graphic& rGraphic, const BitmapEx& rBmp);
    BmpWindow(Window* pParent, const ResId rResId) :
        Window(pParent, rResId),
        bHorz(sal_False), bVert(sal_False),bGraphic(sal_False), bLeftAlign(sal_True) {}
    ~BmpWindow();
    void MirrorVert(sal_Bool bMirror) { bVert = bMirror; Invalidate(); }
    void MirrorHorz(sal_Bool bMirror) { bHorz = bMirror; Invalidate(); }
    void SetGraphic(const Graphic& rGrf);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

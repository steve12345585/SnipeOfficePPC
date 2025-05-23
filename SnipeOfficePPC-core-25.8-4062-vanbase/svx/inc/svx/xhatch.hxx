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

#ifndef _SVX_XHATCH_HXX
#define _SVX_XHATCH_HXX

#include <svx/xenum.hxx>
#include <tools/color.hxx>
#include "svx/svxdllapi.h"

//--------------
// class XHatch
//--------------

class SVX_DLLPUBLIC XHatch
{
protected:
    XHatchStyle     eStyle;
    Color           aColor;
    long            nDistance;
    long            nAngle;

public:
                    XHatch() : eStyle(XHATCH_SINGLE), nDistance(0), nAngle(0) {}
                    XHatch(const Color& rCol, XHatchStyle eStyle = XHATCH_SINGLE,
                           long nDistance = 20, long nAngle = 0);

    bool operator==(const XHatch& rHatch) const;

    void            SetHatchStyle(XHatchStyle eNewStyle) { eStyle = eNewStyle; }
    void            SetColor(const Color& rColor) { aColor = rColor; }
    void            SetDistance(long nNewDistance) { nDistance = nNewDistance; }
    void            SetAngle(long nNewAngle) { nAngle = nNewAngle; }

    XHatchStyle     GetHatchStyle() const { return eStyle; }
    Color           GetColor() const { return aColor; }
    long            GetDistance() const { return nDistance; }
    long            GetAngle() const { return nAngle; }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#ifndef SD_RULER_HXX
#define SD_RULER_HXX


#include <svx/ruler.hxx>


namespace sd {

class DrawViewShell;
class RulerCtrlItem;
class View;
class Window;

class Ruler
    : public SvxRuler
{
public:
    Ruler (
        DrawViewShell& rViewSh,
        ::Window* pParent,
        ::sd::Window* pWin,
        sal_uInt16 nRulerFlags,
        SfxBindings& rBindings,
        WinBits nWinStyle);
    virtual ~Ruler (void);

    void SetNullOffset(const Point& rOffset);

    sal_Bool IsHorizontal() const { return bHorz; }

    using ::Ruler::SetNullOffset;
protected:
    ::sd::View* pSdView;
    ::sd::Window* pSdWin;
    DrawViewShell* pDrViewShell;
    RulerCtrlItem* pCtrlItem;
    sal_Bool bHorz;

    virtual void    MouseButtonDown(const MouseEvent& rMEvt);
    virtual void    MouseButtonUp(const MouseEvent& rMEvt);
    virtual void    MouseMove(const MouseEvent& rMEvt);
    virtual void    Command(const CommandEvent& rCEvt);
    virtual void    ExtraDown();
};

} // end of namespace sd

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

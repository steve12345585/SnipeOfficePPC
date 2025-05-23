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
#ifndef _SVX_COLRCTRL_HXX
#define _SVX_COLRCTRL_HXX

#include <sfx2/dockwin.hxx>
#include <sfx2/childwin.hxx>
#include <svtools/valueset.hxx>
#include <svtools/transfer.hxx>
#include <svl/lstner.hxx>
#include <svx/svxdllapi.h>
#include <svx/xtable.hxx>

class SvData;

/*************************************************************************
|*
|* SvxColorValueSet
|*
\************************************************************************/

class SvxColorValueSet : public ValueSet, public DragSourceHelper
{
    using ValueSet::StartDrag;

private:

    sal_Bool            bLeft;
    Point           aDragPosPixel;

protected:

    void            DoDrag();

    // ValueSet
    virtual void    MouseButtonDown( const MouseEvent& rMEvt );
    virtual void    MouseButtonUp( const MouseEvent& rMEvt );
    virtual void    Command(const CommandEvent& rCEvt );

    // DragSourceHelper
    virtual void    StartDrag( sal_Int8 nAction, const Point& rPtPixel );

                    DECL_STATIC_LINK(SvxColorValueSet, ExecDragHdl, void*);

public:
                    SvxColorValueSet( Window* pParent, const ResId& rResId );

    sal_Bool            IsLeftButton() const { return bLeft; }
};

/*************************************************************************
|*
|* Derivation from SfxChildWindow as "container" for Controller
|*
\************************************************************************/

class SVX_DLLPUBLIC SvxColorChildWindow : public SfxChildWindow
{
 public:
    SvxColorChildWindow( Window*, sal_uInt16, SfxBindings*,
                         SfxChildWinInfo* );

    SFX_DECL_CHILDWINDOW_WITHID(SvxColorChildWindow);
};

/*************************************************************************
|*
|* SvxColorDockingWindow
|*
\************************************************************************/

class SvxColorDockingWindow : public SfxDockingWindow, public SfxListener
{
 friend class SvxColorChildWindow;

private:
    XColorListRef       pColorList;
    SvxColorValueSet    aColorSet;
    sal_uInt16          nLeftSlot;
    sal_uInt16          nRightSlot;
    sal_uInt16          nCols;
    sal_uInt16          nLines;
    long                nCount;
    Size                aColorSize;
    Size                aItemSize;

    void                FillValueSet();
    void                SetSize();
                        DECL_LINK( SelectHdl, void * );

    /** This function is called when the window gets the focus.  It grabs
        the focus to the color value set so that it can be controlled with
        the keyboard.
    */
    virtual void GetFocus (void);

protected:
    virtual sal_Bool    Close();
    virtual void    Resize();
    virtual void    Resizing( Size& rSize );

public:
                    SvxColorDockingWindow( SfxBindings* pBindings,
                                           SfxChildWindow *pCW,
                                           Window* pParent,
                                           const ResId& rResId );
                    ~SvxColorDockingWindow();

    virtual void    Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

    void            SetSlotIDs( sal_uInt16 nLeft, sal_uInt16 nRight )
                        { nLeftSlot = nLeft; nRightSlot = nRight; }

    virtual long    Notify( NotifyEvent& rNEvt );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

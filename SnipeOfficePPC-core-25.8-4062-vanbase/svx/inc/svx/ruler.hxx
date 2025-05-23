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
#ifndef _SVX_RULER_HXX
#define _SVX_RULER_HXX

#include <vcl/menu.hxx>
#include <svtools/ruler.hxx>
#include <svl/lstner.hxx>
#include "svx/svxdllapi.h"

class SvxProtectItem;
class SvxRulerItem;
class SfxBindings;
class SvxLongLRSpaceItem;
class SvxLongULSpaceItem;
class SvxTabStopItem;
class SvxLRSpaceItem;
class SvxPagePosSizeItem;
class SvxColumnItem;
class SfxRectangleItem;
class SvxObjectItem;
class SfxBoolItem;
struct SvxRuler_Impl;

class SVX_DLLPUBLIC SvxRuler: public Ruler, public SfxListener
{
    friend class SvxRulerItem;
    using Window::Notify;

    SvxRulerItem **pCtrlItem;
    SvxLongLRSpaceItem *pLRSpaceItem;  // left and right edge
    SfxRectangleItem *pMinMaxItem; // maxima for dragging
    SvxLongULSpaceItem *pULSpaceItem;  // upper and lower edge
    SvxTabStopItem *pTabStopItem;  // tab stops
    SvxLRSpaceItem *pParaItem;     // paragraphs
    SvxLRSpaceItem *pParaBorderItem; // border distance
    SvxPagePosSizeItem *pPagePosItem; // page distance to the rule
    SvxColumnItem *pColumnItem;    // columns
    SvxObjectItem *pObjectItem;    // object
    Window *pEditWin;
    SvxRuler_Impl *pRuler_Imp;
    sal_Bool bAppSetNullOffset :1;
    sal_Bool bHorz :1;
    long lLogicNullOffset;      // in logic coordinates
    long lAppNullOffset;        // in logic coordinates
    long lMinFrame;             // minimal frame width in pixels
    long lInitialDragPos;
    sal_uInt16 nFlags;
    enum {
        NONE = 0x0000,
        DRAG_OBJECT =  0x0001,
        // reduce size of the last column, shift
        DRAG_OBJECT_SIZE_LINEAR = 0x0002,
        DRAG_OBJECT_SIZE_PROPORTIONAL = 0x0004, // proportional, Ctrl
        // only current line (table; Shift-Ctrl)
        DRAG_OBJECT_ACTLINE_ONLY = 0x0008,
        // currently same key assignment
        DRAG_OBJECT_LEFT_INDENT_ONLY = DRAG_OBJECT_SIZE_PROPORTIONAL
        }
    nDragType;
    sal_uInt16 nDefTabType;
    sal_uInt16 nTabCount;
    sal_uInt16 nTabBufSize;
    long lDefTabDist;
    long lTabPos;
    RulerTab *pTabs;            // tab positions in pixel
    RulerIndent *pIndents;      // paragraph margins in pixel
    RulerBorder *pBorders;
    sal_uInt16 nBorderCount;
    RulerBorder *pObjectBorders;
    SfxBindings *pBindings;
    long nDragOffset;
    long nMaxLeft;
    long nMaxRight;
    sal_Bool bValid;
    sal_Bool bListening;
    sal_Bool bActive;
    void StartListening_Impl();
    long GetCorrectedDragPos( sal_Bool bLeft = sal_True, sal_Bool bRight = sal_True );
    void DrawLine_Impl(long &lTabPos, int, sal_Bool Hori=sal_True);
    sal_uInt16 GetObjectBordersOff(sal_uInt16 nIdx) const;

    // page borders or surrounding frame
    void UpdateFrame(const SvxLongLRSpaceItem *);
    void UpdateFrame(const SvxLongULSpaceItem *);
    void UpdateFrameMinMax(const SfxRectangleItem *);
    // paragraph indentations
    void UpdatePara(const SvxLRSpaceItem *);
    // Border distance
    void UpdateParaBorder(const SvxLRSpaceItem *);
    // Tabs
    void Update(const SvxTabStopItem *);
    // page position and width
    void Update(const SvxPagePosSizeItem *);
    // columns
    void Update(const SvxColumnItem *, sal_uInt16 nSID);
    // object selection
    void Update(const SvxObjectItem *);
    // protect
    void Update( const SvxProtectItem* );
    // left-to-right text
    void UpdateTextRTL( const SfxBoolItem* );
    // paragraph indentations
    void UpdatePara();
    void UpdateTabs();
    void UpdatePage();
    void UpdateFrame();
    void UpdateColumns();
    void UpdateObject();

    long PixelHAdjust(long lPos, long lPos2) const;
    long PixelVAdjust(long lPos, long lPos2) const;
    long PixelAdjust(long lPos, long lPos2) const;

    long ConvertHPosPixel(long lPos) const;
    long ConvertVPosPixel(long lPos) const;
    long ConvertHSizePixel(long lSize) const;
    long ConvertVSizePixel(long lSize) const;

    long ConvertPosPixel(long lPos) const;
    long ConvertSizePixel(long lSize) const;

    long ConvertHPosLogic(long lPos) const;
    long ConvertVPosLogic(long lPos) const;
    long ConvertHSizeLogic(long lSize) const;
    long ConvertVSizeLogic(long lSize) const;

    long ConvertPosLogic(long lPos) const;
    long ConvertSizeLogic(long lSize) const;

    long GetFirstLineIndent() const;
    long GetLeftIndent() const;
    long GetRightIndent() const;
    long GetLogicRightIndent() const;
    long GetPageWidth() const;

    inline long GetLeftFrameMargin() const;
    long GetRightFrameMargin() const;

    void CalcMinMax();

    void EvalModifier();
    void DragMargin1();
    //adjust the left margin either after DragMargin1() or after DragBorders()
    void AdjustMargin1(long lDiff);
    void DragMargin2();
    void DragIndents();
    void DragTabs();
    void DragBorders();
    void DragObjectBorder();

    void ApplyMargins();
    void ApplyIndents();
    void ApplyTabs();
    void ApplyBorders();
    void ApplyObject();

    long GetFrameLeft() const;
    void SetFrameLeft(long);

    long GetLeftMin() const;
    long GetRightMax() const;

    DECL_LINK( TabMenuSelect, Menu * );
    DECL_LINK( MenuSelect, Menu * );
    void PrepareProportional_Impl(RulerType);

    sal_uInt16 GetNextVisible(sal_uInt16 nColumn);
    sal_uInt16 GetPrevVisible(sal_uInt16 nColumn);

    void Update();

enum UpdateType {
    MOVE_ALL,
    MOVE_LEFT,
    MOVE_RIGHT
    };
    void UpdateParaContents_Impl(long lDiff, UpdateType = MOVE_ALL);
protected:
    virtual void    Command( const CommandEvent& rCEvt );
    virtual void    Click();
    virtual long    StartDrag();
    virtual void    Drag();
    virtual void    EndDrag();
    virtual void    ExtraDown();
    virtual void    MouseMove( const MouseEvent& rMEvt );

    virtual void    Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

    // calculation of boundary values for object borders
    // values refer to the page
    virtual sal_Bool    CalcLimits(long &nMax1, long &nMax2, sal_Bool bFirst) const;
    sal_Bool IsActLastColumn(
        sal_Bool bForceDontConsiderHidden = sal_False, sal_uInt16 nAct=USHRT_MAX) const ;
    sal_Bool IsActFirstColumn(
        sal_Bool bForceDontConsiderHidden = sal_False, sal_uInt16 nAct=USHRT_MAX) const;
    sal_uInt16 GetActLeftColumn(
        sal_Bool bForceDontConsiderHidden = sal_False, sal_uInt16 nAct=USHRT_MAX ) const;
    sal_uInt16 GetActRightColumn (
        sal_Bool bForceDontConsiderHidden = sal_False, sal_uInt16 nAct=USHRT_MAX ) const;
    long CalcPropMaxRight(sal_uInt16 nCol=USHRT_MAX) const;

public:
#define     SVXRULER_SUPPORT_TABS                       0x0001
#define     SVXRULER_SUPPORT_PARAGRAPH_MARGINS          0x0002
#define     SVXRULER_SUPPORT_BORDERS                    0x0004
#define     SVXRULER_SUPPORT_OBJECT                     0x0008
#define     SVXRULER_SUPPORT_SET_NULLOFFSET             0x0010
#define     SVXRULER_SUPPORT_NEGATIVE_MARGINS           0x0020
#define     SVXRULER_SUPPORT_PARAGRAPH_MARGINS_VERTICAL 0x0040
#define     SVXRULER_SUPPORT_REDUCED_METRIC             0x0080 //shorten the context menu to select metric

    SvxRuler(Window* pParent,
             Window *pEditWin,
             sal_uInt16 nRulerFlags,
             SfxBindings &rBindings,
             WinBits nWinStyle = WB_STDRULER);
    ~SvxRuler();

    void SetMinFrameSize(long lSize);
    long GetMinFrameSize() const ;

    sal_uInt16 GetRulerFlags() const { return nFlags; }

    void SetDefTabDist(long);

    // set/get NullOffset in logic units
    void SetNullOffsetLogic(long lOff = 0);
    long GetNullOffsetLogic() const { return lAppNullOffset; }

    void SetActive(sal_Bool bOn = sal_True);
    sal_Bool IsActive() const { return bActive; }

    void ForceUpdate() { Update(); }

    //#i24363# tab stops relative to indent
    void SetTabsRelativeToIndent( sal_Bool bRel = sal_True );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

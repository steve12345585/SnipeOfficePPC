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
#ifndef _DVIEW_HXX
#define _DVIEW_HXX

#include <svx/fmview.hxx>

class OutputDevice;
class SwViewImp;
class SwFrm;
class SwFlyFrm;
class SwAnchoredObject;

class SwDrawView : public FmFormView
{
    Point           aAnchorPoint;       // anchor position
    SwViewImp      &rImp;               // a view is always part of a shell

    const SwFrm *CalcAnchor();

    /** determine maximal order number for a 'child' object of given 'parent' object

        The maximal order number will be determined on the current object
        order hierarchy. It's the order number of the 'child' object with the
        highest order number. The calculation can be influenced by parameter
        <_pExclChildObj> - this 'child' object won't be considered.

        @param <_rParentObj>
        input parameter - 'parent' object, for which the maximal order number
        for its 'children' will be determined.

        @param <_pExclChildObj>
        optional input parameter - 'child' object, which will not be considered
        on the calculation of the maximal order number
    */
    sal_uInt32 _GetMaxChildOrdNum( const SwFlyFrm& _rParentObj,
                                   const SdrObject* _pExclChildObj = 0L ) const;

    /** method to move 'repeated' objects of the given moved object to the
        according level

        @param <_rMovedAnchoredObj>
        input parameter - moved object, for which the 'repeated' ones have also
        to be moved.

        @param <_rMovedChildrenObjs>
        input parameter - data collection of moved 'child' objects - the 'repeated'
        ones of these 'children' will also been moved.
    */
    void _MoveRepeatedObjs( const SwAnchoredObject& _rMovedAnchoredObj,
                            const std::vector<SdrObject*>& _rMovedChildObjs ) const;

protected:
    // add custom handles (used by other apps, e.g. AnchorPos)
    virtual void AddCustomHdl();

    // overloaded to allow extra handling when picking SwVirtFlyDrawObj's
    using FmFormView::CheckSingleSdrObjectHit;
    virtual SdrObject* CheckSingleSdrObjectHit(const Point& rPnt, sal_uInt16 nTol, SdrObject* pObj, SdrPageView* pPV, sal_uLong nOptions, const SetOfByte* pMVisLay) const;

public:
    SwDrawView( SwViewImp &rI, SdrModel *pMd, OutputDevice* pOutDev=NULL );

    // from base class
    virtual SdrObject*   GetMaxToTopObj(SdrObject* pObj) const;
    virtual SdrObject*   GetMaxToBtmObj(SdrObject* pObj) const;
    virtual void         MarkListHasChanged();

    // #i7672#
    // Overload to resue edit background color in active text edit view (OutlinerView)
    virtual void ModelHasChanged();

    virtual void         ObjOrderChanged( SdrObject* pObj, sal_uLong nOldPos,
                                            sal_uLong nNewPos );
    virtual sal_Bool TakeDragLimit(SdrDragMode eMode, Rectangle& rRect) const;
    virtual void MakeVisible( const Rectangle&, Window &rWin );
    virtual void CheckPossibilities();

    const SwViewImp &Imp() const { return rImp; }
          SwViewImp &Imp()       { return rImp; }

    // anchor and Xor for dragging
    void ShowDragAnchor();

    virtual void DeleteMarked();

    inline void ValidateMarkList() { FlushComeBackTimer(); }

    // #i99665#
    sal_Bool IsAntiAliasing() const;

    // method to replace marked/selected <SwDrawVirtObj>
    // by its reference object for delete of selection and group selection
    static void ReplaceMarkedDrawVirtObjs( SdrMarkView& _rMarkView );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

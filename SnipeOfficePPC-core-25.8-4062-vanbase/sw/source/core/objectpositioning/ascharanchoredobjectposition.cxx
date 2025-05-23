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

#include <ascharanchoredobjectposition.hxx>
#include <frame.hxx>
#include <txtfrm.hxx>
#include <flyfrms.hxx>
#include <svx/svdobj.hxx>
#include <dcontact.hxx>
#include <frmfmt.hxx>
#include <frmatr.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/ulspitem.hxx>
#include <fmtornt.hxx>

#include <com/sun/star/text/HoriOrientation.hpp>


using namespace ::com::sun::star;
using namespace objectpositioning;

/** constructor

    @author OD
*/
SwAsCharAnchoredObjectPosition::SwAsCharAnchoredObjectPosition(
                                    SdrObject& _rDrawObj,
                                    const Point&    _rProposedAnchorPos,
                                    const AsCharFlags _nFlags,
                                    const SwTwips     _nLineAscent,
                                    const SwTwips     _nLineDescent,
                                    const SwTwips     _nLineAscentInclObjs,
                                    const SwTwips     _nLineDescentInclObjs )
    : SwAnchoredObjectPosition( _rDrawObj ),
      mrProposedAnchorPos( _rProposedAnchorPos ),
      mnFlags( _nFlags ),
      mnLineAscent( _nLineAscent ),
      mnLineDescent( _nLineDescent ),
      mnLineAscentInclObjs( _nLineAscentInclObjs ),
      mnLineDescentInclObjs( _nLineDescentInclObjs ),
      maAnchorPos ( Point() ),
      mnRelPos ( 0 ),
      maObjBoundRect ( SwRect() ),
      mnLineAlignment ( 0 )
{}

/** destructor

    @author OD
*/
SwAsCharAnchoredObjectPosition::~SwAsCharAnchoredObjectPosition()
{}

/** method to cast <SwAnchoredObjectPosition::GetAnchorFrm()> to needed type

    @author OD
*/
const SwTxtFrm& SwAsCharAnchoredObjectPosition::GetAnchorTxtFrm() const
{
    OSL_ENSURE( GetAnchorFrm().ISA(SwTxtFrm),
            "SwAsCharAnchoredObjectPosition::GetAnchorTxtFrm() - wrong anchor frame type" );

    return static_cast<const SwTxtFrm&>(GetAnchorFrm());
}

/** calculate position for object

    OD 30.07.2003 #110978#
    members <maAnchorPos>, <mnRelPos>, <maObjBoundRect> and
    <mnLineAlignment> are calculated.
    calculated position is set at the given object.

    @author OD
*/
void SwAsCharAnchoredObjectPosition::CalcPosition()
{
    const SwTxtFrm& rAnchorFrm = GetAnchorTxtFrm();
    // swap anchor frame, if swapped. Note: destructor takes care of the 'undo'
    SwFrmSwapper aFrmSwapper( &rAnchorFrm, false );

    SWRECTFN( ( &rAnchorFrm ) )

    Point aAnchorPos( mrProposedAnchorPos );

    const SwFrmFmt& rFrmFmt = GetFrmFmt();

    SwRect aObjBoundRect( GetAnchoredObj().GetObjRect() );
    SwTwips nObjWidth = (aObjBoundRect.*fnRect->fnGetWidth)();

    // determine spacing values considering layout-/text-direction
    const SvxLRSpaceItem& rLRSpace = rFrmFmt.GetLRSpace();
    const SvxULSpaceItem& rULSpace = rFrmFmt.GetULSpace();
    SwTwips nLRSpaceLeft, nLRSpaceRight, nULSpaceUpper, nULSpaceLower;
    {
        if ( rAnchorFrm.IsVertical() )
        {
            // Seems to be easier to do it all the horizontal way
            // So, from now on think horizontal.
            rAnchorFrm.SwitchVerticalToHorizontal( aObjBoundRect );
            rAnchorFrm.SwitchVerticalToHorizontal( aAnchorPos );

            // convert the spacing values
            nLRSpaceLeft = rULSpace.GetUpper();
            nLRSpaceRight = rULSpace.GetLower();
            nULSpaceUpper = rLRSpace.GetRight();
            nULSpaceLower = rLRSpace.GetLeft();
        }
        else
        {
            if ( rAnchorFrm.IsRightToLeft() )
            {
                nLRSpaceLeft = rLRSpace.GetRight();
                nLRSpaceRight = rLRSpace.GetLeft();
            }
            else
            {
                nLRSpaceLeft = rLRSpace.GetLeft();
                nLRSpaceRight = rLRSpace.GetRight();
            }

            nULSpaceUpper = rULSpace.GetUpper();
            nULSpaceLower = rULSpace.GetLower();
        }
    }

    // consider left and upper spacing by adjusting anchor position.
    // left spacing is only considered, if requested.
    if( mnFlags & AS_CHAR_ULSPACE )
    {
        aAnchorPos.X() += nLRSpaceLeft;
    }
    aAnchorPos.Y() += nULSpaceUpper;

    // for drawing objects: consider difference between its bounding rectangle
    // and its snapping rectangle by adjusting anchor position.
    // left difference is only considered, if requested.
    if( !IsObjFly() )
    {
        SwRect aSnapRect = GetObject().GetSnapRect();
        if ( rAnchorFrm.IsVertical() )
        {
            rAnchorFrm.SwitchVerticalToHorizontal( aSnapRect );
        }

        if( mnFlags & AS_CHAR_ULSPACE )
        {
            aAnchorPos.X() += aSnapRect.Left() - aObjBoundRect.Left();
        }
        aAnchorPos.Y() += aSnapRect.Top() - aObjBoundRect.Top();
    }

    // enlarge bounding rectangle of object by its spacing.
    aObjBoundRect.Left( aObjBoundRect.Left() - nLRSpaceLeft );
    aObjBoundRect.Width( aObjBoundRect.Width() + nLRSpaceRight );
    aObjBoundRect.Top( aObjBoundRect.Top() - nULSpaceUpper );
    aObjBoundRect.Height( aObjBoundRect.Height() + nULSpaceLower );

    // calculate relative position to given base line.
    const SwFmtVertOrient& rVert = rFrmFmt.GetVertOrient();
    const SwTwips nObjBoundHeight = ( mnFlags & AS_CHAR_ROTATE )
                                    ? aObjBoundRect.Width()
                                    : aObjBoundRect.Height();
    const SwTwips nRelPos = _GetRelPosToBase( nObjBoundHeight, rVert );

    // for initial positioning:
    // adjust the proposed anchor position by difference between
    // calculated relative position to base line and current maximal line ascent.
    // Note: In the following line formatting the base line will be adjusted
    //       by the same difference.
    if( mnFlags & AS_CHAR_INIT && nRelPos < 0 && mnLineAscentInclObjs < -nRelPos )
    {
        if( mnFlags & AS_CHAR_ROTATE )
            aAnchorPos.X() -= mnLineAscentInclObjs + nRelPos;
        else
            aAnchorPos.Y() -= mnLineAscentInclObjs + nRelPos;
    }

    // consider BIDI-multiportion by adjusting proposed anchor position
    if( mnFlags & AS_CHAR_BIDI )
        aAnchorPos.X() -= aObjBoundRect.Width();

    // calculate relative position considering rotation and inside rotation
    // reverse direction.
    Point aRelPos;
    {
        if( mnFlags & AS_CHAR_ROTATE )
        {
            if( mnFlags & AS_CHAR_REVERSE )
                aRelPos.X() = -nRelPos - aObjBoundRect.Width();
            else
            {
                aRelPos.X() = nRelPos;
                aRelPos.Y() = -aObjBoundRect.Height();
            }
        }
        else
            aRelPos.Y() = nRelPos;
    }

    if( !IsObjFly() )
    {
        if( !( mnFlags & AS_CHAR_QUICK ) )
        {
            // save calculated Y-position value for 'automatic' vertical positioning,
            // in order to avoid a switch to 'manual' vertical positioning in
            // <SwDrawContact::_Changed(..)>.
            const sal_Int16 eVertOrient = rVert.GetVertOrient();
            if( rVert.GetPos() != nRelPos && eVertOrient != text::VertOrientation::NONE )
            {
                SwFmtVertOrient aVert( rVert );
                aVert.SetPos( nRelPos );
                const_cast<SwFrmFmt&>(rFrmFmt).LockModify();
                const_cast<SwFrmFmt&>(rFrmFmt).SetFmtAttr( aVert );
                const_cast<SwFrmFmt&>(rFrmFmt).UnlockModify();
            }

            // determine absolute anchor position considering layout directions.
            // Note: Use copy of <aAnchorPos>, because it's needed for
            //       setting relative position.
            Point aAbsAnchorPos( aAnchorPos );
            if ( rAnchorFrm.IsRightToLeft() )
            {
                rAnchorFrm.SwitchLTRtoRTL( aAbsAnchorPos );
                aAbsAnchorPos.X() -= nObjWidth;
            }
            if ( rAnchorFrm.IsVertical() )
                rAnchorFrm.SwitchHorizontalToVertical( aAbsAnchorPos );

            // set proposed anchor position at the drawing object.
            // OD 2004-04-06 #i26791# - distinction between 'master' drawing
            // object and 'virtual' drawing object no longer needed.
            GetObject().SetAnchorPos( aAbsAnchorPos );

            // move drawing object to set its correct relative position.
            {
                SwRect aSnapRect = GetObject().GetSnapRect();
                if ( rAnchorFrm.IsVertical() )
                    rAnchorFrm.SwitchVerticalToHorizontal( aSnapRect );

                Point aDiff;
                if ( rAnchorFrm.IsRightToLeft() )
                    aDiff = aRelPos + aAbsAnchorPos - aSnapRect.TopLeft();
                else
                    aDiff = aRelPos + aAnchorPos - aSnapRect.TopLeft();

                if ( rAnchorFrm.IsVertical() )
                    aDiff = Point( -aDiff.Y(), aDiff.X() );

                // OD 2004-04-06 #i26791# - distinction between 'master' drawing
                // object and 'virtual' drawing object no longer needed.
                GetObject().Move( Size( aDiff.X(), aDiff.Y() ) );
            }
        }

        // switch horizontal, LTR anchor position to absolute values.
        if ( rAnchorFrm.IsRightToLeft() )
        {
            rAnchorFrm.SwitchLTRtoRTL( aAnchorPos );
            aAnchorPos.X() -= nObjWidth;
        }
        if ( rAnchorFrm.IsVertical() )
            rAnchorFrm.SwitchHorizontalToVertical( aAnchorPos );

        // #i44347# - keep last object rectangle at anchored object
       OSL_ENSURE( GetAnchoredObj().ISA(SwAnchoredDrawObject),
                 "<SwAsCharAnchoredObjectPosition::CalcPosition()> - wrong type of anchored object." );
        SwAnchoredDrawObject& rAnchoredDrawObj =
                        static_cast<SwAnchoredDrawObject&>( GetAnchoredObj() );
        rAnchoredDrawObj.SetLastObjRect( rAnchoredDrawObj.GetObjRect().SVRect() );
    }
    else
    {
        // determine absolute anchor position and calculate corresponding
        // relative position and its relative position attribute.
        // Note: The relative position contains the spacing values.
        Point aRelAttr;
        if ( rAnchorFrm.IsRightToLeft() )
        {
            rAnchorFrm.SwitchLTRtoRTL( aAnchorPos );
            aAnchorPos.X() -= nObjWidth;
        }
        if ( rAnchorFrm.IsVertical() )
        {
            rAnchorFrm.SwitchHorizontalToVertical( aAnchorPos );
            aRelAttr = Point( -nRelPos, 0 );
            aRelPos = Point( -aRelPos.Y(), aRelPos.X() );
        }
        else
            aRelAttr = Point( 0, nRelPos );

        // OD 2004-03-23 #i26791#
        OSL_ENSURE( GetAnchoredObj().ISA(SwFlyInCntFrm),
                "<SwAsCharAnchoredObjectPosition::CalcPosition()> - wrong anchored object." );
        const SwFlyInCntFrm& rFlyInCntFrm =
                static_cast<const SwFlyInCntFrm&>(GetAnchoredObj());
        if ( !(mnFlags & AS_CHAR_QUICK) &&
             ( aAnchorPos != rFlyInCntFrm.GetRefPoint() ||
               aRelAttr != rFlyInCntFrm.GetCurrRelPos() ) )
        {
            // set new anchor position and relative position
            SwFlyInCntFrm* pFlyInCntFrm = &(const_cast<SwFlyInCntFrm&>(rFlyInCntFrm));
            pFlyInCntFrm->SetRefPoint( aAnchorPos, aRelAttr, aRelPos );
            if( nObjWidth != (pFlyInCntFrm->Frm().*fnRect->fnGetWidth)() )
            {
                // recalculate object bound rectangle, if object width has changed.
                aObjBoundRect = GetAnchoredObj().GetObjRect();
                aObjBoundRect.Left( aObjBoundRect.Left() - rLRSpace.GetLeft() );
                aObjBoundRect.Width( aObjBoundRect.Width() + rLRSpace.GetRight() );
                aObjBoundRect.Top( aObjBoundRect.Top() - rULSpace.GetUpper() );
                aObjBoundRect.Height( aObjBoundRect.Height() + rULSpace.GetLower() );
            }
        }
        OSL_ENSURE( (rFlyInCntFrm.Frm().*fnRect->fnGetHeight)(),
            "SwAnchoredObjectPosition::CalcPosition(..) - fly frame has an invalid height" );
    }

    // keep calculated values
    maAnchorPos = aAnchorPos;
    mnRelPos = nRelPos;
    maObjBoundRect = aObjBoundRect;
}

/** determine the relative position to base line for object position type AS_CHAR

    OD 29.07.2003 #110978#
    Note about values set at member <mnLineAlignment> -
    value gives feedback for the line formatting.
    0 - no feedback; 1|2|3 - proposed formatting of characters
    at top|at center|at bottom of line.

    @author OD
*/
SwTwips SwAsCharAnchoredObjectPosition::_GetRelPosToBase(
                                            const SwTwips _nObjBoundHeight,
                                            const SwFmtVertOrient& _rVert )
{
    SwTwips nRelPosToBase = 0;

    mnLineAlignment = 0;

    const sal_Int16 eVertOrient = _rVert.GetVertOrient();

    if ( eVertOrient == text::VertOrientation::NONE )
        nRelPosToBase = _rVert.GetPos();
    else
    {
        if ( eVertOrient == text::VertOrientation::CENTER )
            nRelPosToBase -= _nObjBoundHeight /  2;
        else if ( eVertOrient == text::VertOrientation::TOP )
            nRelPosToBase -= _nObjBoundHeight;
        else if ( eVertOrient == text::VertOrientation::BOTTOM )
            nRelPosToBase = 0;
        else if ( eVertOrient == text::VertOrientation::CHAR_CENTER )
            nRelPosToBase -= ( _nObjBoundHeight + mnLineAscent - mnLineDescent ) / 2;
        else if ( eVertOrient == text::VertOrientation::CHAR_TOP )
            nRelPosToBase -= mnLineAscent;
        else if ( eVertOrient == text::VertOrientation::CHAR_BOTTOM )
            nRelPosToBase += mnLineDescent - _nObjBoundHeight;
        else
        {
            if( _nObjBoundHeight >= mnLineAscentInclObjs + mnLineDescentInclObjs )
            {
                // object is at least as high as the line. Thus, no more is
                // positioning necessary. Also, the max. ascent isn't changed.
                nRelPosToBase -= mnLineAscentInclObjs;
                if ( eVertOrient == text::VertOrientation::LINE_CENTER )
                    mnLineAlignment = 2;
                else if ( eVertOrient == text::VertOrientation::LINE_TOP )
                    mnLineAlignment = 1;
                else if ( eVertOrient == text::VertOrientation::LINE_BOTTOM )
                    mnLineAlignment = 3;
            }
            else if ( eVertOrient == text::VertOrientation::LINE_CENTER )
            {
                nRelPosToBase -= ( _nObjBoundHeight + mnLineAscentInclObjs - mnLineDescentInclObjs ) / 2;
                mnLineAlignment = 2;
            }
            else if ( eVertOrient == text::VertOrientation::LINE_TOP )
            {
                nRelPosToBase -= mnLineAscentInclObjs;
                mnLineAlignment = 1;
            }
            else if ( eVertOrient == text::VertOrientation::LINE_BOTTOM )
            {
                nRelPosToBase += mnLineDescentInclObjs - _nObjBoundHeight;
                mnLineAlignment = 3;
            }
        }
    }

    return nRelPosToBase;
}

/** calculated anchored position for object position

    @author OD
*/
Point SwAsCharAnchoredObjectPosition::GetAnchorPos() const
{
    return maAnchorPos;
}

/** calculated relative position to base line for object position

    @author OD
*/
SwTwips SwAsCharAnchoredObjectPosition::GetRelPosY() const
{
    return mnRelPos;
}

/** determined object rectangle including spacing for object

    @author OD
*/
SwRect SwAsCharAnchoredObjectPosition::GetObjBoundRectInclSpacing() const
{
    return maObjBoundRect;
}

/** determined line alignment

    @author OD
*/
sal_uInt8 SwAsCharAnchoredObjectPosition::GetLineAlignment() const
{
    return mnLineAlignment;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

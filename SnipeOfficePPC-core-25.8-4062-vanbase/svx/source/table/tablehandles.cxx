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


#include "tablehandles.hxx"

#include <vcl/svapp.hxx>
#include <vcl/outdev.hxx>
#include <vcl/canvastools.hxx>
#include <vcl/hatch.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/polygon/b2dpolypolygontools.hxx>
#include <basegfx/range/b2drectangle.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <svx/sdr/overlay/overlayobject.hxx>
#include <svx/sdr/overlay/overlaymanager.hxx>
#include <svx/sdrpagewindow.hxx>
#include <svx/sdrpaintwindow.hxx>
#include <svx/svdmrkv.hxx>
#include <svx/svdpagv.hxx>
#include <drawinglayer/primitive2d/polypolygonprimitive2d.hxx>
#include <svx/sdr/overlay/overlayhatchrect.hxx>
#include <drawinglayer/primitive2d/hiddengeometryprimitive2d.hxx>

namespace sdr { namespace table {

// --------------------------------------------------------------------

class OverlayTableEdge : public sdr::overlay::OverlayObject
{
protected:
    basegfx::B2DPolyPolygon maPolyPolygon;
    bool                    mbVisible;

    // geometry creation for OverlayObject
    virtual drawinglayer::primitive2d::Primitive2DSequence createOverlayObjectPrimitive2DSequence();

public:
    OverlayTableEdge( const basegfx::B2DPolyPolygon& rPolyPolygon, bool bVisible );
    virtual ~OverlayTableEdge();
};

// --------------------------------------------------------------------

TableEdgeHdl::TableEdgeHdl( const Point& rPnt, bool bHorizontal, sal_Int32 nMin, sal_Int32 nMax, sal_Int32 nEdges )
: SdrHdl( rPnt, HDL_USER )
, mbHorizontal( bHorizontal )
, mnMin( nMin )
, mnMax( nMax )
, maEdges(nEdges)
{
}

void TableEdgeHdl::SetEdge( sal_Int32 nEdge, sal_Int32 nStart, sal_Int32 nEnd, TableEdgeState eState )
{
    if( (nEdge >= 0) && (nEdge <= sal::static_int_cast<sal_Int32>(maEdges.size())) )
    {
        maEdges[nEdge].mnStart = nStart;
        maEdges[nEdge].mnEnd = nEnd;
        maEdges[nEdge].meState = eState;
    }
    else
    {
        OSL_FAIL( "sdr::table::TableEdgeHdl::SetEdge(), invalid edge!" );
    }
}

Pointer TableEdgeHdl::GetPointer() const
{
    if( mbHorizontal )
        return POINTER_VSPLIT;
    else
        return POINTER_HSPLIT;
}

sal_Int32 TableEdgeHdl::GetValidDragOffset( const SdrDragStat& rDrag ) const
{
    return std::min( std::max( static_cast<sal_Int32>(mbHorizontal ? rDrag.GetDY() : rDrag.GetDX()), mnMin ), mnMax );
}

basegfx::B2DPolyPolygon TableEdgeHdl::getSpecialDragPoly(const SdrDragStat& rDrag) const
{
    basegfx::B2DPolyPolygon aVisible;
    basegfx::B2DPolyPolygon aInvisible;

    // create and return visible and non-visible parts for drag
    getPolyPolygon(aVisible, aInvisible, &rDrag);
    aVisible.append(aInvisible);

    return aVisible;
}

void TableEdgeHdl::getPolyPolygon(basegfx::B2DPolyPolygon& rVisible, basegfx::B2DPolyPolygon& rInvisible, const SdrDragStat* pDrag) const
{
    // changed method to create visible and invisible partial polygons in one run in
    // separate PolyPolygons; both kinds are used
    basegfx::B2DPoint aOffset(aPos.X(), aPos.Y());
    rVisible.clear();
    rInvisible.clear();

    if( pDrag )
    {
        int n = mbHorizontal ? 1 : 0;
        aOffset[n] = aOffset[n] + GetValidDragOffset( *pDrag );
    }

    basegfx::B2DPoint aStart(aOffset), aEnd(aOffset);
    int nPos = mbHorizontal ? 0 : 1;
    TableEdgeVector::const_iterator aIter( maEdges.begin() );

    while( aIter != maEdges.end() )
    {
        TableEdge aEdge(*aIter++);

        aStart[nPos] = aOffset[nPos] + aEdge.mnStart;
        aEnd[nPos] = aOffset[nPos] + aEdge.mnEnd;

        basegfx::B2DPolygon aPolygon;
        aPolygon.append( aStart );
        aPolygon.append( aEnd );

        if(aEdge.meState == Visible)
        {
            rVisible.append(aPolygon);
        }
        else
        {
            rInvisible.append(aPolygon);
        }
    }
}

void TableEdgeHdl::CreateB2dIAObject()
{
    GetRidOfIAObject();

    if(pHdlList && pHdlList->GetView() && !pHdlList->GetView()->areMarkHandlesHidden())
    {
        SdrMarkView* pView = pHdlList->GetView();
        SdrPageView* pPageView = pView->GetSdrPageView();

        if(pPageView)
        {
            basegfx::B2DPolyPolygon aVisible;
            basegfx::B2DPolyPolygon aInvisible;

            // get visible and invisible parts
            getPolyPolygon(aVisible, aInvisible, 0);

            if(aVisible.count() || aInvisible.count())
            {
                for(sal_uInt32 nWindow = 0; nWindow < pPageView->PageWindowCount(); nWindow++)
                {
                    const SdrPageWindow& rPageWindow = *pPageView->GetPageWindow(nWindow);

                    if(rPageWindow.GetPaintWindow().OutputToWindow())
                    {
                        rtl::Reference< ::sdr::overlay::OverlayManager > xManager = rPageWindow.GetOverlayManager();
                        if (xManager.is())
                        {
                            if(aVisible.count())
                            {
                                // create overlay object for visible parts
                                sdr::overlay::OverlayObject* pOverlayObject = new OverlayTableEdge(aVisible, true);
                                xManager->add(*pOverlayObject);
                                maOverlayGroup.append(*pOverlayObject);
                            }

                            if(aInvisible.count())
                            {
                                // also create overlay object vor invisible parts to allow
                                // a standard HitTest using the primitives from that overlay object
                                // (see OverlayTableEdge implementation)
                                sdr::overlay::OverlayObject* pOverlayObject = new OverlayTableEdge(aInvisible, false);
                                xManager->add(*pOverlayObject);
                                maOverlayGroup.append(*pOverlayObject);
                            }
                        }
                    }
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

OverlayTableEdge::OverlayTableEdge( const basegfx::B2DPolyPolygon& rPolyPolygon, bool bVisible )
:   OverlayObject(Color(COL_GRAY))
,   maPolyPolygon( rPolyPolygon )
,   mbVisible(bVisible)
{
}

OverlayTableEdge::~OverlayTableEdge()
{
}

drawinglayer::primitive2d::Primitive2DSequence OverlayTableEdge::createOverlayObjectPrimitive2DSequence()
{
    drawinglayer::primitive2d::Primitive2DSequence aRetval;

    if(maPolyPolygon.count())
    {
        // Discussed with CL. Currently i will leave the transparence out since this
        // a little bit expensive. We may check the look with drag polygons later
        const drawinglayer::primitive2d::Primitive2DReference aReference(
            new drawinglayer::primitive2d::PolyPolygonHairlinePrimitive2D(
                maPolyPolygon,
                getBaseColor().getBColor()));

        if(mbVisible)
        {
            // visible, just return as sequence
            aRetval = drawinglayer::primitive2d::Primitive2DSequence(&aReference, 1);
        }
        else
        {
            // embed in HiddenGeometryPrimitive2D to support HitTest of this invisible
            // overlay object
            const drawinglayer::primitive2d::Primitive2DSequence aSequence(&aReference, 1);
            const drawinglayer::primitive2d::Primitive2DReference aNewReference(
                new drawinglayer::primitive2d::HiddenGeometryPrimitive2D(aSequence));
            aRetval = drawinglayer::primitive2d::Primitive2DSequence(&aNewReference, 1);
        }
    }

    return aRetval;
}

// ====================================================================

TableBorderHdl::TableBorderHdl( const Rectangle& rRect )
: SdrHdl( rRect.TopLeft(), HDL_MOVE )
, maRectangle( rRect )
{

}

Pointer TableBorderHdl::GetPointer() const
{
    return POINTER_MOVE;
}

// create marker for this kind
void TableBorderHdl::CreateB2dIAObject()
{
    GetRidOfIAObject();

    if(pHdlList && pHdlList->GetView() && !pHdlList->GetView()->areMarkHandlesHidden())
    {
        SdrMarkView* pView = pHdlList->GetView();
        SdrPageView* pPageView = pView->GetSdrPageView();

        if(pPageView)
        {
            for(sal_uInt32 nWindow = 0; nWindow < pPageView->PageWindowCount(); nWindow++)
            {
                // const SdrPageViewWinRec& rPageViewWinRec = rPageViewWinList[b];
                const SdrPageWindow& rPageWindow = *pPageView->GetPageWindow(nWindow);

                if(rPageWindow.GetPaintWindow().OutputToWindow())
                {
                    rtl::Reference< ::sdr::overlay::OverlayManager > xManager = rPageWindow.GetOverlayManager();
                    if (xManager.is())
                    {
                        const basegfx::B2DRange aRange(vcl::unotools::b2DRectangleFromRectangle(maRectangle));
                        sdr::overlay::OverlayObject* pOverlayObject = new sdr::overlay::OverlayHatchRect(
                            aRange.getMinimum(),
                            aRange.getMaximum(),
                            Color(0x80, 0x80, 0x80),
                            6.0,
                            0.0,
                            45 * F_PI180,
                            0.0);

                        xManager->add(*pOverlayObject);
                        maOverlayGroup.append(*pOverlayObject);
                    }
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

} // end of namespace table
} // end of namespace sdr

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

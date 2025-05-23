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

#include <tools/debug.hxx>
#include <tools/poly.hxx>

#include <vcl/svapp.hxx>
#include <vcl/region.hxx>
#include <vcl/virdev.hxx>
#include <vcl/window.hxx>
#include <vcl/metaact.hxx>
#include <vcl/gdimtf.hxx>
#include <vcl/print.hxx>
#include <vcl/outdev.hxx>
#include <vcl/unowrap.hxx>

#include <window.h>
#include <region.h>
#include <outdev.h>
#include <sallayout.hxx>
#include <salgdi.hxx>
#include <salframe.hxx>
#include <salvd.hxx>
#include <salprn.hxx>
#include <svdata.hxx>
#include <outdata.hxx>

#include <boost/scoped_ptr.hpp>

#include "basegfx/polygon/b2dpolygon.hxx"

// ----------------------------------------------------------------------------
// The only common SalFrame method
// ----------------------------------------------------------------------------

SalFrameGeometry SalFrame::GetGeometry()
{
    // mirror frame coordinates at parent
    SalFrame *pParent = GetParent();
    if( pParent && Application::GetSettings().GetLayoutRTL() )
    {
        SalFrameGeometry aGeom = maGeometry;
        int parent_x = aGeom.nX - pParent->maGeometry.nX;
        aGeom.nX = pParent->maGeometry.nX + pParent->maGeometry.nWidth - maGeometry.nWidth - parent_x;
        return aGeom;
    }
    else
        return maGeometry;
}

// ----------------------------------------------------------------------------

SalGraphics::SalGraphics()
:   m_nLayout( 0 ),
    m_bAntiAliasB2DDraw(false)
{
    // read global RTL settings
    if( Application::GetSettings().GetLayoutRTL() )
        m_nLayout = SAL_LAYOUT_BIDI_RTL;
}

SalGraphics::~SalGraphics()
{
}

// ----------------------------------------------------------------------------

void SalGraphics::mirror( long& x, const OutputDevice *pOutDev, bool bBack ) const
{
    long w;
    if( pOutDev && pOutDev->GetOutDevType() == OUTDEV_VIRDEV )
        w = pOutDev->GetOutputWidthPixel();
    else
        w = GetGraphicsWidth();

    if( w )
    {
        if( pOutDev && pOutDev->ImplIsAntiparallel() )
        {
            OutputDevice *pOutDevRef = (OutputDevice*) pOutDev;
            // mirror this window back
            if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) )
            {
                long devX = w-pOutDevRef->GetOutputWidthPixel()-pOutDevRef->GetOutOffXPixel();   // re-mirrored mnOutOffX
                if( bBack )
                    x = x - devX + pOutDevRef->GetOutOffXPixel();
                else
                    x = devX + (x - pOutDevRef->GetOutOffXPixel());
            }
            else
            {
                long devX = pOutDevRef->GetOutOffXPixel();   // re-mirrored mnOutOffX
                if( bBack )
                    x = devX + (pOutDevRef->GetOutputWidthPixel() + devX) - (x + 1);
                else
                    x = pOutDevRef->GetOutputWidthPixel() - (x - devX) + pOutDevRef->GetOutOffXPixel() - 1;
            }
        }
        else if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) )
            x = w-1-x;
    }
}

void SalGraphics::mirror( long& x, long& nWidth, const OutputDevice *pOutDev, bool bBack ) const
{
    long w;
    if( pOutDev && pOutDev->GetOutDevType() == OUTDEV_VIRDEV )
        w = pOutDev->GetOutputWidthPixel();
    else
        w = GetGraphicsWidth();

    if( w )
    {
        if( pOutDev && pOutDev->ImplIsAntiparallel() )
        {
            OutputDevice *pOutDevRef = (OutputDevice*) pOutDev;
            // mirror this window back
            if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) )
            {
                long devX = w-pOutDevRef->GetOutputWidthPixel()-pOutDevRef->GetOutOffXPixel();   // re-mirrored mnOutOffX
                if( bBack )
                    x = x - devX + pOutDevRef->GetOutOffXPixel();
                else
                    x = devX + (x - pOutDevRef->GetOutOffXPixel());
            }
            else
            {
                long devX = pOutDevRef->GetOutOffXPixel();   // re-mirrored mnOutOffX
                if( bBack )
                    x = devX + (pOutDevRef->GetOutputWidthPixel() + devX) - (x + nWidth);
                else
                    x = pOutDevRef->GetOutputWidthPixel() - (x - devX) + pOutDevRef->GetOutOffXPixel() - nWidth;
            }
        }
        else if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) )
            x = w-nWidth-x;

    }
}

sal_Bool SalGraphics::mirror( sal_uInt32 nPoints, const SalPoint *pPtAry, SalPoint *pPtAry2, const OutputDevice *pOutDev, bool bBack ) const
{
    long w;
    if( pOutDev && pOutDev->GetOutDevType() == OUTDEV_VIRDEV )
        w = pOutDev->GetOutputWidthPixel();
    else
        w = GetGraphicsWidth();

    if( w )
    {
        sal_uInt32 i, j;

        if( pOutDev && pOutDev->ImplIsAntiparallel() )
        {
            OutputDevice *pOutDevRef = (OutputDevice*) pOutDev;
            // mirror this window back
            if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) )
            {
                long devX = w-pOutDevRef->GetOutputWidthPixel()-pOutDevRef->GetOutOffXPixel();   // re-mirrored mnOutOffX
                if( bBack )
                {
                    for( i=0, j=nPoints-1; i<nPoints; i++,j-- )
                    {
                        //long x = w-1-pPtAry[i].mnX;
                        //pPtAry2[j].mnX = devX + ( pOutDevRef->mnOutWidth - 1 - (x - devX) );
                        pPtAry2[j].mnX = pOutDevRef->GetOutOffXPixel() + (pPtAry[i].mnX - devX);
                        pPtAry2[j].mnY = pPtAry[i].mnY;
                    }
                }
                else
                {
                    for( i=0, j=nPoints-1; i<nPoints; i++,j-- )
                    {
                        //long x = w-1-pPtAry[i].mnX;
                        //pPtAry2[j].mnX = devX + ( pOutDevRef->mnOutWidth - 1 - (x - devX) );
                        pPtAry2[j].mnX = devX + (pPtAry[i].mnX - pOutDevRef->GetOutOffXPixel());
                        pPtAry2[j].mnY = pPtAry[i].mnY;
                    }
                }
            }
            else
            {
                long devX = pOutDevRef->GetOutOffXPixel();   // re-mirrored mnOutOffX
                if( bBack )
                {
                    for( i=0, j=nPoints-1; i<nPoints; i++,j-- )
                    {
                        //long x = w-1-pPtAry[i].mnX;
                        //pPtAry2[j].mnX = devX + ( pOutDevRef->mnOutWidth - 1 - (x - devX) );
                        pPtAry2[j].mnX = pPtAry[i].mnX - pOutDevRef->GetOutputWidthPixel() + devX - pOutDevRef->GetOutOffXPixel() + 1;
                        pPtAry2[j].mnY = pPtAry[i].mnY;
                    }
                }
                else
                {
                    for( i=0, j=nPoints-1; i<nPoints; i++,j-- )
                    {
                        //long x = w-1-pPtAry[i].mnX;
                        //pPtAry2[j].mnX = devX + ( pOutDevRef->mnOutWidth - 1 - (x - devX) );
                        pPtAry2[j].mnX = pOutDevRef->GetOutputWidthPixel() - (pPtAry[i].mnX - devX) + pOutDevRef->GetOutOffXPixel() - 1;
                        pPtAry2[j].mnY = pPtAry[i].mnY;
                    }
                }
            }
        }
        else if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) )
        {
            for( i=0, j=nPoints-1; i<nPoints; i++,j-- )
            {
                pPtAry2[j].mnX = w-1-pPtAry[i].mnX;
                pPtAry2[j].mnY = pPtAry[i].mnY;
            }
        }
        return sal_True;
    }
    else
        return sal_False;
}

void SalGraphics::mirror( Region& rRgn, const OutputDevice *pOutDev, bool bBack ) const
{
    if( rRgn.HasPolyPolygon() )
    {
        basegfx::B2DPolyPolygon aPolyPoly( rRgn.ConvertToB2DPolyPolygon() );
        aPolyPoly = mirror( aPolyPoly, pOutDev, bBack );
        rRgn = Region( aPolyPoly );
    }
    else
    {
        ImplRegionInfo      aInfo;
        bool                bRegionRect;
        Region              aMirroredRegion;
        long nX, nY, nWidth, nHeight;

        bRegionRect = rRgn.ImplGetFirstRect( aInfo, nX, nY, nWidth, nHeight );
        while ( bRegionRect )
        {
            Rectangle aRect( Point(nX, nY), Size(nWidth, nHeight) );
            mirror( aRect, pOutDev, bBack );
            aMirroredRegion.Union( aRect );
            bRegionRect = rRgn.ImplGetNextRect( aInfo, nX, nY, nWidth, nHeight );
        }
        rRgn = aMirroredRegion;
    }
}

void SalGraphics::mirror( Rectangle& rRect, const OutputDevice *pOutDev, bool bBack ) const
{
    long nWidth = rRect.GetWidth();
    long x      = rRect.Left();
    long x_org = x;

    mirror( x, nWidth, pOutDev, bBack );
    rRect.Move( x - x_org, 0 );
}

basegfx::B2DPoint SalGraphics::mirror( const basegfx::B2DPoint& i_rPoint, const OutputDevice *i_pOutDev, bool i_bBack ) const
{
    long w;
    if( i_pOutDev && i_pOutDev->GetOutDevType() == OUTDEV_VIRDEV )
        w = i_pOutDev->GetOutputWidthPixel();
    else
        w = GetGraphicsWidth();

    DBG_ASSERT( w, "missing graphics width" );

    basegfx::B2DPoint aRet( i_rPoint );
    if( w )
    {
        if( i_pOutDev && !i_pOutDev->IsRTLEnabled() )
        {
            OutputDevice *pOutDevRef = (OutputDevice*)i_pOutDev;
            // mirror this window back
            double devX = w-pOutDevRef->GetOutputWidthPixel()-pOutDevRef->GetOutOffXPixel();   // re-mirrored mnOutOffX
            if( i_bBack )
                aRet.setX( i_rPoint.getX() - devX + pOutDevRef->GetOutOffXPixel() );
            else
                aRet.setX( devX + (i_rPoint.getX() - pOutDevRef->GetOutOffXPixel()) );
        }
        else
            aRet.setX( w-1-i_rPoint.getX() );
    }
    return aRet;
}

basegfx::B2DPolygon SalGraphics::mirror( const basegfx::B2DPolygon& i_rPoly, const OutputDevice *i_pOutDev, bool i_bBack ) const
{
    long w;
    if( i_pOutDev && i_pOutDev->GetOutDevType() == OUTDEV_VIRDEV )
        w = i_pOutDev->GetOutputWidthPixel();
    else
        w = GetGraphicsWidth();

    DBG_ASSERT( w, "missing graphics width" );

    basegfx::B2DPolygon aRet;
    if( w )
    {
        sal_Int32 nPoints = i_rPoly.count();
        for( sal_Int32 i = 0; i < nPoints; i++ )
        {
            aRet.append( mirror( i_rPoly.getB2DPoint( i ), i_pOutDev, i_bBack ) );
            if( i_rPoly.isPrevControlPointUsed( i ) )
                aRet.setPrevControlPoint( i, mirror( i_rPoly.getPrevControlPoint( i ), i_pOutDev, i_bBack ) );
            if( i_rPoly.isNextControlPointUsed( i ) )
                aRet.setNextControlPoint( i, mirror( i_rPoly.getNextControlPoint( i ), i_pOutDev, i_bBack ) );
        }
        aRet.setClosed( i_rPoly.isClosed() );
        aRet.flip();
    }
    else
        aRet = i_rPoly;
    return aRet;
}

basegfx::B2DPolyPolygon SalGraphics::mirror( const basegfx::B2DPolyPolygon& i_rPoly, const OutputDevice *i_pOutDev, bool i_bBack ) const
{
    long w;
    if( i_pOutDev && i_pOutDev->GetOutDevType() == OUTDEV_VIRDEV )
        w = i_pOutDev->GetOutputWidthPixel();
    else
        w = GetGraphicsWidth();

    DBG_ASSERT( w, "missing graphics width" );

    basegfx::B2DPolyPolygon aRet;
    if( w )
    {
        sal_Int32 nPoly = i_rPoly.count();
        for( sal_Int32 i = 0; i < nPoly; i++ )
            aRet.append( mirror( i_rPoly.getB2DPolygon( i ), i_pOutDev, i_bBack ) );
        aRet.setClosed( i_rPoly.isClosed() );
        aRet.flip();
    }
    else
        aRet = i_rPoly;
    return aRet;
}

// ----------------------------------------------------------------------------

bool SalGraphics::SetClipRegion( const Region& i_rClip, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        Region aMirror( i_rClip );
        mirror( aMirror, pOutDev );
        return setClipRegion( aMirror );
    }
    return setClipRegion( i_rClip );
}

void    SalGraphics::DrawPixel( long nX, long nY, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
        mirror( nX, pOutDev );
    drawPixel( nX, nY );
}
void    SalGraphics::DrawPixel( long nX, long nY, SalColor nSalColor, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
        mirror( nX, pOutDev );
    drawPixel( nX, nY, nSalColor );
}
void    SalGraphics::DrawLine( long nX1, long nY1, long nX2, long nY2, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        mirror( nX1, pOutDev );
        mirror( nX2, pOutDev );
    }
    drawLine( nX1, nY1, nX2, nY2 );
}
void    SalGraphics::DrawRect( long nX, long nY, long nWidth, long nHeight, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
        mirror( nX, nWidth, pOutDev );
    drawRect( nX, nY, nWidth, nHeight );
}

void SalGraphics::DrawPolyLine( sal_uLong nPoints, const SalPoint* pPtAry, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        SalPoint* pPtAry2 = new SalPoint[nPoints];
        sal_Bool bCopied = mirror( nPoints, pPtAry, pPtAry2, pOutDev );
        drawPolyLine( nPoints, bCopied ? pPtAry2 : pPtAry );
        delete [] pPtAry2;
    }
    else
        drawPolyLine( nPoints, pPtAry );
}

void SalGraphics::DrawPolygon( sal_uLong nPoints, const SalPoint* pPtAry, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        SalPoint* pPtAry2 = new SalPoint[nPoints];
        sal_Bool bCopied = mirror( nPoints, pPtAry, pPtAry2, pOutDev );
        drawPolygon( nPoints, bCopied ? pPtAry2 : pPtAry );
        delete [] pPtAry2;
    }
    else
        drawPolygon( nPoints, pPtAry );
}

void SalGraphics::DrawPolyPolygon( sal_uInt32 nPoly, const sal_uInt32* pPoints, PCONSTSALPOINT* pPtAry, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        // TODO: optimize, reduce new/delete calls
        SalPoint **pPtAry2 = new SalPoint*[nPoly];
        sal_uLong i;
        for(i=0; i<nPoly; i++)
        {
            sal_uLong nPoints = pPoints[i];
            pPtAry2[i] = new SalPoint[ nPoints ];
            mirror( nPoints, pPtAry[i], pPtAry2[i], pOutDev );
        }

        drawPolyPolygon( nPoly, pPoints, (PCONSTSALPOINT*)pPtAry2 );

        for(i=0; i<nPoly; i++)
            delete [] pPtAry2[i];
        delete [] pPtAry2;
    }
    else
        drawPolyPolygon( nPoly, pPoints, pPtAry );
}

bool SalGraphics::DrawPolyPolygon( const ::basegfx::B2DPolyPolygon& i_rPolyPolygon, double i_fTransparency, const OutputDevice* i_pOutDev )
{
    bool bRet = false;
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (i_pOutDev && i_pOutDev->IsRTLEnabled()) )
    {
        basegfx::B2DPolyPolygon aMirror( mirror( i_rPolyPolygon, i_pOutDev ) );
        bRet = drawPolyPolygon( aMirror, i_fTransparency );
    }
    else
        bRet = drawPolyPolygon( i_rPolyPolygon, i_fTransparency );
    return bRet;
}

sal_Bool SalGraphics::DrawPolyLineBezier( sal_uLong nPoints, const SalPoint* pPtAry, const sal_uInt8* pFlgAry, const OutputDevice* pOutDev )
{
    sal_Bool bResult = sal_False;
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        SalPoint* pPtAry2 = new SalPoint[nPoints];
        sal_Bool bCopied = mirror( nPoints, pPtAry, pPtAry2, pOutDev );
        bResult = drawPolyLineBezier( nPoints, bCopied ? pPtAry2 : pPtAry, pFlgAry );
        delete [] pPtAry2;
    }
    else
        bResult = drawPolyLineBezier( nPoints, pPtAry, pFlgAry );
    return bResult;
}

sal_Bool SalGraphics::DrawPolygonBezier( sal_uLong nPoints, const SalPoint* pPtAry, const sal_uInt8* pFlgAry, const OutputDevice* pOutDev )
{
    sal_Bool bResult = sal_False;
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        SalPoint* pPtAry2 = new SalPoint[nPoints];
        sal_Bool bCopied = mirror( nPoints, pPtAry, pPtAry2, pOutDev );
        bResult = drawPolygonBezier( nPoints, bCopied ? pPtAry2 : pPtAry, pFlgAry );
        delete [] pPtAry2;
    }
    else
        bResult = drawPolygonBezier( nPoints, pPtAry, pFlgAry );
    return bResult;
}

sal_Bool SalGraphics::DrawPolyPolygonBezier( sal_uInt32 i_nPoly, const sal_uInt32* i_pPoints,
                                                   const SalPoint* const* i_pPtAry, const sal_uInt8* const* i_pFlgAry, const OutputDevice* i_pOutDev )
{
    sal_Bool bRet = sal_False;
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (i_pOutDev && i_pOutDev->IsRTLEnabled()) )
    {
        // TODO: optimize, reduce new/delete calls
        SalPoint **pPtAry2 = new SalPoint*[i_nPoly];
        sal_uLong i;
        for(i=0; i<i_nPoly; i++)
        {
            sal_uLong nPoints = i_pPoints[i];
            pPtAry2[i] = new SalPoint[ nPoints ];
            mirror( nPoints, i_pPtAry[i], pPtAry2[i], i_pOutDev );
        }

        bRet = drawPolyPolygonBezier( i_nPoly, i_pPoints, (PCONSTSALPOINT*)pPtAry2, i_pFlgAry );

        for(i=0; i<i_nPoly; i++)
            delete [] pPtAry2[i];
        delete [] pPtAry2;
    }
    else
        bRet = drawPolyPolygonBezier( i_nPoly, i_pPoints, i_pPtAry, i_pFlgAry );
    return bRet;
}

bool SalGraphics::DrawPolyLine(
    const ::basegfx::B2DPolygon& i_rPolygon,
    double i_fTransparency,
    const ::basegfx::B2DVector& i_rLineWidth,
    basegfx::B2DLineJoin i_eLineJoin,
    com::sun::star::drawing::LineCap i_eLineCap,
    const OutputDevice* i_pOutDev )
{
    bool bRet = false;
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (i_pOutDev && i_pOutDev->IsRTLEnabled()) )
    {
        basegfx::B2DPolygon aMirror( mirror( i_rPolygon, i_pOutDev ) );
        bRet = drawPolyLine( aMirror, i_fTransparency, i_rLineWidth, i_eLineJoin, i_eLineCap );
    }
    else
        bRet = drawPolyLine( i_rPolygon, i_fTransparency, i_rLineWidth, i_eLineJoin, i_eLineCap );
    return bRet;
}

void    SalGraphics::CopyArea( long nDestX, long nDestY,
                               long nSrcX, long nSrcY,
                               long nSrcWidth, long nSrcHeight,
                               sal_uInt16 nFlags, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        mirror( nDestX, nSrcWidth, pOutDev );
        mirror( nSrcX, nSrcWidth, pOutDev );
    }
    copyArea( nDestX, nDestY, nSrcX, nSrcY, nSrcWidth, nSrcHeight, nFlags );
}
void    SalGraphics::CopyBits( const SalTwoRect* pPosAry,
                               SalGraphics* pSrcGraphics, const OutputDevice *pOutDev, const OutputDevice *pSrcOutDev )
{
    if( ( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) ) ||
        (pSrcGraphics && ( (pSrcGraphics->GetLayout() & SAL_LAYOUT_BIDI_RTL)  || (pSrcOutDev && pSrcOutDev->IsRTLEnabled()) ) ) )
    {
        SalTwoRect pPosAry2 = *pPosAry;
        if( (pSrcGraphics && (pSrcGraphics->GetLayout() & SAL_LAYOUT_BIDI_RTL)) || (pSrcOutDev && pSrcOutDev->IsRTLEnabled()) )
            mirror( pPosAry2.mnSrcX, pPosAry2.mnSrcWidth, pSrcOutDev );
        if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
            mirror( pPosAry2.mnDestX, pPosAry2.mnDestWidth, pOutDev );
        copyBits( &pPosAry2, pSrcGraphics );
    }
    else
        copyBits( pPosAry, pSrcGraphics );
}
void    SalGraphics::DrawBitmap( const SalTwoRect* pPosAry,
                                    const SalBitmap& rSalBitmap, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        SalTwoRect pPosAry2 = *pPosAry;
        mirror( pPosAry2.mnDestX, pPosAry2.mnDestWidth, pOutDev );
        drawBitmap( &pPosAry2, rSalBitmap );
    }
    else
        drawBitmap( pPosAry, rSalBitmap );
}
void SalGraphics::DrawBitmap( const SalTwoRect* pPosAry,
                              const SalBitmap& rSalBitmap,
                              const SalBitmap& rTransparentBitmap, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        SalTwoRect pPosAry2 = *pPosAry;
        mirror( pPosAry2.mnDestX, pPosAry2.mnDestWidth, pOutDev );
        drawBitmap( &pPosAry2, rSalBitmap, rTransparentBitmap );
    }
    else
        drawBitmap( pPosAry, rSalBitmap, rTransparentBitmap );
}
void    SalGraphics::DrawMask( const SalTwoRect* pPosAry,
                                  const SalBitmap& rSalBitmap,
                                  SalColor nMaskColor, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        SalTwoRect pPosAry2 = *pPosAry;
        mirror( pPosAry2.mnDestX, pPosAry2.mnDestWidth, pOutDev );
        drawMask( &pPosAry2, rSalBitmap, nMaskColor );
    }
    else
        drawMask( pPosAry, rSalBitmap, nMaskColor );
}
SalBitmap*  SalGraphics::GetBitmap( long nX, long nY, long nWidth, long nHeight, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
        mirror( nX, nWidth, pOutDev );
    return getBitmap( nX, nY, nWidth, nHeight );
}
SalColor    SalGraphics::GetPixel( long nX, long nY, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
        mirror( nX, pOutDev );
    return getPixel( nX, nY );
}
void    SalGraphics::Invert( long nX, long nY, long nWidth, long nHeight, SalInvert nFlags, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
        mirror( nX, nWidth, pOutDev );
    invert( nX, nY, nWidth, nHeight, nFlags );
}
void    SalGraphics::Invert( sal_uLong nPoints, const SalPoint* pPtAry, SalInvert nFlags, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        SalPoint* pPtAry2 = new SalPoint[nPoints];
        sal_Bool bCopied = mirror( nPoints, pPtAry, pPtAry2, pOutDev );
        invert( nPoints, bCopied ? pPtAry2 : pPtAry, nFlags );
        delete [] pPtAry2;
    }
    else
        invert( nPoints, pPtAry, nFlags );
}

sal_Bool    SalGraphics::DrawEPS( long nX, long nY, long nWidth, long nHeight, void* pPtr, sal_uLong nSize, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
        mirror( nX, nWidth, pOutDev );
    return drawEPS( nX, nY, nWidth, nHeight,  pPtr, nSize );
}

sal_Bool SalGraphics::HitTestNativeControl( ControlType nType, ControlPart nPart, const Rectangle& rControlRegion,
                                                const Point& aPos, sal_Bool& rIsInside, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        Point pt( aPos );
        Rectangle rgn( rControlRegion );
        mirror( pt.X(), pOutDev );
        mirror( rgn, pOutDev );
        return hitTestNativeControl( nType, nPart, rgn, pt, rIsInside );
    }
    else
        return hitTestNativeControl( nType, nPart, rControlRegion, aPos, rIsInside );
}

void SalGraphics::mirror( ImplControlValue& rVal, const OutputDevice* pOutDev, bool bBack ) const
{
    switch( rVal.getType() )
    {
        case CTRL_SLIDER:
        {
            SliderValue* pSlVal = static_cast<SliderValue*>(&rVal);
            mirror(pSlVal->maThumbRect,pOutDev,bBack);
        }
        break;
        case CTRL_SCROLLBAR:
        {
            ScrollbarValue* pScVal = static_cast<ScrollbarValue*>(&rVal);
            mirror(pScVal->maThumbRect,pOutDev,bBack);
            mirror(pScVal->maButton1Rect,pOutDev,bBack);
            mirror(pScVal->maButton2Rect,pOutDev,bBack);
        }
        break;
        case CTRL_SPINBOX:
        case CTRL_SPINBUTTONS:
        {
            SpinbuttonValue* pSpVal = static_cast<SpinbuttonValue*>(&rVal);
            mirror(pSpVal->maUpperRect,pOutDev,bBack);
            mirror(pSpVal->maLowerRect,pOutDev,bBack);
        }
        break;
        case CTRL_TOOLBAR:
        {
            ToolbarValue* pTVal = static_cast<ToolbarValue*>(&rVal);
            mirror(pTVal->maGripRect,pOutDev,bBack);
        }
        break;
    }
}

sal_Bool SalGraphics::DrawNativeControl( ControlType nType, ControlPart nPart, const Rectangle& rControlRegion,
                                                ControlState nState, const ImplControlValue& aValue,
                                                const OUString& aCaption, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        Rectangle rgn( rControlRegion );
        mirror( rgn, pOutDev );
        boost::scoped_ptr< ImplControlValue > mirrorValue( aValue.clone());
        mirror( *mirrorValue, pOutDev );
        sal_Bool bRet = drawNativeControl( nType, nPart, rgn, nState, *mirrorValue, aCaption );
        return bRet;
    }
    else
        return drawNativeControl( nType, nPart, rControlRegion, nState, aValue, aCaption );
}

sal_Bool SalGraphics::GetNativeControlRegion( ControlType nType, ControlPart nPart, const Rectangle& rControlRegion, ControlState nState,
                                                const ImplControlValue& aValue, const OUString& aCaption,
                                                Rectangle &rNativeBoundingRegion, Rectangle &rNativeContentRegion, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        Rectangle rgn( rControlRegion );
        mirror( rgn, pOutDev );
        boost::scoped_ptr< ImplControlValue > mirrorValue( aValue.clone());
        mirror( *mirrorValue, pOutDev );
        if( getNativeControlRegion( nType, nPart, rgn, nState, *mirrorValue, aCaption,
                                                rNativeBoundingRegion, rNativeContentRegion ) )
        {
            mirror( rNativeBoundingRegion, pOutDev, true );
            mirror( rNativeContentRegion, pOutDev, true );
            return sal_True;
        }
        return sal_False;
    }
    else
        return getNativeControlRegion( nType, nPart, rControlRegion, nState, aValue, aCaption,
                                                rNativeBoundingRegion, rNativeContentRegion );
}

bool SalGraphics::DrawAlphaBitmap( const SalTwoRect& rPosAry,
                                   const SalBitmap& rSourceBitmap,
                                   const SalBitmap& rAlphaBitmap,
                                   const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
    {
        SalTwoRect pPosAry2 = rPosAry;
        mirror( pPosAry2.mnDestX, pPosAry2.mnDestWidth, pOutDev );
        return drawAlphaBitmap( pPosAry2, rSourceBitmap, rAlphaBitmap );
    }
    else
        return drawAlphaBitmap( rPosAry, rSourceBitmap, rAlphaBitmap );
}

bool SalGraphics::DrawAlphaRect( long nX, long nY, long nWidth, long nHeight,
                                 sal_uInt8 nTransparency, const OutputDevice *pOutDev )
{
    if( (m_nLayout & SAL_LAYOUT_BIDI_RTL) || (pOutDev && pOutDev->IsRTLEnabled()) )
        mirror( nX, nWidth, pOutDev );

    return drawAlphaRect( nX, nY, nWidth, nHeight, nTransparency );
}

bool SalGraphics::filterText( const rtl::OUString&, rtl::OUString&, xub_StrLen, xub_StrLen&, xub_StrLen&, xub_StrLen& )
{
    return false;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

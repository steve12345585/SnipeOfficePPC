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


#include <limits.h>

#include <tools/bigint.hxx>
#include <tools/debug.hxx>
#include <tools/poly.hxx>

#include <vcl/virdev.hxx>
#include <vcl/region.hxx>
#include <vcl/wrkwin.hxx>
#include <vcl/cursor.hxx>
#include <vcl/metaact.hxx>
#include <vcl/gdimtf.hxx>
#include <vcl/lineinfo.hxx>
#include <vcl/outdev.hxx>

#include <svdata.hxx>
#include <region.h>
#include <window.h>
#include <outdev.h>
#include <salgdi.hxx>

#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/polygon/b2dpolypolygon.hxx>

// =======================================================================

DBG_NAMEEX( OutputDevice )
DBG_NAMEEX( Polygon )
DBG_NAMEEX( PolyPolygon )
DBG_NAMEEX( Region )

// =======================================================================

static int const s_ImplArySize = MAP_PIXEL+1;
static long aImplNumeratorAry[s_ImplArySize] =
    {    1,   1,   5,  50,    1,   1,  1, 1,  1,    1, 1 };
static long aImplDenominatorAry[s_ImplArySize] =
     { 2540, 254, 127, 127, 1000, 100, 10, 1, 72, 1440, 1 };

// -----------------------------------------------------------------------

/*
Reduziert die Genauigkeit bis eine Fraction draus wird (sollte mal
ein Fraction ctor werden) koennte man dann auch mit BigInts machen
*/

static Fraction ImplMakeFraction( long nN1, long nN2, long nD1, long nD2 )
{
    long i = 1;

    if ( nN1 < 0 ) { i = -i; nN1 = -nN1; }
    if ( nN2 < 0 ) { i = -i; nN2 = -nN2; }
    if ( nD1 < 0 ) { i = -i; nD1 = -nD1; }
    if ( nD2 < 0 ) { i = -i; nD2 = -nD2; }
    // alle positiv; i Vorzeichen

    Fraction aF( i*nN1, nD1 );
    aF *= Fraction( nN2, nD2 );

    if( nD1 == 0 || nD2 == 0 ) //under these bad circumstances the following while loop will be endless
    {
        DBG_ASSERT(false,"Invalid parameter for ImplMakeFraction");
        return Fraction( 1, 1 );
    }

    while ( aF.GetDenominator() == -1 )
    {
        if ( nN1 > nN2 )
            nN1 = (nN1 + 1) / 2;
        else
            nN2 = (nN2 + 1) / 2;
        if ( nD1 > nD2 )
            nD1 = (nD1 + 1) / 2;
        else
            nD2 = (nD2 + 1) / 2;

        aF = Fraction( i*nN1, nD1 );
        aF *= Fraction( nN2, nD2 );
    }

    return aF;
}

// -----------------------------------------------------------------------

// Fraction.GetNumerator()
// Fraction.GetDenominator()    > 0
// rOutRes.nPixPerInch?         > 0
// rMapRes.nMapScNum?
// rMapRes.nMapScDenom?         > 0

static void ImplCalcBigIntThreshold( long nDPIX, long nDPIY,
                                     const ImplMapRes& rMapRes,
                                     ImplThresholdRes& rThresRes )
{
    if ( nDPIX && (LONG_MAX / nDPIX < Abs( rMapRes.mnMapScNumX ) ) ) // #111139# avoid div by zero
    {
        rThresRes.mnThresLogToPixX = 0;
        rThresRes.mnThresPixToLogX = 0;
    }
    else
    {
        // Schwellenwerte fuer BigInt Arithmetik berechnen
        long    nDenomHalfX = rMapRes.mnMapScDenomX / 2;
        sal_uLong   nDenomX     = rMapRes.mnMapScDenomX;
        long    nProductX   = nDPIX * rMapRes.mnMapScNumX;

        if ( !nProductX )
            rThresRes.mnThresLogToPixX = LONG_MAX;
        else
            rThresRes.mnThresLogToPixX = Abs( (LONG_MAX - nDenomHalfX) / nProductX );

        if ( !nDenomX )
            rThresRes.mnThresPixToLogX = LONG_MAX;
        else if ( nProductX >= 0 )
            rThresRes.mnThresPixToLogX = (long)(((sal_uLong)LONG_MAX - (sal_uLong)( nProductX/2)) / nDenomX);
        else
            rThresRes.mnThresPixToLogX = (long)(((sal_uLong)LONG_MAX + (sal_uLong)(-nProductX/2)) / nDenomX);
    }

    if ( nDPIY && (LONG_MAX / nDPIY < Abs( rMapRes.mnMapScNumY ) ) ) // #111139# avoid div by zero
    {
        rThresRes.mnThresLogToPixY = 0;
        rThresRes.mnThresPixToLogY = 0;
    }
    else
    {
        // Schwellenwerte fuer BigInt Arithmetik berechnen
        long    nDenomHalfY = rMapRes.mnMapScDenomY / 2;
        sal_uLong   nDenomY     = rMapRes.mnMapScDenomY;
        long    nProductY   = nDPIY * rMapRes.mnMapScNumY;

        if ( !nProductY )
            rThresRes.mnThresLogToPixY = LONG_MAX;
        else
            rThresRes.mnThresLogToPixY = Abs( (LONG_MAX - nDenomHalfY) / nProductY );

        if ( !nDenomY )
            rThresRes.mnThresPixToLogY = LONG_MAX;
        else if ( nProductY >= 0 )
            rThresRes.mnThresPixToLogY = (long)(((sal_uLong)LONG_MAX - (sal_uLong)( nProductY/2)) / nDenomY);
        else
            rThresRes.mnThresPixToLogY = (long)(((sal_uLong)LONG_MAX + (sal_uLong)(-nProductY/2)) / nDenomY);
    }

    rThresRes.mnThresLogToPixX /= 2;
    rThresRes.mnThresLogToPixY /= 2;
    rThresRes.mnThresPixToLogX /= 2;
    rThresRes.mnThresPixToLogY /= 2;
}

// -----------------------------------------------------------------------

static void ImplCalcMapResolution( const MapMode& rMapMode,
                                   long nDPIX, long nDPIY, ImplMapRes& rMapRes )
{
    switch ( rMapMode.GetMapUnit() )
    {
        case MAP_RELATIVE:
            break;
        case MAP_100TH_MM:
            rMapRes.mnMapScNumX   = 1;
            rMapRes.mnMapScDenomX = 2540;
            rMapRes.mnMapScNumY   = 1;
            rMapRes.mnMapScDenomY = 2540;
            break;
        case MAP_10TH_MM:
            rMapRes.mnMapScNumX   = 1;
            rMapRes.mnMapScDenomX = 254;
            rMapRes.mnMapScNumY   = 1;
            rMapRes.mnMapScDenomY = 254;
            break;
        case MAP_MM:
            rMapRes.mnMapScNumX   = 5;      // 10
            rMapRes.mnMapScDenomX = 127;    // 254
            rMapRes.mnMapScNumY   = 5;      // 10
            rMapRes.mnMapScDenomY = 127;    // 254
            break;
        case MAP_CM:
            rMapRes.mnMapScNumX   = 50;     // 100
            rMapRes.mnMapScDenomX = 127;    // 254
            rMapRes.mnMapScNumY   = 50;     // 100
            rMapRes.mnMapScDenomY = 127;    // 254
            break;
        case MAP_1000TH_INCH:
            rMapRes.mnMapScNumX   = 1;
            rMapRes.mnMapScDenomX = 1000;
            rMapRes.mnMapScNumY   = 1;
            rMapRes.mnMapScDenomY = 1000;
            break;
        case MAP_100TH_INCH:
            rMapRes.mnMapScNumX   = 1;
            rMapRes.mnMapScDenomX = 100;
            rMapRes.mnMapScNumY   = 1;
            rMapRes.mnMapScDenomY = 100;
            break;
        case MAP_10TH_INCH:
            rMapRes.mnMapScNumX   = 1;
            rMapRes.mnMapScDenomX = 10;
            rMapRes.mnMapScNumY   = 1;
            rMapRes.mnMapScDenomY = 10;
            break;
        case MAP_INCH:
            rMapRes.mnMapScNumX   = 1;
            rMapRes.mnMapScDenomX = 1;
            rMapRes.mnMapScNumY   = 1;
            rMapRes.mnMapScDenomY = 1;
            break;
        case MAP_POINT:
            rMapRes.mnMapScNumX   = 1;
            rMapRes.mnMapScDenomX = 72;
            rMapRes.mnMapScNumY   = 1;
            rMapRes.mnMapScDenomY = 72;
            break;
        case MAP_TWIP:
            rMapRes.mnMapScNumX   = 1;
            rMapRes.mnMapScDenomX = 1440;
            rMapRes.mnMapScNumY   = 1;
            rMapRes.mnMapScDenomY = 1440;
            break;
        case MAP_PIXEL:
            rMapRes.mnMapScNumX   = 1;
            rMapRes.mnMapScDenomX = nDPIX;
            rMapRes.mnMapScNumY   = 1;
            rMapRes.mnMapScDenomY = nDPIY;
            break;
        case MAP_SYSFONT:
        case MAP_APPFONT:
        case MAP_REALAPPFONT:
            {
            ImplSVData* pSVData = ImplGetSVData();
            if ( !pSVData->maGDIData.mnAppFontX )
            {
                if( pSVData->maWinData.mpFirstFrame )
                    Window::ImplInitAppFontData( pSVData->maWinData.mpFirstFrame );
                else
                {
                    WorkWindow* pWin = new WorkWindow( NULL, 0 );
                    Window::ImplInitAppFontData( pWin );
                    delete pWin;
                }
            }
            if ( rMapMode.GetMapUnit() == MAP_REALAPPFONT )
                rMapRes.mnMapScNumX   = pSVData->maGDIData.mnRealAppFontX;
            else
                rMapRes.mnMapScNumX   = pSVData->maGDIData.mnAppFontX;
            rMapRes.mnMapScDenomX = nDPIX * 40;
            rMapRes.mnMapScNumY   = pSVData->maGDIData.mnAppFontY;
            rMapRes.mnMapScDenomY = nDPIY * 80;
            }
            break;
        default:
            OSL_FAIL( "unhandled MapUnit" );
            break;
    }

    Fraction aScaleX = rMapMode.GetScaleX();
    Fraction aScaleY = rMapMode.GetScaleY();

    // Offset laut MapMode setzen
    Point aOrigin = rMapMode.GetOrigin();
    if ( rMapMode.GetMapUnit() != MAP_RELATIVE )
    {
        rMapRes.mnMapOfsX = aOrigin.X();
        rMapRes.mnMapOfsY = aOrigin.Y();
    }
    else
    {
        BigInt aX( rMapRes.mnMapOfsX );
        aX *= BigInt( aScaleX.GetDenominator() );
        if ( rMapRes.mnMapOfsX >= 0 )
        {
            if ( aScaleX.GetNumerator() >= 0 )
                aX += BigInt( aScaleX.GetNumerator()/2 );
            else
                aX -= BigInt( (aScaleX.GetNumerator()+1)/2 );
        }
        else
        {
            if ( aScaleX.GetNumerator() >= 0 )
                aX -= BigInt( (aScaleX.GetNumerator()-1)/2 );
            else
                aX += BigInt( aScaleX.GetNumerator()/2 );
        }
        aX /= BigInt( aScaleX.GetNumerator() );
        rMapRes.mnMapOfsX = (long)aX + aOrigin.X();
        BigInt aY( rMapRes.mnMapOfsY );
        aY *= BigInt( aScaleY.GetDenominator() );
        if( rMapRes.mnMapOfsY >= 0 )
        {
            if ( aScaleY.GetNumerator() >= 0 )
                aY += BigInt( aScaleY.GetNumerator()/2 );
            else
                aY -= BigInt( (aScaleY.GetNumerator()+1)/2 );
        }
        else
        {
            if ( aScaleY.GetNumerator() >= 0 )
                aY -= BigInt( (aScaleY.GetNumerator()-1)/2 );
            else
                aY += BigInt( aScaleY.GetNumerator()/2 );
        }
        aY /= BigInt( aScaleY.GetNumerator() );
        rMapRes.mnMapOfsY = (long)aY + aOrigin.Y();
    }

    // Scaling Faktor laut MapMode einberechnen
    // aTemp? = rMapRes.mnMapSc? * aScale?
    Fraction aTempX = ImplMakeFraction( rMapRes.mnMapScNumX,
                                        aScaleX.GetNumerator(),
                                        rMapRes.mnMapScDenomX,
                                        aScaleX.GetDenominator() );
    Fraction aTempY = ImplMakeFraction( rMapRes.mnMapScNumY,
                                        aScaleY.GetNumerator(),
                                        rMapRes.mnMapScDenomY,
                                        aScaleY.GetDenominator() );
    rMapRes.mnMapScNumX   = aTempX.GetNumerator();
    rMapRes.mnMapScDenomX = aTempX.GetDenominator();
    rMapRes.mnMapScNumY   = aTempY.GetNumerator();
    rMapRes.mnMapScDenomY = aTempY.GetDenominator();

    // hack: 0/n ungef"ahr 1/max
    if ( !rMapRes.mnMapScNumX )
    {
        rMapRes.mnMapScNumX = 1;
        rMapRes.mnMapScDenomX = LONG_MAX;
    }
    if ( !rMapRes.mnMapScNumY )
    {
        rMapRes.mnMapScNumY = 1;
        rMapRes.mnMapScDenomY = LONG_MAX;
    }
}

// -----------------------------------------------------------------------

inline void ImplCalcMapResolution( const MapMode& rMapMode,
                                   long nDPIX, long nDPIY,
                                   ImplMapRes& rMapRes,
                                   ImplThresholdRes& rThresRes )
{
    ImplCalcMapResolution( rMapMode, nDPIX, nDPIY, rMapRes );
    ImplCalcBigIntThreshold( nDPIX, nDPIY, rMapRes, rThresRes );
}

// -----------------------------------------------------------------------

static long ImplLogicToPixel( long n, long nDPI, long nMapNum, long nMapDenom,
                              long nThres )
{
    // To "use" it...
    (void) nThres;
#if (SAL_TYPES_SIZEOFLONG < 8)
    if( (+n < nThres) && (-n < nThres) )
    {
       n *= nMapNum * nDPI;
       if( nMapDenom != 1 )
       {
          n = (2 * n) / nMapDenom;
          if( n < 0 ) --n; else ++n;
          n /= 2;
       }
    }
    else
#endif
    {
       sal_Int64 n64 = n;
       n64 *= nMapNum;
       n64 *= nDPI;
       if( nMapDenom == 1 )
          n = (long)n64;
       else
       {
          n = (long)(2 * n64 / nMapDenom);
          if( n < 0 ) --n; else ++n;
          n /= 2;
       }
    }
    return n;
}

// -----------------------------------------------------------------------

static long ImplPixelToLogic( long n, long nDPI, long nMapNum, long nMapDenom,
                              long nThres )
{
    // To "use" it...
   (void) nThres;
#if (SAL_TYPES_SIZEOFLONG < 8)
    if( (+n < nThres) && (-n < nThres) )
        n = (2 * n * nMapDenom) / (nDPI * nMapNum);
    else
#endif
    {
        sal_Int64 n64 = n;
        n64 *= nMapDenom;
        long nDenom  = nDPI * nMapNum;
        n = (long)(2 * n64 / nDenom);
    }
    if( n < 0 ) --n; else ++n;
    return (n / 2);
}

// -----------------------------------------------------------------------

long OutputDevice::ImplLogicXToDevicePixel( long nX ) const
{
    if ( !mbMap )
        return nX+mnOutOffX;

    return ImplLogicToPixel( nX + maMapRes.mnMapOfsX, mnDPIX,
                             maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                             maThresRes.mnThresLogToPixX )+mnOutOffX+mnOutOffOrigX;
}

// -----------------------------------------------------------------------

long OutputDevice::ImplLogicYToDevicePixel( long nY ) const
{
    if ( !mbMap )
        return nY+mnOutOffY;

    return ImplLogicToPixel( nY + maMapRes.mnMapOfsY, mnDPIY,
                             maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                             maThresRes.mnThresLogToPixY )+mnOutOffY+mnOutOffOrigY;
}

// -----------------------------------------------------------------------

long OutputDevice::ImplLogicWidthToDevicePixel( long nWidth ) const
{
    if ( !mbMap )
        return nWidth;

    return ImplLogicToPixel( nWidth, mnDPIX,
                             maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                             maThresRes.mnThresLogToPixX );
}

// -----------------------------------------------------------------------

long OutputDevice::ImplLogicHeightToDevicePixel( long nHeight ) const
{
    if ( !mbMap )
        return nHeight;

    return ImplLogicToPixel( nHeight, mnDPIY,
                             maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                             maThresRes.mnThresLogToPixY );
}

float OutputDevice::ImplFloatLogicHeightToDevicePixel( float fLogicHeight) const
{
    if( !mbMap)
        return fLogicHeight;
    float fPixelHeight = (fLogicHeight * mnDPIY * maMapRes.mnMapScNumY) / maMapRes.mnMapScDenomY;
    return fPixelHeight;
}

// -----------------------------------------------------------------------

long OutputDevice::ImplDevicePixelToLogicWidth( long nWidth ) const
{
    if ( !mbMap )
        return nWidth;

    return ImplPixelToLogic( nWidth, mnDPIX,
                             maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                             maThresRes.mnThresPixToLogX );
}

// -----------------------------------------------------------------------

long OutputDevice::ImplDevicePixelToLogicHeight( long nHeight ) const
{
    if ( !mbMap )
        return nHeight;

    return ImplPixelToLogic( nHeight, mnDPIY,
                             maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                             maThresRes.mnThresPixToLogY );
}

// -----------------------------------------------------------------------

Point OutputDevice::ImplLogicToDevicePixel( const Point& rLogicPt ) const
{
    if ( !mbMap )
        return Point( rLogicPt.X()+mnOutOffX, rLogicPt.Y()+mnOutOffY );

    return Point( ImplLogicToPixel( rLogicPt.X() + maMapRes.mnMapOfsX, mnDPIX,
                                    maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                    maThresRes.mnThresLogToPixX )+mnOutOffX+mnOutOffOrigX,
                  ImplLogicToPixel( rLogicPt.Y() + maMapRes.mnMapOfsY, mnDPIY,
                                    maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                    maThresRes.mnThresLogToPixY )+mnOutOffY+mnOutOffOrigY );
}

// -----------------------------------------------------------------------

Size OutputDevice::ImplLogicToDevicePixel( const Size& rLogicSize ) const
{
    if ( !mbMap )
        return rLogicSize;

    return Size( ImplLogicToPixel( rLogicSize.Width(), mnDPIX,
                                   maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                   maThresRes.mnThresLogToPixX ),
                 ImplLogicToPixel( rLogicSize.Height(), mnDPIY,
                                   maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                   maThresRes.mnThresLogToPixY ) );
}

// -----------------------------------------------------------------------

Rectangle OutputDevice::ImplLogicToDevicePixel( const Rectangle& rLogicRect ) const
{
    if ( rLogicRect.IsEmpty() )
        return rLogicRect;

    if ( !mbMap )
    {
        return Rectangle( rLogicRect.Left()+mnOutOffX, rLogicRect.Top()+mnOutOffY,
                          rLogicRect.Right()+mnOutOffX, rLogicRect.Bottom()+mnOutOffY );
    }

    return Rectangle( ImplLogicToPixel( rLogicRect.Left()+maMapRes.mnMapOfsX, mnDPIX,
                                        maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                        maThresRes.mnThresLogToPixX )+mnOutOffX+mnOutOffOrigX,
                      ImplLogicToPixel( rLogicRect.Top()+maMapRes.mnMapOfsY, mnDPIY,
                                        maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                        maThresRes.mnThresLogToPixY )+mnOutOffY+mnOutOffOrigY,
                      ImplLogicToPixel( rLogicRect.Right()+maMapRes.mnMapOfsX, mnDPIX,
                                        maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                        maThresRes.mnThresLogToPixX )+mnOutOffX+mnOutOffOrigX,
                      ImplLogicToPixel( rLogicRect.Bottom()+maMapRes.mnMapOfsY, mnDPIY,
                                        maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                        maThresRes.mnThresLogToPixY )+mnOutOffY+mnOutOffOrigY );
}

// -----------------------------------------------------------------------

Polygon OutputDevice::ImplLogicToDevicePixel( const Polygon& rLogicPoly ) const
{
    if ( !mbMap && !mnOutOffX && !mnOutOffY )
        return rLogicPoly;

    sal_uInt16  i;
    sal_uInt16  nPoints = rLogicPoly.GetSize();
    Polygon aPoly( rLogicPoly );

    // Pointer auf das Point-Array holen (Daten werden kopiert)
    const Point* pPointAry = aPoly.GetConstPointAry();

    if ( mbMap )
    {
        for ( i = 0; i < nPoints; i++ )
        {
            const Point* pPt = &(pPointAry[i]);
            Point aPt;
            aPt.X() = ImplLogicToPixel( pPt->X()+maMapRes.mnMapOfsX, mnDPIX,
                                        maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                        maThresRes.mnThresLogToPixX )+mnOutOffX+mnOutOffOrigX;
            aPt.Y() = ImplLogicToPixel( pPt->Y()+maMapRes.mnMapOfsY, mnDPIY,
                                        maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                        maThresRes.mnThresLogToPixY )+mnOutOffY+mnOutOffOrigY;
            aPoly[i] = aPt;
        }
    }
    else
    {
        for ( i = 0; i < nPoints; i++ )
        {
            Point aPt = pPointAry[i];
            aPt.X() += mnOutOffX;
            aPt.Y() += mnOutOffY;
            aPoly[i] = aPt;
        }
    }

    return aPoly;
}

// -----------------------------------------------------------------------

PolyPolygon OutputDevice::ImplLogicToDevicePixel( const PolyPolygon& rLogicPolyPoly ) const
{
    if ( !mbMap && !mnOutOffX && !mnOutOffY )
        return rLogicPolyPoly;

    PolyPolygon aPolyPoly( rLogicPolyPoly );
    sal_uInt16      nPoly = aPolyPoly.Count();
    for( sal_uInt16 i = 0; i < nPoly; i++ )
    {
        Polygon& rPoly = aPolyPoly[i];
        rPoly = ImplLogicToDevicePixel( rPoly );
    }
    return aPolyPoly;
}

// -----------------------------------------------------------------------

LineInfo OutputDevice::ImplLogicToDevicePixel( const LineInfo& rLineInfo ) const
{
    LineInfo aInfo( rLineInfo );

    if( aInfo.GetStyle() == LINE_DASH )
    {
        if( aInfo.GetDotCount() && aInfo.GetDotLen() )
            aInfo.SetDotLen( Max( ImplLogicWidthToDevicePixel( aInfo.GetDotLen() ), 1L ) );
        else
            aInfo.SetDotCount( 0 );

        if( aInfo.GetDashCount() && aInfo.GetDashLen() )
            aInfo.SetDashLen( Max( ImplLogicWidthToDevicePixel( aInfo.GetDashLen() ), 1L ) );
        else
            aInfo.SetDashCount( 0 );

        aInfo.SetDistance( ImplLogicWidthToDevicePixel( aInfo.GetDistance() ) );

        if( ( !aInfo.GetDashCount() && !aInfo.GetDotCount() ) || !aInfo.GetDistance() )
            aInfo.SetStyle( LINE_SOLID );
    }

    aInfo.SetWidth( ImplLogicWidthToDevicePixel( aInfo.GetWidth() ) );

    return aInfo;
}

// -----------------------------------------------------------------------

Rectangle OutputDevice::ImplDevicePixelToLogic( const Rectangle& rPixelRect ) const
{
    if ( rPixelRect.IsEmpty() )
        return rPixelRect;

    if ( !mbMap )
    {
        return Rectangle( rPixelRect.Left()-mnOutOffX, rPixelRect.Top()-mnOutOffY,
                          rPixelRect.Right()-mnOutOffX, rPixelRect.Bottom()-mnOutOffY );
    }

    return Rectangle( ImplPixelToLogic( rPixelRect.Left()-mnOutOffX-mnOutOffOrigX, mnDPIX,
                                        maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                        maThresRes.mnThresPixToLogX )-maMapRes.mnMapOfsX,
                      ImplPixelToLogic( rPixelRect.Top()-mnOutOffY-mnOutOffOrigY, mnDPIY,
                                        maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                        maThresRes.mnThresPixToLogY )-maMapRes.mnMapOfsY,
                      ImplPixelToLogic( rPixelRect.Right()-mnOutOffX-mnOutOffOrigX, mnDPIX,
                                        maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                        maThresRes.mnThresPixToLogX )-maMapRes.mnMapOfsX,
                      ImplPixelToLogic( rPixelRect.Bottom()-mnOutOffY-mnOutOffOrigY, mnDPIY,
                                        maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                        maThresRes.mnThresPixToLogY )-maMapRes.mnMapOfsY );
}

// -----------------------------------------------------------------------

Region OutputDevice::ImplPixelToDevicePixel( const Region& rRegion ) const
{
    DBG_CHKOBJ( &rRegion, Region, ImplDbgTestRegion );

    if ( !mnOutOffX && !mnOutOffY )
        return rRegion;

    Region aRegion( rRegion );
    aRegion.Move( mnOutOffX+mnOutOffOrigX, mnOutOffY+mnOutOffOrigY );
    return aRegion;
}

// -----------------------------------------------------------------------

void OutputDevice::EnableMapMode( sal_Bool bEnable )
{
    mbMap = (bEnable != 0);

    if( mpAlphaVDev )
        mpAlphaVDev->EnableMapMode( bEnable );
}

// -----------------------------------------------------------------------

void OutputDevice::SetMapMode()
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    if ( mpMetaFile )
        mpMetaFile->AddAction( new MetaMapModeAction( MapMode() ) );

    if ( mbMap || !maMapMode.IsDefault() )
    {
        mbMap       = sal_False;
        maMapMode   = MapMode();

        // create new objects (clip region werden nicht neu skaliert)
        mbNewFont   = sal_True;
        mbInitFont  = sal_True;
        if ( GetOutDevType() == OUTDEV_WINDOW )
        {
            if ( ((Window*)this)->mpWindowImpl->mpCursor )
                ((Window*)this)->mpWindowImpl->mpCursor->ImplNew();
        }

        // #106426# Adapt logical offset when changing mapmode
        mnOutOffLogicX = mnOutOffOrigX; // no mapping -> equal offsets
        mnOutOffLogicY = mnOutOffOrigY;

        // #i75163#
        ImplInvalidateViewTransform();
    }

    if( mpAlphaVDev )
        mpAlphaVDev->SetMapMode();
}

// -----------------------------------------------------------------------

void OutputDevice::SetMapMode( const MapMode& rNewMapMode )
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    sal_Bool bRelMap = (rNewMapMode.GetMapUnit() == MAP_RELATIVE);

    if ( mpMetaFile )
    {
        mpMetaFile->AddAction( new MetaMapModeAction( rNewMapMode ) );
#ifdef DBG_UTIL
        if ( GetOutDevType() != OUTDEV_PRINTER )
            DBG_ASSERTWARNING( bRelMap, "Please record only relative MapModes!" );
#endif
    }

    // Ist der MapMode der gleiche wie vorher, dann mache nichts
    if ( maMapMode == rNewMapMode )
        return;

    if( mpAlphaVDev )
        mpAlphaVDev->SetMapMode( rNewMapMode );

    // Ist Default-MapMode, dann bereche nichts
    sal_Bool bOldMap = mbMap;
    mbMap = !rNewMapMode.IsDefault();
    if ( mbMap )
    {
        // Falls nur der Orign umgesetzt wird, dann scaliere nichts neu
        if ( (rNewMapMode.GetMapUnit() == maMapMode.GetMapUnit()) &&
             (rNewMapMode.GetScaleX()  == maMapMode.GetScaleX())  &&
             (rNewMapMode.GetScaleY()  == maMapMode.GetScaleY())  &&
             (bOldMap                  == mbMap) )
        {
            // Offset setzen
            Point aOrigin = rNewMapMode.GetOrigin();
            maMapRes.mnMapOfsX = aOrigin.X();
            maMapRes.mnMapOfsY = aOrigin.Y();
            maMapMode = rNewMapMode;

            // #i75163#
            ImplInvalidateViewTransform();

            return;
        }
        if ( !bOldMap && bRelMap )
        {
            maMapRes.mnMapScNumX    = 1;
            maMapRes.mnMapScNumY    = 1;
            maMapRes.mnMapScDenomX  = mnDPIX;
            maMapRes.mnMapScDenomY  = mnDPIY;
            maMapRes.mnMapOfsX      = 0;
            maMapRes.mnMapOfsY      = 0;
        }

        // Neue MapMode-Aufloesung berechnen
        ImplCalcMapResolution( rNewMapMode, mnDPIX, mnDPIY, maMapRes, maThresRes );
    }

    // Neuen MapMode setzen
    if ( bRelMap )
    {
        Point aOrigin( maMapRes.mnMapOfsX, maMapRes.mnMapOfsY );
        // aScale? = maMapMode.GetScale?() * rNewMapMode.GetScale?()
        Fraction aScaleX = ImplMakeFraction( maMapMode.GetScaleX().GetNumerator(),
                                             rNewMapMode.GetScaleX().GetNumerator(),
                                             maMapMode.GetScaleX().GetDenominator(),
                                             rNewMapMode.GetScaleX().GetDenominator() );
        Fraction aScaleY = ImplMakeFraction( maMapMode.GetScaleY().GetNumerator(),
                                             rNewMapMode.GetScaleY().GetNumerator(),
                                             maMapMode.GetScaleY().GetDenominator(),
                                             rNewMapMode.GetScaleY().GetDenominator() );
        maMapMode.SetOrigin( aOrigin );
        maMapMode.SetScaleX( aScaleX );
        maMapMode.SetScaleY( aScaleY );
    }
    else
        maMapMode = rNewMapMode;

    // create new objects (clip region werden nicht neu skaliert)
    mbNewFont   = sal_True;
    mbInitFont  = sal_True;
    if ( GetOutDevType() == OUTDEV_WINDOW )
    {
        if ( ((Window*)this)->mpWindowImpl->mpCursor )
            ((Window*)this)->mpWindowImpl->mpCursor->ImplNew();
    }

    // #106426# Adapt logical offset when changing mapmode
    mnOutOffLogicX = ImplPixelToLogic( mnOutOffOrigX, mnDPIX,
                                       maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                       maThresRes.mnThresPixToLogX );
    mnOutOffLogicY = ImplPixelToLogic( mnOutOffOrigY, mnDPIY,
                                       maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                       maThresRes.mnThresPixToLogY );

    // #i75163#
    ImplInvalidateViewTransform();
}

// -----------------------------------------------------------------------

void OutputDevice::SetRelativeMapMode( const MapMode& rNewMapMode )
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    // Ist der MapMode der gleiche wie vorher, dann mache nichts
    if ( maMapMode == rNewMapMode )
        return;

    MapUnit eOld = maMapMode.GetMapUnit();
    MapUnit eNew = rNewMapMode.GetMapUnit();

    // a?F = rNewMapMode.GetScale?() / maMapMode.GetScale?()
    Fraction aXF = ImplMakeFraction( rNewMapMode.GetScaleX().GetNumerator(),
                                     maMapMode.GetScaleX().GetDenominator(),
                                     rNewMapMode.GetScaleX().GetDenominator(),
                                     maMapMode.GetScaleX().GetNumerator() );
    Fraction aYF = ImplMakeFraction( rNewMapMode.GetScaleY().GetNumerator(),
                                     maMapMode.GetScaleY().GetDenominator(),
                                     rNewMapMode.GetScaleY().GetDenominator(),
                                     maMapMode.GetScaleY().GetNumerator() );

    Point aPt( LogicToLogic( Point(), NULL, &rNewMapMode ) );
    if ( eNew != eOld )
    {
        if ( eOld > MAP_PIXEL )
        {
            SAL_WARN( "vcl.gdi", "Not implemented MapUnit" );
        }
        else if ( eNew > MAP_PIXEL )
        {
            SAL_WARN( "vcl.gdi", "Not implemented MapUnit" );
        }
        else
        {
            Fraction aF( aImplNumeratorAry[eNew] * aImplDenominatorAry[eOld],
                         aImplNumeratorAry[eOld] * aImplDenominatorAry[eNew] );

            // a?F =  a?F * aF
            aXF = ImplMakeFraction( aXF.GetNumerator(),   aF.GetNumerator(),
                                    aXF.GetDenominator(), aF.GetDenominator() );
            aYF = ImplMakeFraction( aYF.GetNumerator(),   aF.GetNumerator(),
                                    aYF.GetDenominator(), aF.GetDenominator() );
            if ( eOld == MAP_PIXEL )
            {
                aXF *= Fraction( mnDPIX, 1 );
                aYF *= Fraction( mnDPIY, 1 );
            }
            else if ( eNew == MAP_PIXEL )
            {
                aXF *= Fraction( 1, mnDPIX );
                aYF *= Fraction( 1, mnDPIY );
            }
        }
    }

    MapMode aNewMapMode( MAP_RELATIVE, Point( -aPt.X(), -aPt.Y() ), aXF, aYF );
    SetMapMode( aNewMapMode );

    if ( eNew != eOld )
        maMapMode = rNewMapMode;

    // #106426# Adapt logical offset when changing mapmode
    mnOutOffLogicX = ImplPixelToLogic( mnOutOffOrigX, mnDPIX,
                                       maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                       maThresRes.mnThresPixToLogX );
    mnOutOffLogicY = ImplPixelToLogic( mnOutOffOrigY, mnDPIY,
                                       maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                       maThresRes.mnThresPixToLogY );

    if( mpAlphaVDev )
        mpAlphaVDev->SetRelativeMapMode( rNewMapMode );
}

// -----------------------------------------------------------------------

// #i75163#
basegfx::B2DHomMatrix OutputDevice::GetViewTransformation() const
{
    if(mbMap)
    {
        // #i82615#
        if(!mpOutDevData)
        {
            const_cast< OutputDevice* >(this)->ImplInitOutDevData();
        }

        if(!mpOutDevData->mpViewTransform)
        {
            mpOutDevData->mpViewTransform = new basegfx::B2DHomMatrix;

            const double fScaleFactorX((double)mnDPIX * (double)maMapRes.mnMapScNumX / (double)maMapRes.mnMapScDenomX);
            const double fScaleFactorY((double)mnDPIY * (double)maMapRes.mnMapScNumY / (double)maMapRes.mnMapScDenomY);
            const double fZeroPointX(((double)maMapRes.mnMapOfsX * fScaleFactorX) + (double)mnOutOffOrigX);
            const double fZeroPointY(((double)maMapRes.mnMapOfsY * fScaleFactorY) + (double)mnOutOffOrigY);

            mpOutDevData->mpViewTransform->set(0, 0, fScaleFactorX);
            mpOutDevData->mpViewTransform->set(1, 1, fScaleFactorY);
            mpOutDevData->mpViewTransform->set(0, 2, fZeroPointX);
            mpOutDevData->mpViewTransform->set(1, 2, fZeroPointY);
        }

        return *mpOutDevData->mpViewTransform;
    }
    else
    {
        return basegfx::B2DHomMatrix();
    }
}

// -----------------------------------------------------------------------

// #i75163#
basegfx::B2DHomMatrix OutputDevice::GetInverseViewTransformation() const
{
    if(mbMap)
    {
        // #i82615#
        if(!mpOutDevData)
        {
            const_cast< OutputDevice* >(this)->ImplInitOutDevData();
        }

        if(!mpOutDevData->mpInverseViewTransform)
        {
            GetViewTransformation();
            mpOutDevData->mpInverseViewTransform = new basegfx::B2DHomMatrix(*mpOutDevData->mpViewTransform);
            mpOutDevData->mpInverseViewTransform->invert();
        }

        return *mpOutDevData->mpInverseViewTransform;
    }
    else
    {
        return basegfx::B2DHomMatrix();
    }
}

// -----------------------------------------------------------------------

// #i75163#
basegfx::B2DHomMatrix OutputDevice::GetViewTransformation( const MapMode& rMapMode ) const
{
    // #i82615#
    ImplMapRes          aMapRes;
    ImplThresholdRes    aThresRes;
    ImplCalcMapResolution( rMapMode, mnDPIX, mnDPIY, aMapRes, aThresRes );

    basegfx::B2DHomMatrix aTransform;

    const double fScaleFactorX((double)mnDPIX * (double)aMapRes.mnMapScNumX / (double)aMapRes.mnMapScDenomX);
    const double fScaleFactorY((double)mnDPIY * (double)aMapRes.mnMapScNumY / (double)aMapRes.mnMapScDenomY);
    const double fZeroPointX(((double)aMapRes.mnMapOfsX * fScaleFactorX) + (double)mnOutOffOrigX);
    const double fZeroPointY(((double)aMapRes.mnMapOfsY * fScaleFactorY) + (double)mnOutOffOrigY);

    aTransform.set(0, 0, fScaleFactorX);
    aTransform.set(1, 1, fScaleFactorY);
    aTransform.set(0, 2, fZeroPointX);
    aTransform.set(1, 2, fZeroPointY);

    return aTransform;
}

// -----------------------------------------------------------------------

// #i75163#
basegfx::B2DHomMatrix OutputDevice::GetInverseViewTransformation( const MapMode& rMapMode ) const
{
    basegfx::B2DHomMatrix aMatrix( GetViewTransformation( rMapMode ) );
    aMatrix.invert();
    return aMatrix;
}

// -----------------------------------------------------------------------

basegfx::B2DHomMatrix OutputDevice::ImplGetDeviceTransformation() const
{
    basegfx::B2DHomMatrix aTransformation = GetViewTransformation();
    // TODO: is it worth to cache the transformed result?
    if( mnOutOffX || mnOutOffY )
        aTransformation.translate( mnOutOffX, mnOutOffY );
    return aTransformation;
}

// -----------------------------------------------------------------------

Point OutputDevice::LogicToPixel( const Point& rLogicPt ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    if ( !mbMap )
        return rLogicPt;

    return Point( ImplLogicToPixel( rLogicPt.X() + maMapRes.mnMapOfsX, mnDPIX,
                                    maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                    maThresRes.mnThresLogToPixX )+mnOutOffOrigX,
                  ImplLogicToPixel( rLogicPt.Y() + maMapRes.mnMapOfsY, mnDPIY,
                                    maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                    maThresRes.mnThresLogToPixY )+mnOutOffOrigY );
}

// -----------------------------------------------------------------------

Size OutputDevice::LogicToPixel( const Size& rLogicSize ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    if ( !mbMap )
        return rLogicSize;

    return Size( ImplLogicToPixel( rLogicSize.Width(), mnDPIX,
                                   maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                   maThresRes.mnThresLogToPixX ),
                 ImplLogicToPixel( rLogicSize.Height(), mnDPIY,
                                   maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                   maThresRes.mnThresLogToPixY ) );
}

// -----------------------------------------------------------------------

Rectangle OutputDevice::LogicToPixel( const Rectangle& rLogicRect ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    if ( !mbMap || rLogicRect.IsEmpty() )
        return rLogicRect;

    return Rectangle( ImplLogicToPixel( rLogicRect.Left() + maMapRes.mnMapOfsX, mnDPIX,
                                        maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                        maThresRes.mnThresLogToPixX )+mnOutOffOrigX,
                      ImplLogicToPixel( rLogicRect.Top() + maMapRes.mnMapOfsY, mnDPIY,
                                        maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                        maThresRes.mnThresLogToPixY )+mnOutOffOrigY,
                      ImplLogicToPixel( rLogicRect.Right() + maMapRes.mnMapOfsX, mnDPIX,
                                        maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                        maThresRes.mnThresLogToPixX )+mnOutOffOrigX,
                      ImplLogicToPixel( rLogicRect.Bottom() + maMapRes.mnMapOfsY, mnDPIY,
                                        maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                        maThresRes.mnThresLogToPixY )+mnOutOffOrigY );
}

// -----------------------------------------------------------------------

Polygon OutputDevice::LogicToPixel( const Polygon& rLogicPoly ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );
    DBG_CHKOBJ( &rLogicPoly, Polygon, NULL );

    if ( !mbMap )
        return rLogicPoly;

    sal_uInt16  i;
    sal_uInt16  nPoints = rLogicPoly.GetSize();
    Polygon aPoly( rLogicPoly );

    // Pointer auf das Point-Array holen (Daten werden kopiert)
    const Point* pPointAry = aPoly.GetConstPointAry();

    for ( i = 0; i < nPoints; i++ )
    {
        const Point* pPt = &(pPointAry[i]);
        Point aPt;
        aPt.X() = ImplLogicToPixel( pPt->X() + maMapRes.mnMapOfsX, mnDPIX,
                                    maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                    maThresRes.mnThresLogToPixX )+mnOutOffOrigX;
        aPt.Y() = ImplLogicToPixel( pPt->Y() + maMapRes.mnMapOfsY, mnDPIY,
                                    maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                    maThresRes.mnThresLogToPixY )+mnOutOffOrigY;
        aPoly[i] = aPt;
    }

    return aPoly;
}

// -----------------------------------------------------------------------

PolyPolygon OutputDevice::LogicToPixel( const PolyPolygon& rLogicPolyPoly ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );
    DBG_CHKOBJ( &rLogicPolyPoly, PolyPolygon, NULL );

    if ( !mbMap )
        return rLogicPolyPoly;

    PolyPolygon aPolyPoly( rLogicPolyPoly );
    sal_uInt16      nPoly = aPolyPoly.Count();
    for( sal_uInt16 i = 0; i < nPoly; i++ )
    {
        Polygon& rPoly = aPolyPoly[i];
        rPoly = LogicToPixel( rPoly );
    }
    return aPolyPoly;
}

// -----------------------------------------------------------------------

Region OutputDevice::LogicToPixel( const Region& rLogicRegion ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );
    DBG_CHKOBJ( &rLogicRegion, Region, ImplDbgTestRegion );

    RegionType eType = rLogicRegion.GetType();

    if ( !mbMap || (eType == REGION_EMPTY) || (eType == REGION_NULL) )
        return rLogicRegion;

    Region          aRegion;
    const ImplRegion& rImplRegion = *rLogicRegion.ImplGetImplRegion();
    const PolyPolygon* pPolyPoly = rImplRegion.mpPolyPoly;
    const basegfx::B2DPolyPolygon* pB2DPolyPoly = rImplRegion.mpB2DPolyPoly;

    if ( pPolyPoly )
        aRegion = Region( LogicToPixel( *pPolyPoly ) );
    else if( pB2DPolyPoly )
    {
        basegfx::B2DPolyPolygon aTransformedPoly = *pB2DPolyPoly;
        const ::basegfx::B2DHomMatrix& rTransformationMatrix = GetViewTransformation();
        aTransformedPoly.transform( rTransformationMatrix );
        aRegion = Region( aTransformedPoly );
    }
    else
    {
        long                nX;
        long                nY;
        long                nWidth;
        long                nHeight;
        ImplRegionInfo      aInfo;
        sal_Bool                bRegionRect;

        aRegion.ImplBeginAddRect();
        bRegionRect = rLogicRegion.ImplGetFirstRect( aInfo, nX, nY, nWidth, nHeight );
        while ( bRegionRect )
        {
            Rectangle aRect( Point( nX, nY ), Size( nWidth, nHeight ) );
            aRegion.ImplAddRect( LogicToPixel( aRect ) );
            bRegionRect = rLogicRegion.ImplGetNextRect( aInfo, nX, nY, nWidth, nHeight );
        }
        aRegion.ImplEndAddRect();
    }

    return aRegion;
}

// -----------------------------------------------------------------------

Point OutputDevice::LogicToPixel( const Point& rLogicPt,
                                  const MapMode& rMapMode ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    if ( rMapMode.IsDefault() )
        return rLogicPt;

    // MapMode-Aufloesung berechnen und Umrechnen
    ImplMapRes          aMapRes;
    ImplThresholdRes    aThresRes;
    ImplCalcMapResolution( rMapMode, mnDPIX, mnDPIY, aMapRes, aThresRes );

    return Point( ImplLogicToPixel( rLogicPt.X() + aMapRes.mnMapOfsX, mnDPIX,
                                    aMapRes.mnMapScNumX, aMapRes.mnMapScDenomX,
                                    aThresRes.mnThresLogToPixX )+mnOutOffOrigX,
                  ImplLogicToPixel( rLogicPt.Y() + aMapRes.mnMapOfsY, mnDPIY,
                                    aMapRes.mnMapScNumY, aMapRes.mnMapScDenomY,
                                    aThresRes.mnThresLogToPixY )+mnOutOffOrigY );
}

// -----------------------------------------------------------------------

Size OutputDevice::LogicToPixel( const Size& rLogicSize,
                                 const MapMode& rMapMode ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    if ( rMapMode.IsDefault() )
        return rLogicSize;

    // MapMode-Aufloesung berechnen und Umrechnen
    ImplMapRes          aMapRes;
    ImplThresholdRes    aThresRes;
    ImplCalcMapResolution( rMapMode, mnDPIX, mnDPIY, aMapRes, aThresRes );

    return Size( ImplLogicToPixel( rLogicSize.Width(), mnDPIX,
                                   aMapRes.mnMapScNumX, aMapRes.mnMapScDenomX,
                                   aThresRes.mnThresLogToPixX ),
                 ImplLogicToPixel( rLogicSize.Height(), mnDPIY,
                                   aMapRes.mnMapScNumY, aMapRes.mnMapScDenomY,
                                   aThresRes.mnThresLogToPixY ) );
}

// -----------------------------------------------------------------------

Rectangle OutputDevice::LogicToPixel( const Rectangle& rLogicRect,
                                      const MapMode& rMapMode ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    if ( rMapMode.IsDefault() || rLogicRect.IsEmpty() )
        return rLogicRect;

    // MapMode-Aufloesung berechnen und Umrechnen
    ImplMapRes          aMapRes;
    ImplThresholdRes    aThresRes;
    ImplCalcMapResolution( rMapMode, mnDPIX, mnDPIY, aMapRes, aThresRes );

    return Rectangle( ImplLogicToPixel( rLogicRect.Left() + aMapRes.mnMapOfsX, mnDPIX,
                                        aMapRes.mnMapScNumX, aMapRes.mnMapScDenomX,
                                        aThresRes.mnThresLogToPixX )+mnOutOffOrigX,
                      ImplLogicToPixel( rLogicRect.Top() + aMapRes.mnMapOfsY, mnDPIY,
                                        aMapRes.mnMapScNumY, aMapRes.mnMapScDenomY,
                                        aThresRes.mnThresLogToPixY )+mnOutOffOrigY,
                      ImplLogicToPixel( rLogicRect.Right() + aMapRes.mnMapOfsX, mnDPIX,
                                        aMapRes.mnMapScNumX, aMapRes.mnMapScDenomX,
                                        aThresRes.mnThresLogToPixX )+mnOutOffOrigX,
                      ImplLogicToPixel( rLogicRect.Bottom() + aMapRes.mnMapOfsY, mnDPIY,
                                        aMapRes.mnMapScNumY, aMapRes.mnMapScDenomY,
                                        aThresRes.mnThresLogToPixY )+mnOutOffOrigY );
}

// -----------------------------------------------------------------------

Polygon OutputDevice::LogicToPixel( const Polygon& rLogicPoly,
                                    const MapMode& rMapMode ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );
    DBG_CHKOBJ( &rLogicPoly, Polygon, NULL );

    if ( rMapMode.IsDefault() )
        return rLogicPoly;

    // MapMode-Aufloesung berechnen und Umrechnen
    ImplMapRes          aMapRes;
    ImplThresholdRes    aThresRes;
    ImplCalcMapResolution( rMapMode, mnDPIX, mnDPIY, aMapRes, aThresRes );

    sal_uInt16  i;
    sal_uInt16  nPoints = rLogicPoly.GetSize();
    Polygon aPoly( rLogicPoly );

    // Pointer auf das Point-Array holen (Daten werden kopiert)
    const Point* pPointAry = aPoly.GetConstPointAry();

    for ( i = 0; i < nPoints; i++ )
    {
        const Point* pPt = &(pPointAry[i]);
        Point aPt;
        aPt.X() = ImplLogicToPixel( pPt->X() + aMapRes.mnMapOfsX, mnDPIX,
                                    aMapRes.mnMapScNumX, aMapRes.mnMapScDenomX,
                                    aThresRes.mnThresLogToPixX )+mnOutOffOrigX;
        aPt.Y() = ImplLogicToPixel( pPt->Y() + aMapRes.mnMapOfsY, mnDPIY,
                                    aMapRes.mnMapScNumY, aMapRes.mnMapScDenomY,
                                    aThresRes.mnThresLogToPixY )+mnOutOffOrigY;
        aPoly[i] = aPt;
    }

    return aPoly;
}

// -----------------------------------------------------------------------

basegfx::B2DPolyPolygon OutputDevice::LogicToPixel( const basegfx::B2DPolyPolygon& rLogicPolyPoly,
                                                    const MapMode& rMapMode ) const
{
    basegfx::B2DPolyPolygon aTransformedPoly = rLogicPolyPoly;
    const ::basegfx::B2DHomMatrix& rTransformationMatrix = GetViewTransformation( rMapMode );
    aTransformedPoly.transform( rTransformationMatrix );
    return aTransformedPoly;
}

// -----------------------------------------------------------------------

Point OutputDevice::PixelToLogic( const Point& rDevicePt ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    if ( !mbMap )
        return rDevicePt;

    return Point( ImplPixelToLogic( rDevicePt.X(), mnDPIX,
                                    maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                    maThresRes.mnThresPixToLogX ) - maMapRes.mnMapOfsX - mnOutOffLogicX,
                  ImplPixelToLogic( rDevicePt.Y(), mnDPIY,
                                    maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                    maThresRes.mnThresPixToLogY ) - maMapRes.mnMapOfsY - mnOutOffLogicY );
}

// -----------------------------------------------------------------------

Size OutputDevice::PixelToLogic( const Size& rDeviceSize ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    if ( !mbMap )
        return rDeviceSize;

    return Size( ImplPixelToLogic( rDeviceSize.Width(), mnDPIX,
                                   maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                   maThresRes.mnThresPixToLogX ),
                 ImplPixelToLogic( rDeviceSize.Height(), mnDPIY,
                                   maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                   maThresRes.mnThresPixToLogY ) );
}

// -----------------------------------------------------------------------

Rectangle OutputDevice::PixelToLogic( const Rectangle& rDeviceRect ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    if ( !mbMap || rDeviceRect.IsEmpty() )
        return rDeviceRect;

    return Rectangle( ImplPixelToLogic( rDeviceRect.Left(), mnDPIX,
                                        maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                        maThresRes.mnThresPixToLogX ) - maMapRes.mnMapOfsX - mnOutOffLogicX,
                      ImplPixelToLogic( rDeviceRect.Top(), mnDPIY,
                                        maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                        maThresRes.mnThresPixToLogY ) - maMapRes.mnMapOfsY - mnOutOffLogicY,
                      ImplPixelToLogic( rDeviceRect.Right(), mnDPIX,
                                        maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                        maThresRes.mnThresPixToLogX ) - maMapRes.mnMapOfsX - mnOutOffLogicX,
                      ImplPixelToLogic( rDeviceRect.Bottom(), mnDPIY,
                                        maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                        maThresRes.mnThresPixToLogY ) - maMapRes.mnMapOfsY - mnOutOffLogicY );
}

// -----------------------------------------------------------------------

Polygon OutputDevice::PixelToLogic( const Polygon& rDevicePoly ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );
    DBG_CHKOBJ( &rDevicePoly, Polygon, NULL );

    if ( !mbMap )
        return rDevicePoly;

    sal_uInt16  i;
    sal_uInt16  nPoints = rDevicePoly.GetSize();
    Polygon aPoly( rDevicePoly );

    // Pointer auf das Point-Array holen (Daten werden kopiert)
    const Point* pPointAry = aPoly.GetConstPointAry();

    for ( i = 0; i < nPoints; i++ )
    {
        const Point* pPt = &(pPointAry[i]);
        Point aPt;
        aPt.X() = ImplPixelToLogic( pPt->X(), mnDPIX,
                                    maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                    maThresRes.mnThresPixToLogX ) - maMapRes.mnMapOfsX - mnOutOffLogicX;
        aPt.Y() = ImplPixelToLogic( pPt->Y(), mnDPIY,
                                    maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                    maThresRes.mnThresPixToLogY ) - maMapRes.mnMapOfsY - mnOutOffLogicY;
        aPoly[i] = aPt;
    }

    return aPoly;
}

// -----------------------------------------------------------------------

PolyPolygon OutputDevice::PixelToLogic( const PolyPolygon& rDevicePolyPoly ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );
    DBG_CHKOBJ( &rDevicePolyPoly, PolyPolygon, NULL );

    if ( !mbMap )
        return rDevicePolyPoly;

    PolyPolygon aPolyPoly( rDevicePolyPoly );
    sal_uInt16      nPoly = aPolyPoly.Count();
    for( sal_uInt16 i = 0; i < nPoly; i++ )
    {
        Polygon& rPoly = aPolyPoly[i];
        rPoly = PixelToLogic( rPoly );
    }
    return aPolyPoly;
}

// -----------------------------------------------------------------------

Region OutputDevice::PixelToLogic( const Region& rDeviceRegion ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );
    DBG_CHKOBJ( &rDeviceRegion, Region, ImplDbgTestRegion );

    RegionType eType = rDeviceRegion.GetType();

    if ( !mbMap || (eType == REGION_EMPTY) || (eType == REGION_NULL) )
        return rDeviceRegion;

    Region          aRegion;
    PolyPolygon*    pPolyPoly = rDeviceRegion.ImplGetImplRegion()->mpPolyPoly;

    if ( pPolyPoly )
        aRegion = Region( PixelToLogic( *pPolyPoly ) );
    else
    {
        long                nX;
        long                nY;
        long                nWidth;
        long                nHeight;
        ImplRegionInfo      aInfo;
        sal_Bool                bRegionRect;

        aRegion.ImplBeginAddRect();
        bRegionRect = rDeviceRegion.ImplGetFirstRect( aInfo, nX, nY, nWidth, nHeight );
        while ( bRegionRect )
        {
            Rectangle aRect( Point( nX, nY ), Size( nWidth, nHeight ) );
            aRegion.ImplAddRect( PixelToLogic( aRect ) );
            bRegionRect = rDeviceRegion.ImplGetNextRect( aInfo, nX, nY, nWidth, nHeight );
        }
        aRegion.ImplEndAddRect();
    }

    return aRegion;
}

// -----------------------------------------------------------------------

Point OutputDevice::PixelToLogic( const Point& rDevicePt,
                                  const MapMode& rMapMode ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    // Ist Default-MapMode, dann bereche nichts
    if ( rMapMode.IsDefault() )
        return rDevicePt;

    // MapMode-Aufloesung berechnen und Umrechnen
    ImplMapRes          aMapRes;
    ImplThresholdRes    aThresRes;
    ImplCalcMapResolution( rMapMode, mnDPIX, mnDPIY, aMapRes, aThresRes );

    return Point( ImplPixelToLogic( rDevicePt.X(), mnDPIX,
                                    aMapRes.mnMapScNumX, aMapRes.mnMapScDenomX,
                                    aThresRes.mnThresPixToLogX ) - aMapRes.mnMapOfsX - mnOutOffLogicX,
                  ImplPixelToLogic( rDevicePt.Y(), mnDPIY,
                                    aMapRes.mnMapScNumY, aMapRes.mnMapScDenomY,
                                    aThresRes.mnThresPixToLogY ) - aMapRes.mnMapOfsY - mnOutOffLogicY );
}

// -----------------------------------------------------------------------

Size OutputDevice::PixelToLogic( const Size& rDeviceSize,
                                 const MapMode& rMapMode ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    // Ist Default-MapMode, dann bereche nichts
    if ( rMapMode.IsDefault() )
        return rDeviceSize;

    // MapMode-Aufloesung berechnen und Umrechnen
    ImplMapRes          aMapRes;
    ImplThresholdRes    aThresRes;
    ImplCalcMapResolution( rMapMode, mnDPIX, mnDPIY, aMapRes, aThresRes );

    return Size( ImplPixelToLogic( rDeviceSize.Width(), mnDPIX,
                                   aMapRes.mnMapScNumX, aMapRes.mnMapScDenomX,
                                   aThresRes.mnThresPixToLogX ),
                 ImplPixelToLogic( rDeviceSize.Height(), mnDPIY,
                                   aMapRes.mnMapScNumY, aMapRes.mnMapScDenomY,
                                   aThresRes.mnThresPixToLogY ) );
}

// -----------------------------------------------------------------------

Rectangle OutputDevice::PixelToLogic( const Rectangle& rDeviceRect,
                                      const MapMode& rMapMode ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );

    // Ist Default-MapMode, dann bereche nichts
    if ( rMapMode.IsDefault() || rDeviceRect.IsEmpty() )
        return rDeviceRect;

    // MapMode-Aufloesung berechnen und Umrechnen
    ImplMapRes          aMapRes;
    ImplThresholdRes    aThresRes;
    ImplCalcMapResolution( rMapMode, mnDPIX, mnDPIY, aMapRes, aThresRes );

    return Rectangle( ImplPixelToLogic( rDeviceRect.Left(), mnDPIX,
                                        aMapRes.mnMapScNumX, aMapRes.mnMapScDenomX,
                                        aThresRes.mnThresPixToLogX ) - aMapRes.mnMapOfsX - mnOutOffLogicX,
                      ImplPixelToLogic( rDeviceRect.Top(), mnDPIY,
                                        aMapRes.mnMapScNumY, aMapRes.mnMapScDenomY,
                                        aThresRes.mnThresPixToLogY ) - aMapRes.mnMapOfsY - mnOutOffLogicY,
                      ImplPixelToLogic( rDeviceRect.Right(), mnDPIX,
                                        aMapRes.mnMapScNumX, aMapRes.mnMapScDenomX,
                                        aThresRes.mnThresPixToLogX ) - aMapRes.mnMapOfsX - mnOutOffLogicX,
                      ImplPixelToLogic( rDeviceRect.Bottom(), mnDPIY,
                                        aMapRes.mnMapScNumY, aMapRes.mnMapScDenomY,
                                        aThresRes.mnThresPixToLogY ) - aMapRes.mnMapOfsY - mnOutOffLogicY );
}

// -----------------------------------------------------------------------

Polygon OutputDevice::PixelToLogic( const Polygon& rDevicePoly,
                                    const MapMode& rMapMode ) const
{
    DBG_CHKTHIS( OutputDevice, ImplDbgCheckOutputDevice );
    DBG_CHKOBJ( &rDevicePoly, Polygon, NULL );

    // Ist Default-MapMode, dann bereche nichts
    if ( rMapMode.IsDefault() )
        return rDevicePoly;

    // MapMode-Aufloesung berechnen und Umrechnen
    ImplMapRes          aMapRes;
    ImplThresholdRes    aThresRes;
    ImplCalcMapResolution( rMapMode, mnDPIX, mnDPIY, aMapRes, aThresRes );

    sal_uInt16  i;
    sal_uInt16  nPoints = rDevicePoly.GetSize();
    Polygon aPoly( rDevicePoly );

    // Pointer auf das Point-Array holen (Daten werden kopiert)
    const Point* pPointAry = aPoly.GetConstPointAry();

    for ( i = 0; i < nPoints; i++ )
    {
        const Point* pPt = &(pPointAry[i]);
        Point aPt;
        aPt.X() = ImplPixelToLogic( pPt->X(), mnDPIX,
                                    aMapRes.mnMapScNumX, aMapRes.mnMapScDenomX,
                                    aThresRes.mnThresPixToLogX ) - aMapRes.mnMapOfsX - mnOutOffLogicX;
        aPt.Y() = ImplPixelToLogic( pPt->Y(), mnDPIY,
                                    aMapRes.mnMapScNumY, aMapRes.mnMapScDenomY,
                                    aThresRes.mnThresPixToLogY ) - aMapRes.mnMapOfsY - mnOutOffLogicY;
        aPoly[i] = aPt;
    }

    return aPoly;
}

// -----------------------------------------------------------------------

basegfx::B2DPolygon OutputDevice::PixelToLogic( const basegfx::B2DPolygon& rPixelPoly,
                                                const MapMode& rMapMode ) const
{
    basegfx::B2DPolygon aTransformedPoly = rPixelPoly;
    const ::basegfx::B2DHomMatrix& rTransformationMatrix = GetInverseViewTransformation( rMapMode );
    aTransformedPoly.transform( rTransformationMatrix );
    return aTransformedPoly;
}

// -----------------------------------------------------------------------

basegfx::B2DPolyPolygon OutputDevice::PixelToLogic( const basegfx::B2DPolyPolygon& rPixelPolyPoly,
                                                    const MapMode& rMapMode ) const
{
    basegfx::B2DPolyPolygon aTransformedPoly = rPixelPolyPoly;
    const ::basegfx::B2DHomMatrix& rTransformationMatrix = GetInverseViewTransformation( rMapMode );
    aTransformedPoly.transform( rTransformationMatrix );
    return aTransformedPoly;
}

// -----------------------------------------------------------------------

#define ENTER0( rSource, pMapModeSource, pMapModeDest )                 \
    if ( !pMapModeSource )                                              \
        pMapModeSource = &maMapMode;                                    \
    if ( !pMapModeDest )                                                \
        pMapModeDest = &maMapMode;                                      \
    if ( *pMapModeSource == *pMapModeDest )                             \
        return rSource

// -----------------------------------------------------------------------

#define ENTER1( rSource, pMapModeSource, pMapModeDest )                 \
    ENTER0( rSource, pMapModeSource, pMapModeDest );                    \
                                                                        \
    ImplMapRes aMapResSource;                                           \
    ImplMapRes aMapResDest;                                             \
                                                                        \
    if ( !mbMap || pMapModeSource != &maMapMode )                       \
    {                                                                   \
        if ( pMapModeSource->GetMapUnit() == MAP_RELATIVE )             \
            aMapResSource = maMapRes;                                   \
        ImplCalcMapResolution( *pMapModeSource,                         \
                               mnDPIX, mnDPIY, aMapResSource );         \
    }                                                                   \
    else                                                                \
        aMapResSource = maMapRes;                                       \
    if ( !mbMap || pMapModeDest != &maMapMode )                         \
    {                                                                   \
        if ( pMapModeDest->GetMapUnit() == MAP_RELATIVE )               \
            aMapResDest = maMapRes;                                     \
        ImplCalcMapResolution( *pMapModeDest,                           \
                               mnDPIX, mnDPIY, aMapResDest );           \
    }                                                                   \
    else                                                                \
        aMapResDest = maMapRes

// -----------------------------------------------------------------------

#define ENTER2( eUnitSource, eUnitDest )                                \
    DBG_ASSERT( eUnitSource != MAP_SYSFONT                              \
                && eUnitSource != MAP_APPFONT                           \
                && eUnitSource != MAP_RELATIVE,                         \
                "Source MapUnit nicht erlaubt" );                       \
    DBG_ASSERT( eUnitDest != MAP_SYSFONT                                \
                && eUnitDest != MAP_APPFONT                             \
                && eUnitDest != MAP_RELATIVE,                           \
                "Destination MapUnit nicht erlaubt" );                  \
    DBG_ASSERTWARNING( eUnitSource != MAP_PIXEL,                        \
                       "MAP_PIXEL mit 72dpi angenaehert" );             \
    DBG_ASSERTWARNING( eUnitDest != MAP_PIXEL,                          \
                       "MAP_PIXEL mit 72dpi angenaehert" )

// -----------------------------------------------------------------------

#define ENTER3( eUnitSource, eUnitDest )                                \
    long nNumerator      = 1;       \
    long nDenominator    = 1;       \
    DBG_ASSERT( eUnitSource < s_ImplArySize, "Invalid source map unit");    \
    DBG_ASSERT( eUnitDest < s_ImplArySize, "Invalid destination map unit"); \
    if( (eUnitSource < s_ImplArySize) && (eUnitDest < s_ImplArySize) )  \
    {   \
        nNumerator   = aImplNumeratorAry[eUnitSource] *             \
                           aImplDenominatorAry[eUnitDest];              \
        nDenominator     = aImplNumeratorAry[eUnitDest] *               \
                           aImplDenominatorAry[eUnitSource];            \
    } \
    if ( eUnitSource == MAP_PIXEL )                                     \
        nDenominator *= 72;                                             \
    else if( eUnitDest == MAP_PIXEL )                                   \
        nNumerator *= 72

// -----------------------------------------------------------------------

#define ENTER4( rMapModeSource, rMapModeDest )                          \
    ImplMapRes aMapResSource;                                           \
    ImplMapRes aMapResDest;                                             \
                                                                        \
    ImplCalcMapResolution( rMapModeSource, 72, 72, aMapResSource );     \
    ImplCalcMapResolution( rMapModeDest, 72, 72, aMapResDest )

// -----------------------------------------------------------------------

// return (n1 * n2 * n3) / (n4 * n5)
static long fn5( const long n1,
                 const long n2,
                 const long n3,
                 const long n4,
                 const long n5 )
{
    if ( n1 == 0 || n2 == 0 || n3 == 0 || n4 == 0 || n5 == 0 )
        return 0;
    if ( LONG_MAX / Abs(n2) < Abs(n3) )
    {
        // a6 wird "ubersprungen
        BigInt a7 = n2;
        a7 *= n3;
        a7 *= n1;

        if ( LONG_MAX / Abs(n4) < Abs(n5) )
        {
            BigInt a8 = n4;
            a8 *= n5;

            BigInt a9 = a8;
            a9 /= 2;
            if ( a7.IsNeg() )
                a7 -= a9;
            else
                a7 += a9;

            a7 /= a8;
        } // of if
        else
        {
            long n8 = n4 * n5;

            if ( a7.IsNeg() )
                a7 -= n8 / 2;
            else
                a7 += n8 / 2;

            a7 /= n8;
        } // of else
        return (long)a7;
    } // of if
    else
    {
        long n6 = n2 * n3;

        if ( LONG_MAX / Abs(n1) < Abs(n6) )
        {
            BigInt a7 = n1;
            a7 *= n6;

            if ( LONG_MAX / Abs(n4) < Abs(n5) )
            {
                BigInt a8 = n4;
                a8 *= n5;

                BigInt a9 = a8;
                a9 /= 2;
                if ( a7.IsNeg() )
                    a7 -= a9;
                else
                    a7 += a9;

                a7 /= a8;
            } // of if
            else
            {
                long n8 = n4 * n5;

                if ( a7.IsNeg() )
                    a7 -= n8 / 2;
                else
                    a7 += n8 / 2;

                a7 /= n8;
            } // of else
            return (long)a7;
        } // of if
        else
        {
            long n7 = n1 * n6;

            if ( LONG_MAX / Abs(n4) < Abs(n5) )
            {
                BigInt a7 = n7;
                BigInt a8 = n4;
                a8 *= n5;

                BigInt a9 = a8;
                a9 /= 2;
                if ( a7.IsNeg() )
                    a7 -= a9;
                else
                    a7 += a9;

                a7 /= a8;
                return (long)a7;
            } // of if
            else
            {
                const long n8 = n4 * n5;
                const long n8_2 = n8 / 2;

                if( n7 < 0 )
                {
                    if( ( n7 - LONG_MIN ) >= n8_2 )
                        n7 -= n8_2;
                }
                else if( ( LONG_MAX - n7 ) >= n8_2 )
                    n7 += n8_2;

                return n7 / n8;
            } // of else
        } // of else
    } // of else
}

// -----------------------------------------------------------------------

// return (n1 * n2) / n3
static long fn3( const long n1, const long n2, const long n3 )
{
    if ( n1 == 0 || n2 == 0 || n3 == 0 )
        return 0;
    if ( LONG_MAX / Abs(n1) < Abs(n2) )
    {
        BigInt a4 = n1;
        a4 *= n2;

        if ( a4.IsNeg() )
            a4 -= n3 / 2;
        else
            a4 += n3 / 2;

        a4 /= n3;
        return (long)a4;
    } // of if
    else
    {
        long        n4 = n1 * n2;
        const long  n3_2 = n3 / 2;

        if( n4 < 0 )
        {
            if( ( n4 - LONG_MIN ) >= n3_2 )
                n4 -= n3_2;
        }
        else if( ( LONG_MAX - n4 ) >= n3_2 )
            n4 += n3_2;

        return n4 / n3;
    } // of else
}

// -----------------------------------------------------------------------

Point OutputDevice::LogicToLogic( const Point& rPtSource,
                                  const MapMode* pMapModeSource,
                                  const MapMode* pMapModeDest ) const
{
    ENTER1( rPtSource, pMapModeSource, pMapModeDest );

    return Point( fn5( rPtSource.X() + aMapResSource.mnMapOfsX,
                       aMapResSource.mnMapScNumX, aMapResDest.mnMapScDenomX,
                       aMapResSource.mnMapScDenomX, aMapResDest.mnMapScNumX ) -
                  aMapResDest.mnMapOfsX,
                  fn5( rPtSource.Y() + aMapResSource.mnMapOfsY,
                       aMapResSource.mnMapScNumY, aMapResDest.mnMapScDenomY,
                       aMapResSource.mnMapScDenomY, aMapResDest.mnMapScNumY ) -
                  aMapResDest.mnMapOfsY );
}

// -----------------------------------------------------------------------

Size OutputDevice::LogicToLogic( const Size& rSzSource,
                                 const MapMode* pMapModeSource,
                                 const MapMode* pMapModeDest ) const
{
    ENTER1( rSzSource, pMapModeSource, pMapModeDest );

    return Size( fn5( rSzSource.Width(),
                      aMapResSource.mnMapScNumX, aMapResDest.mnMapScDenomX,
                      aMapResSource.mnMapScDenomX, aMapResDest.mnMapScNumX ),
                 fn5( rSzSource.Height(),
                      aMapResSource.mnMapScNumY, aMapResDest.mnMapScDenomY,
                      aMapResSource.mnMapScDenomY, aMapResDest.mnMapScNumY ) );
}

// -----------------------------------------------------------------------

Rectangle OutputDevice::LogicToLogic( const Rectangle& rRectSource,
                                      const MapMode* pMapModeSource,
                                      const MapMode* pMapModeDest ) const
{
    ENTER1( rRectSource, pMapModeSource, pMapModeDest );

    return Rectangle( fn5( rRectSource.Left() + aMapResSource.mnMapOfsX,
                           aMapResSource.mnMapScNumX, aMapResDest.mnMapScDenomX,
                           aMapResSource.mnMapScDenomX, aMapResDest.mnMapScNumX ) -
                      aMapResDest.mnMapOfsX,
                      fn5( rRectSource.Top() + aMapResSource.mnMapOfsY,
                           aMapResSource.mnMapScNumY, aMapResDest.mnMapScDenomY,
                           aMapResSource.mnMapScDenomY, aMapResDest.mnMapScNumY ) -
                      aMapResDest.mnMapOfsY,
                      fn5( rRectSource.Right() + aMapResSource.mnMapOfsX,
                           aMapResSource.mnMapScNumX, aMapResDest.mnMapScDenomX,
                           aMapResSource.mnMapScDenomX, aMapResDest.mnMapScNumX ) -
                      aMapResDest.mnMapOfsX,
                      fn5( rRectSource.Bottom() + aMapResSource.mnMapOfsY,
                           aMapResSource.mnMapScNumY, aMapResDest.mnMapScDenomY,
                           aMapResSource.mnMapScDenomY, aMapResDest.mnMapScNumY ) -
                      aMapResDest.mnMapOfsY );
}

// -----------------------------------------------------------------------

Point OutputDevice::LogicToLogic( const Point& rPtSource,
                                  const MapMode& rMapModeSource,
                                  const MapMode& rMapModeDest )
{
    if ( rMapModeSource == rMapModeDest )
        return rPtSource;

    MapUnit eUnitSource = rMapModeSource.GetMapUnit();
    MapUnit eUnitDest   = rMapModeDest.GetMapUnit();
    ENTER2( eUnitSource, eUnitDest );

    if ( rMapModeSource.mpImplMapMode->mbSimple &&
         rMapModeDest.mpImplMapMode->mbSimple )
    {
        ENTER3( eUnitSource, eUnitDest );

        return Point( fn3( rPtSource.X(), nNumerator, nDenominator ),
                      fn3( rPtSource.Y(), nNumerator, nDenominator ) );
    }
    else
    {
        ENTER4( rMapModeSource, rMapModeDest );

        return Point( fn5( rPtSource.X() + aMapResSource.mnMapOfsX,
                           aMapResSource.mnMapScNumX, aMapResDest.mnMapScDenomX,
                           aMapResSource.mnMapScDenomX, aMapResDest.mnMapScNumX ) -
                      aMapResDest.mnMapOfsX,
                      fn5( rPtSource.Y() + aMapResSource.mnMapOfsY,
                           aMapResSource.mnMapScNumY, aMapResDest.mnMapScDenomY,
                           aMapResSource.mnMapScDenomY, aMapResDest.mnMapScNumY ) -
                      aMapResDest.mnMapOfsY );
    }
}

// -----------------------------------------------------------------------

Size OutputDevice::LogicToLogic( const Size& rSzSource,
                                 const MapMode& rMapModeSource,
                                 const MapMode& rMapModeDest )
{
    if ( rMapModeSource == rMapModeDest )
        return rSzSource;

    MapUnit eUnitSource = rMapModeSource.GetMapUnit();
    MapUnit eUnitDest   = rMapModeDest.GetMapUnit();
    ENTER2( eUnitSource, eUnitDest );

    if ( rMapModeSource.mpImplMapMode->mbSimple &&
         rMapModeDest.mpImplMapMode->mbSimple )
    {
        ENTER3( eUnitSource, eUnitDest );

        return Size( fn3( rSzSource.Width(),  nNumerator, nDenominator ),
                     fn3( rSzSource.Height(), nNumerator, nDenominator ) );
    }
    else
    {
        ENTER4( rMapModeSource, rMapModeDest );

        return Size( fn5( rSzSource.Width(),
                          aMapResSource.mnMapScNumX, aMapResDest.mnMapScDenomX,
                          aMapResSource.mnMapScDenomX, aMapResDest.mnMapScNumX ),
                     fn5( rSzSource.Height(),
                          aMapResSource.mnMapScNumY, aMapResDest.mnMapScDenomY,
                          aMapResSource.mnMapScDenomY, aMapResDest.mnMapScNumY ) );
    }
}

// -----------------------------------------------------------------------

basegfx::B2DPolygon OutputDevice::LogicToLogic( const basegfx::B2DPolygon& rPolySource,
                                                const MapMode& rMapModeSource,
                                                const MapMode& rMapModeDest )
{
    if ( rMapModeSource == rMapModeDest )
        return rPolySource;

    MapUnit eUnitSource = rMapModeSource.GetMapUnit();
    MapUnit eUnitDest   = rMapModeDest.GetMapUnit();
    ENTER2( eUnitSource, eUnitDest );

    basegfx::B2DHomMatrix aTransform;

    if ( rMapModeSource.mpImplMapMode->mbSimple &&
         rMapModeDest.mpImplMapMode->mbSimple )
    {
        ENTER3( eUnitSource, eUnitDest );

        const double fScaleFactor((double)nNumerator / (double)nDenominator);
        aTransform.set(0, 0, fScaleFactor);
        aTransform.set(1, 1, fScaleFactor);
    }
    else
    {
        ENTER4( rMapModeSource, rMapModeDest );

        const double fScaleFactorX(  (double(aMapResSource.mnMapScNumX) *  double(aMapResDest.mnMapScDenomX))
                                   / (double(aMapResSource.mnMapScDenomX) * double(aMapResDest.mnMapScNumX)) );
        const double fScaleFactorY(  (double(aMapResSource.mnMapScNumY) *  double(aMapResDest.mnMapScDenomY))
                                   / (double(aMapResSource.mnMapScDenomY) * double(aMapResDest.mnMapScNumY)) );
        const double fZeroPointX(double(aMapResSource.mnMapOfsX) * fScaleFactorX - double(aMapResDest.mnMapOfsX));
        const double fZeroPointY(double(aMapResSource.mnMapOfsY) * fScaleFactorY - double(aMapResDest.mnMapOfsY));

        aTransform.set(0, 0, fScaleFactorX);
        aTransform.set(1, 1, fScaleFactorY);
        aTransform.set(0, 2, fZeroPointX);
        aTransform.set(1, 2, fZeroPointY);
    }
    basegfx::B2DPolygon aPoly( rPolySource );
    aPoly.transform( aTransform );
    return aPoly;
}

// -----------------------------------------------------------------------

Rectangle OutputDevice::LogicToLogic( const Rectangle& rRectSource,
                                      const MapMode& rMapModeSource,
                                      const MapMode& rMapModeDest )
{
    if ( rMapModeSource == rMapModeDest )
        return rRectSource;

    MapUnit eUnitSource = rMapModeSource.GetMapUnit();
    MapUnit eUnitDest   = rMapModeDest.GetMapUnit();
    ENTER2( eUnitSource, eUnitDest );

    if ( rMapModeSource.mpImplMapMode->mbSimple &&
         rMapModeDest.mpImplMapMode->mbSimple )
    {
        ENTER3( eUnitSource, eUnitDest );

        return Rectangle( fn3( rRectSource.Left(), nNumerator, nDenominator ),
                          fn3( rRectSource.Top(), nNumerator, nDenominator ),
                          fn3( rRectSource.Right(), nNumerator, nDenominator ),
                          fn3( rRectSource.Bottom(), nNumerator, nDenominator ) );
    }
    else
    {
        ENTER4( rMapModeSource, rMapModeDest );

        return Rectangle( fn5( rRectSource.Left() + aMapResSource.mnMapOfsX,
                               aMapResSource.mnMapScNumX, aMapResDest.mnMapScDenomX,
                               aMapResSource.mnMapScDenomX, aMapResDest.mnMapScNumX ) -
                          aMapResDest.mnMapOfsX,
                          fn5( rRectSource.Top() + aMapResSource.mnMapOfsY,
                               aMapResSource.mnMapScNumY, aMapResDest.mnMapScDenomY,
                               aMapResSource.mnMapScDenomY, aMapResDest.mnMapScNumY ) -
                          aMapResDest.mnMapOfsY,
                          fn5( rRectSource.Right() + aMapResSource.mnMapOfsX,
                               aMapResSource.mnMapScNumX, aMapResDest.mnMapScDenomX,
                               aMapResSource.mnMapScDenomX, aMapResDest.mnMapScNumX ) -
                          aMapResDest.mnMapOfsX,
                          fn5( rRectSource.Bottom() + aMapResSource.mnMapOfsY,
                               aMapResSource.mnMapScNumY, aMapResDest.mnMapScDenomY,
                               aMapResSource.mnMapScDenomY, aMapResDest.mnMapScNumY ) -
                          aMapResDest.mnMapOfsY );
    }
}

// -----------------------------------------------------------------------

long OutputDevice::LogicToLogic( long nLongSource,
                                 MapUnit eUnitSource, MapUnit eUnitDest )
{
    if ( eUnitSource == eUnitDest )
        return nLongSource;

    ENTER2( eUnitSource, eUnitDest );
    ENTER3( eUnitSource, eUnitDest );

    return fn3( nLongSource, nNumerator, nDenominator );
}

// -----------------------------------------------------------------------

void OutputDevice::SetPixelOffset( const Size& rOffset )
{
    mnOutOffOrigX  = rOffset.Width();
    mnOutOffOrigY  = rOffset.Height();

    mnOutOffLogicX = ImplPixelToLogic( mnOutOffOrigX, mnDPIX,
                                       maMapRes.mnMapScNumX, maMapRes.mnMapScDenomX,
                                       maThresRes.mnThresPixToLogX );
    mnOutOffLogicY = ImplPixelToLogic( mnOutOffOrigY, mnDPIY,
                                       maMapRes.mnMapScNumY, maMapRes.mnMapScDenomY,
                                       maThresRes.mnThresPixToLogY );

    if( mpAlphaVDev )
        mpAlphaVDev->SetPixelOffset( rOffset );
}

// -----------------------------------------------------------------------

Size OutputDevice::GetPixelOffset() const
{
    return Size(mnOutOffOrigX, mnOutOffOrigY);
}

// -----------------------------------------------------------------------

long Window::ImplLogicUnitToPixelX( long nX, MapUnit eUnit )
{
    if ( eUnit != MAP_PIXEL )
    {
        ImplFrameData* pFrameData = mpWindowImpl->mpFrameData;

        // Map-Einheit verschieden, dann neu berechnen
        if ( pFrameData->meMapUnit != eUnit )
        {
            pFrameData->meMapUnit = eUnit;
            ImplCalcMapResolution( MapMode( eUnit ), mnDPIX, mnDPIY,
                                   pFrameData->maMapUnitRes );
        }

        // Es wird kein BigInt gebraucht, da diese Funktion nur zur Umrechnung
        // von Fensterposition benutzt wird
        nX  = nX * mnDPIX * pFrameData->maMapUnitRes.mnMapScNumX;
        nX += nX >= 0 ?  (pFrameData->maMapUnitRes.mnMapScDenomX/2) :
                        -((pFrameData->maMapUnitRes.mnMapScDenomX-1)/2);
        nX /= pFrameData->maMapUnitRes.mnMapScDenomX;
    }

    return nX;
}

// -----------------------------------------------------------------------

long Window::ImplLogicUnitToPixelY( long nY, MapUnit eUnit )
{
    if ( eUnit != MAP_PIXEL )
    {
        ImplFrameData* pFrameData = mpWindowImpl->mpFrameData;

        // Map-Einheit verschieden, dann neu berechnen
        if ( pFrameData->meMapUnit != eUnit )
        {
            pFrameData->meMapUnit = eUnit;
            ImplCalcMapResolution( MapMode( eUnit ), mnDPIX, mnDPIY,
                                   pFrameData->maMapUnitRes );
        }

        // Es wird kein BigInt gebraucht, da diese Funktion nur zur Umrechnung
        // von Fensterposition benutzt wird
        nY  = nY * mnDPIY * pFrameData->maMapUnitRes.mnMapScNumY;
        nY += nY >= 0 ?  (pFrameData->maMapUnitRes.mnMapScDenomY/2) :
                        -((pFrameData->maMapUnitRes.mnMapScDenomY-1)/2);
        nY /= pFrameData->maMapUnitRes.mnMapScDenomY;
    }

    return nY;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

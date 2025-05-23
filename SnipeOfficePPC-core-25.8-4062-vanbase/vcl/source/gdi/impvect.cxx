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


#include <stdlib.h>
#include <vcl/bmpacc.hxx>
#include <tools/poly.hxx>
#include <vcl/gdimtf.hxx>
#include <vcl/metaact.hxx>
#include <vcl/svapp.hxx>
#include <vcl/wrkwin.hxx>
#include <vcl/virdev.hxx>
#include <impvect.hxx>

// -----------
// - Defines -
// -----------

#define VECT_POLY_MAX 8192

// -----------------------------------------------------------------------------

#define VECT_FREE_INDEX 0
#define VECT_CONT_INDEX 1
#define VECT_DONE_INDEX 2

// -----------------------------------------------------------------------------

#define VECT_POLY_INLINE_INNER  1UL
#define VECT_POLY_INLINE_OUTER  2UL
#define VECT_POLY_OUTLINE_INNER 4UL
#define VECT_POLY_OUTLINE_OUTER 8UL

// -----------------------------------------------------------------------------

#define VECT_MAP( _def_pIn, _def_pOut, _def_nVal )  _def_pOut[_def_nVal]=(_def_pIn[_def_nVal]=((_def_nVal)*4L)+1L)+5L;
#define BACK_MAP( _def_nVal )                       ((((_def_nVal)+2)>>2)-1)
#define VECT_PROGRESS( _def_pProgress, _def_nVal )  if(_def_pProgress&&_def_pProgress->IsSet())(_def_pProgress->Call((void*)_def_nVal));

// -----------
// - statics -
// -----------

struct ChainMove { long nDX; long nDY; };

static ChainMove aImplMove[ 8 ] =   {
                                        { 1L, 0L },
                                        { 0L, -1L },
                                        { -1L, 0L },
                                        { 0L, 1L },
                                        { 1L, -1L },
                                        { -1, -1L },
                                        { -1L, 1L },
                                        { 1L, 1L }
                                    };

static ChainMove aImplMoveInner[ 8 ] =  {
                                            { 0L, 1L },
                                            { 1L, 0L },
                                            { 0L, -1L },
                                            { -1L, 0L },
                                            { 0L, 1L },
                                            { 1L, 0L },
                                            { 0L, -1L },
                                            { -1L, 0L }
                                        };

static ChainMove aImplMoveOuter[ 8 ] =  {
                                            { 0L, -1L },
                                            { -1L, 0L },
                                            { 0L, 1L },
                                            { 1L, 0L },
                                            { -1L, 0L },
                                            { 0L, 1L },
                                            { 1L, 0L },
                                            { 0L, -1L }
                                        };

// ----------------
// - ImplColorSet -
// ----------------

struct ImplColorSet
{
    BitmapColor maColor;
    sal_uInt16      mnIndex;
    sal_Bool        mbSet;

    sal_Bool        operator<( const ImplColorSet& rSet ) const;
    sal_Bool        operator>( const ImplColorSet& rSet ) const;
};

// ----------------------------------------------------------------------------

inline sal_Bool ImplColorSet::operator<( const ImplColorSet& rSet ) const
{
    return( mbSet && ( !rSet.mbSet || ( maColor.GetLuminance() > rSet.maColor.GetLuminance() ) ) );
}

// ----------------------------------------------------------------------------

inline sal_Bool ImplColorSet::operator>( const ImplColorSet& rSet ) const
{
    return( !mbSet || ( rSet.mbSet && maColor.GetLuminance() < rSet.maColor.GetLuminance() ) );
}

// ----------------------------------------------------------------------------

extern "C" int __LOADONCALLAPI ImplColorSetCmpFnc( const void* p1, const void* p2 )
{
    ImplColorSet*   pSet1 = (ImplColorSet*) p1;
    ImplColorSet*   pSet2 = (ImplColorSet*) p2;
    int             nRet;

    if( pSet1->mbSet && pSet2->mbSet )
    {
        const sal_uInt8 cLum1 = pSet1->maColor.GetLuminance();
        const sal_uInt8 cLum2 = pSet2->maColor.GetLuminance();
        nRet = ( ( cLum1 > cLum2 ) ? -1 : ( ( cLum1 == cLum2 ) ? 0 : 1 ) );
    }
    else if( pSet1->mbSet )
        nRet = -1;
    else if( pSet2->mbSet )
        nRet = 1;
    else
        nRet = 0;

    return nRet;
}

// ------------------
// - ImplPointArray -
// ------------------

class ImplPointArray
{
    Point*              mpArray;
    sal_uLong               mnSize;
    sal_uLong               mnRealSize;

public:

                        ImplPointArray();
                        ~ImplPointArray();

    void                ImplSetSize( sal_uLong nSize );

    sal_uLong               ImplGetRealSize() const { return mnRealSize; }
    void                ImplSetRealSize( sal_uLong nRealSize ) { mnRealSize = nRealSize; }

    inline Point&       operator[]( sal_uLong nPos );
    inline const Point& operator[]( sal_uLong nPos ) const;

    void                ImplCreatePoly( Polygon& rPoly ) const;
};

// -----------------------------------------------------------------------------

ImplPointArray::ImplPointArray() :
    mpArray     ( NULL ),
    mnSize      ( 0UL ),
    mnRealSize  ( 0UL )

{
}

// -----------------------------------------------------------------------------

ImplPointArray::~ImplPointArray()
{
    if( mpArray )
        rtl_freeMemory( mpArray );
}

// -----------------------------------------------------------------------------

void ImplPointArray::ImplSetSize( sal_uLong nSize )
{
    const sal_uLong nTotal = nSize * sizeof( Point );

    mnSize = nSize;
    mnRealSize = 0UL;

    if( mpArray )
        rtl_freeMemory( mpArray );

    mpArray = (Point*) rtl_allocateMemory( nTotal );
    memset( (HPBYTE) mpArray, 0, nTotal );
}

// -----------------------------------------------------------------------------

inline Point& ImplPointArray::operator[]( sal_uLong nPos )
{
    DBG_ASSERT( nPos < mnSize, "ImplPointArray::operator[]: nPos out of range!" );
    return mpArray[ nPos ];
}

// -----------------------------------------------------------------------------

inline const Point& ImplPointArray::operator[]( sal_uLong nPos ) const
{
    DBG_ASSERT( nPos < mnSize, "ImplPointArray::operator[]: nPos out of range!" );
    return mpArray[ nPos ];
}

// -----------------------------------------------------------------------------

void ImplPointArray::ImplCreatePoly( Polygon& rPoly ) const
{
    rPoly = Polygon( sal::static_int_cast<sal_uInt16>(mnRealSize), mpArray );
}

// ---------------
// - ImplVectMap -
// ---------------

class ImplVectMap
{
private:

    Scanline        mpBuf;
    Scanline*       mpScan;
    long            mnWidth;
    long            mnHeight;

                    ImplVectMap() {};

public:

                    ImplVectMap( long nWidth, long nHeight );
                    ~ImplVectMap();

    inline long     Width() const { return mnWidth; }
    inline long     Height() const { return mnHeight; }

    inline void     Set( long nY, long nX, sal_uInt8 cVal );
    inline sal_uInt8        Get( long nY, long nX ) const;

    inline sal_Bool     IsFree( long nY, long nX ) const;
    inline sal_Bool     IsCont( long nY, long nX ) const;
    inline sal_Bool     IsDone( long nY, long nX ) const;

};

// -----------------------------------------------------------------------------

ImplVectMap::ImplVectMap( long nWidth, long nHeight ) :
    mnWidth ( nWidth ),
    mnHeight( nHeight )
{
    const long  nWidthAl = ( nWidth >> 2L ) + 1L;
    const long  nSize = nWidthAl * nHeight;
    Scanline    pTmp = mpBuf = (Scanline) rtl_allocateMemory( nSize );

    memset( mpBuf, 0, nSize );
    mpScan = (Scanline*) rtl_allocateMemory( nHeight * sizeof( Scanline ) );

    for( long nY = 0L; nY < nHeight; pTmp += nWidthAl )
        mpScan[ nY++ ] = pTmp;
}

// -----------------------------------------------------------------------------


ImplVectMap::~ImplVectMap()
{
    rtl_freeMemory( mpBuf );
    rtl_freeMemory( mpScan );
}

// -----------------------------------------------------------------------------

inline void ImplVectMap::Set( long nY, long nX, sal_uInt8 cVal )
{
    const sal_uInt8 cShift = sal::static_int_cast<sal_uInt8>(6 - ( ( nX & 3 ) << 1 ));
    ( ( mpScan[ nY ][ nX >> 2 ] ) &= ~( 3 << cShift ) ) |= ( cVal << cShift );
}

// -----------------------------------------------------------------------------

inline sal_uInt8    ImplVectMap::Get( long nY, long nX ) const
{
    return sal::static_int_cast<sal_uInt8>( ( ( mpScan[ nY ][ nX >> 2 ] ) >> ( 6 - ( ( nX & 3 ) << 1 ) ) ) & 3 );
}

// -----------------------------------------------------------------------------

inline sal_Bool ImplVectMap::IsFree( long nY, long nX ) const
{
    return( VECT_FREE_INDEX == Get( nY, nX ) );
}

// -----------------------------------------------------------------------------

inline sal_Bool ImplVectMap::IsCont( long nY, long nX ) const
{
    return( VECT_CONT_INDEX == Get( nY, nX ) );
}

// -----------------------------------------------------------------------------

inline sal_Bool ImplVectMap::IsDone( long nY, long nX ) const
{
    return( VECT_DONE_INDEX == Get( nY, nX ) );
}

// -------------
// - ImplChain -
// -------------

class ImplChain
{
private:

    Polygon         maPoly;
    Point           maStartPt;
    sal_uLong           mnArraySize;
    sal_uLong           mnCount;
    long            mnResize;
    sal_uInt8*          mpCodes;

    void            ImplGetSpace();

    void            ImplCreate();
    void            ImplCreateInner();
    void            ImplCreateOuter();
    void            ImplPostProcess( const ImplPointArray& rArr );

public:

                    ImplChain( sal_uLong nInitCount = 1024UL, long nResize = -1L );
                    ~ImplChain();

    void            ImplBeginAdd( const Point& rStartPt );
    inline void     ImplAdd( sal_uInt8 nCode );
    void            ImplEndAdd( sal_uLong nTypeFlag );

    const Polygon&  ImplGetPoly() const { return maPoly; }
};

// -----------------------------------------------------------------------------

ImplChain::ImplChain( sal_uLong nInitCount, long nResize ) :
    mnArraySize ( nInitCount ),
    mnCount     ( 0UL ),
    mnResize    ( nResize )
{
    DBG_ASSERT( nInitCount && nResize, "ImplChain::ImplChain(): invalid parameters!" );
    mpCodes = new sal_uInt8[ mnArraySize ];
}

// -----------------------------------------------------------------------------

ImplChain::~ImplChain()
{
    delete[] mpCodes;
}

// -----------------------------------------------------------------------------

void ImplChain::ImplGetSpace()
{
    const sal_uLong nOldArraySize = mnArraySize;
    sal_uInt8*      pNewCodes;

    mnArraySize = ( mnResize < 0L ) ? ( mnArraySize << 1UL ) : ( mnArraySize + (sal_uLong) mnResize );
    pNewCodes = new sal_uInt8[ mnArraySize ];
    memcpy( pNewCodes, mpCodes, nOldArraySize );
    delete[] mpCodes;
    mpCodes = pNewCodes;
}

// -----------------------------------------------------------------------------

void ImplChain::ImplBeginAdd( const Point& rStartPt )
{
    maPoly = Polygon();
    maStartPt = rStartPt;
    mnCount = 0UL;
}

// -----------------------------------------------------------------------------

inline void ImplChain::ImplAdd( sal_uInt8 nCode )
{
    if( mnCount == mnArraySize )
        ImplGetSpace();

    mpCodes[ mnCount++ ] = nCode;
}

// -----------------------------------------------------------------------------

void ImplChain::ImplEndAdd( sal_uLong nFlag )
{
    if( mnCount )
    {
        ImplPointArray aArr;

        if( nFlag & VECT_POLY_INLINE_INNER )
        {
            long nFirstX, nFirstY;
            long nLastX, nLastY;

            nFirstX = nLastX = maStartPt.X();
            nFirstY = nLastY = maStartPt.Y();
            aArr.ImplSetSize( mnCount << 1 );

            sal_uInt16 i, nPolyPos;
            for( i = 0, nPolyPos = 0; i < ( mnCount - 1 ); i++ )
            {
                const sal_uInt8             cMove = mpCodes[ i ];
                const sal_uInt8             cNextMove = mpCodes[ i + 1 ];
                const ChainMove&        rMove = aImplMove[ cMove ];
                const ChainMove&        rMoveInner = aImplMoveInner[ cMove ];
//              Point&                  rPt = aArr[ nPolyPos ];
                sal_Bool                    bDone = sal_True;

                nLastX += rMove.nDX;
                nLastY += rMove.nDY;

                if( cMove < 4 )
                {
                    if( ( cMove == 0 && cNextMove == 3 ) ||
                        ( cMove == 3 && cNextMove == 2 ) ||
                        ( cMove == 2 && cNextMove == 1 ) ||
                        ( cMove == 1 && cNextMove == 0 ) )
                    {
                    }
                    else if( cMove == 2 && cNextMove == 3 )
                    {
                        aArr[ nPolyPos ].X() = nLastX;
                        aArr[ nPolyPos++ ].Y() = nLastY - 1;

                        aArr[ nPolyPos ].X() = nLastX - 1;
                        aArr[ nPolyPos++ ].Y() = nLastY - 1;

                        aArr[ nPolyPos ].X() = nLastX - 1;
                        aArr[ nPolyPos++ ].Y() = nLastY;
                    }
                    else if( cMove == 3 && cNextMove == 0 )
                    {
                        aArr[ nPolyPos ].X() = nLastX - 1;
                        aArr[ nPolyPos++ ].Y() = nLastY;

                        aArr[ nPolyPos ].X() = nLastX - 1;
                        aArr[ nPolyPos++ ].Y() = nLastY + 1;

                        aArr[ nPolyPos ].X() = nLastX;
                        aArr[ nPolyPos++ ].Y() = nLastY + 1;
                    }
                    else if( cMove == 0 && cNextMove == 1 )
                    {
                        aArr[ nPolyPos ].X() = nLastX;
                        aArr[ nPolyPos++ ].Y() = nLastY + 1;

                        aArr[ nPolyPos ].X() = nLastX + 1;
                        aArr[ nPolyPos++ ].Y() = nLastY + 1;

                        aArr[ nPolyPos ].X() = nLastX + 1;
                        aArr[ nPolyPos++ ].Y() = nLastY;
                    }
                    else if( cMove == 1 && cNextMove == 2 )
                    {
                        aArr[ nPolyPos ].X() = nLastX + 1;
                        aArr[ nPolyPos++ ].Y() = nLastY + 1;

                        aArr[ nPolyPos ].X() = nLastX + 1;
                        aArr[ nPolyPos++ ].Y() = nLastY - 1;

                        aArr[ nPolyPos ].X() = nLastX;
                        aArr[ nPolyPos++ ].Y() = nLastY - 1;
                    }
                    else
                        bDone = sal_False;
                }
                else if( cMove == 7 && cNextMove == 0 )
                {
                    aArr[ nPolyPos ].X() = nLastX - 1;
                    aArr[ nPolyPos++ ].Y() = nLastY;

                    aArr[ nPolyPos ].X() = nLastX;
                    aArr[ nPolyPos++ ].Y() = nLastY + 1;
                }
                else if( cMove == 4 && cNextMove == 1 )
                {
                    aArr[ nPolyPos ].X() = nLastX;
                    aArr[ nPolyPos++ ].Y() = nLastY + 1;

                    aArr[ nPolyPos ].X() = nLastX + 1;
                    aArr[ nPolyPos++ ].Y() = nLastY;
                }
                else
                    bDone = sal_False;

                if( !bDone )
                {
                    aArr[ nPolyPos ].X() = nLastX + rMoveInner.nDX;
                    aArr[ nPolyPos++ ].Y() = nLastY + rMoveInner.nDY;
                }
            }

            aArr[ nPolyPos ].X() = nFirstX + 1L;
            aArr[ nPolyPos++ ].Y() = nFirstY + 1L;
            aArr.ImplSetRealSize( nPolyPos );
        }
        else if( nFlag & VECT_POLY_INLINE_OUTER )
        {
            long nFirstX, nFirstY;
            long nLastX, nLastY;

            nFirstX = nLastX = maStartPt.X();
            nFirstY = nLastY = maStartPt.Y();
            aArr.ImplSetSize( mnCount << 1 );

            sal_uInt16 i, nPolyPos;
            for( i = 0, nPolyPos = 0; i < ( mnCount - 1 ); i++ )
            {
                const sal_uInt8             cMove = mpCodes[ i ];
                const sal_uInt8             cNextMove = mpCodes[ i + 1 ];
                const ChainMove&        rMove = aImplMove[ cMove ];
                const ChainMove&        rMoveOuter = aImplMoveOuter[ cMove ];
//              Point&                  rPt = aArr[ nPolyPos ];
                sal_Bool                    bDone = sal_True;

                nLastX += rMove.nDX;
                nLastY += rMove.nDY;

                if( cMove < 4 )
                {
                    if( ( cMove == 0 && cNextMove == 1 ) ||
                        ( cMove == 1 && cNextMove == 2 ) ||
                        ( cMove == 2 && cNextMove == 3 ) ||
                        ( cMove == 3 && cNextMove == 0 ) )
                    {
                    }
                    else if( cMove == 0 && cNextMove == 3 )
                    {
                        aArr[ nPolyPos ].X() = nLastX;
                        aArr[ nPolyPos++ ].Y() = nLastY - 1;

                        aArr[ nPolyPos ].X() = nLastX + 1;
                        aArr[ nPolyPos++ ].Y() = nLastY - 1;

                        aArr[ nPolyPos ].X() = nLastX + 1;
                        aArr[ nPolyPos++ ].Y() = nLastY;
                    }
                    else if( cMove == 3 && cNextMove == 2 )
                    {
                        aArr[ nPolyPos ].X() = nLastX + 1;
                        aArr[ nPolyPos++ ].Y() = nLastY;

                        aArr[ nPolyPos ].X() = nLastX + 1;
                        aArr[ nPolyPos++ ].Y() = nLastY + 1;

                        aArr[ nPolyPos ].X() = nLastX;
                        aArr[ nPolyPos++ ].Y() = nLastY + 1;
                    }
                    else if( cMove == 2 && cNextMove == 1 )
                    {
                        aArr[ nPolyPos ].X() = nLastX;
                        aArr[ nPolyPos++ ].Y() = nLastY + 1;

                        aArr[ nPolyPos ].X() = nLastX - 1;
                        aArr[ nPolyPos++ ].Y() = nLastY + 1;

                        aArr[ nPolyPos ].X() = nLastX - 1;
                        aArr[ nPolyPos++ ].Y() = nLastY;
                    }
                    else if( cMove == 1 && cNextMove == 0 )
                    {
                        aArr[ nPolyPos ].X() = nLastX - 1;
                        aArr[ nPolyPos++ ].Y() = nLastY;

                        aArr[ nPolyPos ].X() = nLastX - 1;
                        aArr[ nPolyPos++ ].Y() = nLastY - 1;

                        aArr[ nPolyPos ].X() = nLastX;
                        aArr[ nPolyPos++ ].Y() = nLastY - 1;
                    }
                    else
                        bDone = sal_False;
                }
                else if( cMove == 7 && cNextMove == 3 )
                {
                    aArr[ nPolyPos ].X() = nLastX;
                    aArr[ nPolyPos++ ].Y() = nLastY - 1;

                    aArr[ nPolyPos ].X() = nLastX + 1;
                    aArr[ nPolyPos++ ].Y() = nLastY;
                }
                else if( cMove == 6 && cNextMove == 2 )
                {
                    aArr[ nPolyPos ].X() = nLastX + 1;
                    aArr[ nPolyPos++ ].Y() = nLastY;

                    aArr[ nPolyPos ].X() = nLastX;
                    aArr[ nPolyPos++ ].Y() = nLastY + 1;
                }
                else
                    bDone = sal_False;

                if( !bDone )
                {
                    aArr[ nPolyPos ].X() = nLastX + rMoveOuter.nDX;
                    aArr[ nPolyPos++ ].Y() = nLastY + rMoveOuter.nDY;
                }
            }

            aArr[ nPolyPos ].X() = nFirstX - 1L;
            aArr[ nPolyPos++ ].Y() = nFirstY - 1L;
            aArr.ImplSetRealSize( nPolyPos );
        }
        else
        {
            long nLastX = maStartPt.X(), nLastY = maStartPt.Y();

            aArr.ImplSetSize( mnCount + 1 );
            aArr[ 0 ] = Point( nLastX, nLastY );

            for( sal_uLong i = 0; i < mnCount; )
            {
                const ChainMove& rMove = aImplMove[ mpCodes[ i ] ];
                aArr[ ++i ] = Point( nLastX += rMove.nDX, nLastY += rMove.nDY );
            }

            aArr.ImplSetRealSize( mnCount + 1 );
        }

        ImplPostProcess( aArr );
    }
    else
        maPoly.SetSize( 0 );
}

// -----------------------------------------------------------------------------

void ImplChain::ImplPostProcess( const ImplPointArray& rArr )
{
    ImplPointArray  aNewArr1;
    ImplPointArray  aNewArr2;
    Point*          pLast;
    Point*          pLeast;
    sal_uLong           nNewPos;
    sal_uLong           nCount = rArr.ImplGetRealSize();
    sal_uLong           n;

    // pass 1
    aNewArr1.ImplSetSize( nCount );
    pLast = &( aNewArr1[ 0 ] );
    pLast->X() = BACK_MAP( rArr[ 0 ].X() );
    pLast->Y() = BACK_MAP( rArr[ 0 ].Y() );

    for( n = nNewPos = 1; n < nCount; )
    {
        const Point& rPt = rArr[ n++ ];
        const long   nX = BACK_MAP( rPt.X() );
        const long   nY = BACK_MAP( rPt.Y() );

        if( nX != pLast->X() || nY != pLast->Y() )
        {
            pLast = pLeast = &( aNewArr1[ nNewPos++ ] );
            pLeast->X() = nX;
            pLeast->Y() = nY;
        }
    }

    aNewArr1.ImplSetRealSize( nCount = nNewPos );

    // pass 2
    aNewArr2.ImplSetSize( nCount );
    pLast = &( aNewArr2[ 0 ] );
    *pLast = aNewArr1[ 0 ];

    for( n = nNewPos = 1; n < nCount; )
    {
        pLeast = &( aNewArr1[ n++ ] );

        if( pLeast->X() == pLast->X() )
        {
            while( n < nCount && aNewArr1[ n ].X() == pLast->X() )
                pLeast = &( aNewArr1[ n++ ] );
        }
        else if( pLeast->Y() == pLast->Y() )
        {
            while( n < nCount && aNewArr1[ n ].Y() == pLast->Y() )
                pLeast = &( aNewArr1[ n++ ] );
        }

        aNewArr2[ nNewPos++ ] = *( pLast = pLeast );
    }

    aNewArr2.ImplSetRealSize( nNewPos );
    aNewArr2.ImplCreatePoly( maPoly );
}

// ------------------
// - ImplVectorizer -
// ------------------

ImplVectorizer::ImplVectorizer()
{
}

// -----------------------------------------------------------------------------

ImplVectorizer::~ImplVectorizer()
{
}

// -----------------------------------------------------------------------------

sal_Bool ImplVectorizer::ImplVectorize( const Bitmap& rColorBmp, GDIMetaFile& rMtf,
                                    sal_uInt8 cReduce, sal_uLong nFlags, const Link* pProgress )
{
    sal_Bool bRet = sal_False;

    VECT_PROGRESS( pProgress, 0 );

    Bitmap*             pBmp = new Bitmap( rColorBmp );
    BitmapReadAccess*   pRAcc = pBmp->AcquireReadAccess();

    if( pRAcc )
    {
        PolyPolygon         aPolyPoly;
        double              fPercent = 0.0;
        double              fPercentStep_2 = 0.0;
        const long          nWidth = pRAcc->Width();
        const long          nHeight = pRAcc->Height();
        const sal_uInt16        nColorCount = pRAcc->GetPaletteEntryCount();
        sal_uInt16              n;
        ImplColorSet*       pColorSet = (ImplColorSet*) new sal_uInt8[ 256 * sizeof( ImplColorSet ) ];

        memset( pColorSet, 0, 256 * sizeof( ImplColorSet ) );
        rMtf.Clear();

        // get used palette colors and sort them from light to dark colors
        for( n = 0; n < nColorCount; n++ )
        {
            pColorSet[ n ].mnIndex = n;
            pColorSet[ n ].maColor = pRAcc->GetPaletteColor( n );
        }

        for( long nY = 0L; nY < nHeight; nY++ )
            for( long nX = 0L; nX < nWidth; nX++ )
                pColorSet[ pRAcc->GetPixel( nY, nX ).GetIndex() ].mbSet = 1;

        qsort( pColorSet, 256, sizeof( ImplColorSet ), ImplColorSetCmpFnc );

        for( n = 0; n < 256; n++ )
            if( !pColorSet[ n ].mbSet )
                break;

        if( n )
            fPercentStep_2 = 45.0 / n;

        VECT_PROGRESS( pProgress, FRound( fPercent += 10.0 ) );

        for( sal_uInt16 i = 0; i < n; i++ )
        {
            const BitmapColor   aBmpCol( pRAcc->GetPaletteColor( pColorSet[ i ].mnIndex ) );
            const Color         aFindColor( aBmpCol.GetRed(), aBmpCol.GetGreen(), aBmpCol.GetBlue() );
//          const sal_uInt8         cLum = aFindColor.GetLuminance();
            ImplVectMap*        pMap = ImplExpand( pRAcc, aFindColor );

            VECT_PROGRESS( pProgress, FRound( fPercent += fPercentStep_2 ) );

            if( pMap )
            {
                aPolyPoly.Clear();
                ImplCalculate( pMap, aPolyPoly, cReduce, nFlags );
                delete pMap;

                if( aPolyPoly.Count() )
                {
                    ImplLimitPolyPoly( aPolyPoly );

                    if( nFlags & BMP_VECTORIZE_REDUCE_EDGES )
                        aPolyPoly.Optimize( POLY_OPTIMIZE_EDGES );

                    if( aPolyPoly.Count() )
                    {
                        rMtf.AddAction( new MetaLineColorAction( aFindColor, sal_True ) );
                        rMtf.AddAction( new MetaFillColorAction( aFindColor, sal_True ) );
                        rMtf.AddAction( new MetaPolyPolygonAction( aPolyPoly ) );
                    }
                }
            }

            VECT_PROGRESS( pProgress, FRound( fPercent += fPercentStep_2 ) );
        }

        delete[] (sal_uInt8*) pColorSet;

        if( rMtf.GetActionSize() )
        {
            MapMode         aMap( MAP_100TH_MM );
            VirtualDevice   aVDev;
            const Size      aLogSize1( aVDev.PixelToLogic( Size( 1, 1 ), aMap ) );

            rMtf.SetPrefMapMode( aMap );
            rMtf.SetPrefSize( Size( nWidth + 2, nHeight + 2 ) );
            rMtf.Move( 1, 1 );
            rMtf.Scale( aLogSize1.Width(), aLogSize1.Height() );
            bRet = sal_True;
        }
    }

    pBmp->ReleaseAccess( pRAcc );
    delete pBmp;
    VECT_PROGRESS( pProgress, 100 );

    return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool ImplVectorizer::ImplVectorize( const Bitmap& rMonoBmp,
                                    PolyPolygon& rPolyPoly,
                                    sal_uLong nFlags, const Link* pProgress )
{
    Bitmap*             pBmp = new Bitmap( rMonoBmp );
    BitmapReadAccess*   pRAcc;
    ImplVectMap*        pMap;
    sal_Bool                bRet = sal_False;

    VECT_PROGRESS( pProgress, 10 );

    if( pBmp->GetBitCount() > 1 )
        pBmp->Convert( BMP_CONVERSION_1BIT_THRESHOLD );

    VECT_PROGRESS( pProgress, 30 );

    pRAcc = pBmp->AcquireReadAccess();
    pMap = ImplExpand( pRAcc, COL_BLACK );
    pBmp->ReleaseAccess( pRAcc );
    delete pBmp;

    VECT_PROGRESS( pProgress, 60 );

    if( pMap )
    {
        rPolyPoly.Clear();
        ImplCalculate( pMap, rPolyPoly, 0, nFlags );
        delete pMap;
        ImplLimitPolyPoly( rPolyPoly );

        if( nFlags & BMP_VECTORIZE_REDUCE_EDGES )
            rPolyPoly.Optimize( POLY_OPTIMIZE_EDGES );

        // #i14895#:setting the correct direction for polygons
        // that represent holes and non-holes; non-hole polygons
        // need to have a right orientation, holes need to have a
        // left orientation in order to be treated correctly by
        // several external tools like Flash viewers
        sal_Int32   nFirstPoly = -1;
        sal_uInt16  nCurPoly( 0 ), nCount( rPolyPoly.Count() );

        for( ; nCurPoly < nCount; ++nCurPoly )
        {
            const Polygon&      rPoly = rPolyPoly.GetObject( nCurPoly );
            const sal_uInt16    nSize( rPoly.GetSize() );
            sal_uInt16          nDepth( 0 ), i( 0 );
            const bool          bRight( rPoly.IsRightOrientated() );

            for( ; i < nCount; ++i )
                if( ( i != nCurPoly ) && rPolyPoly.GetObject( i ).IsInside( rPoly[ 0 ] ) )
                    ++nDepth;

            const bool bHole( ( nDepth & 0x0001 ) == 1 );

            if( nSize && ( ( !bRight && !bHole ) || ( bRight && bHole ) ) )
            {
                Polygon     aNewPoly( nSize );
                sal_uInt16  nPrim( 0 ), nSec( nSize - 1 );

                if( rPoly.HasFlags() )
                {
                    while( nPrim < nSize )
                    {
                        aNewPoly.SetPoint( rPoly.GetPoint( nSec ), nPrim );
                        aNewPoly.SetFlags( nPrim++, rPoly.GetFlags( nSec-- ) );
                    }
                }
                else
                    while( nPrim < nSize )
                        aNewPoly.SetPoint( rPoly.GetPoint( nSec-- ), nPrim++ );

                rPolyPoly.Replace( aNewPoly, nCurPoly );
            }

            if( ( 0 == nDepth ) && ( -1 == nFirstPoly ) )
                nFirstPoly = nCurPoly;
        }

        // put outmost polygon to the front
        if( nFirstPoly > 0 )
        {
            const Polygon aFirst( rPolyPoly.GetObject( static_cast< sal_uInt16 >( nFirstPoly ) ) );

            rPolyPoly.Remove( static_cast< sal_uInt16 >( nFirstPoly ) );
            rPolyPoly.Insert( aFirst, 0 );
        }

        bRet = sal_True;
    }

    VECT_PROGRESS( pProgress, 100 );

    return bRet;
}

// -----------------------------------------------------------------------------

void ImplVectorizer::ImplLimitPolyPoly( PolyPolygon& rPolyPoly )
{
    if( rPolyPoly.Count() > VECT_POLY_MAX )
    {
        PolyPolygon aNewPolyPoly;
        long        nReduce = 0;
        sal_uInt16      nNewCount;

        do
        {
            aNewPolyPoly.Clear();
            nReduce++;

            for( sal_uInt16 i = 0, nCount = rPolyPoly.Count(); i < nCount; i++ )
            {
                const Rectangle aBound( rPolyPoly[ i ].GetBoundRect() );

                if( aBound.GetWidth() > nReduce && aBound.GetHeight() > nReduce )
                {
                    if( rPolyPoly[ i ].GetSize() )
                        aNewPolyPoly.Insert( rPolyPoly[ i ] );
                }
            }

            nNewCount = aNewPolyPoly.Count();
        }
        while( nNewCount > VECT_POLY_MAX );

        rPolyPoly = aNewPolyPoly;
    }
}

// -----------------------------------------------------------------------------

ImplVectMap* ImplVectorizer::ImplExpand( BitmapReadAccess* pRAcc, const Color& rColor )
{
    ImplVectMap* pMap = NULL;

       if( pRAcc && pRAcc->Width() && pRAcc->Height() )
    {
        const long          nOldWidth = pRAcc->Width();
        const long          nOldHeight = pRAcc->Height();
        const long          nNewWidth = ( nOldWidth << 2L ) + 4L;
        const long          nNewHeight = ( nOldHeight << 2L ) + 4L;
        const BitmapColor   aTest( pRAcc->GetBestMatchingColor( rColor ) );
        long*               pMapIn = new long[ Max( nOldWidth, nOldHeight ) ];
        long*               pMapOut = new long[ Max( nOldWidth, nOldHeight ) ];
        long                nX, nY, nTmpX, nTmpY;

        pMap = new ImplVectMap( nNewWidth, nNewHeight );

        for( nX = 0L; nX < nOldWidth; nX++ )
            VECT_MAP( pMapIn, pMapOut, nX );

        for( nY = 0L, nTmpY = 5L; nY < nOldHeight; nY++, nTmpY += 4L )
        {
            for( nX = 0L; nX < nOldWidth; )
            {
                if( pRAcc->GetPixel( nY, nX ) == aTest )
                {
                    nTmpX = pMapIn[ nX++ ];
                    nTmpY -= 3L;

                    pMap->Set( nTmpY++, nTmpX, VECT_CONT_INDEX );
                    pMap->Set( nTmpY++, nTmpX, VECT_CONT_INDEX );
                    pMap->Set( nTmpY++, nTmpX, VECT_CONT_INDEX );
                    pMap->Set( nTmpY, nTmpX, VECT_CONT_INDEX );

                    while( nX < nOldWidth && pRAcc->GetPixel( nY, nX ) == aTest )
                         nX++;

                    nTmpX = pMapOut[ nX - 1L ];
                    nTmpY -= 3L;

                    pMap->Set( nTmpY++, nTmpX, VECT_CONT_INDEX );
                    pMap->Set( nTmpY++, nTmpX, VECT_CONT_INDEX );
                    pMap->Set( nTmpY++, nTmpX, VECT_CONT_INDEX );
                    pMap->Set( nTmpY, nTmpX, VECT_CONT_INDEX );
                }
                else
                    nX++;
            }
        }

        for( nY = 0L; nY < nOldHeight; nY++ )
            VECT_MAP( pMapIn, pMapOut, nY );

        for( nX = 0L, nTmpX = 5L; nX < nOldWidth; nX++, nTmpX += 4L )
        {
            for( nY = 0L; nY < nOldHeight; )
            {
                if( pRAcc->GetPixel( nY, nX ) == aTest )
                {
                    nTmpX -= 3L;
                    nTmpY = pMapIn[ nY++ ];

                    pMap->Set( nTmpY, nTmpX++, VECT_CONT_INDEX );
                    pMap->Set( nTmpY, nTmpX++, VECT_CONT_INDEX );
                    pMap->Set( nTmpY, nTmpX++, VECT_CONT_INDEX );
                    pMap->Set( nTmpY, nTmpX, VECT_CONT_INDEX );

                    while( nY < nOldHeight && pRAcc->GetPixel( nY, nX ) == aTest )
                        nY++;

                    nTmpX -= 3L;
                    nTmpY = pMapOut[ nY - 1L ];

                    pMap->Set( nTmpY, nTmpX++, VECT_CONT_INDEX );
                    pMap->Set( nTmpY, nTmpX++, VECT_CONT_INDEX );
                    pMap->Set( nTmpY, nTmpX++, VECT_CONT_INDEX );
                    pMap->Set( nTmpY, nTmpX, VECT_CONT_INDEX );
                }
                else
                    nY++;
            }
        }

        // cleanup
        delete[] pMapIn;
        delete[] pMapOut;
    }

    return pMap;
}

// -----------------------------------------------------------------------------

void ImplVectorizer::ImplCalculate( ImplVectMap* pMap, PolyPolygon& rPolyPoly, sal_uInt8 cReduce, sal_uLong nFlags )
{
    const long nWidth = pMap->Width(), nHeight= pMap->Height();

    for( long nY = 0L; nY < nHeight; nY++ )
    {
        long    nX = 0L;
        sal_Bool    bInner = sal_True;

        while( nX < nWidth )
        {
            // skip free
            while( ( nX < nWidth ) && pMap->IsFree( nY, nX ) )
                nX++;

            if( nX == nWidth )
                break;

            if( pMap->IsCont( nY, nX ) )
            {
                // new contour
                ImplChain   aChain;
                const Point aStartPt( nX++, nY );

                // get chain code
                aChain.ImplBeginAdd( aStartPt );
                ImplGetChain( pMap, aStartPt, aChain );

                if( nFlags & BMP_VECTORIZE_INNER )
                    aChain.ImplEndAdd( bInner ? VECT_POLY_INLINE_INNER : VECT_POLY_INLINE_OUTER );
                else
                    aChain.ImplEndAdd( bInner ? VECT_POLY_OUTLINE_INNER : VECT_POLY_OUTLINE_OUTER );

                const Polygon& rPoly = aChain.ImplGetPoly();

                if( rPoly.GetSize() > 2 )
                {
                    if( cReduce )
                    {
                        const Rectangle aBound( rPoly.GetBoundRect() );

                        if( aBound.GetWidth() > cReduce && aBound.GetHeight() > cReduce )
                            rPolyPoly.Insert( rPoly );
                    }
                    else
                        rPolyPoly.Insert( rPoly  );
                }

                // skip rest of detected contour
                while( pMap->IsCont( nY, nX ) )
                    nX++;
            }
            else
            {
                // process done segment
                const long nStartSegX = nX++;

                while( pMap->IsDone( nY, nX ) )
                    nX++;

                if( ( ( nX - nStartSegX ) == 1L ) || ( ImplIsUp( pMap, nY, nStartSegX ) != ImplIsUp( pMap, nY, nX - 1L ) ) )
                    bInner = !bInner;
            }
        }
    }
}

// -----------------------------------------------------------------------------

sal_Bool ImplVectorizer::ImplGetChain(  ImplVectMap* pMap, const Point& rStartPt, ImplChain& rChain )
{
    long                nActX = rStartPt.X();
    long                nActY = rStartPt.Y();
    long                nTryX;
    long                nTryY;
    sal_uLong               nFound;
    sal_uLong               nLastDir = 0UL;
    sal_uLong               nDir;

    do
    {
        nFound = 0UL;

        // first try last direction
        nTryX = nActX + aImplMove[ nLastDir ].nDX;
        nTryY = nActY + aImplMove[ nLastDir ].nDY;

        if( pMap->IsCont( nTryY, nTryX ) )
        {
            rChain.ImplAdd( (sal_uInt8) nLastDir );
            pMap->Set( nActY = nTryY, nActX = nTryX, VECT_DONE_INDEX );
            nFound = 1UL;
        }
        else
        {
            // try other directions
            for( nDir = 0UL; nDir < 8UL; nDir++ )
            {
                // we already tried nLastDir
                if( nDir != nLastDir )
                {
                    nTryX = nActX + aImplMove[ nDir ].nDX;
                    nTryY = nActY + aImplMove[ nDir ].nDY;

                    if( pMap->IsCont( nTryY, nTryX ) )
                    {
                        rChain.ImplAdd( (sal_uInt8) nDir );
                        pMap->Set( nActY = nTryY, nActX = nTryX, VECT_DONE_INDEX );
                        nFound = 1UL;
                        nLastDir = nDir;
                        break;
                    }
                }
            }
        }
    }
    while( nFound );

    return sal_True;
}

// -----------------------------------------------------------------------------

sal_Bool ImplVectorizer::ImplIsUp( ImplVectMap* pMap, long nY, long nX ) const
{
    if( pMap->IsDone( nY - 1L, nX ) )
        return sal_True;
    else if( pMap->IsDone( nY + 1L, nX ) )
        return sal_False;
    else if( pMap->IsDone( nY - 1L, nX - 1L ) || pMap->IsDone( nY - 1L, nX + 1L ) )
        return sal_True;
    else
        return sal_False;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


#include <vcl/animate.hxx>
#include <tools/debug.hxx>
#include <tools/stream.hxx>
#include <rtl/crc.h>
#include <vcl/virdev.hxx>
#include <vcl/window.hxx>
#include <impanmvw.hxx>
DBG_NAME( Animation )

// -----------
// - Defines -
// -----------

#define MIN_TIMEOUT 2L
#define INC_TIMEOUT 0L

// -----------
// - statics -
// -----------

sal_uLong Animation::mnAnimCount = 0UL;

// -------------------
// - AnimationBitmap -
// -------------------

sal_uLong AnimationBitmap::GetChecksum() const
{
    sal_uInt32  nCrc = aBmpEx.GetChecksum();
    SVBT32      aBT32;

    UInt32ToSVBT32( aPosPix.X(), aBT32 );
    nCrc = rtl_crc32( nCrc, aBT32, 4 );

    UInt32ToSVBT32( aPosPix.Y(), aBT32 );
    nCrc = rtl_crc32( nCrc, aBT32, 4 );

    UInt32ToSVBT32( aSizePix.Width(), aBT32 );
    nCrc = rtl_crc32( nCrc, aBT32, 4 );

    UInt32ToSVBT32( aSizePix.Height(), aBT32 );
    nCrc = rtl_crc32( nCrc, aBT32, 4 );

    UInt32ToSVBT32( (long) nWait, aBT32 );
    nCrc = rtl_crc32( nCrc, aBT32, 4 );

    UInt32ToSVBT32( (long) eDisposal, aBT32 );
    nCrc = rtl_crc32( nCrc, aBT32, 4 );

    UInt32ToSVBT32( (long) bUserInput, aBT32 );
    nCrc = rtl_crc32( nCrc, aBT32, 4 );

    return nCrc;
}

// -------------
// - Animation -
// -------------

Animation::Animation() :
    mnLoopCount         ( 0 ),
    mnLoops             ( 0 ),
    mnPos               ( 0 ),
    meCycleMode         ( CYCLE_NORMAL ),
    mbIsInAnimation     ( sal_False ),
    mbLoopTerminated    ( sal_False ),
    mbIsWaiting         ( sal_False )
{
    DBG_CTOR( Animation, NULL );
    maTimer.SetTimeoutHdl( LINK( this, Animation, ImplTimeoutHdl ) );
}

// -----------------------------------------------------------------------

Animation::Animation( const Animation& rAnimation ) :
    maBitmapEx          ( rAnimation.maBitmapEx ),
    maGlobalSize        ( rAnimation.maGlobalSize ),
    mnLoopCount         ( rAnimation.mnLoopCount ),
    mnPos               ( rAnimation.mnPos ),
    meCycleMode         ( rAnimation.meCycleMode ),
    mbIsInAnimation     ( sal_False ),
    mbLoopTerminated    ( rAnimation.mbLoopTerminated ),
    mbIsWaiting         ( rAnimation.mbIsWaiting )
{
    DBG_CTOR( Animation, NULL );

    for( size_t i = 0, nCount = rAnimation.maList.size(); i < nCount; i++ )
        maList.push_back( new AnimationBitmap( *rAnimation.maList[ i ] ) );

    maTimer.SetTimeoutHdl( LINK( this, Animation, ImplTimeoutHdl ) );
    mnLoops = mbLoopTerminated ? 0 : mnLoopCount;
}

// -----------------------------------------------------------------------

Animation::~Animation()
{
    DBG_DTOR( Animation, NULL );

    if( mbIsInAnimation )
        Stop();

    for( size_t i = 0, n = maList.size(); i < n; ++i )
        delete maList[ i ];

    for( size_t i = 0, n = maViewList.size(); i < n; ++i )
        delete maViewList[ i ];
}

// -----------------------------------------------------------------------

Animation& Animation::operator=( const Animation& rAnimation )
{
    Clear();

    for( size_t i = 0, nCount = rAnimation.maList.size(); i < nCount; i++ )
        maList.push_back( new AnimationBitmap( *rAnimation.maList[ i ] ) );

    maGlobalSize = rAnimation.maGlobalSize;
    maBitmapEx = rAnimation.maBitmapEx;
    meCycleMode = rAnimation.meCycleMode;
    mnLoopCount = rAnimation.mnLoopCount;
    mnPos = rAnimation.mnPos;
    mbLoopTerminated = rAnimation.mbLoopTerminated;
    mbIsWaiting = rAnimation.mbIsWaiting;
    mnLoops = mbLoopTerminated ? 0 : mnLoopCount;

    return *this;
}

// -----------------------------------------------------------------------

sal_Bool Animation::operator==( const Animation& rAnimation ) const
{
    const size_t nCount = maList.size();
    sal_Bool bRet = sal_False;

    if(  rAnimation.maList.size() == nCount
      && rAnimation.maBitmapEx    == maBitmapEx
      && rAnimation.maGlobalSize  == maGlobalSize
      && rAnimation.meCycleMode   == meCycleMode
      )
    {
        bRet = sal_True;

        for( size_t n = 0; n < nCount; n++ )
        {
            if( ( *maList[ n ] ) != ( *rAnimation.maList[ n ] ) )
            {
                bRet = sal_False;
                break;
            }
        }
    }

    return bRet;
}

void Animation::Clear()
{
    maTimer.Stop();
    mbIsInAnimation = sal_False;
    maGlobalSize = Size();
    maBitmapEx.SetEmpty();

    for( size_t i = 0, n = maList.size(); i < n; ++i )
        delete maList[ i ];
    maList.clear();

    for( size_t i = 0, n = maViewList.size(); i < n; ++i )
        delete maViewList[ i ];
    maViewList.clear();
}

// -----------------------------------------------------------------------

sal_Bool Animation::IsTransparent() const
{
    Point       aPoint;
    Rectangle   aRect( aPoint, maGlobalSize );
    sal_Bool        bRet = sal_False;

    // Falls irgendein 'kleines' Bildchen durch den Hintergrund
    // ersetzt werden soll, muessen wir 'transparent' sein, um
    // richtig dargestellt zu werden, da die Appl. aus Optimierungsgruenden
    // kein Invalidate auf nicht-transp. Grafiken ausfuehren
    for( size_t i = 0, nCount = maList.size(); i < nCount; i++ )
    {
        const AnimationBitmap* pAnimBmp = maList[ i ];

        if(  DISPOSE_BACK == pAnimBmp->eDisposal
          && Rectangle( pAnimBmp->aPosPix, pAnimBmp->aSizePix ) != aRect
          )
        {
            bRet = sal_True;
            break;
        }
    }

    if( !bRet )
        bRet = maBitmapEx.IsTransparent();

    return bRet;
}

// -----------------------------------------------------------------------

sal_uLong Animation::GetSizeBytes() const
{
    sal_uLong nSizeBytes = GetBitmapEx().GetSizeBytes();

    for( size_t i = 0, nCount = maList.size(); i < nCount; i++ )
    {
        const AnimationBitmap* pAnimBmp = maList[ i ];
        nSizeBytes += pAnimBmp->aBmpEx.GetSizeBytes();
    }

    return nSizeBytes;
}

// -----------------------------------------------------------------------

sal_uLong Animation::GetChecksum() const
{
    SVBT32      aBT32;
    sal_uInt32  nCrc = GetBitmapEx().GetChecksum();

    UInt32ToSVBT32( maList.size(), aBT32 );
    nCrc = rtl_crc32( nCrc, aBT32, 4 );

    UInt32ToSVBT32( maGlobalSize.Width(), aBT32 );
    nCrc = rtl_crc32( nCrc, aBT32, 4 );

    UInt32ToSVBT32( maGlobalSize.Height(), aBT32 );
    nCrc = rtl_crc32( nCrc, aBT32, 4 );

    UInt32ToSVBT32( (long) meCycleMode, aBT32 );
    nCrc = rtl_crc32( nCrc, aBT32, 4 );

    for( size_t i = 0, nCount = maList.size(); i < nCount; i++ )
    {
        UInt32ToSVBT32( maList[ i ]->GetChecksum(), aBT32 );
        nCrc = rtl_crc32( nCrc, aBT32, 4 );
    }

    return nCrc;
}

// -----------------------------------------------------------------------

sal_Bool Animation::Start( OutputDevice* pOut, const Point& rDestPt, const Size& rDestSz, long nExtraData,
                       OutputDevice* pFirstFrameOutDev )
{
    sal_Bool bRet = sal_False;

    if( !maList.empty() )
    {
        if(  ( pOut->GetOutDevType() == OUTDEV_WINDOW )
          && !mbLoopTerminated
          && ( ANIMATION_TIMEOUT_ON_CLICK != maList[ mnPos ]->nWait )
          )
        {
            ImplAnimView*   pView;
            ImplAnimView*   pMatch = NULL;

            for( size_t i = 0; i < maViewList.size(); ++i )
            {
                pView = maViewList[ i ];
                if( pView->ImplMatches( pOut, nExtraData ) )
                {
                    if( pView->ImplGetOutPos() == rDestPt &&
                        pView->ImplGetOutSizePix() == pOut->LogicToPixel( rDestSz ) )
                    {
                        pView->ImplRepaint();
                        pMatch = pView;
                    }
                    else
                    {
                        delete maViewList[ i ];
                        maViewList.erase( maViewList.begin() + i );
                        pView = NULL;
                    }

                    break;
                }
            }

            if( maViewList.empty() )
            {
                maTimer.Stop();
                mbIsInAnimation = sal_False;
                mnPos = 0UL;
            }

            if( !pMatch )
                maViewList.push_back( new ImplAnimView( this, pOut, rDestPt, rDestSz, nExtraData, pFirstFrameOutDev ) );

            if( !mbIsInAnimation )
            {
                ImplRestartTimer( maList[ mnPos ]->nWait );
                mbIsInAnimation = sal_True;
            }
        }
        else
            Draw( pOut, rDestPt, rDestSz );

        bRet = sal_True;
    }

    return bRet;
}

// -----------------------------------------------------------------------

void Animation::Stop( OutputDevice* pOut, long nExtraData )
{
    for( size_t i = 0; i < maViewList.size(); )
    {


        ImplAnimView* pView = maViewList[ i ];
        if( pView->ImplMatches( pOut, nExtraData ) )
        {
            delete pView;
            maViewList.erase( maViewList.begin() + i );
        }
        else
            i++;
    }

    if( maViewList.empty() )
    {
        maTimer.Stop();
        mbIsInAnimation = sal_False;
    }
}

// -----------------------------------------------------------------------

void Animation::Draw( OutputDevice* pOut, const Point& rDestPt ) const
{
    Draw( pOut, rDestPt, pOut->PixelToLogic( maGlobalSize ) );
}

// -----------------------------------------------------------------------

void Animation::Draw( OutputDevice* pOut, const Point& rDestPt, const Size& rDestSz ) const
{
    const size_t nCount = maList.size();

    if( nCount )
    {
        AnimationBitmap* pObj = maList[ Min( mnPos, nCount - 1 ) ];

        if(  pOut->GetConnectMetaFile()
          || ( pOut->GetOutDevType() == OUTDEV_PRINTER )
          )
            maList[ 0 ]->aBmpEx.Draw( pOut, rDestPt, rDestSz );
        else if( ANIMATION_TIMEOUT_ON_CLICK == pObj->nWait )
            pObj->aBmpEx.Draw( pOut, rDestPt, rDestSz );
        else
        {
            const size_t nOldPos = mnPos;
            ( (Animation*) this )->mnPos = mbLoopTerminated ? ( nCount - 1UL ) : mnPos;
            delete new ImplAnimView( (Animation*) this, pOut, rDestPt, rDestSz, 0 );
            ( (Animation*) this )->mnPos = nOldPos;
        }
    }
}

// -----------------------------------------------------------------------

void Animation::ImplRestartTimer( sal_uLong nTimeout )
{
    maTimer.SetTimeout( Max( nTimeout, (sal_uLong)(MIN_TIMEOUT + ( mnAnimCount - 1 ) * INC_TIMEOUT) ) * 10L );
    maTimer.Start();
}

// -----------------------------------------------------------------------
typedef ::std::vector< AInfo* > AInfoList_impl;

IMPL_LINK_NOARG(Animation, ImplTimeoutHdl)
{
    const size_t nAnimCount = maList.size();
    AInfoList_impl aAInfoList;

    if( nAnimCount )
    {
        ImplAnimView*   pView;
        sal_Bool        bGlobalPause = sal_True;

        if( maNotifyLink.IsSet() )
        {
            AInfo* pAInfo;

            // create AInfo-List
            for( size_t i = 0, n = maViewList.size(); i < n; ++i )
                aAInfoList.push_back( maViewList[ i ]->ImplCreateAInfo() );

            maNotifyLink.Call( this );

            // set view state from AInfo structure
            for( size_t i = 0, n = aAInfoList.size(); i < n; ++i )
            {
                pAInfo = aAInfoList[ i ];
                if( !pAInfo->pViewData )
                {
                    pView = new ImplAnimView( this, pAInfo->pOutDev,
                                              pAInfo->aStartOrg, pAInfo->aStartSize, pAInfo->nExtraData );

                    maViewList.push_back( pView );
                }
                else
                    pView = (ImplAnimView*) pAInfo->pViewData;

                pView->ImplPause( pAInfo->bPause );
                pView->ImplSetMarked( sal_True );
            }

            // delete AInfo structures
            for( size_t i = 0, n = aAInfoList.size(); i < n; ++i )
                delete aAInfoList[ i ];
            aAInfoList.clear();

            // delete all unmarked views and reset marked state
            for( size_t i = 0; i < maViewList.size(); )
            {
                pView = maViewList[ i ];
                if( !pView->ImplIsMarked() )
                {
                    delete pView;
                    maViewList.erase( maViewList.begin() + i );
                }
                else
                {
                    if( !pView->ImplIsPause() )
                        bGlobalPause = sal_False;

                    pView->ImplSetMarked( sal_False );
                    i++;
                }
            }
        }
        else
            bGlobalPause = sal_False;

        if( maViewList.empty() )
            Stop();
        else if( bGlobalPause )
            ImplRestartTimer( 10 );
        else
        {
            AnimationBitmap* pStepBmp = (++mnPos < maList.size()) ? maList[ mnPos ] : NULL;

            if( !pStepBmp )
            {
                if( mnLoops == 1 )
                {
                    Stop();
                    mbLoopTerminated = sal_True;
                    mnPos = nAnimCount - 1UL;
                    maBitmapEx = maList[ mnPos ]->aBmpEx;
                    return 0L;
                }
                else
                {
                    if( mnLoops )
                        mnLoops--;

                    mnPos = 0;
                    pStepBmp = maList[ mnPos ];
                }
            }

            // Paint all views; after painting check, if view is
            // marked; in this case remove view, because area of output
            // lies out of display area of window; mark state is
            // set from view itself
            for( size_t i = 0; i < maViewList.size(); )
            {
                pView = maViewList[ i ];
                pView->ImplDraw( mnPos );

                if( pView->ImplIsMarked() )
                {
                    delete pView;
                    maViewList.erase( maViewList.begin() + i );
                }
                else
                    i++;
            }

            // stop or restart timer
            if( maViewList.empty() )
                Stop();
            else
                ImplRestartTimer( pStepBmp->nWait );
        }
    }
    else
        Stop();

    return 0L;
}

// -----------------------------------------------------------------------

sal_Bool Animation::Insert( const AnimationBitmap& rStepBmp )
{
    sal_Bool bRet = sal_False;

    if( !IsInAnimation() )
    {
        Point       aPoint;
        Rectangle   aGlobalRect( aPoint, maGlobalSize );

        maGlobalSize = aGlobalRect.Union( Rectangle( rStepBmp.aPosPix, rStepBmp.aSizePix ) ).GetSize();
        maList.push_back( new AnimationBitmap( rStepBmp ) );

        // zunaechst nehmen wir die erste BitmapEx als Ersatz-BitmapEx
        if( maList.size() == 1 )
            maBitmapEx = rStepBmp.aBmpEx;

        bRet = sal_True;
    }

    return bRet;
}

// -----------------------------------------------------------------------

const AnimationBitmap& Animation::Get( sal_uInt16 nAnimation ) const
{
    DBG_ASSERT( ( nAnimation < maList.size() ), "No object at this position" );
    return *maList[ nAnimation ];
}

// -----------------------------------------------------------------------

void Animation::Replace( const AnimationBitmap& rNewAnimationBitmap, sal_uInt16 nAnimation )
{
    DBG_ASSERT( ( nAnimation < maList.size() ), "No object at this position" );

    delete maList[ nAnimation ];
    maList[ nAnimation ] = new AnimationBitmap( rNewAnimationBitmap );

    // Falls wir an erster Stelle einfuegen,
    // muessen wir natuerlich auch,
    // auch die Ersatzdarstellungs-BitmapEx
    // aktualisieren;
    if ( (  !nAnimation
         && (  !mbLoopTerminated
            || ( maList.size() == 1 )
            )
         )
         ||
         (  ( nAnimation == maList.size() - 1 )
         && mbLoopTerminated
         )
       )
    {
        maBitmapEx = rNewAnimationBitmap.aBmpEx;
    }
}

// -----------------------------------------------------------------------

void Animation::SetLoopCount( const sal_uLong nLoopCount )
{
    mnLoopCount = nLoopCount;
    ResetLoopCount();
}

// -----------------------------------------------------------------------

void Animation::ResetLoopCount()
{
    mnLoops = mnLoopCount;
    mbLoopTerminated = sal_False;
}

// -----------------------------------------------------------------------

sal_Bool Animation::Convert( BmpConversion eConversion )
{
    DBG_ASSERT( !IsInAnimation(), "Animation modified while it is animated" );

    sal_Bool bRet;

    if( !IsInAnimation() && !maList.empty() )
    {
        bRet = sal_True;

        for( size_t i = 0, n = maList.size(); ( i < n ) && bRet; ++i )
            bRet = maList[ i ]->aBmpEx.Convert( eConversion );

        maBitmapEx.Convert( eConversion );
    }
    else
        bRet = sal_False;

    return bRet;
}

// -----------------------------------------------------------------------

sal_Bool Animation::ReduceColors( sal_uInt16 nNewColorCount, BmpReduce eReduce )
{
    DBG_ASSERT( !IsInAnimation(), "Animation modified while it is animated" );

    sal_Bool bRet;

    if( !IsInAnimation() && !maList.empty() )
    {
        bRet = sal_True;

        for( size_t i = 0, n = maList.size(); ( i < n ) && bRet; ++i )
            bRet = maList[ i ]->aBmpEx.ReduceColors( nNewColorCount, eReduce );

        maBitmapEx.ReduceColors( nNewColorCount, eReduce );
    }
    else
        bRet = sal_False;

    return bRet;
}

// -----------------------------------------------------------------------

sal_Bool Animation::Invert()
{
    DBG_ASSERT( !IsInAnimation(), "Animation modified while it is animated" );

    sal_Bool bRet;

    if( !IsInAnimation() && !maList.empty() )
    {
        bRet = sal_True;

        for( size_t i = 0, n = maList.size(); ( i < n ) && bRet; ++i )
            bRet = maList[ i ]->aBmpEx.Invert();

        maBitmapEx.Invert();
    }
    else
        bRet = sal_False;

    return bRet;
}

// -----------------------------------------------------------------------

sal_Bool Animation::Mirror( sal_uLong nMirrorFlags )
{
    DBG_ASSERT( !IsInAnimation(), "Animation modified while it is animated" );

    sal_Bool    bRet;

    if( !IsInAnimation() && !maList.empty() )
    {
        bRet = sal_True;

        if( nMirrorFlags )
        {
            for( size_t i = 0, n = maList.size(); ( i < n ) && bRet; ++i )
            {
                AnimationBitmap* pStepBmp = maList[ i ];
                if( ( bRet = pStepBmp->aBmpEx.Mirror( nMirrorFlags ) ) == sal_True )
                {
                    if( nMirrorFlags & BMP_MIRROR_HORZ )
                        pStepBmp->aPosPix.X() = maGlobalSize.Width() - pStepBmp->aPosPix.X() - pStepBmp->aSizePix.Width();

                    if( nMirrorFlags & BMP_MIRROR_VERT )
                        pStepBmp->aPosPix.Y() = maGlobalSize.Height() - pStepBmp->aPosPix.Y() - pStepBmp->aSizePix.Height();
                }
            }

            maBitmapEx.Mirror( nMirrorFlags );
        }
    }
    else
        bRet = sal_False;

    return bRet;
}

// -----------------------------------------------------------------------

sal_Bool Animation::Adjust( short nLuminancePercent, short nContrastPercent,
             short nChannelRPercent, short nChannelGPercent, short nChannelBPercent,
             double fGamma, sal_Bool bInvert )
{
    DBG_ASSERT( !IsInAnimation(), "Animation modified while it is animated" );

    sal_Bool bRet;

    if( !IsInAnimation() && !maList.empty() )
    {
        bRet = sal_True;

        for( size_t i = 0, n = maList.size(); ( i < n ) && bRet; ++i )
        {
            bRet = maList[ i ]->aBmpEx.Adjust( nLuminancePercent,
                                               nContrastPercent,
                                               nChannelRPercent,
                                               nChannelGPercent,
                                               nChannelBPercent,
                                               fGamma, bInvert
                                             );
        }

        maBitmapEx.Adjust( nLuminancePercent, nContrastPercent,
                           nChannelRPercent, nChannelGPercent, nChannelBPercent,
                           fGamma, bInvert );
    }
    else
        bRet = sal_False;

    return bRet;
}

// -----------------------------------------------------------------------

sal_Bool Animation::Filter( BmpFilter eFilter, const BmpFilterParam* pFilterParam, const Link* pProgress )
{
    DBG_ASSERT( !IsInAnimation(), "Animation modified while it is animated" );

    sal_Bool bRet;

    if( !IsInAnimation() && !maList.empty() )
    {
        bRet = sal_True;

        for( size_t i = 0, n = maList.size(); ( i < n ) && bRet; ++i )
            bRet = maList[ i ]->aBmpEx.Filter( eFilter, pFilterParam, pProgress );

        maBitmapEx.Filter( eFilter, pFilterParam, pProgress );
    }
    else
        bRet = sal_False;

    return bRet;
}

// -----------------------------------------------------------------------

SvStream& operator<<( SvStream& rOStm, const Animation& rAnimation )
{
    const sal_uInt16 nCount = rAnimation.Count();

    if( nCount )
    {
        const sal_uInt32    nDummy32 = 0UL;

        // Falls keine BitmapEx gesetzt wurde, schreiben wir
        // einfach die erste Bitmap der Animation
        if( !rAnimation.GetBitmapEx().GetBitmap() )
            rOStm << rAnimation.Get( 0 ).aBmpEx;
        else
            rOStm << rAnimation.GetBitmapEx();

        // Kennung schreiben ( SDANIMA1 )
        rOStm << (sal_uInt32) 0x5344414e << (sal_uInt32) 0x494d4931;

        for( sal_uInt16 i = 0; i < nCount; i++ )
        {
            const AnimationBitmap&  rAnimBmp = rAnimation.Get( i );
            const sal_uInt16            nRest = nCount - i - 1;

            // AnimationBitmap schreiben
            rOStm << rAnimBmp.aBmpEx;
            rOStm << rAnimBmp.aPosPix;
            rOStm << rAnimBmp.aSizePix;
            rOStm << rAnimation.maGlobalSize;
            rOStm << (sal_uInt16) ( ( ANIMATION_TIMEOUT_ON_CLICK == rAnimBmp.nWait ) ? 65535 : rAnimBmp.nWait );
            rOStm << (sal_uInt16) rAnimBmp.eDisposal;
            rOStm << (sal_uInt8) rAnimBmp.bUserInput;
            rOStm << (sal_uInt32) rAnimation.mnLoopCount;
            rOStm << nDummy32;  // unbenutzt
            rOStm << nDummy32;  // unbenutzt
            rOStm << nDummy32;  // unbenutzt
            write_lenPrefixed_uInt8s_FromOString<sal_uInt16>(rOStm, rtl::OString()); // dummy
            rOStm << nRest;     // Anzahl der Strukturen, die noch _folgen_
        }
    }

    return rOStm;
}

// -----------------------------------------------------------------------

SvStream& operator>>( SvStream& rIStm, Animation& rAnimation )
{
    Bitmap  aBmp;
    sal_uLong   nStmPos = rIStm.Tell();
    sal_uInt32  nAnimMagic1, nAnimMagic2;
    sal_uInt16  nOldFormat = rIStm.GetNumberFormatInt();
    sal_Bool    bReadAnimations = sal_False;

    rIStm.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    nStmPos = rIStm.Tell();
    rIStm >> nAnimMagic1 >> nAnimMagic2;

    rAnimation.Clear();

    // Wenn die BitmapEx am Anfang schon gelesen
    // wurde ( von Graphic ), koennen wir direkt die Animationsbitmaps einlesen
    if( ( nAnimMagic1 == 0x5344414e ) && ( nAnimMagic2 == 0x494d4931 ) && !rIStm.GetError() )
        bReadAnimations = sal_True;
    // ansonsten versuchen wir erstmal die Bitmap(-Ex) zu lesen
    else
    {
        rIStm.Seek( nStmPos );
        rIStm >> rAnimation.maBitmapEx;
        nStmPos = rIStm.Tell();
        rIStm >> nAnimMagic1 >> nAnimMagic2;

        if( ( nAnimMagic1 == 0x5344414e ) && ( nAnimMagic2 == 0x494d4931 ) && !rIStm.GetError() )
            bReadAnimations = sal_True;
        else
            rIStm.Seek( nStmPos );
    }

    // ggf. Animationsbitmaps lesen
    if( bReadAnimations )
    {
        AnimationBitmap aAnimBmp;
        BitmapEx        aBmpEx;
        sal_uInt32          nTmp32;
        sal_uInt16          nTmp16;
        sal_uInt8           cTmp;

        do
        {
            rIStm >> aAnimBmp.aBmpEx;
            rIStm >> aAnimBmp.aPosPix;
            rIStm >> aAnimBmp.aSizePix;
            rIStm >> rAnimation.maGlobalSize;
            rIStm >> nTmp16; aAnimBmp.nWait = ( ( 65535 == nTmp16 ) ? ANIMATION_TIMEOUT_ON_CLICK : nTmp16 );
            rIStm >> nTmp16; aAnimBmp.eDisposal = ( Disposal) nTmp16;
            rIStm >> cTmp; aAnimBmp.bUserInput = (sal_Bool) cTmp;
            rIStm >> nTmp32; rAnimation.mnLoopCount = (sal_uInt16) nTmp32;
            rIStm >> nTmp32;    // unbenutzt
            rIStm >> nTmp32;    // unbenutzt
            rIStm >> nTmp32;    // unbenutzt
            read_lenPrefixed_uInt8s_ToOString<sal_uInt16>(rIStm); // unbenutzt
            rIStm >> nTmp16;    // Rest zu lesen

            rAnimation.Insert( aAnimBmp );
        }
        while( nTmp16 && !rIStm.GetError() );

        rAnimation.ResetLoopCount();
    }

    rIStm.SetNumberFormatInt( nOldFormat );

    return rIStm;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

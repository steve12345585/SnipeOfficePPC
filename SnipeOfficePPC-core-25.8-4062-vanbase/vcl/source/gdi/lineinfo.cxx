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

#include <tools/stream.hxx>
#include <tools/vcompat.hxx>
#include <tools/debug.hxx>
#include <vcl/lineinfo.hxx>
#include <basegfx/polygon/b2dpolypolygon.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <basegfx/polygon/b2dlinegeometry.hxx>
#include <numeric>

DBG_NAME( LineInfo )

// ----------------
// - ImplLineInfo -
// ----------------

ImplLineInfo::ImplLineInfo() :
    mnRefCount  ( 1 ),
    meStyle     ( LINE_SOLID ),
    mnWidth     ( 0 ),
    mnDashCount ( 0 ),
    mnDashLen   ( 0 ),
    mnDotCount  ( 0 ),
    mnDotLen    ( 0 ),
    mnDistance  ( 0 ),
    meLineJoin  ( basegfx::B2DLINEJOIN_ROUND ),
    meLineCap   ( com::sun::star::drawing::LineCap_BUTT )
{
}

// -----------------------------------------------------------------------

ImplLineInfo::ImplLineInfo( const ImplLineInfo& rImplLineInfo ) :
    mnRefCount  ( 1 ),
    meStyle     ( rImplLineInfo.meStyle ),
    mnWidth     ( rImplLineInfo.mnWidth ),
    mnDashCount ( rImplLineInfo.mnDashCount ),
    mnDashLen   ( rImplLineInfo.mnDashLen ),
    mnDotCount  ( rImplLineInfo.mnDotCount ),
    mnDotLen    ( rImplLineInfo.mnDotLen ),
    mnDistance  ( rImplLineInfo.mnDistance ),
    meLineJoin  ( rImplLineInfo.meLineJoin ),
    meLineCap   ( rImplLineInfo.meLineCap )
{
}

// -----------------------------------------------------------------------

inline bool ImplLineInfo::operator==( const ImplLineInfo& rB ) const
{
    return(meStyle == rB.meStyle
        && mnWidth == rB.mnWidth
        && mnDashCount == rB.mnDashCount
        && mnDashLen == rB.mnDashLen
        && mnDotCount == rB.mnDotCount
        && mnDotLen == rB.mnDotLen
        && mnDistance == rB.mnDistance
        && meLineJoin == rB.meLineJoin
        && meLineCap == rB.meLineCap);
}

// ------------
// - LineInfo -
// ------------

LineInfo::LineInfo( LineStyle eStyle, long nWidth )
{
    DBG_CTOR( LineInfo, NULL );
    mpImplLineInfo = new ImplLineInfo;
    mpImplLineInfo->meStyle = eStyle;
    mpImplLineInfo->mnWidth = nWidth;
}

// -----------------------------------------------------------------------

LineInfo::LineInfo( const LineInfo& rLineInfo )
{
    DBG_CTOR( LineInfo, NULL );
    DBG_CHKOBJ( &rLineInfo, LineInfo, NULL );
    mpImplLineInfo = rLineInfo.mpImplLineInfo;
    mpImplLineInfo->mnRefCount++;
}

// -----------------------------------------------------------------------

LineInfo::~LineInfo()
{
    DBG_DTOR( LineInfo, NULL );
    if( !( --mpImplLineInfo->mnRefCount ) )
        delete mpImplLineInfo;
}

// -----------------------------------------------------------------------

LineInfo& LineInfo::operator=( const LineInfo& rLineInfo )
{
    DBG_CHKTHIS( LineInfo, NULL );
    DBG_CHKOBJ( &rLineInfo, LineInfo, NULL );

    rLineInfo.mpImplLineInfo->mnRefCount++;

    if( !( --mpImplLineInfo->mnRefCount ) )
        delete mpImplLineInfo;

    mpImplLineInfo = rLineInfo.mpImplLineInfo;
    return *this;
}

// -----------------------------------------------------------------------

sal_Bool LineInfo::operator==( const LineInfo& rLineInfo ) const
{
    DBG_CHKTHIS( LineInfo, NULL );
    DBG_CHKOBJ( &rLineInfo, LineInfo, NULL );

    return( mpImplLineInfo == rLineInfo.mpImplLineInfo ||
           *mpImplLineInfo == *rLineInfo.mpImplLineInfo );
}

// -----------------------------------------------------------------------

void LineInfo::ImplMakeUnique()
{
    if( mpImplLineInfo->mnRefCount != 1 )
    {
        if( mpImplLineInfo->mnRefCount )
            mpImplLineInfo->mnRefCount--;

        mpImplLineInfo = new ImplLineInfo( *mpImplLineInfo );
    }
}

// -----------------------------------------------------------------------

void LineInfo::SetStyle( LineStyle eStyle )
{
    DBG_CHKTHIS( LineInfo, NULL );
    ImplMakeUnique();
    mpImplLineInfo->meStyle = eStyle;
}

// -----------------------------------------------------------------------

void LineInfo::SetWidth( long nWidth )
{
    DBG_CHKTHIS( LineInfo, NULL );
    ImplMakeUnique();
    mpImplLineInfo->mnWidth = nWidth;
}

// -----------------------------------------------------------------------

void LineInfo::SetDashCount( sal_uInt16 nDashCount )
{
    DBG_CHKTHIS( LineInfo, NULL );
    ImplMakeUnique();
    mpImplLineInfo->mnDashCount = nDashCount;
}

// -----------------------------------------------------------------------

void LineInfo::SetDashLen( long nDashLen )
{
    DBG_CHKTHIS( LineInfo, NULL );
    ImplMakeUnique();
    mpImplLineInfo->mnDashLen = nDashLen;
}

// -----------------------------------------------------------------------

void LineInfo::SetDotCount( sal_uInt16 nDotCount )
{
    DBG_CHKTHIS( LineInfo, NULL );
    ImplMakeUnique();
    mpImplLineInfo->mnDotCount = nDotCount;
}

// -----------------------------------------------------------------------

void LineInfo::SetDotLen( long nDotLen )
{
    DBG_CHKTHIS( LineInfo, NULL );
    ImplMakeUnique();
    mpImplLineInfo->mnDotLen = nDotLen;
}

// -----------------------------------------------------------------------

void LineInfo::SetDistance( long nDistance )
{
    DBG_CHKTHIS( LineInfo, NULL );
    ImplMakeUnique();
    mpImplLineInfo->mnDistance = nDistance;
}

// -----------------------------------------------------------------------

void LineInfo::SetLineJoin(basegfx::B2DLineJoin eLineJoin)
{
    DBG_CHKTHIS( LineInfo, NULL );

    if(eLineJoin != mpImplLineInfo->meLineJoin)
    {
        ImplMakeUnique();
        mpImplLineInfo->meLineJoin = eLineJoin;
    }
}

// -----------------------------------------------------------------------

void LineInfo::SetLineCap(com::sun::star::drawing::LineCap eLineCap)
{
    DBG_CHKTHIS( LineInfo, NULL );
    if(eLineCap != mpImplLineInfo->meLineCap)
    {
        ImplMakeUnique();
        mpImplLineInfo->meLineCap = eLineCap;
    }
}

// -----------------------------------------------------------------------

sal_Bool LineInfo::IsDefault() const
{
    return( !mpImplLineInfo->mnWidth
        && ( LINE_SOLID == mpImplLineInfo->meStyle )
        && ( com::sun::star::drawing::LineCap_BUTT == mpImplLineInfo->meLineCap));
}

// -----------------------------------------------------------------------

SvStream& operator>>( SvStream& rIStm, ImplLineInfo& rImplLineInfo )
{
    VersionCompat   aCompat( rIStm, STREAM_READ );
    sal_uInt16          nTmp16(0);
    sal_Int32       nTmp32(0);

    //#fdo39428 SvStream no longer supports operator>>(long&)
    rIStm >> nTmp16; rImplLineInfo.meStyle = (LineStyle) nTmp16;
    rIStm >> nTmp32;
    rImplLineInfo.mnWidth = nTmp32;

    if( aCompat.GetVersion() >= 2 )
    {
        // version 2
        rIStm >> rImplLineInfo.mnDashCount >> nTmp32;
        rImplLineInfo.mnDashLen = nTmp32;
        rIStm >> rImplLineInfo.mnDotCount >> nTmp32;
        rImplLineInfo.mnDotLen = nTmp32;
        rIStm >> nTmp32;
        rImplLineInfo.mnDistance = nTmp32;
    }

    if( aCompat.GetVersion() >= 3 )
    {
        // version 3
        rIStm >> nTmp16; rImplLineInfo.meLineJoin = (basegfx::B2DLineJoin) nTmp16;
    }

    if( aCompat.GetVersion() >= 4 )
    {
        // version 4
        rIStm >> nTmp16; rImplLineInfo.meLineCap = (com::sun::star::drawing::LineCap) nTmp16;
    }

    return rIStm;
}

// -----------------------------------------------------------------------

SvStream& operator<<( SvStream& rOStm, const ImplLineInfo& rImplLineInfo )
{
    VersionCompat aCompat( rOStm, STREAM_WRITE, 4 );

    //#fdo39428 SvStream no longer supports operator<<(long)
    // version 1
    rOStm << (sal_uInt16) rImplLineInfo.meStyle << sal::static_int_cast<sal_Int32>(rImplLineInfo.mnWidth);

    // since version2
    rOStm << rImplLineInfo.mnDashCount << sal::static_int_cast<sal_Int32>(rImplLineInfo.mnDashLen);
    rOStm << rImplLineInfo.mnDotCount << sal::static_int_cast<sal_Int32>(rImplLineInfo.mnDotLen);
    rOStm << sal::static_int_cast<sal_Int32>(rImplLineInfo.mnDistance);

    // since version3
    rOStm << (sal_uInt16) rImplLineInfo.meLineJoin;

    // since version4
    rOStm << (sal_uInt16) rImplLineInfo.meLineCap;

    return rOStm;
}

// -----------------------------------------------------------------------

SvStream& operator>>( SvStream& rIStm, LineInfo& rLineInfo )
{
    rLineInfo.ImplMakeUnique();
    return( rIStm >> *rLineInfo.mpImplLineInfo );
}

// -----------------------------------------------------------------------

SvStream& operator<<( SvStream& rOStm, const LineInfo& rLineInfo )
{
    return( rOStm << *rLineInfo.mpImplLineInfo );
}

// -----------------------------------------------------------------------

void LineInfo::applyToB2DPolyPolygon(
    basegfx::B2DPolyPolygon& io_rLinePolyPolygon,
    basegfx::B2DPolyPolygon& o_rFillPolyPolygon) const
{
    o_rFillPolyPolygon.clear();

    if(io_rLinePolyPolygon.count())
    {
        if(LINE_DASH == GetStyle())
        {
            ::std::vector< double > fDotDashArray;
            const double fDashLen(GetDashLen());
            const double fDotLen(GetDotLen());
            const double fDistance(GetDistance());

            for(sal_uInt16 a(0); a < GetDashCount(); a++)
            {
                fDotDashArray.push_back(fDashLen);
                fDotDashArray.push_back(fDistance);
            }

            for(sal_uInt16 b(0); b < GetDotCount(); b++)
            {
                fDotDashArray.push_back(fDotLen);
                fDotDashArray.push_back(fDistance);
            }

            const double fAccumulated(::std::accumulate(fDotDashArray.begin(), fDotDashArray.end(), 0.0));

            if(fAccumulated > 0.0)
            {
                basegfx::B2DPolyPolygon aResult;

                for(sal_uInt32 c(0); c < io_rLinePolyPolygon.count(); c++)
                {
                    basegfx::B2DPolyPolygon aLineTraget;
                    basegfx::tools::applyLineDashing(
                        io_rLinePolyPolygon.getB2DPolygon(c),
                        fDotDashArray,
                        &aLineTraget);
                    aResult.append(aLineTraget);
                }

                io_rLinePolyPolygon = aResult;
            }
        }

        if(GetWidth() > 1 && io_rLinePolyPolygon.count())
        {
            const double fHalfLineWidth((GetWidth() * 0.5) + 0.5);

            for(sal_uInt32 a(0); a < io_rLinePolyPolygon.count(); a++)
            {
                o_rFillPolyPolygon.append(basegfx::tools::createAreaGeometry(
                    io_rLinePolyPolygon.getB2DPolygon(a),
                    fHalfLineWidth,
                    GetLineJoin(),
                    GetLineCap()));
            }

            io_rLinePolyPolygon.clear();
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

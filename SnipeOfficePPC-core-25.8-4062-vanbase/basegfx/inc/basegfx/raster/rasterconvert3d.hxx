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

#ifndef _BGFX_RASTER_RASTERCONVERT3D_HXX
#define _BGFX_RASTER_RASTERCONVERT3D_HXX

#include <sal/types.h>
#include <vector>
#include <basegfx/color/bcolor.hxx>
#include <basegfx/vector/b3dvector.hxx>
#include <basegfx/point/b2dpoint.hxx>
#include <basegfx/vector/b2dvector.hxx>
#include <basegfx/basegfxdllapi.h>

//////////////////////////////////////////////////////////////////////////////
// predeclarations

namespace basegfx
{
    class B3DPolygon;
    class B3DPolyPolygon;
}

//////////////////////////////////////////////////////////////////////////////
// interpolators for double precision

namespace basegfx
{
    class ip_single
    {
    private:
        double                                      mfVal;
        double                                      mfInc;

    public:
        ip_single()
        :   mfVal(0.0),
            mfInc(0.0)
        {}

        ip_single(double fVal, double fInc)
        :   mfVal(fVal),
            mfInc(fInc)
        {}

        double getVal() const { return mfVal; }
        double getInc() const { return mfInc; }

        void increment(double fStep) { mfVal += fStep * mfInc; }
    };
} // end of namespace basegfx

namespace basegfx
{
    class ip_double
    {
    private:
        ip_single                                   maX;
        ip_single                                   maY;

    public:
        ip_double()
        :   maX(),
            maY()
        {}

        ip_double(double fXVal, double fXInc, double fYVal, double fYInc)
        :   maX(fXVal, fXInc),
            maY(fYVal, fYInc)
        {}

        const ip_single& getX() const { return maX; }
        const ip_single& getY() const { return maY; }

        void increment(double fStep) { maX.increment(fStep); maY.increment(fStep); }
    };
} // end of namespace basegfx

namespace basegfx
{
    class ip_triple
    {
    private:
        ip_single                                   maX;
        ip_single                                   maY;
        ip_single                                   maZ;

    public:
        ip_triple()
        :   maX(),
            maY(),
            maZ()
        {}

        ip_triple(double fXVal, double fXInc, double fYVal, double fYInc, double fZVal, double fZInc)
        :   maX(fXVal, fXInc),
            maY(fYVal, fYInc),
            maZ(fZVal, fZInc)
        {}

        const ip_single& getX() const { return maX; }
        const ip_single& getY() const { return maY; }
        const ip_single& getZ() const { return maZ; }

        void increment(double fStep) { maX.increment(fStep); maY.increment(fStep); maZ.increment(fStep); }
    };
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////
// InterpolatorProvider3D to have a common source for allocating interpolators
// which may then be addressed using the index to the vectors

namespace basegfx
{
    #define SCANLINE_EMPTY_INDEX (0xffffffff)

    class InterpolatorProvider3D
    {
    private:
        ::std::vector< ip_triple >                  maColorInterpolators;
        ::std::vector< ip_triple >                  maNormalInterpolators;
        ::std::vector< ip_double >                  maTextureInterpolators;
        ::std::vector< ip_triple >                  maInverseTextureInterpolators;

    protected:
        sal_uInt32 addColorInterpolator(const BColor& rA, const BColor& rB, double fInvYDelta)
        {
            B3DVector aDelta(rB.getRed() - rA.getRed(), rB.getGreen() - rA.getGreen(), rB.getBlue() - rA.getBlue());
            aDelta *= fInvYDelta;
            maColorInterpolators.push_back(ip_triple(rA.getRed(), aDelta.getX(), rA.getGreen(), aDelta.getY(), rA.getBlue(), aDelta.getZ()));
            return (maColorInterpolators.size() - 1L);
        }

        sal_uInt32 addNormalInterpolator(const B3DVector& rA, const B3DVector& rB, double fInvYDelta)
        {
            B3DVector aDelta(rB.getX() - rA.getX(), rB.getY() - rA.getY(), rB.getZ() - rA.getZ());
            aDelta *= fInvYDelta;
            maNormalInterpolators.push_back(ip_triple(rA.getX(), aDelta.getX(), rA.getY(), aDelta.getY(), rA.getZ(), aDelta.getZ()));
            return (maNormalInterpolators.size() - 1L);
        }

        sal_uInt32 addTextureInterpolator(const B2DPoint& rA, const B2DPoint& rB, double fInvYDelta)
        {
            B2DVector aDelta(rB.getX() - rA.getX(), rB.getY() - rA.getY());
            aDelta *= fInvYDelta;
            maTextureInterpolators.push_back(ip_double(rA.getX(), aDelta.getX(), rA.getY(), aDelta.getY()));
            return (maTextureInterpolators.size() - 1L);
        }

        sal_uInt32 addInverseTextureInterpolator(const B2DPoint& rA, const B2DPoint& rB, double fZEyeA, double fZEyeB, double fInvYDelta)
        {
            const double fInvZEyeA(fTools::equalZero(fZEyeA) ? fZEyeA : 1.0 / fZEyeA);
            const double fInvZEyeB(fTools::equalZero(fZEyeB) ? fZEyeB : 1.0 / fZEyeB);
            const B2DPoint aInvA(rA * fInvZEyeA);
            const B2DPoint aInvB(rB * fInvZEyeB);
            double fZDelta(fInvZEyeB - fInvZEyeA);
            B2DVector aDelta(aInvB.getX() - aInvA.getX(), aInvB.getY() - aInvA.getY());

            fZDelta *= fInvYDelta;
            aDelta *= fInvYDelta;

            maInverseTextureInterpolators.push_back(ip_triple(aInvA.getX(), aDelta.getX(), aInvA.getY(), aDelta.getY(), fInvZEyeA, fZDelta));
            return (maInverseTextureInterpolators.size() - 1L);
        }

        void reset()
        {
            maColorInterpolators.clear();
            maNormalInterpolators.clear();
            maTextureInterpolators.clear();
            maInverseTextureInterpolators.clear();
        }

    public:
        InterpolatorProvider3D() {}

        ::std::vector< ip_triple >& getColorInterpolators() { return maColorInterpolators; }
        ::std::vector< ip_triple >& getNormalInterpolators() { return maNormalInterpolators; }
        ::std::vector< ip_double >& getTextureInterpolators() { return maTextureInterpolators; }
        ::std::vector< ip_triple >& getInverseTextureInterpolators() { return maInverseTextureInterpolators; }
    };
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////
// RasterConversionLineEntry3D for Raterconversion of 3D PolyPolygons

namespace basegfx
{
    class RasterConversionLineEntry3D
    {
    private:
        ip_single                                   maX;
        ip_single                                   maZ;
        sal_Int32                                   mnY;
        sal_uInt32                                  mnCount;

        sal_uInt32                                  mnColorIndex;
        sal_uInt32                                  mnNormalIndex;
        sal_uInt32                                  mnTextureIndex;
        sal_uInt32                                  mnInverseTextureIndex;

    public:
        RasterConversionLineEntry3D(const double& rfX, const double& rfDeltaX, const double& rfZ, const double& rfDeltaZ, sal_Int32 nY, sal_uInt32 nCount)
        :   maX(rfX, rfDeltaX),
            maZ(rfZ, rfDeltaZ),
            mnY(nY),
            mnCount(nCount),
            mnColorIndex(SCANLINE_EMPTY_INDEX),
            mnNormalIndex(SCANLINE_EMPTY_INDEX),
            mnTextureIndex(SCANLINE_EMPTY_INDEX),
            mnInverseTextureIndex(SCANLINE_EMPTY_INDEX)
        {}

        void setColorIndex(sal_uInt32 nIndex) { mnColorIndex = nIndex; }
        void setNormalIndex(sal_uInt32 nIndex) { mnNormalIndex = nIndex; }
        void setTextureIndex(sal_uInt32 nIndex) { mnTextureIndex = nIndex; }
        void setInverseTextureIndex(sal_uInt32 nIndex) { mnInverseTextureIndex = nIndex; }

        bool operator<(const RasterConversionLineEntry3D& rComp) const
        {
            if(mnY == rComp.mnY)
            {
                return maX.getVal() < rComp.maX.getVal();
            }

            return mnY < rComp.mnY;
        }

        bool decrementRasterConversionLineEntry3D(sal_uInt32 nStep)
        {
            if(nStep >= mnCount)
            {
                return false;
            }
            else
            {
                mnCount -= nStep;
                return true;
            }
        }

        void incrementRasterConversionLineEntry3D(sal_uInt32 nStep, InterpolatorProvider3D& rProvider)
        {
            const double fStep((double)nStep);
            maX.increment(fStep);
            maZ.increment(fStep);
            mnY += nStep;

            if(SCANLINE_EMPTY_INDEX != mnColorIndex)
            {
                rProvider.getColorInterpolators()[mnColorIndex].increment(fStep);
            }

            if(SCANLINE_EMPTY_INDEX != mnNormalIndex)
            {
                rProvider.getNormalInterpolators()[mnNormalIndex].increment(fStep);
            }

            if(SCANLINE_EMPTY_INDEX != mnTextureIndex)
            {
                rProvider.getTextureInterpolators()[mnTextureIndex].increment(fStep);
            }

            if(SCANLINE_EMPTY_INDEX != mnInverseTextureIndex)
            {
                rProvider.getInverseTextureInterpolators()[mnInverseTextureIndex].increment(fStep);
            }
        }

        // data read access
        const ip_single& getX() const { return maX; }
        sal_Int32 getY() const { return mnY; }
        const ip_single& getZ() const { return maZ; }
        sal_uInt32 getColorIndex() const { return mnColorIndex; }
        sal_uInt32 getNormalIndex() const { return mnNormalIndex; }
        sal_uInt32 getTextureIndex() const { return mnTextureIndex; }
        sal_uInt32 getInverseTextureIndex() const { return mnInverseTextureIndex; }
    };
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////
// the basic RaterConverter itself. Only one method needs to be overloaded. The
// class itself is strictly virtual

namespace basegfx
{
    class BASEGFX_DLLPUBLIC RasterConverter3D : public InterpolatorProvider3D
    {
    private:
        // the line entries for an area conversion run
        ::std::vector< RasterConversionLineEntry3D >            maLineEntries;

        struct lineComparator
        {
            bool operator()(const RasterConversionLineEntry3D* pA, const RasterConversionLineEntry3D* pB)
            {
                OSL_ENSURE(pA && pB, "lineComparator: empty pointer (!)");
                return pA->getX().getVal() < pB->getX().getVal();
            }
        };

        void addArea(const B3DPolygon& rFill, const B3DHomMatrix* pViewToEye);
        void addArea(const B3DPolyPolygon& rFill, const B3DHomMatrix* pViewToEye);
        void addEdge(const B3DPolygon& rFill, sal_uInt32 a, sal_uInt32 b, const B3DHomMatrix* pViewToEye);

        void rasterconvertB3DArea(sal_Int32 nStartLine, sal_Int32 nStopLine);
        void rasterconvertB3DEdge(const B3DPolygon& rLine, sal_uInt32 nA, sal_uInt32 nB, sal_Int32 nStartLine, sal_Int32 nStopLine, sal_uInt16 nLineWidth);

        virtual void processLineSpan(const RasterConversionLineEntry3D& rA, const RasterConversionLineEntry3D& rB, sal_Int32 nLine, sal_uInt32 nSpanCount) = 0;

    public:
        RasterConverter3D();
        virtual ~RasterConverter3D();

        void rasterconvertB3DPolyPolygon(const B3DPolyPolygon& rFill, const B3DHomMatrix* pViewToEye, sal_Int32 nStartLine, sal_Int32 nStopLine);
        void rasterconvertB3DPolygon(const B3DPolygon& rLine, sal_Int32 nStartLine, sal_Int32 nStopLine, sal_uInt16 nLineWidth);
    };
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////

#endif /* _BGFX_RASTER_RASTERCONVERT3D_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

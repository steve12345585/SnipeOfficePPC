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

#include <drawinglayer/texture/texture3d.hxx>
#include <vcl/bmpacc.hxx>
#include <drawinglayer/primitive3d/hatchtextureprimitive3d.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace texture
    {
        GeoTexSvxMono::GeoTexSvxMono(const basegfx::BColor& rSingleColor, double fOpacity)
        :   maSingleColor(rSingleColor),
            mfOpacity(fOpacity)
        {
        }

        bool GeoTexSvxMono::operator==(const GeoTexSvx& rGeoTexSvx) const
        {
            const GeoTexSvxMono* pCompare = dynamic_cast< const GeoTexSvxMono* >(&rGeoTexSvx);
            return (pCompare
                && maSingleColor == pCompare->maSingleColor
                && mfOpacity == pCompare->mfOpacity);
        }

        void GeoTexSvxMono::modifyBColor(const basegfx::B2DPoint& /*rUV*/, basegfx::BColor& rBColor, double& /*rfOpacity*/) const
        {
            rBColor = maSingleColor;
        }

        void GeoTexSvxMono::modifyOpacity(const basegfx::B2DPoint& /*rUV*/, double& rfOpacity) const
        {
            rfOpacity = mfOpacity;
        }
    } // end of namespace texture
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace texture
    {
        GeoTexSvxBitmap::GeoTexSvxBitmap(const Bitmap& rBitmap, const basegfx::B2DPoint& rTopLeft, const basegfx::B2DVector& rSize)
        :   maBitmap(rBitmap),
            mpRead(0L),
            maTopLeft(rTopLeft),
            maSize(rSize),
            mfMulX(0.0),
            mfMulY(0.0)
        {
            mpRead = maBitmap.AcquireReadAccess();
            OSL_ENSURE(mpRead, "GeoTexSvxBitmap: Got no read access to Bitmap (!)");
            mfMulX = (double)mpRead->Width() / maSize.getX();
            mfMulY = (double)mpRead->Height() / maSize.getY();
        }

        GeoTexSvxBitmap::~GeoTexSvxBitmap()
        {
            delete mpRead;
        }

        bool GeoTexSvxBitmap::impIsValid(const basegfx::B2DPoint& rUV, sal_Int32& rX, sal_Int32& rY) const
        {
            if(mpRead)
            {
                rX = (sal_Int32)((rUV.getX() - maTopLeft.getX()) * mfMulX);

                if(rX >= 0L && rX < mpRead->Width())
                {
                    rY = (sal_Int32)((rUV.getY() - maTopLeft.getY()) * mfMulY);

                    return (rY >= 0L && rY < mpRead->Height());
                }
            }

            return false;
        }

        void GeoTexSvxBitmap::modifyBColor(const basegfx::B2DPoint& rUV, basegfx::BColor& rBColor, double& rfOpacity) const
        {
            sal_Int32 nX, nY;

            if(impIsValid(rUV, nX, nY))
            {
                const double fConvertColor(1.0 / 255.0);
                const BitmapColor aBMCol(mpRead->GetColor(nY, nX));
                const basegfx::BColor aBSource(
                    (double)aBMCol.GetRed() * fConvertColor,
                    (double)aBMCol.GetGreen() * fConvertColor,
                    (double)aBMCol.GetBlue() * fConvertColor);

                rBColor = aBSource;
            }
            else
            {
                rfOpacity = 0.0;
            }
        }

        void GeoTexSvxBitmap::modifyOpacity(const basegfx::B2DPoint& rUV, double& rfOpacity) const
        {
            sal_Int32 nX, nY;

            if(impIsValid(rUV, nX, nY))
            {
                const BitmapColor aBMCol(mpRead->GetColor(nY, nX));
                const Color aColor(aBMCol.GetRed(), aBMCol.GetGreen(), aBMCol.GetBlue());

                rfOpacity = ((double)(0xff - aColor.GetLuminance()) * (1.0 / 255.0));
            }
            else
            {
                rfOpacity = 0.0;
            }
        }
    } // end of namespace texture
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace texture
    {
        GeoTexSvxBitmapTiled::GeoTexSvxBitmapTiled(const Bitmap& rBitmap, const basegfx::B2DPoint& rTopLeft, const basegfx::B2DVector& rSize)
        :   GeoTexSvxBitmap(rBitmap, rTopLeft, rSize)
        {
        }

        void GeoTexSvxBitmapTiled::modifyBColor(const basegfx::B2DPoint& rUV, basegfx::BColor& rBColor, double& rfOpacity) const
        {
            if(mpRead)
            {
                GeoTexSvxBitmap::modifyBColor(impGetCorrected(rUV), rBColor, rfOpacity);
            }
        }

        void GeoTexSvxBitmapTiled::modifyOpacity(const basegfx::B2DPoint& rUV, double& rfOpacity) const
        {
            if(mpRead)
            {
                GeoTexSvxBitmap::modifyOpacity(impGetCorrected(rUV), rfOpacity);
            }
        }
    } // end of namespace texture
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace texture
    {
        GeoTexSvxMultiHatch::GeoTexSvxMultiHatch(const primitive3d::HatchTexturePrimitive3D& rPrimitive, double fLogicPixelSize)
        :   mfLogicPixelSize(fLogicPixelSize),
            mp0(0L),
            mp1(0L),
            mp2(0L)
        {
            const attribute::FillHatchAttribute& rHatch(rPrimitive.getHatch());
            const basegfx::B2DRange aOutlineRange(0.0, 0.0, rPrimitive.getTextureSize().getX(), rPrimitive.getTextureSize().getY());
            const double fAngleA(rHatch.getAngle());
            maColor = rHatch.getColor();
            mbFillBackground = rHatch.isFillBackground();
            mp0 = new GeoTexSvxHatch(aOutlineRange, rHatch.getDistance(), fAngleA);

            if(attribute::HATCHSTYLE_DOUBLE == rHatch.getStyle() || attribute::HATCHSTYLE_TRIPLE == rHatch.getStyle())
            {
                mp1 = new GeoTexSvxHatch(aOutlineRange, rHatch.getDistance(), fAngleA + F_PI2);
            }

            if(attribute::HATCHSTYLE_TRIPLE == rHatch.getStyle())
            {
                mp2 = new GeoTexSvxHatch(aOutlineRange, rHatch.getDistance(), fAngleA + F_PI4);
            }
        }

        GeoTexSvxMultiHatch::~GeoTexSvxMultiHatch()
        {
            delete mp0;
            delete mp1;
            delete mp2;
        }

        bool GeoTexSvxMultiHatch::impIsOnHatch(const basegfx::B2DPoint& rUV) const
        {
            if(mp0->getDistanceToHatch(rUV) < mfLogicPixelSize)
            {
                return true;
            }

            if(mp1 && mp1->getDistanceToHatch(rUV) < mfLogicPixelSize)
            {
                return true;
            }

            if(mp2 && mp2->getDistanceToHatch(rUV) < mfLogicPixelSize)
            {
                return true;
            }

            return false;
        }

        void GeoTexSvxMultiHatch::modifyBColor(const basegfx::B2DPoint& rUV, basegfx::BColor& rBColor, double& rfOpacity) const
        {
            if(impIsOnHatch(rUV))
            {
                rBColor = maColor;
            }
            else if(!mbFillBackground)
            {
                rfOpacity = 0.0;
            }
        }

        void GeoTexSvxMultiHatch::modifyOpacity(const basegfx::B2DPoint& rUV, double& rfOpacity) const
        {
            if(mbFillBackground || impIsOnHatch(rUV))
            {
                rfOpacity = 1.0;
            }
            else
            {
                rfOpacity = 0.0;
            }
        }
    } // end of namespace texture
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////
// eof

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

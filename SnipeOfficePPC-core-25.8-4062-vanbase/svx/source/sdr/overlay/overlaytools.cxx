/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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


#include <svx/sdr/overlay/overlaytools.hxx>
#include <svx/sdr/primitive2d/svx_primitivetypes2d.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <drawinglayer/primitive2d/bitmapprimitive2d.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <drawinglayer/primitive2d/polygonprimitive2d.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <drawinglayer/primitive2d/polypolygonprimitive2d.hxx>
#include <drawinglayer/geometry/viewinformation2d.hxx>
#include <basegfx/matrix/b2dhommatrixtools.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        OverlayBitmapExPrimitive::OverlayBitmapExPrimitive(
            const BitmapEx& rBitmapEx,
            const basegfx::B2DPoint& rBasePosition,
            sal_uInt16 nCenterX,
            sal_uInt16 nCenterY)
        :   DiscreteMetricDependentPrimitive2D(),
            maBitmapEx(rBitmapEx),
            maBasePosition(rBasePosition),
            mnCenterX(nCenterX),
            mnCenterY(nCenterY)
        {}

        Primitive2DSequence OverlayBitmapExPrimitive::create2DDecomposition(const geometry::ViewInformation2D& /*rViewInformation*/) const
        {
            Primitive2DSequence aRetval;
            const Size aBitmapSize(getBitmapEx().GetSizePixel());

            if(aBitmapSize.Width() && aBitmapSize.Height() && basegfx::fTools::more(getDiscreteUnit(), 0.0))
            {
                // calculate back from internal bitmap's extreme coordinates (the edges)
                // to logical coordinates. Only use a unified scaling value (getDiscreteUnit(),
                // the prepared one which expresses how many logic units form a discrete unit)
                // for this step. This primitive is to be displayed always unscaled (in it's pixel size)
                // and unrotated, more like a marker
                const double fLeft(((0.0 - getCenterX()) * getDiscreteUnit()) + getBasePosition().getX());
                const double fTop(((0.0 - getCenterY()) * getDiscreteUnit()) + getBasePosition().getY());
                const double fRight(((aBitmapSize.getWidth() - getCenterX()) * getDiscreteUnit()) + getBasePosition().getX());
                const double fBottom(((aBitmapSize.getHeight() - getCenterY()) * getDiscreteUnit()) + getBasePosition().getY());

                // create a BitmapPrimitive2D using those positions
                basegfx::B2DHomMatrix aTransform;

                aTransform.set(0, 0, fRight - fLeft);
                aTransform.set(1, 1, fBottom - fTop);
                aTransform.set(0, 2, fLeft);
                aTransform.set(1, 2, fTop);

                const Primitive2DReference aPrimitive(new BitmapPrimitive2D(getBitmapEx(), aTransform));
                aRetval = Primitive2DSequence(&aPrimitive, 1);
            }

            return aRetval;
        }

        bool OverlayBitmapExPrimitive::operator==( const BasePrimitive2D& rPrimitive ) const
        {
            if(DiscreteMetricDependentPrimitive2D::operator==(rPrimitive))
            {
                const OverlayBitmapExPrimitive& rCompare = static_cast< const OverlayBitmapExPrimitive& >(rPrimitive);

                return (getBitmapEx() == rCompare.getBitmapEx()
                    && getBasePosition() == rCompare.getBasePosition()
                    && getCenterX() == rCompare.getCenterX()
                    && getCenterY() == rCompare.getCenterY());
            }

            return false;
        }

        ImplPrimitrive2DIDBlock(OverlayBitmapExPrimitive, PRIMITIVE2D_ID_OVERLAYBITMAPEXPRIMITIVE)

    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        OverlayCrosshairPrimitive::OverlayCrosshairPrimitive(
            const basegfx::B2DPoint& rBasePosition,
            const basegfx::BColor& rRGBColorA,
            const basegfx::BColor& rRGBColorB,
            double fDiscreteDashLength)
        :   ViewportDependentPrimitive2D(),
            maBasePosition(rBasePosition),
            maRGBColorA(rRGBColorA),
            maRGBColorB(rRGBColorB),
            mfDiscreteDashLength(fDiscreteDashLength)
        {}

        Primitive2DSequence OverlayCrosshairPrimitive::create2DDecomposition(const geometry::ViewInformation2D& /*rViewInformation*/) const
        {
            // use the prepared Viewport information accessible using getViewport()
            Primitive2DSequence aRetval;

            if(!getViewport().isEmpty())
            {
                aRetval.realloc(2);
                basegfx::B2DPolygon aPolygon;

                aPolygon.append(basegfx::B2DPoint(getViewport().getMinX(), getBasePosition().getY()));
                aPolygon.append(basegfx::B2DPoint(getViewport().getMaxX(), getBasePosition().getY()));

                aRetval[0] = Primitive2DReference(
                    new PolygonMarkerPrimitive2D(
                        aPolygon,
                        getRGBColorA(),
                        getRGBColorB(),
                        getDiscreteDashLength()));

                aPolygon.clear();
                aPolygon.append(basegfx::B2DPoint(getBasePosition().getX(), getViewport().getMinY()));
                aPolygon.append(basegfx::B2DPoint(getBasePosition().getX(), getViewport().getMaxY()));

                aRetval[1] = Primitive2DReference(
                    new PolygonMarkerPrimitive2D(
                        aPolygon,
                        getRGBColorA(),
                        getRGBColorB(),
                        getDiscreteDashLength()));
            }

            return aRetval;
        }

        bool OverlayCrosshairPrimitive::operator==( const BasePrimitive2D& rPrimitive ) const
        {
            if(ViewportDependentPrimitive2D::operator==(rPrimitive))
            {
                const OverlayCrosshairPrimitive& rCompare = static_cast< const OverlayCrosshairPrimitive& >(rPrimitive);

                return (getBasePosition() == rCompare.getBasePosition()
                    && getRGBColorA() == rCompare.getRGBColorA()
                    && getRGBColorB() == rCompare.getRGBColorB()
                    && getDiscreteDashLength() == rCompare.getDiscreteDashLength());
            }

            return false;
        }

        ImplPrimitrive2DIDBlock(OverlayCrosshairPrimitive, PRIMITIVE2D_ID_OVERLAYCROSSHAIRPRIMITIVE)

    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        OverlayHatchRectanglePrimitive::OverlayHatchRectanglePrimitive(
            const basegfx::B2DRange& rObjectRange,
            double fDiscreteHatchDistance,
            double fHatchRotation,
            const basegfx::BColor& rHatchColor,
            double fDiscreteGrow,
            double fDiscreteShrink,
            double fRotation)
        :   DiscreteMetricDependentPrimitive2D(),
            maObjectRange(rObjectRange),
            mfDiscreteHatchDistance(fDiscreteHatchDistance),
            mfHatchRotation(fHatchRotation),
            maHatchColor(rHatchColor),
            mfDiscreteGrow(fDiscreteGrow),
            mfDiscreteShrink(fDiscreteShrink),
            mfRotation(fRotation)
        {}

        Primitive2DSequence OverlayHatchRectanglePrimitive::create2DDecomposition(const geometry::ViewInformation2D& /*rViewInformation*/) const
        {
            Primitive2DSequence aRetval;

            if(basegfx::fTools::more(getDiscreteUnit(), 0.0))
            {
                basegfx::B2DRange aInnerRange(getObjectRange());
                basegfx::B2DRange aOuterRange(getObjectRange());
                basegfx::B2DPolyPolygon aHatchPolyPolygon;

                aOuterRange.grow(getDiscreteUnit() * getDiscreteGrow());
                aInnerRange.grow(getDiscreteUnit() * -getDiscreteShrink());

                aHatchPolyPolygon.append(basegfx::tools::createPolygonFromRect(aOuterRange));

                if(!aInnerRange.isEmpty())
                {
                    aHatchPolyPolygon.append(basegfx::tools::createPolygonFromRect(aInnerRange));
                }

                if(!basegfx::fTools::equalZero(getRotation()))
                {
                    const basegfx::B2DHomMatrix aTransform(basegfx::tools::createRotateAroundPoint(
                        getObjectRange().getMinX(), getObjectRange().getMinY(), getRotation()));

                    aHatchPolyPolygon.transform(aTransform);
                }

                const basegfx::BColor aEmptyColor(0.0, 0.0, 0.0);
                const drawinglayer::attribute::FillHatchAttribute aFillHatchAttribute(
                    drawinglayer::attribute::HATCHSTYLE_SINGLE,
                    getDiscreteHatchDistance() * getDiscreteUnit(),
                    getHatchRotation() - getRotation(),
                    getHatchColor(),
                    false);
                const Primitive2DReference aPrimitive(
                    new PolyPolygonHatchPrimitive2D(
                        aHatchPolyPolygon,
                        aEmptyColor,
                        aFillHatchAttribute));

                aRetval = Primitive2DSequence(&aPrimitive, 1);
            }

            return aRetval;
        }

        bool OverlayHatchRectanglePrimitive::operator==( const BasePrimitive2D& rPrimitive ) const
        {
            if(DiscreteMetricDependentPrimitive2D::operator==(rPrimitive))
            {
                const OverlayHatchRectanglePrimitive& rCompare = static_cast< const OverlayHatchRectanglePrimitive& >(rPrimitive);

                return (getObjectRange() == rCompare.getObjectRange()
                    && getDiscreteHatchDistance() == rCompare.getDiscreteHatchDistance()
                    && getHatchRotation() == rCompare.getHatchRotation()
                    && getHatchColor() == rCompare.getHatchColor()
                    && getDiscreteGrow() == rCompare.getDiscreteGrow()
                    && getDiscreteShrink() == rCompare.getDiscreteShrink()
                    && getRotation() == rCompare.getRotation());
            }

            return false;
        }

        ImplPrimitrive2DIDBlock(OverlayHatchRectanglePrimitive, PRIMITIVE2D_ID_OVERLAYHATCHRECTANGLEPRIMITIVE)

    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        OverlayHelplineStripedPrimitive::OverlayHelplineStripedPrimitive(
            const basegfx::B2DPoint& rBasePosition,
            HelplineStyle eStyle,
            const basegfx::BColor& rRGBColorA,
            const basegfx::BColor& rRGBColorB,
            double fDiscreteDashLength)
        :   ViewportDependentPrimitive2D(),
            maBasePosition(rBasePosition),
            meStyle(eStyle),
            maRGBColorA(rRGBColorA),
            maRGBColorB(rRGBColorB),
            mfDiscreteDashLength(fDiscreteDashLength)
        {}

        Primitive2DSequence OverlayHelplineStripedPrimitive::create2DDecomposition(const geometry::ViewInformation2D& rViewInformation) const
        {
            // use the prepared Viewport information accessible using getViewport()
            Primitive2DSequence aRetval;

            if(!getViewport().isEmpty())
            {
                switch(getStyle())
                {
                    case HELPLINESTYLE_VERTICAL :
                    {
                        aRetval.realloc(1);
                        basegfx::B2DPolygon aLine;

                        aLine.append(basegfx::B2DPoint(getBasePosition().getX(), getViewport().getMinY()));
                        aLine.append(basegfx::B2DPoint(getBasePosition().getX(), getViewport().getMaxY()));

                        aRetval[0] = Primitive2DReference(
                            new PolygonMarkerPrimitive2D(
                                aLine,
                                getRGBColorA(),
                                getRGBColorB(),
                                getDiscreteDashLength()));
                        break;
                    }

                    case HELPLINESTYLE_HORIZONTAL :
                    {
                        aRetval.realloc(1);
                        basegfx::B2DPolygon aLine;

                        aLine.append(basegfx::B2DPoint(getViewport().getMinX(), getBasePosition().getY()));
                        aLine.append(basegfx::B2DPoint(getViewport().getMaxX(), getBasePosition().getY()));

                        aRetval[0] = Primitive2DReference(
                            new PolygonMarkerPrimitive2D(
                                aLine,
                                getRGBColorA(),
                                getRGBColorB(),
                                getDiscreteDashLength()));
                        break;
                    }

                    default: // case HELPLINESTYLE_POINT :
                    {
                        const double fDiscreteUnit((rViewInformation.getInverseObjectToViewTransformation() * basegfx::B2DVector(1.0, 0.0)).getLength());
                        aRetval.realloc(2);
                        basegfx::B2DPolygon aLineA, aLineB;

                        aLineA.append(basegfx::B2DPoint(getBasePosition().getX(), getBasePosition().getY() - fDiscreteUnit));
                        aLineA.append(basegfx::B2DPoint(getBasePosition().getX(), getBasePosition().getY() + fDiscreteUnit));

                        aRetval[0] = Primitive2DReference(
                            new PolygonMarkerPrimitive2D(
                                aLineA,
                                getRGBColorA(),
                                getRGBColorB(),
                                getDiscreteDashLength()));

                        aLineB.append(basegfx::B2DPoint(getBasePosition().getX() - fDiscreteUnit, getBasePosition().getY()));
                        aLineB.append(basegfx::B2DPoint(getBasePosition().getX() + fDiscreteUnit, getBasePosition().getY()));

                        aRetval[1] = Primitive2DReference(
                            new PolygonMarkerPrimitive2D(
                                aLineB,
                                getRGBColorA(),
                                getRGBColorB(),
                                getDiscreteDashLength()));

                        break;
                    }
                }
            }

            return aRetval;
        }

        bool OverlayHelplineStripedPrimitive::operator==( const BasePrimitive2D& rPrimitive ) const
        {
            if(ViewportDependentPrimitive2D::operator==(rPrimitive))
            {
                const OverlayHelplineStripedPrimitive& rCompare = static_cast< const OverlayHelplineStripedPrimitive& >(rPrimitive);

                return (getBasePosition() == rCompare.getBasePosition()
                    && getStyle() == rCompare.getStyle()
                    && getRGBColorA() == rCompare.getRGBColorA()
                    && getRGBColorB() == rCompare.getRGBColorB()
                    && getDiscreteDashLength() == rCompare.getDiscreteDashLength());
            }

            return false;
        }

        ImplPrimitrive2DIDBlock(OverlayHelplineStripedPrimitive, PRIMITIVE2D_ID_OVERLAYHELPLINESTRIPEDPRIMITIVE)

    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive2d
    {
        OverlayRollingRectanglePrimitive::OverlayRollingRectanglePrimitive(
            const basegfx::B2DRange& aRollingRectangle,
            const basegfx::BColor& rRGBColorA,
            const basegfx::BColor& rRGBColorB,
            double fDiscreteDashLength)
        :   ViewportDependentPrimitive2D(),
            maRollingRectangle(aRollingRectangle),
            maRGBColorA(rRGBColorA),
            maRGBColorB(rRGBColorB),
            mfDiscreteDashLength(fDiscreteDashLength)
        {}

        Primitive2DSequence OverlayRollingRectanglePrimitive::create2DDecomposition(const geometry::ViewInformation2D& /*rViewInformation*/) const
        {
            // use the prepared Viewport information accessible using getViewport()
            Primitive2DSequence aRetval;

            if(!getViewport().isEmpty())
            {
                basegfx::B2DPolygon aLine;
                aRetval.realloc(8);

                // Left lines
                aLine.append(basegfx::B2DPoint(getViewport().getMinX(), getRollingRectangle().getMinY()));
                aLine.append(basegfx::B2DPoint(getRollingRectangle().getMinX(), getRollingRectangle().getMinY()));
                aRetval[0] = Primitive2DReference(new PolygonMarkerPrimitive2D(aLine, getRGBColorA(), getRGBColorB(), getDiscreteDashLength()));

                aLine.clear();
                aLine.append(basegfx::B2DPoint(getViewport().getMinX(), getRollingRectangle().getMaxY()));
                aLine.append(basegfx::B2DPoint(getRollingRectangle().getMinX(), getRollingRectangle().getMaxY()));
                aRetval[1] = Primitive2DReference(new PolygonMarkerPrimitive2D(aLine, getRGBColorA(), getRGBColorB(), getDiscreteDashLength()));

                // Right lines
                aLine.clear();
                aLine.append(basegfx::B2DPoint(getRollingRectangle().getMaxX(), getRollingRectangle().getMinY()));
                aLine.append(basegfx::B2DPoint(getViewport().getMaxX(), getRollingRectangle().getMinY()));
                aRetval[2] = Primitive2DReference(new PolygonMarkerPrimitive2D(aLine, getRGBColorA(), getRGBColorB(), getDiscreteDashLength()));

                aLine.clear();
                aLine.append(basegfx::B2DPoint(getRollingRectangle().getMaxX(), getRollingRectangle().getMaxY()));
                aLine.append(basegfx::B2DPoint(getViewport().getMaxX(), getRollingRectangle().getMaxY()));
                aRetval[3] = Primitive2DReference(new PolygonMarkerPrimitive2D(aLine, getRGBColorA(), getRGBColorB(), getDiscreteDashLength()));

                // Top lines
                aLine.clear();
                aLine.append(basegfx::B2DPoint(getRollingRectangle().getMinX(), getViewport().getMinY()));
                aLine.append(basegfx::B2DPoint(getRollingRectangle().getMinX(), getRollingRectangle().getMinY()));
                aRetval[4] = Primitive2DReference(new PolygonMarkerPrimitive2D(aLine, getRGBColorA(), getRGBColorB(), getDiscreteDashLength()));

                aLine.clear();
                aLine.append(basegfx::B2DPoint(getRollingRectangle().getMaxX(), getViewport().getMinY()));
                aLine.append(basegfx::B2DPoint(getRollingRectangle().getMaxX(), getRollingRectangle().getMinY()));
                aRetval[5] = Primitive2DReference(new PolygonMarkerPrimitive2D(aLine, getRGBColorA(), getRGBColorB(), getDiscreteDashLength()));

                // Bottom lines
                aLine.clear();
                aLine.append(basegfx::B2DPoint(getRollingRectangle().getMinX(), getRollingRectangle().getMaxY()));
                aLine.append(basegfx::B2DPoint(getRollingRectangle().getMinX(), getViewport().getMaxY()));
                aRetval[6] = Primitive2DReference(new PolygonMarkerPrimitive2D(aLine, getRGBColorA(), getRGBColorB(), getDiscreteDashLength()));

                aLine.clear();
                aLine.append(basegfx::B2DPoint(getRollingRectangle().getMaxX(), getRollingRectangle().getMaxY()));
                aLine.append(basegfx::B2DPoint(getRollingRectangle().getMaxX(), getViewport().getMaxY()));
                aRetval[7] = Primitive2DReference(new PolygonMarkerPrimitive2D(aLine, getRGBColorA(), getRGBColorB(), getDiscreteDashLength()));
            }

            return aRetval;
        }

        bool OverlayRollingRectanglePrimitive::operator==( const BasePrimitive2D& rPrimitive ) const
        {
            if(ViewportDependentPrimitive2D::operator==(rPrimitive))
            {
                const OverlayRollingRectanglePrimitive& rCompare = static_cast< const OverlayRollingRectanglePrimitive& >(rPrimitive);

                return (getRollingRectangle() == rCompare.getRollingRectangle()
                    && getRGBColorA() == rCompare.getRGBColorA()
                    && getRGBColorB() == rCompare.getRGBColorB()
                    && getDiscreteDashLength() == rCompare.getDiscreteDashLength());
            }

            return false;
        }

        ImplPrimitrive2DIDBlock(OverlayRollingRectanglePrimitive, PRIMITIVE2D_ID_OVERLAYROLLINGRECTANGLEPRIMITIVE)

    } // end of namespace primitive2d
} // end of namespace drawinglayer

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

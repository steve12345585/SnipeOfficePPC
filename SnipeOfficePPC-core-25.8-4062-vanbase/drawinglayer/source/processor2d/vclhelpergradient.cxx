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

#include <vclhelpergradient.hxx>
#include <basegfx/range/b2drange.hxx>
#include <vcl/outdev.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/polygon/b2dpolypolygontools.hxx>
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <drawinglayer/texture/texture.hxx>

//////////////////////////////////////////////////////////////////////////////
// support methods for vcl direct gradient renderering

namespace drawinglayer
{
    namespace
    {
        sal_uInt32 impCalcGradientSteps(OutputDevice& rOutDev, sal_uInt32 nSteps, const basegfx::B2DRange& rRange, sal_uInt32 nMaxDist)
        {
            if(nSteps == 0L)
            {
                const Size aSize(rOutDev.LogicToPixel(Size(basegfx::fround(rRange.getWidth()), basegfx::fround(rRange.getHeight()))));
                nSteps = (aSize.getWidth() + aSize.getHeight()) >> 3L;
            }

            if(nSteps < 2L)
            {
                nSteps = 2L;
            }

            if(nSteps > nMaxDist)
            {
                nSteps = nMaxDist;
            }

            return nSteps;
        }

        void impDrawGradientToOutDevSimple(
            OutputDevice& rOutDev,
            const basegfx::B2DPolyPolygon& rTargetForm,
            const ::std::vector< basegfx::B2DHomMatrix >& rMatrices,
            const ::std::vector< basegfx::BColor >& rColors,
            const basegfx::B2DPolygon& rUnitPolygon)
        {
            rOutDev.SetLineColor();

            for(sal_uInt32 a(0L); a < rColors.size(); a++)
            {
                // set correct color
                const basegfx::BColor aFillColor(rColors[a]);
                rOutDev.SetFillColor(Color(aFillColor));

                if(a)
                {
                    if(a - 1L < static_cast< sal_uInt32 >(rMatrices.size()))
                    {
                        basegfx::B2DPolygon aNewPoly(rUnitPolygon);
                        aNewPoly.transform(rMatrices[a - 1L]);
                        rOutDev.DrawPolygon(aNewPoly);
                    }
                }
                else
                {
                    rOutDev.DrawPolyPolygon(rTargetForm);
                }
            }
        }

        void impDrawGradientToOutDevComplex(
            OutputDevice& rOutDev,
            const basegfx::B2DPolyPolygon& rTargetForm,
            const ::std::vector< basegfx::B2DHomMatrix >& rMatrices,
            const ::std::vector< basegfx::BColor >& rColors,
            const basegfx::B2DPolygon& rUnitPolygon)
        {
            PolyPolygon aVclTargetForm(rTargetForm);
            ::std::vector< Polygon > aVclPolygons;
            sal_uInt32 a;

            // remember and set to XOR
            rOutDev.SetLineColor();
            rOutDev.Push(PUSH_RASTEROP);
            rOutDev.SetRasterOp(ROP_XOR);

            // draw gradient PolyPolygons
            for(a = 0L; a < rMatrices.size(); a++)
            {
                // create polygon and remember
                basegfx::B2DPolygon aNewPoly(rUnitPolygon);
                aNewPoly.transform(rMatrices[a]);
                aVclPolygons.push_back(Polygon(aNewPoly));

                // set correct color
                if(rColors.size() > a)
                {
                    const basegfx::BColor aFillColor(rColors[a]);
                    rOutDev.SetFillColor(Color(aFillColor));
                }

                // create vcl PolyPolygon and draw it
                if(a)
                {
                    PolyPolygon aVclPolyPoly(aVclPolygons[a - 1L]);
                    aVclPolyPoly.Insert(aVclPolygons[a]);
                    rOutDev.DrawPolyPolygon(aVclPolyPoly);
                }
                else
                {
                    PolyPolygon aVclPolyPoly(aVclTargetForm);
                    aVclPolyPoly.Insert(aVclPolygons[0L]);
                    rOutDev.DrawPolyPolygon(aVclPolyPoly);
                }
            }

            // draw last poly in last color
            if(!rColors.empty())
            {
                const basegfx::BColor aFillColor(rColors[rColors.size() - 1L]);
                rOutDev.SetFillColor(Color(aFillColor));
                rOutDev.DrawPolygon(aVclPolygons[aVclPolygons.size() - 1L]);
            }

            // draw object form in black and go back to XOR
            rOutDev.SetFillColor(COL_BLACK);
            rOutDev.SetRasterOp(ROP_0);
            rOutDev.DrawPolyPolygon(aVclTargetForm);
            rOutDev.SetRasterOp(ROP_XOR);

            // draw gradient PolyPolygons again
            for(a = 0L; a < rMatrices.size(); a++)
            {
                // set correct color
                if(rColors.size() > a)
                {
                    const basegfx::BColor aFillColor(rColors[a]);
                    rOutDev.SetFillColor(Color(aFillColor));
                }

                // create vcl PolyPolygon and draw it
                if(a)
                {
                    PolyPolygon aVclPolyPoly(aVclPolygons[a - 1L]);
                    aVclPolyPoly.Insert(aVclPolygons[a]);
                    rOutDev.DrawPolyPolygon(aVclPolyPoly);
                }
                else
                {
                    PolyPolygon aVclPolyPoly(aVclTargetForm);
                    aVclPolyPoly.Insert(aVclPolygons[0L]);
                    rOutDev.DrawPolyPolygon(aVclPolyPoly);
                }
            }

            // draw last poly in last color
            if(!rColors.empty())
            {
                const basegfx::BColor aFillColor(rColors[rColors.size() - 1L]);
                rOutDev.SetFillColor(Color(aFillColor));
                rOutDev.DrawPolygon(aVclPolygons[aVclPolygons.size() - 1L]);
            }

            // reset drawmode
            rOutDev.Pop();
        }
    } // end of anonymous namespace
} // end of namespace drawinglayer

namespace drawinglayer
{
    void impDrawGradientToOutDev(
        OutputDevice& rOutDev,
        const basegfx::B2DPolyPolygon& rTargetForm,
        attribute::GradientStyle eGradientStyle,
        sal_uInt32 nSteps,
        const basegfx::BColor& rStart,
        const basegfx::BColor& rEnd,
        double fBorder, double fAngle, double fOffsetX, double fOffsetY, bool bSimple)
    {
        const basegfx::B2DRange aOutlineRange(basegfx::tools::getRange(rTargetForm));
        ::std::vector< basegfx::B2DHomMatrix > aMatrices;
        ::std::vector< basegfx::BColor > aColors;
        basegfx::B2DPolygon aUnitPolygon;

        // make sure steps is not too high/low
        nSteps = impCalcGradientSteps(rOutDev, nSteps, aOutlineRange, sal_uInt32((rStart.getMaximumDistance(rEnd) * 127.5) + 0.5));

        // create geometries
        switch(eGradientStyle)
        {
            case attribute::GRADIENTSTYLE_LINEAR:
            {
                texture::GeoTexSvxGradientLinear aGradient(aOutlineRange, rStart, rEnd, nSteps, fBorder, fAngle);
                aGradient.appendTransformations(aMatrices);
                aGradient.appendColors(aColors);
                aUnitPolygon = basegfx::tools::createUnitPolygon();
                break;
            }
            case attribute::GRADIENTSTYLE_AXIAL:
            {
                texture::GeoTexSvxGradientAxial aGradient(aOutlineRange, rStart, rEnd, nSteps, fBorder, fAngle);
                aGradient.appendTransformations(aMatrices);
                aGradient.appendColors(aColors);
                aUnitPolygon = basegfx::tools::createPolygonFromRect(basegfx::B2DRange(-1, -1, 1, 1));
                break;
            }
            case attribute::GRADIENTSTYLE_RADIAL:
            {
                texture::GeoTexSvxGradientRadial aGradient(aOutlineRange, rStart, rEnd, nSteps, fBorder, fOffsetX, fOffsetY);
                aGradient.appendTransformations(aMatrices);
                aGradient.appendColors(aColors);
                aUnitPolygon = basegfx::tools::createPolygonFromCircle(basegfx::B2DPoint(0,0), 1);
                break;
            }
            case attribute::GRADIENTSTYLE_ELLIPTICAL:
            {
                texture::GeoTexSvxGradientElliptical aGradient(aOutlineRange, rStart, rEnd, nSteps, fBorder, fOffsetX, fOffsetX, fAngle);
                aGradient.appendTransformations(aMatrices);
                aGradient.appendColors(aColors);
                aUnitPolygon = basegfx::tools::createPolygonFromCircle(basegfx::B2DPoint(0,0), 1);
                break;
            }
            case attribute::GRADIENTSTYLE_SQUARE:
            {
                texture::GeoTexSvxGradientSquare aGradient(aOutlineRange, rStart, rEnd, nSteps, fBorder, fOffsetX, fOffsetX, fAngle);
                aGradient.appendTransformations(aMatrices);
                aGradient.appendColors(aColors);
                aUnitPolygon = basegfx::tools::createPolygonFromRect(basegfx::B2DRange(-1, -1, 1, 1));
                break;
            }
            case attribute::GRADIENTSTYLE_RECT:
            {
                texture::GeoTexSvxGradientRect aGradient(aOutlineRange, rStart, rEnd, nSteps, fBorder, fOffsetX, fOffsetX, fAngle);
                aGradient.appendTransformations(aMatrices);
                aGradient.appendColors(aColors);
                aUnitPolygon = basegfx::tools::createPolygonFromRect(basegfx::B2DRange(-1, -1, 1, 1));
                break;
            }
        }

        // paint them with mask using the XOR method
        if(!aMatrices.empty())
        {
            if(bSimple)
            {
                impDrawGradientToOutDevSimple(rOutDev, rTargetForm, aMatrices, aColors, aUnitPolygon);
            }
            else
            {
                impDrawGradientToOutDevComplex(rOutDev, rTargetForm, aMatrices, aColors, aUnitPolygon);
            }
        }
    }
} // end of namespace drawinglayer

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

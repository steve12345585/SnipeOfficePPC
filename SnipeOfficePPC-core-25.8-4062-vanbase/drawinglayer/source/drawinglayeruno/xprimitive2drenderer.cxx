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

#include <com/sun/star/graphic/XPrimitive2DRenderer.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/xml/sax/XParser.hpp>
#include <com/sun/star/xml/sax/InputSource.hpp>
#include <comphelper/processfactory.hxx>
#include <drawinglayer/geometry/viewinformation2d.hxx>
#include <basegfx/numeric/ftools.hxx>
#include <vcl/bitmapex.hxx>
#include <drawinglayer/tools/converters.hxx>
#include <vcl/canvastools.hxx>
#include <com/sun/star/geometry/RealRectangle2D.hpp>
#include <basegfx/matrix/b2dhommatrixtools.hxx>
#include <drawinglayer/primitive2d/transformprimitive2d.hxx>

//////////////////////////////////////////////////////////////////////////////

using namespace ::com::sun::star;

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace unorenderer
    {
        class XPrimitive2DRenderer : public ::cppu::WeakAggImplHelper2< graphic::XPrimitive2DRenderer, lang::XServiceInfo >
        {
        private:
            XPrimitive2DRenderer(const XPrimitive2DRenderer&);
            XPrimitive2DRenderer& operator=(const XPrimitive2DRenderer&);

        protected:
        public:
            XPrimitive2DRenderer();
            virtual ~XPrimitive2DRenderer();

            // XPrimitive2DRenderer
            virtual uno::Reference< rendering::XBitmap > SAL_CALL rasterize(
                const uno::Sequence< uno::Reference< graphic::XPrimitive2D > >& Primitive2DSequence,
                const uno::Sequence< beans::PropertyValue >& aViewInformationSequence,
                ::sal_uInt32 DPI_X,
                ::sal_uInt32 DPI_Y,
                const ::com::sun::star::geometry::RealRectangle2D& Range,
                ::sal_uInt32 MaximumQuadraticPixels) throw (uno::RuntimeException);

            // XServiceInfo
            virtual rtl::OUString SAL_CALL getImplementationName() throw(uno::RuntimeException);
            virtual ::sal_Bool SAL_CALL supportsService(const rtl::OUString&) throw(uno::RuntimeException);
            virtual uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames() throw(uno::RuntimeException);
        };
    } // end of namespace unorenderer
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////
// uno functions

namespace drawinglayer
{
    namespace unorenderer
    {
        uno::Sequence< rtl::OUString > XPrimitive2DRenderer_getSupportedServiceNames()
        {
            static rtl::OUString aServiceName(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.graphic.Primitive2DTools" ) );
            static uno::Sequence< rtl::OUString > aServiceNames( &aServiceName, 1 );

            return( aServiceNames );
        }

        rtl::OUString XPrimitive2DRenderer_getImplementationName()
        {
            return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "drawinglayer::unorenderer::XPrimitive2DRenderer" ) );
        }

        uno::Reference< uno::XInterface > SAL_CALL XPrimitive2DRenderer_createInstance(const uno::Reference< lang::XMultiServiceFactory >&)
        {
            return static_cast< ::cppu::OWeakObject* >(new XPrimitive2DRenderer);
        }
    } // end of namespace unorenderer
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace unorenderer
    {
        XPrimitive2DRenderer::XPrimitive2DRenderer()
        {
        }

        XPrimitive2DRenderer::~XPrimitive2DRenderer()
        {
        }

        uno::Reference< rendering::XBitmap > XPrimitive2DRenderer::rasterize(
            const uno::Sequence< uno::Reference< graphic::XPrimitive2D > >& Primitive2DSequence,
            const uno::Sequence< beans::PropertyValue >& aViewInformationSequence,
            ::sal_uInt32 DPI_X,
            ::sal_uInt32 DPI_Y,
            const ::com::sun::star::geometry::RealRectangle2D& Range,
            ::sal_uInt32 MaximumQuadraticPixels) throw (uno::RuntimeException)
        {
            uno::Reference< rendering::XBitmap > XBitmap;

            if(Primitive2DSequence.hasElements())
            {
                const basegfx::B2DRange aRange(Range.X1, Range.Y1, Range.X2, Range.Y2);
                const double fWidth(aRange.getWidth());
                const double fHeight(aRange.getHeight());

                if(basegfx::fTools::more(fWidth, 0.0) && basegfx::fTools::more(fHeight, 0.0))
                {
                    if(0 == DPI_X)
                    {
                        DPI_X = 75;
                    }

                    if(0 == DPI_Y)
                    {
                        DPI_Y = 75;
                    }

                    if(0 == MaximumQuadraticPixels)
                    {
                        MaximumQuadraticPixels = 500000;
                    }

                    const geometry::ViewInformation2D aViewInformation2D(aViewInformationSequence);
                    const double fFactor100th_mmToInch(2.54/1000.0);
                    const sal_uInt32 nDiscreteWidth(basegfx::fround((fWidth * fFactor100th_mmToInch) * DPI_X));
                    const sal_uInt32 nDiscreteHeight(basegfx::fround((fHeight * fFactor100th_mmToInch) * DPI_Y));

                    basegfx::B2DHomMatrix aEmbedding(
                        basegfx::tools::createTranslateB2DHomMatrix(
                            -aRange.getMinX(),
                            -aRange.getMinY()));

                    aEmbedding.scale(
                        nDiscreteWidth / fWidth,
                        nDiscreteHeight / fHeight);

                    const primitive2d::Primitive2DReference xEmbedRef(
                        new primitive2d::TransformPrimitive2D(
                            aEmbedding,
                            Primitive2DSequence));
                    const primitive2d::Primitive2DSequence xEmbedSeq(&xEmbedRef, 1);

                    BitmapEx aBitmapEx(
                        tools::convertToBitmapEx(
                            xEmbedSeq,
                            aViewInformation2D,
                            nDiscreteWidth,
                            nDiscreteHeight,
                            MaximumQuadraticPixels));

                    if(!aBitmapEx.IsEmpty())
                    {
                        const uno::Reference< rendering::XGraphicDevice > xGraphicDevice;

                        aBitmapEx.SetPrefMapMode(MapMode(MAP_100TH_MM));
                        aBitmapEx.SetPrefSize(Size(basegfx::fround(fWidth), basegfx::fround(fHeight)));
                        XBitmap = vcl::unotools::xBitmapFromBitmapEx(xGraphicDevice, aBitmapEx);
                    }
                }
            }

            return XBitmap;
        }

        rtl::OUString SAL_CALL XPrimitive2DRenderer::getImplementationName() throw(uno::RuntimeException)
        {
            return(XPrimitive2DRenderer_getImplementationName());
        }

        sal_Bool SAL_CALL XPrimitive2DRenderer::supportsService(const rtl::OUString& rServiceName) throw(uno::RuntimeException)
        {
            const uno::Sequence< rtl::OUString > aServices(XPrimitive2DRenderer_getSupportedServiceNames());

            for(sal_Int32 nService(0); nService < aServices.getLength(); nService++)
            {
                if(rServiceName == aServices[nService])
                {
                    return sal_True;
                }
            }

            return sal_False;
        }

        uno::Sequence< rtl::OUString > SAL_CALL XPrimitive2DRenderer::getSupportedServiceNames() throw(uno::RuntimeException)
        {
            return XPrimitive2DRenderer_getSupportedServiceNames();
        }

    } // end of namespace unorenderer
} // end of namespace drawinglayer

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#ifndef _NULLCANVAS_CANVASHELPER_HXX_
#define _NULLCANVAS_CANVASHELPER_HXX_

#include <com/sun/star/rendering/XCanvas.hpp>
#include <com/sun/star/rendering/XIntegerBitmap.hpp>

#include <basegfx/vector/b2isize.hxx>
#include <basegfx/vector/b2dsize.hxx>

#include <boost/utility.hpp>


namespace nullcanvas
{
    class SpriteCanvas;

    /** Helper class for basic canvas functionality. Also offers
        optional backbuffer painting, when providing it with a second
        HDC to render into.
     */
    class CanvasHelper : private ::boost::noncopyable
    {
    public:
        CanvasHelper();

        /// Release all references
        void disposing();

        /** Initialize canvas helper

            This method late-initializes the canvas helper, providing
            it with the necessary device and size. Note that the
            CanvasHelper does <em>not</em> take ownership of the
            passed rDevice reference, nor does it perform any
            reference counting. Thus, to prevent the reference counted
            SpriteCanvas object from deletion, the user of this class
            is responsible for holding ref-counted references itself!

            @param rSizePixel
            Size of the output surface in pixel.

            @param rDevice
            Reference device this canvas is associated with

         */
        void init( const ::basegfx::B2ISize& rSizePixel,
                   SpriteCanvas&             rDevice,
                   bool                      bHasAlpha );


        // CanvasHelper functionality
        // ==========================

        // XCanvas (only providing, not implementing the
        // interface. Also note subtle method parameter differences)
        void clear();
        void drawPoint( const ::com::sun::star::rendering::XCanvas*     pCanvas,
                        const ::com::sun::star::geometry::RealPoint2D&  aPoint,
                        const ::com::sun::star::rendering::ViewState&   viewState,
                        const ::com::sun::star::rendering::RenderState& renderState );
        void drawLine( const ::com::sun::star::rendering::XCanvas*      pCanvas,
                       const ::com::sun::star::geometry::RealPoint2D&   aStartPoint,
                       const ::com::sun::star::geometry::RealPoint2D&   aEndPoint,
                       const ::com::sun::star::rendering::ViewState&    viewState,
                       const ::com::sun::star::rendering::RenderState&  renderState );
        void drawBezier( const ::com::sun::star::rendering::XCanvas*            pCanvas,
                         const ::com::sun::star::geometry::RealBezierSegment2D& aBezierSegment,
                         const ::com::sun::star::geometry::RealPoint2D&         aEndPoint,
                         const ::com::sun::star::rendering::ViewState&          viewState,
                         const ::com::sun::star::rendering::RenderState&        renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive >
            drawPolyPolygon( const ::com::sun::star::rendering::XCanvas*            pCanvas,
                             const ::com::sun::star::uno::Reference<
                                     ::com::sun::star::rendering::XPolyPolygon2D >& xPolyPolygon,
                             const ::com::sun::star::rendering::ViewState&          viewState,
                             const ::com::sun::star::rendering::RenderState&        renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive >
            strokePolyPolygon( const ::com::sun::star::rendering::XCanvas*          pCanvas,
                               const ::com::sun::star::uno::Reference<
                                       ::com::sun::star::rendering::XPolyPolygon2D >&   xPolyPolygon,
                               const ::com::sun::star::rendering::ViewState&        viewState,
                               const ::com::sun::star::rendering::RenderState&      renderState,
                               const ::com::sun::star::rendering::StrokeAttributes& strokeAttributes );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive >
            strokeTexturedPolyPolygon( const ::com::sun::star::rendering::XCanvas*          pCanvas,
                                       const ::com::sun::star::uno::Reference<
                                               ::com::sun::star::rendering::XPolyPolygon2D >&   xPolyPolygon,
                                       const ::com::sun::star::rendering::ViewState&        viewState,
                                       const ::com::sun::star::rendering::RenderState&      renderState,
                                       const ::com::sun::star::uno::Sequence<
                                               ::com::sun::star::rendering::Texture >&      textures,
                                       const ::com::sun::star::rendering::StrokeAttributes& strokeAttributes );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive >
            strokeTextureMappedPolyPolygon( const ::com::sun::star::rendering::XCanvas*             pCanvas,
                                            const ::com::sun::star::uno::Reference<
                                                    ::com::sun::star::rendering::XPolyPolygon2D >&  xPolyPolygon,
                                            const ::com::sun::star::rendering::ViewState&           viewState,
                                            const ::com::sun::star::rendering::RenderState&         renderState,
                                            const ::com::sun::star::uno::Sequence<
                                                    ::com::sun::star::rendering::Texture >&         textures,
                                            const ::com::sun::star::uno::Reference<
                                                    ::com::sun::star::geometry::XMapping2D >&       xMapping,
                                            const ::com::sun::star::rendering::StrokeAttributes&    strokeAttributes );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XPolyPolygon2D >
            queryStrokeShapes( const ::com::sun::star::rendering::XCanvas*          pCanvas,
                               const ::com::sun::star::uno::Reference<
                                       ::com::sun::star::rendering::XPolyPolygon2D >&   xPolyPolygon,
                               const ::com::sun::star::rendering::ViewState&        viewState,
                               const ::com::sun::star::rendering::RenderState&      renderState,
                               const ::com::sun::star::rendering::StrokeAttributes& strokeAttributes );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive >
            fillPolyPolygon( const ::com::sun::star::rendering::XCanvas*            pCanvas,
                             const ::com::sun::star::uno::Reference<
                                     ::com::sun::star::rendering::XPolyPolygon2D >&     xPolyPolygon,
                             const ::com::sun::star::rendering::ViewState&          viewState,
                             const ::com::sun::star::rendering::RenderState&        renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive >
            fillTexturedPolyPolygon( const ::com::sun::star::rendering::XCanvas*            pCanvas,
                                     const ::com::sun::star::uno::Reference<
                                             ::com::sun::star::rendering::XPolyPolygon2D >&     xPolyPolygon,
                                     const ::com::sun::star::rendering::ViewState&          viewState,
                                     const ::com::sun::star::rendering::RenderState&        renderState,
                                     const ::com::sun::star::uno::Sequence<
                                             ::com::sun::star::rendering::Texture >&        textures );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive >
            fillTextureMappedPolyPolygon( const ::com::sun::star::rendering::XCanvas*           pCanvas,
                                          const ::com::sun::star::uno::Reference<
                                                  ::com::sun::star::rendering::XPolyPolygon2D >&    xPolyPolygon,
                                          const ::com::sun::star::rendering::ViewState&         viewState,
                                          const ::com::sun::star::rendering::RenderState&       renderState,
                                          const ::com::sun::star::uno::Sequence<
                                                  ::com::sun::star::rendering::Texture >&       textures,
                                          const ::com::sun::star::uno::Reference<
                                                  ::com::sun::star::geometry::XMapping2D >&         xMapping );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCanvasFont > SAL_CALL
            createFont( const ::com::sun::star::rendering::XCanvas*             pCanvas,
                        const ::com::sun::star::rendering::FontRequest&         fontRequest,
                        const ::com::sun::star::uno::Sequence<
                                ::com::sun::star::beans::PropertyValue >&       extraFontProperties,
                        const ::com::sun::star::geometry::Matrix2D&             fontMatrix );

        ::com::sun::star::uno::Sequence< ::com::sun::star::rendering::FontInfo >
            queryAvailableFonts( const ::com::sun::star::rendering::XCanvas*        pCanvas,
                                 const ::com::sun::star::rendering::FontInfo&       aFilter,
                                 const ::com::sun::star::uno::Sequence<
                                         ::com::sun::star::beans::PropertyValue >&  aFontProperties );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive >
            drawText( const ::com::sun::star::rendering::XCanvas*           pCanvas,
                      const ::com::sun::star::rendering::StringContext&     text,
                      const ::com::sun::star::uno::Reference<
                              ::com::sun::star::rendering::XCanvasFont >&   xFont,
                      const ::com::sun::star::rendering::ViewState&         viewState,
                      const ::com::sun::star::rendering::RenderState&       renderState,
                      sal_Int8                                              textDirection );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive >
            drawTextLayout( const ::com::sun::star::rendering::XCanvas*     pCanvas,
                            const ::com::sun::star::uno::Reference<
                                ::com::sun::star::rendering::XTextLayout >& layoutetText,
                            const ::com::sun::star::rendering::ViewState&   viewState,
                            const ::com::sun::star::rendering::RenderState& renderState );

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive >
            drawBitmap( const ::com::sun::star::rendering::XCanvas*     pCanvas,
                        const ::com::sun::star::uno::Reference<
                                ::com::sun::star::rendering::XBitmap >& xBitmap,
                        const ::com::sun::star::rendering::ViewState&   viewState,
                        const ::com::sun::star::rendering::RenderState& renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XCachedPrimitive >
            drawBitmapModulated( const ::com::sun::star::rendering::XCanvas*        pCanvas,
                                 const ::com::sun::star::uno::Reference<
                                         ::com::sun::star::rendering::XBitmap >&        xBitmap,
                                 const ::com::sun::star::rendering::ViewState&      viewState,
                                 const ::com::sun::star::rendering::RenderState&    renderState );
        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XGraphicDevice >
            getDevice();

        // BitmapCanvasHelper functionality
        // ================================

        void copyRect( const ::com::sun::star::rendering::XCanvas*          pCanvas,
                       const ::com::sun::star::uno::Reference<
                               ::com::sun::star::rendering::XBitmapCanvas >&    sourceCanvas,
                       const ::com::sun::star::geometry::RealRectangle2D&   sourceRect,
                       const ::com::sun::star::rendering::ViewState&        sourceViewState,
                       const ::com::sun::star::rendering::RenderState&      sourceRenderState,
                       const ::com::sun::star::geometry::RealRectangle2D&   destRect,
                       const ::com::sun::star::rendering::ViewState&        destViewState,
                       const ::com::sun::star::rendering::RenderState&      destRenderState );

        ::com::sun::star::geometry::IntegerSize2D getSize();

        ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XBitmap >
            getScaledBitmap( const ::com::sun::star::geometry::RealSize2D&  newSize,
                             sal_Bool                                       beFast );

        ::com::sun::star::uno::Sequence< sal_Int8 >
            getData( ::com::sun::star::rendering::IntegerBitmapLayout&      bitmapLayout,
                     const ::com::sun::star::geometry::IntegerRectangle2D&  rect );

        void setData( const ::com::sun::star::uno::Sequence< sal_Int8 >&         data,
                      const ::com::sun::star::rendering::IntegerBitmapLayout&    bitmapLayout,
                      const ::com::sun::star::geometry::IntegerRectangle2D&      rect );

        void setPixel( const ::com::sun::star::uno::Sequence< sal_Int8 >&        color,
                       const ::com::sun::star::rendering::IntegerBitmapLayout&   bitmapLayout,
                       const ::com::sun::star::geometry::IntegerPoint2D&         pos );

        ::com::sun::star::uno::Sequence< sal_Int8 >
            getPixel( ::com::sun::star::rendering::IntegerBitmapLayout& bitmapLayout,
                      const ::com::sun::star::geometry::IntegerPoint2D& pos );

        ::com::sun::star::rendering::IntegerBitmapLayout getMemoryLayout();

        /** Called from XCanvas base classes, to notify that content
            is _about_ to change
        */
        void modifying() {}

        bool hasAlpha() const;

    protected:
        /** Phyical output device

            Deliberately not a refcounted reference, because of
            potential circular references for spritecanvas.
         */
        SpriteCanvas*                   mpDevice;

    private:
        /// Current size of the output surface in pixel
        ::basegfx::B2ISize              maSize;

        /// When true, content is able to represent alpha
        bool                            mbHaveAlpha;
    };
}

#endif /* _NULLCANVAS_CANVASHELPER_HXX_ */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

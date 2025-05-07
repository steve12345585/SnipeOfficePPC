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

#ifndef _DXCANVAS_SPRITECANVASHELPER_HXX_
#define _DXCANVAS_SPRITECANVASHELPER_HXX_

#include <com/sun/star/rendering/XSpriteCanvas.hpp>
#include <com/sun/star/rendering/XIntegerBitmap.hpp>

#include <canvas/spriteredrawmanager.hxx>
#include <canvas/rendering/isurfaceproxy.hxx>
#include <canvas/rendering/isurfaceproxymanager.hxx>

#include "dx_bitmapcanvashelper.hxx"
#include "dx_impltools.hxx"
#include "dx_rendermodule.hxx"
#include "dx_surfacebitmap.hxx"

#include <basegfx/range/b2irectangle.hxx>

namespace dxcanvas
{
    class SpriteCanvas;

    class SpriteCanvasHelper : public BitmapCanvasHelper
    {
    public:
        SpriteCanvasHelper();

        void init( SpriteCanvas&                                    rParent,
                   ::canvas::SpriteRedrawManager&                   rManager,
                   const IDXRenderModuleSharedPtr&                  rRenderModule,
                   const ::canvas::ISurfaceProxyManagerSharedPtr&   rSurfaceProxy,
                   const DXSurfaceBitmapSharedPtr&                  rBackBuffer,
                   const ::basegfx::B2ISize&                        rOutputOffset );

        /// Dispose all internal references
        void disposing();

        // XSpriteCanvas
        ::com::sun::star::uno::Reference<
              ::com::sun::star::rendering::XAnimatedSprite >        createSpriteFromAnimation(
                const ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XAnimation >& animation );

        ::com::sun::star::uno::Reference<
              ::com::sun::star::rendering::XAnimatedSprite >        createSpriteFromBitmaps(
                const ::com::sun::star::uno::Sequence<
                      ::com::sun::star::uno::Reference<
                            ::com::sun::star::rendering::XBitmap > >& animationBitmaps,
                sal_Int8                                              interpolationMode );

        ::com::sun::star::uno::Reference<
              ::com::sun::star::rendering::XCustomSprite >      createCustomSprite(
                const ::com::sun::star::geometry::RealSize2D& spriteSize );

        ::com::sun::star::uno::Reference<
              ::com::sun::star::rendering::XSprite >                createClonedSprite(
                const ::com::sun::star::uno::Reference< ::com::sun::star::rendering::XSprite >& original );

        /** Actually perform the screen update

            @param rCurrArea
            Current window area in absolute screen coordinates

            @param bUpdateAll
            sal_True, if everything must be updated, not only changed
            sprites

            @param io_bSurfaceDirty
            In/out parameter, whether backbuffer surface is dirty (if
            yes, we're performing a full update, anyway)
         */
        sal_Bool updateScreen( const ::basegfx::B2IRectangle& rCurrArea,
                               sal_Bool                       bUpdateAll,
                               bool&                          io_bSurfaceDirty );


        // SpriteRedrawManager functor calls
        // -------------------------------------------------

        /** Gets called for simple background repaints
         */
        void backgroundPaint( const ::basegfx::B2DRange& rUpdateRect );

        /** Gets called when area can be handled by scrolling.

            Called method must copy screen content from rMoveStart to
            rMoveEnd, and restore the background in the uncovered
            areas.

            @param rMoveStart
            Source rect of the scroll

            @param rMoveEnd
            Dest rect of the scroll

            @param rUpdateArea
            All info necessary, should rMoveStart be partially or
            fully outside the outdev
         */
        void scrollUpdate( const ::basegfx::B2DRange&                           rMoveStart,
                           const ::basegfx::B2DRange&                           rMoveEnd,
                           const ::canvas::SpriteRedrawManager::UpdateArea&     rUpdateArea );

        void opaqueUpdate( const ::basegfx::B2DRange&                          rTotalArea,
                           const ::std::vector< ::canvas::Sprite::Reference >& rSortedUpdateSprites );

        void genericUpdate( const ::basegfx::B2DRange&                          rTotalArea,
                            const ::std::vector< ::canvas::Sprite::Reference >& rSortedUpdateSprites );

    private:
        /// For generating sprites
        SpriteCanvas*                                   mpSpriteSurface;

        /// Set from the SpriteCanvas: instance coordinating sprite redraw
        ::canvas::SpriteRedrawManager*                  mpRedrawManager;

        /// DX device, handling all low-level rendering
        IDXRenderModuleSharedPtr                        mpRenderModule;

        ::canvas::ISurfaceProxyManagerSharedPtr         mpSurfaceProxy;

        /// Backbuffer, contains the static canvas render output
        DXSurfaceBitmapSharedPtr                        mpBackBuffer;

        /// Completely temporary rect storage (used by sprite repaint)
        mutable ::basegfx::B2IRange                     maUpdateRect;

        /// Completely temporary rect storage (used by sprite repaint)
        mutable ::basegfx::B2IRange                     maScrapRect;

        /// When true, show small bound rects around each sprite
        bool                                            mbShowSpriteBounds;
    };
}

#endif /* _DXCANVAS_SPRITECANVASHELPER_HXX_ */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

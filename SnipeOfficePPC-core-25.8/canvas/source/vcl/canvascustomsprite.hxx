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

#ifndef _VCLCANVAS_CANVASCUSTOMSPRITE_HXX
#define _VCLCANVAS_CANVASCUSTOMSPRITE_HXX

#include <cppuhelper/compbase4.hxx>
#include <comphelper/uno3.hxx>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/rendering/XCustomSprite.hpp>
#include <com/sun/star/rendering/XIntegerBitmap.hpp>
#include <com/sun/star/rendering/XPolyPolygon2D.hpp>

#include <vcl/virdev.hxx>

#include <canvas/vclwrapper.hxx>
#include <canvas/base/disambiguationhelper.hxx>
#include <canvas/base/spritesurface.hxx>
#include <canvas/base/canvascustomspritebase.hxx>

#include "sprite.hxx"
#include "canvashelper.hxx"
#include "spritehelper.hxx"
#include "backbuffer.hxx"
#include "impltools.hxx"
#include "spritecanvas.hxx"
#include "repainttarget.hxx"


namespace vclcanvas
{
    typedef ::cppu::WeakComponentImplHelper4< ::com::sun::star::rendering::XCustomSprite,
                                               ::com::sun::star::rendering::XBitmapCanvas,
                                              ::com::sun::star::rendering::XIntegerBitmap,
                                                ::com::sun::star::lang::XServiceInfo >  CanvasCustomSpriteBase_Base;
    /** Mixin Sprite

        Have to mixin the Sprite interface before deriving from
        ::canvas::CanvasCustomSpriteBase, as this template should
        already implement some of those interface methods.

        The reason why this appears kinda convoluted is the fact that
        we cannot specify non-IDL types as WeakComponentImplHelperN
        template args, and furthermore, don't want to derive
        ::canvas::CanvasCustomSpriteBase directly from
        ::canvas::Sprite (because derivees of
        ::canvas::CanvasCustomSpriteBase have to explicitly forward
        the XInterface methods (e.g. via DECLARE_UNO3_AGG_DEFAULTS)
        anyway). Basically, ::canvas::CanvasCustomSpriteBase should
        remain a base class that provides implementation, not to
        enforce any specific interface on its derivees.
     */
    class CanvasCustomSpriteSpriteBase_Base : public ::canvas::DisambiguationHelper< CanvasCustomSpriteBase_Base >,
                                                 public Sprite
    {
    };

    typedef ::canvas::CanvasCustomSpriteBase< CanvasCustomSpriteSpriteBase_Base,
                                              SpriteHelper,
                                              CanvasHelper,
                                              tools::LocalGuard,
                                              ::cppu::OWeakObject >                     CanvasCustomSpriteBaseT;

    /* Definition of CanvasCustomSprite class */

    class CanvasCustomSprite : public CanvasCustomSpriteBaseT,
                               public RepaintTarget
    {
    public:
        CanvasCustomSprite( const ::com::sun::star::geometry::RealSize2D& rSpriteSize,
                            ::com::sun::star::rendering::XGraphicDevice&  rDevice,
                            const ::canvas::SpriteSurface::Reference&     rOwningSpriteCanvas,
                            const OutDevProviderSharedPtr&                rOutDevProvider,
                            bool                                          bShowSpriteBounds );

        // Forwarding the XComponent implementation to the
        // cppu::ImplHelper templated base
        //                                    Classname           Base doing refcount          Base implementing the XComponent interface
        //                                          |                    |                         |
        //                                          V                    V                         V
        DECLARE_UNO3_XCOMPONENT_AGG_DEFAULTS( CanvasCustomSprite, CanvasCustomSpriteBase_Base, ::cppu::WeakComponentImplHelperBase );

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
        virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw( ::com::sun::star::uno::RuntimeException );
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()  throw( ::com::sun::star::uno::RuntimeException );

        // Sprite
        virtual void redraw( OutputDevice& rOutDev,
                             bool          bBufferedUpdate ) const;
        virtual void redraw( OutputDevice&              rOutDev,
                             const ::basegfx::B2DPoint& rPos,
                             bool                       bBufferedUpdate ) const;

        // RepaintTarget
        virtual bool repaint( const GraphicObjectSharedPtr&                   rGrf,
                              const ::com::sun::star::rendering::ViewState&   viewState,
                              const ::com::sun::star::rendering::RenderState& renderState,
                              const ::Point&                                  rPt,
                              const ::Size&                                   rSz,
                              const GraphicAttr&                              rAttr ) const;
    };
}

#endif /* _VCLCANVAS_CANVASCUSTOMSPRITE_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#ifndef _DXCANVAS_SPRITECANVAS_HXX_
#define _DXCANVAS_SPRITECANVAS_HXX_

#include <rtl/ref.hxx>

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XServiceName.hpp>
#include <com/sun/star/awt/XWindowListener.hpp>
#include <com/sun/star/util/XUpdatable.hpp>
#include <com/sun/star/rendering/XSpriteCanvas.hpp>
#include <com/sun/star/rendering/XIntegerBitmap.hpp>
#include <com/sun/star/rendering/XGraphicDevice.hpp>
#include <com/sun/star/rendering/XBufferController.hpp>

#include <cppuhelper/compbase9.hxx>
#include <comphelper/uno3.hxx>

#include <canvas/base/spritecanvasbase.hxx>
#include <canvas/base/disambiguationhelper.hxx>
#include <canvas/base/bufferedgraphicdevicebase.hxx>

#include "dx_bitmapprovider.hxx"
#include "dx_spritecanvashelper.hxx"
#include "dx_surfacebitmap.hxx"
#include "dx_impltools.hxx"
#include "dx_spritedevicehelper.hxx"


namespace dxcanvas
{
    typedef ::cppu::WeakComponentImplHelper9< ::com::sun::star::rendering::XSpriteCanvas,
                                              ::com::sun::star::rendering::XIntegerBitmap,
                                              ::com::sun::star::rendering::XGraphicDevice,
                                              ::com::sun::star::lang::XMultiServiceFactory,
                                              ::com::sun::star::rendering::XBufferController,
                                              ::com::sun::star::awt::XWindowListener,
                                              ::com::sun::star::util::XUpdatable,
                                              ::com::sun::star::beans::XPropertySet,
                                              ::com::sun::star::lang::XServiceName >    WindowGraphicDeviceBase_Base;
    typedef ::canvas::BufferedGraphicDeviceBase< ::canvas::DisambiguationHelper< WindowGraphicDeviceBase_Base >,
                                                   SpriteDeviceHelper,
                                                   ::osl::MutexGuard,
                                                   ::cppu::OWeakObject >    SpriteCanvasBase_Base;
    /** Mixin SpriteSurface

        Have to mixin the SpriteSurface before deriving from
        ::canvas::SpriteCanvasBase, as this template should already
        implement some of those interface methods.

        The reason why this appears kinda convoluted is the fact that
        we cannot specify non-IDL types as WeakComponentImplHelperN
        template args, and furthermore, don't want to derive
        ::canvas::SpriteCanvasBase directly from
        ::canvas::SpriteSurface (because derivees of
        ::canvas::SpriteCanvasBase have to explicitly forward the
        XInterface methods (e.g. via DECLARE_UNO3_AGG_DEFAULTS)
        anyway). Basically, ::canvas::CanvasCustomSpriteBase should
        remain a base class that provides implementation, not to
        enforce any specific interface on its derivees.
     */
    class SpriteCanvasBaseSpriteSurface_Base : public SpriteCanvasBase_Base,
                                               public ::canvas::SpriteSurface
    {
    };

    typedef ::canvas::SpriteCanvasBase< SpriteCanvasBaseSpriteSurface_Base,
                                        SpriteCanvasHelper,
                                        ::osl::MutexGuard,
                                        ::cppu::OWeakObject >           SpriteCanvasBaseT;

    /** Product of this component's factory.

        The SpriteCanvas object combines the actual Window canvas with
        the XGraphicDevice interface. This is because there's a
        one-to-one relation between them, anyway, since each window
        can have exactly one canvas and one associated
        XGraphicDevice. And to avoid messing around with circular
        references, this is implemented as one single object.
     */
    class SpriteCanvas : public SpriteCanvasBaseT, public BitmapProvider
    {
    public:
        SpriteCanvas( const ::com::sun::star::uno::Sequence<
                            ::com::sun::star::uno::Any >&               aArguments,
                      const ::com::sun::star::uno::Reference<
                            ::com::sun::star::uno::XComponentContext >& rxContext );

        void initialize();

        /// Dispose all internal references
        virtual void disposeThis();

        // Forwarding the XComponent implementation to the
        // cppu::ImplHelper templated base
        //                                    Classname     Base doing refcounting        Base implementing the XComponent interface
        //                                       |                 |                            |
        //                                       V                 V                            V
        DECLARE_UNO3_XCOMPONENT_AGG_DEFAULTS( SpriteCanvas, WindowGraphicDeviceBase_Base, ::cppu::WeakComponentImplHelperBase );

        // XBufferController (partial)
        virtual ::sal_Bool SAL_CALL showBuffer( ::sal_Bool bUpdateAll ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::sal_Bool SAL_CALL switchBuffer( ::sal_Bool bUpdateAll ) throw (::com::sun::star::uno::RuntimeException);

        // XSpriteCanvas (partial)
        virtual sal_Bool SAL_CALL updateScreen( sal_Bool bUpdateAll ) throw (::com::sun::star::uno::RuntimeException);

        // XServiceName
        virtual ::rtl::OUString SAL_CALL getServiceName(  ) throw (::com::sun::star::uno::RuntimeException);

        /// Retrieve rendermodule object for this Canvas
        const IDXRenderModuleSharedPtr& getRenderModule() const;

        /// Get backbuffer for this canvas
        const DXSurfaceBitmapSharedPtr& getBackBuffer() const;

        // BitmapProvider
        virtual IBitmapSharedPtr getBitmap() const;

     private:
        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > maArguments;
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > mxComponentContext;
    };

    typedef ::rtl::Reference< SpriteCanvas > SpriteCanvasRef;
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

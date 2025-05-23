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

#ifndef _DXCANVAS_DXSURFACEBITMAP_HXX
#define _DXCANVAS_DXSURFACEBITMAP_HXX

#include <canvas/rendering/isurfaceproxy.hxx>
#include <canvas/rendering/isurfaceproxymanager.hxx>
#include "dx_ibitmap.hxx"
#include "dx_canvasfont.hxx" //winstuff
#include "dx_gdiplususer.hxx"
#include "dx_rendermodule.hxx"

namespace dxcanvas
{
    class DXSurfaceBitmap : public IBitmap
    {
    public:
        DXSurfaceBitmap( const ::basegfx::B2IVector&                    rSize,
                         const canvas::ISurfaceProxyManagerSharedPtr&   rMgr,
                         const IDXRenderModuleSharedPtr&                rRenderModule,
                         bool                                           bWithAlpha );

        bool resize( const ::basegfx::B2IVector& rSize );
        void clear();

        virtual GraphicsSharedPtr         getGraphics();

        virtual BitmapSharedPtr           getBitmap() const;
        virtual ::basegfx::B2IVector      getSize() const;
        virtual bool                      hasAlpha() const;

        COMReference<surface_type>              getSurface() const { return mpSurface; }

        bool draw( double                           fAlpha,
                   const ::basegfx::B2DPoint&       rPos,
                   const ::basegfx::B2DHomMatrix&   rTransform );

        bool draw( const ::basegfx::B2IRange&       rArea );

        bool draw( double                           fAlpha,
                   const ::basegfx::B2DPoint&       rPos,
                   const ::basegfx::B2DRange&       rArea,
                   const ::basegfx::B2DHomMatrix&   rTransform );

        bool draw( double                           fAlpha,
                   const ::basegfx::B2DPoint&       rPos,
                   const ::basegfx::B2DPolyPolygon& rClipPoly,
                   const ::basegfx::B2DHomMatrix&   rTransform );

        virtual ::com::sun::star::uno::Sequence< sal_Int8 > getData(
            ::com::sun::star::rendering::IntegerBitmapLayout&       bitmapLayout,
            const ::com::sun::star::geometry::IntegerRectangle2D&   rect );

        virtual void setData(
            const ::com::sun::star::uno::Sequence< sal_Int8 >&      data,
            const ::com::sun::star::rendering::IntegerBitmapLayout& bitmapLayout,
            const ::com::sun::star::geometry::IntegerRectangle2D&   rect );

        virtual void setPixel(
            const ::com::sun::star::uno::Sequence< sal_Int8 >&      color,
            const ::com::sun::star::rendering::IntegerBitmapLayout& bitmapLayout,
            const ::com::sun::star::geometry::IntegerPoint2D&       pos );

        virtual ::com::sun::star::uno::Sequence< sal_Int8 > getPixel(
            ::com::sun::star::rendering::IntegerBitmapLayout&       bitmapLayout,
            const ::com::sun::star::geometry::IntegerPoint2D&       pos );

#ifdef DX_DEBUG_IMAGES
        void imageDebugger();
#endif
    private:
        void init();

        // Refcounted global GDI+ state container
        GDIPlusUserSharedPtr mpGdiPlusUser;

        // size of this image in pixels [integral unit]
        ::basegfx::B2IVector maSize;

        // pointer to the rendermodule, needed to create surfaces
        // which are used as container for the actual pixel data.
        // generally we could use any kind of storage, but GDI+
        // is not willing to render antialiased fonts unless we
        // use this special kind of container, don't ask me why...
        IDXRenderModuleSharedPtr mpRenderModule;

        // pointer to the surface manager, needed in case clients
        // want to resize the bitmap.
        canvas::ISurfaceProxyManagerSharedPtr mpSurfaceManager;

        // access point to the surface proxy which handles
        // the hardware-dependent rendering stuff.
        canvas::ISurfaceProxySharedPtr mpSurfaceProxy;

        // container for pixel data, we need to use a directx
        // surface since GDI+ sucks...
        COMReference<surface_type> mpSurface;

        // since GDI+ does not work correctly in case we
        // run on a 16bit display [don't ask me why] we need
        // to occasionally render to a native GDI+ bitmap.
        BitmapSharedPtr mpGDIPlusBitmap;
        // Graphics for the mpGDIPlusBitmap
        GraphicsSharedPtr mpGraphics;

        // internal implementation of the iColorBuffer interface
        canvas::IColorBufferSharedPtr mpColorBuffer;

        // indicates whether the associated surface needs
        // to refresh its contents or not. in other words,
        // this flag is set iff both representations are
        // out of sync.
        mutable bool mbIsSurfaceDirty;

        // true if the bitmap contains an alpha channel
        bool mbAlpha;
    };

    typedef ::boost::shared_ptr< DXSurfaceBitmap > DXSurfaceBitmapSharedPtr;
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

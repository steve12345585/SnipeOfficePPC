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

#ifndef _NULLCANVAS_CANVASBITMAP_HXX
#define _NULLCANVAS_CANVASBITMAP_HXX

#include <cppuhelper/compbase3.hxx>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/rendering/XBitmapCanvas.hpp>
#include <com/sun/star/rendering/XIntegerBitmap.hpp>

#include <basegfx/vector/b2isize.hxx>

#include <boost/shared_ptr.hpp>

#include <canvas/base/integerbitmapbase.hxx>
#include <canvas/base/disambiguationhelper.hxx>

#include "null_canvashelper.hxx"
#include "null_spritecanvas.hxx"
#include "null_usagecounter.hxx"


/* Definition of CanvasBitmap class */

namespace nullcanvas
{
    typedef ::cppu::WeakComponentImplHelper3< ::com::sun::star::rendering::XBitmapCanvas,
                                              ::com::sun::star::rendering::XIntegerBitmap,
                                               ::com::sun::star::lang::XServiceInfo >   CanvasBitmapBase_Base;
    typedef ::canvas::IntegerBitmapBase< ::canvas::DisambiguationHelper< CanvasBitmapBase_Base >,
                                         CanvasHelper,
                                         ::osl::MutexGuard,
                                         ::cppu::OWeakObject >                          CanvasBitmap_Base;

    class CanvasBitmap : public CanvasBitmap_Base,
                         private UsageCounter< CanvasBitmap >
    {
    public:
        /** Create a canvas bitmap for the given surface

            @param rSize
            Size of the bitmap

            @param rDevice
            Reference device, with which bitmap should be compatible
         */
        CanvasBitmap( const ::basegfx::B2ISize& rSize,
                      const DeviceRef&          rDevice,
                      bool                      bHasAlpha );

        /// Dispose all internal references
        virtual void disposeThis();

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw (::com::sun::star::uno::RuntimeException);

    private:
        /** MUST hold here, too, since CanvasHelper only contains a
            raw pointer (without refcounting)
        */
        DeviceRef               mpDevice;
    };
}

#endif /* _NULLCANVAS_CANVASBITMAP_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

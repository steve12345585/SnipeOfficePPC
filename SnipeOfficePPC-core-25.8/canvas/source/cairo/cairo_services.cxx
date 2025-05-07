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

#include <canvas/debug.hxx>
#include <canvas/verbosetrace.hxx>
#include <canvas/canvastools.hxx>

#include <com/sun/star/registry/XRegistryKey.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <cppuhelper/factory.hxx>
#include <cppuhelper/implementationentry.hxx>
#include <comphelper/servicedecl.hxx>

#include <algorithm>

#include "cairo_canvas.hxx"
#include "cairo_spritecanvas.hxx"


using namespace ::com::sun::star;

#if defined(WNT) || defined (MACOSX)
#  error "The cairo canvas should not be enabled on Windows or Mac cf fdo#46901"
#endif

namespace cairocanvas
{
    static uno::Reference<uno::XInterface> initCanvas( Canvas* pCanvas )
    {
        uno::Reference<uno::XInterface> xRet(static_cast<cppu::OWeakObject*>(pCanvas));
        pCanvas->initialize();
        return xRet;
    }

    namespace sdecl = comphelper::service_decl;
    sdecl::class_< Canvas, sdecl::with_args<true> > serviceImpl1(&initCanvas);
    const sdecl::ServiceDecl cairoCanvasDecl(
        serviceImpl1,
        CANVAS_IMPLEMENTATION_NAME,
        CANVAS_SERVICE_NAME );

    static uno::Reference<uno::XInterface> initSpriteCanvas( SpriteCanvas* pCanvas )
    {
        uno::Reference<uno::XInterface> xRet(static_cast<cppu::OWeakObject*>(pCanvas));
        pCanvas->initialize();
        return xRet;
    }

    namespace sdecl = comphelper::service_decl;
    sdecl::class_< SpriteCanvas, sdecl::with_args<true> > serviceImpl2(&initSpriteCanvas);
    const sdecl::ServiceDecl cairoSpriteCanvasDecl(
        serviceImpl2,
        SPRITECANVAS_IMPLEMENTATION_NAME,
        SPRITECANVAS_SERVICE_NAME );
}

// The C shared lib entry points
COMPHELPER_SERVICEDECL_EXPORTS2(cairocanvas, cairocanvas::cairoCanvasDecl, cairocanvas::cairoSpriteCanvasDecl)

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

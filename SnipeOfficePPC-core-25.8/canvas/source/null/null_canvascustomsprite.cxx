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
#include <tools/diagnose_ex.h>
#include <canvas/verbosetrace.hxx>

#include <rtl/logfile.hxx>
#include <rtl/math.hxx>

#include <canvas/canvastools.hxx>

#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/point/b2dpoint.hxx>

#include "null_canvascustomsprite.hxx"
#include "null_spritecanvas.hxx"


using namespace ::com::sun::star;

namespace nullcanvas
{
    CanvasCustomSprite::CanvasCustomSprite( const ::com::sun::star::geometry::RealSize2D&   rSpriteSize,
                                            const SpriteCanvasRef&                          rRefDevice ) :
        mpSpriteCanvas( rRefDevice )
    {
        ENSURE_OR_THROW( rRefDevice.get(),
                          "CanvasCustomSprite::CanvasCustomSprite(): Invalid sprite canvas" );

        maCanvasHelper.init( ::basegfx::B2ISize(
                                 ::canvas::tools::roundUp( rSpriteSize.Width ),
                                 ::canvas::tools::roundUp( rSpriteSize.Height ) ),
                             *rRefDevice.get(),
                             true );

        maSpriteHelper.init( rSpriteSize,
                             rRefDevice );
    }

    void CanvasCustomSprite::disposeThis()
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        mpSpriteCanvas.clear();

        // forward to parent
        CanvasCustomSpriteBaseT::disposeThis();
    }

    void CanvasCustomSprite::redraw() const
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        maSpriteHelper.redraw( mbSurfaceDirty );
    }

#define IMPLEMENTATION_NAME "NullCanvas.CanvasCustomSprite"
#define SERVICE_NAME "com.sun.star.rendering.CanvasCustomSprite"

    ::rtl::OUString SAL_CALL CanvasCustomSprite::getImplementationName() throw( uno::RuntimeException )
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( IMPLEMENTATION_NAME ) );
    }

    sal_Bool SAL_CALL CanvasCustomSprite::supportsService( const ::rtl::OUString& ServiceName ) throw( uno::RuntimeException )
    {
        return ServiceName == SERVICE_NAME;
    }

    uno::Sequence< ::rtl::OUString > SAL_CALL CanvasCustomSprite::getSupportedServiceNames()  throw( uno::RuntimeException )
    {
        uno::Sequence< ::rtl::OUString > aRet(1);
        aRet[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( SERVICE_NAME ) );

        return aRet;
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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


#include "implbitmap.hxx"
#include "implbitmapcanvas.hxx"

#include <basegfx/matrix/b2dhommatrix.hxx>
#include <canvas/canvastools.hxx>


using namespace ::com::sun::star;

namespace cppcanvas
{

    namespace internal
    {

        ImplBitmap::ImplBitmap( const CanvasSharedPtr&                      rParentCanvas,
                                const uno::Reference< rendering::XBitmap >& rBitmap ) :
            CanvasGraphicHelper( rParentCanvas ),
            mxBitmap( rBitmap ),
            mpBitmapCanvas()
        {
            OSL_ENSURE( mxBitmap.is(), "ImplBitmap::ImplBitmap: no valid bitmap" );

            uno::Reference< rendering::XBitmapCanvas > xBitmapCanvas( rBitmap,
                                                                      uno::UNO_QUERY );
            if( xBitmapCanvas.is() )
                mpBitmapCanvas.reset( new ImplBitmapCanvas(
                                          uno::Reference< rendering::XBitmapCanvas >(rBitmap,
                                                                                     uno::UNO_QUERY) ) );
        }

        ImplBitmap::~ImplBitmap()
        {
        }

        bool ImplBitmap::draw() const
        {
            CanvasSharedPtr pCanvas( getCanvas() );

            OSL_ENSURE( pCanvas.get() != NULL &&
                        pCanvas->getUNOCanvas().is(),
                        "ImplBitmap::draw: invalid canvas" );

            if( pCanvas.get() == NULL ||
                !pCanvas->getUNOCanvas().is() )
            {
                return false;
            }

            // TODO(P1): implement caching
            pCanvas->getUNOCanvas()->drawBitmap( mxBitmap,
                                                 pCanvas->getViewState(),
                                                 getRenderState() );

            return true;
        }

        bool ImplBitmap::drawAlphaModulated( double nAlphaModulation ) const
        {
            CanvasSharedPtr pCanvas( getCanvas() );

            OSL_ENSURE( pCanvas.get() != NULL &&
                        pCanvas->getUNOCanvas().is(),
                        "ImplBitmap::drawAlphaModulated(): invalid canvas" );

            if( pCanvas.get() == NULL ||
                !pCanvas->getUNOCanvas().is() )
            {
                return false;
            }

            rendering::RenderState aLocalState( getRenderState() );
            uno::Sequence<rendering::ARGBColor> aCol(1);
            aCol[0] = rendering::ARGBColor( nAlphaModulation, 1.0, 1.0, 1.0 );
            aLocalState.DeviceColor =
                pCanvas->getUNOCanvas()->getDevice()->getDeviceColorSpace()->convertFromARGB(aCol);

            // TODO(P1): implement caching
            pCanvas->getUNOCanvas()->drawBitmapModulated( mxBitmap,
                                                          pCanvas->getViewState(),
                                                          aLocalState );

            return true;
        }

        BitmapCanvasSharedPtr ImplBitmap::getBitmapCanvas() const
        {
            return mpBitmapCanvas;
        }

        uno::Reference< rendering::XBitmap > ImplBitmap::getUNOBitmap() const
        {
            return mxBitmap;
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

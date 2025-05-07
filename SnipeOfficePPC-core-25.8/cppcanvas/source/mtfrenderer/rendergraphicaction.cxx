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


#include <rtl/logfile.hxx>
#include <com/sun/star/rendering/XBitmap.hpp>
#include <com/sun/star/rendering/RepaintResult.hpp>
#include <com/sun/star/rendering/XCachedPrimitive.hpp>
#include <vcl/rendergraphicrasterizer.hxx>
#include <tools/gen.hxx>
#include <vcl/canvastools.hxx>
#include <canvas/canvastools.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/vector/b2dsize.hxx>
#include <basegfx/point/b2dpoint.hxx>
#include <basegfx/range/b2drange.hxx>
#include <basegfx/tools/canvastools.hxx>
#include <boost/utility.hpp>
#include "cachedprimitivebase.hxx"
#include "rendergraphicaction.hxx"
#include "outdevstate.hxx"
#include "mtftools.hxx"
#include <basegfx/matrix/b2dhommatrixtools.hxx>


using namespace ::com::sun::star;

namespace cppcanvas
{
    namespace internal
    {
        namespace
        {

            class RenderGraphicAction : public CachedPrimitiveBase
            {
            public:
                RenderGraphicAction( const ::vcl::RenderGraphic& rRenderGraphic,
                              const ::basegfx::B2DPoint& rDstPoint,
                              const ::basegfx::B2DVector& rDstSize,
                              const CanvasSharedPtr&,
                              const OutDevState& );

                virtual bool renderSubset( const ::basegfx::B2DHomMatrix& rTransformation,
                                           const Subset& rSubset ) const;

                virtual ::basegfx::B2DRange getBounds( const ::basegfx::B2DHomMatrix& rTransformation ) const;
                virtual ::basegfx::B2DRange getBounds( const ::basegfx::B2DHomMatrix& rTransformation,
                                                       const Subset& rSubset ) const;

                virtual sal_Int32 getActionCount() const;

            private:
                using Action::render;
                virtual bool renderPrimitive( uno::Reference< rendering::XCachedPrimitive >& rCachedPrimitive,
                                              const ::basegfx::B2DHomMatrix& rTransformation ) const;

                ::vcl::RenderGraphic                    maRenderGraphic;
                uno::Reference< rendering::XBitmap >    mxBitmap;
                CanvasSharedPtr                         mpCanvas;
                rendering::RenderState                  maState;
            };

            RenderGraphicAction::RenderGraphicAction( const ::vcl::RenderGraphic& rRenderGraphic,
                                                      const ::basegfx::B2DPoint& rDstPoint,
                                                      const ::basegfx::B2DVector& rDstSize,
                                                      const CanvasSharedPtr& rCanvas,
                                                      const OutDevState& rState ) :
                CachedPrimitiveBase( rCanvas, true ),
                maRenderGraphic( rRenderGraphic ),
                mpCanvas( rCanvas )
            {
                tools::initRenderState( maState,rState );

                const ::vcl::RenderGraphicRasterizer aRasterizer( rRenderGraphic );
                const BitmapEx aBmpEx( aRasterizer.Rasterize( ::vcl::unotools::sizeFromB2DSize( rDstSize ) ) );
                const Size aRasteredSizePixel( aBmpEx.GetSizePixel() );

                if( aRasteredSizePixel.Width() && aRasteredSizePixel.Height() )
                {
                    const ::basegfx::B2DVector aScale( rDstSize.getX() / aRasteredSizePixel.Width(),
                                                       rDstSize.getY() / aRasteredSizePixel.Height() );
                    const basegfx::B2DHomMatrix aLocalTransformation(
                        basegfx::tools::createScaleTranslateB2DHomMatrix( aScale, rDstPoint)) ;

                    ::canvas::tools::appendToRenderState( maState, aLocalTransformation );

                    // correct clip (which is relative to original transform)
                    tools::modifyClip( maState, rState, rCanvas, rDstPoint, &aScale, NULL );

                    mxBitmap = ::vcl::unotools::xBitmapFromBitmapEx( rCanvas->getUNOCanvas()->getDevice(), aBmpEx );
                }
            }

            bool RenderGraphicAction::renderPrimitive( uno::Reference< rendering::XCachedPrimitive >& rCachedPrimitive,
                                                       const ::basegfx::B2DHomMatrix&                 rTransformation ) const
            {
                RTL_LOGFILE_CONTEXT( aLog, "::cppcanvas::internal::RenderGraphicAction::renderPrimitive()" );
                RTL_LOGFILE_CONTEXT_TRACE1( aLog, "::cppcanvas::internal::RenderGraphicAction: 0x%X", this );

                if( mxBitmap.is() )
                {
                    rendering::RenderState aLocalState( maState );
                    ::canvas::tools::prependToRenderState(aLocalState, rTransformation);

                    rCachedPrimitive = mpCanvas->getUNOCanvas()->drawBitmap( mxBitmap,
                                                                             mpCanvas->getViewState(),
                                                                             aLocalState );
                }

                return true;
            }

            bool RenderGraphicAction::renderSubset( const ::basegfx::B2DHomMatrix&    rTransformation,
                                                    const Subset&                    rSubset ) const
            {
                // rendergraphic only contains a single action, fail if subset
                // requests different range
                if( rSubset.mnSubsetBegin != 0 ||
                    rSubset.mnSubsetEnd != 1 )
                    return false;

                return CachedPrimitiveBase::render( rTransformation );
            }

            ::basegfx::B2DRange RenderGraphicAction::getBounds( const ::basegfx::B2DHomMatrix& rTransformation ) const
            {
                ::basegfx::B2DRange aRet;

                if( mxBitmap.is() )
                {
                    rendering::RenderState aLocalState( maState );
                    ::canvas::tools::prependToRenderState(aLocalState, rTransformation);

                    geometry::IntegerSize2D aSize( mxBitmap->getSize() );

                    aRet = tools::calcDevicePixelBounds( ::basegfx::B2DRange( 0, 0, aSize.Width, aSize.Height ),
                                                         mpCanvas->getViewState(), aLocalState );
                }

                return( aRet );
            }

            ::basegfx::B2DRange RenderGraphicAction::getBounds( const ::basegfx::B2DHomMatrix&  rTransformation,
                                                         const Subset&                  rSubset ) const
            {
                // rendergraphic only contains a single action, empty bounds
                // if subset requests different range
                if( rSubset.mnSubsetBegin != 0 ||
                    rSubset.mnSubsetEnd != 1 )
                    return ::basegfx::B2DRange();

                return getBounds( rTransformation );
            }

            sal_Int32 RenderGraphicAction::getActionCount() const
            {
                return 1;
            }
        }

        ActionSharedPtr RenderGraphicActionFactory::createRenderGraphicAction( const ::vcl::RenderGraphic& rRenderGraphic,
                                                                 const ::basegfx::B2DPoint&  rDstPoint,
                                                                 const ::basegfx::B2DVector& rDstSize,
                                                                 const CanvasSharedPtr&      rCanvas,
                                                                 const OutDevState&          rState )
        {
            return ActionSharedPtr( new RenderGraphicAction(rRenderGraphic,
                                                     rDstPoint,
                                                     rDstSize,
                                                     rCanvas,
                                                     rState ) );
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

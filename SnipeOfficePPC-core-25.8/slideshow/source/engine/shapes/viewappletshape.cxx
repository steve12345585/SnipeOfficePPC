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


// must be first
#include <canvas/verbosetrace.hxx>
#include <tools/diagnose_ex.h>
#include <canvas/debug.hxx>

#include <comphelper/anytostring.hxx>
#include <cppuhelper/exc_hlp.hxx>

#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/range/b2irange.hxx>
#include <basegfx/tools/canvastools.hxx>

#include <cppcanvas/spritecanvas.hxx>
#include <canvas/canvastools.hxx>

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/rendering/XCanvas.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/awt/WindowDescriptor.hpp>
#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/awt/XWindow2.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/VclWindowPeerAttribute.hpp>
#include <com/sun/star/awt/PosSize.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XSynchronousFrameLoader.hpp>

#include "viewappletshape.hxx"
#include "tools.hxx"


using namespace ::com::sun::star;

namespace slideshow
{
    namespace internal
    {
        ViewAppletShape::ViewAppletShape( const ViewLayerSharedPtr&                       rViewLayer,
                                          const uno::Reference< drawing::XShape >&        rxShape,
                                          const ::rtl::OUString&                          rServiceName,
                                          const char**                                    pPropCopyTable,
                                          sal_Size                                        nNumPropEntries,
                                          const uno::Reference< uno::XComponentContext >& rxContext ) :
            mpViewLayer( rViewLayer ),
            mxViewer(),
            mxFrame(),
            mxComponentContext( rxContext )
        {
            ENSURE_OR_THROW( rxShape.is(), "ViewAppletShape::ViewAppletShape(): Invalid Shape" );
            ENSURE_OR_THROW( mpViewLayer, "ViewAppletShape::ViewAppletShape(): Invalid View" );
            ENSURE_OR_THROW( mpViewLayer->getCanvas(), "ViewAppletShape::ViewAppletShape(): Invalid ViewLayer canvas" );
            ENSURE_OR_THROW( mxComponentContext.is(), "ViewAppletShape::ViewAppletShape(): Invalid component context" );

            uno::Reference<lang::XMultiComponentFactory> xFactory(
                mxComponentContext->getServiceManager(),
                uno::UNO_QUERY_THROW );

            mxViewer.set( xFactory->createInstanceWithContext( rServiceName,
                                                               mxComponentContext),
                          uno::UNO_QUERY_THROW );

            uno::Reference< beans::XPropertySet > xShapePropSet( rxShape,
                                                                 uno::UNO_QUERY_THROW );
            uno::Reference< beans::XPropertySet > mxViewerPropSet( mxViewer,
                                                                   uno::UNO_QUERY_THROW );

            // copy shape properties to applet viewer
            ::rtl::OUString aPropName;
            for( sal_Size i=0; i<nNumPropEntries; ++i )
            {
                aPropName = ::rtl::OUString::createFromAscii( pPropCopyTable[i] );
                mxViewerPropSet->setPropertyValue( aPropName,
                                                   xShapePropSet->getPropertyValue(
                                                       aPropName ));
            }
        }

        // ---------------------------------------------------------------------

        ViewAppletShape::~ViewAppletShape()
        {
            try
            {
                endApplet();
            }
            catch (uno::Exception &)
            {
                OSL_FAIL( rtl::OUStringToOString(
                                comphelper::anyToString(
                                    cppu::getCaughtException() ),
                                RTL_TEXTENCODING_UTF8 ).getStr() );
            }
        }

        // ---------------------------------------------------------------------

        ViewLayerSharedPtr ViewAppletShape::getViewLayer() const
        {
            return mpViewLayer;
        }

        // ---------------------------------------------------------------------

        bool ViewAppletShape::startApplet( const ::basegfx::B2DRectangle& rBounds )
        {
            ENSURE_OR_RETURN_FALSE( mpViewLayer && mpViewLayer->getCanvas() && mpViewLayer->getCanvas()->getUNOCanvas().is(),
                               "ViewAppletShape::startApplet(): Invalid or disposed view" );
            try
            {
                ::cppcanvas::CanvasSharedPtr pCanvas = mpViewLayer->getCanvas();

                uno::Reference< beans::XPropertySet > xPropSet( pCanvas->getUNOCanvas()->getDevice(),
                                                                uno::UNO_QUERY_THROW );

                uno::Reference< awt::XWindow2 > xParentWindow(
                    xPropSet->getPropertyValue(
                        ::rtl::OUString("Window" )),
                    uno::UNO_QUERY_THROW );

                uno::Reference<lang::XMultiComponentFactory> xFactory(
                    mxComponentContext->getServiceManager() );

                if( xFactory.is() )
                {
                    // create an awt window to contain the applet
                    // ==========================================

                    uno::Reference< awt::XToolkit > xToolkit(
                        xFactory->createInstanceWithContext(
                            ::rtl::OUString("com.sun.star.awt.Toolkit" ),
                            mxComponentContext ),
                        uno::UNO_QUERY_THROW );

                    awt::WindowDescriptor aOwnWinDescriptor( awt::WindowClass_SIMPLE,
                                                             ::rtl::OUString(),
                                                             uno::Reference< awt::XWindowPeer >(xParentWindow,
                                                                                                uno::UNO_QUERY_THROW),
                                                             0,
                                                             awt::Rectangle(),
                                                             awt::WindowAttribute::SHOW
                                                             | awt::VclWindowPeerAttribute::CLIPCHILDREN );

                    uno::Reference< awt::XWindowPeer > xNewWinPeer(
                        xToolkit->createWindow( aOwnWinDescriptor ));
                    uno::Reference< awt::XWindow > xOwnWindow( xNewWinPeer,
                                                               uno::UNO_QUERY_THROW );


                    // create a frame, and load the applet into it
                    // ===========================================

                    mxFrame.set(
                        xFactory->createInstanceWithContext(
                            ::rtl::OUString("com.sun.star.frame.Frame" ),
                            mxComponentContext ),
                        uno::UNO_QUERY_THROW );

                    mxFrame->initialize( xOwnWindow );

                    uno::Reference < frame::XSynchronousFrameLoader > xLoader( mxViewer,
                                                                               uno::UNO_QUERY_THROW );
                    xLoader->load( uno::Sequence < beans::PropertyValue >(),
                                   mxFrame );


                    // resize surrounding window and applet to current shape size
                    // ==========================================================

                    ::basegfx::B2DRange aTmpRange;
                    ::canvas::tools::calcTransformedRectBounds( aTmpRange,
                                                                rBounds,
                                                                mpViewLayer->getTransformation() );
                    const ::basegfx::B2IRange& rPixelBounds(
                        ::basegfx::unotools::b2ISurroundingRangeFromB2DRange( aTmpRange ));

                    uno::Reference< awt::XWindow > xSurroundingWindow( mxFrame->getContainerWindow() );
                    if( xSurroundingWindow.is() )
                        xSurroundingWindow->setPosSize( static_cast<sal_Int32>(rPixelBounds.getMinX()),
                                                        static_cast<sal_Int32>(rPixelBounds.getMinY()),
                                                        static_cast<sal_Int32>(rPixelBounds.getWidth()),
                                                        static_cast<sal_Int32>(rPixelBounds.getHeight()),
                                                        awt::PosSize::POSSIZE );

                    uno::Reference< awt::XWindow > xAppletWindow( mxFrame->getComponentWindow() );
                    if( xAppletWindow.is() )
                        xAppletWindow->setPosSize( 0, 0,
                                                   static_cast<sal_Int32>(rPixelBounds.getWidth()),
                                                   static_cast<sal_Int32>(rPixelBounds.getHeight()),
                                                   awt::PosSize::POSSIZE );
                }
            }
            catch (uno::Exception &)
            {
                return false;
            }

            return true;
        }

        // ---------------------------------------------------------------------

        void ViewAppletShape::endApplet()
        {
            uno::Reference<util::XCloseable> xCloseable(
                mxFrame,
                uno::UNO_QUERY );

            if( xCloseable.is() )
            {
                xCloseable->close( sal_True );
                mxFrame.clear();
            }
        }

        // ---------------------------------------------------------------------

        bool ViewAppletShape::render( const ::basegfx::B2DRectangle& rBounds ) const
        {
            ::cppcanvas::CanvasSharedPtr pCanvas = mpViewLayer->getCanvas();

            if( !pCanvas )
                return false;

            if( !mxFrame.is() )
            {
                // fill the shape background with black
                fillRect( pCanvas,
                          rBounds,
                          0xFFFFFFFFU );
            }

            return true;
        }

        bool ViewAppletShape::resize( const ::basegfx::B2DRectangle& rBounds ) const
        {
            if( !mxFrame.is() )
                return false;

            ::basegfx::B2DRange aTmpRange;
            ::canvas::tools::calcTransformedRectBounds( aTmpRange,
                                                        rBounds,
                                                        mpViewLayer->getTransformation() );
            const ::basegfx::B2IRange& rPixelBounds(
                ::basegfx::unotools::b2ISurroundingRangeFromB2DRange( aTmpRange ));

            uno::Reference< awt::XWindow > xFrameWindow( mxFrame->getContainerWindow() );
            if( xFrameWindow.is() )
                xFrameWindow->setPosSize( static_cast<sal_Int32>(rPixelBounds.getMinX()),
                                          static_cast<sal_Int32>(rPixelBounds.getMinY()),
                                          static_cast<sal_Int32>(rPixelBounds.getWidth()),
                                          static_cast<sal_Int32>(rPixelBounds.getHeight()),
                                          awt::PosSize::POSSIZE );

            uno::Reference< awt::XWindow > xAppletWindow( mxFrame->getComponentWindow() );
            if( xAppletWindow.is() )
                xAppletWindow->setPosSize( 0, 0,
                                           static_cast<sal_Int32>(rPixelBounds.getWidth()),
                                           static_cast<sal_Int32>(rPixelBounds.getHeight()),
                                           awt::PosSize::POSSIZE );

            return true;
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

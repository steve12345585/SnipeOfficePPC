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


#include <vcl/graph.hxx>
#include <vcl/bmpacc.hxx>
#include <vcl/alpha.hxx>
#include <com/sun/star/graphic/XGraphicRasterizer.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/io/XSeekable.hpp>
#include <cppuhelper/implbase2.hxx>
#include <osl/module.h>

#include <cairo.h>

#include <vector>

#if defined MACOSX
    #define VCL_RSVG_GOBJECT_LIBNAME    "libgobject-2.0.0.dylib"
    #define VCL_RSVG_LIBRSVG_LIBNAME    "librsvg-2.2.dylib"
#elif defined UNX
    #define VCL_RSVG_GOBJECT_LIBNAME    "libgobject-2.0.so.0"
    #define VCL_RSVG_LIBRSVG_LIBNAME    "librsvg-2.so.2"
#elif defined WNT
    #define VCL_RSVG_GOBJECT_LIBNAME    "gobjectlo.dll"
    #define VCL_RSVG_LIBRSVG_LIBNAME    "librsvg-2-2.dll"
#else
    #define VCL_RSVG_GOBJECT_LIBNAME    "nogobjectlib"
    #define VCL_RSVG_LIBRSVG_LIBNAME    "nolibrsvglib"
#endif

using namespace ::com::sun::star;

// -----------------------------------------------------
// - external stuff for dynamic library function calls -
// -----------------------------------------------------

typedef int gboolean;
typedef unsigned char guint8;
typedef sal_Size gsize;
typedef void* gpointer;

struct GError;

struct RsvgHandle;
struct RsvgDimensionData
{
    int width;
    int height;
    double em;
    double ex;
};

namespace vcl
{
namespace rsvg
{
// -----------------
// - Uno functions -
// ----------------

uno::Sequence< ::rtl::OUString > Rasterizer_getSupportedServiceNames()
{
    static ::rtl::OUString aServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.graphic.GraphicRasterizer_RSVG" ) );
    static uno::Sequence< ::rtl::OUString > aServiceNames( &aServiceName, 1 );

    return( aServiceNames );
}

// -----------------------------------------------------------------------------

::rtl::OUString Rasterizer_getImplementationName()
{
    return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "vcl::rsvg::Rasterizer" ) );
}

// ------------------
// - LibraryWrapper -
// ------------------

class LibraryWrapper
{
public:

    static LibraryWrapper& get();

    bool isValid() const { return( ( mpGObjectLib != NULL ) && ( mpRSVGLib != NULL ) ); }

    // G-Object
    gpointer g_object_unref( gpointer pointer ) { return( (*mp_g_object_unref)( pointer ) ); };

    // LibRSVG
    void rsvg_init() { (*mp_rsvg_init)(); }
    RsvgHandle* rsvg_handle_new_from_data( const guint8* data, gsize size, GError** error) { return( (*mp_rsvg_handle_new_from_data)( data, size, error ) ); }
    void rsvg_handle_set_dpi_x_y( RsvgHandle* handle, double dpix, double dpiy ) { (*mp_rsvg_handle_set_dpi_x_y)( handle, dpix, dpiy ); }
    void rsvg_handle_get_dimensions( RsvgHandle* handle, RsvgDimensionData* dimensions ) { (*mp_rsvg_handle_get_dimensions)( handle, dimensions ); }
    gboolean rsvg_handle_render_cairo( RsvgHandle* handle, cairo_t* cairo ) { return( (*mp_rsvg_handle_render_cairo)( handle, cairo ) ); }

private:

    LibraryWrapper();

private:

    oslModule mpGObjectLib;
    oslModule mpRSVGLib;

    // GObject
    gpointer (*mp_g_object_unref)( gpointer );

    // LibRSVG
    void (*mp_rsvg_init)( void );
    RsvgHandle* (*mp_rsvg_handle_new_from_data)( const guint8*, gsize, GError** );
    void (*mp_rsvg_handle_set_dpi_x_y)( RsvgHandle*, double, double );
    void (*mp_rsvg_handle_get_dimensions)( RsvgHandle*, RsvgDimensionData* );
    gboolean (*mp_rsvg_handle_render_cairo)( RsvgHandle*, cairo_t* );
};

// -----------------------------------------------------------------------------

LibraryWrapper& LibraryWrapper::get()
{
    static LibraryWrapper* pLibraryInstance = NULL;

    if( !pLibraryInstance )
        pLibraryInstance = new LibraryWrapper;

    return( *pLibraryInstance );
}

// -----------------------------------------------------------------------------

LibraryWrapper::LibraryWrapper() :
    mpGObjectLib( NULL ),
    mpRSVGLib( NULL )
{
    const ::rtl::OUString aGObjectLibName( RTL_CONSTASCII_USTRINGPARAM( VCL_RSVG_GOBJECT_LIBNAME ) );
    const ::rtl::OUString aRSVGLibName( RTL_CONSTASCII_USTRINGPARAM( VCL_RSVG_LIBRSVG_LIBNAME ) );
    bool bCont = true;

    // GObject
    if( bCont && ( NULL != ( mpGObjectLib = osl_loadModule( aGObjectLibName.pData, SAL_LOADMODULE_DEFAULT ) ) ||
                   NULL != ( mpGObjectLib = osl_loadModuleRelative( (oslGenericFunction)LibraryWrapper::get,
                                                                   aGObjectLibName.pData, SAL_LOADMODULE_DEFAULT ) )

                  ) )
    {
        mp_g_object_unref = ( gpointer (*)( gpointer ) ) osl_getAsciiFunctionSymbol( mpGObjectLib, "g_object_unref" );

        if( !( mp_g_object_unref ) )
        {
            OSL_TRACE( "not all needed symbols were found in g-object library" );
            bCont = false;
        }
    }

    // LibRSVG
    if( bCont && ( NULL != ( mpRSVGLib = osl_loadModule( aRSVGLibName.pData, SAL_LOADMODULE_DEFAULT ) ) ||
                   NULL != ( mpRSVGLib = osl_loadModuleRelative( (oslGenericFunction)LibraryWrapper::get,
                                                                   aRSVGLibName.pData, SAL_LOADMODULE_DEFAULT ) )
                  ) )
    {
        mp_rsvg_init = ( void (*)( void ) ) osl_getAsciiFunctionSymbol( mpRSVGLib, "rsvg_init" );
        mp_rsvg_handle_new_from_data = ( RsvgHandle* (*)( const guint8*, gsize, GError** ) ) osl_getAsciiFunctionSymbol( mpRSVGLib, "rsvg_handle_new_from_data" );
        mp_rsvg_handle_set_dpi_x_y = ( void (*)( RsvgHandle*, double, double ) ) osl_getAsciiFunctionSymbol( mpRSVGLib, "rsvg_handle_set_dpi_x_y" );
        mp_rsvg_handle_get_dimensions = ( void (*)( RsvgHandle*, RsvgDimensionData* ) ) osl_getAsciiFunctionSymbol( mpRSVGLib, "rsvg_handle_get_dimensions" );
        mp_rsvg_handle_render_cairo = ( gboolean (*)( RsvgHandle*, cairo_t* ) ) osl_getAsciiFunctionSymbol( mpRSVGLib, "rsvg_handle_render_cairo" );

        if( !( mp_rsvg_init &&
               mp_rsvg_handle_new_from_data &&
               mp_rsvg_handle_set_dpi_x_y &&
               mp_rsvg_handle_get_dimensions &&
               mp_rsvg_handle_render_cairo ) )
        {
            OSL_TRACE( "not all needed symbols were found in librsvg library" );
            bCont = false;
        }
    }

    OSL_ENSURE( mpGObjectLib, "g-object library could not be loaded" );
    OSL_ENSURE( mpRSVGLib, "librsvg library could not be loaded" );

    bCont = bCont && mpGObjectLib != NULL && mpRSVGLib != NULL;

    // unload all libraries in case of failure
    if( !bCont )
    {
        if( mpRSVGLib )
        {
            osl_unloadModule( mpRSVGLib );
            mpRSVGLib = NULL;
        }

        if( mpGObjectLib )
        {
            osl_unloadModule( mpGObjectLib );
            mpGObjectLib = NULL;
        }
    }
    else
        rsvg_init();
}

// ---------------------------
// - ::vcl::rsvg::Rasterizer -
// ---------------------------

class Rasterizer : public ::cppu::WeakAggImplHelper2< graphic::XGraphicRasterizer, lang::XServiceInfo >
{
public:
                Rasterizer();
    virtual     ~Rasterizer();

    // XGraphicRasterizer
    virtual ::sal_Bool SAL_CALL initializeData( const uno::Reference< io::XInputStream >& DataStream,
                                                ::sal_uInt32 DPI_X, ::sal_uInt32 DPI_Y,
                                                awt::Size& DefaultSizePixel )
        throw ( uno::RuntimeException );

    virtual uno::Reference< graphic::XGraphic > SAL_CALL rasterize( ::sal_uInt32 Width,
                                                                    ::sal_uInt32 Height,
                                                                    double RotateAngle,
                                                                    double ShearAngle_X,
                                                                    double ShearAngle_Y,
                                                                    const uno::Sequence< beans::PropertyValue >& RasterizeProperties )
        throw (uno::RuntimeException);

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName()
        throw( uno::RuntimeException);

    virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& )
        throw( uno::RuntimeException );

    virtual uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
        throw( uno::RuntimeException );

protected:

    void        implFreeRsvgHandle();

    uno::Reference< graphic::XGraphic > implGetXGraphicFromSurface( cairo_surface_t* pSurface ) const;

private:

                Rasterizer( const Rasterizer& );
    Rasterizer& operator=( const Rasterizer& );

private:

    RsvgHandle* mpRsvgHandle;
    sal_Int32   mnDefaultWidth;
    sal_Int32   mnDefaultHeight;
    bool        mbLibInit;
};

// -----------------------------------------------------------------------------

Rasterizer::Rasterizer() :
    mpRsvgHandle( NULL ),
    mnDefaultWidth( 0 ),
    mnDefaultHeight( 0 ),
    mbLibInit( true )
{
    try
    {
        LibraryWrapper& rLib = LibraryWrapper::get();
        mbLibInit = rLib.isValid();
    }
    catch( ... )
    {
        mbLibInit = false;
    }
}

// -----------------------------------------------------------------------------

Rasterizer::~Rasterizer()
{
    implFreeRsvgHandle();
}

// -----------------------------------------------------------------------------

void Rasterizer::implFreeRsvgHandle()
{
    if( mpRsvgHandle )
    {
        LibraryWrapper::get().g_object_unref( static_cast< gpointer >( mpRsvgHandle ) );
        mpRsvgHandle = NULL;
    }
}

// -----------------------------------------------------------------------------

uno::Reference< graphic::XGraphic > Rasterizer::implGetXGraphicFromSurface( cairo_surface_t* pSurface ) const
{
    unsigned char*  pData = cairo_image_surface_get_data( pSurface );
    const sal_Int32 nWidth = cairo_image_surface_get_width( pSurface );
    const sal_Int32 nHeight = cairo_image_surface_get_height( pSurface );
    const sal_Int32 nStride = cairo_image_surface_get_stride( pSurface );

    uno::Reference< graphic::XGraphic > xRet;

    if( pData && nWidth && nHeight && nStride )
    {
        Size                aSize( nWidth, nHeight );
        Bitmap              aBmp( aSize, 24 );
        AlphaMask           aAlphaMask( aSize );
        BitmapWriteAccess*  pBmpAcc = aBmp.AcquireWriteAccess();
        BitmapWriteAccess*  pAlpAcc = aAlphaMask.AcquireWriteAccess();

        if( pBmpAcc && pAlpAcc )
        {
            BitmapColor aPixel, aWhitePixel( 255, 255, 255 ), aAlpha( 0 ), aFullAlpha( 255 );
            sal_uInt32* pRow = reinterpret_cast< sal_uInt32* >( pData );

            for( sal_Int32 nY = 0; nY < nHeight; ++nY, pRow = reinterpret_cast< sal_uInt32* >( pData += nStride )  )
            {
                for( sal_Int32 nX = 0; nX < nWidth; ++nX )
                {
                    const register sal_uInt32 nPixel = *pRow++;
                    const register sal_uInt32 nAlpha = nPixel >> 24;

                    if( !nAlpha )
                    {
                        pBmpAcc->SetPixel( nY, nX, aWhitePixel );
                        pAlpAcc->SetPixel( nY, nX, aFullAlpha );
                    }
                    else
                    {
                        aPixel.SetRed( static_cast< sal_uInt8 >( ( ( ( nPixel & 0x00ff0000 ) >> 16 ) * 255 ) / nAlpha ) );
                        aPixel.SetGreen( static_cast< sal_uInt8 >( ( ( ( nPixel & 0x0000ff00 ) >> 8 ) * 255 ) / nAlpha ) );
                        aPixel.SetBlue( static_cast< sal_uInt8 >( ( ( nPixel & 0x000000ff ) * 255 ) / nAlpha ) );
                        pBmpAcc->SetPixel( nY, nX, aPixel );

                        aAlpha.SetIndex( static_cast< sal_uInt8 >( 255 - nAlpha ) );
                        pAlpAcc->SetPixel( nY, nX, aAlpha );
                    }
                }
            }

            aBmp.ReleaseAccess( pBmpAcc );
            aAlphaMask.ReleaseAccess( pAlpAcc );

            const Graphic aGraphic( BitmapEx( aBmp, aAlphaMask ) );
            xRet.set( aGraphic.GetXGraphic(), uno::UNO_QUERY );
        }
        else
        {
            aBmp.ReleaseAccess( pBmpAcc );
            aAlphaMask.ReleaseAccess( pAlpAcc );
        }
    }

    return( xRet );
}

// -----------------------------------------------------------------------------

::sal_Bool SAL_CALL Rasterizer::initializeData( const uno::Reference< io::XInputStream >& rDataStream,
                                                ::sal_uInt32 nDPI_X, ::sal_uInt32 nDPI_Y,
                                                awt::Size& rDefaultSizePixel )
        throw ( uno::RuntimeException )
{
    LibraryWrapper& rLib = LibraryWrapper::get();

    implFreeRsvgHandle();

    if( mbLibInit && rDataStream.is() )
    {
        ::std::vector< sal_Int8 >       aDataBuffer;
        uno::Reference< io::XSeekable > xSeekable( rDataStream, uno::UNO_QUERY );
        sal_Int32                       nReadSize, nBlockSize = ( xSeekable.is() ? xSeekable->getLength() : 65536 );
        uno::Sequence< sal_Int8 >       aStmBuffer( nBlockSize );

        do
        {
            nReadSize = rDataStream->readBytes( aStmBuffer, nBlockSize );

            if( nReadSize > 0 )
            {
                const sal_Int8* pArray = aStmBuffer.getArray();
                aDataBuffer.insert( aDataBuffer.end(), pArray, pArray + nReadSize );
            }
        }
        while( nReadSize == nBlockSize );

        if (aDataBuffer.size())
        {
            //See: fdo#50975 rsvg_handle_new_from_data calls
            //rsvg_handle_fill_with_data which itself calls rsvg_handle_close
            //on success.  Older versions of librsvg (e.g. 2.16.1) don't
            //protect against rsvg_handle_close getting called twice, so we
            //shouldn't additionally call svg_handle_close here.
            mpRsvgHandle = rLib.rsvg_handle_new_from_data( reinterpret_cast< sal_uInt8* >( &aDataBuffer[ 0 ] ),
                                                                       aDataBuffer.size(), NULL );
        }
    }

    // get default dimensions of image
    mnDefaultWidth = mnDefaultHeight = 0;

    if( mpRsvgHandle )
    {
        RsvgDimensionData aDefaultDimension = { 0, 0, 0.0, 0.0 };

        rLib.rsvg_handle_set_dpi_x_y( mpRsvgHandle, nDPI_X ? nDPI_X: 72, nDPI_Y ? nDPI_Y : 72 );
        rLib.rsvg_handle_get_dimensions( mpRsvgHandle, &aDefaultDimension );

        mnDefaultWidth = aDefaultDimension.width;
        mnDefaultHeight = aDefaultDimension.height;
    }

    rDefaultSizePixel.Width = mnDefaultWidth;
    rDefaultSizePixel.Height = mnDefaultHeight;

    return( mpRsvgHandle != NULL );
}

// -----------------------------------------------------------------------------

uno::Reference< graphic::XGraphic > SAL_CALL Rasterizer::rasterize( ::sal_uInt32 nWidth,
                                                                    ::sal_uInt32 nHeight,
                                                                    double /*fRotateAngle*/,
                                                                    double /*fShearAngle_X*/,
                                                                    double /*ShearAngle_Y*/,
                                                                    const uno::Sequence< beans::PropertyValue >&
                                                                        /*rRasterizeProperties*/ )
        throw ( uno::RuntimeException )
{
    LibraryWrapper& rLib = LibraryWrapper::get();
    uno::Reference< graphic::XGraphic > xRet;

    if( mpRsvgHandle && rLib.isValid() && nWidth && nHeight && mnDefaultWidth && mnDefaultHeight )
    {
        cairo_surface_t* pSurface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, nWidth, nHeight );

        if( pSurface && ( CAIRO_STATUS_SUCCESS == cairo_surface_status( pSurface ) ) )
        {
            cairo_t* pCr = cairo_create( pSurface );

            if( pCr )
            {
                cairo_matrix_t aMatrix;

                cairo_matrix_init_identity( &aMatrix );
                cairo_matrix_scale( &aMatrix,
                                     static_cast< double >( nWidth ) / mnDefaultWidth,
                                     static_cast< double >( nHeight ) / mnDefaultHeight );
                cairo_transform( pCr, &aMatrix );

                if( rLib.rsvg_handle_render_cairo( mpRsvgHandle, pCr ) )
                {
                    xRet = implGetXGraphicFromSurface( pSurface );
                }

                cairo_destroy( pCr );
            }

           cairo_surface_destroy( pSurface );
            OSL_ENSURE( xRet.is(), "SVG *not* rendered successfully" );
        }
    }

    return( xRet );
}

// -----------------------------------------------------------------------------

::rtl::OUString SAL_CALL Rasterizer::getImplementationName()
    throw( uno::RuntimeException )
{
    return( Rasterizer_getImplementationName() );
}

// -----------------------------------------------------------------------------

sal_Bool SAL_CALL Rasterizer::supportsService( const ::rtl::OUString& rServiceName )
        throw( uno::RuntimeException )
{
    const uno::Sequence< ::rtl::OUString > aServices( Rasterizer_getSupportedServiceNames() );

    for( sal_Int32 nService = 0; nService < aServices.getLength(); ++nService )
    {
        if( rServiceName == aServices[ nService ] )
            return sal_True;
    }

    return sal_False;
}

// -----------------------------------------------------------------------------

uno::Sequence< ::rtl::OUString > SAL_CALL Rasterizer::getSupportedServiceNames()
    throw( uno::RuntimeException )
{
    return( Rasterizer_getSupportedServiceNames() );
}

// ------------------------------
// - Uno instantiation function -
// ------------------------------

uno::Reference< uno::XInterface > SAL_CALL Rasterizer_createInstance( const uno::Reference< lang::XMultiServiceFactory >&  )
{
    return static_cast< ::cppu::OWeakObject* >( new rsvg::Rasterizer );
}

} // namespace rsvg
} // namespace vcl

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

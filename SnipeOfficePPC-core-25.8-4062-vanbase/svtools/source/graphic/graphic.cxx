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

#include <osl/mutex.hxx>
#include <vcl/svapp.hxx>
#include <com/sun/star/graphic/GraphicType.hpp>
#include <com/sun/star/graphic/XGraphicTransformer.hpp>
#include <vcl/graph.hxx>
#include "graphic.hxx"
#include <comphelper/servicehelper.hxx>
#include <string.h>

using namespace com::sun::star;

namespace unographic {

// -------------------
// - GraphicProvider -
// -------------------

Graphic::Graphic() :
    mpGraphic( NULL )
{
}

// ------------------------------------------------------------------------------

Graphic::~Graphic()
    throw()
{
    delete mpGraphic;
}

// ------------------------------------------------------------------------------

void Graphic::init( const ::Graphic& rGraphic )
    throw()
{
    delete mpGraphic;
    mpGraphic = new ::Graphic( rGraphic );
    ::unographic::GraphicDescriptor::init( *mpGraphic );
}

// ------------------------------------------------------------------------------

uno::Any SAL_CALL Graphic::queryAggregation( const uno::Type& rType )
    throw( uno::RuntimeException )
{
    uno::Any aAny;
    if( rType == ::getCppuType((const uno::Reference< graphic::XGraphic >*)0) )
        aAny <<= uno::Reference< graphic::XGraphic >( this );
    else if( rType == ::getCppuType((const uno::Reference< awt::XBitmap >*)0) )
        aAny <<= uno::Reference< awt::XBitmap >( this );
    else if( rType == ::getCppuType((const uno::Reference< lang::XUnoTunnel >*)0) )
        aAny <<= uno::Reference< lang::XUnoTunnel >(this);
    else
        aAny <<= ::unographic::GraphicDescriptor::queryAggregation( rType );

    return aAny ;
}

// ------------------------------------------------------------------------------

uno::Any SAL_CALL Graphic::queryInterface( const uno::Type & rType )
    throw( uno::RuntimeException )
{
    ::com::sun::star::uno::Any aReturn = ::unographic::GraphicDescriptor::queryInterface( rType );
    if ( !aReturn.hasValue() )
        aReturn = ::cppu::queryInterface ( rType, static_cast< graphic::XGraphicTransformer*>( this ) );
    return aReturn;
}

// ------------------------------------------------------------------------------

void SAL_CALL Graphic::acquire()
    throw()
{
    ::unographic::GraphicDescriptor::acquire();
}

// ------------------------------------------------------------------------------

void SAL_CALL Graphic::release() throw()
{
    ::unographic::GraphicDescriptor::release();
}

// ------------------------------------------------------------------------------

namespace
{
    class theGraphicUnoTunnelId : public rtl::Static< UnoTunnelIdInit, theGraphicUnoTunnelId > {};
}

uno::Sequence< sal_Int8 > SAL_CALL Graphic::getImplementationId_Static()
    throw(uno::RuntimeException)
{
    return theGraphicUnoTunnelId::get().getSeq();
}

// ------------------------------------------------------------------------------

::rtl::OUString Graphic::getImplementationName_Static()
    throw()
{
    return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.graphic.Graphic" ) );
}

// ------------------------------------------------------------------------------

uno::Sequence< ::rtl::OUString > Graphic::getSupportedServiceNames_Static()
    throw()
{
    uno::Sequence< ::rtl::OUString > aSeq( 1 );

    aSeq.getArray()[ 0 ] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.graphic.Graphic" ) );

    return aSeq;
}

// ------------------------------------------------------------------------------

::rtl::OUString SAL_CALL Graphic::getImplementationName()
    throw( uno::RuntimeException )
{
    return getImplementationName_Static();
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL Graphic::supportsService( const ::rtl::OUString& rServiceName )
    throw( uno::RuntimeException )
{
    if( ::unographic::GraphicDescriptor::supportsService( rServiceName ) )
        return true;
    else
    {
        uno::Sequence< ::rtl::OUString >    aSNL( getSupportedServiceNames() );
        const ::rtl::OUString*              pArray = aSNL.getConstArray();

        for( int i = 0; i < aSNL.getLength(); i++ )
            if( pArray[i] == rServiceName )
                return true;

        return false;
    }
}

// ------------------------------------------------------------------------------

uno::Sequence< ::rtl::OUString > SAL_CALL Graphic::getSupportedServiceNames()
    throw( uno::RuntimeException )
{
    uno::Sequence< ::rtl::OUString >    aRet( ::unographic::GraphicDescriptor::getSupportedServiceNames() );
    uno::Sequence< ::rtl::OUString >    aNew( getSupportedServiceNames_Static() );
    sal_Int32                           nOldCount = aRet.getLength();

    aRet.realloc( nOldCount + aNew.getLength() );

    for( sal_Int32 i = 0; i < aNew.getLength(); ++i )
        aRet[ nOldCount++ ] = aNew[ i ];

    return aRet;
}

// ------------------------------------------------------------------------------

uno::Sequence< uno::Type > SAL_CALL Graphic::getTypes()
    throw(uno::RuntimeException)
{
    uno::Sequence< uno::Type >  aRet( ::unographic::GraphicDescriptor::getTypes() );
    sal_Int32                   nOldCount = aRet.getLength();

    aRet.realloc( nOldCount + 2 );
    aRet[ nOldCount ] = ::getCppuType((const uno::Reference< graphic::XGraphic>*)0);
    aRet[ nOldCount+1 ] = ::getCppuType((const uno::Reference< awt::XBitmap>*)0);

    return aRet;
}

// ------------------------------------------------------------------------------

uno::Sequence< sal_Int8 > SAL_CALL Graphic::getImplementationId()
    throw(uno::RuntimeException)
{
    return getImplementationId_Static();
}

// ------------------------------------------------------------------------------

::sal_Int8 SAL_CALL Graphic::getType()
     throw (uno::RuntimeException)
{
    ::sal_Int8 cRet = graphic::GraphicType::EMPTY;

    if( mpGraphic && ( mpGraphic->GetType() != GRAPHIC_NONE ) )
        cRet = ( ( mpGraphic->GetType() == GRAPHIC_BITMAP ) ? graphic::GraphicType::PIXEL : graphic::GraphicType::VECTOR );

    return cRet;
}

//----------------------------------------------------------------------
// XBitmap
//----------------------------------------------------------------------

awt::Size SAL_CALL Graphic::getSize(  ) throw (uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    ::Size aVclSize;
    if( mpGraphic && ( mpGraphic->GetType() != GRAPHIC_NONE ) )
        aVclSize = mpGraphic->GetSizePixel();

    return awt::Size( aVclSize.Width(), aVclSize.Height() );
}

//----------------------------------------------------------------------

uno::Sequence< ::sal_Int8 > SAL_CALL Graphic::getDIB(  ) throw (uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    if( mpGraphic && ( mpGraphic->GetType() != GRAPHIC_NONE ) )
    {
        SvMemoryStream aMem;
        aMem << mpGraphic->GetBitmapEx().GetBitmap();
        return ::com::sun::star::uno::Sequence<sal_Int8>( (sal_Int8*) aMem.GetData(), aMem.Tell() );
    }
    else
    {
        return uno::Sequence<sal_Int8>();
    }
}

//----------------------------------------------------------------------

uno::Sequence< ::sal_Int8 > SAL_CALL Graphic::getMaskDIB(  ) throw (uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    if( mpGraphic && ( mpGraphic->GetType() != GRAPHIC_NONE ) )
    {
        SvMemoryStream aMem;
        aMem << mpGraphic->GetBitmapEx().GetMask();
        return ::com::sun::star::uno::Sequence<sal_Int8>( (sal_Int8*) aMem.GetData(), aMem.Tell() );
    }
    else
    {
        return uno::Sequence<sal_Int8>();
    }
}

//----------------------------------------------------------------------
const ::Graphic* Graphic::getImplementation( const uno::Reference< uno::XInterface >& rxIFace )
    throw()
{
    uno::Reference< lang::XUnoTunnel > xTunnel( rxIFace, uno::UNO_QUERY );
    return( xTunnel.is() ? reinterpret_cast< ::Graphic* >( xTunnel->getSomething( getImplementationId_Static() ) ) : NULL );
}

//----------------------------------------------------------------------
sal_Int64 SAL_CALL Graphic::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw( uno::RuntimeException )
{
    return( ( rId.getLength() == 16 && 0 == memcmp( getImplementationId().getConstArray(), rId.getConstArray(), 16 ) ) ?
            reinterpret_cast< sal_Int64 >( mpGraphic ) :
            0 );
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

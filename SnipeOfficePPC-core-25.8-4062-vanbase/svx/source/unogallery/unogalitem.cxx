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


#include "unogalitem.hxx"
#include "unogaltheme.hxx"
#include "svx/galtheme.hxx"
#include "svx/galmisc.hxx"
#include <svx/fmmodel.hxx>
#include <osl/mutex.hxx>
#include <vcl/svapp.hxx>
#include <vcl/graph.hxx>
#include <svl/itemprop.hxx>
#include <svl/itempool.hxx>
#include <comphelper/servicehelper.hxx>
#include "galobj.hxx"

#include <com/sun/star/beans/PropertyState.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/gallery/GalleryItemType.hpp>

#define UNOGALLERY_GALLERYITEMTYPE  1
#define UNOGALLERY_URL              2
#define UNOGALLERY_TITLE            3
#define UNOGALLERY_THUMBNAIL        4
#define UNOGALLERY_GRAPHIC          5
#define UNOGALLERY_DRAWING          6

using namespace ::com::sun::star;

namespace unogallery {

// -----------------
// - GalleryItem -
// -----------------

GalleryItem::GalleryItem( ::unogallery::GalleryTheme& rTheme, const GalleryObject& rObject ) :
    ::comphelper::PropertySetHelper( createPropertySetInfo() ),
    mpTheme( &rTheme ),
    mpGalleryObject( &rObject )
{
    mpTheme->implRegisterGalleryItem( *this );
}

// ------------------------------------------------------------------------------

GalleryItem::~GalleryItem()
    throw()
{
    if( mpTheme )
        mpTheme->implDeregisterGalleryItem( *this );
}

// ------------------------------------------------------------------------------

bool GalleryItem::isValid() const
{
    return( mpTheme != NULL );
}

// ------------------------------------------------------------------------------

uno::Any SAL_CALL GalleryItem::queryAggregation( const uno::Type & rType )
    throw( uno::RuntimeException )
{
    uno::Any aAny;

    if( rType == ::getCppuType((const uno::Reference< lang::XServiceInfo >*)0) )
        aAny <<= uno::Reference< lang::XServiceInfo >(this);
    else if( rType == ::getCppuType((const uno::Reference< lang::XTypeProvider >*)0) )
        aAny <<= uno::Reference< lang::XTypeProvider >(this);
    else if( rType == ::getCppuType((const uno::Reference< gallery::XGalleryItem >*)0) )
        aAny <<= uno::Reference< gallery::XGalleryItem >(this);
    else if( rType == ::getCppuType((const uno::Reference< beans::XPropertySet >*)0) )
        aAny <<= uno::Reference< beans::XPropertySet >(this);
    else if( rType == ::getCppuType((const uno::Reference< beans::XPropertyState >*)0) )
        aAny <<= uno::Reference< beans::XPropertyState >(this);
    else if( rType == ::getCppuType((const uno::Reference< beans::XMultiPropertySet >*)0) )
        aAny <<= uno::Reference< beans::XMultiPropertySet >(this);
    else
        aAny <<= OWeakAggObject::queryAggregation( rType );

    return aAny;
}

// ------------------------------------------------------------------------------

uno::Any SAL_CALL GalleryItem::queryInterface( const uno::Type & rType )
    throw( uno::RuntimeException )
{
    return OWeakAggObject::queryInterface( rType );
}

// ------------------------------------------------------------------------------

void SAL_CALL GalleryItem::acquire()
    throw()
{
    OWeakAggObject::acquire();
}

// ------------------------------------------------------------------------------

void SAL_CALL GalleryItem::release()
    throw()
{
    OWeakAggObject::release();
}

// ------------------------------------------------------------------------------

::rtl::OUString GalleryItem::getImplementationName_Static()
    throw()
{
    return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.gallery.GalleryItem" ) );
}

// ------------------------------------------------------------------------------

uno::Sequence< ::rtl::OUString > GalleryItem::getSupportedServiceNames_Static()
    throw()
{
    uno::Sequence< ::rtl::OUString > aSeq( 1 );

    aSeq.getArray()[ 0 ] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.gallery.GalleryItem" ) );

    return aSeq;
}

// ------------------------------------------------------------------------------

::rtl::OUString SAL_CALL GalleryItem::getImplementationName()
    throw( uno::RuntimeException )
{
    return getImplementationName_Static();
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL GalleryItem::supportsService( const ::rtl::OUString& ServiceName )
    throw( uno::RuntimeException )
{
    uno::Sequence< ::rtl::OUString >    aSNL( getSupportedServiceNames() );
    const ::rtl::OUString*              pArray = aSNL.getConstArray();

    for( int i = 0; i < aSNL.getLength(); i++ )
        if( pArray[i] == ServiceName )
            return true;

    return false;
}

// ------------------------------------------------------------------------------

uno::Sequence< ::rtl::OUString > SAL_CALL GalleryItem::getSupportedServiceNames()
    throw( uno::RuntimeException )
{
    return getSupportedServiceNames_Static();
}

// ------------------------------------------------------------------------------

uno::Sequence< uno::Type > SAL_CALL GalleryItem::getTypes()
    throw(uno::RuntimeException)
{
    uno::Sequence< uno::Type >  aTypes( 6 );
    uno::Type*                  pTypes = aTypes.getArray();

    *pTypes++ = ::getCppuType((const uno::Reference< lang::XServiceInfo>*)0);
    *pTypes++ = ::getCppuType((const uno::Reference< lang::XTypeProvider>*)0);
    *pTypes++ = ::getCppuType((const uno::Reference< gallery::XGalleryItem>*)0);
    *pTypes++ = ::getCppuType((const uno::Reference< beans::XPropertySet>*)0);
    *pTypes++ = ::getCppuType((const uno::Reference< beans::XPropertyState>*)0);
    *pTypes++ = ::getCppuType((const uno::Reference< beans::XMultiPropertySet>*)0);

    return aTypes;
}

namespace
{
    class theGalleryItemImplementationId : public rtl::Static< UnoTunnelIdInit, theGalleryItemImplementationId > {};
}

uno::Sequence< sal_Int8 > SAL_CALL GalleryItem::getImplementationId()
    throw(uno::RuntimeException)
{
    return theGalleryItemImplementationId::get().getSeq();
}

// ------------------------------------------------------------------------------

sal_Int8 SAL_CALL GalleryItem::getType()
    throw (uno::RuntimeException)
{
    const SolarMutexGuard aGuard;
    sal_Int8            nRet = gallery::GalleryItemType::EMPTY;

    if( isValid() )
    {
        switch( implGetObject()->eObjKind )
        {
            case( SGA_OBJ_SOUND ):
            case( SGA_OBJ_VIDEO ):
                nRet = gallery::GalleryItemType::MEDIA;
            break;

            case( SGA_OBJ_SVDRAW ):
                nRet = gallery::GalleryItemType::DRAWING;
            break;

            default:
                nRet = gallery::GalleryItemType::GRAPHIC;
            break;
        }
    }

    return nRet;
}

// ------------------------------------------------------------------------------

::comphelper::PropertySetInfo* GalleryItem::createPropertySetInfo()
{
    SolarMutexGuard aGuard;
    ::comphelper::PropertySetInfo*  pRet = new ::comphelper::PropertySetInfo();

    static ::comphelper::PropertyMapEntry aEntries[] =
    {
        { MAP_CHAR_LEN( "GalleryItemType" ), UNOGALLERY_GALLERYITEMTYPE, &::getCppuType( (const sal_Int8*)(0)),
          beans::PropertyAttribute::READONLY, 0 },

        { MAP_CHAR_LEN( "URL" ), UNOGALLERY_URL, &::getCppuType( (const ::rtl::OUString*)(0)),
          beans::PropertyAttribute::READONLY, 0 },

        { MAP_CHAR_LEN( "Title" ), UNOGALLERY_TITLE, &::getCppuType( (const ::rtl::OUString*)(0)),
          0, 0 },

        { MAP_CHAR_LEN( "Thumbnail" ), UNOGALLERY_THUMBNAIL, &::getCppuType( (const uno::Reference< graphic::XGraphic >*)(0)),
          beans::PropertyAttribute::READONLY, 0 },

        { MAP_CHAR_LEN( "Graphic" ), UNOGALLERY_GRAPHIC, &::getCppuType( (const uno::Reference< graphic::XGraphic >*)(0)),
          beans::PropertyAttribute::READONLY, 0 },

        { MAP_CHAR_LEN( "Drawing" ), UNOGALLERY_DRAWING, &::getCppuType( (const uno::Reference< lang::XComponent >*)(0) ),
          beans::PropertyAttribute::READONLY, 0 },

        { 0,0,0,0,0,0}
    };

    pRet->acquire();
    pRet->add( aEntries );

    return pRet;
}

// ------------------------------------------------------------------------------

void GalleryItem::_setPropertyValues( const comphelper::PropertyMapEntry** ppEntries, const uno::Any* pValues )
    throw( beans::UnknownPropertyException,
           beans::PropertyVetoException,
           lang::IllegalArgumentException,
           lang::WrappedTargetException )
{
    const SolarMutexGuard aGuard;

    while( *ppEntries )
    {
        if( UNOGALLERY_TITLE == (*ppEntries)->mnHandle )
        {
            ::rtl::OUString aNewTitle;

            if( *pValues >>= aNewTitle )
            {
                ::GalleryTheme* pGalTheme = ( isValid() ? mpTheme->implGetTheme() : NULL );

                if( pGalTheme )
                {
                    SgaObject* pObj = pGalTheme->ImplReadSgaObject( const_cast< GalleryObject* >( implGetObject() ) );

                    if( pObj )
                    {
                        if( ::rtl::OUString( pObj->GetTitle() ) != aNewTitle )
                        {
                            pObj->SetTitle( aNewTitle );
                            pGalTheme->InsertObject( *pObj );
                        }

                        delete pObj;
                    }
                }
            }
            else
            {
                throw lang::IllegalArgumentException();
            }
        }

        ++ppEntries;
        ++pValues;
    }
}

// ------------------------------------------------------------------------------

void GalleryItem::_getPropertyValues( const comphelper::PropertyMapEntry** ppEntries, uno::Any* pValue )
    throw( beans::UnknownPropertyException,
           lang::WrappedTargetException )
{
    const SolarMutexGuard aGuard;

    while( *ppEntries )
    {
        switch( (*ppEntries)->mnHandle )
        {
            case( UNOGALLERY_GALLERYITEMTYPE ):
            {
                *pValue <<= sal_Int8( getType() );
            }
            break;

            case( UNOGALLERY_URL ):
            {
                ::GalleryTheme* pGalTheme = ( isValid() ? mpTheme->implGetTheme() : NULL );

                if( pGalTheme )
                    *pValue <<= ::rtl::OUString( implGetObject()->aURL.GetMainURL( INetURLObject::NO_DECODE ) );
            }
            break;

            case( UNOGALLERY_TITLE ):
            {
                ::GalleryTheme* pGalTheme = ( isValid() ? mpTheme->implGetTheme() : NULL );

                if( pGalTheme )
                {
                    SgaObject* pObj = pGalTheme->AcquireObject( pGalTheme->ImplGetGalleryObjectPos( implGetObject() ) );

                    if( pObj )
                    {
                        *pValue <<= ::rtl::OUString( pObj->GetTitle() );
                        pGalTheme->ReleaseObject( pObj );
                    }
                }
            }
            break;

            case( UNOGALLERY_THUMBNAIL ):
            {
                ::GalleryTheme* pGalTheme = ( isValid() ? mpTheme->implGetTheme() : NULL );

                if( pGalTheme )
                {
                    SgaObject* pObj = pGalTheme->AcquireObject( pGalTheme->ImplGetGalleryObjectPos( implGetObject() ) );

                    if( pObj )
                    {
                        Graphic aThumbnail;

                        if( pObj->IsThumbBitmap() )
                            aThumbnail = pObj->GetThumbBmp();
                        else
                            aThumbnail = pObj->GetThumbMtf();

                        *pValue <<= aThumbnail.GetXGraphic();
                        pGalTheme->ReleaseObject( pObj );
                    }
                }
            }
            break;

            case( UNOGALLERY_GRAPHIC ):
            {
                ::GalleryTheme* pGalTheme = ( isValid() ? mpTheme->implGetTheme() : NULL );
                Graphic         aGraphic;

                if( pGalTheme && pGalTheme->GetGraphic( pGalTheme->ImplGetGalleryObjectPos( implGetObject() ), aGraphic ) )
                    *pValue <<= aGraphic.GetXGraphic();
            }
            break;

            case( UNOGALLERY_DRAWING ):
            {
                if( gallery::GalleryItemType::DRAWING == getType() )
                {
                    ::GalleryTheme* pGalTheme = ( isValid() ? mpTheme->implGetTheme() : NULL );
                    FmFormModel*    pModel = new FmFormModel;

                    pModel->GetItemPool().FreezeIdRanges();

                    if( pGalTheme && pGalTheme->GetModel( pGalTheme->ImplGetGalleryObjectPos( implGetObject() ), *pModel ) )
                    {
                        uno::Reference< lang::XComponent > xDrawing( new GalleryDrawingModel( pModel ) );

                        pModel->setUnoModel( uno::Reference< uno::XInterface >::query( xDrawing ) );
                        *pValue <<= xDrawing;
                    }
                    else
                        delete pModel;
                }
            }
            break;
        }

        ++ppEntries;
        ++pValue;
    }
}

// ------------------------------------------------------------------------------

const ::GalleryObject* GalleryItem::implGetObject() const
{
    return mpGalleryObject;
}

// ------------------------------------------------------------------------------

void GalleryItem::implSetInvalid()
{
    if( mpTheme )
    {
        mpTheme = NULL;
        mpGalleryObject = NULL;
    }
}

// -----------------------
// - GalleryDrawingModel -
// -----------------------

GalleryDrawingModel::GalleryDrawingModel( SdrModel* pDoc )
    throw() :
    SvxUnoDrawingModel( pDoc )
{
}

// -----------------------------------------------------------------------------

GalleryDrawingModel::~GalleryDrawingModel()
    throw()
{
    delete GetDoc();
}

// -----------------------------------------------------------------------------

UNO3_GETIMPLEMENTATION_IMPL( GalleryDrawingModel );

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

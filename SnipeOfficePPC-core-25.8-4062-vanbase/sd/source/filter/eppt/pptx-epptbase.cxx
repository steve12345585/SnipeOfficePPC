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


#include "eppt.hxx"
#include "epptdef.hxx"

#include <comphelper/extract.hxx>
#include <tools/globname.hxx>
#include <tools/datetime.hxx>
#include <tools/poly.hxx>
#include <tools/stream.hxx>
#include <vcl/graph.hxx>
#include <vcl/bmpacc.hxx>
#include <vcl/gradient.hxx>
#include <vcl/virdev.hxx>
#include <rtl/ustring.hxx>
#include <rtl/strbuf.hxx>
#include <svtools/fltcall.hxx>
#include <vcl/wmf.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/docinf.hxx>
#include <svx/unoapi.hxx>
#include <svx/svdobj.hxx>
#include <svx/svdoole2.hxx>
#include <svx/svdmodel.hxx>
#include <svx/svdpage.hxx>
#include <com/sun/star/view/PaperOrientation.hpp>
#include <com/sun/star/view/PaperFormat.hpp>
#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>
#include <com/sun/star/office/XAnnotation.hpp>
#include <com/sun/star/office/XAnnotationAccess.hpp>
#include <com/sun/star/office/XAnnotationEnumeration.hpp>
#include <com/sun/star/geometry/RealPoint2D.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/animations/TransitionType.hpp>
#include <com/sun/star/animations/TransitionSubType.hpp>
#include <com/sun/star/awt/FontDescriptor.hpp>
#include <com/sun/star/awt/FontFamily.hpp>
#include <com/sun/star/awt/FontPitch.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/presentation/XPresentationPage.hpp>
#include <com/sun/star/text/XSimpleText.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <editeng/svxenum.hxx>
#include <editeng/flditem.hxx>
#include <sot/storinfo.hxx>
#include <filter/msfilter/msoleexp.hxx>
#include <filter/msfilter/msdffimp.hxx>
#include <filter/msfilter/svxmsbas.hxx>

#ifdef DEBUG
#define DBG(x) x
#include <stdio.h>
#else
#define DBG(x)
#endif

using namespace com::sun::star;

using namespace ::com::sun::star::animations;
using namespace ::com::sun::star::awt::FontFamily;
using namespace ::com::sun::star::awt::FontPitch;
using namespace ::com::sun::star::presentation;

using ::com::sun::star::awt::FontDescriptor;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::container::XNameAccess;
using ::com::sun::star::container::XNamed;
using ::com::sun::star::drawing::XDrawPagesSupplier;
using ::com::sun::star::drawing::XMasterPagesSupplier;
using ::com::sun::star::drawing::XShapes;
using ::com::sun::star::drawing::XMasterPageTarget;
using ::com::sun::star::drawing::XDrawPage;
using ::com::sun::star::frame::XModel;
using ::com::sun::star::style::XStyleFamiliesSupplier;
using ::com::sun::star::style::XStyle;
using ::com::sun::star::task::XStatusIndicator;
using ::com::sun::star::text::XSimpleText;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Exception;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::UNO_QUERY;

static PHLayout pPHLayout[] =
{
    { EPP_LAYOUT_TITLESLIDE,            { 0x0d, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x00, 0x0d, 0x10, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_TITLEANDBODYSLIDE,     { 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x00, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_TITLEANDBODYSLIDE,     { 0x0d, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x14, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      { 0x0d, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x00, 0x0d, 0x0e, sal_True, sal_True, sal_True },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      { 0x0d, 0x0e, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x14, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_BLANCSLIDE,            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x00, 0x0d, 0x0e, sal_False, sal_False, sal_False },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      { 0x0d, 0x0e, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x16, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      { 0x0d, 0x14, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x14, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_TITLEANDBODYSLIDE,     { 0x0d, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x15, 0x0d, 0x0e, sal_True, sal_False, sal_False },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      { 0x0d, 0x16, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x16, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      { 0x0d, 0x0e, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x13, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_TITLEANDBODYSLIDE,     { 0x0d, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x13, 0x0d, 0x0e, sal_True, sal_False, sal_False },
    { EPP_LAYOUT_RIGHTCOLUMN2ROWS,      { 0x0d, 0x0e, 0x13, 0x13, 0x00, 0x00, 0x00, 0x00 }, 0x13, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      { 0x0d, 0x13, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x13, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_2ROWSANDTITLE,         { 0x0d, 0x13, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x13, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_LEFTCOLUMN2ROWS,       { 0x0d, 0x13, 0x13, 0x0e, 0x00, 0x00, 0x00, 0x00 }, 0x13, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_TOPROW2COLUMN,         { 0x0d, 0x13, 0x13, 0x0e, 0x00, 0x00, 0x00, 0x00 }, 0x13, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_2ROWSANDTITLE,         { 0x0d, 0x0e, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x13, 0x0d, 0x0e, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_4OBJECTS,              { 0x0d, 0x13, 0x13, 0x13, 0x13, 0x00, 0x00, 0x00 }, 0x13, 0x0d, 0x0e, sal_True, sal_False, sal_False },
    { EPP_LAYOUT_ONLYTITLE,             { 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x00, 0x0d, 0x0e, sal_True, sal_False, sal_False },
    { EPP_LAYOUT_BLANCSLIDE,            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x00, 0x0d, 0x0e, sal_False, sal_False, sal_False },
    { EPP_LAYOUT_TITLERIGHT2BODIESLEFT, { 0x11, 0x12, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x14, 0x11, 0x12, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_TITLERIGHTBODYLEFT,    { 0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x00, 0x11, 0x12, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_TITLEANDBODYSLIDE,     { 0x0d, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x00, 0x0d, 0x12, sal_True, sal_True, sal_False },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      { 0x0d, 0x16, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x16, 0x0d, 0x12, sal_True, sal_True, sal_False }
};

#define PPT_WRITER_BASE_INIT_VALUES \
    maFraction              ( 1, 576 ), \
    maMapModeSrc            ( MAP_100TH_MM ), \
    maMapModeDest           ( MAP_INCH, Point(), maFraction, maFraction ), \
    meLatestPageType        ( NORMAL )

PPTWriterBase::PPTWriterBase() :
    PPT_WRITER_BASE_INIT_VALUES
{
    DBG(printf ("PPTWriterBase::PPTWriterBase()\n"));
}

PPTWriterBase::PPTWriterBase( const Reference< XModel > & rXModel,
                              const Reference< XStatusIndicator > & rXStatInd ) :
    mXModel                 ( rXModel ),
    mXStatusIndicator       ( rXStatInd ),
    mbStatusIndicator       ( false ),
    PPT_WRITER_BASE_INIT_VALUES
{
}

// ---------------------------------------------------------------------------------------------

PPTWriterBase::~PPTWriterBase()
{
    // Possibly unnecessary sanity check for mXStatusIndicator.is().
    // In 3.3 we had a bug report of a crash where it was null,
    // https://bugzilla.novell.com/show_bug.cgi?id=694119 (non-public,
    // bug report, sorry).
    if ( mbStatusIndicator && mXStatusIndicator.is() )
        mXStatusIndicator->end();
}

// ---------------------------------------------------------------------------------------------

void PPTWriterBase::exportPPT( const std::vector< com::sun::star::beans::PropertyValue >& rMediaData )
{
    if ( !InitSOIface() )
        return;

    FontCollectionEntry aDefaultFontDesc( rtl::OUString( "Times New Roman" ),
                                          ROMAN,
                                          awt::FontPitch::VARIABLE,
                                                    RTL_TEXTENCODING_MS_1252 );
    maFontCollection.GetId( aDefaultFontDesc ); // default is always times new roman

    if ( !GetPageByIndex( 0, NOTICE ) )
        return;

    sal_Int32 nWidth = 21000;
    if ( ImplGetPropertyValue( mXPagePropSet, rtl::OUString(  "Width" ) ) )
        mAny >>= nWidth;
    sal_Int32 nHeight = 29700;
    if ( ImplGetPropertyValue( mXPagePropSet, rtl::OUString( "Height" ) ) )
        mAny >>= nHeight;

    maNotesPageSize = MapSize( awt::Size( nWidth, nHeight ) );

    if ( !GetPageByIndex( 0, MASTER ) )
        return;

    nWidth = 28000;
    if ( ImplGetPropertyValue( mXPagePropSet, rtl::OUString( "Width" ) ) )
        mAny >>= nWidth;
    nHeight = 21000;
    if ( ImplGetPropertyValue( mXPagePropSet, rtl::OUString( "Height" ) ) )
        mAny >>= nHeight;
    maDestPageSize = MapSize( awt::Size( nWidth, nHeight ) );

    DBG(printf( "call exportDocumentPre()\n"));
    exportPPTPre(rMediaData);

    if ( !GetStyleSheets() )
        return;

    if ( !ImplCreateDocument() )
         return;

    sal_uInt32 i;

    for ( i = 0; i < mnPages; i++ )
    {
    if ( GetPageByIndex( i, NORMAL ) ) {
        sal_uInt32 nMasterNum = GetMasterIndex( NORMAL );
        ImplWriteLayout( GetLayoutOffset( mXPagePropSet ), nMasterNum );
    }
    }

    for ( i = 0; i < mnMasterPages; i++ )
    {
        if ( !CreateSlideMaster( i ) )
            return;
    }
    if ( !CreateMainNotes() )
        return;

    for ( i = 0; i < mnPages; i++ )
    {
        DBG(printf( "call ImplCreateSlide( %" SAL_PRIuUINT32 " )\n", i));
        if ( !CreateSlide( i ) )
            return;
    }

    for ( i = 0; i < mnPages; i++ )
    {
        if ( !CreateNotes( i ) )
            return;
    }

    DBG(printf( "call exportDocumentPost()\n"));
    exportPPTPost();
}

// ---------------------------------------------------------------------------------------------

sal_Bool PPTWriterBase::InitSOIface()
{
    while( sal_True )
    {
        mXDrawPagesSupplier = Reference< XDrawPagesSupplier >( mXModel, UNO_QUERY );
        if ( !mXDrawPagesSupplier.is() )
            break;

        mXMasterPagesSupplier = Reference< XMasterPagesSupplier >( mXModel, UNO_QUERY );
        if ( !mXMasterPagesSupplier.is() )
            break;
        mXDrawPages = mXMasterPagesSupplier->getMasterPages();
        if ( !mXDrawPages.is() )
            break;
        mnMasterPages = mXDrawPages->getCount();
        mXDrawPages = mXDrawPagesSupplier->getDrawPages();
        if( !mXDrawPages.is() )
            break;
        mnPages =  mXDrawPages->getCount();
        if ( !GetPageByIndex( 0, NORMAL ) )
            break;

        return sal_True;
    }
    return sal_False;
}

// ---------------------------------------------------------------------------------------------

sal_Bool PPTWriterBase::GetPageByIndex( sal_uInt32 nIndex, PageType ePageType )
{
    while( sal_True )
    {
        if ( ePageType != meLatestPageType )
        {
            switch( ePageType )
            {
                case NORMAL :
                case NOTICE :
                {
                    mXDrawPages = mXDrawPagesSupplier->getDrawPages();
                    if( !mXDrawPages.is() )
                        return sal_False;
                }
                break;

                case MASTER :
                {
                    mXDrawPages = mXMasterPagesSupplier->getMasterPages();
                    if( !mXDrawPages.is() )
                        return sal_False;
                }
                break;
                default:
                    break;
            }
            meLatestPageType = ePageType;
        }
        Any aAny( mXDrawPages->getByIndex( nIndex ) );
        aAny >>= mXDrawPage;
        if ( !mXDrawPage.is() )
            break;
        if ( ePageType == NOTICE )
        {
            Reference< XPresentationPage > aXPresentationPage( mXDrawPage, UNO_QUERY );
            if ( !aXPresentationPage.is() )
                break;
            mXDrawPage = aXPresentationPage->getNotesPage();
            if ( !mXDrawPage.is() )
                break;
        }
        mXPagePropSet = Reference< XPropertySet >( mXDrawPage, UNO_QUERY );
        if ( !mXPagePropSet.is() )
            break;

        mXShapes = Reference< XShapes >( mXDrawPage, UNO_QUERY );
        if ( !mXShapes.is() )
            break;

        /* try to get the "real" background PropertySet. If the normal page is not supporting this property, it is
           taken the property from the master */
        sal_Bool bHasBackground = GetPropertyValue( aAny, mXPagePropSet, rtl::OUString( "Background" ), sal_True );
        if ( bHasBackground )
            bHasBackground = ( aAny >>= mXBackgroundPropSet );
        if ( !bHasBackground )
        {
            Reference< XMasterPageTarget > aXMasterPageTarget( mXDrawPage, UNO_QUERY );
            if ( aXMasterPageTarget.is() )
            {
                Reference< XDrawPage > aXMasterDrawPage;
                aXMasterDrawPage = aXMasterPageTarget->getMasterPage();
                if ( aXMasterDrawPage.is() )
                {
                    Reference< XPropertySet > aXMasterPagePropSet;
                    aXMasterPagePropSet = Reference< XPropertySet >
                        ( aXMasterDrawPage, UNO_QUERY );
                    if ( aXMasterPagePropSet.is() )
                    {
                        sal_Bool bBackground = GetPropertyValue( aAny, aXMasterPagePropSet, rtl::OUString( "Background" ) );
                        if ( bBackground )
                        {
                            aAny >>= mXBackgroundPropSet;
                        }
                    }
                }
            }
        }
        return sal_True;
    }
    return sal_False;
}

// ---------------------------------------------------------------------------------------------

sal_Bool PPTWriterBase::CreateSlide( sal_uInt32 nPageNum )
{
    Any aAny;

    if ( !GetPageByIndex( nPageNum, NORMAL ) )
        return sal_False;

    sal_uInt32 nMasterNum = GetMasterIndex( NORMAL );
    SetCurrentStyleSheet( nMasterNum );

    Reference< XPropertySet > aXBackgroundPropSet;
    sal_Bool bHasBackground = GetPropertyValue( aAny, mXPagePropSet, rtl::OUString( "Background" ) );
    if ( bHasBackground )
        bHasBackground = ( aAny >>= aXBackgroundPropSet );

    sal_uInt16 nMode = 7;   // Bit 1: Follow master objects, Bit 2: Follow master scheme, Bit 3: Follow master background
    if ( bHasBackground )
        nMode &=~4;

/* sj: Don't know what's IsBackgroundVisible for, have to ask cl
    if ( GetPropertyValue( aAny, mXPagePropSet, String( RTL_CONSTASCII_USTRINGPARAM( "IsBackgroundVisible" ) ) ) )
    {
        sal_Bool bBackgroundVisible;
        if ( aAny >>= bBackgroundVisible )
        {
            if ( bBackgroundVisible )
                nMode &= ~4;
        }
    }
*/
    if ( GetPropertyValue( aAny, mXPagePropSet, rtl::OUString( "IsBackgroundObjectsVisible" ) ) )
    {
        sal_Bool bBackgroundObjectsVisible = sal_False;
        if ( aAny >>= bBackgroundObjectsVisible )
        {
            if ( !bBackgroundObjectsVisible )
                nMode &= ~1;
        }
    }

    ImplWriteSlide( nPageNum, nMasterNum, nMode, bHasBackground, aXBackgroundPropSet );

    return sal_True;
};

// ---------------------------------------------------------------------------------------------

sal_Bool PPTWriterBase::CreateNotes( sal_uInt32 nPageNum )
{
    if ( !GetPageByIndex( nPageNum, NOTICE ) )
        return sal_False;
    SetCurrentStyleSheet( GetMasterIndex( NORMAL ) );

    ImplWriteNotes( nPageNum );

    return sal_True;
};

// ---------------------------------------------------------------------------------------------

sal_Bool PPTWriterBase::CreateSlideMaster( sal_uInt32 nPageNum )
{
    if ( !GetPageByIndex( nPageNum, MASTER ) )
        return sal_False;
    SetCurrentStyleSheet( nPageNum );

    if ( !ImplGetPropertyValue( mXPagePropSet, rtl::OUString( "Background" ) ) )                // Backgroundshape laden
        return sal_False;
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > aXBackgroundPropSet;
    if ( !( mAny >>= aXBackgroundPropSet ) )
        return sal_False;

    ImplWriteSlideMaster( nPageNum, aXBackgroundPropSet );

    return sal_True;
};

// ---------------------------------------------------------------------------------------------

sal_Int32 PPTWriterBase::GetLayoutOffset( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& rXPropSet ) const
{
    ::com::sun::star::uno::Any aAny;
    sal_Int32 nLayout = 20;
    if ( GetPropertyValue( aAny, rXPropSet, rtl::OUString( "Layout" ) ), sal_True )
        aAny >>= nLayout;

    DBG(printf("GetLayoutOffset %" SAL_PRIdINT32 "\n", nLayout));

    return nLayout;
}

sal_Int32 PPTWriterBase::GetLayoutOffsetFixed( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& rXPropSet ) const
{
    sal_Int32 nLayout = GetLayoutOffset( rXPropSet );

    if ( ( nLayout >= 21 ) && ( nLayout <= 26 ) )   // NOTES _> HANDOUT6
        nLayout = 20;
    if ( ( nLayout >= 27 ) && ( nLayout <= 30 ) )   // VERTICAL LAYOUT
        nLayout -= 6;
    else if ( nLayout > 30 )
        nLayout = 20;

    return nLayout;
}

PHLayout& PPTWriterBase::GetLayout(  const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& rXPropSet ) const
{
    return pPHLayout[ GetLayoutOffsetFixed( rXPropSet ) ];
}

// ---------------------------------------------------------------------------------------------

PHLayout& PPTWriterBase::GetLayout( sal_Int32 nOffset ) const
{
    if( nOffset >= 0 && nOffset < EPP_LAYOUT_SIZE )
        return pPHLayout[ nOffset ];

    DBG(printf("asked %" SAL_PRIdINT32 " for layout outside of 0,%d array scope\n", nOffset, EPP_LAYOUT_SIZE ));

    return pPHLayout[ 0 ];
}

// ---------------------------------------------------------------------------------------------

sal_uInt32 PPTWriterBase::GetMasterIndex( PageType ePageType )
{
    sal_uInt32 nRetValue = 0;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XMasterPageTarget >
        aXMasterPageTarget( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );

    if ( aXMasterPageTarget.is() )
    {
        ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >
            aXDrawPage = aXMasterPageTarget->getMasterPage();
        if ( aXDrawPage.is() )
        {
            ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
                aXPropertySet( aXDrawPage, ::com::sun::star::uno::UNO_QUERY );

            if ( aXPropertySet.is() )
            {
                if ( ImplGetPropertyValue( aXPropertySet, rtl::OUString( "Number" ) ) )
                    nRetValue |= *(sal_Int16*)mAny.getValue();
                if ( nRetValue & 0xffff )           // ueberlauf vermeiden
                    nRetValue--;
            }
        }
    }
    if ( ePageType == NOTICE )
        nRetValue += mnMasterPages;
    return nRetValue;
}

//  -----------------------------------------------------------------------

sal_Bool PPTWriterBase::SetCurrentStyleSheet( sal_uInt32 nPageNum )
{
    sal_Bool bRet = sal_False;
    if ( nPageNum >= maStyleSheetList.size() )
        nPageNum = 0;
    else
        bRet = sal_True;
    mpStyleSheet = maStyleSheetList[ nPageNum ];
    return bRet;
}

// ---------------------------------------------------------------------------------------------

sal_Bool PPTWriterBase::GetStyleSheets()
{
    int             nInstance, nLevel;
    sal_Bool        bRetValue = sal_False;
    sal_uInt32      nPageNum;

    for ( nPageNum = 0; nPageNum < mnMasterPages; nPageNum++ )
    {
        Reference< XNamed >
            aXNamed;

        Reference< XNameAccess >
            aXNameAccess;

        Reference< XStyleFamiliesSupplier >
            aXStyleFamiliesSupplier( mXModel, UNO_QUERY );

        Reference< XPropertySet >
            aXPropSet( mXModel, UNO_QUERY );

        sal_uInt16 nDefaultTab = ( aXPropSet.is() && ImplGetPropertyValue( aXPropSet, rtl::OUString( "TabStop" ) ) )
            ? (sal_uInt16)( *(sal_Int32*)mAny.getValue() / 4.40972 )
            : 1250;

        maStyleSheetList.push_back( new PPTExStyleSheet( nDefaultTab, (PPTExBulletProvider&)*this ) );
        SetCurrentStyleSheet( nPageNum );
        if ( GetPageByIndex( nPageNum, MASTER ) )
            aXNamed = Reference< XNamed >
                        ( mXDrawPage, UNO_QUERY );

        if ( aXStyleFamiliesSupplier.is() )
            aXNameAccess = aXStyleFamiliesSupplier->getStyleFamilies();

        bRetValue = aXNamed.is() && aXNameAccess.is() && aXStyleFamiliesSupplier.is();
        if  ( bRetValue )
        {
            for ( nInstance = EPP_TEXTTYPE_Title; nInstance <= EPP_TEXTTYPE_CenterTitle; nInstance++ )
            {
                String aStyle;
                String aFamily;
                switch ( nInstance )
                {
                    case EPP_TEXTTYPE_CenterTitle :
                    case EPP_TEXTTYPE_Title :
                    {
                        aStyle = rtl::OUString( "title" );
                        aFamily = aXNamed->getName();
                    }
                    break;
                    case EPP_TEXTTYPE_Body :
                    {
                        aStyle = rtl::OUString( "outline1" );      // SD_LT_SEPARATOR
                        aFamily = aXNamed->getName();
                    }
                    break;
                    case EPP_TEXTTYPE_Other :
                    {
                        aStyle = rtl::OUString( "standard" );
                        aFamily = rtl::OUString( "graphics" );
                    }
                    break;
                    case EPP_TEXTTYPE_CenterBody :
                    {
                        aStyle = rtl::OUString( "subtitle" );
                        aFamily = aXNamed->getName();
                    }
                    break;
                }
                if ( aStyle.Len() && aFamily.Len() )
                {
                    try
                    {
                        Reference< XNameAccess >xNameAccess;
                        if ( aXNameAccess->hasByName( aFamily ) )
                        {
                            Any aAny( aXNameAccess->getByName( aFamily ) );
                            if( aAny.getValue() && ::cppu::extractInterface( xNameAccess, aAny ) )
                            {
                                Reference< XNameAccess > aXFamily;
                                if ( aAny >>= aXFamily )
                                {
                                    if ( aXFamily->hasByName( aStyle ) )
                                    {
                                        Reference< XStyle > xStyle;
                                        aAny = aXFamily->getByName( aStyle );
                                        if( aAny.getValue() && ::cppu::extractInterface( xStyle, aAny ) )
                                        {
                                            Reference< XStyle > aXStyle;
                                            aAny >>= aXStyle;
                                            Reference< XPropertySet >
                                                xPropSet( aXStyle, UNO_QUERY );
                                            if( xPropSet.is() )
                                                mpStyleSheet->SetStyleSheet( xPropSet, maFontCollection, nInstance, 0 );
                                            for ( nLevel = 1; nLevel < 5; nLevel++ )
                                            {
                                                if ( nInstance == EPP_TEXTTYPE_Body )
                                                {
                                                    sal_Unicode cTemp = aStyle.GetChar( aStyle.Len() - 1 );
                                                    aStyle.SetChar( aStyle.Len() - 1, ++cTemp );
                                                    if ( aXFamily->hasByName( aStyle ) )
                                                    {
                                                        aXFamily->getByName( aStyle ) >>= xStyle;
                                                        if( xStyle.is() )
                                                        {
                                                            Reference< XPropertySet >
                                                                xPropertySet( xStyle, UNO_QUERY );
                                                            if ( xPropertySet.is() )
                                                                mpStyleSheet->SetStyleSheet( xPropertySet, maFontCollection, nInstance, nLevel );
                                                        }
                                                    }
                                                }
                                                else
                                                    mpStyleSheet->SetStyleSheet( xPropSet, maFontCollection, nInstance, nLevel );
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    catch( Exception& )
                    {
                    //
                    }
                }
            }
            for ( ; nInstance <= EPP_TEXTTYPE_QuarterBody; nInstance++ )
            {

            }
        }
    }
    return bRetValue;
}

//  -----------------------------------------------------------------------

sal_Bool PPTWriterBase::CreateMainNotes()
{
    if ( !GetPageByIndex( 0, NOTICE ) )
        return sal_False;
    SetCurrentStyleSheet( 0 );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XMasterPageTarget >
        aXMasterPageTarget( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );

    if ( !aXMasterPageTarget.is() )
        return sal_False;

    mXDrawPage = aXMasterPageTarget->getMasterPage();
    if ( !mXDrawPage.is() )
        return sal_False;

    mXPropSet = ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet >
            ( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );
    if ( !mXPropSet.is() )
        return sal_False;

    mXShapes = ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShapes >
            ( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );
    if ( !mXShapes.is() )
        return sal_False;

    return ImplCreateMainNotes();
}

//  -----------------------------------------------------------------------

awt::Size PPTWriterBase::MapSize( const awt::Size& rSize )
{
    Size aRetSize( OutputDevice::LogicToLogic( Size( rSize.Width, rSize.Height ), maMapModeSrc, maMapModeDest ) );

    if ( !aRetSize.Width() )
        aRetSize.Width()++;
    if ( !aRetSize.Height() )
        aRetSize.Height()++;
    return awt::Size( aRetSize.Width(), aRetSize.Height() );
}

//  -----------------------------------------------------------------------

awt::Point PPTWriterBase::MapPoint( const awt::Point& rPoint )
{
    Point aRet( OutputDevice::LogicToLogic( Point( rPoint.X, rPoint.Y ), maMapModeSrc, maMapModeDest ) );
    return awt::Point( aRet.X(), aRet.Y() );
}

//  -----------------------------------------------------------------------

Rectangle PPTWriterBase::MapRectangle( const awt::Rectangle& rRect )
{
    ::com::sun::star::awt::Point    aPoint( rRect.X, rRect.Y );
    ::com::sun::star::awt::Size     aSize( rRect.Width, rRect.Height );
    ::com::sun::star::awt::Point    aP( MapPoint( aPoint ) );
    ::com::sun::star::awt::Size     aS( MapSize( aSize ) );
    return Rectangle( Point( aP.X, aP.Y ), Size( aS.Width, aS.Height ) );
}

//  -----------------------------------------------------------------------

sal_Bool PPTWriterBase::GetShapeByIndex( sal_uInt32 nIndex, sal_Bool bGroup )
{
    while(sal_True)
    {
        if (  ( bGroup == sal_False ) || ( GetCurrentGroupLevel() == 0 ) )
        {
            Any aAny( mXShapes->getByIndex( nIndex ) );
            aAny >>= mXShape;
        }
        else
        {
            Any aAny( GetCurrentGroupAccess()->getByIndex( GetCurrentGroupIndex() ) );
            aAny >>= mXShape;
        }
        if ( !mXShape.is() )
            break;

        Any aAny( mXShape->queryInterface( ::getCppuType( (const Reference< XPropertySet >*) 0 ) ));
        aAny >>= mXPropSet;

        if ( !mXPropSet.is() )
            break;
        maPosition = MapPoint( mXShape->getPosition() );
        maSize = MapSize( mXShape->getSize() );
        maRect = Rectangle( Point( maPosition.X, maPosition.Y ), Size( maSize.Width, maSize.Height ) );

        rtl::OStringBuffer aTypeBuffer(rtl::OUStringToOString(
            mXShape->getShapeType(), RTL_TEXTENCODING_UTF8));
        // "com.sun.star." entfernen
        aTypeBuffer.remove(0, RTL_CONSTASCII_LENGTH("com.sun.star."));

        sal_Int32 nPos = aTypeBuffer.toString().indexOf("Shape");
        aTypeBuffer.remove(nPos, RTL_CONSTASCII_LENGTH("Shape"));
        mType = aTypeBuffer.makeStringAndClear();

        mbPresObj = mbEmptyPresObj = sal_False;
        if ( ImplGetPropertyValue( rtl::OUString( "IsPresentationObject" ) ) )
            mAny >>= mbPresObj;

        if ( mbPresObj && ImplGetPropertyValue( rtl::OUString( "IsEmptyPresentationObject" ) ) )
            mAny >>= mbEmptyPresObj;

        mnAngle = ( PropValue::GetPropertyValue( aAny,
            mXPropSet, rtl::OUString( "RotateAngle" ), sal_True ) )
                ? *((sal_Int32*)aAny.getValue() )
                : 0;

        return sal_True;
    }
    return sal_False;
}

//  -----------------------------------------------------------------------

sal_Int8 PPTWriterBase::GetTransition( sal_Int16 nTransitionType, sal_Int16 nTransitionSubtype, FadeEffect eEffect, sal_uInt8& nDirection )
{
    sal_Int8 nPPTTransitionType = 0;
    nDirection = 0;

    switch( nTransitionType )
    {
    case TransitionType::FADE :
    {
        if ( nTransitionSubtype == TransitionSubType::CROSSFADE )
        nPPTTransitionType = PPT_TRANSITION_TYPE_SMOOTHFADE;
        else if ( nTransitionSubtype == TransitionSubType::FADEOVERCOLOR )
        nPPTTransitionType = PPT_TRANSITION_TYPE_FADE;
    }
    break;
    case PPT_TRANSITION_TYPE_COMB :
    {
        nPPTTransitionType = PPT_TRANSITION_TYPE_COMB;
        if ( nTransitionSubtype == TransitionSubType::COMBVERTICAL )
        nDirection++;
    }
    break;
    case TransitionType::PUSHWIPE :
    {
        nPPTTransitionType = PPT_TRANSITION_TYPE_PUSH;
        switch( nTransitionSubtype )
        {
        case TransitionSubType::FROMRIGHT: nDirection = 0; break;
        case TransitionSubType::FROMBOTTOM: nDirection = 1; break;
        case TransitionSubType::FROMLEFT: nDirection = 2; break;
        case TransitionSubType::FROMTOP: nDirection = 3; break;
        }
    }
    break;
    case TransitionType::PINWHEELWIPE :
    {
        nPPTTransitionType = PPT_TRANSITION_TYPE_WHEEL;
        switch( nTransitionSubtype )
        {
        case TransitionSubType::ONEBLADE: nDirection = 1; break;
        case TransitionSubType::TWOBLADEVERTICAL : nDirection = 2; break;
        case TransitionSubType::THREEBLADE : nDirection = 3; break;
        case TransitionSubType::FOURBLADE: nDirection = 4; break;
        case TransitionSubType::EIGHTBLADE: nDirection = 8; break;
        }
    }
    break;
    case TransitionType::FANWIPE :
    {
        nPPTTransitionType = PPT_TRANSITION_TYPE_WEDGE;
    }
    break;
    case TransitionType::ELLIPSEWIPE :
    {
        nPPTTransitionType = PPT_TRANSITION_TYPE_CIRCLE;
    }
    break;
    case TransitionType::FOURBOXWIPE :
    {
        nPPTTransitionType = PPT_TRANSITION_TYPE_PLUS;
    }
    break;
    case TransitionType::IRISWIPE :
    {
        switch( nTransitionSubtype ) {
        case TransitionSubType::RECTANGLE:
            nPPTTransitionType = PPT_TRANSITION_TYPE_ZOOM;
            nDirection = (eEffect == FadeEffect_FADE_FROM_CENTER) ? 0 : 1;
            break;
        default:
            nPPTTransitionType = PPT_TRANSITION_TYPE_DIAMOND;
            break;
        }
    }
    break;
    }

    return nPPTTransitionType;
}

sal_Int8 PPTWriterBase::GetTransition( FadeEffect eEffect, sal_uInt8& nDirection )
{
    sal_Int8 nPPTTransitionType = 0;

    switch ( eEffect )
    {
    default :
    case FadeEffect_RANDOM :
        nPPTTransitionType = PPT_TRANSITION_TYPE_RANDOM;
        break;

    case FadeEffect_HORIZONTAL_STRIPES :
        nDirection++;
    case FadeEffect_VERTICAL_STRIPES :
        nPPTTransitionType = PPT_TRANSITION_TYPE_BLINDS;
        break;

    case FadeEffect_VERTICAL_CHECKERBOARD :
        nDirection++;
    case FadeEffect_HORIZONTAL_CHECKERBOARD :
        nPPTTransitionType = PPT_TRANSITION_TYPE_CHECKER;
        break;

    case FadeEffect_MOVE_FROM_UPPERLEFT :
        nDirection++;
    case FadeEffect_MOVE_FROM_UPPERRIGHT :
        nDirection++;
    case FadeEffect_MOVE_FROM_LOWERLEFT :
        nDirection++;
    case FadeEffect_MOVE_FROM_LOWERRIGHT :
        nDirection++;
    case FadeEffect_MOVE_FROM_TOP :
        nDirection++;
    case FadeEffect_MOVE_FROM_LEFT :
        nDirection++;
    case FadeEffect_MOVE_FROM_BOTTOM :
        nDirection++;
    case FadeEffect_MOVE_FROM_RIGHT :
        nPPTTransitionType = PPT_TRANSITION_TYPE_COVER;
        break;

    case FadeEffect_DISSOLVE :
        nPPTTransitionType = PPT_TRANSITION_TYPE_DISSOLVE;
        break;

    case FadeEffect_VERTICAL_LINES :
        nDirection++;
    case FadeEffect_HORIZONTAL_LINES :
        nPPTTransitionType = PPT_TRANSITION_TYPE_RANDOM_BARS;
        break;

    case FadeEffect_CLOSE_HORIZONTAL :
        nDirection++;
    case FadeEffect_OPEN_HORIZONTAL :
        nDirection++;
    case FadeEffect_CLOSE_VERTICAL :
        nDirection++;
    case FadeEffect_OPEN_VERTICAL :
        nPPTTransitionType = PPT_TRANSITION_TYPE_SPLIT;
        break;

    case FadeEffect_FADE_FROM_UPPERLEFT :
        nDirection++;
    case FadeEffect_FADE_FROM_UPPERRIGHT :
        nDirection++;
    case FadeEffect_FADE_FROM_LOWERLEFT :
        nDirection++;
    case FadeEffect_FADE_FROM_LOWERRIGHT :
        nDirection += 4;
        nPPTTransitionType = PPT_TRANSITION_TYPE_STRIPS;
        break;

    case FadeEffect_UNCOVER_TO_LOWERRIGHT :
        nDirection++;
    case FadeEffect_UNCOVER_TO_LOWERLEFT :
        nDirection++;
    case FadeEffect_UNCOVER_TO_UPPERRIGHT :
        nDirection++;
    case FadeEffect_UNCOVER_TO_UPPERLEFT :
        nDirection++;
    case FadeEffect_UNCOVER_TO_BOTTOM :
        nDirection++;
    case FadeEffect_UNCOVER_TO_RIGHT :
        nDirection++;
    case FadeEffect_UNCOVER_TO_TOP :
        nDirection++;
    case FadeEffect_UNCOVER_TO_LEFT :
        nPPTTransitionType = PPT_TRANSITION_TYPE_PULL;
        break;

    case FadeEffect_FADE_FROM_TOP :
        nDirection++;
    case FadeEffect_FADE_FROM_LEFT :
        nDirection++;
    case FadeEffect_FADE_FROM_BOTTOM :
        nDirection++;
    case FadeEffect_FADE_FROM_RIGHT :
        nPPTTransitionType = PPT_TRANSITION_TYPE_WIPE;
        break;

    case FadeEffect_ROLL_FROM_TOP :
        nDirection++;
    case FadeEffect_ROLL_FROM_LEFT :
        nDirection++;
    case FadeEffect_ROLL_FROM_BOTTOM :
        nDirection++;
    case FadeEffect_ROLL_FROM_RIGHT :
        nPPTTransitionType = PPT_TRANSITION_TYPE_WIPE;
        break;

    case FadeEffect_FADE_TO_CENTER :
        nDirection++;
    case FadeEffect_FADE_FROM_CENTER :
        nPPTTransitionType = PPT_TRANSITION_TYPE_ZOOM;
        break;

    case FadeEffect_NONE :
        nDirection = 2;
        break;
    }

    return nPPTTransitionType;
}

//  -----------------------------------------------------------------------

sal_Bool PPTWriterBase::ContainsOtherShapeThanPlaceholders( sal_Bool bForOOMLX )
{
    sal_uInt32 nShapes = mXShapes->getCount();
    sal_Bool bOtherThanPlaceHolders = sal_False;

    if ( nShapes )
    for ( sal_uInt32 nIndex = 0; ( nIndex < nShapes ) && ( bOtherThanPlaceHolders == sal_False ); nIndex++ ) {
        if ( GetShapeByIndex( nIndex ) && mType != "drawing.Page" ) {
        if( bForOOMLX &&
            ( mType == "presentation.Page" ||
              mType == "presentation.Notes" ) ) {
            Reference< XSimpleText > rXText( mXShape, UNO_QUERY );

            if( rXText.is() && !rXText->getString().isEmpty() )
            bOtherThanPlaceHolders = sal_True;
        } else
            bOtherThanPlaceHolders = sal_True;
        }
        DBG(printf("mType == %s\n", mType.getStr()));
    }

    return bOtherThanPlaceHolders;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

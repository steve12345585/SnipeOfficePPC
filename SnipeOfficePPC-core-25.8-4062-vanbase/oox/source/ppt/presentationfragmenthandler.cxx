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

#include "comphelper/anytostring.hxx"
#include "cppuhelper/exc_hlp.hxx"

#include <com/sun/star/drawing/XMasterPagesSupplier.hpp>
#include <com/sun/star/drawing/XDrawPages.hpp>
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>
#include <com/sun/star/drawing/XMasterPageTarget.hpp>
#include <com/sun/star/xml/dom/XDocument.hpp>
#include <com/sun/star/xml/sax/XFastSAXSerializable.hpp>
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/presentation/XPresentationPage.hpp>
#include <com/sun/star/task/XStatusIndicator.hpp>

#include "oox/drawingml/theme.hxx"
#include "oox/drawingml/drawingmltypes.hxx"
#include "oox/drawingml/themefragmenthandler.hxx"
#include "oox/drawingml/textliststylecontext.hxx"
#include "oox/ppt/pptshape.hxx"
#include "oox/ppt/presentationfragmenthandler.hxx"
#include "oox/ppt/slidefragmenthandler.hxx"
#include "oox/ppt/layoutfragmenthandler.hxx"
#include "oox/ppt/pptimport.hxx"

using rtl::OUString;
using namespace ::com::sun::star;
using namespace ::oox::core;
using namespace ::oox::drawingml;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::presentation;
using namespace ::com::sun::star::xml::sax;

namespace oox { namespace ppt {

PresentationFragmentHandler::PresentationFragmentHandler( XmlFilterBase& rFilter, const OUString& rFragmentPath ) throw()
: FragmentHandler2( rFilter, rFragmentPath )
, mpTextListStyle( new TextListStyle )
{
    TextParagraphPropertiesVector& rParagraphDefaulsVector( mpTextListStyle->getListStyle() );
    TextParagraphPropertiesVector::iterator aParagraphDefaultIter( rParagraphDefaulsVector.begin() );
    while( aParagraphDefaultIter != rParagraphDefaulsVector.end() )
    {
        // ppt is having zero bottom margin per default, whereas OOo is 0,5cm,
        // so this attribute needs to be set always
        (*aParagraphDefaultIter++)->getParaBottomMargin() = TextSpacing( 0 );
    }
}

PresentationFragmentHandler::~PresentationFragmentHandler() throw()
{
}

void ResolveTextFields( XmlFilterBase& rFilter )
{
    const oox::core::TextFieldStack& rTextFields = rFilter.getTextFieldStack();
    if ( rTextFields.size() )
    {
        Reference< frame::XModel > xModel( rFilter.getModel() );
        oox::core::TextFieldStack::const_iterator aIter( rTextFields.begin() );
        while( aIter != rTextFields.end() )
        {
            const OUString sURL = CREATE_OUSTRING( "URL" );
            Reference< drawing::XDrawPagesSupplier > xDPS( xModel, uno::UNO_QUERY_THROW );
            Reference< drawing::XDrawPages > xDrawPages( xDPS->getDrawPages(), uno::UNO_QUERY_THROW );

            const oox::core::TextField& rTextField( *aIter++ );
            Reference< XPropertySet > xPropSet( rTextField.xTextField, UNO_QUERY );
            Reference< XPropertySetInfo > xPropSetInfo( xPropSet->getPropertySetInfo() );
            if ( xPropSetInfo->hasPropertyByName( sURL ) )
            {
                rtl::OUString aURL;
                if ( xPropSet->getPropertyValue( sURL ) >>= aURL )
                {
                    const OUString sSlide = CREATE_OUSTRING( "#Slide " );
                    const OUString sNotes = CREATE_OUSTRING( "#Notes " );
                    sal_Bool bNotes = sal_False;
                    sal_Int32 nPageNumber = 0;
                    if ( aURL.match( sSlide ) )
                        nPageNumber = aURL.copy( sSlide.getLength() ).toInt32();
                    else if ( aURL.match( sNotes ) )
                    {
                        nPageNumber = aURL.copy( sNotes.getLength() ).toInt32();
                        bNotes = sal_True;
                    }
                    if ( nPageNumber )
                    {
                        try
                        {
                            Reference< XDrawPage > xDrawPage;
                            xDrawPages->getByIndex( nPageNumber - 1 ) >>= xDrawPage;
                            if ( bNotes )
                            {
                                Reference< ::com::sun::star::presentation::XPresentationPage > xPresentationPage( xDrawPage, UNO_QUERY_THROW );
                                xDrawPage = xPresentationPage->getNotesPage();
                            }
                            Reference< container::XNamed > xNamed( xDrawPage, UNO_QUERY_THROW );
                            aURL = CREATE_OUSTRING( "#" ).concat( xNamed->getName() );
                            xPropSet->setPropertyValue( sURL, Any( aURL ) );
                            Reference< text::XTextContent > xContent( rTextField.xTextField, UNO_QUERY);
                            Reference< text::XTextRange > xTextRange( rTextField.xTextCursor, UNO_QUERY );
                            rTextField.xText->insertTextContent( xTextRange, xContent, sal_True );
                        }
                        catch( uno::Exception& )
                        {
                        }
                    }
                }
            }
        }
    }
}

void PresentationFragmentHandler::finalizeImport()
{
    // todo: localized progress bar text
    const Reference< task::XStatusIndicator >& rxStatusIndicator( getFilter().getStatusIndicator() );
    if ( rxStatusIndicator.is() )
        rxStatusIndicator->start( rtl::OUString(), 10000 );

    try
    {
        PowerPointImport& rFilter = dynamic_cast< PowerPointImport& >( getFilter() );

        Reference< frame::XModel > xModel( rFilter.getModel() );
        Reference< drawing::XDrawPage > xSlide;
        sal_uInt32 nSlide;

        // importing slide pages and its corresponding notes page
        Reference< drawing::XDrawPagesSupplier > xDPS( xModel, uno::UNO_QUERY_THROW );
        Reference< drawing::XDrawPages > xDrawPages( xDPS->getDrawPages(), uno::UNO_QUERY_THROW );

        for( nSlide = 0; nSlide < maSlidesVector.size(); nSlide++ )
        {
            if ( rxStatusIndicator.is() )
                rxStatusIndicator->setValue( ( nSlide * 10000 ) / maSlidesVector.size() );

            if( nSlide == 0 )
                xDrawPages->getByIndex( 0 ) >>= xSlide;
            else
                xSlide = xDrawPages->insertNewByIndex( nSlide );

            OUString aSlideFragmentPath = getFragmentPathFromRelId( maSlidesVector[ nSlide ] );
            if( !aSlideFragmentPath.isEmpty() )
            {
				rtl::OUString aMasterFragmentPath;
                SlidePersistPtr pMasterPersistPtr;
                SlidePersistPtr pSlidePersistPtr( new SlidePersist( rFilter, sal_False, sal_False, xSlide,
                                    ShapePtr( new PPTShape( Slide, "com.sun.star.drawing.GroupShape" ) ), mpTextListStyle ) );

                FragmentHandlerRef xSlideFragmentHandler( new SlideFragmentHandler( rFilter, aSlideFragmentPath, pSlidePersistPtr, Slide ) );

                // importing the corresponding masterpage/layout
                OUString aLayoutFragmentPath = xSlideFragmentHandler->getFragmentPathFromFirstType( CREATE_OFFICEDOC_RELATION_TYPE( "slideLayout" ) );
                if ( !aLayoutFragmentPath.isEmpty() )
                {
                    // importing layout
                    RelationsRef xLayoutRelations = rFilter.importRelations( aLayoutFragmentPath );
                    aMasterFragmentPath = xLayoutRelations->getFragmentPathFromFirstType( CREATE_OFFICEDOC_RELATION_TYPE( "slideMaster" ) );
                    if( !aMasterFragmentPath.isEmpty() )
                    {
                        // check if the corresponding masterpage+layout has already been imported
                        std::vector< SlidePersistPtr >& rMasterPages( rFilter.getMasterPages() );
                        std::vector< SlidePersistPtr >::iterator aIter( rMasterPages.begin() );
                        while( aIter != rMasterPages.end() )
                        {
                            if ( ( (*aIter)->getPath() == aMasterFragmentPath ) && ( (*aIter)->getLayoutPath() == aLayoutFragmentPath ) )
                            {
                                pMasterPersistPtr = *aIter;
                                break;
                            }
                            ++aIter;
                        }

                        if ( !pMasterPersistPtr.get() )
                        {   // masterpersist not found, we have to load it
                            Reference< drawing::XDrawPage > xMasterPage;
                            Reference< drawing::XMasterPagesSupplier > xMPS( xModel, uno::UNO_QUERY_THROW );
                            Reference< drawing::XDrawPages > xMasterPages( xMPS->getMasterPages(), uno::UNO_QUERY_THROW );

                            if( !(rFilter.getMasterPages().size() ))
                                xMasterPages->getByIndex( 0 ) >>= xMasterPage;
                            else
                                xMasterPage = xMasterPages->insertNewByIndex( xMasterPages->getCount() );

                            pMasterPersistPtr = SlidePersistPtr( new SlidePersist( rFilter, sal_True, sal_False, xMasterPage,
                                ShapePtr( new PPTShape( Master, "com.sun.star.drawing.GroupShape" ) ), mpTextListStyle ) );
                            pMasterPersistPtr->setLayoutPath( aLayoutFragmentPath );
                            rFilter.getMasterPages().push_back( pMasterPersistPtr );
                            rFilter.setActualSlidePersist( pMasterPersistPtr );
                            FragmentHandlerRef xMasterFragmentHandler( new SlideFragmentHandler( rFilter, aMasterFragmentPath, pMasterPersistPtr, Master ) );

                            // set the correct theme
                            OUString aThemeFragmentPath = xMasterFragmentHandler->getFragmentPathFromFirstType( CREATE_OFFICEDOC_RELATION_TYPE( "theme" ) );
                            if( !aThemeFragmentPath.isEmpty() )
                            {
                                std::map< OUString, oox::drawingml::ThemePtr >& rThemes( rFilter.getThemes() );
                                std::map< OUString, oox::drawingml::ThemePtr >::iterator aIter2( rThemes.find( aThemeFragmentPath ) );
                                if( aIter2 == rThemes.end() )
                                {
                                    oox::drawingml::ThemePtr pThemePtr( new oox::drawingml::Theme() );
                                    pMasterPersistPtr->setTheme( pThemePtr );
                                    Reference<xml::dom::XDocument> xDoc=
                                        rFilter.importFragment(aThemeFragmentPath);

                                    rFilter.importFragment(
                                        new ThemeFragmentHandler(
                                            rFilter, aThemeFragmentPath, *pThemePtr ),
                                        Reference<xml::sax::XFastSAXSerializable>(
                                            xDoc,
                                            UNO_QUERY_THROW));
                                    rThemes[ aThemeFragmentPath ] = pThemePtr;
                                    pThemePtr->setFragment(xDoc);
                                }
                                else
                                {
                                    pMasterPersistPtr->setTheme( (*aIter2).second );
                                }
                            }
                            importSlide( xMasterFragmentHandler, pMasterPersistPtr );
                            rFilter.importFragment( new LayoutFragmentHandler( rFilter, aLayoutFragmentPath, pMasterPersistPtr ) );
                            pMasterPersistPtr->createBackground( rFilter );
                            pMasterPersistPtr->createXShapes( rFilter );
                        }
                    }
                }

                // importing slide page
                if (pMasterPersistPtr.get()) {
                    pSlidePersistPtr->setMasterPersist( pMasterPersistPtr );
                    pSlidePersistPtr->setTheme( pMasterPersistPtr->getTheme() );
                    Reference< drawing::XMasterPageTarget > xMasterPageTarget( pSlidePersistPtr->getPage(), UNO_QUERY );
                    if( xMasterPageTarget.is() )
                        xMasterPageTarget->setMasterPage( pMasterPersistPtr->getPage() );
                }
                rFilter.getDrawPages().push_back( pSlidePersistPtr );
                rFilter.setActualSlidePersist( pSlidePersistPtr );
                importSlide( xSlideFragmentHandler, pSlidePersistPtr );
                pSlidePersistPtr->createBackground( rFilter );
                pSlidePersistPtr->createXShapes( rFilter );

                // now importing the notes page
                OUString aNotesFragmentPath = xSlideFragmentHandler->getFragmentPathFromFirstType( CREATE_OFFICEDOC_RELATION_TYPE( "notesSlide" ) );
                if( !aNotesFragmentPath.isEmpty() )
                {
                    Reference< XPresentationPage > xPresentationPage( xSlide, UNO_QUERY );
                    if ( xPresentationPage.is() )
                    {
                        Reference< XDrawPage > xNotesPage( xPresentationPage->getNotesPage() );
                        if ( xNotesPage.is() )
                        {
                            SlidePersistPtr pNotesPersistPtr( new SlidePersist( rFilter, sal_False, sal_True, xNotesPage,
                                ShapePtr( new PPTShape( Slide, "com.sun.star.drawing.GroupShape" ) ), mpTextListStyle ) );
                            FragmentHandlerRef xNotesFragmentHandler( new SlideFragmentHandler( getFilter(), aNotesFragmentPath, pNotesPersistPtr, Slide ) );
                            rFilter.getNotesPages().push_back( pNotesPersistPtr );
                            rFilter.setActualSlidePersist( pNotesPersistPtr );
                            importSlide( xNotesFragmentHandler, pNotesPersistPtr );
                            pNotesPersistPtr->createBackground( rFilter );
                            pNotesPersistPtr->createXShapes( rFilter );
                        }
                    }
                }
            }
        }
        ResolveTextFields( rFilter );
    }
    catch( uno::Exception& )
    {
        OSL_FAIL( OString(rtl::OString("oox::ppt::PresentationFragmentHandler::EndDocument(), "
                    "exception caught: ") +
            rtl::OUStringToOString(
                comphelper::anyToString( cppu::getCaughtException() ),
                RTL_TEXTENCODING_UTF8 )).getStr() );

    }

    // todo error handling;
    if ( rxStatusIndicator.is() )
        rxStatusIndicator->end();
}

// CT_Presentation
::oox::core::ContextHandlerRef PresentationFragmentHandler::onCreateContext( sal_Int32 aElementToken, const AttributeList& rAttribs )
{
    switch( aElementToken )
    {
    case PPT_TOKEN( presentation ):
    case PPT_TOKEN( sldMasterIdLst ):
    case PPT_TOKEN( notesMasterIdLst ):
    case PPT_TOKEN( sldIdLst ):
        return this;
    case PPT_TOKEN( sldMasterId ):
        maSlideMasterVector.push_back( rAttribs.getString( R_TOKEN( id ), OUString() ) );
        return this;
    case PPT_TOKEN( sldId ):
        maSlidesVector.push_back( rAttribs.getString( R_TOKEN( id ), OUString() ) );
        return this;
    case PPT_TOKEN( notesMasterId ):
        maNotesMasterVector.push_back( rAttribs.getString( R_TOKEN( id ), OUString() ) );
        return this;
    case PPT_TOKEN( sldSz ):
        maSlideSize = GetSize2D( rAttribs.getFastAttributeList() );
        return this;
    case PPT_TOKEN( notesSz ):
        maNotesSize = GetSize2D( rAttribs.getFastAttributeList() );
        return this;
    case PPT_TOKEN( custShowLst ):
        return new CustomShowListContext( *this, maCustomShowList );
    case PPT_TOKEN( defaultTextStyle ):
        return new TextListStyleContext( *this, *mpTextListStyle );
    }
    return this;
}

bool PresentationFragmentHandler::importSlide( const FragmentHandlerRef& rxSlideFragmentHandler,
        const SlidePersistPtr pSlidePersistPtr )
{
    Reference< drawing::XDrawPage > xSlide( pSlidePersistPtr->getPage() );
    SlidePersistPtr pMasterPersistPtr( pSlidePersistPtr->getMasterPersist() );
    if ( pMasterPersistPtr.get() )
    {
        const OUString sLayout = CREATE_OUSTRING( "Layout" );
        uno::Reference< beans::XPropertySet > xSet( xSlide, uno::UNO_QUERY_THROW );
        xSet->setPropertyValue( sLayout, Any( pMasterPersistPtr->getLayoutFromValueToken() ) );
    }
    while( xSlide->getCount() )
    {
        Reference< drawing::XShape > xShape;
        xSlide->getByIndex(0) >>= xShape;
        xSlide->remove( xShape );
    }

    Reference< XPropertySet > xPropertySet( xSlide, UNO_QUERY );
    if ( xPropertySet.is() )
    {
        static const OUString sWidth = CREATE_OUSTRING( "Width" );
        static const OUString sHeight = CREATE_OUSTRING( "Height" );
        awt::Size& rPageSize( pSlidePersistPtr->isNotesPage() ? maNotesSize : maSlideSize );
        xPropertySet->setPropertyValue( sWidth, Any( rPageSize.Width ) );
        xPropertySet->setPropertyValue( sHeight, Any( rPageSize.Height ) );

        oox::ppt::HeaderFooter aHeaderFooter( pSlidePersistPtr->getHeaderFooter() );
        if ( !pSlidePersistPtr->isMasterPage() )
            aHeaderFooter.mbSlideNumber = aHeaderFooter.mbHeader = aHeaderFooter.mbFooter = aHeaderFooter.mbDateTime = sal_False;
        try
        {
            static const OUString sIsHeaderVisible = CREATE_OUSTRING( "IsHeaderVisible" );
            static const OUString sIsFooterVisible = CREATE_OUSTRING( "IsFooterVisible" );
            static const OUString sIsDateTimeVisible = CREATE_OUSTRING( "IsDateTimeVisible" );
            static const OUString sIsPageNumberVisible = CREATE_OUSTRING( "IsPageNumberVisible" );

            if ( pSlidePersistPtr->isNotesPage() )
                xPropertySet->setPropertyValue( sIsHeaderVisible, Any( aHeaderFooter.mbHeader ) );
            xPropertySet->setPropertyValue( sIsFooterVisible, Any( aHeaderFooter.mbFooter ) );
            xPropertySet->setPropertyValue( sIsDateTimeVisible, Any( aHeaderFooter.mbDateTime ) );
            xPropertySet->setPropertyValue( sIsPageNumberVisible, Any( aHeaderFooter.mbSlideNumber ) );
        }
        catch( uno::Exception& )
        {
        }
    }
    pSlidePersistPtr->setPath( rxSlideFragmentHandler->getFragmentPath() );
    return getFilter().importFragment( rxSlideFragmentHandler );
}

} }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

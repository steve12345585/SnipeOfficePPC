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

#include <osl/diagnose.h>

#include "diagramdefinitioncontext.hxx"
#include "diagramfragmenthandler.hxx"
#include "datamodelcontext.hxx"
#include "oox/drawingml/colorchoicecontext.hxx"

using namespace ::oox::core;
using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::uno;
using ::rtl::OUString;

namespace oox { namespace drawingml {

DiagramDataFragmentHandler::DiagramDataFragmentHandler( XmlFilterBase& rFilter,
                                                        const OUString& rFragmentPath,
                                                        const DiagramDataPtr pDataPtr )
    throw( )
    : FragmentHandler( rFilter, rFragmentPath )
    , mpDataPtr( pDataPtr )
{
}

DiagramDataFragmentHandler::~DiagramDataFragmentHandler( ) throw ()
{

}

void SAL_CALL DiagramDataFragmentHandler::endDocument()
    throw (SAXException, RuntimeException)
{

}


Reference< XFastContextHandler > SAL_CALL
DiagramDataFragmentHandler::createFastChildContext( ::sal_Int32 aElement,
                                                    const Reference< XFastAttributeList >& )
    throw ( SAXException, RuntimeException)
{
    Reference< XFastContextHandler > xRet;

    switch( aElement )
    {
    case DGM_TOKEN( dataModel ):
        xRet.set( new DataModelContext( *this, mpDataPtr ) );
        break;
    default:
        break;
    }

    if( !xRet.is() )
        xRet = getFastContextHandler();

    return xRet;
}

///////////////////

DiagramLayoutFragmentHandler::DiagramLayoutFragmentHandler( XmlFilterBase& rFilter,
                                                        const OUString& rFragmentPath,
                                                        const DiagramLayoutPtr pDataPtr )
    throw( )
    : FragmentHandler( rFilter, rFragmentPath )
    , mpDataPtr( pDataPtr )
{
}

DiagramLayoutFragmentHandler::~DiagramLayoutFragmentHandler( ) throw ()
{

}

void SAL_CALL DiagramLayoutFragmentHandler::endDocument()
    throw (SAXException, RuntimeException)
{

}


Reference< XFastContextHandler > SAL_CALL
DiagramLayoutFragmentHandler::createFastChildContext( ::sal_Int32 aElement,
                                                      const Reference< XFastAttributeList >& xAttribs )
    throw ( SAXException, RuntimeException)
{
    Reference< XFastContextHandler > xRet;

    switch( aElement )
    {
    case DGM_TOKEN( layoutDef ):
        xRet.set( new DiagramDefinitionContext( *this, xAttribs, mpDataPtr ) );
        break;
    default:
        break;
    }

    if( !xRet.is() )
        xRet = getFastContextHandler();

    return xRet;
}

///////////////////////

DiagramQStylesFragmentHandler::DiagramQStylesFragmentHandler( XmlFilterBase& rFilter,
                                                              const OUString& rFragmentPath,
                                                              DiagramQStyleMap& rStylesMap ) :
    FragmentHandler2( rFilter, rFragmentPath ),
    maStyleName(),
    maStyleEntry(),
    mrStylesMap( rStylesMap )
{}

::oox::core::ContextHandlerRef DiagramQStylesFragmentHandler::createStyleMatrixContext(
    sal_Int32 nElement,
    const AttributeList& rAttribs,
    ShapeStyleRef& o_rStyle )
{
    o_rStyle.mnThemedIdx = (nElement == DGM_TOKEN(fontRef)) ?
        rAttribs.getToken( XML_idx, XML_none ) : rAttribs.getInteger( XML_idx, 0 );
    return new ColorContext( *this, o_rStyle.maPhClr );
}

::oox::core::ContextHandlerRef DiagramQStylesFragmentHandler::onCreateContext( sal_Int32 nElement,
                                                                               const AttributeList& rAttribs )
{
    // state-table like way of navigating the color fragment. we
    // currently ignore everything except styleLbl in the colorsDef
    // element
    switch( getCurrentElement() )
    {
        case XML_ROOT_CONTEXT:
            return nElement == DGM_TOKEN(styleDef) ? this : NULL;
        case DGM_TOKEN(styleDef):
            return nElement == DGM_TOKEN(styleLbl) ? this : NULL;
        case DGM_TOKEN(styleLbl):
            return nElement == DGM_TOKEN(style) ? this : NULL;
        case DGM_TOKEN(style):
        {
            switch( nElement )
            {
                case DGM_TOKEN(lnRef) :     // CT_StyleMatrixReference
                    return createStyleMatrixContext(nElement,rAttribs,
                                                    maStyleEntry.maLineStyle);
                case DGM_TOKEN(fillRef) :   // CT_StyleMatrixReference
                    return createStyleMatrixContext(nElement,rAttribs,
                                                    maStyleEntry.maFillStyle);
                case DGM_TOKEN(effectRef) : // CT_StyleMatrixReference
                    return createStyleMatrixContext(nElement,rAttribs,
                                                    maStyleEntry.maEffectStyle);
                case DGM_TOKEN(fontRef) :   // CT_FontRe    ference
                    return createStyleMatrixContext(nElement,rAttribs,
                                                    maStyleEntry.maTextStyle);
            }
            return 0;
        }
    }

    return 0;
}


void DiagramQStylesFragmentHandler::onStartElement( const AttributeList& rAttribs )
{
    if( getCurrentElement() == DGM_TOKEN( styleDef ) )
    {
        maStyleName = rAttribs.getString( XML_name, OUString() );
        maStyleEntry = mrStylesMap[maStyleName];
    }
}

/////////////////////

void DiagramQStylesFragmentHandler::onEndElement( )
{
    if( getCurrentElement() == DGM_TOKEN(styleLbl) )
        mrStylesMap[maStyleName] = maStyleEntry;
}

ColorFragmentHandler::ColorFragmentHandler( ::oox::core::XmlFilterBase& rFilter,
                                            const ::rtl::OUString& rFragmentPath,
                                            DiagramColorMap& rColorsMap ) :
    FragmentHandler2(rFilter,rFragmentPath),
    maColorName(),
    maColorEntry(),
    mrColorsMap(rColorsMap)
{}

::oox::core::ContextHandlerRef ColorFragmentHandler::onCreateContext( sal_Int32 nElement,
                                                                      const AttributeList& /*rAttribs*/ )
{
    // state-table like way of navigating the color fragment. we
    // currently ignore everything except styleLbl in the colorsDef
    // element
    switch( getCurrentElement() )
    {
        case XML_ROOT_CONTEXT:
            return nElement == DGM_TOKEN(colorsDef) ? this : NULL;
        case DGM_TOKEN(colorsDef):
            return nElement == DGM_TOKEN(styleLbl) ? this : NULL;
        case DGM_TOKEN(styleLbl):
            return ((nElement == DGM_TOKEN(fillClrLst)) ||
                    (nElement == DGM_TOKEN(linClrLst)) ||
                    (nElement == DGM_TOKEN(effectClrLst)) ||
                    (nElement == DGM_TOKEN(txLinClrLst)) ||
                    (nElement == DGM_TOKEN(txFillClrLst)) ||
                    (nElement == DGM_TOKEN(txEffectClrLst))) ? this : NULL;

        // the actual colors - defer to color fragment handlers.

        // TODO(F1): well, actually, there might be *several* color
        // definitions in it, after all it's called list. but
        // apparently colorChoiceContext doesn't handle that anyway...
        case DGM_TOKEN(fillClrLst):
            return new ColorContext( *this, maColorEntry.maFillColor );
        case DGM_TOKEN(linClrLst):
            return new ColorContext( *this, maColorEntry.maLineColor );
        case DGM_TOKEN(effectClrLst):
            return new ColorContext( *this, maColorEntry.maEffectColor );
        case DGM_TOKEN(txFillClrLst):
            return new ColorContext( *this, maColorEntry.maTextFillColor );
        case DGM_TOKEN(txLinClrLst):
            return new ColorContext( *this, maColorEntry.maTextLineColor );
        case DGM_TOKEN(txEffectClrLst):
            return new ColorContext( *this, maColorEntry.maTextEffectColor );
    }

    return 0;
}


void ColorFragmentHandler::onStartElement( const AttributeList& rAttribs )
{
    if( getCurrentElement() == DGM_TOKEN(styleLbl) )
    {
        maColorName = rAttribs.getString( XML_name, OUString() );
        maColorEntry = mrColorsMap[maColorName];
    }
}

void ColorFragmentHandler::onEndElement( )
{
    if( getCurrentElement() == DGM_TOKEN(styleLbl) )
        mrColorsMap[maColorName] = maColorEntry;
}


} }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

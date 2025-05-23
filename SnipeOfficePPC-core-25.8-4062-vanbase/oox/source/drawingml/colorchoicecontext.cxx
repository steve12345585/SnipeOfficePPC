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

#include "oox/drawingml/colorchoicecontext.hxx"
#include "oox/helper/attributelist.hxx"
#include "oox/drawingml/color.hxx"

using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::RuntimeException;
using ::com::sun::star::xml::sax::SAXException;
using ::com::sun::star::xml::sax::XFastAttributeList;
using ::com::sun::star::xml::sax::XFastContextHandler;
using ::oox::core::ContextHandler;

namespace oox {
namespace drawingml {

// ============================================================================

ColorValueContext::ColorValueContext( ContextHandler& rParent, Color& rColor ) :
    ContextHandler( rParent ),
    mrColor( rColor )
{
}

void ColorValueContext::startFastElement( sal_Int32 nElement, const Reference< XFastAttributeList >& rxAttribs )
    throw (SAXException, RuntimeException)
{
    AttributeList aAttribs( rxAttribs );
    switch( nElement )
    {
        case A_TOKEN( scrgbClr ):
            mrColor.setScrgbClr(
                aAttribs.getInteger( XML_r, 0 ),
                aAttribs.getInteger( XML_g, 0 ),
                aAttribs.getInteger( XML_b, 0 ) );
        break;

        case A_TOKEN( srgbClr ):
            mrColor.setSrgbClr( aAttribs.getIntegerHex( XML_val, 0 ) );
        break;

        case A_TOKEN( hslClr ):
            mrColor.setHslClr(
                aAttribs.getInteger( XML_hue, 0 ),
                aAttribs.getInteger( XML_sat, 0 ),
                aAttribs.getInteger( XML_lum, 0 ) );
        break;

        case A_TOKEN( sysClr ):
            mrColor.setSysClr(
                aAttribs.getToken( XML_val, XML_TOKEN_INVALID ),
                aAttribs.getIntegerHex( XML_lastClr, -1 ) );
        break;

        case A_TOKEN( schemeClr ):
            mrColor.setSchemeClr( aAttribs.getToken( XML_val, XML_TOKEN_INVALID ) );
        break;

        case A_TOKEN( prstClr ):
            mrColor.setPrstClr( aAttribs.getToken( XML_val, XML_TOKEN_INVALID ) );
        break;
    }
}

Reference< XFastContextHandler > ColorValueContext::createFastChildContext(
        sal_Int32 nElement, const Reference< XFastAttributeList >& rxAttribs ) throw (SAXException, RuntimeException)
{
    AttributeList aAttribs( rxAttribs );
    switch( nElement )
    {
        case A_TOKEN( alpha ):
        case A_TOKEN( alphaMod ):
        case A_TOKEN( alphaOff ):
        case A_TOKEN( blue ):
        case A_TOKEN( blueMod ):
        case A_TOKEN( blueOff ):
        case A_TOKEN( hue ):
        case A_TOKEN( hueMod ):
        case A_TOKEN( hueOff ):
        case A_TOKEN( lum ):
        case A_TOKEN( lumMod ):
        case A_TOKEN( lumOff ):
        case A_TOKEN( green ):
        case A_TOKEN( greenMod ):
        case A_TOKEN( greenOff ):
        case A_TOKEN( red ):
        case A_TOKEN( redMod ):
        case A_TOKEN( redOff ):
        case A_TOKEN( sat ):
        case A_TOKEN( satMod ):
        case A_TOKEN( satOff ):
        case A_TOKEN( shade ):
        case A_TOKEN( tint ):
            mrColor.addTransformation( nElement, aAttribs.getInteger( XML_val, 0 ) );
        break;
        case A_TOKEN( comp ):
        case A_TOKEN( gamma ):
        case A_TOKEN( gray ):
        case A_TOKEN( inv ):
        case A_TOKEN( invGamma ):
            mrColor.addTransformation( nElement );
        break;
    }
    return 0;
}

// ============================================================================

ColorContext::ColorContext( ContextHandler& rParent, Color& rColor ) :
    ContextHandler( rParent ),
    mrColor( rColor )
{
}

Reference< XFastContextHandler > ColorContext::createFastChildContext(
        sal_Int32 nElement, const Reference< XFastAttributeList >& ) throw (SAXException, RuntimeException)
{
    switch( nElement )
    {
        case A_TOKEN( scrgbClr ):
        case A_TOKEN( srgbClr ):
        case A_TOKEN( hslClr ):
        case A_TOKEN( sysClr ):
        case A_TOKEN( schemeClr ):
        case A_TOKEN( prstClr ):
            return new ColorValueContext( *this, mrColor );
    }
    return 0;
}

// ============================================================================

} // namespace drawingml
} // namespace oox

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

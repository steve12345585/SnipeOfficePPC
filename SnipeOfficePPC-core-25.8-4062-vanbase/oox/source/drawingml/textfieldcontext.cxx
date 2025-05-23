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

#include "oox/drawingml/textfieldcontext.hxx"
#include "oox/drawingml/textparagraphpropertiescontext.hxx"
#include "oox/drawingml/textcharacterpropertiescontext.hxx"
#include "oox/drawingml/textfield.hxx"

using ::rtl::OUString;
using namespace ::oox::core;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;

namespace oox { namespace drawingml {

TextFieldContext::TextFieldContext( ContextHandler& rParent,
            const Reference< XFastAttributeList >& rXAttributes,
            TextField& rTextField)
    : ContextHandler( rParent )
        , mrTextField( rTextField )
        , mbIsInText( false )
{
    mrTextField.setUuid( rXAttributes->getOptionalValue( XML_id ) );
    mrTextField.setType( rXAttributes->getOptionalValue( XML_type ) );
}

void TextFieldContext::endFastElement( sal_Int32 aElementToken ) throw (SAXException, RuntimeException)
{
    if( aElementToken == (A_TOKEN( t )) )
    {
        mbIsInText = false;
    }
}

void TextFieldContext::characters( const OUString& aChars ) throw (SAXException, RuntimeException)
{
    if( mbIsInText )
    {
        mrTextField.getText() += aChars;
    }
}

Reference< XFastContextHandler > TextFieldContext::createFastChildContext( sal_Int32 aElementToken, const Reference< XFastAttributeList >& xAttribs )
    throw (SAXException, RuntimeException)
{
    Reference< XFastContextHandler > xRet;
    switch( aElementToken )
    {
    case A_TOKEN( rPr ):
        xRet.set( new TextCharacterPropertiesContext( *this, xAttribs, mrTextField.getTextCharacterProperties() ) );
        break;
    case A_TOKEN( pPr ):
        xRet.set( new TextParagraphPropertiesContext( *this, xAttribs, mrTextField.getTextParagraphProperties() ) );
        break;
    case A_TOKEN( t ):
        mbIsInText = true;
        break;
    }
    if ( !xRet.is() )
        xRet.set( this );
    return xRet;
}


} }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

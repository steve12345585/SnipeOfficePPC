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

#include "diagramdefinitioncontext.hxx"
#include "layoutnodecontext.hxx"
#include "datamodelcontext.hxx"

using namespace ::oox::core;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;
using ::rtl::OUString;

namespace oox { namespace drawingml {


// CT_DiagramDefinition
DiagramDefinitionContext::DiagramDefinitionContext( ContextHandler& rParent,
                                                    const Reference< XFastAttributeList >& xAttributes,
                                                    const DiagramLayoutPtr &pLayout )
    : ContextHandler( rParent )
    , mpLayout( pLayout )
{
    OSL_TRACE( "OOX: DiagramDefinitionContext::DiagramDefinitionContext()" );
    mpLayout->setDefStyle( xAttributes->getOptionalValue( XML_defStyle ) );
    OUString sValue = xAttributes->getOptionalValue( XML_minVer );
    if( sValue.isEmpty() )
    {
        sValue = CREATE_OUSTRING( "http://schemas.openxmlformats.org/drawingml/2006/diagram" );
    }
    mpLayout->setMinVer( sValue );
    mpLayout->setUniqueId( xAttributes->getOptionalValue( XML_uniqueId ) );
}


DiagramDefinitionContext::~DiagramDefinitionContext()
{
    mpLayout->getNode()->dump(0);
}

void SAL_CALL DiagramDefinitionContext::endFastElement( ::sal_Int32 )
    throw (SAXException, RuntimeException)
{

}


Reference< XFastContextHandler > SAL_CALL
DiagramDefinitionContext::createFastChildContext( ::sal_Int32 aElement,
                                                  const Reference< XFastAttributeList >& xAttribs )
    throw (SAXException, RuntimeException)
{
    Reference< XFastContextHandler > xRet;

    switch( aElement )
    {
    case DGM_TOKEN( title ):
        mpLayout->setTitle( xAttribs->getOptionalValue( XML_val ) );
        break;
    case DGM_TOKEN( desc ):
        mpLayout->setDesc( xAttribs->getOptionalValue( XML_val ) );
        break;
    case DGM_TOKEN( layoutNode ):
    {
        LayoutNodePtr pNode( new LayoutNode() );
        mpLayout->getNode() = pNode;
        pNode->setChildOrder( xAttribs->getOptionalValueToken( XML_chOrder, XML_b ) );
        pNode->setMoveWith( xAttribs->getOptionalValue( XML_moveWith ) );
        pNode->setStyleLabel( xAttribs->getOptionalValue( XML_styleLbl ) );
        xRet.set( new LayoutNodeContext( *this, xAttribs, pNode ) );
        break;
    }
     case DGM_TOKEN( clrData ):
        // TODO, does not matter for the UI. skip.
        return xRet;
    case DGM_TOKEN( sampData ):
        mpLayout->getSampData().reset( new DiagramData );
        xRet.set( new DataModelContext( *this, mpLayout->getSampData() ) );
        break;
    case DGM_TOKEN( styleData ):
        mpLayout->getStyleData().reset( new DiagramData );
        xRet.set( new DataModelContext( *this, mpLayout->getStyleData() ) );
        break;
    case DGM_TOKEN( cat ):
    case DGM_TOKEN( catLst ):
        // TODO, does not matter for the UI
    default:
        break;
    }
    if( !xRet.is() )
        xRet.set(this);

    return xRet;
}


} }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

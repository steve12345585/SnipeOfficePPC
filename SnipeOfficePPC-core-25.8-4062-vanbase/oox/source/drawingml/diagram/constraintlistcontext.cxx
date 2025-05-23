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

#include "constraintlistcontext.hxx"
#include "oox/helper/attributelist.hxx"

using namespace ::oox::core;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;
using ::rtl::OUString;

namespace oox { namespace drawingml {

// CT_ConstraintLists
ConstraintListContext::ConstraintListContext( ContextHandler& rParent,
                                              const Reference< XFastAttributeList >&,
                                              const LayoutAtomPtr &pNode )
    : ContextHandler( rParent )
    , mpNode( pNode )
{
    OSL_ENSURE( pNode, "Node must NOT be NULL" );
}


ConstraintListContext::~ConstraintListContext()
{
}

void SAL_CALL ConstraintListContext::endFastElement( ::sal_Int32 )
    throw (SAXException, RuntimeException)
{
}

Reference< XFastContextHandler > SAL_CALL
ConstraintListContext::createFastChildContext( ::sal_Int32 aElement,
                                               const Reference< XFastAttributeList >& xAttribs )
    throw (SAXException, RuntimeException)
{
    Reference< XFastContextHandler > xRet;

    OUString aEmptyStr;

    switch( aElement )
    {
    case DGM_TOKEN( constr ):
    {
        ConstraintAtomPtr pNode( new ConstraintAtom() );
        mpNode->addChild( pNode );

        AttributeList aAttribs( xAttribs );
        pNode->setFor( aAttribs.getToken( XML_for, XML_none ) );
        pNode->setForName( aAttribs.getString( XML_forName, aEmptyStr ) );
        pNode->setPointType( aAttribs.getToken( XML_ptType, XML_none ) );
        pNode->setType( aAttribs.getToken( XML_type, XML_none ) );
        pNode->setRefFor( aAttribs.getToken( XML_refFor, XML_none ) );
        pNode->setRefForName( aAttribs.getString( XML_refForName, aEmptyStr ) );
        pNode->setRefType( aAttribs.getToken( XML_refType, XML_none ) );
        pNode->setRefPointType( aAttribs.getToken( XML_refPtType, XML_none ) );
        pNode->setFactor( aAttribs.getDouble( XML_fact, 1.0 ) );
        pNode->setValue( aAttribs.getDouble( XML_val, 0.0 ) );
        pNode->setOperator( aAttribs.getToken( XML_op, XML_none ) );
        break;
    }
    default:
        break;
    }
    if( !xRet.is() )
        xRet.set(this);

    return xRet;
}


} }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

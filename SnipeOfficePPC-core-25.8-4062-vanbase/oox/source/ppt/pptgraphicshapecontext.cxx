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

#include <com/sun/star/xml/sax/FastToken.hpp>
#include <com/sun/star/drawing/LineStyle.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XNamed.hpp>

#include "oox/ppt/pptshape.hxx"
#include "oox/ppt/pptgraphicshapecontext.hxx"
#include "oox/ppt/pptshapepropertiescontext.hxx"
#include "oox/ppt/slidepersist.hxx"
#include "oox/drawingml/shapestylecontext.hxx"
#include "oox/token/namespaces.hxx"
#include "oox/drawingml/fillpropertiesgroupcontext.hxx"
#include "oox/drawingml/lineproperties.hxx"
#include "oox/drawingml/drawingmltypes.hxx"
#include "oox/drawingml/customshapegeometry.hxx"
#include "oox/drawingml/textbodycontext.hxx"
#include <oox/token/tokens.hxx>

using rtl::OUString;
using namespace oox::core;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::xml::sax;

namespace oox { namespace ppt {

// CT_Shape
PPTGraphicShapeContext::PPTGraphicShapeContext( ContextHandler& rParent, const SlidePersistPtr pSlidePersistPtr, oox::drawingml::ShapePtr pMasterShapePtr, oox::drawingml::ShapePtr pShapePtr )
: oox::drawingml::GraphicShapeContext( rParent, pMasterShapePtr, pShapePtr )
, mpSlidePersistPtr( pSlidePersistPtr )
{
}

// if nFirstPlaceholder can't be found, it will be searched for nSecondPlaceholder
static oox::drawingml::ShapePtr findPlaceholder( sal_Int32 nFirstPlaceholder, sal_Int32 nSecondPlaceholder, std::vector< oox::drawingml::ShapePtr >& rShapes )
{
    oox::drawingml::ShapePtr pPlaceholder = PPTShape::findPlaceholder( nFirstPlaceholder, rShapes );
    return !nSecondPlaceholder || pPlaceholder.get() ? pPlaceholder : PPTShape::findPlaceholder( nSecondPlaceholder, rShapes );
}

Reference< XFastContextHandler > PPTGraphicShapeContext::createFastChildContext( sal_Int32 aElementToken, const Reference< XFastAttributeList >& xAttribs ) throw (SAXException, RuntimeException)
{
    Reference< XFastContextHandler > xRet;

    switch( aElementToken )
    {
    // nvSpPr CT_ShapeNonVisual begin
//  case NMSP_PPT|XML_drElemPr:
//      break;
    case PPT_TOKEN(cNvPr):
        mpShapePtr->setId( xAttribs->getOptionalValue( XML_id ) );
        mpShapePtr->setName( xAttribs->getOptionalValue( XML_name ) );
        break;
    case PPT_TOKEN(ph):
    {
        sal_Int32 nSubType( xAttribs->getOptionalValueToken( XML_type, XML_obj ) );
        mpShapePtr->setSubType( nSubType );
        OUString sIdx( xAttribs->getOptionalValue( XML_idx ) );
        sal_Bool bHasIdx = !sIdx.isEmpty();
        sal_Int32 nIdx = sIdx.toInt32();
        if( xAttribs->hasAttribute( XML_idx ) )
            mpShapePtr->setSubTypeIndex( nIdx );

        if ( nSubType || bHasIdx )
        {
            PPTShape* pPPTShapePtr = dynamic_cast< PPTShape* >( mpShapePtr.get() );
            if ( pPPTShapePtr )
            {
                oox::ppt::ShapeLocation eShapeLocation = pPPTShapePtr->getShapeLocation();
                oox::drawingml::ShapePtr pPlaceholder;

                if ( bHasIdx && eShapeLocation == Slide )
                {
                    // TODO: use id to shape map
                    SlidePersistPtr pMasterPersist( mpSlidePersistPtr->getMasterPersist() );
                    if ( pMasterPersist.get() && xAttribs->hasAttribute( XML_idx ) )
                        pPlaceholder = PPTShape::findPlaceholderByIndex( nIdx, pMasterPersist->getShapes()->getChildren() );
                }
                if ( !pPlaceholder.get() && ( ( eShapeLocation == Slide ) || ( eShapeLocation == Layout ) ) )
                {
                    // inheriting properties from placeholder objects by cloning shape

                    sal_Int32 nFirstPlaceholder = 0;
                    sal_Int32 nSecondPlaceholder = 0;
                    switch( nSubType )
                    {
                        case XML_ctrTitle :     // slide/layout
                            nFirstPlaceholder = XML_ctrTitle;
                            nSecondPlaceholder = XML_title;
                            break;
                        case XML_subTitle :     // slide/layout
                            nFirstPlaceholder = XML_subTitle;
                            nSecondPlaceholder = XML_title;
                            break;
                        case XML_obj :          // slide/layout
                            nFirstPlaceholder = XML_body;
                            break;
                        case XML_dt :           // slide/layout/master/notes/notesmaster/handoutmaster
                        case XML_sldNum :       // slide/layout/master/notes/notesmaster/handoutmaster
                        case XML_ftr :          // slide/layout/master/notes/notesmaster/handoutmaster
                        case XML_hdr :          // notes/notesmaster/handoutmaster
                        case XML_body :         // slide/layout/master/notes/notesmaster
                        case XML_title :        // slide/layout/master/
                        case XML_chart :        // slide/layout
                        case XML_tbl :          // slide/layout
                        case XML_clipArt :      // slide/layout
                        case XML_dgm :          // slide/layout
                        case XML_media :        // slide/layout
                        case XML_sldImg :       // notes/notesmaster
                        case XML_pic :          // slide/layout
                            nFirstPlaceholder = nSubType;
                        default:
                            break;
                    }
                    if ( nFirstPlaceholder )
                    {
                        if ( eShapeLocation == Layout )     // for layout objects the referenced object can be found within the same shape tree
                            pPlaceholder = findPlaceholder( nFirstPlaceholder, nSecondPlaceholder, mpSlidePersistPtr->getShapes()->getChildren() );
                        else if ( eShapeLocation == Slide ) // normal slide shapes have to search within the corresponding master tree for referenced objects
                        {
                            SlidePersistPtr pMasterPersist( mpSlidePersistPtr->getMasterPersist() );
                            if ( pMasterPersist.get() )
                                pPlaceholder = findPlaceholder( nFirstPlaceholder, nSecondPlaceholder, pMasterPersist->getShapes()->getChildren() );
                        }
                    }
                }
                if ( pPlaceholder.get() )
                {
                    bool bUseText = true;
                    // TODO: Check if pPlaceholder->getSubType is none (i.e. none explicitly specified)
                    if( pPlaceholder->getSubType() == XML_obj )
                        bUseText = false;
                    mpShapePtr->applyShapeReference( *pPlaceholder.get(), bUseText );
                    PPTShape* pPPTShape = dynamic_cast< PPTShape* >( pPlaceholder.get() );
                    if ( pPPTShape )
                        pPPTShape->setReferenced( sal_True );
                    pPPTShapePtr->setPlaceholder( pPlaceholder );
                }
            }
        }
        break;
    }
    // nvSpPr CT_ShapeNonVisual end

    case PPT_TOKEN(spPr):
        xRet = new PPTShapePropertiesContext( *this, *mpShapePtr );
        break;

    case PPT_TOKEN(style):
        xRet = new oox::drawingml::ShapeStyleContext( *this, *mpShapePtr );
        break;

    case PPT_TOKEN(txBody):
    {
        oox::drawingml::TextBodyPtr xTextBody( new oox::drawingml::TextBody );
        mpShapePtr->setTextBody( xTextBody );
        xRet = new oox::drawingml::TextBodyContext( *this, *xTextBody );
        break;
    }
    }

    if( !xRet.is() )
        xRet.set( GraphicShapeContext::createFastChildContext( aElementToken, xAttribs ) );

    return xRet;
}


} }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

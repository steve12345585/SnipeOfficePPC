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

#include "XMLChartStyleContext.hxx"
#include <xmloff/xmltoken.hxx>
#include "xmloff/xmlnmspe.hxx"
#include <xmloff/xmlnumfi.hxx>
#include <xmloff/families.hxx>

#include "XMLChartPropertyContext.hxx"

using namespace com::sun::star;
using ::xmloff::token::IsXMLToken;
using ::xmloff::token::XML_DATA_STYLE_NAME;
using ::xmloff::token::XML_PERCENTAGE_DATA_STYLE_NAME;
using ::xmloff::token::XML_TEXT_PROPERTIES;
using ::xmloff::token::XML_PARAGRAPH_PROPERTIES;
using ::xmloff::token::XML_GRAPHIC_PROPERTIES;
using ::xmloff::token::XML_CHART_PROPERTIES;


TYPEINIT1( XMLChartStyleContext, XMLPropStyleContext );

// protected

void XMLChartStyleContext::SetAttribute(
    sal_uInt16 nPrefixKey,
    const ::rtl::OUString& rLocalName,
    const ::rtl::OUString& rValue )
{
    if( IsXMLToken( rLocalName, XML_DATA_STYLE_NAME ) )
    {
        msDataStyleName =rValue;
    }
    else if( IsXMLToken( rLocalName, XML_PERCENTAGE_DATA_STYLE_NAME ) )
    {
        msPercentageDataStyleName =rValue;
    }
    else
    {
        XMLShapeStyleContext::SetAttribute( nPrefixKey, rLocalName, rValue );
    }
}

//public

// CTOR
XMLChartStyleContext::XMLChartStyleContext(
    SvXMLImport& rImport, sal_uInt16 nPrfx,
    const ::rtl::OUString& rLName,
    const uno::Reference< xml::sax::XAttributeList > & xAttrList,
    SvXMLStylesContext& rStyles, sal_uInt16 nFamily ) :

        XMLShapeStyleContext( rImport, nPrfx, rLName, xAttrList, rStyles, nFamily ),
        mrStyles( rStyles )
{}

// DTOR
XMLChartStyleContext::~XMLChartStyleContext()
{}

namespace
{

    void lcl_NumberFormatStyleToProperty( const ::rtl::OUString& rStyleName, const ::rtl::OUString& rPropertyName,
                                      const SvXMLStylesContext& rStylesContext,
                                      const uno::Reference< beans::XPropertySet >& rPropSet )
{
    if( !rStyleName.isEmpty())
    {
        SvXMLNumFormatContext* pStyle = (SvXMLNumFormatContext *)rStylesContext.FindStyleChildContext(
            XML_STYLE_FAMILY_DATA_STYLE, rStyleName, sal_True );
        if( pStyle )
        {
            uno::Any aNumberFormat;
            sal_Int32 nNumberFormat = pStyle->GetKey();
            aNumberFormat <<= nNumberFormat;
            rPropSet->setPropertyValue( rPropertyName, aNumberFormat );
        }
    }
}

}// anonymous namespace

void XMLChartStyleContext::FillPropertySet(
    const uno::Reference< beans::XPropertySet > & rPropSet )
{
    try
    {
        XMLShapeStyleContext::FillPropertySet( rPropSet );
    }
    catch( beans::UnknownPropertyException&  )
    {
        DBG_ASSERT( false, "unknown property exception -> shape style not completly imported for chart style" );
    }

    lcl_NumberFormatStyleToProperty( msDataStyleName, rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "NumberFormat" )), mrStyles, rPropSet );
    lcl_NumberFormatStyleToProperty( msPercentageDataStyleName, rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PercentageNumberFormat" )), mrStyles, rPropSet );
}

SvXMLImportContext *XMLChartStyleContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const ::rtl::OUString& rLocalName,
    const uno::Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext* pContext = NULL;

    if( XML_NAMESPACE_STYLE == nPrefix )
    {
        sal_uInt32 nFamily = 0;
        if( IsXMLToken( rLocalName, XML_TEXT_PROPERTIES ) )
            nFamily = XML_TYPE_PROP_TEXT;
        else if( IsXMLToken( rLocalName, XML_PARAGRAPH_PROPERTIES ) )
            nFamily = XML_TYPE_PROP_PARAGRAPH;
        else if( IsXMLToken( rLocalName, XML_GRAPHIC_PROPERTIES ) )
            nFamily = XML_TYPE_PROP_GRAPHIC;
        else if( IsXMLToken( rLocalName, XML_CHART_PROPERTIES ) )
            nFamily = XML_TYPE_PROP_CHART;
        if( nFamily )
        {
            UniReference < SvXMLImportPropertyMapper > xImpPrMap =
                GetStyles()->GetImportPropertyMapper( GetFamily() );
            if( xImpPrMap.is() )
                pContext = new XMLChartPropertyContext(
                    GetImport(), nPrefix, rLocalName, xAttrList, nFamily,
                    GetProperties(), xImpPrMap );
        }
    }

    if( !pContext )
        pContext = XMLShapeStyleContext::CreateChildContext( nPrefix, rLocalName,
                                                             xAttrList );

    return pContext;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#include <com/sun/star/style/TabAlign.hpp>
#include <rtl/ustrbuf.hxx>
#include <xmloff/xmltkmap.hxx>
#include <xmloff/nmspmap.hxx>
#include "xmloff/xmlnmspe.hxx"
#include <xmloff/xmlimp.hxx>
#include <com/sun/star/style/TabStop.hpp>
#include <xmloff/xmltoken.hxx>
#include "xmloff/i18nmap.hxx"
#include <xmloff/xmluconv.hxx>
#include <svl/svstdarr.hxx>
#include "xmltabi.hxx"

using ::rtl::OUString;
using ::rtl::OUStringBuffer;

using namespace ::com::sun::star;
using namespace ::xmloff::token;

enum SvXMLTokenMapAttrs
{
    XML_TOK_TABSTOP_POSITION,
    XML_TOK_TABSTOP_TYPE,
    XML_TOK_TABSTOP_CHAR,
    XML_TOK_TABSTOP_LEADER_STYLE,
    XML_TOK_TABSTOP_LEADER_TEXT,
    XML_TOK_TABSTOP_END=XML_TOK_UNKNOWN
};

static SvXMLTokenMapEntry aTabsAttributesAttrTokenMap[] =
{
    { XML_NAMESPACE_STYLE, XML_POSITION,     XML_TOK_TABSTOP_POSITION },
    { XML_NAMESPACE_STYLE, XML_TYPE,         XML_TOK_TABSTOP_TYPE },
    { XML_NAMESPACE_STYLE, XML_CHAR,         XML_TOK_TABSTOP_CHAR },
    { XML_NAMESPACE_STYLE, XML_LEADER_TEXT,  XML_TOK_TABSTOP_LEADER_TEXT },
    { XML_NAMESPACE_STYLE, XML_LEADER_STYLE,  XML_TOK_TABSTOP_LEADER_STYLE },
    XML_TOKEN_MAP_END
};

// ---

class SvxXMLTabStopContext_Impl : public SvXMLImportContext
{
private:
     style::TabStop aTabStop;

public:
    TYPEINFO();

    SvxXMLTabStopContext_Impl( SvXMLImport& rImport, sal_uInt16 nPrfx,
                               const OUString& rLName,
                               const uno::Reference< xml::sax::XAttributeList > & xAttrList );

    virtual ~SvxXMLTabStopContext_Impl();

    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                                   const OUString& rLocalName,
                                   const uno::Reference< xml::sax::XAttributeList > & xAttrList );

    const style::TabStop& getTabStop() const { return aTabStop; }
};

TYPEINIT1( SvxXMLTabStopContext_Impl, SvXMLImportContext );

SvxXMLTabStopContext_Impl::SvxXMLTabStopContext_Impl(
                               SvXMLImport& rImport, sal_uInt16 nPrfx,
                               const OUString& rLName,
                               const uno::Reference< xml::sax::XAttributeList > & xAttrList )
: SvXMLImportContext( rImport, nPrfx, rLName )
{
    aTabStop.Position = 0;
    aTabStop.Alignment = style::TabAlign_LEFT;
    aTabStop.DecimalChar = sal_Unicode( ',' );
    aTabStop.FillChar = sal_Unicode( ' ' );
    sal_Unicode cTextFillChar = 0;

    SvXMLTokenMap aTokenMap( aTabsAttributesAttrTokenMap );

    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        const OUString& rAttrName = xAttrList->getNameByIndex( i );
        OUString aLocalName;
        sal_uInt16 nPrefix =
            GetImport().GetNamespaceMap().GetKeyByAttrName( rAttrName,
                                                            &aLocalName );
        const OUString& rValue = xAttrList->getValueByIndex( i );

        sal_Int32 nVal;
        switch( aTokenMap.Get( nPrefix, aLocalName ) )
        {
        case XML_TOK_TABSTOP_POSITION:
            if (GetImport().GetMM100UnitConverter().convertMeasureToCore(
                    nVal, rValue))
            {
                aTabStop.Position = nVal;
            }
            break;
        case XML_TOK_TABSTOP_TYPE:
            if( IsXMLToken( rValue, XML_LEFT ) )
            {
                aTabStop.Alignment = style::TabAlign_LEFT;
            }
            else if( IsXMLToken( rValue, XML_RIGHT ) )
            {
                aTabStop.Alignment = style::TabAlign_RIGHT;
            }
            else if( IsXMLToken( rValue, XML_CENTER ) )
            {
                aTabStop.Alignment = style::TabAlign_CENTER;
            }
            else if( IsXMLToken( rValue, XML_CHAR ) )
            {
                aTabStop.Alignment = style::TabAlign_DECIMAL;
            }
            else if( IsXMLToken( rValue, XML_DEFAULT ) )
            {
                aTabStop.Alignment = style::TabAlign_DEFAULT;
            }
            break;
        case XML_TOK_TABSTOP_CHAR:
            if( !rValue.isEmpty() )
                aTabStop.DecimalChar = rValue[0];
            break;
        case XML_TOK_TABSTOP_LEADER_STYLE:
            if( IsXMLToken( rValue, XML_NONE ) )
                aTabStop.FillChar = ' ';
            else if( IsXMLToken( rValue, XML_DOTTED ) )
                aTabStop.FillChar = '.';
            else
                aTabStop.FillChar = '_';
            break;
        case XML_TOK_TABSTOP_LEADER_TEXT:
            if( !rValue.isEmpty() )
                cTextFillChar = rValue[0];
            break;
        }
    }

    if( cTextFillChar != 0 && aTabStop.FillChar != ' ' )
        aTabStop.FillChar = cTextFillChar;
}

SvxXMLTabStopContext_Impl::~SvxXMLTabStopContext_Impl()
{
}

SvXMLImportContext *SvxXMLTabStopContext_Impl::CreateChildContext(
                                   sal_uInt16 nPrefix,
                                   const OUString& rLocalName,
                                   const uno::Reference< xml::sax::XAttributeList > & )
{
    return new SvXMLImportContext( GetImport(), nPrefix, rLocalName );
}




class SvxXMLTabStopArray_Impl : public std::vector<SvxXMLTabStopContext_Impl *> {};


// ---

TYPEINIT1( SvxXMLTabStopImportContext, XMLElementPropertyContext );

SvxXMLTabStopImportContext::SvxXMLTabStopImportContext(
                                SvXMLImport& rImport, sal_uInt16 nPrfx,
                                const OUString& rLName,
                                const XMLPropertyState& rProp,
                                 ::std::vector< XMLPropertyState > &rProps )
: XMLElementPropertyContext( rImport, nPrfx, rLName, rProp, rProps ),
  mpTabStops( NULL )
{
}

SvxXMLTabStopImportContext::~SvxXMLTabStopImportContext()
{
    if( mpTabStops )
    {
        while( !mpTabStops->empty() )
        {
            SvxXMLTabStopContext_Impl *pTabStop = mpTabStops->back();
            mpTabStops->pop_back();
            pTabStop->ReleaseRef();
        }
    }

    delete mpTabStops;
}

SvXMLImportContext *SvxXMLTabStopImportContext::CreateChildContext(
                                   sal_uInt16 nPrefix,
                                   const OUString& rLocalName,
                                   const uno::Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = 0;

    if( XML_NAMESPACE_STYLE == nPrefix && IsXMLToken( rLocalName, XML_TAB_STOP ) )
    {
        // create new tabstop import context
        SvxXMLTabStopContext_Impl *pTabStopContext =
            new SvxXMLTabStopContext_Impl( GetImport(), nPrefix, rLocalName,
                                           xAttrList );

        // add new tabstop to array of tabstops
        if( !mpTabStops )
            mpTabStops = new SvxXMLTabStopArray_Impl;

        mpTabStops->push_back( pTabStopContext );
        pTabStopContext->AddRef();

        pContext = pTabStopContext;
    }
    else
    {
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );
    }

    return pContext;
}

void SvxXMLTabStopImportContext::EndElement( )
{
    sal_uInt16 nCount = mpTabStops ? mpTabStops->size() : 0;
    uno::Sequence< style::TabStop> aSeq( nCount );

    if( mpTabStops )
    {
        sal_uInt16 nNewCount = 0;

        style::TabStop* pTabStops = aSeq.getArray();
        for( sal_uInt16 i=0; i < nCount; i++ )
        {
            SvxXMLTabStopContext_Impl *pTabStopContext = (*mpTabStops)[i];
            const style::TabStop& rTabStop = pTabStopContext->getTabStop();
            sal_Bool bDflt = style::TabAlign_DEFAULT == rTabStop.Alignment;
            if( !bDflt || 0==i )
            {
                *pTabStops++ = pTabStopContext->getTabStop();
                nNewCount++;
            }
            if( bDflt && 0==i )
                break;
        }

        if( nCount != nNewCount )
            aSeq.realloc( nNewCount );
    }
    aProp.maValue <<= aSeq;

    SetInsert( sal_True );
    XMLElementPropertyContext::EndElement();

}




/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

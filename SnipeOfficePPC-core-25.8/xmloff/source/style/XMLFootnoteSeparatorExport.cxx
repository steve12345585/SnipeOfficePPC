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


#include "XMLFootnoteSeparatorExport.hxx"

#include <tools/debug.hxx>

#include <sax/tools/converter.hxx>

#include <xmloff/xmlexp.hxx>
#include "xmloff/xmlnmspe.hxx"
#include <xmloff/xmluconv.hxx>
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmlprmap.hxx>

#include <xmloff/PageMasterStyleMap.hxx>
#include <com/sun/star/text/HorizontalAdjust.hpp>
#include <rtl/ustrbuf.hxx>


using namespace ::com::sun::star;
using namespace ::xmloff::token;
using ::rtl::OUStringBuffer;
using ::std::vector;

XMLFootnoteSeparatorExport::XMLFootnoteSeparatorExport(SvXMLExport& rExp) :
    rExport(rExp)
{
}

XMLFootnoteSeparatorExport::~XMLFootnoteSeparatorExport()
{
}


void XMLFootnoteSeparatorExport::exportXML(
    const vector<XMLPropertyState> * pProperties,
    sal_uInt32
    #ifdef DBG_UTIL
    nIdx
    #endif
    ,
    const UniReference<XMLPropertySetMapper> & rMapper)
{
    DBG_ASSERT(NULL != pProperties, "Need property states");

    // intialize values
    sal_Int16 eLineAdjust = text::HorizontalAdjust_LEFT;
    sal_Int32 nLineColor = 0;
    sal_Int32 nLineDistance = 0;
    sal_Int8 nLineRelWidth = 0;
    sal_Int32 nLineTextDistance = 0;
    sal_Int16 nLineWeight = 0;
    sal_Int8 nLineStyle = 0;

    // find indices into property map and get values
    sal_uInt32 nCount = pProperties->size();
    for(sal_uInt32 i = 0; i < nCount; i++)
    {
        const XMLPropertyState& rState = (*pProperties)[i];

        if( rState.mnIndex == -1 )
            continue;

        switch (rMapper->GetEntryContextId(rState.mnIndex))
        {
        case CTF_PM_FTN_LINE_ADJUST:
            rState.maValue >>= eLineAdjust;
            break;
        case CTF_PM_FTN_LINE_COLOR:
            rState.maValue >>= nLineColor;
            break;
        case CTF_PM_FTN_DISTANCE:
            rState.maValue >>= nLineDistance;
            break;
        case CTF_PM_FTN_LINE_WIDTH:
            rState.maValue >>= nLineRelWidth;
            break;
        case CTF_PM_FTN_LINE_DISTANCE:
            rState.maValue >>= nLineTextDistance;
            break;
        case CTF_PM_FTN_LINE_WEIGTH:
            DBG_ASSERT( i == nIdx,
                        "received wrong property state index" );
            rState.maValue >>= nLineWeight;
            break;
        case CTF_PM_FTN_LINE_STYLE:
            rState.maValue >>= nLineStyle;
            break;
        }
    }

    OUStringBuffer sBuf;

    // weight/width
    if (nLineWeight > 0)
    {
        rExport.GetMM100UnitConverter().convertMeasureToXML(sBuf, nLineWeight);
        rExport.AddAttribute(XML_NAMESPACE_STYLE, XML_WIDTH,
                             sBuf.makeStringAndClear());
    }

    // line text distance
    if (nLineTextDistance > 0)
    {
        rExport.GetMM100UnitConverter().convertMeasureToXML(sBuf,
                nLineTextDistance);
        rExport.AddAttribute(XML_NAMESPACE_STYLE, XML_DISTANCE_BEFORE_SEP,
                             sBuf.makeStringAndClear());
    }

    // line distance
    if (nLineDistance > 0)
    {
        rExport.GetMM100UnitConverter().convertMeasureToXML(sBuf,
                nLineDistance);
        rExport.AddAttribute(XML_NAMESPACE_STYLE, XML_DISTANCE_AFTER_SEP,
                             sBuf.makeStringAndClear());
    }

    // line style
    static const SvXMLEnumMapEntry aXML_LineStyle_Enum[] =
    {
        { XML_NONE,     0 },
        { XML_SOLID,    1 },
        { XML_DOTTED,   2 },
        { XML_DASH,     3 },
        { XML_TOKEN_INVALID, 0 }
    };
    if (rExport.GetMM100UnitConverter().convertEnum(
            sBuf, nLineStyle, aXML_LineStyle_Enum ) )
    {
        rExport.AddAttribute(XML_NAMESPACE_STYLE, XML_LINE_STYLE,
                sBuf.makeStringAndClear());
    }

    // adjustment
    static const SvXMLEnumMapEntry aXML_HorizontalAdjust_Enum[] =
    {
        { XML_LEFT,     text::HorizontalAdjust_LEFT },
        { XML_CENTER,   text::HorizontalAdjust_CENTER },
        { XML_RIGHT,    text::HorizontalAdjust_RIGHT },
        { XML_TOKEN_INVALID, 0 }
    };

    if (rExport.GetMM100UnitConverter().convertEnum(
        sBuf, eLineAdjust, aXML_HorizontalAdjust_Enum))
    {
        rExport.AddAttribute(XML_NAMESPACE_STYLE, XML_ADJUSTMENT,
                             sBuf.makeStringAndClear());
    }

    // relative line width
    ::sax::Converter::convertPercent(sBuf, nLineRelWidth);
    rExport.AddAttribute(XML_NAMESPACE_STYLE, XML_REL_WIDTH,
                         sBuf.makeStringAndClear());

    // color
    ::sax::Converter::convertColor(sBuf, nLineColor);
    rExport.AddAttribute(XML_NAMESPACE_STYLE, XML_COLOR,
                         sBuf.makeStringAndClear());

    // line-style

    SvXMLElementExport aElem(rExport, XML_NAMESPACE_STYLE,
                             XML_FOOTNOTE_SEP, sal_True, sal_True);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

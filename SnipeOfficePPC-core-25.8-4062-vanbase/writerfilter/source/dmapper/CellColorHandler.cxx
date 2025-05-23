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
#include <CellColorHandler.hxx>
#include <PropertyMap.hxx>
#include <doctok/resourceids.hxx>
#include <ConversionHelper.hxx>
#include <ooxml/resourceids.hxx>
#include <sal/macros.h>
#include "dmapperLoggers.hxx"

#define OOXML_COLOR_AUTO 0x0a //todo: AutoColor needs symbol

namespace writerfilter {
namespace dmapper {

using namespace ::com::sun::star;
using namespace ::writerfilter;


CellColorHandler::CellColorHandler() :
LoggedProperties(dmapper_logger, "CellColorHandler"),
m_nShadowType( 0 ),
m_nColor( 0xffffffff ),
m_nFillColor( 0xffffffff ),
    m_OutputFormat( Form )
{
}

CellColorHandler::~CellColorHandler()
{
}

void CellColorHandler::lcl_attribute(Id rName, Value & rVal)
{
    sal_Int32 nIntValue = rVal.getInt();
    switch( rName )
    {
        case NS_rtf::LN_cellTopColor:
        case NS_rtf::LN_cellLeftColor:
        case NS_rtf::LN_cellBottomColor:
        case NS_rtf::LN_cellRightColor:
            // nIntValue contains the color, directly
        break;
        case NS_ooxml::LN_CT_Shd_val:
        {
            //might be clear, pct5...90, some hatch types
            //TODO: The values need symbolic names!
            m_nShadowType = nIntValue; //clear == 0, solid: 1, pct5: 2, pct50:8, pct95: x3c, horzStripe:0x0e, thinVertStripe: 0x15
        }
        break;
        case NS_ooxml::LN_CT_Shd_fill:
            if( nIntValue == OOXML_COLOR_AUTO )
                nIntValue = 0xffffff; //fill color auto means white
            m_nFillColor = nIntValue;
        break;
        case NS_ooxml::LN_CT_Shd_color:
            if( nIntValue == OOXML_COLOR_AUTO )
                nIntValue = 0; //shading color auto means black
            //color of the shading
            m_nColor = nIntValue;
        break;
        case NS_ooxml::LN_CT_Shd_themeFill:
        case NS_ooxml::LN_CT_Shd_themeFillTint:
        case NS_ooxml::LN_CT_Shd_themeFillShade:
            // ignored
            break;
        default:
            OSL_FAIL( "unknown attribute");
    }
}

void CellColorHandler::lcl_sprm(Sprm & rSprm)
{
    (void)rSprm;
}

TablePropertyMapPtr  CellColorHandler::getProperties()
{
    TablePropertyMapPtr pPropertyMap(new TablePropertyMap);
//code from binary word filter
    static const sal_Int32 eMSGrayScale[] =
    {
        // Nul-Brush
           0,   // 0
        // Solid-Brush
        1000,   // 1
        // percent values
          50,   // 2
         100,   // 3
         200,   // 4
         250,   // 5
         300,   // 6
         400,   // 7
         500,   // 8
         600,   // 9
         700,   // 10
         750,   // 11
         800,   // 12
         900,   // 13
         333, // 14 Dark Horizontal
         333, // 15 Dark Vertical
         333, // 16 Dark Forward Diagonal
         333, // 17 Dark Backward Diagonal
         333, // 18 Dark Cross
         333, // 19 Dark Diagonal Cross
         333, // 20 Horizontal
         333, // 21 Vertical
         333, // 22 Forward Diagonal
         333, // 23 Backward Diagonal
         333, // 24 Cross
         333, // 25 Diagonal Cross
         // some undefined values
         500, // 26
         500, // 27
         500, // 28
         500, // 29
         500, // 30
         500, // 31
         500, // 32
         500, // 33
         500, // 34
         // different shading types
          25,   // 35
          75,   // 36
         125,   // 37
         150,   // 38
         175,   // 39
         225,   // 40
         275,   // 41
         325,   // 42
         350,   // 43
         375,   // 44
         425,   // 45
         450,   // 46
         475,   // 47
         525,   // 48
         550,   // 49
         575,   // 50
         625,   // 51
         650,   // 52
         675,   // 53
         725,   // 54
         775,   // 55
         825,   // 56
         850,   // 57
         875,   // 58
         925,   // 59
         950,   // 60
         975,   // 61
         // und zu guter Letzt:
         970
    };// 62
    if( m_nShadowType >= (sal_Int32)SAL_N_ELEMENTS( eMSGrayScale ) )
        m_nShadowType = 0;

    sal_Int32 nWW8BrushStyle = eMSGrayScale[m_nShadowType];
    sal_Int32 nApplyColor = 0;
    if( !nWW8BrushStyle )
    {
        // Null-Brush
            nApplyColor = m_nFillColor;
    }
    else
    {
        sal_Int32 nFore = m_nColor;
        sal_Int32 nBack = m_nFillColor;

        sal_uInt32 nRed = ((nFore & 0xff0000)>>0x10) * nWW8BrushStyle;
        sal_uInt32 nGreen = ((nFore & 0xff00)>>0x8) * nWW8BrushStyle;
        sal_uInt32 nBlue = (nFore & 0xff) * nWW8BrushStyle;
        nRed += ((nBack & 0xff0000)>>0x10)  * (1000L - nWW8BrushStyle);
        nGreen += ((nBack & 0xff00)>>0x8)* (1000L - nWW8BrushStyle);
        nBlue += (nBack & 0xff) * (1000L - nWW8BrushStyle);

        nApplyColor = ( (nRed/1000) << 0x10 ) + ((nGreen/1000) << 8) + nBlue/1000;
    }

    pPropertyMap->Insert( m_OutputFormat == Form ? PROP_BACK_COLOR
                        : m_OutputFormat == Paragraph ? PROP_PARA_BACK_COLOR
                        : PROP_CHAR_BACK_COLOR, false, uno::makeAny( nApplyColor ));
    return pPropertyMap;
}
} //namespace dmapper
} //namespace writerfilter

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

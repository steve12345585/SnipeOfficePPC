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
#include <MeasureHandler.hxx>
#include <PropertyMap.hxx>
#include <doctok/resourceids.hxx>
#include <ConversionHelper.hxx>
#include <ooxml/resourceids.hxx>
#include <com/sun/star/text/SizeType.hpp>
#include "dmapperLoggers.hxx"

namespace writerfilter {
namespace dmapper {

using namespace ::com::sun::star;



MeasureHandler::MeasureHandler() :
LoggedProperties(dmapper_logger, "MeasureHandler"),
m_nMeasureValue( 0 ),
m_nUnit( -1 ),
m_nRowHeightSizeType( text::SizeType::MIN )
{
}


MeasureHandler::~MeasureHandler()
{
}


void MeasureHandler::lcl_attribute(Id rName, Value & rVal)
{
    sal_Int32 nIntValue = rVal.getInt();
    (void)rName;
    switch( rName )
    {
        case NS_rtf::LN_unit:
        case NS_ooxml::LN_CT_TblWidth_type:// = 90668;
            //can be: NS_ooxml::LN_Value_ST_TblWidth_nil, NS_ooxml::LN_Value_ST_TblWidth_pct,
            //        NS_ooxml::LN_Value_ST_TblWidth_dxa, NS_ooxml::LN_Value_ST_TblWidth_auto;
            m_nUnit = nIntValue;
        break;
        case NS_ooxml::LN_CT_Height_hRule: // 90666;
        {
            OUString sHeightType = rVal.getString();
            if ( sHeightType == "exact" )
                m_nRowHeightSizeType = text::SizeType::FIX;
        }
        break;
        case NS_rtf::LN_trleft:
        case NS_rtf::LN_preferredWidth:
        case NS_ooxml::LN_CT_TblWidth_w:// = 90667;
            m_nMeasureValue = nIntValue;
        break;
        case NS_ooxml::LN_CT_Height_val: // 90665 -- a string value
        {
            m_nUnit = NS_ooxml::LN_Value_ST_TblWidth_dxa;
            OUString sHeight = rVal.getString();
            m_nMeasureValue = sHeight.toInt32();
        }
        break;
        default:
            OSL_FAIL( "unknown attribute");
    }
}


void MeasureHandler::lcl_sprm(Sprm & rSprm)
{
    (void)rSprm;
}


sal_Int32 MeasureHandler::getMeasureValue() const
{
    sal_Int32 nRet = 0;
    if( m_nMeasureValue != 0 && m_nUnit >= 0 )
    {
        // TODO m_nUnit 3 - twip, other values unknown :-(
        if( m_nUnit == 3 || sal::static_int_cast<Id>(m_nUnit) == NS_ooxml::LN_Value_ST_TblWidth_dxa)
        {
            nRet = ConversionHelper::convertTwipToMM100( m_nMeasureValue );
        }
        //todo: handle additional width types:
        //NS_ooxml::LN_Value_ST_TblWidth_nil, NS_ooxml::LN_Value_ST_TblWidth_pct,
        //NS_ooxml::LN_Value_ST_TblWidth_dxa, NS_ooxml::LN_Value_ST_TblWidth_auto;
    }
    return nRet;
}

} //namespace dmapper
} //namespace writerfilter

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

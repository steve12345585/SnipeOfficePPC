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
#ifndef INCLUDED_DMAPPER_CONVERSIONHELPER_HXX
#define INCLUDED_DMAPPER_CONVERSIONHELPER_HXX

#include <sal/types.h>
#include <rtl/ustring.hxx>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/table/BorderLine2.hpp>
namespace com{ namespace sun{ namespace star{
    namespace lang{
        struct Locale;
    }
    namespace table{
        struct BorderLine2;
}}}}

namespace writerfilter {
namespace dmapper{
namespace ConversionHelper{

    // create a border line and return the distance value
    sal_Int32 MakeBorderLine( sal_Int32 nSprmValue, ::com::sun::star::table::BorderLine2& rToFill );
    void MakeBorderLine( sal_Int32 nLineThickness,   sal_Int32 nLineType,
                              sal_Int32 nLineColor,
                              ::com::sun::star::table::BorderLine2& rToFill, bool bIsOOXML );
    //convert the number format string form MS format to SO format
    OUString ConvertMSFormatStringToSO(
            const OUString& rFormat, ::com::sun::star::lang::Locale& rLocale, bool bHijri);
    sal_Int32 convertTwipToMM100(sal_Int32 _t);
    // probably the most useless unit in the world - English Metric Units (EMU) 360 000 EMU == 1cm
    sal_Int32 convertEMUToMM100(sal_Int32 _t);
    sal_Int32 ConvertColor(sal_Int32 nWordColor );
    sal_Int16 convertTableJustification( sal_Int32 nIntValue );
    sal_Int16 ConvertNumberingType(sal_Int32 nNFC);

} // namespace ConversionHelper
} //namespace dmapper
} // namespace writerfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

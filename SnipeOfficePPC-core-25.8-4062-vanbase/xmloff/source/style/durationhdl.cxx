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

#include "durationhdl.hxx"
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/util/Duration.hpp>
#include <rtl/ustrbuf.hxx>
#include <sax/tools/converter.hxx>

using ::rtl::OUString;
using ::rtl::OUStringBuffer;

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;

// ---------------------------------------------------------------------------


sal_Bool XMLDurationMS16PropHdl_Impl::importXML(
        const OUString& rStrImpValue,
           Any& rValue,
        const SvXMLUnitConverter& ) const
{
    Duration aDuration;
    if (!::sax::Converter::convertDuration( aDuration,  rStrImpValue ))
        return false;

    const sal_Int16 nMS = ((aDuration.Hours * 60 + aDuration.Minutes) * 60
            + aDuration.Seconds) * 100 + (aDuration.MilliSeconds / 10);
    rValue <<= nMS;

    return sal_True;
}

sal_Bool XMLDurationMS16PropHdl_Impl::exportXML(
        OUString& rStrExpValue,
        const Any& rValue,
        const SvXMLUnitConverter& ) const
{
    sal_Int16 nMS = sal_Int16();

    if(rValue >>= nMS)
    {
        OUStringBuffer aOut;
        Duration aDuration(false, 0, 0, 0, 0, 0, 0, nMS * 10);
        ::sax::Converter::convertDuration(aOut, aDuration);
        rStrExpValue = aOut.makeStringAndClear();
        return sal_True;
    }

    return sal_False;
}

XMLDurationMS16PropHdl_Impl::~XMLDurationMS16PropHdl_Impl()
{
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

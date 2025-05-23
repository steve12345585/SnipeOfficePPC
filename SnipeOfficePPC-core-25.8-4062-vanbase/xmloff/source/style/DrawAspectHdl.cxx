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


#include "DrawAspectHdl.hxx"

#include <com/sun/star/uno/Any.hxx>

#include <rtl/ustrbuf.hxx>

#include <tools/solar.h>

#include <sax/tools/converter.hxx>

#include <xmloff/xmlement.hxx>
#include <xmloff/xmltoken.hxx>


using ::rtl::OUString;
using ::rtl::OUStringBuffer;

using namespace ::com::sun::star;
using namespace ::xmloff::token;

///////////////////////////////////////////////////////////////////////////////
//
// class XMLMeasurePropHdl
//

SvXMLEnumMapEntry const pXML_DrawAspect_Enum[] =
{
    { XML_CONTENT,          1   },
    { XML_THUMBNAIL,        2   },
    { XML_ICON,             4   },
    { XML_PRINT,            8   },
    { XML_TOKEN_INVALID, 0 }
};

DrawAspectHdl::~DrawAspectHdl()
{
    // nothing to do
}

sal_Bool DrawAspectHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& ) const
{
    sal_Int64 nAspect = 0;

    ::sax::Converter::convertNumber64( nAspect, rStrImpValue );
    rValue <<= nAspect;

    return nAspect > 0;
}

sal_Bool DrawAspectHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& ) const
{
    sal_Bool bRet = sal_False;
      OUStringBuffer aOut;

    sal_Int64 nAspect = 0;
    if( ( rValue >>= nAspect ) && nAspect > 0 )
    {
        // store the aspect as an integer value
        aOut.append( nAspect );

        rStrExpValue = aOut.makeStringAndClear();

        bRet = sal_True;
    }

    return bRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

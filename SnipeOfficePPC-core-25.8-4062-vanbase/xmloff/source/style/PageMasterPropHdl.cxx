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


#include "PageMasterPropHdl.hxx"

#include <sax/tools/converter.hxx>

#include <xmloff/xmltoken.hxx>
#include <xmloff/xmluconv.hxx>
#include <xmloff/xmlnumi.hxx>
#include <xmloff/xmlnume.hxx>
#include <rtl/ustrbuf.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/style/PageStyleLayout.hpp>
#include <comphelper/types.hxx>
#include <comphelper/extract.hxx>

using ::rtl::OUString;
using ::rtl::OUStringBuffer;

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::style;
using namespace ::comphelper;
using namespace ::xmloff::token;


//______________________________________________________________________________

#define DEFAULT_PAPERTRAY   (sal_Int32(-1))


//______________________________________________________________________________
// property handler for style:page-usage (style::PageStyleLayout)

XMLPMPropHdl_PageStyleLayout::~XMLPMPropHdl_PageStyleLayout()
{
}

bool XMLPMPropHdl_PageStyleLayout::equals( const Any& rAny1, const Any& rAny2 ) const
{
    style::PageStyleLayout eLayout1, eLayout2;
    return ((rAny1 >>= eLayout1) && (rAny2 >>= eLayout2)) ? (eLayout1 == eLayout2) : sal_False;
}

sal_Bool XMLPMPropHdl_PageStyleLayout::importXML(
        const OUString& rStrImpValue,
        Any& rValue,
        const SvXMLUnitConverter& ) const
{
    sal_Bool bRet = sal_True;

    if( IsXMLToken( rStrImpValue, XML_ALL ) )
        rValue <<= PageStyleLayout_ALL;
    else if( IsXMLToken( rStrImpValue, XML_LEFT ) )
        rValue <<= PageStyleLayout_LEFT;
    else if( IsXMLToken( rStrImpValue, XML_RIGHT ) )
        rValue <<= PageStyleLayout_RIGHT;
    else if( IsXMLToken( rStrImpValue, XML_MIRRORED ) )
        rValue <<= PageStyleLayout_MIRRORED;
    else
        bRet = sal_False;

    return bRet;
}

sal_Bool XMLPMPropHdl_PageStyleLayout::exportXML(
        OUString& rStrExpValue,
        const Any& rValue,
        const SvXMLUnitConverter& ) const
{
    sal_Bool        bRet = sal_False;
    PageStyleLayout eLayout;

    if( rValue >>= eLayout )
    {
        bRet = sal_True;
        switch( eLayout )
        {
            case PageStyleLayout_ALL:
                rStrExpValue = GetXMLToken( XML_ALL );
            break;
            case PageStyleLayout_LEFT:
                rStrExpValue = GetXMLToken( XML_LEFT );
            break;
            case PageStyleLayout_RIGHT:
                rStrExpValue = GetXMLToken( XML_RIGHT );
            break;
            case PageStyleLayout_MIRRORED:
                rStrExpValue = GetXMLToken( XML_MIRRORED );
            break;
            default:
                bRet = sal_False;
        }
    }

    return bRet;
}


//______________________________________________________________________________
// property handler for style:num-format (style::NumberingType)

XMLPMPropHdl_NumFormat::~XMLPMPropHdl_NumFormat()
{
}

sal_Bool XMLPMPropHdl_NumFormat::importXML(
        const OUString& rStrImpValue,
        Any& rValue,
        const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Int16 nSync = sal_Int16();
    sal_Int16 nNumType = NumberingType::NUMBER_NONE;
    rUnitConverter.convertNumFormat( nNumType, rStrImpValue, OUString(),
                                     sal_True );

    if( !(rValue >>= nSync) )
        nSync = NumberingType::NUMBER_NONE;

    // if num-letter-sync appears before num-format, the function
    // XMLPMPropHdl_NumLetterSync::importXML() sets the value
    // NumberingType::CHARS_LOWER_LETTER_N
    if( nSync == NumberingType::CHARS_LOWER_LETTER_N )
    {
        switch( nNumType )
        {
            case NumberingType::CHARS_LOWER_LETTER:
                nNumType = NumberingType::CHARS_LOWER_LETTER_N;
            break;
            case NumberingType::CHARS_UPPER_LETTER:
                nNumType = NumberingType::CHARS_UPPER_LETTER_N;
            break;
        }
    }
    rValue <<= nNumType;

    return sal_True;
}

sal_Bool XMLPMPropHdl_NumFormat::exportXML(
        OUString& rStrExpValue,
        const Any& rValue,
        const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool    bRet = sal_False;
    sal_Int16   nNumType = sal_Int16();

    if( rValue >>= nNumType )
    {
        OUStringBuffer aBuffer( 10 );
        rUnitConverter.convertNumFormat( aBuffer, nNumType );
        rStrExpValue = aBuffer.makeStringAndClear();
        bRet = sal_True;
    }
    return bRet;
}


//______________________________________________________________________________
// property handler for style:num-letter-sync (style::NumberingType)

XMLPMPropHdl_NumLetterSync::~XMLPMPropHdl_NumLetterSync()
{
}

sal_Bool XMLPMPropHdl_NumLetterSync::importXML(
        const OUString& rStrImpValue,
        Any& rValue,
        const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Int16 nNumType;
    sal_Int16 nSync = NumberingType::NUMBER_NONE;
    rUnitConverter.convertNumFormat( nSync, rStrImpValue,
                                     GetXMLToken( XML_A ), sal_True );

    if( !(rValue >>= nNumType) )
        nNumType = NumberingType::NUMBER_NONE;

    if( nSync == NumberingType::CHARS_LOWER_LETTER_N )
    {
        switch( nNumType )
        {
            case NumberingType::CHARS_LOWER_LETTER:
                nNumType = NumberingType::CHARS_LOWER_LETTER_N;
            break;
            case NumberingType::CHARS_UPPER_LETTER:
                nNumType = NumberingType::CHARS_UPPER_LETTER_N;
            break;
        }
    }
    rValue <<= nNumType;

    return sal_True;
}

sal_Bool XMLPMPropHdl_NumLetterSync::exportXML(
        OUString& rStrExpValue,
        const Any& rValue,
        const SvXMLUnitConverter& rUnitConverter ) const
{
    sal_Bool    bRet = sal_False;
    sal_Int16   nNumType = sal_Int16();

    if( rValue >>= nNumType )
    {
        OUStringBuffer aBuffer( 5 );
        rUnitConverter.convertNumLetterSync( aBuffer, nNumType );
        rStrExpValue = aBuffer.makeStringAndClear();
        bRet = !rStrExpValue.isEmpty();
    }
    return bRet;
}


//______________________________________________________________________________
// property handler for style:paper-tray-number

XMLPMPropHdl_PaperTrayNumber::~XMLPMPropHdl_PaperTrayNumber()
{
}

sal_Bool XMLPMPropHdl_PaperTrayNumber::importXML(
        const OUString& rStrImpValue,
        Any& rValue,
        const SvXMLUnitConverter& ) const
{
    sal_Bool bRet = sal_False;

    if( IsXMLToken( rStrImpValue, XML_DEFAULT ) )
    {
        rValue <<= DEFAULT_PAPERTRAY;
        bRet = sal_True;
    }
    else
    {
        sal_Int32 nPaperTray;
        if (::sax::Converter::convertNumber( nPaperTray, rStrImpValue, 0 ))
        {
            rValue <<= nPaperTray;
            bRet = sal_True;
        }
    }

    return bRet;
}

sal_Bool XMLPMPropHdl_PaperTrayNumber::exportXML(
        OUString& rStrExpValue,
        const Any& rValue,
        const SvXMLUnitConverter& ) const
{
    sal_Bool    bRet = sal_False;
    sal_Int32   nPaperTray = 0;

    if( rValue >>= nPaperTray )
    {
        if( nPaperTray == DEFAULT_PAPERTRAY )
            rStrExpValue = GetXMLToken( XML_DEFAULT );
        else
        {
            OUStringBuffer aBuffer;
            ::sax::Converter::convertNumber( aBuffer, nPaperTray );
            rStrExpValue = aBuffer.makeStringAndClear();
        }
        bRet = sal_True;
    }
    return bRet;
}


//______________________________________________________________________________
// property handler for style:print

XMLPMPropHdl_Print::XMLPMPropHdl_Print( enum XMLTokenEnum eValue ) :
    sAttrValue( GetXMLToken( eValue ) )
{
}

XMLPMPropHdl_Print::~XMLPMPropHdl_Print()
{
}

sal_Bool XMLPMPropHdl_Print::importXML(
        const OUString& rStrImpValue,
        Any& rValue,
        const SvXMLUnitConverter& ) const
{
    sal_Unicode cToken  = ' ';
    sal_Int32   nTokenIndex = 0;
    sal_Bool    bFound  = sal_False;

    do
    {
        bFound = (sAttrValue == rStrImpValue.getToken( 0, cToken, nTokenIndex ));
    }
    while ( (nTokenIndex >= 0) && !bFound );

    setBOOL( rValue, bFound );
    return sal_True;
}

sal_Bool XMLPMPropHdl_Print::exportXML(
        OUString& rStrExpValue,
        const Any& rValue,
        const SvXMLUnitConverter& ) const
{
    if( getBOOL( rValue ) )
    {
        if( !rStrExpValue.isEmpty() )
            rStrExpValue += OUString( RTL_CONSTASCII_USTRINGPARAM( " " ) );
        rStrExpValue += sAttrValue;
    }

    return sal_True;
}

//______________________________________________________________________________
// property handler for style:table-centering

XMLPMPropHdl_CenterHorizontal::~XMLPMPropHdl_CenterHorizontal()
{
}

sal_Bool XMLPMPropHdl_CenterHorizontal::importXML(
        const OUString& rStrImpValue,
        Any& rValue,
        const SvXMLUnitConverter& ) const
{
    sal_Bool bRet = sal_False;

    if (!rStrImpValue.isEmpty())
        if (IsXMLToken( rStrImpValue, XML_BOTH) ||
            IsXMLToken( rStrImpValue, XML_HORIZONTAL))
        {
            rValue = ::cppu::bool2any(sal_True);
            bRet = sal_True;
        }


    return bRet;
}

sal_Bool XMLPMPropHdl_CenterHorizontal::exportXML(
        OUString& rStrExpValue,
        const Any& rValue,
        const SvXMLUnitConverter& ) const
{
    sal_Bool    bRet = sal_False;

    if ( ::cppu::any2bool( rValue ) )
    {
        bRet = sal_True;
        if (!rStrExpValue.isEmpty())
            rStrExpValue = GetXMLToken(XML_BOTH);
        else
            rStrExpValue = GetXMLToken(XML_HORIZONTAL);
    }

    return bRet;
}

XMLPMPropHdl_CenterVertical::~XMLPMPropHdl_CenterVertical()
{
}

sal_Bool XMLPMPropHdl_CenterVertical::importXML(
        const OUString& rStrImpValue,
        Any& rValue,
        const SvXMLUnitConverter& ) const
{
    sal_Bool bRet = sal_False;

    if (!rStrImpValue.isEmpty())
        if (IsXMLToken(rStrImpValue, XML_BOTH) ||
            IsXMLToken(rStrImpValue, XML_VERTICAL) )
        {
            rValue = ::cppu::bool2any(sal_True);
            bRet = sal_True;
        }

    return bRet;
}

sal_Bool XMLPMPropHdl_CenterVertical::exportXML(
        OUString& rStrExpValue,
        const Any& rValue,
        const SvXMLUnitConverter& ) const
{
    sal_Bool    bRet = sal_False;

    if ( ::cppu::any2bool( rValue ) )
    {
        bRet = sal_True;
        if (!rStrExpValue.isEmpty())
            rStrExpValue = GetXMLToken(XML_BOTH);
        else
            rStrExpValue = GetXMLToken(XML_VERTICAL);
    }

    return bRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#include "oox/helper/attributelist.hxx"

#include <osl/diagnose.h>
#include <rtl/ustrbuf.hxx>
#include "oox/token/tokenmap.hxx"

namespace oox {

// ============================================================================

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::xml::sax;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;

// ============================================================================

namespace {

const sal_Int32 XSTRING_ENCCHAR_LEN     = 7;

bool lclAddHexDigit( sal_Unicode& orcChar, sal_Unicode cDigit, int nBitShift )
{
    if( ('0' <= cDigit) && (cDigit <= '9') ) { orcChar |= ((cDigit - '0') << nBitShift); return true; }
    if( ('a' <= cDigit) && (cDigit <= 'f') ) { orcChar |= ((cDigit - 'a' + 10) << nBitShift); return true; }
    if( ('A' <= cDigit) && (cDigit <= 'F') ) { orcChar |= ((cDigit - 'A' + 10) << nBitShift); return true; }
    return false;
}

sal_Unicode lclGetXChar( const sal_Unicode*& rpcStr, const sal_Unicode* pcEnd )
{
    sal_Unicode cChar = 0;
    if( (pcEnd - rpcStr >= XSTRING_ENCCHAR_LEN) &&
        (rpcStr[ 0 ] == '_') &&
        (rpcStr[ 1 ] == 'x') &&
        (rpcStr[ 6 ] == '_') &&
        lclAddHexDigit( cChar, rpcStr[ 2 ], 12 ) &&
        lclAddHexDigit( cChar, rpcStr[ 3 ], 8 ) &&
        lclAddHexDigit( cChar, rpcStr[ 4 ], 4 ) &&
        lclAddHexDigit( cChar, rpcStr[ 5 ], 0 ) )
    {
        rpcStr += XSTRING_ENCCHAR_LEN;
        return cChar;
    }
    return *rpcStr++;
}

} // namespace

// ----------------------------------------------------------------------------

sal_Int32 AttributeConversion::decodeToken( const OUString& rValue )
{
    return StaticTokenMap::get().getTokenFromUnicode( rValue );
}

OUString AttributeConversion::decodeXString( const OUString& rValue )
{
    // string shorter than one encoded character - no need to decode
    if( rValue.getLength() < XSTRING_ENCCHAR_LEN )
        return rValue;
    OUStringBuffer aBuffer;
    const sal_Unicode* pcStr = rValue.getStr();
    const sal_Unicode* pcEnd = pcStr + rValue.getLength();
    while( pcStr < pcEnd )
        aBuffer.append( lclGetXChar( pcStr, pcEnd ) );
    return aBuffer.makeStringAndClear();
}

double AttributeConversion::decodeDouble( const OUString& rValue )
{
    return rValue.toDouble();
}

sal_Int32 AttributeConversion::decodeInteger( const OUString& rValue )
{
    return rValue.toInt32();
}

sal_uInt32 AttributeConversion::decodeUnsigned( const OUString& rValue )
{
    return getLimitedValue< sal_uInt32, sal_Int64 >( rValue.toInt64(), 0, SAL_MAX_UINT32 );
}

sal_Int64 AttributeConversion::decodeHyper( const OUString& rValue )
{
    return rValue.toInt64();
}

sal_Int32 AttributeConversion::decodeIntegerHex( const OUString& rValue )
{
    return rValue.toInt32( 16 );
}

// ============================================================================

AttributeList::AttributeList( const Reference< XFastAttributeList >& rxAttribs ) :
    mxAttribs( rxAttribs )
{
    OSL_ENSURE( mxAttribs.is(), "AttributeList::AttributeList - missing attribute list interface" );
}

bool AttributeList::hasAttribute( sal_Int32 nAttrToken ) const
{
    return mxAttribs->hasAttribute( nAttrToken );
}

// optional return values -----------------------------------------------------

OptValue< sal_Int32 > AttributeList::getToken( sal_Int32 nAttrToken ) const
{
    sal_Int32 nToken = mxAttribs->getOptionalValueToken( nAttrToken, XML_TOKEN_INVALID );
    return OptValue< sal_Int32 >( nToken != XML_TOKEN_INVALID, nToken );
}

OptValue< OUString > AttributeList::getString( sal_Int32 nAttrToken ) const
{
    // check if the attribute exists (empty string may be different to missing attribute)
    if( mxAttribs->hasAttribute( nAttrToken ) )
        return OptValue< OUString >( mxAttribs->getOptionalValue( nAttrToken ) );
    return OptValue< OUString >();
}

OptValue< OUString > AttributeList::getXString( sal_Int32 nAttrToken ) const
{
    // check if the attribute exists (empty string may be different to missing attribute)
    if( mxAttribs->hasAttribute( nAttrToken ) )
        return OptValue< OUString >( AttributeConversion::decodeXString( mxAttribs->getOptionalValue( nAttrToken ) ) );
    return OptValue< OUString >();
}

OptValue< double > AttributeList::getDouble( sal_Int32 nAttrToken ) const
{
    OUString aValue = mxAttribs->getOptionalValue( nAttrToken );
    bool bValid = !aValue.isEmpty();
    return OptValue< double >( bValid, bValid ? AttributeConversion::decodeDouble( aValue ) : 0.0 );
}

OptValue< sal_Int32 > AttributeList::getInteger( sal_Int32 nAttrToken ) const
{
    OUString aValue = mxAttribs->getOptionalValue( nAttrToken );
    bool bValid = !aValue.isEmpty();
    return OptValue< sal_Int32 >( bValid, bValid ? AttributeConversion::decodeInteger( aValue ) : 0 );
}

OptValue< sal_uInt32 > AttributeList::getUnsigned( sal_Int32 nAttrToken ) const
{
    OUString aValue = mxAttribs->getOptionalValue( nAttrToken );
    bool bValid = !aValue.isEmpty();
    return OptValue< sal_uInt32 >( bValid, AttributeConversion::decodeUnsigned( aValue ) );
}

OptValue< sal_Int64 > AttributeList::getHyper( sal_Int32 nAttrToken ) const
{
    OUString aValue = mxAttribs->getOptionalValue( nAttrToken );
    bool bValid = !aValue.isEmpty();
    return OptValue< sal_Int64 >( bValid, bValid ? AttributeConversion::decodeHyper( aValue ) : 0 );
}

OptValue< sal_Int32 > AttributeList::getIntegerHex( sal_Int32 nAttrToken ) const
{
    OUString aValue = mxAttribs->getOptionalValue( nAttrToken );
    bool bValid = !aValue.isEmpty();
    return OptValue< sal_Int32 >( bValid, bValid ? AttributeConversion::decodeIntegerHex( aValue ) : 0 );
}

OptValue< bool > AttributeList::getBool( sal_Int32 nAttrToken ) const
{
    // boolean attributes may be "t", "f", "true", "false", "on", "off", "1", or "0"
    switch( getToken( nAttrToken, XML_TOKEN_INVALID ) )
    {
        case XML_t:     return OptValue< bool >( true );  // used in VML
        case XML_true:  return OptValue< bool >( true );
        case XML_on:    return OptValue< bool >( true );
        case XML_f:     return OptValue< bool >( false ); // used in VML
        case XML_false: return OptValue< bool >( false );
        case XML_off:   return OptValue< bool >( false );
    }
    OptValue< sal_Int32 > onValue = getInteger( nAttrToken );
    return OptValue< bool >( onValue.has(), onValue.get() != 0 );
}

OptValue< DateTime > AttributeList::getDateTime( sal_Int32 nAttrToken ) const
{
    OUString aValue = mxAttribs->getOptionalValue( nAttrToken );
    DateTime aDateTime;
    bool bValid = (aValue.getLength() == 19) && (aValue[ 4 ] == '-') && (aValue[ 7 ] == '-') &&
        (aValue[ 10 ] == 'T') && (aValue[ 13 ] == ':') && (aValue[ 16 ] == ':');
    if( bValid )
    {
        aDateTime.Year    = static_cast< sal_uInt16 >( aValue.copy( 0, 4 ).toInt32() );
        aDateTime.Month   = static_cast< sal_uInt16 >( aValue.copy( 5, 2 ).toInt32() );
        aDateTime.Day     = static_cast< sal_uInt16 >( aValue.copy( 8, 2 ).toInt32() );
        aDateTime.Hours   = static_cast< sal_uInt16 >( aValue.copy( 11, 2 ).toInt32() );
        aDateTime.Minutes = static_cast< sal_uInt16 >( aValue.copy( 14, 2 ).toInt32() );
        aDateTime.Seconds = static_cast< sal_uInt16 >( aValue.copy( 17, 2 ).toInt32() );
    }
    return OptValue< DateTime >( bValid, aDateTime );
}

// defaulted return values ----------------------------------------------------

sal_Int32 AttributeList::getToken( sal_Int32 nAttrToken, sal_Int32 nDefault ) const
{
    return mxAttribs->getOptionalValueToken( nAttrToken, nDefault );
}

OUString AttributeList::getString( sal_Int32 nAttrToken, const OUString& rDefault ) const
{
    // try to avoid slow exception throw/catch if we can
    if (rDefault.isEmpty())
        return mxAttribs->getOptionalValue( nAttrToken );

    try
    {
        return mxAttribs->getValue( nAttrToken );
    }
    catch( Exception& )
    {
    }
    return rDefault;
}

OUString AttributeList::getXString( sal_Int32 nAttrToken, const OUString& rDefault ) const
{
    return getXString( nAttrToken ).get( rDefault );
}

double AttributeList::getDouble( sal_Int32 nAttrToken, double fDefault ) const
{
    return getDouble( nAttrToken ).get( fDefault );
}

sal_Int32 AttributeList::getInteger( sal_Int32 nAttrToken, sal_Int32 nDefault ) const
{
    return getInteger( nAttrToken ).get( nDefault );
}

sal_uInt32 AttributeList::getUnsigned( sal_Int32 nAttrToken, sal_uInt32 nDefault ) const
{
    return getUnsigned( nAttrToken ).get( nDefault );
}

sal_Int64 AttributeList::getHyper( sal_Int32 nAttrToken, sal_Int64 nDefault ) const
{
    return getHyper( nAttrToken ).get( nDefault );
}

sal_Int32 AttributeList::getIntegerHex( sal_Int32 nAttrToken, sal_Int32 nDefault ) const
{
    return getIntegerHex( nAttrToken ).get( nDefault );
}

bool AttributeList::getBool( sal_Int32 nAttrToken, bool bDefault ) const
{
    return getBool( nAttrToken ).get( bDefault );
}

DateTime AttributeList::getDateTime( sal_Int32 nAttrToken, const DateTime& rDefault ) const
{
    return getDateTime( nAttrToken ).get( rDefault );
}

// ============================================================================

} // namespace oox

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

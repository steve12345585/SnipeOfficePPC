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

#ifndef OOX_HELPER_HELPER_HXX
#define OOX_HELPER_HELPER_HXX

#include <algorithm>
#include <limits>
#include <boost/static_assert.hpp>
#include <osl/endian.h>
#include <rtl/math.hxx>
#include <rtl/string.hxx>
#include <rtl/ustring.hxx>
#include <string.h>

namespace oox {

// Helper macros ==============================================================

/** Expands to the number of elements in a STATIC data array. */
#define STATIC_ARRAY_SIZE( array ) \
    (sizeof(array)/sizeof(*(array)))

/** Expands to a pointer behind the last element of a STATIC data array (like
    STL end()). */
#define STATIC_ARRAY_END( array ) \
    ((array)+STATIC_ARRAY_SIZE(array))

/** Expands to the 'index'-th element of a STATIC data array, or to 'def', if
    'index' is out of the array limits. */
#define STATIC_ARRAY_SELECT( array, index, def ) \
    ((static_cast<size_t>(index) < STATIC_ARRAY_SIZE(array)) ? ((array)[static_cast<size_t>(index)]) : (def))

/** Expands to a temporary ::rtl::OString, created from a literal(!) character
    array. */
#define CREATE_OSTRING( ascii ) \
    ::rtl::OString( RTL_CONSTASCII_STRINGPARAM( ascii ) )

/** Expands to a temporary ::rtl::OUString, created from a literal(!) ASCII(!)
    character array. */
#define CREATE_OUSTRING( ascii ) \
    ::rtl::OUString::intern( RTL_CONSTASCII_USTRINGPARAM( ascii ) )

/** Convert an OUString to an ASCII C string. Use for debug purposes only. */
#define OUSTRING_TO_CSTR( str ) \
    ::rtl::OUStringToOString( str, RTL_TEXTENCODING_ASCII_US ).getStr()

// Common constants ===========================================================

const sal_uInt8 WINDOWS_CHARSET_ANSI        = 0;
const sal_uInt8 WINDOWS_CHARSET_DEFAULT     = 1;
const sal_uInt8 WINDOWS_CHARSET_SYMBOL      = 2;
const sal_uInt8 WINDOWS_CHARSET_APPLE_ROMAN = 77;
const sal_uInt8 WINDOWS_CHARSET_SHIFTJIS    = 128;
const sal_uInt8 WINDOWS_CHARSET_HANGEUL     = 129;
const sal_uInt8 WINDOWS_CHARSET_JOHAB       = 130;
const sal_uInt8 WINDOWS_CHARSET_GB2312      = 134;
const sal_uInt8 WINDOWS_CHARSET_BIG5        = 136;
const sal_uInt8 WINDOWS_CHARSET_GREEK       = 161;
const sal_uInt8 WINDOWS_CHARSET_TURKISH     = 162;
const sal_uInt8 WINDOWS_CHARSET_VIETNAMESE  = 163;
const sal_uInt8 WINDOWS_CHARSET_HEBREW      = 177;
const sal_uInt8 WINDOWS_CHARSET_ARABIC      = 178;
const sal_uInt8 WINDOWS_CHARSET_BALTIC      = 186;
const sal_uInt8 WINDOWS_CHARSET_RUSSIAN     = 204;
const sal_uInt8 WINDOWS_CHARSET_THAI        = 222;
const sal_uInt8 WINDOWS_CHARSET_EASTERN     = 238;
const sal_uInt8 WINDOWS_CHARSET_OEM         = 255;

// ----------------------------------------------------------------------------

const sal_Int32 API_RGB_TRANSPARENT         = -1;       ///< Transparent color for API calls.
const sal_Int32 API_RGB_BLACK               = 0x000000;  ///< Black color for API calls.
const sal_Int32 API_RGB_WHITE               = 0xFFFFFF;  ///< White color for API calls.

const sal_Int16 API_LINE_SOLID              = 0;
const sal_Int16 API_LINE_DOTTED             = 1;
const sal_Int16 API_LINE_DASHED             = 2;

const sal_Int16 API_LINE_NONE               = 0;
const sal_Int16 API_LINE_HAIR               = 2;
const sal_Int16 API_LINE_THIN               = 35;
const sal_Int16 API_LINE_MEDIUM             = 88;
const sal_Int16 API_LINE_THICK              = 141;

const sal_Int16 API_ESCAPE_NONE             = 0;        ///< No escapement.
const sal_Int16 API_ESCAPE_SUPERSCRIPT      = 101;      ///< Superscript: raise characters automatically (magic value 101).
const sal_Int16 API_ESCAPE_SUBSCRIPT        = -101;     ///< Subscript: lower characters automatically (magic value -101).

const sal_Int8 API_ESCAPEHEIGHT_NONE        = 100;      ///< Relative character height if not escaped.
const sal_Int8 API_ESCAPEHEIGHT_DEFAULT     = 58;       ///< Relative character height if escaped.

// ============================================================================

// Limitate values ------------------------------------------------------------

template< typename ReturnType, typename Type >
inline ReturnType getLimitedValue( Type nValue, Type nMin, Type nMax )
{
    return static_cast< ReturnType >( ::std::min( ::std::max( nValue, nMin ), nMax ) );
}

template< typename ReturnType, typename Type >
inline ReturnType getIntervalValue( Type nValue, Type nBegin, Type nEnd )
{
//    this BOOST_STATIC_ASSERT fails with suncc
//    BOOST_STATIC_ASSERT( ::std::numeric_limits< Type >::is_integer );
    Type nInterval = nEnd - nBegin;
    Type nCount = (nValue < nBegin) ? -((nBegin - nValue - 1) / nInterval + 1) : ((nValue - nBegin) / nInterval);
    return static_cast< ReturnType >( nValue - nCount * nInterval );
}

template< typename ReturnType >
inline ReturnType getDoubleIntervalValue( double fValue, double fBegin, double fEnd )
{
    double fInterval = fEnd - fBegin;
    double fCount = (fValue < fBegin) ? -(::rtl::math::approxFloor( (fBegin - fValue - 1.0) / fInterval ) + 1.0) : ::rtl::math::approxFloor( (fValue - fBegin) / fInterval );
    return static_cast< ReturnType >( fValue - fCount * fInterval );
}

// Read from bitfields --------------------------------------------------------

/** Returns true, if at least one of the bits set in nMask is set in nBitField. */
template< typename Type >
inline bool getFlag( Type nBitField, Type nMask )
{
    return (nBitField & nMask) != 0;
}

/** Returns nSet, if at least one bit of nMask is set in nBitField, otherwise nUnset. */
template< typename ReturnType, typename Type >
inline ReturnType getFlagValue( Type nBitField, Type nMask, ReturnType nSet, ReturnType nUnset )
{
    return getFlag( nBitField, nMask ) ? nSet : nUnset;
}

/** Extracts a value from a bit field.

    Returns the data fragment from nBitField, that starts at bit nStartBit
    (0-based, bit 0 is rightmost) with the width of nBitCount. The returned
    value will be right-aligned (normalized).
    For instance: extractValue<T>(0x4321,8,4) returns 3 (value in bits 8-11).
 */
template< typename ReturnType, typename Type >
inline ReturnType extractValue( Type nBitField, sal_uInt8 nStartBit, sal_uInt8 nBitCount )
{
    sal_uInt64 nMask = 1; nMask <<= nBitCount; --nMask;
    return static_cast< ReturnType >( nMask & (nBitField >> nStartBit) );
}

// Write to bitfields ---------------------------------------------------------

/** Sets or clears (according to bSet) all set bits of nMask in ornBitField. */
template< typename Type >
inline void setFlag( Type& ornBitField, Type nMask, bool bSet = true )
{
    if( bSet ) ornBitField |= nMask; else ornBitField &= ~nMask;
}

/** Inserts a value into a bitfield.

    Inserts the lower nBitCount bits of nValue into ornBitField, starting
    there at bit nStartBit. Other contents of ornBitField keep unchanged.
 */
template< typename Type, typename InsertType >
void insertValue( Type& ornBitField, InsertType nValue, sal_uInt8 nStartBit, sal_uInt8 nBitCount )
{
    sal_uInt64 nMask = 1; nMask <<= nBitCount; --nMask;
    Type nNewValue = static_cast< Type >( nValue & nMask );
    (ornBitField &= ~(nMask << nStartBit)) |= (nNewValue << nStartBit);
}

// ============================================================================

/** Optional value, similar to ::boost::optional<>, with convenience accessors.
 */
template< typename Type >
class OptValue
{
public:
    inline explicit     OptValue() : maValue(), mbHasValue( false ) {}
    inline explicit     OptValue( const Type& rValue ) : maValue( rValue ), mbHasValue( true ) {}
    inline explicit     OptValue( bool bHasValue, const Type& rValue ) : maValue( rValue ), mbHasValue( bHasValue ) {}

    inline bool         has() const { return mbHasValue; }
    inline bool         operator!() const { return !mbHasValue; }
    inline bool         differsFrom( const Type& rValue ) const { return mbHasValue && (maValue != rValue); }

    inline const Type&  get() const { return maValue; }
    inline const Type&  get( const Type& rDefValue ) const { return mbHasValue ? maValue : rDefValue; }

    inline void         reset() { mbHasValue = false; }
    inline void         set( const Type& rValue ) { maValue = rValue; mbHasValue = true; }
    inline Type&        use() { mbHasValue = true; return maValue; }

    inline OptValue&    operator=( const Type& rValue ) { set( rValue ); return *this; }
    inline bool         operator==( const OptValue& rValue ) const {
                                    return ( ( mbHasValue == false && rValue.mbHasValue == false ) ||
                                        ( mbHasValue == rValue.mbHasValue && maValue == rValue.maValue ) );
                        }
    inline void         assignIfUsed( const OptValue& rValue ) { if( rValue.mbHasValue ) set( rValue.maValue ); }

private:
    Type                maValue;
    bool                mbHasValue;
};

// ============================================================================

/** Provides platform independent functions to convert from or to little-endian
    byte order, e.g. for reading data from or writing data to memory or a
    binary stream.

    On big-endian platforms, the byte order in the passed values is swapped,
    this can be used for converting big-endian to and from little-endian data.

    On little-endian platforms, the conversion functions are implemented empty,
    thus compilers should completely optimize away the function call.
 */
class ByteOrderConverter
{
public:
#ifdef OSL_BIGENDIAN
    inline static void  convertLittleEndian( sal_Int8& ) {}     // present for usage in templates
    inline static void  convertLittleEndian( sal_uInt8& ) {}    // present for usage in templates
    inline static void  convertLittleEndian( sal_Int16& rnValue )  { swap2( reinterpret_cast< sal_uInt8* >( &rnValue ) ); }
    inline static void  convertLittleEndian( sal_uInt16& rnValue ) { swap2( reinterpret_cast< sal_uInt8* >( &rnValue ) ); }
    inline static void  convertLittleEndian( sal_Int32& rnValue )  { swap4( reinterpret_cast< sal_uInt8* >( &rnValue ) ); }
    inline static void  convertLittleEndian( sal_uInt32& rnValue ) { swap4( reinterpret_cast< sal_uInt8* >( &rnValue ) ); }
    inline static void  convertLittleEndian( sal_Int64& rnValue )  { swap8( reinterpret_cast< sal_uInt8* >( &rnValue ) ); }
    inline static void  convertLittleEndian( sal_uInt64& rnValue ) { swap8( reinterpret_cast< sal_uInt8* >( &rnValue ) ); }
    inline static void  convertLittleEndian( float& rfValue )      { swap4( reinterpret_cast< sal_uInt8* >( &rfValue ) ); }
    inline static void  convertLittleEndian( double& rfValue )     { swap8( reinterpret_cast< sal_uInt8* >( &rfValue ) ); }

    template< typename Type >
    inline static void  convertLittleEndianArray( Type* pnArray, size_t nElemCount );

    inline static void  convertLittleEndianArray( sal_Int8*, size_t ) {}
    inline static void  convertLittleEndianArray( sal_uInt8*, size_t ) {}

#else
    template< typename Type >
    inline static void  convertLittleEndian( Type& ) {}

    template< typename Type >
    inline static void  convertLittleEndianArray( Type*, size_t ) {}

#endif

    /** Reads a value from memory, assuming memory buffer in little-endian.
        @param ornValue  (out-parameter) Contains the value read from memory.
        @param pSrcBuffer  The memory buffer to read the value from.
     */
    template< typename Type >
    inline static void  readLittleEndian( Type& ornValue, const void* pSrcBuffer );

    /** Writes a value to memory, while converting it to little-endian.
        @param pDstBuffer  The memory buffer to write the value to.
        @param nValue  The value to be written to memory in little-endian.
     */
    template< typename Type >
    inline static void  writeLittleEndian( void* pDstBuffer, Type nValue );

#ifdef OSL_BIGENDIAN
private:
    inline static void  swap2( sal_uInt8* pnData );
    inline static void  swap4( sal_uInt8* pnData );
    inline static void  swap8( sal_uInt8* pnData );
#endif
};

// ----------------------------------------------------------------------------

template< typename Type >
inline void ByteOrderConverter::readLittleEndian( Type& ornValue, const void* pSrcBuffer )
{
    memcpy( &ornValue, pSrcBuffer, sizeof( Type ) );
    convertLittleEndian( ornValue );
}

template< typename Type >
inline void ByteOrderConverter::writeLittleEndian( void* pDstBuffer, Type nValue )
{
    convertLittleEndian( nValue );
    memcpy( pDstBuffer, &nValue, sizeof( Type ) );
}

#ifdef OSL_BIGENDIAN
template< typename Type >
inline void ByteOrderConverter::convertLittleEndianArray( Type* pnArray, size_t nElemCount )
{
    for( Type* pnArrayEnd = pnArray + nElemCount; pnArray != pnArrayEnd; ++pnArray )
        convertLittleEndian( *pnArray );
}

inline void ByteOrderConverter::swap2( sal_uInt8* pnData )
{
    ::std::swap( pnData[ 0 ], pnData[ 1 ] );
}

inline void ByteOrderConverter::swap4( sal_uInt8* pnData )
{
    ::std::swap( pnData[ 0 ], pnData[ 3 ] );
    ::std::swap( pnData[ 1 ], pnData[ 2 ] );
}

inline void ByteOrderConverter::swap8( sal_uInt8* pnData )
{
    ::std::swap( pnData[ 0 ], pnData[ 7 ] );
    ::std::swap( pnData[ 1 ], pnData[ 6 ] );
    ::std::swap( pnData[ 2 ], pnData[ 5 ] );
    ::std::swap( pnData[ 3 ], pnData[ 4 ] );
}
#endif

// ============================================================================

} // namespace oox

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

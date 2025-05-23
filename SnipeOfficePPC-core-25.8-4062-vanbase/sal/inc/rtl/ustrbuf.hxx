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

#ifndef _RTL_USTRBUF_HXX_
#define _RTL_USTRBUF_HXX_

#include "sal/config.h"

#include <cassert>
#include <string.h>

#include <osl/diagnose.h>
#include <rtl/ustrbuf.h>
#include <rtl/ustring.hxx>
#include <rtl/stringutils.hxx>

#ifdef RTL_FAST_STRING
#include <rtl/stringconcat.hxx>
#endif

// The unittest uses slightly different code to help check that the proper
// calls are made. The class is put into a different namespace to make
// sure the compiler generates a different (if generating also non-inline)
// copy of the function and does not merge them together. The class
// is "brought" into the proper rtl namespace by a typedef below.
#ifdef RTL_STRING_UNITTEST
#define rtl rtlunittest
#endif

namespace rtl
{

#ifdef RTL_STRING_UNITTEST
#undef rtl
#endif

/** A string buffer implements a mutable sequence of characters.
    <p>
    String buffers are safe for use by multiple threads. The methods
    are synchronized where necessary so that all the operations on any
    particular instance behave as if they occur in some serial order.
    <p>
    String buffers are used by the compiler to implement the binary
    string concatenation operator <code>+</code>. For example, the code:
    <p><blockquote><pre>
        x = "a" + 4 + "c"
    </pre></blockquote><p>
    is compiled to the equivalent of:
    <p><blockquote><pre>
        x = new OUStringBuffer().append("a").append(4).append("c")
                              .makeStringAndClear()
    </pre></blockquote><p>
    The principal operations on a <code>OUStringBuffer</code> are the
    <code>append</code> and <code>insert</code> methods, which are
    overloaded so as to accept data of any type. Each effectively
    converts a given datum to a string and then appends or inserts the
    characters of that string to the string buffer. The
    <code>append</code> method always adds these characters at the end
    of the buffer; the <code>insert</code> method adds the characters at
    a specified point.
    <p>
    For example, if <code>z</code> refers to a string buffer object
    whose current contents are "<code>start</code>", then
    the method call <code>z.append("le")</code> would cause the string
    buffer to contain "<code>startle</code>", whereas
    <code>z.insert(4, "le")</code> would alter the string buffer to
    contain "<code>starlet</code>".
    <p>
    Every string buffer has a capacity. As long as the length of the
    character sequence contained in the string buffer does not exceed
    the capacity, it is not necessary to allocate a new internal
    buffer array. If the internal buffer overflows, it is
    automatically made larger.
 */
class SAL_WARN_UNUSED OUStringBuffer
{
public:
    /**
        Constructs a string buffer with no characters in it and an
        initial capacity of 16 characters.
     */
    OUStringBuffer()
        : pData(NULL)
        , nCapacity( 16 )
    {
        rtl_uString_new_WithLength( &pData, nCapacity );
    }

    /**
        Allocates a new string buffer that contains the same sequence of
        characters as the string buffer argument.

        @param   value   a <code>OUStringBuffer</code>.
     */
    OUStringBuffer( const OUStringBuffer & value )
        : pData(NULL)
        , nCapacity( value.nCapacity )
    {
        rtl_uStringbuffer_newFromStringBuffer( &pData, value.nCapacity, value.pData );
    }

    /**
        Constructs a string buffer with no characters in it and an
        initial capacity specified by the <code>length</code> argument.

        @param      length   the initial capacity.
     */
    explicit OUStringBuffer(int length)
        : pData(NULL)
        , nCapacity( length )
    {
        rtl_uString_new_WithLength( &pData, length );
    }

    /**
        Constructs a string buffer so that it represents the same
        sequence of characters as the string argument.

        The initial
        capacity of the string buffer is <code>16</code> plus the length
        of the string argument.

        @param   value   the initial contents of the buffer.
     */
    OUStringBuffer(OUString value)
        : pData(NULL)
        , nCapacity( value.getLength() + 16 )
    {
        rtl_uStringbuffer_newFromStr_WithLength( &pData, value.getStr(), value.getLength() );
    }

#ifdef HAVE_SFINAE_ANONYMOUS_BROKEN // see OUString ctors
    template< int N >
    OUStringBuffer( const char (&literal)[ N ] )
        : pData(NULL)
        , nCapacity( N - 1 + 16 )
    {
        assert( strlen( literal ) == N - 1 );
        rtl_uString_newFromLiteral( &pData, literal, N - 1, 16 );
#ifdef RTL_STRING_UNITTEST
        rtl_string_unittest_const_literal = true;
#endif
    }

    /**
     * It is an error to call this overload. Strings cannot directly use non-const char[].
     * @internal
     */
    template< int N >
    OUStringBuffer( char (&value)[ N ] )
#ifndef RTL_STRING_UNITTEST
        ; // intentionally not implemented
#else
    {
        (void) value; // unused
        pData = 0;
        nCapacity = 10;
        rtl_uString_newFromLiteral( &pData, "!!br0ken!!", 10, 0 ); // set to garbage
        rtl_string_unittest_invalid_conversion = true;
    }
#endif
#else // HAVE_SFINAE_ANONYMOUS_BROKEN
    template< typename T >
    OUStringBuffer( T& literal, typename internal::ConstCharArrayDetector< T, internal::Dummy >::Type = internal::Dummy() )
        : pData(NULL)
        , nCapacity( internal::ConstCharArrayDetector< T, void >::size - 1 + 16 )
    {
        assert( strlen( literal ) == internal::ConstCharArrayDetector< T >::size - 1 );
        rtl_uString_newFromLiteral( &pData, literal, internal::ConstCharArrayDetector< T, void >::size - 1, 16 );
#ifdef RTL_STRING_UNITTEST
        rtl_string_unittest_const_literal = true;
#endif
    }
#endif // HAVE_SFINAE_ANONYMOUS_BROKEN

#ifdef RTL_STRING_UNITTEST
    /**
     * Only used by unittests to detect incorrect conversions.
     * @internal
     */
    template< typename T >
    OUStringBuffer( T&, typename internal::ExceptConstCharArrayDetector< T >::Type = internal::Dummy() )
    {
        pData = 0;
        nCapacity = 10;
        rtl_uString_newFromLiteral( &pData, "!!br0ken!!", 10, 0 ); // set to garbage
        rtl_string_unittest_invalid_conversion = true;
    }
    /**
     * Only used by unittests to detect incorrect conversions.
     * @internal
     */
    template< typename T >
    OUStringBuffer( const T&, typename internal::ExceptCharArrayDetector< T >::Type = internal::Dummy() )
    {
        pData = 0;
        nCapacity = 10;
        rtl_uString_newFromLiteral( &pData, "!!br0ken!!", 10, 0 ); // set to garbage
        rtl_string_unittest_invalid_conversion = true;
    }
#endif

#ifdef RTL_FAST_STRING
    template< typename T1, typename T2 >
    OUStringBuffer( const OUStringConcat< T1, T2 >& c )
    {
        const sal_Int32 l = c.length();
        rtl_uString* buffer = NULL;
        nCapacity = l + 16;
        rtl_uString_new_WithLength( &buffer, nCapacity ); // TODO this clears, not necessary
        sal_Unicode* end = c.addData( buffer->buffer );
        *end = '\0';
        buffer->length = end - buffer->buffer;
        // TODO realloc in case buffer->length is noticeably smaller than l ?
        pData = buffer;
    }
#endif
    /** Assign to this a copy of value.
     */
    OUStringBuffer& operator = ( const OUStringBuffer& value )
    {
        if (this != &value)
        {
            rtl_uStringbuffer_newFromStringBuffer(&pData,
                                                  value.nCapacity,
                                                  value.pData);
            nCapacity = value.nCapacity;
        }
        return *this;
    }

    /**
        Release the string data.
     */
    ~OUStringBuffer()
    {
        rtl_uString_release( pData );
    }

    /**
        Fill the string data in the new string and clear the buffer.

        This method is more efficient than the contructor of the string. It does
        not copy the buffer.

        @return the string previously contained in the buffer.
     */
    OUString makeStringAndClear()
    {
        return OUString(
                  rtl_uStringBuffer_makeStringAndClear( &pData, &nCapacity ),
                  SAL_NO_ACQUIRE );
    }

    /**
        Returns the length (character count) of this string buffer.

        @return  the number of characters in this string buffer.
     */
    sal_Int32 getLength() const
    {
        return pData->length;
    }

    /**
        Returns the current capacity of the String buffer.

        The capacity
        is the amount of storage available for newly inserted
        characters. The real buffer size is 2 bytes longer, because
        all strings are 0 terminated.

        @return  the current capacity of this string buffer.
     */
    sal_Int32 getCapacity() const
    {
        return nCapacity;
    }

    /**
        Ensures that the capacity of the buffer is at least equal to the
        specified minimum.

        The new capacity will be at least as large as the maximum of the current
        length (so that no contents of the buffer is destroyed) and the given
        minimumCapacity.  If the given minimumCapacity is negative, nothing is
        changed.

        @param   minimumCapacity   the minimum desired capacity.
     */
    void ensureCapacity(sal_Int32 minimumCapacity)
    {
        rtl_uStringbuffer_ensureCapacity( &pData, &nCapacity, minimumCapacity );
    }

    /**
        Sets the length of this String buffer.

        If the <code>newLength</code> argument is less than the current
        length of the string buffer, the string buffer is truncated to
        contain exactly the number of characters given by the
        <code>newLength</code> argument.
        <p>
        If the <code>newLength</code> argument is greater than or equal
        to the current length, sufficient null characters
        (<code>'&#92;u0000'</code>) are appended to the string buffer so that
        length becomes the <code>newLength</code> argument.
        <p>
        The <code>newLength</code> argument must be greater than or equal
        to <code>0</code>.

        @param      newLength   the new length of the buffer.
     */
    void setLength(sal_Int32 newLength)
    {
        assert(newLength >= 0);
        // Avoid modifications if pData points to const empty string:
        if( newLength != pData->length )
        {
            if( newLength > nCapacity )
                rtl_uStringbuffer_ensureCapacity(&pData, &nCapacity, newLength);
            else
                pData->buffer[newLength] = 0;
            pData->length = newLength;
        }
    }

    /**
        Returns the character at a specific index in this string buffer.

        The first character of a string buffer is at index
        <code>0</code>, the next at index <code>1</code>, and so on, for
        array indexing.
        <p>
        The index argument must be greater than or equal to
        <code>0</code>, and less than the length of this string buffer.

        @param      index   the index of the desired character.
        @return     the character at the specified index of this string buffer.
     */
    SAL_DEPRECATED("use rtl::OUStringBuffer::operator [] instead")
    sal_Unicode charAt( sal_Int32 index ) const
    {
        assert(index >= 0 && index < pData->length);
        return pData->buffer[ index ];
    }

    /**
        The character at the specified index of this string buffer is set
        to <code>ch</code>.

        The index argument must be greater than or equal to
        <code>0</code>, and less than the length of this string buffer.

        @param      index   the index of the character to modify.
        @param      ch      the new character.
     */
    SAL_DEPRECATED("use rtl::OUStringBuffer::operator [] instead")
    OUStringBuffer & setCharAt(sal_Int32 index, sal_Unicode ch)
    {
        assert(index >= 0 && index < pData->length);
        pData->buffer[ index ] = ch;
        return *this;
    }

    /**
        Return a null terminated unicode character array.
     */
    const sal_Unicode*  getStr() const { return pData->buffer; }

    /**
      Access to individual characters.

      @param index must be non-negative and less than length.

      @return a reference to the character at the given index.

      @Since SnipeOffice 3.5
    */
    sal_Unicode & operator [](sal_Int32 index) { return pData->buffer[index]; }

    /**
        Return a OUString instance reflecting the current content
        of this OUStringBuffer.
     */
    const OUString toString() const
    {
        return OUString(pData->buffer, pData->length);
    }

    /**
        Appends the string to this string buffer.

        The characters of the <code>OUString</code> argument are appended, in
        order, to the contents of this string buffer, increasing the
        length of this string buffer by the length of the argument.

        @param   str   a string.
        @return  this string buffer.
     */
    OUStringBuffer & append(const OUString &str)
    {
        return append( str.getStr(), str.getLength() );
    }

    /**
        Appends the content of a stringbuffer to this string buffer.

        The characters of the <code>OUStringBuffer</code> argument are appended, in
        order, to the contents of this string buffer, increasing the
        length of this string buffer by the length of the argument.

        @param   str   a string.
        @return  this string buffer.

        @Since SnipeOffice 4.0
     */
    OUStringBuffer & append(const OUStringBuffer &str)
    {
        if(str.getLength() > 0)
        {
            append( str.getStr(), str.getLength() );
        }
        return *this;
    }

    /**
        Appends the string representation of the <code>char</code> array
        argument to this string buffer.

        The characters of the array argument are appended, in order, to
        the contents of this string buffer. The length of this string
        buffer increases by the length of the argument.

        @param   str   the characters to be appended.
        @return  this string buffer.
     */
    OUStringBuffer & append( const sal_Unicode * str )
    {
        return append( str, rtl_ustr_getLength( str ) );
    }

    /**
        Appends the string representation of the <code>char</code> array
        argument to this string buffer.

        Characters of the character array <code>str</code> are appended,
        in order, to the contents of this string buffer. The length of this
        string buffer increases by the value of <code>len</code>.

        @param str the characters to be appended; must be non-null, and must
        point to at least len characters
        @param len the number of characters to append; must be non-negative
        @return  this string buffer.
     */
    OUStringBuffer & append( const sal_Unicode * str, sal_Int32 len)
    {
        // insert behind the last character
        rtl_uStringbuffer_insert( &pData, &nCapacity, getLength(), str, len );
        return *this;
    }

    /**
        @overload
        This function accepts an ASCII string literal as its argument.
        @Since SnipeOffice 3.6
     */
    template< typename T >
    typename internal::ConstCharArrayDetector< T, OUStringBuffer& >::Type append( T& literal )
    {
        assert( strlen( literal ) == internal::ConstCharArrayDetector< T >::size - 1 );
        rtl_uStringbuffer_insert_ascii( &pData, &nCapacity, getLength(), literal,
            internal::ConstCharArrayDetector< T, void >::size - 1 );
        return *this;
    }

    /**
        Appends a 8-Bit ASCII character string to this string buffer.

       Since this method is optimized for performance. the ASCII
        character values are not converted in any way. The caller
        has to make sure that all ASCII characters are in the
        allowed range between 0 and 127. The ASCII string must be
        NULL-terminated.
        <p>
        The characters of the array argument are appended, in order, to
        the contents of this string buffer. The length of this string
        buffer increases by the length of the argument.

        @param   str   the 8-Bit ASCII characters to be appended.
        @return  this string buffer.
     */
    OUStringBuffer & appendAscii( const sal_Char * str )
    {
        return appendAscii( str, rtl_str_getLength( str ) );
    }

    /**
        Appends a 8-Bit ASCII character string to this string buffer.

        Since this method is optimized for performance. the ASCII
        character values are not converted in any way. The caller
        has to make sure that all ASCII characters are in the
        allowed range between 0 and 127. The ASCII string must be
        NULL-terminated.
        <p>
        Characters of the character array <code>str</code> are appended,
        in order, to the contents of this string buffer. The length of this
        string buffer increases by the value of <code>len</code>.

        @param str the 8-Bit ASCII characters to be appended; must be non-null,
        and must point to at least len characters
        @param len the number of characters to append; must be non-negative
        @return  this string buffer.
     */
    OUStringBuffer & appendAscii( const sal_Char * str, sal_Int32 len)
    {
        rtl_uStringbuffer_insert_ascii( &pData, &nCapacity, getLength(), str, len );
        return *this;
    }

    /**
        Appends the string representation of the <code>sal_Bool</code>
        argument to the string buffer.

        The argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then appended to this string buffer.

        @param   b   a <code>sal_Bool</code>.
        @return  this string buffer.
     */
    OUStringBuffer & append(sal_Bool b)
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFBOOLEAN];
        return append( sz, rtl_ustr_valueOfBoolean( sz, b ) );
    }

    /**
        Appends the string representation of the ASCII <code>char</code>
        argument to this string buffer.

        The argument is appended to the contents of this string buffer.
        The length of this string buffer increases by <code>1</code>.

        @param   c   an ASCII <code>char</code>.
        @return  this string buffer.

        @Since SnipeOffice 3.5
     */
    OUStringBuffer & append(char c)
    {
        assert(static_cast< unsigned char >(c) <= 0x7F);
        return append(sal_Unicode(c));
    }

    /**
        Appends the string representation of the <code>char</code>
        argument to this string buffer.

        The argument is appended to the contents of this string buffer.
        The length of this string buffer increases by <code>1</code>.

        @param   c   a <code>char</code>.
        @return  this string buffer.
     */
    OUStringBuffer & append(sal_Unicode c)
    {
        return append( &c, 1 );
    }

    /**
        Appends the string representation of the <code>sal_Int32</code>
        argument to this string buffer.

        The argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then appended to this string buffer.

        @param   i   an <code>sal_Int32</code>.
        @return  this string buffer.
     */
    OUStringBuffer & append(sal_Int32 i, sal_Int16 radix = 10 )
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFINT32];
        return append( sz, rtl_ustr_valueOfInt32( sz, i, radix ) );
    }

    /**
        Appends the string representation of the <code>long</code>
        argument to this string buffer.

        The argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then appended to this string buffer.

        @param   l   a <code>long</code>.
        @return  this string buffer.
     */
    OUStringBuffer & append(sal_Int64 l, sal_Int16 radix = 10 )
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFINT64];
        return append( sz, rtl_ustr_valueOfInt64( sz, l, radix ) );
    }

    /**
        Appends the string representation of the <code>float</code>
        argument to this string buffer.

        The argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then appended to this string buffer.

        @param   f   a <code>float</code>.
        @return  this string buffer.
     */
    OUStringBuffer & append(float f)
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFFLOAT];
        return append( sz, rtl_ustr_valueOfFloat( sz, f ) );
    }

    /**
        Appends the string representation of the <code>double</code>
        argument to this string buffer.

        The argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then appended to this string buffer.

        @param   d   a <code>double</code>.
        @return  this string buffer.
     */
    OUStringBuffer & append(double d)
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFDOUBLE];
        return append( sz, rtl_ustr_valueOfDouble( sz, d ) );
    }

    /**
       Appends a single UTF-32 character to this string buffer.

       <p>The single UTF-32 character will be represented within the string
       buffer as either one or two UTF-16 code units.</p>

       @param c a well-formed UTF-32 code unit (that is, a value in the range
       <code>0</code>&ndash;<code>0x10FFFF</code>, but excluding
       <code>0xD800</code>&ndash;<code>0xDFFF</code>)

       @return
       this string buffer
     */
    OUStringBuffer & appendUtf32(sal_uInt32 c) {
        return insertUtf32(getLength(), c);
    }

    /**
        Inserts the string into this string buffer.

        The characters of the <code>String</code> argument are inserted, in
        order, into this string buffer at the indicated offset. The length
        of this string buffer is increased by the length of the argument.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      str      a string.
        @return     this string buffer.
     */
    OUStringBuffer & insert(sal_Int32 offset, const OUString & str)
    {
        return insert( offset, str.getStr(), str.getLength() );
    }

    /**
        Inserts the string representation of the <code>char</code> array
        argument into this string buffer.

        The characters of the array argument are inserted into the
        contents of this string buffer at the position indicated by
        <code>offset</code>. The length of this string buffer increases by
        the length of the argument.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      str      a character array.
        @return     this string buffer.
     */
    OUStringBuffer & insert( sal_Int32 offset, const sal_Unicode * str )
    {
        return insert( offset, str, rtl_ustr_getLength( str ) );
    }

    /**
        Inserts the string representation of the <code>char</code> array
        argument into this string buffer.

        The characters of the array argument are inserted into the
        contents of this string buffer at the position indicated by
        <code>offset</code>. The length of this string buffer increases by
        the length of the argument.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      str      a character array.
        @param      len     the number of characters to append.
        @return     this string buffer.
     */
    OUStringBuffer & insert( sal_Int32 offset, const sal_Unicode * str, sal_Int32 len)
    {
        // insert behind the last character
        rtl_uStringbuffer_insert( &pData, &nCapacity, offset, str, len );
        return *this;
    }

    /**
        @overload
        This function accepts an ASCII string literal as its argument.
        @Since SnipeOffice 3.6
     */
    template< typename T >
    typename internal::ConstCharArrayDetector< T, OUStringBuffer& >::Type insert( sal_Int32 offset, T& literal )
    {
        assert( strlen( literal ) == internal::ConstCharArrayDetector< T >::size - 1 );
        rtl_uStringbuffer_insert_ascii( &pData, &nCapacity, offset, literal,
            internal::ConstCharArrayDetector< T, void >::size - 1 );
        return *this;
    }

    /**
        Inserts the string representation of the <code>sal_Bool</code>
        argument into this string buffer.

        The second argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then inserted into this string buffer at the indicated
        offset.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      b        a <code>sal_Bool</code>.
        @return     this string buffer.
     */
    OUStringBuffer & insert(sal_Int32 offset, sal_Bool b)
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFBOOLEAN];
        return insert( offset, sz, rtl_ustr_valueOfBoolean( sz, b ) );
    }

    /**
        Inserts the string representation of the <code>char</code>
        argument into this string buffer.

        The second argument is inserted into the contents of this string
        buffer at the position indicated by <code>offset</code>. The length
        of this string buffer increases by one.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      c        a <code>char</code>.
        @return     this string buffer.

        @Since SnipeOffice 3.6
     */
    OUStringBuffer & insert(sal_Int32 offset, char c)
    {
        sal_Unicode u = c;
        return insert( offset, &u, 1 );
    }

    /**
        Inserts the string representation of the <code>char</code>
        argument into this string buffer.

        The second argument is inserted into the contents of this string
        buffer at the position indicated by <code>offset</code>. The length
        of this string buffer increases by one.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      c        a <code>char</code>.
        @return     this string buffer.
     */
    OUStringBuffer & insert(sal_Int32 offset, sal_Unicode c)
    {
        return insert( offset, &c, 1 );
    }

    /**
        Inserts the string representation of the second <code>sal_Int32</code>
        argument into this string buffer.

        The second argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then inserted into this string buffer at the indicated
        offset.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      i        an <code>sal_Int32</code>.
        @param      radix    the radix.
        @return     this string buffer.
        @exception  StringIndexOutOfBoundsException  if the offset is invalid.
     */
    OUStringBuffer & insert(sal_Int32 offset, sal_Int32 i, sal_Int16 radix = 10 )
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFINT32];
        return insert( offset, sz, rtl_ustr_valueOfInt32( sz, i, radix ) );
    }

    /**
        Inserts the string representation of the <code>long</code>
        argument into this string buffer.

        The second argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then inserted into this string buffer at the indicated
        offset.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      l        a <code>long</code>.
        @param      radix    the radix.
        @return     this string buffer.
        @exception  StringIndexOutOfBoundsException  if the offset is invalid.
     */
    OUStringBuffer & insert(sal_Int32 offset, sal_Int64 l, sal_Int16 radix = 10 )
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFINT64];
        return insert( offset, sz, rtl_ustr_valueOfInt64( sz, l, radix ) );
    }

    /**
        Inserts the string representation of the <code>float</code>
        argument into this string buffer.

        The second argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then inserted into this string buffer at the indicated
        offset.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      f        a <code>float</code>.
        @return     this string buffer.
        @exception  StringIndexOutOfBoundsException  if the offset is invalid.
     */
    OUStringBuffer insert(sal_Int32 offset, float f)
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFFLOAT];
        return insert( offset, sz, rtl_ustr_valueOfFloat( sz, f ) );
    }

    /**
        Inserts the string representation of the <code>double</code>
        argument into this string buffer.

        The second argument is converted to a string as if by the method
        <code>String.valueOf</code>, and the characters of that
        string are then inserted into this string buffer at the indicated
        offset.
        <p>
        The offset argument must be greater than or equal to
        <code>0</code>, and less than or equal to the length of this
        string buffer.

        @param      offset   the offset.
        @param      d        a <code>double</code>.
        @return     this string buffer.
        @exception  StringIndexOutOfBoundsException  if the offset is invalid.
     */
    OUStringBuffer & insert(sal_Int32 offset, double d)
    {
        sal_Unicode sz[RTL_USTR_MAX_VALUEOFDOUBLE];
        return insert( offset, sz, rtl_ustr_valueOfDouble( sz, d ) );
    }

    /**
       Inserts a single UTF-32 character into this string buffer.

       <p>The single UTF-32 character will be represented within the string
       buffer as either one or two UTF-16 code units.</p>

       @param offset the offset into this string buffer (from zero to the length
       of this string buffer, inclusive)

       @param c a well-formed UTF-32 code unit (that is, a value in the range
       <code>0</code>&ndash;<code>0x10FFFF</code>, but excluding
       <code>0xD800</code>&ndash;<code>0xDFFF</code>)

       @return this string buffer
     */
    OUStringBuffer & insertUtf32(sal_Int32 offset, sal_uInt32 c) {
        rtl_uStringbuffer_insertUtf32(&pData, &nCapacity, offset, c);
        return *this;
    }

    /**
        Removes the characters in a substring of this sequence.

        The substring begins at the specified <code>start</code> and
        is <code>len</code> characters long.

        start must be >= 0 && <= This->length

        @param  start       The beginning index, inclusive
        @param  len         The substring length
        @return this string buffer.
     */
    OUStringBuffer & remove( sal_Int32 start, sal_Int32 len )
    {
        rtl_uStringbuffer_remove( &pData, start, len );
        return *this;
    }

    /**
        Removes the tail of a string buffer start at the indicate position

        start must be >= 0 && <= This->length

        @param  start       The beginning index, inclusive. default to 0
        @return this string buffer.

        @Since SnipeOffice 4.0
     */
    OUStringBuffer & truncate( sal_Int32 start = 0 )
    {
        rtl_uStringbuffer_remove( &pData, start, getLength() - start );
        return *this;
    }

    /**
       Replace all occurrences of
       oldChar in this string buffer with newChar.

       @Since SnipeOffice 4.0

       @param    oldChar     the old character.
       @param    newChar     the new character.
       @return   this string buffer
    */
    OUStringBuffer& replace( sal_Unicode oldChar, sal_Unicode newChar )
    {
        sal_Int32 index = 0;
        while((index = indexOf(oldChar, index)) >= 0)
        {
            pData->buffer[ index ] = newChar;
        }
        return *this;
    }

    /** Allows access to the internal data of this OUStringBuffer, for effective
        manipulation.

        This method should be used with care.  After you have called this
        method, you may use the returned pInternalData or pInternalCapacity only
        as long as you make no other method call on this OUStringBuffer.

        @param pInternalData
        This output parameter receives a pointer to the internal data
        (rtl_uString pointer).  pInternalData itself must not be null.

        @param pInternalCapacity
        This output parameter receives a pointer to the internal capacity.
        pInternalCapacity itself must not be null.
     */
    inline void accessInternals(rtl_uString *** pInternalData,
                                sal_Int32 ** pInternalCapacity)
    {
        *pInternalData = &pData;
        *pInternalCapacity = &nCapacity;
    }


    /**
       Returns the index within this string of the first occurrence of the
       specified character, starting the search at the specified index.

       @Since SnipeOffice 4.0

       @param    ch          character to be located.
       @param    fromIndex   the index to start the search from.
                             The index must be greater or equal than 0
                             and less or equal as the string length.
       @return   the index of the first occurrence of the character in the
                 character sequence represented by this string that is
                 greater than or equal to fromIndex, or
                 -1 if the character does not occur.
    */
    sal_Int32 indexOf( sal_Unicode ch, sal_Int32 fromIndex = 0 ) const SAL_THROW(())
    {
        sal_Int32 ret = rtl_ustr_indexOfChar_WithLength( pData->buffer+fromIndex, pData->length-fromIndex, ch );
        return (ret < 0 ? ret : ret+fromIndex);
    }

    /**
       Returns the index within this string of the last occurrence of the
       specified character, searching backward starting at the end.

       @Since SnipeOffice 4.0

       @param    ch          character to be located.
       @return   the index of the last occurrence of the character in the
                 character sequence represented by this string, or
                 -1 if the character does not occur.
    */
    sal_Int32 lastIndexOf( sal_Unicode ch ) const SAL_THROW(())
    {
        return rtl_ustr_lastIndexOfChar_WithLength( pData->buffer, pData->length, ch );
    }

    /**
       Returns the index within this string of the last occurrence of the
       specified character, searching backward starting before the specified
       index.

       @Since SnipeOffice 4.0

       @param    ch          character to be located.
       @param    fromIndex   the index before which to start the search.
       @return   the index of the last occurrence of the character in the
                 character sequence represented by this string that
                 is less than fromIndex, or -1
                 if the character does not occur before that point.
    */
    sal_Int32 lastIndexOf( sal_Unicode ch, sal_Int32 fromIndex ) const SAL_THROW(())
    {
        return rtl_ustr_lastIndexOfChar_WithLength( pData->buffer, fromIndex, ch );
    }

    /**
       Returns the index within this string of the first occurrence of the
       specified substring, starting at the specified index.

       If str doesn't include any character, always -1 is
       returned. This is also the case, if both strings are empty.

       @Since SnipeOffice 4.0

       @param    str         the substring to search for.
       @param    fromIndex   the index to start the search from.
       @return   If the string argument occurs one or more times as a substring
                 within this string at the starting index, then the index
                 of the first character of the first such substring is
                 returned. If it does not occur as a substring starting
                 at fromIndex or beyond, -1 is returned.
    */
    sal_Int32 indexOf( const OUString & str, sal_Int32 fromIndex = 0 ) const SAL_THROW(())
    {
        sal_Int32 ret = rtl_ustr_indexOfStr_WithLength( pData->buffer+fromIndex, pData->length-fromIndex,
                                                        str.pData->buffer, str.pData->length );
        return (ret < 0 ? ret : ret+fromIndex);
    }

    /**
       @overload
       This function accepts an ASCII string literal as its argument.

       @Since SnipeOffice 4.0
    */
    template< typename T >
    typename internal::ConstCharArrayDetector< T, sal_Int32 >::Type indexOf( T& literal, sal_Int32 fromIndex = 0 ) const SAL_THROW(())
    {
        assert( strlen( literal ) == internal::ConstCharArrayDetector< T >::size - 1 );
        sal_Int32 ret = rtl_ustr_indexOfAscii_WithLength(
            pData->buffer + fromIndex, pData->length - fromIndex, literal,
            internal::ConstCharArrayDetector< T, void >::size - 1);
        return ret < 0 ? ret : ret + fromIndex;
    }

    /**
       Returns the index within this string of the last occurrence of
       the specified substring, searching backward starting at the end.

       The returned index indicates the starting index of the substring
       in this string.
       If str doesn't include any character, always -1 is
       returned. This is also the case, if both strings are empty.

       @Since SnipeOffice 4.0

       @param    str         the substring to search for.
       @return   If the string argument occurs one or more times as a substring
                 within this string, then the index of the first character of
                 the last such substring is returned. If it does not occur as
                 a substring, -1 is returned.
    */
    sal_Int32 lastIndexOf( const OUString & str ) const SAL_THROW(())
    {
        return rtl_ustr_lastIndexOfStr_WithLength( pData->buffer, pData->length,
                                                   str.pData->buffer, str.pData->length );
    }

    /**
       Returns the index within this string of the last occurrence of
       the specified substring, searching backward starting before the specified
       index.

       The returned index indicates the starting index of the substring
       in this string.
       If str doesn't include any character, always -1 is
       returned. This is also the case, if both strings are empty.

       @Since SnipeOffice 4.0

       @param    str         the substring to search for.
       @param    fromIndex   the index before which to start the search.
       @return   If the string argument occurs one or more times as a substring
                 within this string before the starting index, then the index
                 of the first character of the last such substring is
                 returned. Otherwise, -1 is returned.
    */
    sal_Int32 lastIndexOf( const OUString & str, sal_Int32 fromIndex ) const SAL_THROW(())
    {
        return rtl_ustr_lastIndexOfStr_WithLength( pData->buffer, fromIndex,
                                                   str.pData->buffer, str.pData->length );
    }

    /**
       @overload
       This function accepts an ASCII string literal as its argument.
       @Since SnipeOffice 4.0
    */
    template< typename T >
    typename internal::ConstCharArrayDetector< T, sal_Int32 >::Type lastIndexOf( T& literal ) const SAL_THROW(())
    {
        assert( strlen( literal ) == internal::ConstCharArrayDetector< T >::size - 1 );
        return rtl_ustr_lastIndexOfAscii_WithLength(
            pData->buffer, pData->length, literal, internal::ConstCharArrayDetector< T, void >::size - 1);
    }

    /**
       Strip the given character from the start of the buffer.

       @Since SnipeOffice 4.0

       @param    c         the character to strip
       @return   The number of characters stripped

    */
    sal_Int32 stripStart(sal_Unicode c = (sal_Unicode)' ')
    {
        sal_Int32 index;
        for(index = 0; index < getLength() ; index++)
        {
            if(pData->buffer[ index ] != c)
            {
                break;
            }
        }
        if(index)
        {
            remove(0, index);
        }
        return index;
    }

    /**
       Strip the given character from the end of the buffer.

       @Since SnipeOffice 4.0

       @param    c         the character to strip
       @return   The number of characters stripped

    */
    sal_Int32 stripEnd(sal_Unicode c = (sal_Unicode)' ')
    {
        sal_Int32 result = getLength();
        sal_Int32 index;
        for(index = getLength(); index > 0 ; index--)
        {
            if(pData->buffer[ index - 1 ] != c)
            {
                break;
            }
        }
        if(index < getLength())
        {
            truncate(index);
        }
        return result - getLength();
    }
    /**
       Strip the given character from the both end of the buffer.

       @Since SnipeOffice 4.0

       @param    c         the character to strip
       @return   The number of characters stripped

    */
    sal_Int32 strip(sal_Unicode c = (sal_Unicode)' ')
    {
        return stripStart(c) + stripEnd(c);
    }

private:
    /**
        A pointer to the data structur which contains the data.
     */
    rtl_uString * pData;

    /**
        The len of the pData->buffer.
     */
    sal_Int32       nCapacity;
};

#ifdef RTL_FAST_STRING
template<>
struct ToStringHelper< OUStringBuffer >
    {
    static int length( const OUStringBuffer& s ) { return s.getLength(); }
    static sal_Unicode* addData( sal_Unicode* buffer, const OUStringBuffer& s ) { return addDataHelper( buffer, s.getStr(), s.getLength()); }
    static const bool allowOStringConcat = false;
    static const bool allowOUStringConcat = true;
    };
#endif

}

#ifdef RTL_STRING_UNITTEST
namespace rtl
{
typedef rtlunittest::OUStringBuffer OUStringBuffer;
}
#endif

#ifdef RTL_USING
using ::rtl::OUStringBuffer;
#endif

#endif  /* _RTL_USTRBUF_HXX_ */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

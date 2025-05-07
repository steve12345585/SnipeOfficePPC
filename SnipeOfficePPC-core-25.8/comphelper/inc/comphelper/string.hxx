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

#ifndef INCLUDED_COMPHELPER_STRING_HXX
#define INCLUDED_COMPHELPER_STRING_HXX

#include "sal/config.h"

#include <cstddef>
#include "comphelper/comphelperdllapi.h"
#include <sal/types.h>
#include <rtl/strbuf.hxx>
#include <rtl/ustrbuf.hxx>
#include <com/sun/star/uno/Sequence.hxx>

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/i18n/XCollator.hpp>
#include <com/sun/star/i18n/XBreakIterator.hpp>

// rtl::OUString helper functions that are not widespread or mature enough to
// go into the stable URE API:
namespace comphelper { namespace string {

/** Allocate a new string containing space for a given number of characters.

    The reference count of the new string will be 1. The length of the string
    will be nLen. This function does not handle out-of-memory conditions.

    The characters of the capacity are not cleared, and the length is set to
    nLen, unlike the similar method of rtl_uString_new_WithLength which
    zeros out the buffer, and sets the length to 0. So should be somewhat
    more efficient for allocating a new string.

    call rtl_uString_release to release the string
    alternatively pass ownership to an OUString with
    rtl::OUString(newStr, SAL_NO_ACQUIRE);

    @param newStr
    pointer to the new string.

    @param len
    the number of characters.
 */
COMPHELPER_DLLPUBLIC rtl_uString * SAL_CALL rtl_uString_alloc(sal_Int32 nLen);

/** Allocate a new string containing space for a given number of characters.

    The reference count of the new string will be 1. The length of the string
    will be nLen. This function does not handle out-of-memory conditions.

    The characters of the capacity are not cleared, and the length is set to
    nLen, unlike the similar method of rtl_String_new_WithLength which
    zeros out the buffer, and sets the length to 0. So should be somewhat
    more efficient for allocating a new string.

    call rtl_String_release to release the string
    alternatively pass ownership to an OUString with
    rtl::OUString(newStr, SAL_NO_ACQUIRE);

    @param newStr
    pointer to the new string.

    @param len
    the number of characters.
 */
COMPHELPER_DLLPUBLIC rtl_String * SAL_CALL rtl_string_alloc(sal_Int32 nLen);

/** Removes all occurrences of a character from within the source string

    @deprecated  Use rtl::OString::replaceAll(rtl::OString(c), rtl::OString())
    instead.

    @param rIn      The input OString
    @param c        The character to be removed

    @return         The resulting OString
 */
inline rtl::OString remove(const rtl::OString &rIn,
    sal_Char c)
{ return rIn.replaceAll(rtl::OString(c), rtl::OString()); }

/** Removes all occurrences of a character from within the source string

    @deprecated  Use
    rtl::OUString::replaceAll(rtl::OUString(c), rtl::OUString()) instead.

    @param rIn      The input OUString
    @param c        The character to be removed

    @return         The resulting OUString
 */
inline rtl::OUString remove(const rtl::OUString &rIn,
    sal_Unicode c)
{ return rIn.replaceAll(rtl::OUString(c), rtl::OUString()); }

/** Strips occurrences of a character from the start of the source string

    @param rIn      The input OString
    @param c        The character to be stripped from the start

    @return         The resulting OString
 */
COMPHELPER_DLLPUBLIC rtl::OString stripStart(const rtl::OString &rIn,
    sal_Char c);

/** Strips occurrences of a character from the start of the source string

    @param rIn      The input OUString
    @param c        The character to be stripped from the start

    @return         The resulting OUString
 */
COMPHELPER_DLLPUBLIC rtl::OUString stripStart(const rtl::OUString &rIn,
    sal_Unicode c);

/** Strips occurrences of a character from the end of the source string

    @param rIn      The input OString
    @param c        The character to be stripped from the end

    @return         The resulting OString
 */
COMPHELPER_DLLPUBLIC rtl::OString stripEnd(const rtl::OString &rIn,
    sal_Char c);

/** Strips occurrences of a character from the end of the source string

    @param rIn      The input OUString
    @param c        The character to be stripped from the end

    @return         The resulting OUString
 */
COMPHELPER_DLLPUBLIC rtl::OUString stripEnd(const rtl::OUString &rIn,
    sal_Unicode c);

/** Strips occurrences of a character from the start and end of the source string

    @param rIn      The input OString
    @param c        The character to be stripped from the start and end

    @return         The resulting OString
 */
COMPHELPER_DLLPUBLIC rtl::OString strip(const rtl::OString &rIn,
    sal_Char c);

/** Strips occurrences of a character from the start and end of the source string

    @param rIn      The input OUString
    @param c        The character to be stripped from the start and end

    @return         The resulting OUString
 */
COMPHELPER_DLLPUBLIC rtl::OUString strip(const rtl::OUString &rIn,
    sal_Unicode c);

/** Returns a token in an OString

    @deprecated  Use rtl::OString::getToken(nToken, cTok) instead.

  @param    rIn         the input OString
  @param    nToken      the number of the token to return
  @param    cTok        the character which seperate the tokens.
  @return   the token   if token is negative or doesn't exist an empty token
                        is returned
*/
inline rtl::OString getToken(const rtl::OString &rIn,
    sal_Int32 nToken, sal_Char cTok) SAL_THROW(())
{
    return rIn.getToken(nToken, cTok);
}

/** Returns a token in an OUString

    @deprecated  Use rtl::OUString::getToken(nToken, cTok) instead.

  @param    rIn         the input OUString
  @param    nToken      the number of the token to return
  @param    cTok        the character which seperate the tokens.
  @return   the token   if token is negative or doesn't exist an empty token
                        is returned
*/
inline rtl::OUString getToken(const rtl::OUString &rIn,
    sal_Int32 nToken, sal_Unicode cTok) SAL_THROW(())
{
    return rIn.getToken(nToken, cTok);
}

/** Returns number of tokens in an OUString

  @param    rIn     the input OString
  @param    cTok    the character which seperate the tokens.
  @return   the number of tokens
*/
COMPHELPER_DLLPUBLIC sal_Int32 getTokenCount(const rtl::OString &rIn, sal_Char cTok);

/** Returns number of tokens in an OUString

  @param    rIn     the input OUString
  @param    cTok    the character which seperate the tokens.
  @return   the number of tokens
*/
COMPHELPER_DLLPUBLIC sal_Int32 getTokenCount(const rtl::OUString &rIn, sal_Unicode cTok);

namespace detail
{
    template<typename B> B& truncateToLength(B& rBuffer, sal_Int32 nLen)
    {
        if (nLen < rBuffer.getLength())
            rBuffer.remove(nLen, rBuffer.getLength()-nLen);
        return rBuffer;
    }
}

/** Truncate a buffer to a given length.

    If the StringBuffer has more characters than nLength it will be truncated
    on the right to nLength characters.

    Has no effect if the StringBuffer is <= nLength

    @param rBuf   StringBuffer to operate on
    @param nLength   Length to truncate the buffer to

    @return         rBuf;
 */
COMPHELPER_DLLPUBLIC inline rtl::OStringBuffer& truncateToLength(
    rtl::OStringBuffer& rBuffer, sal_Int32 nLength) SAL_THROW(())
{
    return detail::truncateToLength(rBuffer, nLength);
}

COMPHELPER_DLLPUBLIC inline rtl::OUStringBuffer& truncateToLength(
    rtl::OUStringBuffer& rBuffer, sal_Int32 nLength) SAL_THROW(())
{
    return detail::truncateToLength(rBuffer, nLength);
}

namespace detail
{
    template<typename B, typename U> B& padToLength(B& rBuffer, sal_Int32 nLen,
        U cFill = '\0')
    {
        sal_Int32 nOrigLen = rBuffer.getLength();
        if (nLen > nOrigLen)
        {
            rBuffer.setLength(nLen);
            for (sal_Int32 i = nOrigLen; i < nLen; ++i)
                rBuffer[i] = cFill;
        }
        return rBuffer;
    }
}

/** Pad a buffer to a given length using a given char.

    If the StringBuffer has less characters than nLength it will be expanded on
    the right to nLength characters, with the expansion filled using cFill.

    Has no effect if the StringBuffer is >= nLength

    @param rBuf   StringBuffer to operate on
    @param nLength   Length to pad the buffer to
    @param cFill  character to fill expansion with

    @return         rBuf;
 */
COMPHELPER_DLLPUBLIC inline rtl::OStringBuffer& padToLength(
    rtl::OStringBuffer& rBuffer, sal_Int32 nLength,
    sal_Char cFill = '\0') SAL_THROW(())
{
    return detail::padToLength(rBuffer, nLength, cFill);
}

COMPHELPER_DLLPUBLIC inline rtl::OUStringBuffer& padToLength(
    rtl::OUStringBuffer& rBuffer, sal_Int32 nLength,
    sal_Unicode cFill = '\0') SAL_THROW(())
{
    return detail::padToLength(rBuffer, nLength, cFill);
}

COMPHELPER_DLLPUBLIC rtl::OUString removeTrailingChars(
    const rtl::OUString& rStr, sal_Unicode cChar);

/** Convert a sequence of strings to a single comma separated string.

    Note that no escaping of commas or anything fancy is done.

    @param i_rSeq   A list of strings to be concatenated.

    @return         A single string containing the concatenation of the given
                    list, interspersed with the string ", ".
 */
COMPHELPER_DLLPUBLIC ::rtl::OUString convertCommaSeparated(
    ::com::sun::star::uno::Sequence< ::rtl::OUString > const & i_rSeq);

/** Convert a decimal string to a number.

    The string must be base-10, no sign but can contain any
    codepoint listed in the "Number, Decimal Digit" Unicode
    category.

    No verification is made about the validity of the string,
    passing string not containing decimal digit code points
    gives unspecified results

    If your string is guaranteed to contain only ASCII digit
    use rtl::OUString::toInt32 instead.

    @param str  The string to convert containing only decimal
                digit codepoints.

    @return     The value of the string as an int32.
 */
COMPHELPER_DLLPUBLIC sal_uInt32 decimalStringToNumber(
    ::rtl::OUString const & str );

/** Convert a single comma separated string to a sequence of strings.

    Note that no escaping of commas or anything fancy is done.

    @param i_rString    A string containing comma-separated words.

    @return         A sequence of strings resulting from splitting the given
                    string at ',' tokens and stripping whitespace.
 */
COMPHELPER_DLLPUBLIC ::com::sun::star::uno::Sequence< ::rtl::OUString >
    convertCommaSeparated( ::rtl::OUString const & i_rString );

/**
  Compares two strings using natural order.

  For non digit characters, the comparison use the same algorithm as
  rtl_str_compare. When a number is encountered during the comparison,
  natural order is used. Thus, Heading 10 will be considered as greater
  than Heading 2. Numerical comparison is done using decimal representation.

  Beware that "MyString 001" and "MyString 1" will be considered as equal
  since leading 0 are meaningless.

  @param    str         the object to be compared.
  @return   0 - if both strings are equal
            < 0 - if this string is less than the string argument
            > 0 - if this string is greater than the string argument
*/
COMPHELPER_DLLPUBLIC sal_Int32 compareNatural( const ::rtl::OUString &rLHS, const ::rtl::OUString &rRHS,
    const ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XCollator > &rCollator,
    const ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XBreakIterator > &rBI,
    const ::com::sun::star::lang::Locale &rLocale );

class COMPHELPER_DLLPUBLIC NaturalStringSorter
{
private:
    ::com::sun::star::lang::Locale m_aLocale;
    ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XCollator > m_xCollator;
    ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XBreakIterator > m_xBI;
public:
    NaturalStringSorter(
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > &rContext,
        const ::com::sun::star::lang::Locale &rLocale);
    sal_Int32 compare(const rtl::OUString &rLHS, const rtl::OUString &rRHS) const
    {
        return compareNatural(rLHS, rRHS, m_xCollator, m_xBI, m_aLocale);
    }
    const ::com::sun::star::lang::Locale& getLocale() const { return m_aLocale; }
};

/** Determine if an OString contains solely ASCII numeric digits

    @param rString  An OString

    @return         false if string contains any characters outside
                    the ASCII '0'-'9' range
                    true otherwise, including for empty string
 */
COMPHELPER_DLLPUBLIC bool isdigitAsciiString(const rtl::OString &rString);

/** Determine if an OUString contains solely ASCII numeric digits

    @param rString  An OUString

    @return         false if string contains any characters outside
                    the ASCII '0'-'9' range
                    true otherwise, including for empty string
 */
COMPHELPER_DLLPUBLIC bool isdigitAsciiString(const rtl::OUString &rString);

COMPHELPER_DLLPUBLIC inline bool isdigitAscii(sal_Unicode c)
{
    return ((c >= '0') && (c <= '9'));
}

COMPHELPER_DLLPUBLIC inline bool isxdigitAscii(sal_Unicode c)
{
    return isdigitAscii(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

COMPHELPER_DLLPUBLIC inline bool islowerAscii(sal_Unicode c)
{
    return ((c >= 'a') && (c <= 'z'));
}

COMPHELPER_DLLPUBLIC inline bool isupperAscii(sal_Unicode c)
{
    return ((c >= 'A') && (c <= 'Z'));
}

COMPHELPER_DLLPUBLIC inline bool isalphaAscii(sal_Unicode c)
{
    return islowerAscii(c) || isupperAscii(c);
}

COMPHELPER_DLLPUBLIC inline bool isalnumAscii(sal_Unicode c)
{
    return isalphaAscii(c) || isdigitAscii(c);
}

//============================================================
//= a helper for static ascii pseudo-unicode strings
//============================================================
struct COMPHELPER_DLLPUBLIC ConstAsciiString
{
    const sal_Char* ascii;
    sal_Int32       length;

    operator rtl::OUString() const
    {
        return rtl::OUString(ascii, length, RTL_TEXTENCODING_ASCII_US);
    }
};

} }

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

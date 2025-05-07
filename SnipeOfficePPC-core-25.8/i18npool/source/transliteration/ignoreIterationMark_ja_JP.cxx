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


#include <utility>
#include <comphelper/string.hxx>
#include <i18nutil/oneToOneMapping.hxx>
#define TRANSLITERATION_IterationMark_ja_JP
#include <transliteration_Ignore.hxx>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;

using ::rtl::OUString;

namespace com { namespace sun { namespace star { namespace i18n {

OneToOneMappingTable_t ignoreIterationMark_ja_JP_mappingTable[] = {
    { 0x3046, 0x3094 },  // HIRAGANA LETTER U --> HIRAGANA LETTER VU
    { 0x304B, 0x304C },  // HIRAGANA LETTER KA --> HIRAGANA LETTER GA
    { 0x304D, 0x304E },  // HIRAGANA LETTER KI --> HIRAGANA LETTER GI
    { 0x304F, 0x3050 },  // HIRAGANA LETTER KU --> HIRAGANA LETTER GU
    { 0x3051, 0x3052 },  // HIRAGANA LETTER KE --> HIRAGANA LETTER GE
    { 0x3053, 0x3054 },  // HIRAGANA LETTER KO --> HIRAGANA LETTER GO
    { 0x3055, 0x3056 },  // HIRAGANA LETTER SA --> HIRAGANA LETTER ZA
    { 0x3057, 0x3058 },  // HIRAGANA LETTER SI --> HIRAGANA LETTER ZI
    { 0x3059, 0x305A },  // HIRAGANA LETTER SU --> HIRAGANA LETTER ZU
    { 0x305B, 0x305C },  // HIRAGANA LETTER SE --> HIRAGANA LETTER ZE
    { 0x305D, 0x305E },  // HIRAGANA LETTER SO --> HIRAGANA LETTER ZO
    { 0x305F, 0x3060 },  // HIRAGANA LETTER TA --> HIRAGANA LETTER DA
    { 0x3061, 0x3062 },  // HIRAGANA LETTER TI --> HIRAGANA LETTER DI
    { 0x3064, 0x3065 },  // HIRAGANA LETTER TU --> HIRAGANA LETTER DU
    { 0x3066, 0x3067 },  // HIRAGANA LETTER TE --> HIRAGANA LETTER DE
    { 0x3068, 0x3069 },  // HIRAGANA LETTER TO --> HIRAGANA LETTER DO
    { 0x306F, 0x3070 },  // HIRAGANA LETTER HA --> HIRAGANA LETTER BA
    { 0x3072, 0x3073 },  // HIRAGANA LETTER HI --> HIRAGANA LETTER BI
    { 0x3075, 0x3076 },  // HIRAGANA LETTER HU --> HIRAGANA LETTER BU
    { 0x3078, 0x3079 },  // HIRAGANA LETTER HE --> HIRAGANA LETTER BE
    { 0x307B, 0x307C },  // HIRAGANA LETTER HO --> HIRAGANA LETTER BO
    { 0x309D, 0x309E },  // HIRAGANA ITERATION MARK --> HIRAGANA VOICED ITERATION MARK
    { 0x30A6, 0x30F4 },  // KATAKANA LETTER U --> KATAKANA LETTER VU
    { 0x30AB, 0x30AC },  // KATAKANA LETTER KA --> KATAKANA LETTER GA
    { 0x30AD, 0x30AE },  // KATAKANA LETTER KI --> KATAKANA LETTER GI
    { 0x30AF, 0x30B0 },  // KATAKANA LETTER KU --> KATAKANA LETTER GU
    { 0x30B1, 0x30B2 },  // KATAKANA LETTER KE --> KATAKANA LETTER GE
    { 0x30B3, 0x30B4 },  // KATAKANA LETTER KO --> KATAKANA LETTER GO
    { 0x30B5, 0x30B6 },  // KATAKANA LETTER SA --> KATAKANA LETTER ZA
    { 0x30B7, 0x30B8 },  // KATAKANA LETTER SI --> KATAKANA LETTER ZI
    { 0x30B9, 0x30BA },  // KATAKANA LETTER SU --> KATAKANA LETTER ZU
    { 0x30BB, 0x30BC },  // KATAKANA LETTER SE --> KATAKANA LETTER ZE
    { 0x30BD, 0x30BE },  // KATAKANA LETTER SO --> KATAKANA LETTER ZO
    { 0x30BF, 0x30C0 },  // KATAKANA LETTER TA --> KATAKANA LETTER DA
    { 0x30C1, 0x30C2 },  // KATAKANA LETTER TI --> KATAKANA LETTER DI
    { 0x30C4, 0x30C5 },  // KATAKANA LETTER TU --> KATAKANA LETTER DU
    { 0x30C6, 0x30C7 },  // KATAKANA LETTER TE --> KATAKANA LETTER DE
    { 0x30C8, 0x30C9 },  // KATAKANA LETTER TO --> KATAKANA LETTER DO
    { 0x30CF, 0x30D0 },  // KATAKANA LETTER HA --> KATAKANA LETTER BA
    { 0x30D2, 0x30D3 },  // KATAKANA LETTER HI --> KATAKANA LETTER BI
    { 0x30D5, 0x30D6 },  // KATAKANA LETTER HU --> KATAKANA LETTER BU
    { 0x30D8, 0x30D9 },  // KATAKANA LETTER HE --> KATAKANA LETTER BE
    { 0x30DB, 0x30DC },  // KATAKANA LETTER HO --> KATAKANA LETTER BO
    { 0x30EF, 0x30F7 },  // KATAKANA LETTER WA --> KATAKANA LETTER VA
    { 0x30F0, 0x30F8 },  // KATAKANA LETTER WI --> KATAKANA LETTER VI
    { 0x30F1, 0x30F9 },  // KATAKANA LETTER WE --> KATAKANA LETTER VE
    { 0x30F2, 0x30FA },  // KATAKANA LETTER WO --> KATAKANA LETTER VO
    { 0x30FD, 0x30FE }   // KATAKANA ITERATION MARK --> KATAKANA VOICED ITERATION MARK
};


OUString SAL_CALL
ignoreIterationMark_ja_JP::folding( const OUString& inStr, sal_Int32 startPos, sal_Int32 nCount, Sequence< sal_Int32 >& offset )
  throw(RuntimeException)
{
    oneToOneMapping aTable(ignoreIterationMark_ja_JP_mappingTable, sizeof(ignoreIterationMark_ja_JP_mappingTable));

    // Create a string buffer which can hold nCount + 1 characters.
    // The reference count is 1 now.
    rtl_uString * newStr = comphelper::string::rtl_uString_alloc(nCount);
    sal_Unicode * dst = newStr->buffer;
    const sal_Unicode * src = inStr.getStr() + startPos;

    sal_Int32 * p = 0;
    sal_Int32 position = 0;
    if (useOffset) {
        // Allocate nCount length to offset argument.
        offset.realloc( nCount );
        p = offset.getArray();
        position = startPos;
    }

    //
    sal_Unicode previousChar = *src ++;
    sal_Unicode currentChar;

    // Conversion
    while (-- nCount > 0) {
        currentChar = *src ++;

        switch ( currentChar ) {
            case 0x30fd: // KATAKANA ITERATION MARK
            case 0x309d: // HIRAGANA ITERATION MARK
            case 0x3005: // IDEOGRAPHIC ITERATION MARK
                currentChar = previousChar;
                break;
            case 0x30fe: // KATAKANA VOICED ITERATION MARK
            case 0x309e: // HIRAGANA VOICED ITERATION MARK
                currentChar = aTable[ previousChar ];
                break;
        }
        if (useOffset)
            *p ++ = position ++;
        *dst ++ = previousChar;
        previousChar = currentChar;
    }

    if (nCount == 0) {
        if (useOffset)
            *p = position;
        *dst ++ = previousChar;
    }

    *dst = (sal_Unicode) 0;

    newStr->length = sal_Int32(dst - newStr->buffer);
    if (useOffset)
        offset.realloc(newStr->length);
    return OUString(newStr, SAL_NO_ACQUIRE); // take ownership

}

} } } }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

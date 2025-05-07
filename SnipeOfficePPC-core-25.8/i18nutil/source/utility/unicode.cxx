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

#include <com/sun/star/i18n/UnicodeType.hpp>
#include <com/sun/star/i18n/KCharacterType.hpp>
#include <com/sun/star/i18n/ScriptType.hpp>
#include <i18nutil/unicode.hxx>
#include "unicode_data.h"

// Workaround for glibc braindamage:
// glibc 2.4's langinfo.h does "#define CURRENCY_SYMBOL __CURRENCY_SYMBOL"
// which (obviously) breaks UnicodeType::CURRENCY_SYMBOL
#undef CURRENCY_SYMBOL

using namespace ::com::sun::star::i18n;

static ScriptTypeList defaultTypeList[] = {
    { UnicodeScript_kBasicLatin,
      UnicodeScript_kBasicLatin,
      UnicodeScript_kBasicLatin },      // 0,
    { UnicodeScript_kLatin1Supplement,
      UnicodeScript_kLatin1Supplement,
      UnicodeScript_kLatin1Supplement },// 1,
    { UnicodeScript_kLatinExtendedA,
      UnicodeScript_kLatinExtendedA,
      UnicodeScript_kLatinExtendedA }, // 2,
    { UnicodeScript_kLatinExtendedB,
      UnicodeScript_kLatinExtendedB,
      UnicodeScript_kLatinExtendedB }, // 3,
    { UnicodeScript_kIPAExtension,
      UnicodeScript_kIPAExtension,
      UnicodeScript_kIPAExtension }, // 4,
    { UnicodeScript_kSpacingModifier,
      UnicodeScript_kSpacingModifier,
      UnicodeScript_kSpacingModifier }, // 5,
    { UnicodeScript_kCombiningDiacritical,
      UnicodeScript_kCombiningDiacritical,
      UnicodeScript_kCombiningDiacritical }, // 6,
    { UnicodeScript_kGreek,
      UnicodeScript_kGreek,
      UnicodeScript_kGreek }, // 7,
    { UnicodeScript_kCyrillic,
      UnicodeScript_kCyrillic,
      UnicodeScript_kCyrillic }, // 8,
    { UnicodeScript_kArmenian,
      UnicodeScript_kArmenian,
      UnicodeScript_kArmenian }, // 9,
    { UnicodeScript_kHebrew,
      UnicodeScript_kHebrew,
      UnicodeScript_kHebrew }, // 10,
    { UnicodeScript_kArabic,
      UnicodeScript_kArabic,
      UnicodeScript_kArabic }, // 11,
    { UnicodeScript_kSyriac,
      UnicodeScript_kSyriac,
      UnicodeScript_kSyriac }, // 12,
    { UnicodeScript_kThaana,
      UnicodeScript_kThaana,
      UnicodeScript_kThaana }, // 13,
    { UnicodeScript_kDevanagari,
      UnicodeScript_kDevanagari,
      UnicodeScript_kDevanagari }, // 14,
    { UnicodeScript_kBengali,
      UnicodeScript_kBengali,
      UnicodeScript_kBengali }, // 15,
    { UnicodeScript_kGurmukhi,
      UnicodeScript_kGurmukhi,
      UnicodeScript_kGurmukhi }, // 16,
    { UnicodeScript_kGujarati,
      UnicodeScript_kGujarati,
      UnicodeScript_kGujarati }, // 17,
    { UnicodeScript_kOriya,
      UnicodeScript_kOriya,
      UnicodeScript_kOriya }, // 18,
    { UnicodeScript_kTamil,
      UnicodeScript_kTamil,
      UnicodeScript_kTamil }, // 19,
    { UnicodeScript_kTelugu,
      UnicodeScript_kTelugu,
      UnicodeScript_kTelugu }, // 20,
    { UnicodeScript_kKannada,
      UnicodeScript_kKannada,
      UnicodeScript_kKannada }, // 21,
    { UnicodeScript_kMalayalam,
      UnicodeScript_kMalayalam,
      UnicodeScript_kMalayalam }, // 22,
    { UnicodeScript_kSinhala,
      UnicodeScript_kSinhala,
      UnicodeScript_kSinhala }, // 23,
    { UnicodeScript_kThai,
      UnicodeScript_kThai,
      UnicodeScript_kThai }, // 24,
    { UnicodeScript_kLao,
      UnicodeScript_kLao,
      UnicodeScript_kLao }, // 25,
    { UnicodeScript_kTibetan,
      UnicodeScript_kTibetan,
      UnicodeScript_kTibetan }, // 26,
    { UnicodeScript_kMyanmar,
      UnicodeScript_kMyanmar,
      UnicodeScript_kMyanmar }, // 27,
    { UnicodeScript_kGeorgian,
      UnicodeScript_kGeorgian,
      UnicodeScript_kGeorgian }, // 28,
    { UnicodeScript_kHangulJamo,
      UnicodeScript_kHangulJamo,
      UnicodeScript_kHangulJamo }, // 29,
    { UnicodeScript_kEthiopic,
      UnicodeScript_kEthiopic,
      UnicodeScript_kEthiopic }, // 30,
    { UnicodeScript_kCherokee,
      UnicodeScript_kCherokee,
      UnicodeScript_kCherokee }, // 31,
    { UnicodeScript_kUnifiedCanadianAboriginalSyllabics,
      UnicodeScript_kUnifiedCanadianAboriginalSyllabics,
      UnicodeScript_kUnifiedCanadianAboriginalSyllabics }, // 32,
    { UnicodeScript_kOgham,
      UnicodeScript_kOgham,
      UnicodeScript_kOgham }, // 33,
    { UnicodeScript_kRunic,
      UnicodeScript_kRunic,
      UnicodeScript_kRunic }, // 34,
    { UnicodeScript_kKhmer,
      UnicodeScript_kKhmer,
      UnicodeScript_kKhmer }, // 35,
    { UnicodeScript_kMongolian,
      UnicodeScript_kMongolian,
      UnicodeScript_kMongolian }, // 36,
    { UnicodeScript_kLatinExtendedAdditional,
      UnicodeScript_kLatinExtendedAdditional,
      UnicodeScript_kLatinExtendedAdditional }, // 37,
    { UnicodeScript_kGreekExtended,
      UnicodeScript_kGreekExtended,
      UnicodeScript_kGreekExtended }, // 38,
    { UnicodeScript_kGeneralPunctuation,
      UnicodeScript_kGeneralPunctuation,
      UnicodeScript_kGeneralPunctuation }, // 39,
    { UnicodeScript_kSuperSubScript,
      UnicodeScript_kSuperSubScript,
      UnicodeScript_kSuperSubScript }, // 40,
    { UnicodeScript_kCurrencySymbolScript,
      UnicodeScript_kCurrencySymbolScript,
      UnicodeScript_kCurrencySymbolScript }, // 41,
    { UnicodeScript_kSymbolCombiningMark,
      UnicodeScript_kSymbolCombiningMark,
      UnicodeScript_kSymbolCombiningMark }, // 42,
    { UnicodeScript_kLetterlikeSymbol,
      UnicodeScript_kLetterlikeSymbol,
      UnicodeScript_kLetterlikeSymbol }, // 43,
    { UnicodeScript_kNumberForm,
      UnicodeScript_kNumberForm,
      UnicodeScript_kNumberForm }, // 44,
    { UnicodeScript_kArrow,
      UnicodeScript_kArrow,
      UnicodeScript_kArrow }, // 45,
    { UnicodeScript_kMathOperator,
      UnicodeScript_kMathOperator,
      UnicodeScript_kMathOperator }, // 46,
    { UnicodeScript_kMiscTechnical,
      UnicodeScript_kMiscTechnical,
      UnicodeScript_kMiscTechnical }, // 47,
    { UnicodeScript_kControlPicture,
      UnicodeScript_kControlPicture,
      UnicodeScript_kControlPicture }, // 48,
    { UnicodeScript_kOpticalCharacter,
      UnicodeScript_kOpticalCharacter,
      UnicodeScript_kOpticalCharacter }, // 49,
    { UnicodeScript_kEnclosedAlphanumeric,
      UnicodeScript_kEnclosedAlphanumeric,
      UnicodeScript_kEnclosedAlphanumeric }, // 50,
    { UnicodeScript_kBoxDrawing,
      UnicodeScript_kBoxDrawing,
      UnicodeScript_kBoxDrawing }, // 51,
    { UnicodeScript_kBlockElement,
      UnicodeScript_kBlockElement,
      UnicodeScript_kBlockElement }, // 52,
    { UnicodeScript_kGeometricShape,
      UnicodeScript_kGeometricShape,
      UnicodeScript_kGeometricShape }, // 53,
    { UnicodeScript_kMiscSymbol,
      UnicodeScript_kMiscSymbol,
      UnicodeScript_kMiscSymbol }, // 54,
    { UnicodeScript_kDingbat,
      UnicodeScript_kDingbat,
      UnicodeScript_kDingbat }, // 55,
    { UnicodeScript_kBraillePatterns,
      UnicodeScript_kBraillePatterns,
      UnicodeScript_kBraillePatterns }, // 56,
    { UnicodeScript_kCJKRadicalsSupplement,
      UnicodeScript_kCJKRadicalsSupplement,
      UnicodeScript_kCJKRadicalsSupplement }, // 57,
    { UnicodeScript_kKangxiRadicals,
      UnicodeScript_kKangxiRadicals,
      UnicodeScript_kKangxiRadicals }, // 58,
    { UnicodeScript_kIdeographicDescriptionCharacters,
      UnicodeScript_kIdeographicDescriptionCharacters,
      UnicodeScript_kIdeographicDescriptionCharacters }, // 59,
    { UnicodeScript_kCJKSymbolPunctuation,
      UnicodeScript_kCJKSymbolPunctuation,
      UnicodeScript_kCJKSymbolPunctuation }, // 60,
    { UnicodeScript_kHiragana,
      UnicodeScript_kHiragana,
      UnicodeScript_kHiragana }, // 61,
    { UnicodeScript_kKatakana,
      UnicodeScript_kKatakana,
      UnicodeScript_kKatakana }, // 62,
    { UnicodeScript_kBopomofo,
      UnicodeScript_kBopomofo,
      UnicodeScript_kBopomofo }, // 63,
    { UnicodeScript_kHangulCompatibilityJamo,
      UnicodeScript_kHangulCompatibilityJamo,
      UnicodeScript_kHangulCompatibilityJamo }, // 64,
    { UnicodeScript_kKanbun,
      UnicodeScript_kKanbun,
      UnicodeScript_kKanbun }, // 65,
    { UnicodeScript_kBopomofoExtended,
      UnicodeScript_kBopomofoExtended,
      UnicodeScript_kBopomofoExtended }, // 66,
    { UnicodeScript_kEnclosedCJKLetterMonth,
      UnicodeScript_kEnclosedCJKLetterMonth,
      UnicodeScript_kEnclosedCJKLetterMonth }, // 67,
    { UnicodeScript_kCJKCompatibility,
      UnicodeScript_kCJKCompatibility,
      UnicodeScript_kCJKCompatibility }, // 68,
    { UnicodeScript_k_CJKUnifiedIdeographsExtensionA,
      UnicodeScript_k_CJKUnifiedIdeographsExtensionA,
      UnicodeScript_k_CJKUnifiedIdeographsExtensionA }, // 69,
    { UnicodeScript_kCJKUnifiedIdeograph,
      UnicodeScript_kCJKUnifiedIdeograph,
      UnicodeScript_kCJKUnifiedIdeograph }, // 70,
    { UnicodeScript_kYiSyllables,
      UnicodeScript_kYiSyllables,
      UnicodeScript_kYiSyllables }, // 71,
    { UnicodeScript_kYiRadicals,
      UnicodeScript_kYiRadicals,
      UnicodeScript_kYiRadicals }, // 72,
    { UnicodeScript_kHangulSyllable,
      UnicodeScript_kHangulSyllable,
      UnicodeScript_kHangulSyllable }, // 73,
    { UnicodeScript_kHighSurrogate,
      UnicodeScript_kHighSurrogate,
      UnicodeScript_kHighSurrogate }, // 74,
    { UnicodeScript_kHighPrivateUseSurrogate,
      UnicodeScript_kHighPrivateUseSurrogate,
      UnicodeScript_kHighPrivateUseSurrogate }, // 75,
    { UnicodeScript_kLowSurrogate,
      UnicodeScript_kLowSurrogate,
      UnicodeScript_kLowSurrogate }, // 76,
    { UnicodeScript_kPrivateUse,
      UnicodeScript_kPrivateUse,
      UnicodeScript_kPrivateUse }, // 77,
    { UnicodeScript_kCJKCompatibilityIdeograph,
      UnicodeScript_kCJKCompatibilityIdeograph,
      UnicodeScript_kCJKCompatibilityIdeograph }, // 78,
    { UnicodeScript_kAlphabeticPresentation,
      UnicodeScript_kAlphabeticPresentation,
      UnicodeScript_kAlphabeticPresentation }, // 79,
    { UnicodeScript_kArabicPresentationA,
      UnicodeScript_kArabicPresentationA,
      UnicodeScript_kArabicPresentationA }, // 80,
    { UnicodeScript_kCombiningHalfMark,
      UnicodeScript_kCombiningHalfMark,
      UnicodeScript_kCombiningHalfMark }, // 81,
    { UnicodeScript_kCJKCompatibilityForm,
      UnicodeScript_kCJKCompatibilityForm,
      UnicodeScript_kCJKCompatibilityForm }, // 82,
    { UnicodeScript_kSmallFormVariant,
      UnicodeScript_kSmallFormVariant,
      UnicodeScript_kSmallFormVariant }, // 83,
    { UnicodeScript_kArabicPresentationB,
      UnicodeScript_kArabicPresentationB,
      UnicodeScript_kArabicPresentationB }, // 84,
    { UnicodeScript_kNoScript,
      UnicodeScript_kNoScript,
      UnicodeScript_kNoScript }, // 85,
    { UnicodeScript_kHalfwidthFullwidthForm,
      UnicodeScript_kHalfwidthFullwidthForm,
      UnicodeScript_kHalfwidthFullwidthForm }, // 86,
    { UnicodeScript_kScriptCount,
      UnicodeScript_kScriptCount,
      UnicodeScript_kNoScript } // 87,
};

sal_Int16 SAL_CALL
unicode::getUnicodeScriptType( const sal_Unicode ch, ScriptTypeList* typeList, sal_Int16 unknownType ) {

    if (!typeList) {
        typeList = defaultTypeList;
        unknownType = UnicodeScript_kNoScript;
    }

    sal_Int16 i = 0, type = typeList[0].to;
    while (type < UnicodeScript_kScriptCount && ch > UnicodeScriptType[type][UnicodeScriptTypeTo]) {
        type = typeList[++i].to;
    }

    return (type < UnicodeScript_kScriptCount &&
            ch >= UnicodeScriptType[typeList[i].from][UnicodeScriptTypeFrom]) ?
            typeList[i].value : unknownType;
}

sal_Unicode SAL_CALL
unicode::getUnicodeScriptStart( UnicodeScript type) {
    return UnicodeScriptType[type][UnicodeScriptTypeFrom];
}

sal_Unicode SAL_CALL
unicode::getUnicodeScriptEnd( UnicodeScript type) {
    return UnicodeScriptType[type][UnicodeScriptTypeTo];
}

sal_Int16 SAL_CALL
unicode::getUnicodeType( const sal_Unicode ch ) {
    static sal_Unicode c = 0x00;
    static sal_Int16 r = 0x00;

    if (ch == c) return r;
    else c = ch;

    sal_Int16 address = UnicodeTypeIndex[ch >> 8];
    return r = (sal_Int16)((address < UnicodeTypeNumberBlock) ? UnicodeTypeBlockValue[address] :
        UnicodeTypeValue[((address - UnicodeTypeNumberBlock) << 8) + (ch & 0xff)]);
}

sal_uInt8 SAL_CALL
unicode::getUnicodeDirection( const sal_Unicode ch ) {
    static sal_Unicode c = 0x00;
    static sal_uInt8 r = 0x00;

    if (ch == c) return r;
    else c = ch;

    sal_Int16 address = UnicodeDirectionIndex[ch >> 8];
    return r = ((address < UnicodeDirectionNumberBlock) ? UnicodeDirectionBlockValue[address] :
        UnicodeDirectionValue[((address - UnicodeDirectionNumberBlock) << 8) + (ch & 0xff)]);

}

#define bit(name)   (1 << name)

#define UPPERMASK   bit(UnicodeType::UPPERCASE_LETTER)

#define LOWERMASK   bit(UnicodeType::LOWERCASE_LETTER)

#define TITLEMASK   bit(UnicodeType::TITLECASE_LETTER)

#define DIGITMASK   bit(UnicodeType::DECIMAL_DIGIT_NUMBER)|\
            bit(UnicodeType::LETTER_NUMBER)|\
            bit(UnicodeType::OTHER_NUMBER)

#define ALPHAMASK   UPPERMASK|LOWERMASK|TITLEMASK|\
            bit(UnicodeType::MODIFIER_LETTER)|\
            bit(UnicodeType::OTHER_LETTER)

#define BASEMASK    DIGITMASK|ALPHAMASK|\
            bit(UnicodeType::NON_SPACING_MARK)|\
            bit(UnicodeType::ENCLOSING_MARK)|\
            bit(UnicodeType::COMBINING_SPACING_MARK)

#define SPACEMASK   bit(UnicodeType::SPACE_SEPARATOR)|\
            bit(UnicodeType::LINE_SEPARATOR)|\
            bit(UnicodeType::PARAGRAPH_SEPARATOR)

#define PUNCTUATIONMASK bit(UnicodeType::DASH_PUNCTUATION)|\
            bit(UnicodeType::INITIAL_PUNCTUATION)|\
            bit(UnicodeType::FINAL_PUNCTUATION)|\
            bit(UnicodeType::CONNECTOR_PUNCTUATION)|\
            bit(UnicodeType::OTHER_PUNCTUATION)

#define SYMBOLMASK  bit(UnicodeType::MATH_SYMBOL)|\
            bit(UnicodeType::CURRENCY_SYMBOL)|\
            bit(UnicodeType::MODIFIER_SYMBOL)|\
            bit(UnicodeType::OTHER_SYMBOL)

#define PRINTMASK   BASEMASK|SPACEMASK|PUNCTUATIONMASK|SYMBOLMASK

#define CONTROLMASK bit(UnicodeType::CONTROL)|\
            bit(UnicodeType::FORMAT)|\
            bit(UnicodeType::LINE_SEPARATOR)|\
            bit(UnicodeType::PARAGRAPH_SEPARATOR)

#define IsType(func, mask)  \
sal_Bool SAL_CALL func( const sal_Unicode ch) {\
    return (bit(getUnicodeType(ch)) & (mask)) != 0;\
}

IsType(unicode::isUpper, UPPERMASK)
IsType(unicode::isLower, LOWERMASK)
IsType(unicode::isControl, CONTROLMASK)
IsType(unicode::isPrint, PRINTMASK)
IsType(unicode::isAlpha, ALPHAMASK)
IsType(unicode::isDigit, DIGITMASK)
IsType(unicode::isAlphaDigit, ALPHAMASK|DIGITMASK)
IsType(unicode::isSpace, SPACEMASK)

#define CONTROLSPACE    bit(0x09)|bit(0x0a)|bit(0x0b)|bit(0x0c)|bit(0x0d)|\
            bit(0x1c)|bit(0x1d)|bit(0x1e)|bit(0x1f)

sal_Bool SAL_CALL unicode::isWhiteSpace( const sal_Unicode ch) {
    return (ch != 0xa0 && isSpace(ch)) || (ch <= 0x1F && (bit(ch) & (CONTROLSPACE)));
}

sal_Int16 SAL_CALL unicode::getScriptClassFromUScriptCode(UScriptCode eScript)
{
    //See unicode/uscript.h
    static sal_Int16 scriptTypes[] =
    {
        ScriptType::WEAK, ScriptType::WEAK, ScriptType::COMPLEX, ScriptType::LATIN, ScriptType::COMPLEX,
        ScriptType::ASIAN, ScriptType::LATIN, ScriptType::LATIN, ScriptType::LATIN, ScriptType::COMPLEX,
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::LATIN, ScriptType::LATIN, ScriptType::LATIN,
    // 15
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::ASIAN, ScriptType::ASIAN, ScriptType::COMPLEX,
        ScriptType::ASIAN, ScriptType::COMPLEX, ScriptType::ASIAN, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::LATIN, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::LATIN,
    // 30
        ScriptType::LATIN, ScriptType::COMPLEX, ScriptType::LATIN, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::LATIN, ScriptType::ASIAN, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
    // 45
        ScriptType::COMPLEX, ScriptType::LATIN, ScriptType::LATIN, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::LATIN, ScriptType::LATIN, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::LATIN,
        ScriptType::COMPLEX, ScriptType::LATIN, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
    // 60
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::LATIN, ScriptType::LATIN, ScriptType::COMPLEX,
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::ASIAN, ScriptType::ASIAN,
    // 75
        ScriptType::COMPLEX, ScriptType::LATIN, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::LATIN, ScriptType::LATIN, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
    // 90
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::WEAK, ScriptType::WEAK, ScriptType::COMPLEX,
    // 105
        ScriptType::ASIAN, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::ASIAN,
    // 120
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::WEAK, ScriptType::WEAK,
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
    // 135
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX, ScriptType::COMPLEX,
        ScriptType::COMPLEX,
        ScriptType::WEAK
    };

    sal_Int16 nRet;
    if (eScript < USCRIPT_COMMON)
        nRet = ScriptType::WEAK;
    else if (static_cast<size_t>(eScript) >= SAL_N_ELEMENTS(scriptTypes))
        nRet = ScriptType::COMPLEX;         // anything new is going to be pretty wild
    else
        nRet = scriptTypes[eScript];
    return nRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

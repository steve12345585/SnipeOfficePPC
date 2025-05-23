/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the Original Code is
 *       [ Caolan McNamara <caolanm@redhat.com> ]
 * Portions created by the Initial Developer are Copyright (C) 2010 the
 * Initial Developer. All Rights Reserved.
 *
 * Contributor(s): Caolan McNamara <caolanm@redhat.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */
#include <svtools/sampletext.hxx>
#include <vcl/font.hxx>
#include <vcl/outdev.hxx>
#include <vcl/virdev.hxx>
#include <vcl/metric.hxx>
#include <i18nutil/unicode.hxx>

bool isOpenSymbolFont(const Font &rFont)
{
    return rFont.GetName().EqualsIgnoreCaseAscii("starsymbol") ||
           rFont.GetName().EqualsIgnoreCaseAscii("opensymbol");
}

bool isSymbolFont(const Font &rFont)
{
    return (rFont.GetCharSet() == RTL_TEXTENCODING_SYMBOL) ||
            rFont.GetName().EqualsIgnoreCaseAscii("cmsy10") ||
            rFont.GetName().EqualsIgnoreCaseAscii("cmex10") ||
            rFont.GetName().EqualsIgnoreCaseAscii("feta26") ||
            rFont.GetName().EqualsIgnoreCaseAscii("jsMath-cmsy10") ||
            rFont.GetName().EqualsIgnoreCaseAscii("jsMath-cmex10") ||
            rFont.GetName().EqualsIgnoreCaseAscii("msam10") ||
            rFont.GetName().EqualsIgnoreCaseAscii("msbm10") ||
            rFont.GetName().EqualsIgnoreCaseAscii("wasy10") ||
            rFont.GetName().EqualsIgnoreCaseAscii("Denemo") ||
            rFont.GetName().EqualsIgnoreCaseAscii("GlyphBasic1") ||
            rFont.GetName().EqualsIgnoreCaseAscii("GlyphBasic2") ||
            rFont.GetName().EqualsIgnoreCaseAscii("GlyphBasic3") ||
            rFont.GetName().EqualsIgnoreCaseAscii("GlyphBasic4") ||
            rFont.GetName().EqualsIgnoreCaseAscii("Letters Laughing") ||
            rFont.GetName().EqualsIgnoreCaseAscii("MusiQwik") ||
            rFont.GetName().EqualsIgnoreCaseAscii("MusiSync") ||
            isOpenSymbolFont(rFont);
}

bool canRenderNameOfSelectedFont(OutputDevice &rDevice)
{
    const Font &rFont = rDevice.GetFont();
    return !isSymbolFont(rFont) && (STRING_LEN == rDevice.HasGlyphs(rFont, rFont.GetName()));
}

rtl::OUString makeShortRepresentativeSymbolTextForSelectedFont(OutputDevice &rDevice)
{
    const bool bOpenSymbol = isOpenSymbolFont(rDevice.GetFont());

    if (!bOpenSymbol)
    {
        FontCharMap aFontCharMap;
        bool bHasCharMap = rDevice.GetFontCharMap(aFontCharMap);
        if( bHasCharMap )
        {
            // use some sample characters available in the font
            sal_Unicode aText[8];

            // start just above the PUA used by most symbol fonts
            sal_uInt32 cNewChar = 0xFF00;
#ifdef QUARTZ
            // on MacOSX there are too many non-presentable symbols above the codepoint 0x0192
            if( !bOpenSymbol )
                cNewChar = 0x0192;
#endif

            const int nMaxCount = sizeof(aText)/sizeof(*aText) - 1;
            int nSkip = aFontCharMap.GetCharCount() / nMaxCount;
            if( nSkip > 10 )
                nSkip = 10;
            else if( nSkip <= 0 )
                nSkip = 1;
            for( int i = 0; i < nMaxCount; ++i )
            {
                sal_uInt32 cOldChar = cNewChar;
                for( int j = nSkip; --j >= 0; )
                    cNewChar = aFontCharMap.GetPrevChar( cNewChar );
                if( cOldChar == cNewChar )
                    break;
                aText[ i ] = static_cast<sal_Unicode>(cNewChar); // TODO: support UCS4 samples
                aText[ i+1 ] = 0;
            }

            return rtl::OUString(aText);
        }
    }

    static sal_Unicode aImplSymbolFontText[] = {
        0xF021,0xF032,0xF043,0xF054,0xF065,0xF076,0xF0B7,0xF0C8,0};
    static sal_Unicode aImplStarSymbolText[] = {
        0x2706,0x2704,0x270D,0xE033,0x2211,0x2288,0};
    const sal_Unicode* pText = bOpenSymbol ? aImplStarSymbolText : aImplSymbolFontText;
    rtl::OUString sSampleText(pText);
    bool bHasSampleTextGlyphs = (STRING_LEN == rDevice.HasGlyphs(rDevice.GetFont(), sSampleText));
    return bHasSampleTextGlyphs ? sSampleText : rtl::OUString();
}

//These ones are typically for use in the font dropdown box beside the
//fontname, so say things roughly like "Script/Alphabet/Name-Of-Major-Language"
//
//Here we don't always know the language of course, only the script that can be
//written with the font. Often that's one single language written in that
//script, or a handful of related languages where the name of the script is the
//same between languages, or the name in the major language is known by most
//readers of the minor languages, e.g. Yiddish is written with the HEBREW
//script as well, the vast majority of Yiddish readers will be able to read
//Hebrew as well.
rtl::OUString makeShortRepresentativeTextForScript(UScriptCode eScript)
{
    rtl::OUString sSampleText;
    switch (eScript)
    {
        case USCRIPT_GREEK:
        {
            const sal_Unicode aGrek[] = {
                0x0391, 0x03BB, 0x03C6, 0x03AC, 0x03B2, 0x03B7, 0x03C4, 0x03BF
            };
            sSampleText = rtl::OUString(aGrek, SAL_N_ELEMENTS(aGrek));
            break;
        }
        case USCRIPT_HEBREW:
        {
            const sal_Unicode aHebr[] = {
                0x05D0, 0x05B8, 0x05DC, 0x05B6, 0x05E3, 0x05BE, 0x05D1, 0x05B5,
                0x05BC, 0x05D9, 0x05EA, 0x0020, 0x05E2, 0x05B4, 0x05D1, 0x05B0,
                0x05E8, 0x05B4, 0x05D9
            };
            sSampleText = rtl::OUString(aHebr, SAL_N_ELEMENTS(aHebr));
            break;
        }
        case USCRIPT_ARABIC:
        {
            const sal_Unicode aArab[] = {
                0x0623, 0x0628, 0x062C, 0x062F, 0x064A, 0x0629, 0x0020, 0x0639,
                0x0631, 0x0628, 0x064A, 0x0629
            };
            sSampleText = rtl::OUString(aArab, SAL_N_ELEMENTS(aArab));
            break;
        }
        case USCRIPT_DEVANAGARI:
        {
            const sal_Unicode aDeva[] = {
                0x0926, 0x0947, 0x0935, 0x0928, 0x093E, 0x0917, 0x0930, 0x0940
            };
            sSampleText = rtl::OUString(aDeva, SAL_N_ELEMENTS(aDeva));
            break;
        }
        case USCRIPT_BENGALI:
        {
            const sal_Unicode aBeng[] = {
                0x09AC, 0x09BE, 0x0982, 0x09B2, 0x09BE, 0x0020, 0x09B2, 0x09BF,
                0x09AA, 0x09BF
            };
            sSampleText = rtl::OUString(aBeng, SAL_N_ELEMENTS(aBeng));
            break;
        }
        case USCRIPT_GURMUKHI:
        {
            const sal_Unicode aGuru[] = {
                0x0A17, 0x0A41, 0x0A30, 0x0A2E, 0x0A41, 0x0A16, 0x0A40
            };
            sSampleText = rtl::OUString(aGuru, SAL_N_ELEMENTS(aGuru));
            break;
        }
        case USCRIPT_GUJARATI:
        {
            const sal_Unicode aGujr[] = {
                0x0A97, 0x0AC1, 0x0A9C, 0x0AB0, 0x0ABE, 0x0AA4, 0x0aC0, 0x0020,
                0x0AB2, 0x0ABF, 0x0AAA, 0x0ABF
            };
            sSampleText = rtl::OUString(aGujr, SAL_N_ELEMENTS(aGujr));
            break;
        }
        case USCRIPT_ORIYA:
        {
            const sal_Unicode aOrya[] = {
                0x0B09, 0x0B24, 0x0B4D, 0x0B15, 0x0B33, 0x0020, 0x0B32, 0x0B3F,
                0x0B2A, 0x0B3F
            };
            sSampleText = rtl::OUString(aOrya, SAL_N_ELEMENTS(aOrya));
            break;
        }
        case USCRIPT_TAMIL:
        {
            const sal_Unicode aTaml[] = {
                0x0B85, 0x0BB0, 0x0BBF, 0x0B9A, 0x0BCD, 0x0B9A, 0x0BC1, 0x0BB5,
                0x0B9F, 0x0BBF
            };
            sSampleText = rtl::OUString(aTaml, SAL_N_ELEMENTS(aTaml));
            break;
        }
        case USCRIPT_TELUGU:
        {
            const sal_Unicode aTelu[] = {
                0x0C24, 0x0C46, 0x0C32, 0x0C41, 0x0C17, 0x0C41
            };
            sSampleText = rtl::OUString(aTelu, SAL_N_ELEMENTS(aTelu));
            break;
        }
        case USCRIPT_KANNADA:
        {
            const sal_Unicode aKnda[] = {
                0x0C95, 0x0CA8, 0x0CCD, 0x0CA8, 0x0CA1, 0x0020, 0x0CB2, 0x0CBF,
                0x0CAA, 0x0CBF
            };
            sSampleText = rtl::OUString(aKnda, SAL_N_ELEMENTS(aKnda));
            break;
        }
        case USCRIPT_MALAYALAM:
        {
            const sal_Unicode aMlym[] = {
                0x0D2E, 0x0D32, 0x0D2F, 0x0D3E, 0x0D33, 0x0D32, 0x0D3F, 0x0D2A,
                0x0D3F
            };
            sSampleText = rtl::OUString(aMlym, SAL_N_ELEMENTS(aMlym));
            break;
        }
        case USCRIPT_THAI:
        {
            const sal_Unicode aThai[] = {
                0x0E2D, 0x0E31, 0x0E01, 0x0E29, 0x0E23, 0x0E44, 0x0E17, 0x0E22
            };
            sSampleText = rtl::OUString(aThai, SAL_N_ELEMENTS(aThai));
            break;
        }
        case USCRIPT_LAO:
        {
            const sal_Unicode aLao[] = {
                0x0EAD, 0x0EB1, 0x0E81, 0x0EAA, 0x0EAD, 0x0E99, 0x0EA5, 0x0EB2,
                0x0EA7
            };
            sSampleText = rtl::OUString(aLao, SAL_N_ELEMENTS(aLao));
            break;
        }
        case USCRIPT_GEORGIAN:
        {
            const sal_Unicode aGeorgian[] = {
                0x10D3, 0x10D0, 0x10DB, 0x10EC, 0x10D4, 0x10E0, 0x10DA, 0x10DD,
                0x10D1, 0x10D0
            };
            sSampleText = rtl::OUString(aGeorgian, SAL_N_ELEMENTS(aGeorgian));
            break;
        }
        case USCRIPT_HANGUL:
        case USCRIPT_KOREAN:
        {
            const sal_Unicode aHang[] = {
                0xD55C, 0xAE00
            };
            sSampleText = rtl::OUString(aHang, SAL_N_ELEMENTS(aHang));
            break;
        }
        case USCRIPT_TIBETAN:
        {
            const sal_Unicode aTibt[] = {
                0x0F51, 0x0F56, 0x0F74, 0x0F0B, 0x0F45, 0x0F53, 0x0F0B
            };
            sSampleText = rtl::OUString(aTibt, SAL_N_ELEMENTS(aTibt));
            break;
        }
        case USCRIPT_SYRIAC:
        {
            const sal_Unicode aSyri[] = {
                0x0723, 0x071B, 0x072A, 0x0722, 0x0713, 0x0720, 0x0710
            };
            sSampleText = rtl::OUString(aSyri, SAL_N_ELEMENTS(aSyri));
            break;
        }
        case USCRIPT_THAANA:
        {
            const sal_Unicode aThaa[] = {
                0x078C, 0x07A7, 0x0782, 0x07A6
            };
            sSampleText = rtl::OUString(aThaa, SAL_N_ELEMENTS(aThaa));
            break;
        }
        case USCRIPT_SINHALA:
        {
            const sal_Unicode aSinh[] = {
                0x0DC1, 0x0DD4, 0x0DAF, 0x0DCA, 0x0DB0, 0x0020, 0x0DC3, 0x0DD2,
                0x0D82, 0x0DC4, 0x0DBD
            };
            sSampleText = rtl::OUString(aSinh, SAL_N_ELEMENTS(aSinh));
            break;
        }
        case USCRIPT_MYANMAR:
        {
            const sal_Unicode aMymr[] = {
                0x1019, 0x103C, 0x1014, 0x103A, 0x1019, 0x102C, 0x1021, 0x1000,
                0x1039, 0x1001, 0x101B, 0x102C
            };
            sSampleText = rtl::OUString(aMymr, SAL_N_ELEMENTS(aMymr));
            break;
        }
        case USCRIPT_ETHIOPIC:
        {
            const sal_Unicode aEthi[] = {
                0x130D, 0x12D5, 0x12DD
            };
            sSampleText = rtl::OUString(aEthi, SAL_N_ELEMENTS(aEthi));
            break;
        }
        case USCRIPT_CHEROKEE:
        {
            const sal_Unicode aCher[] = {
                0x13D7, 0x13AA, 0x13EA, 0x13B6, 0x13D9, 0x13D7
            };
            sSampleText = rtl::OUString(aCher, SAL_N_ELEMENTS(aCher));
            break;
        }
        case USCRIPT_KHMER:
        {
            const sal_Unicode aKhmr[] = {
                0x17A2, 0x1780, 0x17D2, 0x1781, 0x179A, 0x1780, 0x17D2, 0x179A,
                0x1798, 0x1781, 0x17C1, 0x1798, 0x179A, 0x1797, 0x17B6, 0x179F,
                0x17B6
            };
            sSampleText = rtl::OUString(aKhmr, SAL_N_ELEMENTS(aKhmr));
            break;
        }
        case USCRIPT_MONGOLIAN:
        {
            const sal_Unicode aMongolian[] = {
                0x182A, 0x1822, 0x1834, 0x1822, 0x182D, 0x180C
            };
            sSampleText = rtl::OUString(aMongolian, SAL_N_ELEMENTS(aMongolian));
            break;
        }
        case USCRIPT_TAGALOG:
        {
            const sal_Unicode aTagalog[] = {
                0x170A, 0x170A, 0x170C, 0x1712
            };
            sSampleText = rtl::OUString(aTagalog, SAL_N_ELEMENTS(aTagalog));
            break;
        }
        case USCRIPT_NEW_TAI_LUE:
        {
            const sal_Unicode aTalu[] = {
                0x1991, 0x19BA, 0x199F, 0x19B9, 0x19C9
            };
            sSampleText = rtl::OUString(aTalu, SAL_N_ELEMENTS(aTalu));
            break;
        }
        case USCRIPT_TRADITIONAL_HAN:
        {
            const sal_Unicode aHant[] = {
                0x7E41
            };
            sSampleText = rtl::OUString(aHant, SAL_N_ELEMENTS(aHant));
            break;
        }
        case USCRIPT_SIMPLIFIED_HAN:
        {
            const sal_Unicode aHans[] = {
                0x7B80
            };
            sSampleText = rtl::OUString(aHans, SAL_N_ELEMENTS(aHans));
            break;
        }
        case USCRIPT_HAN:
        {
            const sal_Unicode aSimplifiedAndTraditionalChinese[] = {
                0x7B80, 0x7E41
            };
            sSampleText = rtl::OUString(aSimplifiedAndTraditionalChinese,
                SAL_N_ELEMENTS(aSimplifiedAndTraditionalChinese));
            break;
        }
        case USCRIPT_JAPANESE:
        {
            const sal_Unicode aJpan[] = {
                0x65E5, 0x672C, 0x8A9E
            };
            sSampleText = rtl::OUString(aJpan, SAL_N_ELEMENTS(aJpan));
            break;
        }
        case USCRIPT_YI:
        {
            const sal_Unicode aYiii[] = {
                0xA188,  0xA320, 0xA071, 0xA0B7
            };
            sSampleText = rtl::OUString(aYiii, SAL_N_ELEMENTS(aYiii));
            break;
        }
        case USCRIPT_PHAGS_PA:
        {
            const sal_Unicode aPhag[] = {
                0xA84F, 0xA861, 0xA843, 0x0020, 0xA863, 0xA861, 0xA859, 0x0020,
                0xA850, 0xA85C, 0xA85E
            };
            sSampleText = rtl::OUString(aPhag, SAL_N_ELEMENTS(aPhag));
            break;
        }
        case USCRIPT_TAI_LE:
        {
            const sal_Unicode aTale[] = {
                0x1956, 0x196D, 0x1970, 0x1956, 0x196C, 0x1973, 0x1951, 0x1968,
                0x1952, 0x1970
            };
            sSampleText = rtl::OUString(aTale, SAL_N_ELEMENTS(aTale));
            break;
        }
        case USCRIPT_LATIN:
            sSampleText = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Lorem ipsum"));
            break;
        default:
            break;
    }
    return sSampleText;
}

rtl::OUString makeRepresentativeTextForScript(UScriptCode eScript)
{
    rtl::OUString sSampleText;
    switch (eScript)
    {
        case USCRIPT_TRADITIONAL_HAN:
        case USCRIPT_SIMPLIFIED_HAN:
        case USCRIPT_HAN:
        {
            //Three Character Classic
            const sal_Unicode aZh[] = {
                0x4EBA, 0x4E4B, 0x521D, 0x0020, 0x6027, 0x672C, 0x5584
            };
            sSampleText = rtl::OUString(aZh, SAL_N_ELEMENTS(aZh));
            break;
        }
        case USCRIPT_JAPANESE:
        {
            //'Beautiful Japanese'
            const sal_Unicode aJa[] = {
                0x7F8E, 0x3057, 0x3044, 0x65E5, 0x672C, 0x8A9E
            };
            sSampleText = rtl::OUString(aJa, SAL_N_ELEMENTS(aJa));
            break;
        }
        case USCRIPT_KOREAN:
        case USCRIPT_HANGUL:
        {
            //The essential condition for...
            const sal_Unicode aKo[] = {
                0xD0A4, 0xC2A4, 0xC758, 0x0020, 0xACE0, 0xC720, 0xC870,
                0xAC74, 0xC740
            };
            sSampleText = rtl::OUString(aKo, SAL_N_ELEMENTS(aKo));
            break;
        }
        default:
            break;
    }

    if (sSampleText.isEmpty())
        sSampleText = makeShortRepresentativeTextForScript(eScript);
    return sSampleText;
}

rtl::OUString makeShortMinimalTextForScript(UScriptCode eScript)
{
    rtl::OUString sSampleText;
    switch (eScript)
    {
        case USCRIPT_GREEK:
        {
            const sal_Unicode aGrek[] = {
                0x0391, 0x0392
            };
            sSampleText = rtl::OUString(aGrek, SAL_N_ELEMENTS(aGrek));
            break;
        }
        case USCRIPT_HEBREW:
        {
            const sal_Unicode aHebr[] = {
                0x05D0, 0x05D1
            };
            sSampleText = rtl::OUString(aHebr, SAL_N_ELEMENTS(aHebr));
            break;
        }
        default:
            break;
    }
    return sSampleText;
}

rtl::OUString makeMinimalTextForScript(UScriptCode eScript)
{
    return makeShortMinimalTextForScript(eScript);
}

//These ones are typically for use in the font preview window in format
//character
//
//There we generally know the language. Though its possible for the language to
//be "none".
//
//Currently we fall back to makeShortRepresentativeTextForScript when we don't
//have suitable strings
rtl::OUString makeRepresentativeTextForLanguage(LanguageType eLang)
{
    rtl::OUString sRet;
    switch( eLang & LANGUAGE_MASK_PRIMARY )
    {
        case LANGUAGE_CHINESE & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_HAN);
            break;
        case LANGUAGE_GREEK & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_GREEK);
            break;
        case LANGUAGE_HEBREW & LANGUAGE_MASK_PRIMARY:
        case LANGUAGE_YIDDISH & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_HEBREW);
            break;
        case LANGUAGE_ARABIC_SAUDI_ARABIA & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_ARABIC);
            break;
        case LANGUAGE_HINDI & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_DEVANAGARI);
            break;
        case LANGUAGE_ASSAMESE & LANGUAGE_MASK_PRIMARY:
        {
            const sal_Unicode aAs[] = {
                0x0985, 0x09B8, 0x09AE, 0x09C0, 0x09AF, 0x09BC, 0x09BE,
                0x0020, 0x0986, 0x0996, 0x09F0
            };
            sRet = rtl::OUString(aAs, SAL_N_ELEMENTS(aAs));
            break;
        }
        case LANGUAGE_BENGALI & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_BENGALI);
            break;
        case LANGUAGE_PUNJABI & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_GURMUKHI);
            break;
        case LANGUAGE_GUJARATI & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_GUJARATI);
            break;
        case LANGUAGE_ORIYA & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_ORIYA);
            break;
        case LANGUAGE_TAMIL & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_TAMIL);
            break;
        case LANGUAGE_TELUGU & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_TELUGU);
            break;
        case LANGUAGE_KANNADA & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_KANNADA);
            break;
        case LANGUAGE_MALAYALAM & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_MALAYALAM);
            break;
        case LANGUAGE_THAI & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_THAI);
            break;
        case LANGUAGE_LAO & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_LAO);
            break;
        case LANGUAGE_GEORGIAN & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_GEORGIAN);
            break;
        case LANGUAGE_KOREAN & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_KOREAN);
            break;
        case LANGUAGE_TIBETAN & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_TIBETAN);
            break;
        case LANGUAGE_SYRIAC & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_SYRIAC);
            break;
        case LANGUAGE_SINHALESE_SRI_LANKA & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_SINHALA);
            break;
        case LANGUAGE_BURMESE & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_MYANMAR);
            break;
        case LANGUAGE_AMHARIC_ETHIOPIA & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_ETHIOPIC);
            break;
        case LANGUAGE_CHEROKEE_UNITED_STATES & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_CHEROKEE);
            break;
        case LANGUAGE_KHMER & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_KHMER);
            break;
        case LANGUAGE_MONGOLIAN & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_MONGOLIAN);
            break;
        case LANGUAGE_JAPANESE & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_JAPANESE);
            break;
        case LANGUAGE_YI & LANGUAGE_MASK_PRIMARY:
            sRet = makeRepresentativeTextForScript(USCRIPT_YI);
            break;
        case LANGUAGE_GAELIC_IRELAND & LANGUAGE_MASK_PRIMARY:
        {
            const sal_Unicode aGa[] = {
                'T', 0x00E9, 'a', 'c', 's', ' ', 'S', 'a', 'm', 'p', 'l', 'a', 'c', 'h'
            };
            sRet = rtl::OUString(aGa, SAL_N_ELEMENTS(aGa));
            break;
        }
        default:
            break;
    }

    return sRet;
}

namespace
{
#if OSL_DEBUG_LEVEL > 2
    void lcl_dump_unicode_coverage(const boost::dynamic_bitset<sal_uInt32> &rIn)
    {
        if (rIn.none())
        {
            fprintf(stderr, "<NONE>\n");
            return;
        }
        if (rIn[vcl::UnicodeCoverage::BASIC_LATIN])
            fprintf(stderr, "BASIC_LATIN\n");
        if (rIn[vcl::UnicodeCoverage::LATIN_1_SUPPLEMENT])
            fprintf(stderr, "LATIN_1_SUPPLEMENT\n");
        if (rIn[vcl::UnicodeCoverage::LATIN_EXTENDED_A])
            fprintf(stderr, "LATIN_EXTENDED_A\n");
        if (rIn[vcl::UnicodeCoverage::LATIN_EXTENDED_B])
            fprintf(stderr, "LATIN_EXTENDED_B\n");
        if (rIn[vcl::UnicodeCoverage::IPA_EXTENSIONS])
            fprintf(stderr, "IPA_EXTENSIONS\n");
        if (rIn[vcl::UnicodeCoverage::SPACING_MODIFIER_LETTERS])
            fprintf(stderr, "SPACING_MODIFIER_LETTERS\n");
        if (rIn[vcl::UnicodeCoverage::COMBINING_DIACRITICAL_MARKS])
            fprintf(stderr, "COMBINING_DIACRITICAL_MARKS\n");
        if (rIn[vcl::UnicodeCoverage::GREEK_AND_COPTIC])
            fprintf(stderr, "GREEK_AND_COPTIC\n");
        if (rIn[vcl::UnicodeCoverage::COPTIC])
            fprintf(stderr, "COPTIC\n");
        if (rIn[vcl::UnicodeCoverage::CYRILLIC])
            fprintf(stderr, "CYRILLIC\n");
        if (rIn[vcl::UnicodeCoverage::ARMENIAN])
            fprintf(stderr, "ARMENIAN\n");
        if (rIn[vcl::UnicodeCoverage::HEBREW])
            fprintf(stderr, "HEBREW\n");
        if (rIn[vcl::UnicodeCoverage::VAI])
            fprintf(stderr, "VAI\n");
        if (rIn[vcl::UnicodeCoverage::ARABIC])
            fprintf(stderr, "ARABIC\n");
        if (rIn[vcl::UnicodeCoverage::NKO])
            fprintf(stderr, "NKO\n");
        if (rIn[vcl::UnicodeCoverage::DEVANAGARI])
            fprintf(stderr, "DEVANAGARI\n");
        if (rIn[vcl::UnicodeCoverage::BENGALI])
            fprintf(stderr, "BENGALI\n");
        if (rIn[vcl::UnicodeCoverage::GURMUKHI])
            fprintf(stderr, "GURMUKHI\n");
        if (rIn[vcl::UnicodeCoverage::GUJARATI])
            fprintf(stderr, "GUJARATI\n");
        if (rIn[vcl::UnicodeCoverage::ORIYA])
            fprintf(stderr, "ORIYA\n");
        if (rIn[vcl::UnicodeCoverage::TAMIL])
            fprintf(stderr, "TAMIL\n");
        if (rIn[vcl::UnicodeCoverage::TELUGU])
            fprintf(stderr, "TELUGU\n");
        if (rIn[vcl::UnicodeCoverage::KANNADA])
            fprintf(stderr, "KANNADA\n");
        if (rIn[vcl::UnicodeCoverage::MALAYALAM])
            fprintf(stderr, "MALAYALAM\n");
        if (rIn[vcl::UnicodeCoverage::THAI])
            fprintf(stderr, "THAI\n");
        if (rIn[vcl::UnicodeCoverage::LAO])
            fprintf(stderr, "LAO\n");
        if (rIn[vcl::UnicodeCoverage::GEORGIAN])
            fprintf(stderr, "GEORGIAN\n");
        if (rIn[vcl::UnicodeCoverage::BALINESE])
            fprintf(stderr, "BALINESE\n");
        if (rIn[vcl::UnicodeCoverage::HANGUL_JAMO])
            fprintf(stderr, "HANGUL_JAMO\n");
        if (rIn[vcl::UnicodeCoverage::LATIN_EXTENDED_ADDITIONAL])
            fprintf(stderr, "LATIN_EXTENDED_ADDITIONAL\n");
        if (rIn[vcl::UnicodeCoverage::GREEK_EXTENDED])
            fprintf(stderr, "GREEK_EXTENDED\n");
        if (rIn[vcl::UnicodeCoverage::GENERAL_PUNCTUATION])
            fprintf(stderr, "GENERAL_PUNCTUATION\n");
        if (rIn[vcl::UnicodeCoverage::SUPERSCRIPTS_AND_SUBSCRIPTS])
            fprintf(stderr, "SUPERSCRIPTS_AND_SUBSCRIPTS\n");
        if (rIn[vcl::UnicodeCoverage::CURRENCY_SYMBOLS])
            fprintf(stderr, "CURRENCY_SYMBOLS\n");
        if (rIn[vcl::UnicodeCoverage::COMBINING_DIACRITICAL_MARKS_FOR_SYMBOLS])
            fprintf(stderr, "COMBINING_DIACRITICAL_MARKS_FOR_SYMBOLS\n");
        if (rIn[vcl::UnicodeCoverage::LETTERLIKE_SYMBOLS])
            fprintf(stderr, "LETTERLIKE_SYMBOLS\n");
        if (rIn[vcl::UnicodeCoverage::NUMBER_FORMS])
            fprintf(stderr, "NUMBER_FORMS\n");
        if (rIn[vcl::UnicodeCoverage::ARROWS])
            fprintf(stderr, "ARROWS\n");
        if (rIn[vcl::UnicodeCoverage::MATHEMATICAL_OPERATORS])
            fprintf(stderr, "MATHEMATICAL_OPERATORS\n");
        if (rIn[vcl::UnicodeCoverage::MISCELLANEOUS_TECHNICAL])
            fprintf(stderr, "MISCELLANEOUS_TECHNICAL\n");
        if (rIn[vcl::UnicodeCoverage::CONTROL_PICTURES])
            fprintf(stderr, "CONTROL_PICTURES\n");
        if (rIn[vcl::UnicodeCoverage::OPTICAL_CHARACTER_RECOGNITION])
            fprintf(stderr, "OPTICAL_CHARACTER_RECOGNITION\n");
        if (rIn[vcl::UnicodeCoverage::ENCLOSED_ALPHANUMERICS])
            fprintf(stderr, "ENCLOSED_ALPHANUMERICS\n");
        if (rIn[vcl::UnicodeCoverage::BOX_DRAWING])
            fprintf(stderr, "BOX_DRAWING\n");
        if (rIn[vcl::UnicodeCoverage::BLOCK_ELEMENTS])
            fprintf(stderr, "BLOCK_ELEMENTS\n");
        if (rIn[vcl::UnicodeCoverage::GEOMETRIC_SHAPES])
            fprintf(stderr, "GEOMETRIC_SHAPES\n");
        if (rIn[vcl::UnicodeCoverage::MISCELLANEOUS_SYMBOLS])
            fprintf(stderr, "MISCELLANEOUS_SYMBOLS\n");
        if (rIn[vcl::UnicodeCoverage::DINGBATS])
            fprintf(stderr, "DINGBATS\n");
        if (rIn[vcl::UnicodeCoverage::CJK_SYMBOLS_AND_PUNCTUATION])
            fprintf(stderr, "CJK_SYMBOLS_AND_PUNCTUATION\n");
        if (rIn[vcl::UnicodeCoverage::HIRAGANA])
            fprintf(stderr, "HIRAGANA\n");
        if (rIn[vcl::UnicodeCoverage::KATAKANA])
            fprintf(stderr, "KATAKANA\n");
        if (rIn[vcl::UnicodeCoverage::BOPOMOFO])
            fprintf(stderr, "BOPOMOFO\n");
        if (rIn[vcl::UnicodeCoverage::HANGUL_COMPATIBILITY_JAMO])
            fprintf(stderr, "HANGUL_COMPATIBILITY_JAMO\n");
        if (rIn[vcl::UnicodeCoverage::PHAGS_PA])
            fprintf(stderr, "PHAGS_PA\n");
        if (rIn[vcl::UnicodeCoverage::ENCLOSED_CJK_LETTERS_AND_MONTHS])
            fprintf(stderr, "ENCLOSED_CJK_LETTERS_AND_MONTHS\n");
        if (rIn[vcl::UnicodeCoverage::CJK_COMPATIBILITY])
            fprintf(stderr, "CJK_COMPATIBILITY\n");
        if (rIn[vcl::UnicodeCoverage::HANGUL_SYLLABLES])
            fprintf(stderr, "HANGUL_SYLLABLES\n");
        if (rIn[vcl::UnicodeCoverage::NONPLANE_0])
            fprintf(stderr, "NONPLANE_0\n");
        if (rIn[vcl::UnicodeCoverage::PHOENICIAN])
            fprintf(stderr, "PHOENICIAN\n");
        if (rIn[vcl::UnicodeCoverage::CJK_UNIFIED_IDEOGRAPHS])
            fprintf(stderr, "CJK_UNIFIED_IDEOGRAPHS\n");
        if (rIn[vcl::UnicodeCoverage::PRIVATE_USE_AREA_PLANE_0])
            fprintf(stderr, "PRIVATE_USE_AREA_PLANE_0\n");
        if (rIn[vcl::UnicodeCoverage::CJK_STROKES])
            fprintf(stderr, "CJK_STROKES\n");
        if (rIn[vcl::UnicodeCoverage::ALPHABETIC_PRESENTATION_FORMS])
            fprintf(stderr, "ALPHABETIC_PRESENTATION_FORMS\n");
        if (rIn[vcl::UnicodeCoverage::ARABIC_PRESENTATION_FORMS_A])
            fprintf(stderr, "ARABIC_PRESENTATION_FORMS_A\n");
        if (rIn[vcl::UnicodeCoverage::COMBINING_HALF_MARKS])
            fprintf(stderr, "COMBINING_HALF_MARKS\n");
        if (rIn[vcl::UnicodeCoverage::VERTICAL_FORMS])
            fprintf(stderr, "VERTICAL_FORMS\n");
        if (rIn[vcl::UnicodeCoverage::SMALL_FORM_VARIANTS])
            fprintf(stderr, "SMALL_FORM_VARIANTS\n");
        if (rIn[vcl::UnicodeCoverage::ARABIC_PRESENTATION_FORMS_B])
            fprintf(stderr, "ARABIC_PRESENTATION_FORMS_B\n");
        if (rIn[vcl::UnicodeCoverage::HALFWIDTH_AND_FULLWIDTH_FORMS])
            fprintf(stderr, "HALFWIDTH_AND_FULLWIDTH_FORMS\n");
        if (rIn[vcl::UnicodeCoverage::SPECIALS])
            fprintf(stderr, "SPECIALS\n");
        if (rIn[vcl::UnicodeCoverage::TIBETAN])
            fprintf(stderr, "TIBETAN\n");
        if (rIn[vcl::UnicodeCoverage::SYRIAC])
            fprintf(stderr, "SYRIAC\n");
        if (rIn[vcl::UnicodeCoverage::THAANA])
            fprintf(stderr, "THAANA\n");
        if (rIn[vcl::UnicodeCoverage::SINHALA])
            fprintf(stderr, "SINHALA\n");
        if (rIn[vcl::UnicodeCoverage::MYANMAR])
            fprintf(stderr, "MYANMAR\n");
        if (rIn[vcl::UnicodeCoverage::ETHIOPIC])
            fprintf(stderr, "ETHIOPIC\n");
        if (rIn[vcl::UnicodeCoverage::CHEROKEE])
            fprintf(stderr, "CHEROKEE\n");
        if (rIn[vcl::UnicodeCoverage::UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS])
            fprintf(stderr, "UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS\n");
        if (rIn[vcl::UnicodeCoverage::OGHAM])
            fprintf(stderr, "OGHAM\n");
        if (rIn[vcl::UnicodeCoverage::RUNIC])
            fprintf(stderr, "RUNIC\n");
        if (rIn[vcl::UnicodeCoverage::KHMER])
            fprintf(stderr, "KHMER\n");
        if (rIn[vcl::UnicodeCoverage::MONGOLIAN])
            fprintf(stderr, "MONGOLIAN\n");
        if (rIn[vcl::UnicodeCoverage::BRAILLE_PATTERNS])
            fprintf(stderr, "BRAILLE_PATTERNS\n");
        if (rIn[vcl::UnicodeCoverage::YI_SYLLABLES])
            fprintf(stderr, "YI_SYLLABLES\n");
        if (rIn[vcl::UnicodeCoverage::TAGALOG])
            fprintf(stderr, "TAGALOG\n");
        if (rIn[vcl::UnicodeCoverage::OLD_ITALIC])
            fprintf(stderr, "OLD_ITALIC\n");
        if (rIn[vcl::UnicodeCoverage::GOTHIC])
            fprintf(stderr, "GOTHIC\n");
        if (rIn[vcl::UnicodeCoverage::DESERET])
            fprintf(stderr, "DESERET\n");
        if (rIn[vcl::UnicodeCoverage::BYZANTINE_MUSICAL_SYMBOLS])
            fprintf(stderr, "BYZANTINE_MUSICAL_SYMBOLS\n");
        if (rIn[vcl::UnicodeCoverage::MATHEMATICAL_ALPHANUMERIC_SYMBOLS])
            fprintf(stderr, "MATHEMATICAL_ALPHANUMERIC_SYMBOLS\n");
        if (rIn[vcl::UnicodeCoverage::PRIVATE_USE_PLANE_15])
            fprintf(stderr, "PRIVATE_USE_PLANE_15\n");
        if (rIn[vcl::UnicodeCoverage::VARIATION_SELECTORS])
            fprintf(stderr, "VARIATION_SELECTORS\n");
        if (rIn[vcl::UnicodeCoverage::TAGS])
            fprintf(stderr, "TAGS\n");
        if (rIn[vcl::UnicodeCoverage::LIMBU])
            fprintf(stderr, "LIMBU\n");
        if (rIn[vcl::UnicodeCoverage::TAI_LE])
            fprintf(stderr, "TAI_LE\n");
        if (rIn[vcl::UnicodeCoverage::NEW_TAI_LUE])
            fprintf(stderr, "NEW_TAI_LUE\n");
        if (rIn[vcl::UnicodeCoverage::BUGINESE])
            fprintf(stderr, "BUGINESE\n");
        if (rIn[vcl::UnicodeCoverage::GLAGOLITIC])
            fprintf(stderr, "GLAGOLITIC\n");
        if (rIn[vcl::UnicodeCoverage::TIFINAGH])
            fprintf(stderr, "TIFINAGH\n");
        if (rIn[vcl::UnicodeCoverage::YIJING_HEXAGRAM_SYMBOLS])
            fprintf(stderr, "YIJING_HEXAGRAM_SYMBOLS\n");
        if (rIn[vcl::UnicodeCoverage::SYLOTI_NAGRI])
            fprintf(stderr, "SYLOTI_NAGRI\n");
        if (rIn[vcl::UnicodeCoverage::LINEAR_B_SYLLABARY])
            fprintf(stderr, "LINEAR_B_SYLLABARY\n");
        if (rIn[vcl::UnicodeCoverage::ANCIENT_GREEK_NUMBERS])
            fprintf(stderr, "ANCIENT_GREEK_NUMBERS\n");
        if (rIn[vcl::UnicodeCoverage::UGARITIC])
            fprintf(stderr, "UGARITIC\n");
        if (rIn[vcl::UnicodeCoverage::OLD_PERSIAN])
            fprintf(stderr, "OLD_PERSIAN\n");
        if (rIn[vcl::UnicodeCoverage::SHAVIAN])
            fprintf(stderr, "SHAVIAN\n");
        if (rIn[vcl::UnicodeCoverage::OSMANYA])
            fprintf(stderr, "OSMANYA\n");
        if (rIn[vcl::UnicodeCoverage::CYPRIOT_SYLLABARY])
            fprintf(stderr, "CYPRIOT_SYLLABARY\n");
        if (rIn[vcl::UnicodeCoverage::KHAROSHTHI])
            fprintf(stderr, "KHAROSHTHI\n");
        if (rIn[vcl::UnicodeCoverage::TAI_XUAN_JING_SYMBOLS])
            fprintf(stderr, "TAI_XUAN_JING_SYMBOLS\n");
        if (rIn[vcl::UnicodeCoverage::CUNEIFORM])
            fprintf(stderr, "CUNEIFORM\n");
        if (rIn[vcl::UnicodeCoverage::COUNTING_ROD_NUMERALS])
            fprintf(stderr, "COUNTING_ROD_NUMERALS\n");
        if (rIn[vcl::UnicodeCoverage::SUNDANESE])
            fprintf(stderr, "SUNDANESE\n");
        if (rIn[vcl::UnicodeCoverage::LEPCHA])
            fprintf(stderr, "LEPCHA\n");
        if (rIn[vcl::UnicodeCoverage::OL_CHIKI])
            fprintf(stderr, "OL_CHIKI\n");
        if (rIn[vcl::UnicodeCoverage::SAURASHTRA])
            fprintf(stderr, "SAURASHTRA\n");
        if (rIn[vcl::UnicodeCoverage::KAYAH_LI])
            fprintf(stderr, "KAYAH_LI\n");
        if (rIn[vcl::UnicodeCoverage::REJANG])
            fprintf(stderr, "REJANG\n");
        if (rIn[vcl::UnicodeCoverage::CHAM])
            fprintf(stderr, "CHAM\n");
        if (rIn[vcl::UnicodeCoverage::ANCIENT_SYMBOLS])
            fprintf(stderr, "ANCIENT_SYMBOLS\n");
        if (rIn[vcl::UnicodeCoverage::PHAISTOS_DISC])
            fprintf(stderr, "PHAISTOS_DISC\n");
        if (rIn[vcl::UnicodeCoverage::CARIAN])
            fprintf(stderr, "CARIAN\n");
        if (rIn[vcl::UnicodeCoverage::DOMINO_TILES])
            fprintf(stderr, "DOMINO_TILES\n");
        if (rIn[vcl::UnicodeCoverage::RESERVED1])
            fprintf(stderr, "RESERVED1\n");
        if (rIn[vcl::UnicodeCoverage::RESERVED2])
            fprintf(stderr, "RESERVED2\n");
        if (rIn[vcl::UnicodeCoverage::RESERVED3])
            fprintf(stderr, "RESERVED3\n");
        if (rIn[vcl::UnicodeCoverage::RESERVED4])
            fprintf(stderr, "RESERVED4\n");
        if (rIn[vcl::UnicodeCoverage::RESERVED5])
            fprintf(stderr, "RESERVED5\n");
    }

    void lcl_dump_codepage_coverage(const boost::dynamic_bitset<sal_uInt32> &rIn)
    {
        if (rIn.none())
        {
            fprintf(stderr, "<NONE>\n");
            return;
        }
        if (rIn[vcl::CodePageCoverage::CP1252])
            fprintf(stderr, "CP1252\n");
        if (rIn[vcl::CodePageCoverage::CP1250])
            fprintf(stderr, "CP1250\n");
        if (rIn[vcl::CodePageCoverage::CP1251])
            fprintf(stderr, "CP1251\n");
        if (rIn[vcl::CodePageCoverage::CP1253])
            fprintf(stderr, "CP1253\n");
        if (rIn[vcl::CodePageCoverage::CP1254])
            fprintf(stderr, "CP1254\n");
        if (rIn[vcl::CodePageCoverage::CP1255])
            fprintf(stderr, "CP1255\n");
        if (rIn[vcl::CodePageCoverage::CP1256])
            fprintf(stderr, "CP1256\n");
        if (rIn[vcl::CodePageCoverage::CP1257])
            fprintf(stderr, "CP1257\n");
        if (rIn[vcl::CodePageCoverage::CP1258])
            fprintf(stderr, "CP1258\n");
        if (rIn[vcl::CodePageCoverage::CP874])
            fprintf(stderr, "CP874\n");
        if (rIn[vcl::CodePageCoverage::CP932])
            fprintf(stderr, "CP932\n");
        if (rIn[vcl::CodePageCoverage::CP936])
            fprintf(stderr, "CP936\n");
        if (rIn[vcl::CodePageCoverage::CP949])
            fprintf(stderr, "CP949\n");
        if (rIn[vcl::CodePageCoverage::CP950])
            fprintf(stderr, "CP950\n");
        if (rIn[vcl::CodePageCoverage::CP1361])
            fprintf(stderr, "CP1361\n");
        if (rIn[vcl::CodePageCoverage::CP869])
            fprintf(stderr, "CP869\n");
        if (rIn[vcl::CodePageCoverage::CP866])
            fprintf(stderr, "CP866\n");
        if (rIn[vcl::CodePageCoverage::CP865])
            fprintf(stderr, "CP865\n");
        if (rIn[vcl::CodePageCoverage::CP864])
            fprintf(stderr, "CP864\n");
        if (rIn[vcl::CodePageCoverage::CP863])
            fprintf(stderr, "CP863\n");
        if (rIn[vcl::CodePageCoverage::CP862])
            fprintf(stderr, "CP862\n");
        if (rIn[vcl::CodePageCoverage::CP861])
            fprintf(stderr, "CP861\n");
        if (rIn[vcl::CodePageCoverage::CP860])
            fprintf(stderr, "CP860\n");
        if (rIn[vcl::CodePageCoverage::CP857])
            fprintf(stderr, "CP857\n");
        if (rIn[vcl::CodePageCoverage::CP855])
            fprintf(stderr, "CP855\n");
        if (rIn[vcl::CodePageCoverage::CP852])
            fprintf(stderr, "CP852\n");
        if (rIn[vcl::CodePageCoverage::CP775])
            fprintf(stderr, "CP775\n");
        if (rIn[vcl::CodePageCoverage::CP737])
            fprintf(stderr, "CP737\n");
        if (rIn[vcl::CodePageCoverage::CP780])
            fprintf(stderr, "CP780\n");
        if (rIn[vcl::CodePageCoverage::CP850])
            fprintf(stderr, "CP850\n");
        if (rIn[vcl::CodePageCoverage::CP437])
            fprintf(stderr, "CP437\n");
    }
#endif

    boost::dynamic_bitset<sal_uInt32> getMaskByScriptType(sal_Int16 nScriptType)
    {
        boost::dynamic_bitset<sal_uInt32> aMask(vcl::UnicodeCoverage::MAX_UC_ENUM);
        aMask.set();

        for (size_t i = 0; i < vcl::UnicodeCoverage::MAX_UC_ENUM; ++i)
        {
            using vcl::UnicodeCoverage::UnicodeCoverageEnum;
            UScriptCode eScriptCode = otCoverageToScript(static_cast<UnicodeCoverageEnum>(i));
            if (unicode::getScriptClassFromUScriptCode(eScriptCode) == nScriptType)
                aMask.set(i, false);
        }

        return aMask;
    }

    //false for all bits considered "Latin" by LibreOffice
    boost::dynamic_bitset<sal_uInt32> getLatinMask()
    {
        static boost::dynamic_bitset<sal_uInt32> aMask(getMaskByScriptType(com::sun::star::i18n::ScriptType::LATIN));
        return aMask;
    }

    //false for all bits considered "Asian" by LibreOffice
    boost::dynamic_bitset<sal_uInt32> getCJKMask()
    {
        static boost::dynamic_bitset<sal_uInt32> aMask(getMaskByScriptType(com::sun::star::i18n::ScriptType::ASIAN));
        return aMask;
    }

    //false for all bits considered "Complex" by LibreOffice
    boost::dynamic_bitset<sal_uInt32> getCTLMask()
    {
        static boost::dynamic_bitset<sal_uInt32> aMask(getMaskByScriptType(com::sun::star::i18n::ScriptType::COMPLEX));
        return aMask;
    }

    //false for all bits considered "WEAK" by LibreOffice
    boost::dynamic_bitset<sal_uInt32> getWeakMask()
    {
        static boost::dynamic_bitset<sal_uInt32> aMask(getMaskByScriptType(com::sun::star::i18n::ScriptType::WEAK));
        return aMask;
    }

    //Nearly every font supports some basic Latin
    boost::dynamic_bitset<sal_uInt32> getCommonLatnSubsetMask()
    {
        boost::dynamic_bitset<sal_uInt32> aMask(vcl::UnicodeCoverage::MAX_UC_ENUM);
        aMask.set();
        aMask.set(vcl::UnicodeCoverage::BASIC_LATIN, false);
        aMask.set(vcl::UnicodeCoverage::LATIN_1_SUPPLEMENT, false);
        aMask.set(vcl::UnicodeCoverage::LATIN_EXTENDED_A, false);
        aMask.set(vcl::UnicodeCoverage::LATIN_EXTENDED_B, false);
        aMask.set(vcl::UnicodeCoverage::LATIN_EXTENDED_ADDITIONAL, false);
        return aMask;
    }

    UScriptCode getScript(const vcl::FontCapabilities &rFontCapabilities)
    {
        using vcl::UnicodeCoverage::UnicodeCoverageEnum;

        boost::dynamic_bitset<sal_uInt32> aMasked = rFontCapabilities.maUnicodeRange & getWeakMask();

        if (aMasked.count() == 1)
            return otCoverageToScript(static_cast<UnicodeCoverageEnum>(aMasked.find_first()));

        if (aMasked[vcl::UnicodeCoverage::ARABIC])
        {
            aMasked.set(vcl::UnicodeCoverage::ARABIC_PRESENTATION_FORMS_A, false);
            aMasked.set(vcl::UnicodeCoverage::ARABIC_PRESENTATION_FORMS_B, false);
            aMasked.set(vcl::UnicodeCoverage::NKO, false);
            //Probably strongly tuned for Arabic
            if (aMasked.count() == 1)
                return USCRIPT_ARABIC;
            if (aMasked.count() == 2 && aMasked[vcl::UnicodeCoverage::SYRIAC])
                return USCRIPT_SYRIAC;
        }

        if (aMasked[vcl::UnicodeCoverage::DEVANAGARI])
        {
            aMasked.set(vcl::UnicodeCoverage::DEVANAGARI, false);
            //Probably strongly tuned for a single Indic script
            if (aMasked.count() == 1)
                return otCoverageToScript(static_cast<UnicodeCoverageEnum>(aMasked.find_first()));
        }

        aMasked.set(vcl::UnicodeCoverage::GREEK_EXTENDED, false);
        aMasked.set(vcl::UnicodeCoverage::GREEK_AND_COPTIC, false);
        if (aMasked.count() == 1)
            return otCoverageToScript(static_cast<UnicodeCoverageEnum>(aMasked.find_first()));

        if (aMasked[vcl::UnicodeCoverage::CYRILLIC])
        {
            //Probably strongly tuned for Georgian
            if (aMasked.count() == 2 && aMasked[vcl::UnicodeCoverage::GEORGIAN])
                return USCRIPT_GEORGIAN;
        }

        aMasked &= getCJKMask();

        aMasked.set(vcl::UnicodeCoverage::CYRILLIC, false);
        aMasked.set(vcl::UnicodeCoverage::THAI, false);
        aMasked.set(vcl::UnicodeCoverage::DESERET, false);
        aMasked.set(vcl::UnicodeCoverage::PHAGS_PA, false);

        //So, possibly a CJK font
        if (!aMasked.count() && !rFontCapabilities.maCodePageRange.empty())
        {
            boost::dynamic_bitset<sal_uInt32> aCJKCodePageMask(vcl::CodePageCoverage::MAX_CP_ENUM);
            aCJKCodePageMask.set(vcl::CodePageCoverage::CP932);
            aCJKCodePageMask.set(vcl::CodePageCoverage::CP936);
            aCJKCodePageMask.set(vcl::CodePageCoverage::CP949);
            aCJKCodePageMask.set(vcl::CodePageCoverage::CP950);
            aCJKCodePageMask.set(vcl::CodePageCoverage::CP1361);
            boost::dynamic_bitset<sal_uInt32> aMaskedCodePage =
                rFontCapabilities.maCodePageRange & aCJKCodePageMask;
            //fold Korean
            if (aMaskedCodePage[vcl::CodePageCoverage::CP1361])
            {
                aMaskedCodePage.set(vcl::CodePageCoverage::CP949);
                aMaskedCodePage.set(vcl::CodePageCoverage::CP1361, false);
            }

            if (aMaskedCodePage.count() == 1)
            {
                if (aMaskedCodePage[vcl::CodePageCoverage::CP932])
                    return USCRIPT_JAPANESE;
                if (aMaskedCodePage[vcl::CodePageCoverage::CP949])
                    return USCRIPT_KOREAN;
                if (aMaskedCodePage[vcl::CodePageCoverage::CP936])
                    return USCRIPT_SIMPLIFIED_HAN;
                if (aMaskedCodePage[vcl::CodePageCoverage::CP950])
                    return USCRIPT_TRADITIONAL_HAN;
            }

            if (aMaskedCodePage.count())
                return USCRIPT_HAN;
        }

        return USCRIPT_COMMON;
    }
}

namespace
{
    UScriptCode attemptToDisambiguateHan(UScriptCode eScript, OutputDevice &rDevice)
    {
        //If we're a CJK font, see if we seem to be tuned for C, J or K
        if (eScript == USCRIPT_HAN)
        {
            const Font &rFont = rDevice.GetFont();

            bool bKore = false, bJpan = false, bHant = false, bHans = false;

            const sal_Unicode aKorean[] = { 0x3131 };
            rtl::OUString sKorean(aKorean, SAL_N_ELEMENTS(aKorean));
            if (STRING_LEN == rDevice.HasGlyphs(rFont, sKorean))
                bKore = true;

            const sal_Unicode aJapanese[] = { 0x3007, 0x9F9D };
            rtl::OUString sJapanese(aJapanese, SAL_N_ELEMENTS(aJapanese));
            if (STRING_LEN == rDevice.HasGlyphs(rFont, sJapanese))
                bJpan = true;

            const sal_Unicode aTraditionalChinese[] = { 0x570B };
            rtl::OUString sTraditionalChinese(aTraditionalChinese, SAL_N_ELEMENTS(aTraditionalChinese));
            if (STRING_LEN == rDevice.HasGlyphs(rFont, sTraditionalChinese))
                bHant = true;

            const sal_Unicode aSimplifiedChinese[] = { 0x56FD };
            rtl::OUString sSimplifiedChinese(aSimplifiedChinese, SAL_N_ELEMENTS(aSimplifiedChinese));
            if (STRING_LEN == rDevice.HasGlyphs(rFont, sSimplifiedChinese))
                bHans = true;

            if (bKore && !bJpan && !bHans)
                eScript = USCRIPT_KOREAN;
            else if (bJpan && !bKore && !bHant)
                eScript = USCRIPT_JAPANESE;
            else if (bHant && !bHans && !bKore && !bJpan)
                eScript = USCRIPT_TRADITIONAL_HAN;
            else if (bHans && !bHant && !bKore && !bJpan)
                eScript = USCRIPT_SIMPLIFIED_HAN;
            //otherwise fall-through as USCRIPT_HAN and expect a combind Hant/Hans preview
        }
        return eScript;
    }
}

rtl::OUString makeShortRepresentativeTextForSelectedFont(OutputDevice &rDevice)
{
    vcl::FontCapabilities aFontCapabilities;
    if (!rDevice.GetFontCapabilities(aFontCapabilities))
        return rtl::OUString();

#if OSL_DEBUG_LEVEL > 2
    fprintf(stderr, "font is %s\n",
        rtl::OUStringToOString(rDevice.GetFont().GetName(), RTL_TEXTENCODING_UTF8).getStr());
    lcl_dump_unicode_coverage(aFontCapabilities.maUnicodeRange);
    lcl_dump_codepage_coverage(aFontCapabilities.maCodePageRange);
#endif

    aFontCapabilities.maUnicodeRange &= getCommonLatnSubsetMask();

    //If this font is probably tuned to display a single non-Latin
    //script and the font name is itself in Latin, then show a small
    //chunk of representative text for that script
    UScriptCode eScript = getScript(aFontCapabilities);
    if (!eScript != USCRIPT_COMMON)
        return rtl::OUString();

    eScript = attemptToDisambiguateHan(eScript, rDevice);

    rtl::OUString sSampleText = makeShortRepresentativeTextForScript(eScript);
    bool bHasSampleTextGlyphs = (STRING_LEN == rDevice.HasGlyphs(rDevice.GetFont(), sSampleText));
    return bHasSampleTextGlyphs ? sSampleText : rtl::OUString();
}

UScriptCode otCoverageToScript(vcl::UnicodeCoverage::UnicodeCoverageEnum eOTCoverage)
{
    UScriptCode eRet = USCRIPT_COMMON;
    switch (eOTCoverage)
    {
        case vcl::UnicodeCoverage::BASIC_LATIN:
        case vcl::UnicodeCoverage::LATIN_1_SUPPLEMENT:
        case vcl::UnicodeCoverage::LATIN_EXTENDED_A:
        case vcl::UnicodeCoverage::LATIN_EXTENDED_B:
            eRet = USCRIPT_LATIN;
            break;
        case vcl::UnicodeCoverage::COMBINING_DIACRITICAL_MARKS:
            eRet = USCRIPT_INHERITED;
            break;
        case vcl::UnicodeCoverage::GREEK_AND_COPTIC:
            eRet = USCRIPT_GREEK;
            break;
        case vcl::UnicodeCoverage::COPTIC:
            eRet = USCRIPT_COPTIC;
            break;
        case vcl::UnicodeCoverage::CYRILLIC:
            eRet = USCRIPT_CYRILLIC;
            break;
        case vcl::UnicodeCoverage::ARMENIAN:
            eRet = USCRIPT_ARMENIAN;
            break;
        case vcl::UnicodeCoverage::HEBREW:
            eRet = USCRIPT_HEBREW;
            break;
        case vcl::UnicodeCoverage::VAI:
            eRet = USCRIPT_VAI;
            break;
        case vcl::UnicodeCoverage::ARABIC:
            eRet = USCRIPT_ARABIC;
            break;
        case vcl::UnicodeCoverage::NKO:
            eRet = USCRIPT_NKO;
            break;
        case vcl::UnicodeCoverage::DEVANAGARI:
            eRet = USCRIPT_DEVANAGARI;
            break;
        case vcl::UnicodeCoverage::BENGALI:
            eRet = USCRIPT_BENGALI;
            break;
        case vcl::UnicodeCoverage::GURMUKHI:
            eRet = USCRIPT_GURMUKHI;
            break;
        case vcl::UnicodeCoverage::GUJARATI:
            eRet = USCRIPT_GUJARATI;
            break;
        case vcl::UnicodeCoverage::ORIYA:
            eRet = USCRIPT_ORIYA;
            break;
        case vcl::UnicodeCoverage::TAMIL:
            eRet = USCRIPT_TAMIL;
            break;
        case vcl::UnicodeCoverage::TELUGU:
            eRet = USCRIPT_TELUGU;
            break;
        case vcl::UnicodeCoverage::KANNADA:
            eRet = USCRIPT_KANNADA;
            break;
        case vcl::UnicodeCoverage::MALAYALAM:
            eRet = USCRIPT_MALAYALAM;
            break;
        case vcl::UnicodeCoverage::THAI:
            eRet = USCRIPT_THAI;
            break;
        case vcl::UnicodeCoverage::LAO:
            eRet = USCRIPT_LAO;
            break;
        case vcl::UnicodeCoverage::GEORGIAN:
            eRet = USCRIPT_GEORGIAN;
            break;
        case vcl::UnicodeCoverage::BALINESE:
            eRet = USCRIPT_BALINESE;
            break;
        case vcl::UnicodeCoverage::HANGUL_JAMO:
            eRet = USCRIPT_HANGUL;
            break;
        case vcl::UnicodeCoverage::LATIN_EXTENDED_ADDITIONAL:
            eRet = USCRIPT_LATIN;
            break;
        case vcl::UnicodeCoverage::GREEK_EXTENDED:
            eRet = USCRIPT_GREEK;
            break;
        case vcl::UnicodeCoverage::CURRENCY_SYMBOLS:
            eRet = USCRIPT_SYMBOLS;
            break;
        case vcl::UnicodeCoverage::COMBINING_DIACRITICAL_MARKS_FOR_SYMBOLS:
            eRet = USCRIPT_INHERITED;
            break;
        case vcl::UnicodeCoverage::LETTERLIKE_SYMBOLS:
        case vcl::UnicodeCoverage::NUMBER_FORMS:
        case vcl::UnicodeCoverage::ARROWS:
            eRet = USCRIPT_SYMBOLS;
            break;
        case vcl::UnicodeCoverage::MATHEMATICAL_OPERATORS:
            eRet = USCRIPT_MATHEMATICAL_NOTATION;
            break;
        case vcl::UnicodeCoverage::MISCELLANEOUS_TECHNICAL:
        case vcl::UnicodeCoverage::OPTICAL_CHARACTER_RECOGNITION:
        case vcl::UnicodeCoverage::BOX_DRAWING:
        case vcl::UnicodeCoverage::BLOCK_ELEMENTS:
        case vcl::UnicodeCoverage::GEOMETRIC_SHAPES:
        case vcl::UnicodeCoverage::MISCELLANEOUS_SYMBOLS:
        case vcl::UnicodeCoverage::DINGBATS:
        case vcl::UnicodeCoverage::CJK_SYMBOLS_AND_PUNCTUATION:
            eRet = USCRIPT_SYMBOLS;
            break;
        case vcl::UnicodeCoverage::HIRAGANA:
            eRet = USCRIPT_HIRAGANA;
            break;
        case vcl::UnicodeCoverage::KATAKANA:
            eRet = USCRIPT_KATAKANA;
            break;
        case vcl::UnicodeCoverage::BOPOMOFO:
            eRet = USCRIPT_BOPOMOFO;
            break;
        case vcl::UnicodeCoverage::HANGUL_COMPATIBILITY_JAMO:
            eRet = USCRIPT_HANGUL;
            break;
        case vcl::UnicodeCoverage::PHAGS_PA:
            eRet = USCRIPT_PHAGS_PA;
            break;
        case vcl::UnicodeCoverage::ENCLOSED_CJK_LETTERS_AND_MONTHS:
            eRet = USCRIPT_HANGUL;
            break;
        case vcl::UnicodeCoverage::CJK_COMPATIBILITY:
            eRet = USCRIPT_HAN;
            break;
        case vcl::UnicodeCoverage::HANGUL_SYLLABLES:
            eRet = USCRIPT_HANGUL;
            break;
        case vcl::UnicodeCoverage::PHOENICIAN:
            eRet = USCRIPT_PHOENICIAN;
            break;
        case vcl::UnicodeCoverage::CJK_UNIFIED_IDEOGRAPHS:
        case vcl::UnicodeCoverage::CJK_STROKES:
            eRet = USCRIPT_HAN;
            break;
        case vcl::UnicodeCoverage::ARABIC_PRESENTATION_FORMS_A:
            eRet = USCRIPT_ARABIC;
            break;
        case vcl::UnicodeCoverage::COMBINING_HALF_MARKS:
            eRet = USCRIPT_INHERITED;
            break;
        case vcl::UnicodeCoverage::ARABIC_PRESENTATION_FORMS_B:
            eRet = USCRIPT_ARABIC;
            break;
        case vcl::UnicodeCoverage::TIBETAN:
            eRet = USCRIPT_TIBETAN;
            break;
        case vcl::UnicodeCoverage::SYRIAC:
            eRet = USCRIPT_SYRIAC;
            break;
        case vcl::UnicodeCoverage::THAANA:
            eRet = USCRIPT_THAANA;
            break;
        case vcl::UnicodeCoverage::SINHALA:
            eRet = USCRIPT_SINHALA;
            break;
        case vcl::UnicodeCoverage::MYANMAR:
            eRet = USCRIPT_MYANMAR;
            break;
        case vcl::UnicodeCoverage::ETHIOPIC:
            eRet = USCRIPT_ETHIOPIC;
            break;
        case vcl::UnicodeCoverage::CHEROKEE:
            eRet = USCRIPT_CHEROKEE;
            break;
        case vcl::UnicodeCoverage::UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS:
            eRet = USCRIPT_CANADIAN_ABORIGINAL;
            break;
        case vcl::UnicodeCoverage::OGHAM:
            eRet = USCRIPT_OGHAM;
            break;
        case vcl::UnicodeCoverage::RUNIC:
            eRet = USCRIPT_RUNIC;
            break;
        case vcl::UnicodeCoverage::KHMER:
            eRet = USCRIPT_KHMER;
            break;
        case vcl::UnicodeCoverage::MONGOLIAN:
            eRet = USCRIPT_MONGOLIAN;
            break;
        case vcl::UnicodeCoverage::BRAILLE_PATTERNS:
            eRet = USCRIPT_BRAILLE;
            break;
        case vcl::UnicodeCoverage::YI_SYLLABLES:
            eRet = USCRIPT_YI;
            break;
        case vcl::UnicodeCoverage::TAGALOG:
            eRet = USCRIPT_TAGALOG;
            break;
        case vcl::UnicodeCoverage::OLD_ITALIC:
            eRet = USCRIPT_OLD_ITALIC;
            break;
        case vcl::UnicodeCoverage::GOTHIC:
            eRet = USCRIPT_GOTHIC;
            break;
        case vcl::UnicodeCoverage::DESERET:
            eRet = USCRIPT_DESERET;
            break;
        case vcl::UnicodeCoverage::BYZANTINE_MUSICAL_SYMBOLS:
        case vcl::UnicodeCoverage::MATHEMATICAL_ALPHANUMERIC_SYMBOLS:
        case vcl::UnicodeCoverage::PRIVATE_USE_PLANE_15:
            eRet = USCRIPT_SYMBOLS;
            break;
        case vcl::UnicodeCoverage::VARIATION_SELECTORS:
            eRet = USCRIPT_INHERITED;
            break;
        case vcl::UnicodeCoverage::TAGS:
            eRet = USCRIPT_SYMBOLS;
            break;
        case vcl::UnicodeCoverage::LIMBU:
            eRet = USCRIPT_LIMBU;
            break;
        case vcl::UnicodeCoverage::TAI_LE:
            eRet = USCRIPT_TAI_LE;
            break;
        case vcl::UnicodeCoverage::NEW_TAI_LUE:
            eRet = USCRIPT_NEW_TAI_LUE;
            break;
        case vcl::UnicodeCoverage::BUGINESE:
            eRet = USCRIPT_BUGINESE;
            break;
        case vcl::UnicodeCoverage::GLAGOLITIC:
            eRet = USCRIPT_GLAGOLITIC;
            break;
        case vcl::UnicodeCoverage::TIFINAGH:
            eRet = USCRIPT_TIFINAGH;
            break;
        case vcl::UnicodeCoverage::YIJING_HEXAGRAM_SYMBOLS:
            eRet = USCRIPT_SYMBOLS;
            break;
        case vcl::UnicodeCoverage::SYLOTI_NAGRI:
            eRet = USCRIPT_SYLOTI_NAGRI;
            break;
        case vcl::UnicodeCoverage::LINEAR_B_SYLLABARY:
            eRet = USCRIPT_LINEAR_B;
            break;
        case vcl::UnicodeCoverage::ANCIENT_GREEK_NUMBERS:
            eRet = USCRIPT_GREEK;
            break;
        case vcl::UnicodeCoverage::UGARITIC:
            eRet = USCRIPT_UGARITIC;
            break;
        case vcl::UnicodeCoverage::OLD_PERSIAN:
            eRet = USCRIPT_OLD_PERSIAN;
            break;
        case vcl::UnicodeCoverage::SHAVIAN:
            eRet = USCRIPT_SHAVIAN;
            break;
        case vcl::UnicodeCoverage::OSMANYA:
            eRet = USCRIPT_OSMANYA;
            break;
        case vcl::UnicodeCoverage::CYPRIOT_SYLLABARY:
            eRet = USCRIPT_CYPRIOT;
            break;
        case vcl::UnicodeCoverage::KHAROSHTHI:
            eRet = USCRIPT_KHAROSHTHI;
            break;
        case vcl::UnicodeCoverage::CUNEIFORM:
            eRet = USCRIPT_CUNEIFORM;
            break;
        case vcl::UnicodeCoverage::SUNDANESE:
            eRet = USCRIPT_SUNDANESE;
            break;
        case vcl::UnicodeCoverage::LEPCHA:
            eRet = USCRIPT_LEPCHA;
            break;
        case vcl::UnicodeCoverage::OL_CHIKI:
            eRet = USCRIPT_OL_CHIKI;
            break;
        case vcl::UnicodeCoverage::SAURASHTRA:
            eRet = USCRIPT_SAURASHTRA;
            break;
        case vcl::UnicodeCoverage::KAYAH_LI:
            eRet = USCRIPT_KAYAH_LI;
            break;
        case vcl::UnicodeCoverage::REJANG:
            eRet = USCRIPT_REJANG;
            break;
        case vcl::UnicodeCoverage::CHAM:
            eRet = USCRIPT_CHAM;
            break;
        case vcl::UnicodeCoverage::CARIAN:
            eRet = USCRIPT_CARIAN;
            break;
        case vcl::UnicodeCoverage::DOMINO_TILES:
        case vcl::UnicodeCoverage::TAI_XUAN_JING_SYMBOLS:
        case vcl::UnicodeCoverage::COUNTING_ROD_NUMERALS:
        case vcl::UnicodeCoverage::ANCIENT_SYMBOLS:
        case vcl::UnicodeCoverage::PHAISTOS_DISC:
            eRet = USCRIPT_SYMBOLS;
            break;
        case vcl::UnicodeCoverage::IPA_EXTENSIONS:
        case vcl::UnicodeCoverage::SPECIALS:
        case vcl::UnicodeCoverage::HALFWIDTH_AND_FULLWIDTH_FORMS:
        case vcl::UnicodeCoverage::VERTICAL_FORMS:
        case vcl::UnicodeCoverage::SMALL_FORM_VARIANTS:
        case vcl::UnicodeCoverage::ALPHABETIC_PRESENTATION_FORMS:
        case vcl::UnicodeCoverage::PRIVATE_USE_AREA_PLANE_0:
        case vcl::UnicodeCoverage::NONPLANE_0:
        case vcl::UnicodeCoverage::ENCLOSED_ALPHANUMERICS:
        case vcl::UnicodeCoverage::CONTROL_PICTURES:
        case vcl::UnicodeCoverage::SUPERSCRIPTS_AND_SUBSCRIPTS:
        case vcl::UnicodeCoverage::GENERAL_PUNCTUATION:
        case vcl::UnicodeCoverage::SPACING_MODIFIER_LETTERS:
        case vcl::UnicodeCoverage::RESERVED1:
        case vcl::UnicodeCoverage::RESERVED2:
        case vcl::UnicodeCoverage::RESERVED3:
        case vcl::UnicodeCoverage::RESERVED4:
        case vcl::UnicodeCoverage::RESERVED5:
        case vcl::UnicodeCoverage::MAX_UC_ENUM:
            break;
    }
    return eRet;
}

rtl::OUString makeRepresentativeTextForFont(sal_Int16 nScriptType, const Font &rFont)
{
    rtl::OUString sRet(makeRepresentativeTextForLanguage(rFont.GetLanguage()));

    VirtualDevice aDevice;
    if (sRet.isEmpty() || (STRING_LEN != aDevice.HasGlyphs(rFont, sRet)))
    {
        aDevice.SetFont(rFont);
        vcl::FontCapabilities aFontCapabilities;
        if (aDevice.GetFontCapabilities(aFontCapabilities))
        {
#if OSL_DEBUG_LEVEL > 2
            lcl_dump_unicode_coverage(aFontCapabilities.maUnicodeRange);
#endif

            aFontCapabilities.maUnicodeRange &= getWeakMask();

            if (nScriptType != com::sun::star::i18n::ScriptType::ASIAN)
            {
                aFontCapabilities.maUnicodeRange &= getCJKMask();
                aFontCapabilities.maCodePageRange.clear();
            }
            if (nScriptType != com::sun::star::i18n::ScriptType::LATIN)
                aFontCapabilities.maUnicodeRange &= getLatinMask();
            if (nScriptType != com::sun::star::i18n::ScriptType::COMPLEX)
                aFontCapabilities.maUnicodeRange &= getCTLMask();

#if OSL_DEBUG_LEVEL > 2
            fprintf(stderr, "minimal\n");
            lcl_dump_unicode_coverage(aFontCapabilities.maUnicodeRange);
            lcl_dump_codepage_coverage(aFontCapabilities.maCodePageRange);
#endif

            UScriptCode eScript = getScript(aFontCapabilities);

            if (nScriptType == com::sun::star::i18n::ScriptType::ASIAN)
                eScript = attemptToDisambiguateHan(eScript, aDevice);

            sRet = makeRepresentativeTextForScript(eScript);
        }

        if (sRet.isEmpty())
        {
            if (nScriptType == com::sun::star::i18n::ScriptType::COMPLEX)
            {
                sRet = makeRepresentativeTextForScript(USCRIPT_HEBREW);
                if (STRING_LEN != aDevice.HasGlyphs(rFont, sRet))
                {
                    sRet = makeMinimalTextForScript(USCRIPT_HEBREW);
                    if (STRING_LEN != aDevice.HasGlyphs(rFont, sRet))
                        sRet = makeRepresentativeTextForScript(USCRIPT_ARABIC);
                }
            }
            else if (nScriptType == com::sun::star::i18n::ScriptType::LATIN)
                sRet = makeRepresentativeTextForScript(USCRIPT_LATIN);
        }
    }

    return sRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

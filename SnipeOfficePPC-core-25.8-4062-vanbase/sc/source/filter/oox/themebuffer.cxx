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

#include "themebuffer.hxx"

#include "stylesbuffer.hxx"

namespace oox {
namespace xls {

// ============================================================================

using ::oox::drawingml::ClrScheme;

// ============================================================================

namespace {

/** Specifies default theme fonts for a specific locale. */
struct BuiltinThemeFont
{
    const sal_Char*     mpcLocale;          /// The locale for this font setting.
    const sal_Char*     mpcHeadFont;        /// Default heading font.
    const sal_Char*     mpcBodyFont;        /// Default body font.
};

#define FONT_JA "\357\274\255\357\274\263 \357\274\260\343\202\264\343\202\267\343\203\203\343\202\257"
#define FONT_KO "\353\247\221\354\235\200 \352\263\240\353\224\225"
#define FONT_CS "\345\256\213\344\275\223"
#define FONT_CT "\346\226\260\347\264\260\346\230\216\351\253\224"

static const BuiltinThemeFont spBuiltinThemeFonts[] =
{ //  locale    headings font           body font
    { "*",      "Cambria",              "Calibri"           },  // Default
    { "ar",     "Times New Roman",      "Arial"             },  // Arabic
    { "bn",     "Vrinda",               "Vrinda"            },  // Bengali
    { "div",    "MV Boli",              "MV Boli"           },  // Divehi
    { "fa",     "Times New Roman",      "Arial"             },  // Farsi
    { "gu",     "Shruti",               "Shruti"            },  // Gujarati
    { "he",     "Times New Roman",      "Arial"             },  // Hebrew
    { "hi",     "Mangal",               "Mangal"            },  // Hindi
    { "ja",     FONT_JA,                FONT_JA             },  // Japanese
    { "kn",     "Tunga",                "Tunga"             },  // Kannada
    { "ko",     FONT_KO,                FONT_KO             },  // Korean
    { "kok",    "Mangal",               "Mangal"            },  // Konkani
    { "ml",     "Kartika",              "Kartika"           },  // Malayalam
    { "mr",     "Mangal",               "Mangal"            },  // Marathi
    { "pa",     "Raavi",                "Raavi"             },  // Punjabi
    { "sa",     "Mangal",               "Mangal"            },  // Sanskrit
    { "syr",    "Estrangelo Edessa",    "Estrangelo Edessa" },  // Syriac
    { "ta",     "Latha",                "Latha"             },  // Tamil
    { "te",     "Gautami",              "Gautami"           },  // Telugu
    { "th",     "Tahoma",               "Tahoma"            },  // Thai
    { "ur",     "Times New Roman",      "Arial"             },  // Urdu
    { "vi",     "Times New Roman",      "Arial"             },  // Vietnamese
    { "zh",     FONT_CS,                FONT_CS             },  // Chinese, Simplified
    { "zh-HK",  FONT_CT,                FONT_CT             },  // Chinese, Hong Kong
    { "zh-MO",  FONT_CT,                FONT_CT             },  // Chinese, Macau
    { "zh-TW",  FONT_CT,                FONT_CT             }   // Chinese, Taiwan
};

} // namespace

// ----------------------------------------------------------------------------

ThemeBuffer::ThemeBuffer( const WorkbookHelper& rHelper ) :
    WorkbookHelper( rHelper ),
    mxDefFontModel( new FontModel )
{
    switch( getFilterType() )
    {
        case FILTER_OOXML:
            //! TODO: locale dependent font name
            mxDefFontModel->maName = "Cambria";
            mxDefFontModel->mfHeight = 11.0;
        break;
        case FILTER_BIFF:
            //! TODO: BIFF dependent font name
            mxDefFontModel->maName = "Arial";
            mxDefFontModel->mfHeight = 10.0;
        break;
        case FILTER_UNKNOWN: break;
    }
}

ThemeBuffer::~ThemeBuffer()
{
}

sal_Int32 ThemeBuffer::getColorByToken( sal_Int32 nToken ) const
{
    sal_Int32 nColor = 0;
    return getClrScheme().getColor( nToken, nColor ) ? nColor : API_RGB_TRANSPARENT;
}

// ============================================================================

} // namespace xls
} // namespace oox

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

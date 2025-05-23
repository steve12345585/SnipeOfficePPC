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
#ifndef _UNOTOOLS_FONTDEFS_HXX
#define _UNOTOOLS_FONTDEFS_HXX

#include <unotools/unotoolsdllapi.h>
#include <sal/types.h>
#include <tools/string.hxx>

namespace utl {
  class FontSubstConfiguration;
  struct FontNameAttr;
}

// ----------------
// - SubsFontName -
// ----------------

#define SUBSFONT_ONLYONE    ((sal_uLong)0x00000001)
#define SUBSFONT_MS         ((sal_uLong)0x00000002)
#define SUBSFONT_PS         ((sal_uLong)0x00000004)
#define SUBSFONT_HTML       ((sal_uLong)0x00000008)

UNOTOOLS_DLLPUBLIC String GetSubsFontName( const String& rName, sal_uLong nFlags );

UNOTOOLS_DLLPUBLIC void AddTokenFontName( String& rName, const String& rNewToken );

struct UNOTOOLS_DLLPUBLIC FontNameHash { int operator()(const String&) const; };

// ---------------
// - ConvertChar -
// ---------------

class UNOTOOLS_DLLPUBLIC ConvertChar
{
public:
    const sal_Unicode*  mpCvtTab;
    const char*         mpSubsFontName;
    sal_Unicode         (*mpCvtFunc)( sal_Unicode );
    sal_Unicode         RecodeChar( sal_Unicode c ) const;
    void                RecodeString( String& rStra, xub_StrLen nIndex, xub_StrLen nLen ) const;
    static const ConvertChar* GetRecodeData( const String& rOrgFontName, const String& rMapFontName );
};


// Default-Font
#define DEFAULTFONT_SANS_UNICODE        ((sal_uInt16)1)
#define DEFAULTFONT_SANS                ((sal_uInt16)2)
#define DEFAULTFONT_SERIF               ((sal_uInt16)3)
#define DEFAULTFONT_FIXED               ((sal_uInt16)4)
#define DEFAULTFONT_SYMBOL              ((sal_uInt16)5)
#define DEFAULTFONT_UI_SANS             ((sal_uInt16)1000)
#define DEFAULTFONT_UI_FIXED            ((sal_uInt16)1001)
#define DEFAULTFONT_LATIN_TEXT          ((sal_uInt16)2000)
#define DEFAULTFONT_LATIN_PRESENTATION  ((sal_uInt16)2001)
#define DEFAULTFONT_LATIN_SPREADSHEET   ((sal_uInt16)2002)
#define DEFAULTFONT_LATIN_HEADING       ((sal_uInt16)2003)
#define DEFAULTFONT_LATIN_DISPLAY       ((sal_uInt16)2004)
#define DEFAULTFONT_LATIN_FIXED         ((sal_uInt16)2005)
#define DEFAULTFONT_CJK_TEXT            ((sal_uInt16)3000)
#define DEFAULTFONT_CJK_PRESENTATION    ((sal_uInt16)3001)
#define DEFAULTFONT_CJK_SPREADSHEET     ((sal_uInt16)3002)
#define DEFAULTFONT_CJK_HEADING         ((sal_uInt16)3003)
#define DEFAULTFONT_CJK_DISPLAY         ((sal_uInt16)3004)
#define DEFAULTFONT_CTL_TEXT            ((sal_uInt16)4000)
#define DEFAULTFONT_CTL_PRESENTATION    ((sal_uInt16)4001)
#define DEFAULTFONT_CTL_SPREADSHEET     ((sal_uInt16)4002)
#define DEFAULTFONT_CTL_HEADING         ((sal_uInt16)4003)
#define DEFAULTFONT_CTL_DISPLAY         ((sal_uInt16)4004)

UNOTOOLS_DLLPUBLIC String GetNextFontToken( const String& rTokenStr, xub_StrLen& rIndex );

UNOTOOLS_DLLPUBLIC void GetEnglishSearchFontName( String& rName );

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

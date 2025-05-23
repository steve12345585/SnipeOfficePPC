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

#ifndef _IODETECT_HXX_
#define _IODETECT_HXX_

#include <sfx2/docfilt.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/fcontnr.hxx>
#include <osl/endian.h>
#include <tools/string.hxx>
#include <swddllapi.h>

#define FILTER_RTF      "RTF"       ///< RTF filter
#define sRtfWH          "WH_RTF"
#define FILTER_TEXT     "TEXT"      ///< text filter with default codeset
#define FILTER_BAS      "BAS"       ///< StarBasic (identical to ANSI)
#define FILTER_WW8      "CWW8"      ///< WinWord 97 filter
#define FILTER_TEXT_DLG "TEXT_DLG"  ///< text filter with encoding dialog
#define FILTER_XML      "CXML"      ///< XML filter
#define FILTER_XMLV     "CXMLV"     ///< XML filter
#define FILTER_XMLVW    "CXMLVWEB"  ///< XML filter
#define sHTML           "HTML"
#define sWW1            "WW1"
#define sWW5            "WW6"
#define sWW6            "CWW6"

#define sSWRITER        "swriter"
#define sSWRITERWEB     "swriter/web"

struct SwIoDetect
{
    const sal_Char* pName;
    sal_uInt16 nLen;

    inline SwIoDetect( const sal_Char *pN, sal_uInt16 nL )
        : pName( pN ), nLen( nL )
    {}

    inline int IsFilter( const String& rNm ) const
    {
        return pName && rNm.EqualsAscii( pName, 0, nLen );
    }

    const sal_Char* IsReader( const sal_Char* pHeader, sal_uLong nLen_,
            const String &rFileName, const String& rUserData ) const;
};

enum ReaderWriterEnum {
    READER_WRITER_RTF,
    READER_WRITER_BAS,
    READER_WRITER_WW6,
    READER_WRITER_WW8,
    READER_WRITER_RTF_WH,
    READER_WRITER_HTML,
    READER_WRITER_WW1,
    READER_WRITER_WW5,
    READER_WRITER_XML,
    READER_WRITER_TEXT_DLG,
    READER_WRITER_TEXT,
    MAXFILTER
};

extern SWD_DLLPUBLIC SwIoDetect aFilterDetect[];

/** The following class is a wrapper for basic i/o functions of Writer 3.0.
 Everything is static. All filter names mentioned are Writer-internal
 names, i.e. the names in front of the equality sign in INSTALL.INI, like SWG
 or ASCII.*/

class SwIoSystem
{
public:
    /// find for an internal format name the corresponding filter entry
    SWD_DLLPUBLIC static const SfxFilter*
        GetFilterOfFormat( const String& rFormat,
            const SfxFilterContainer* pCnt = 0 );

    /** Detect for the given file which filter should be used. The filter name
     is returned. If no filter could be found, the name of the ASCII filter
     is returned! */
    SWD_DLLPUBLIC static const SfxFilter*
        GetFileFilter( const String& rFileName,
            const String& rPrefFltName,
            SfxMedium* pMedium = 0 );

    /** Detect whether the given file is in the given format.
     For now, only our own filters are supported! */
    static sal_Bool IsFileFilter(SfxMedium& rMedium, const String& rFmtName);

    static sal_Bool IsValidStgFilter( SotStorage& , const SfxFilter& );
    static sal_Bool IsValidStgFilter( const com::sun::star::uno::Reference < com::sun::star::embed::XStorage >& rStg, const SfxFilter& rFilter);

    SWD_DLLPUBLIC static bool
        IsDetectableText( const sal_Char* pBuf, sal_uLong &rLen,
            CharSet *pCharSet=0, bool *pSwap=0, LineEnd *pLineEnd=0, bool bEncodedFilter = false );

    static const SfxFilter* GetTextFilter( const sal_Char* pBuf, sal_uLong nLen );

    SWD_DLLPUBLIC static const String
        GetSubStorageName( const SfxFilter& rFltr );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

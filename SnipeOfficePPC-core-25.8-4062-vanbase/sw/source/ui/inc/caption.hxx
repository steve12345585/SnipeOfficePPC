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
#ifndef _CAPTION_HXX
#define _CAPTION_HXX

#include <tools/string.hxx>
#include <tools/globname.hxx>
#include <SwCapObjType.hxx>
#include "swdllapi.h"

class SW_DLLPUBLIC InsCaptionOpt
{
private:
    sal_Bool            bUseCaption;
    SwCapObjType    eObjType;
    SvGlobalName    aOleId;
    String          sCategory;
    sal_uInt16          nNumType;
    ::rtl::OUString sNumberSeparator;
    String          sCaption;
    sal_uInt16          nPos;
    sal_uInt16          nLevel;
    String          sSeparator;
    String          sCharacterStyle;

    sal_Bool         bIgnoreSeqOpts;    // is not being saved
    sal_Bool         bCopyAttributes;   //          -""-

public:
    InsCaptionOpt(const SwCapObjType eType = FRAME_CAP, const SvGlobalName* pOleId = 0);
    InsCaptionOpt(const InsCaptionOpt&);
    ~InsCaptionOpt();

    inline sal_Bool&            UseCaption()                    { return bUseCaption; }
    inline sal_Bool             UseCaption() const              { return bUseCaption; }

    inline SwCapObjType     GetObjType() const              { return eObjType; }
    inline void             SetObjType(const SwCapObjType eType) { eObjType = eType; }

    inline const SvGlobalName&  GetOleId() const                { return aOleId; }
    inline void             SetOleId(const SvGlobalName &rId)   { aOleId = rId; }

    inline const String&    GetCategory() const             { return sCategory; }
    inline void             SetCategory(const String& rCat) { sCategory = rCat; }

    inline sal_uInt16           GetNumType() const              { return nNumType; }
    inline void             SetNumType(const sal_uInt16 nNT)    { nNumType = nNT; }

    const ::rtl::OUString&  GetNumSeparator() const { return sNumberSeparator; }
    void                    SetNumSeparator(const ::rtl::OUString& rSet) {sNumberSeparator = rSet;}

    inline const String&    GetCaption() const              { return sCaption; }
    inline void             SetCaption(const String& rCap)  { sCaption = rCap; }

    inline sal_uInt16           GetPos() const                  { return nPos; }
    inline void             SetPos(const sal_uInt16 nP)         { nPos = nP; }

    inline sal_uInt16           GetLevel() const                { return nLevel; }
    inline void             SetLevel(const sal_uInt16 nLvl)     { nLevel = nLvl; }

    inline const String&    GetSeparator() const                { return sSeparator; }
    inline void             SetSeparator(const String& rSep)    { sSeparator = rSep; }

    const String&           GetCharacterStyle() const { return sCharacterStyle; }
    void                    SetCharacterStyle(const String& rStyle)
                                    { sCharacterStyle = rStyle; }

    inline sal_Bool&            IgnoreSeqOpts()                 { return bIgnoreSeqOpts; }
    inline sal_Bool             IgnoreSeqOpts() const           { return bIgnoreSeqOpts; }

    inline sal_Bool&            CopyAttributes()                { return bCopyAttributes; }
    inline sal_Bool             CopyAttributes() const          { return bCopyAttributes; }

    sal_Bool                    operator==( const InsCaptionOpt& rOpt ) const;
    InsCaptionOpt&          operator= ( const InsCaptionOpt& rOpt );
    inline sal_Bool             operator< ( const InsCaptionOpt & rObj ) const
                                                { return aOleId < rObj.aOleId; }
};

#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

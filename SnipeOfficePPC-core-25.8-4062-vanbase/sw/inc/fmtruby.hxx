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
#ifndef _FMTRUBY_HXX
#define _FMTRUBY_HXX


#include <tools/string.hxx>
#include <svl/poolitem.hxx>

class SwTxtRuby;

class SW_DLLPUBLIC SwFmtRuby : public SfxPoolItem
{
    friend class SwTxtRuby;

    String sRubyTxt;                        ///< The ruby txt.
    String sCharFmtName;                    ///< Name of the charformat.
    SwTxtRuby* pTxtAttr;                    ///< The TextAttribute.
    sal_uInt16 nCharFmtId;                  ///< PoolId of the charformat.
    sal_uInt16 nPosition;                   ///< Position of the Ruby-character.
    sal_uInt16 nAdjustment;                 ///< Specific adjustment of the Ruby-ch.

public:
    SwFmtRuby( const String& rRubyTxt );
    SwFmtRuby( const SwFmtRuby& rAttr );
    virtual ~SwFmtRuby();

    SwFmtRuby& operator=( const SwFmtRuby& rAttr );

    // "Pure virtual methods" of SfxPoolItem.
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*    Clone( SfxItemPool* pPool = 0 ) const;

    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper* pIntl = 0 ) const;

    virtual bool QueryValue( com::sun::star::uno::Any& rVal,
                                        sal_uInt8 nMemberId = 0 ) const;
    virtual bool PutValue( const com::sun::star::uno::Any& rVal,
                                        sal_uInt8 nMemberId = 0 );


    const SwTxtRuby* GetTxtRuby() const         { return pTxtAttr; }
    SwTxtRuby* GetTxtRuby()                     { return pTxtAttr; }

    const String& GetText() const               { return sRubyTxt; }
    void SetText( const String& rTxt )          { sRubyTxt = rTxt; }

    const String& GetCharFmtName() const        { return sCharFmtName; }
    void SetCharFmtName( const String& rNm )    { sCharFmtName = rNm; }

    sal_uInt16 GetCharFmtId() const                 { return nCharFmtId; }
    void SetCharFmtId( sal_uInt16 nNew )            { nCharFmtId = nNew; }

    sal_uInt16 GetPosition() const                  { return nPosition; }
    void SetPosition( sal_uInt16 nNew )             { nPosition = nNew; }

    sal_uInt16 GetAdjustment() const                { return nAdjustment; }
    void SetAdjustment( sal_uInt16 nNew )           { nAdjustment = nNew; }
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

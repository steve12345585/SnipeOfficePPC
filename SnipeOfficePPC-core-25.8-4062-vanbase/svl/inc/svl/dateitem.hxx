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
#ifndef _DATETIMEITEM_HXX
#define _DATETIMEITEM_HXX

#include <tools/rtti.hxx>
#include <tools/datetime.hxx>

#include <svl/poolitem.hxx>

class SvStream;

DBG_NAMEEX(SfxDateTimeItem)

// class SfxDateTimeItem -------------------------------------------------

class SfxDateTimeItem : public SfxPoolItem
{
private:
    DateTime                aDateTime;

public:
                            TYPEINFO();

                            SfxDateTimeItem( sal_uInt16 nWhich,
                                             const DateTime& rDT );
                            SfxDateTimeItem( const SfxDateTimeItem& rCpy );

                            ~SfxDateTimeItem() {
                                DBG_DTOR(SfxDateTimeItem, 0); }

    virtual int             operator==( const SfxPoolItem& ) const;
    using SfxPoolItem::Compare;
    virtual int             Compare( const SfxPoolItem &rWith ) const;
    virtual SfxPoolItem*    Create( SvStream&, sal_uInt16 nItemVersion ) const;
    virtual SvStream&       Store( SvStream&, sal_uInt16 nItemVersion ) const;
    virtual SfxPoolItem*    Clone( SfxItemPool* pPool = 0 ) const;

    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    XubString &rText,
                                    const IntlWrapper * pIntlWrapper = 0 )
        const;

    const DateTime&         GetDateTime() const { return aDateTime; }
    void                    SetDateTime( const DateTime& rDT ) {
                                DBG_ASSERT( GetRefCount() == 0,
                                            "SetDateTime() with pooled item" );
                                aDateTime = rDT; }

    virtual bool PutValue  ( const com::sun::star::uno::Any& rVal,
                             sal_uInt8 nMemberId = 0 );
    virtual bool QueryValue( com::sun::star::uno::Any& rVal,
                             sal_uInt8 nMemberId = 0 ) const;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

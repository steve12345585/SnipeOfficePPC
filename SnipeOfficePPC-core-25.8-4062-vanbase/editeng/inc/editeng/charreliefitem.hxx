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
#ifndef _SVX_CHARRELIEFITEM_HXX
#define _SVX_CHARRELIEFITEM_HXX

#include <svl/eitem.hxx>
#include <vcl/fntstyle.hxx>
#include <editeng/editengdllapi.h>

// class SvxCharRotateItem ----------------------------------------------

/* [Description]

    This item defines a character relief and has currently the values
    emboss, relief.
*/

class EDITENG_DLLPUBLIC SvxCharReliefItem : public SfxEnumItem
{
public:
    TYPEINFO();

    SvxCharReliefItem( FontRelief eValue /*= RELIEF_NONE*/,
                       const sal_uInt16 nId );

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
    virtual SfxPoolItem*    Create(SvStream &, sal_uInt16) const;
    virtual SvStream&       Store(SvStream & rStrm, sal_uInt16 nIVer) const;
    virtual sal_uInt16      GetVersion( sal_uInt16 nFileVersion ) const;

    virtual rtl::OUString   GetValueTextByPos( sal_uInt16 nPos ) const;
    virtual sal_uInt16      GetValueCount() const;

    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText,
                                    const IntlWrapper * = 0 ) const;

    virtual bool            QueryValue( com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 ) const;
    virtual bool            PutValue( const com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 );

    inline SvxCharReliefItem& operator=( const SvxCharReliefItem& rItem )
    {
        SetValue( rItem.GetValue() );
        return *this;
    }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

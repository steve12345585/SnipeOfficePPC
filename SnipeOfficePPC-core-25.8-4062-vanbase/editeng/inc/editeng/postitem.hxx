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
#ifndef _SVX_POSTITEM_HXX
#define _SVX_POSTITEM_HXX

#include <vcl/vclenum.hxx>
#include <svl/eitem.hxx>
#include <editeng/editengdllapi.h>

class SvXMLUnitConverter;
namespace rtl
{
    class OUString;
}

// class SvxPostureItem --------------------------------------------------

/*  [Description]

    This item describes the font setting (Italic)
*/

class EDITENG_DLLPUBLIC SvxPostureItem : public SfxEnumItem
{
public:
    TYPEINFO();

    SvxPostureItem( const FontItalic ePost /*= ITALIC_NONE*/,
                    const sal_uInt16 nId  );

    // "pure virtual Methods" from SfxPoolItem + SwEnumItem
    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText, const IntlWrapper * = 0 ) const;

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
    virtual SfxPoolItem*    Create(SvStream &, sal_uInt16) const;
    virtual SvStream&       Store(SvStream &, sal_uInt16 nItemVersion) const;
    virtual rtl::OUString   GetValueTextByPos( sal_uInt16 nPos ) const;
    virtual sal_uInt16      GetValueCount() const;

    virtual bool            QueryValue( com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 ) const;
    virtual bool            PutValue( const com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 );

    virtual int             HasBoolValue() const;
    virtual sal_Bool        GetBoolValue() const;
    virtual void            SetBoolValue( sal_Bool bVal );

    inline SvxPostureItem& operator=(const SvxPostureItem& rPost) {
        SetValue( rPost.GetValue() );
        return *this;
    }

    // enum cast
    FontItalic              GetPosture() const
                                { return (FontItalic)GetValue(); }
    void                    SetPosture( FontItalic eNew )
                                { SetValue( (sal_uInt16)eNew ); }
};

#endif // #ifndef _SVX_POSTITEM_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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
#ifndef _SVX_FMITEMS_HXX
#define _SVX_FMITEMS_HXX


#include <svl/poolitem.hxx>
#include <comphelper/uno3.hxx>

//==================================================================
class FmInterfaceItem : public SfxPoolItem
{
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  xInterface;

public:
    TYPEINFO();

    FmInterfaceItem( const sal_uInt16 nId, const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rxInterface )
        :SfxPoolItem( nId )
        ,xInterface( rxInterface )
    {}

    inline FmInterfaceItem&  operator=( const FmInterfaceItem &rCpy );

    // "pure virtual Methoden" vom SfxPoolItem
    virtual int             operator==( const SfxPoolItem& ) const;

    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
    virtual SfxPoolItem*    Create(SvStream &, sal_uInt16) const;
    virtual SvStream&       Store(SvStream &, sal_uInt16 nItemVersion ) const;

    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >         GetInterface() const { return xInterface; }
};

inline FmInterfaceItem& FmInterfaceItem::operator=( const FmInterfaceItem &rCpy )
{
    xInterface = rCpy.xInterface;

    return *this;
}



#endif // _SVX_FMITEMS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

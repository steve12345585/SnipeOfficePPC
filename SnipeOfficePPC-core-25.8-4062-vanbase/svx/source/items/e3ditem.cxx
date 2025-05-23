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

#include <com/sun/star/drawing/Direction3D.hpp>
#include <tools/stream.hxx>

#include <svx/e3ditem.hxx>

using namespace ::rtl;
using namespace ::com::sun::star;

// STATIC DATA -----------------------------------------------------------

DBG_NAMEEX(SvxB3DVectorItem)
DBG_NAME(SvxB3DVectorItem)

// -----------------------------------------------------------------------

TYPEINIT1_FACTORY(SvxB3DVectorItem, SfxPoolItem, new SvxB3DVectorItem);

// -----------------------------------------------------------------------

SvxB3DVectorItem::SvxB3DVectorItem()
{
    DBG_CTOR(SvxB3DVectorItem, 0);
}

SvxB3DVectorItem::~SvxB3DVectorItem()
{
    DBG_DTOR(SvxB3DVectorItem, 0);
}

// -----------------------------------------------------------------------

SvxB3DVectorItem::SvxB3DVectorItem( sal_uInt16 _nWhich, const basegfx::B3DVector& rVal ) :
    SfxPoolItem( _nWhich ),
    aVal( rVal )
{
    DBG_CTOR(SvxB3DVectorItem, 0);
}

// -----------------------------------------------------------------------

SvxB3DVectorItem::SvxB3DVectorItem( const SvxB3DVectorItem& rItem ) :
    SfxPoolItem( rItem ),
    aVal( rItem.aVal )
{
    DBG_CTOR(SvxB3DVectorItem, 0);
}

// -----------------------------------------------------------------------

int SvxB3DVectorItem::operator==( const SfxPoolItem &rItem ) const
{
    DBG_CHKTHIS(SvxB3DVectorItem, 0);
    DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );
    return ((SvxB3DVectorItem&)rItem).aVal == aVal;
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxB3DVectorItem::Clone( SfxItemPool* /*pPool*/ ) const
{
    DBG_CHKTHIS(SvxB3DVectorItem, 0);
    return new SvxB3DVectorItem( *this );
}

// -----------------------------------------------------------------------

SfxPoolItem* SvxB3DVectorItem::Create(SvStream &rStream, sal_uInt16 /*nVersion*/) const
{
    DBG_CHKTHIS(SvxB3DVectorItem, 0);
    basegfx::B3DVector aStr;
    double fValue;
    rStream >> fValue; aStr.setX(fValue);
    rStream >> fValue; aStr.setY(fValue);
    rStream >> fValue; aStr.setZ(fValue);
    return new SvxB3DVectorItem(Which(), aStr);
}

// -----------------------------------------------------------------------

SvStream& SvxB3DVectorItem::Store(SvStream &rStream, sal_uInt16 /*nItemVersion*/) const
{
    DBG_CHKTHIS(SvxB3DVectorItem, 0);

    // ## if (nItemVersion)
    double fValue;
    fValue = aVal.getX(); rStream << fValue;
    fValue = aVal.getY(); rStream << fValue;
    fValue = aVal.getZ(); rStream << fValue;

    return rStream;
}

// -----------------------------------------------------------------------

bool SvxB3DVectorItem::QueryValue( uno::Any& rVal, sal_uInt8 /*nMemberId*/ ) const
{
    drawing::Direction3D aDirection;

    // Werte eintragen
    aDirection.DirectionX = aVal.getX();
    aDirection.DirectionY = aVal.getY();
    aDirection.DirectionZ = aVal.getZ();

    rVal <<= aDirection;
    return true;
}

// -----------------------------------------------------------------------

bool SvxB3DVectorItem::PutValue( const uno::Any& rVal, sal_uInt8 /*nMemberId*/ )
{
    drawing::Direction3D aDirection;
    if(!(rVal >>= aDirection))
        return false;

    aVal.setX(aDirection.DirectionX);
    aVal.setY(aDirection.DirectionY);
    aVal.setZ(aDirection.DirectionZ);
    return true;
}

// -----------------------------------------------------------------------

sal_uInt16 SvxB3DVectorItem::GetVersion (sal_uInt16 nFileFormatVersion) const
{
    return (nFileFormatVersion == SOFFICE_FILEFORMAT_31) ? USHRT_MAX : 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

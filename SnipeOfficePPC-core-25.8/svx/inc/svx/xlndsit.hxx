/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef _SVX_XLNDSIT_HXX
#define _SVX_XLNDSIT_HXX

#include <svx/xit.hxx>

#include <svx/xdash.hxx>
#include "svx/svxdllapi.h"

class SdrModel;

//--------------------
// class XLineDashItem
//--------------------
class SVX_DLLPUBLIC XLineDashItem : public NameOrIndex
{
    XDash   aDash;

public:
                            TYPEINFO();
                            XLineDashItem() : NameOrIndex(XATTR_LINEDASH, -1) {}
                            XLineDashItem(const String& rName, const XDash& rTheDash);
                            XLineDashItem(SfxItemPool* pPool, const XDash& rTheDash);
                            XLineDashItem(const XLineDashItem& rItem);
                            XLineDashItem(SvStream& rIn);

    virtual int             operator==(const SfxPoolItem& rItem) const;
    virtual SfxPoolItem*    Clone(SfxItemPool* pPool = 0) const;
    virtual SfxPoolItem*    Create(SvStream& rIn, sal_uInt16 nVer) const;
    virtual SvStream&       Store(SvStream& rOut, sal_uInt16 nItemVersion ) const;

    virtual bool            QueryValue( com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 ) const;
    virtual bool            PutValue( const com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 );

    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                    SfxMapUnit eCoreMetric,
                                    SfxMapUnit ePresMetric,
                                    String &rText, const IntlWrapper * = 0 ) const;
    virtual bool            HasMetrics() const;
    virtual bool            ScaleMetrics(long nMul, long nDiv);

    const XDash&			GetDashValue() const;
    void                    SetDashValue(const XDash& rNew)   { aDash = rNew; Detach(); } // SetValue -> SetDashValue

    static sal_Bool CompareValueFunc( const NameOrIndex* p1, const NameOrIndex* p2 );
    XLineDashItem* checkForUniqueItem( SdrModel* pModel ) const;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

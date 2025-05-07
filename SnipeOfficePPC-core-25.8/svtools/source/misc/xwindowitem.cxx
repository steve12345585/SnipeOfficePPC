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


#include "svtools/xwindowitem.hxx"

#include <vcl/window.hxx>


using namespace ::com::sun::star;

//////////////////////////////////////////////////////////////////////

TYPEINIT1_FACTORY( XWindowItem, SfxPoolItem, new XWindowItem );


XWindowItem::XWindowItem() :
    SfxPoolItem()
{
}


XWindowItem::XWindowItem( const XWindowItem &rItem ) :
    SfxPoolItem( Which() ),
    m_xWin( rItem.m_xWin )
{
}


XWindowItem::~XWindowItem()
{
}


SfxPoolItem * XWindowItem::Clone( SfxItemPool* /*pPool*/ ) const
{
    return new XWindowItem( *this );
}


int XWindowItem::operator == ( const SfxPoolItem & rAttr ) const
{
    DBG_ASSERT( SfxPoolItem::operator==(rAttr), "unequal types" );

    const XWindowItem * pItem = dynamic_cast< const XWindowItem * >(&rAttr);
    return pItem ? m_xWin == pItem->m_xWin : 0;
}


//////////////////////////////////////////////////////////////////////


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

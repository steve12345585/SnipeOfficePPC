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


#include <tools/debug.hxx>
#include <tools/stream.hxx>
#include <tools/vcompat.hxx>

#include <svl/cntwall.hxx>

#define CNTWALLPAPERITEM_STREAM_MAGIC   ( (sal_uInt32)0xfefefefe )
#define CNTWALLPAPERITEM_STREAM_SEEKREL (-( (long)( sizeof( sal_uInt32 ) ) ) )

TYPEINIT1( CntWallpaperItem, SfxPoolItem );

// -----------------------------------------------------------------------
CntWallpaperItem::CntWallpaperItem( sal_uInt16 which )
    : SfxPoolItem( which ), _nColor( COL_TRANSPARENT ), _nStyle( 0 )
{
}

// -----------------------------------------------------------------------
CntWallpaperItem::CntWallpaperItem( sal_uInt16 which, SvStream& rStream, sal_uInt16 nVersion )
    : SfxPoolItem( which ), _nColor( COL_TRANSPARENT ), _nStyle( 0 )
{
    sal_uInt32 nMagic = 0;
    rStream >> nMagic;
    if ( nMagic == CNTWALLPAPERITEM_STREAM_MAGIC )
    {
        // Okay, data were stored by CntWallpaperItem.

        _aURL = readUnicodeString(rStream, nVersion >= 1);
        // !!! Color stream operators do not work - they discard any
        // transparency info !!!
        _nColor.Read( rStream, sal_True );
        rStream >> _nStyle;
    }
    else
    {
        rStream.SeekRel( CNTWALLPAPERITEM_STREAM_SEEKREL );

        // Data were stored by SfxWallpaperItem ( SO < 6.0 ). The only
        // thing we can do here is to get the URL and to position the stream.

        {
            // "Read" Wallpaper member - The version compat object positions
            // the stream after the wallpaper data in its dtor. We must use
            // this trick here as no VCL must be used here ( No Wallpaper
            // object allowed ).
            VersionCompat aCompat( rStream, STREAM_READ );
        }

        // Read SfxWallpaperItem's string member _aURL.
        _aURL = readUnicodeString(rStream, false);

        // "Read" SfxWallpaperItem's string member _aFilter.
        read_lenPrefixed_uInt8s_ToOString<sal_uInt16>(rStream);
    }
}

// -----------------------------------------------------------------------
CntWallpaperItem::CntWallpaperItem( const CntWallpaperItem& rItem ) :
    SfxPoolItem( rItem ),
    _aURL( rItem._aURL ),
    _nColor( rItem._nColor ),
    _nStyle( rItem._nStyle )
{
}

// -----------------------------------------------------------------------
CntWallpaperItem::~CntWallpaperItem()
{
}

// -----------------------------------------------------------------------
int CntWallpaperItem::operator==( const SfxPoolItem& rItem ) const
{
    DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );

    const CntWallpaperItem& rWallItem = (const CntWallpaperItem&)rItem;

    if( ( rWallItem._nStyle == _nStyle ) &&
        ( rWallItem._nColor == _nColor ) &&
        ( rWallItem._aURL == _aURL ) )
        return sal_True;
    else
        return sal_False;
}

//============================================================================
// virtual
sal_uInt16 CntWallpaperItem::GetVersion(sal_uInt16) const
{
    return 1; // because it uses SfxPoolItem::read/writeUnicodeString()
}

// -----------------------------------------------------------------------
SfxPoolItem* CntWallpaperItem::Create( SvStream& rStream, sal_uInt16 nVersion) const
{
    return new CntWallpaperItem( Which(), rStream, nVersion );
}

// -----------------------------------------------------------------------
SvStream& CntWallpaperItem::Store( SvStream& rStream, sal_uInt16 ) const
{
    rStream << CNTWALLPAPERITEM_STREAM_MAGIC;
    writeUnicodeString(rStream, _aURL);
    // !!! Color stream operators do not work - they discard any
    // transparency info !!!
    // ??? Why the hell Color::Write(...) isn't const ???
    (const_cast< CntWallpaperItem* >(this))->_nColor.Write( rStream, sal_True );
    rStream << _nStyle;

    return rStream;
}

// -----------------------------------------------------------------------
SfxPoolItem* CntWallpaperItem::Clone( SfxItemPool* ) const
{
    return new CntWallpaperItem( *this );
}

//----------------------------------------------------------------------------
// virtual
bool CntWallpaperItem::QueryValue( com::sun::star::uno::Any&, sal_uInt8) const
{
    OSL_FAIL("Not implemented!");
    return false;
}

//----------------------------------------------------------------------------
// virtual
bool CntWallpaperItem::PutValue( const com::sun::star::uno::Any&, sal_uInt8)
{
    OSL_FAIL("Not implemented!");
    return false;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

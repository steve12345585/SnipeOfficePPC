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

#include <svl/lckbitem.hxx>
#include <svl/poolitem.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <tools/cachestr.hxx>

// -----------------------------------------------------------------------

TYPEINIT1_AUTOFACTORY(SfxLockBytesItem, SfxPoolItem);

// -----------------------------------------------------------------------

SfxLockBytesItem::SfxLockBytesItem()
{
}

// -----------------------------------------------------------------------

SfxLockBytesItem::SfxLockBytesItem( sal_uInt16 nW, SvStream &rStream )
:   SfxPoolItem( nW )
{
    rStream.Seek( 0L );
    _xVal = new SvLockBytes( new SvCacheStream(), sal_True );

    SvStream aLockBytesStream( _xVal );
    rStream >> aLockBytesStream;
}

// -----------------------------------------------------------------------

SfxLockBytesItem::SfxLockBytesItem( const SfxLockBytesItem& rItem )
:   SfxPoolItem( rItem ),
    _xVal( rItem._xVal )
{
}

// -----------------------------------------------------------------------

SfxLockBytesItem::~SfxLockBytesItem()
{
}

// -----------------------------------------------------------------------

int SfxLockBytesItem::operator==( const SfxPoolItem& rItem ) const
{
    return ((SfxLockBytesItem&)rItem)._xVal == _xVal;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxLockBytesItem::Clone(SfxItemPool *) const
{
    return new SfxLockBytesItem( *this );
}

// -----------------------------------------------------------------------

#define MAX_BUF 32000

SfxPoolItem* SfxLockBytesItem::Create( SvStream &rStream, sal_uInt16 ) const
{
    sal_uInt32 nSize = 0;
    sal_uLong nActRead = 0;
    sal_Char cTmpBuf[MAX_BUF];
    SvMemoryStream aNewStream;
    rStream >> nSize;

    do {
        sal_uLong nToRead;
        if( (nSize - nActRead) > MAX_BUF )
            nToRead = MAX_BUF;
        else
            nToRead = nSize - nActRead;
        nActRead += rStream.Read( cTmpBuf, nToRead );
        aNewStream.Write( cTmpBuf, nToRead );
    } while( nSize > nActRead );

    return new SfxLockBytesItem( Which(), aNewStream );
}

// -----------------------------------------------------------------------

SvStream& SfxLockBytesItem::Store(SvStream &rStream, sal_uInt16 ) const
{
    SvStream aLockBytesStream( _xVal );
    sal_uInt32 nSize = aLockBytesStream.Seek( STREAM_SEEK_TO_END );
    aLockBytesStream.Seek( 0L );

    rStream << nSize;
    rStream << aLockBytesStream;

    return rStream;
}

//----------------------------------------------------------------------------
// virtual
bool SfxLockBytesItem::PutValue( const com::sun::star::uno::Any& rVal, sal_uInt8 )
{
    com::sun::star::uno::Sequence< sal_Int8 > aSeq;
    if ( rVal >>= aSeq )
    {
        if ( aSeq.getLength() )
        {
            SvCacheStream* pStream = new SvCacheStream;
            pStream->Write( (void*)aSeq.getConstArray(), aSeq.getLength() );
            pStream->Seek(0);

            _xVal = new SvLockBytes( pStream, sal_True );
        }
        else
            _xVal = NULL;

        return true;
    }

    OSL_FAIL( "SfxLockBytesItem::PutValue - Wrong type!" );
    return true;
}

//----------------------------------------------------------------------------
// virtual
bool SfxLockBytesItem::QueryValue( com::sun::star::uno::Any& rVal, sal_uInt8 ) const
{
    if ( _xVal.Is() )
    {
        sal_uInt32 nLen;
        SvLockBytesStat aStat;

        if ( _xVal->Stat( &aStat, SVSTATFLAG_DEFAULT ) == ERRCODE_NONE )
            nLen = aStat.nSize;
        else
            return false;

        sal_uLong nRead = 0;
        com::sun::star::uno::Sequence< sal_Int8 > aSeq( nLen );

        _xVal->ReadAt( 0, aSeq.getArray(), nLen, &nRead );
        rVal <<= aSeq;
    }
    else
    {
        com::sun::star::uno::Sequence< sal_Int8 > aSeq( 0 );
        rVal <<= aSeq;
    }

    return true;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

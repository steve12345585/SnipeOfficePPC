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


#include "buffer.hxx"
#include "sbcomp.hxx"

const static sal_uInt32 UP_LIMIT=0xFFFFFF00L;

// The SbiBuffer will be expanded in increments of at least 16 Bytes.
// This is necessary, because many classes emanate from a buffer length
// of x*16 Bytes.

SbiBuffer::SbiBuffer( SbiParser* p, short n )
{
    pParser = p;
    n = ( (n + 15 ) / 16 ) * 16;
    if( !n ) n = 16;
    pBuf  = NULL;
    pCur  = NULL;
    nInc  = n;
    nSize =
    nOff  = 0;
}

SbiBuffer::~SbiBuffer()
{
    delete[] pBuf;
}

// Reach out the buffer
// This lead to the deletion of the buffer!

char* SbiBuffer::GetBuffer()
{
    char* p = pBuf;
    pBuf = NULL;
    pCur = NULL;
    return p;
}

// Test, if the buffer can contain n Bytes.
// In case of doubt it will be enlarged

sal_Bool SbiBuffer::Check( sal_uInt16 n )
{
    if( !n ) return sal_True;
    if( ( static_cast<sal_uInt32>( nOff )+ n ) >  static_cast<sal_uInt32>( nSize ) )
    {
        if( nInc == 0 )
            return sal_False;
        sal_uInt16 nn = 0;
        while( nn < n ) nn = nn + nInc;
        char* p;
        if( ( static_cast<sal_uInt32>( nSize ) + nn ) > UP_LIMIT ) p = NULL;
        else p = new char [nSize + nn];
        if( !p )
        {
            pParser->Error( SbERR_PROG_TOO_LARGE );
            nInc = 0;
            delete[] pBuf; pBuf = NULL;
            return sal_False;
        }
        else
        {
            if( nSize ) memcpy( p, pBuf, nSize );
            delete[] pBuf;
            pBuf = p;
            pCur = pBuf + nOff;
            nSize = nSize + nn;
        }
    }
    return sal_True;
}

// Patch of a Location

void SbiBuffer::Patch( sal_uInt32 off, sal_uInt32 val )
{
    if( ( off + sizeof( sal_uInt32 ) ) < nOff )
    {
        sal_uInt16 val1 = static_cast<sal_uInt16>( val & 0xFFFF );
        sal_uInt16 val2 = static_cast<sal_uInt16>( val >> 16 );
        sal_uInt8* p = (sal_uInt8*) pBuf + off;
        *p++ = (char) ( val1 & 0xFF );
        *p++ = (char) ( val1 >> 8 );
        *p++ = (char) ( val2 & 0xFF );
        *p   = (char) ( val2 >> 8 );
    }
}

// Forward References upon label und procedures
// establish a linkage. The beginning of the linkage is at the passed parameter,
// the end of the linkage is 0.

void SbiBuffer::Chain( sal_uInt32 off )
{
    if( off && pBuf )
    {
        sal_uInt8 *ip;
        sal_uInt32 i = off;
        sal_uInt32 val1 = (nOff & 0xFFFF);
        sal_uInt32 val2 = (nOff >> 16);
        do
        {
            ip = (sal_uInt8*) pBuf + i;
            sal_uInt8* pTmp = ip;
                     i =  *pTmp++; i |= *pTmp++ << 8; i |= *pTmp++ << 16; i |= *pTmp++ << 24;

            if( i >= nOff )
            {
                pParser->Error( SbERR_INTERNAL_ERROR, "BACKCHAIN" );
                break;
            }
            *ip++ = (char) ( val1 & 0xFF );
            *ip++ = (char) ( val1 >> 8 );
            *ip++ = (char) ( val2 & 0xFF );
            *ip   = (char) ( val2 >> 8 );
        } while( i );
    }
}

sal_Bool SbiBuffer::operator +=( sal_Int8 n )
{
    if( Check( 1 ) )
    {
        *pCur++ = (char) n; nOff++; return sal_True;
    } else return sal_False;
}

sal_Bool SbiBuffer::operator +=( sal_uInt8 n )
{
    if( Check( 1 ) )
    {
        *pCur++ = (char) n; nOff++; return sal_True;
    } else return sal_False;
}

sal_Bool SbiBuffer::operator +=( sal_Int16 n )
{
    if( Check( 2 ) )
    {
        *pCur++ = (char) ( n & 0xFF );
        *pCur++ = (char) ( n >> 8 );
        nOff += 2; return sal_True;
    } else return sal_False;
}

sal_Bool SbiBuffer::operator +=( sal_uInt16 n )
{
    if( Check( 2 ) )
    {
        *pCur++ = (char) ( n & 0xFF );
        *pCur++ = (char) ( n >> 8 );
        nOff += 2; return sal_True;
    } else return sal_False;
}

sal_Bool SbiBuffer::operator +=( sal_uInt32 n )
{
    if( Check( 4 ) )
    {
        sal_uInt16 n1 = static_cast<sal_uInt16>( n & 0xFFFF );
        sal_uInt16 n2 = static_cast<sal_uInt16>( n >> 16 );
        if ( operator +=( n1 ) && operator +=( n2 ) )
            return sal_True;
        return sal_True;
    }
    return sal_False;
}

sal_Bool SbiBuffer::operator +=( sal_Int32 n )
{
    return operator +=( (sal_uInt32) n );
}


sal_Bool SbiBuffer::operator +=( const String& n )
{
    sal_uInt16 l = n.Len() + 1;
    if( Check( l ) )
    {
        rtl::OString aByteStr(rtl::OUStringToOString(n, osl_getThreadTextEncoding()));
        memcpy( pCur, aByteStr.getStr(), l );
        pCur += l;
        nOff = nOff + l;
        return sal_True;
    }
    else return sal_False;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

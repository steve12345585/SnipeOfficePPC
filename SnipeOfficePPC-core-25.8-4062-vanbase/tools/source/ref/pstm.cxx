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

#include <tools/debug.hxx>
#include <tools/pstm.hxx>
#include <rtl/strbuf.hxx>

#define STOR_NO_OPTIMIZE

void SvClassManager::Register( sal_Int32 nClassId, SvCreateInstancePersist pFunc )
{
#ifdef DBG_UTIL
    SvCreateInstancePersist p;
    p = Get( nClassId );
    DBG_ASSERT( !p || p == pFunc, "register class with same id" );
#endif
    aAssocTable.insert(Map::value_type(nClassId, pFunc));
}

SvCreateInstancePersist SvClassManager::Get( sal_Int32 nClassId )
{
    Map::const_iterator i(aAssocTable.find(nClassId));
    return i == aAssocTable.end() ? 0 : i->second;
}

// SvRttiBase
TYPEINIT0( SvRttiBase );

// SvPersistBaseMemberList

#define PERSIST_LIST_VER        (sal_uInt8)0
#define PERSIST_LIST_DBGUTIL    (sal_uInt8)0x80

void TOOLS_DLLPUBLIC WritePersistListObjects(const SvPersistListWriteable& rList, SvPersistStream & rStm, bool bOnlyStreamed )
{
#ifdef STOR_NO_OPTIMIZE
    rStm << (sal_uInt8)(PERSIST_LIST_VER | PERSIST_LIST_DBGUTIL);
    sal_uInt32 nObjPos = rStm.WriteDummyLen();
#else
    sal_uInt8 bTmp = PERSIST_LIST_VER;
    rStm << bTmp;
#endif
    sal_uInt32 nCountMember = rList.size();
    sal_uIntPtr  nCountPos = rStm.Tell();
    sal_uInt32 nWriteCount = 0;
    rStm << nCountMember;
    // Don't change the list, as it causes side-effects while saving
    for( sal_uIntPtr n = 0; n < nCountMember; n++ )
    {
        SvPersistBase * pObj = rList.GetPersistBase( n );
        if( !bOnlyStreamed || rStm.IsStreamed( pObj ) )
        { // Object should be stored
            rStm << pObj;
            nWriteCount++;
        }
    }
    if( nWriteCount != nCountMember )
    {
        // Didn't write all members, adjust count
        sal_uIntPtr nPos = rStm.Tell();
        rStm.Seek( nCountPos );
        rStm << nWriteCount;
        rStm.Seek( nPos );
    }
#ifdef STOR_NO_OPTIMIZE
    rStm.WriteLen( nObjPos );
#endif
}

void TOOLS_DLLPUBLIC ReadObjects( SvPersistListReadable& rLst, SvPersistStream & rStm )
{
    sal_uInt8 nVer;
    rStm >> nVer;

    if( (nVer & ~PERSIST_LIST_DBGUTIL) != PERSIST_LIST_VER )
    {
        rStm.SetError( SVSTREAM_GENERALERROR );
        OSL_FAIL( "persist list, false version" );
    }

    sal_uInt32 nObjLen(0), nObjPos(0);
    if( nVer & PERSIST_LIST_DBGUTIL )
        nObjLen = rStm.ReadLen( &nObjPos );

    sal_uInt32 nCount;
    rStm >> nCount;
    for( sal_uIntPtr n = 0; n < nCount && rStm.GetError() == SVSTREAM_OK; n++ )
    {
        SvPersistBase * pObj;
        rStm >> pObj;
        if( pObj )
            rLst.push_back( pObj );
    }
#ifdef DBG_UTIL
            if( nObjLen + nObjPos != rStm.Tell() )
            {
                rtl::OStringBuffer aStr(
                    RTL_CONSTASCII_STRINGPARAM("false list len: read = "));
                aStr.append(static_cast<sal_Int64>(rStm.Tell() - nObjPos));
                aStr.append(RTL_CONSTASCII_STRINGPARAM(", should = "));
                aStr.append(static_cast<sal_Int64>(nObjLen));
                OSL_FAIL(aStr.getStr());
            }
#else
            (void)nObjLen;
#endif
}

/** Constructor

    @param rMgr       Stores factories for objects that can persisted
    @param pStream    This stream is used as the medium for PersistStream
    @param nStartIdxP Start value for object identifier (set > 0 )

    @warning Objects rMgr and pStream must not be manipulated while used in
             SvPersistStream. An Exception to this is pvStream
             (cf. <SvPersistStream::SetStream>).
    @see SvPersistStream::SetStream
*/
SvPersistStream::SvPersistStream( SvClassManager & rMgr, SvStream * pStream, sal_uInt32 nStartIdxP )
    : rClassMgr( rMgr )
    , pStm( pStream )
    , aPUIdx( nStartIdxP )
    , nStartIdx( nStartIdxP )
    , pRefStm( NULL )
    , nFlags( 0 )
{
    DBG_ASSERT( nStartIdx != 0, "zero index not allowed" );
    bIsWritable = sal_True;
    if( pStm )
    {
        SetVersion( pStm->GetVersion() );
        SetError( pStm->GetError() );
        SyncSvStream( pStm->Tell() );
    }
}

SvPersistStream::~SvPersistStream()
{
    SetStream( NULL );
}

/**

    @param pStream    This stream is used as the medium for PersistStream

    @warning pStream is used as the medium for PersistStream.
             It must not be manipulated while used in SvPersistStream.
             An Exception to this is pvStream (cf. <SvPersistStream::SetStream>).
*/
void SvPersistStream::SetStream( SvStream * pStream )
{
    if( pStm != pStream )
    {
        if( pStm )
        {
            SyncSysStream();
            pStm->SetError( GetError() );
        }
        pStm = pStream;
    }
    if( pStm )
    {
        SetVersion( pStm->GetVersion() );
        SetError( pStm->GetError() );
        SyncSvStream( pStm->Tell() );
    }
}

/** Returns the identifier of this stream class.

    @return ID_PERSISTSTREAM

    @see SvStream::IsA
*/
sal_uInt16 SvPersistStream::IsA() const
{
    return ID_PERSISTSTREAM;
}

void SvPersistStream::ResetError()
{
    SvStream::ResetError();
    DBG_ASSERT( pStm, "stream not set" );
    pStm->ResetError();
}

sal_uIntPtr SvPersistStream::GetData( void* pData, sal_uIntPtr nSize )
{
    DBG_ASSERT( pStm, "stream not set" );
    sal_uIntPtr nRet = pStm->Read( pData, nSize );
    SetError( pStm->GetError() );
    return nRet;
}

sal_uIntPtr SvPersistStream::PutData( const void* pData, sal_uIntPtr nSize )
{
    DBG_ASSERT( pStm, "stream not set" );
    sal_uIntPtr nRet = pStm->Write( pData, nSize );
    SetError( pStm->GetError() );
    return nRet;
}

sal_uIntPtr SvPersistStream::SeekPos( sal_uIntPtr nPos )
{
    DBG_ASSERT( pStm, "stream not set" );
    sal_uIntPtr nRet = pStm->Seek( nPos );
    SetError( pStm->GetError() );
    return nRet;
}

void SvPersistStream::FlushData()
{
}

sal_uIntPtr SvPersistStream::GetIndex( SvPersistBase * pObj ) const
{
    PersistBaseMap::const_iterator it = aPTable.find( pObj );
    if( it == aPTable.end() )
    {
        if ( pRefStm )
            return pRefStm->GetIndex( pObj );
        else
            return 0;
    }
    return it->second;
}

SvPersistBase * SvPersistStream::GetObject( sal_uIntPtr nIdx ) const
{
    if( nIdx >= nStartIdx )
        return aPUIdx.Get( nIdx );
    else if( pRefStm )
        return pRefStm->GetObject( nIdx );
    return NULL;
}

#define LEN_1           0x80
#define LEN_2           0x40
#define LEN_4           0x20
#define LEN_5           0x10

/** Reads a compressed word from the stream.

    For details on what format is used for compression, see
    <SvPersistStream::WriteCompressed>.

    @param rStm Source to read compressed data from

    @return Uncompressed word
    @see SvPersistStream::WriteCompressed
*/
sal_uInt32 SvPersistStream::ReadCompressed( SvStream & rStm )
{
    sal_uInt32 nRet(0);
    sal_uInt8   nMask;
    rStm >> nMask;
    if( nMask & LEN_1 )
        nRet = ~LEN_1 & nMask;
    else if( nMask & LEN_2 )
    {
        nRet = ~LEN_2 & nMask;
        nRet <<= 8;
        rStm >> nMask;
        nRet |= nMask;
    }
    else if( nMask & LEN_4 )
    {
        nRet = ~LEN_4 & nMask;
        nRet <<= 8;
        rStm >> nMask;
        nRet |= nMask;
        nRet <<= 16;
        sal_uInt16 n;
        rStm >> n;
        nRet |= n;
    }
    else if( nMask & LEN_5 )
    {
        if( nMask & 0x0F )
        {
            rStm.SetError( SVSTREAM_FILEFORMAT_ERROR );
            OSL_FAIL( "format error" );
        }
        rStm >> nRet;
    }
    else
    {
        rStm.SetError( SVSTREAM_FILEFORMAT_ERROR );
        OSL_FAIL( "format error" );
    }
    return nRet;
}

/** Writes compressed stream

    @param rStm Source for writing compressed data
    @param nVal This value will be compressed and written

    nVal is compressed and written to stream using the following algorithm
    nVal < 0x80         =>  0x80        + nVal of size 1 Byte.
    nVal < 0x4000       =>  0x4000      + nVal of size 2 Byte.
    nVal < 0x20000000   =>  0x20000000  + nVal of size 4 Byte.
    nVal > 0x1FFFFFFF   =>  0x1000000000+ nVal of size 5 Byte.

    @see SvPersistStream::ReadCompressed
*/
void SvPersistStream::WriteCompressed( SvStream & rStm, sal_uInt32 nVal )
{
#ifdef STOR_NO_OPTIMIZE
    if( nVal < 0x80 )
        rStm << (sal_uInt8)(LEN_1 | nVal);
    else if( nVal < 0x4000 )
    {
        rStm << (sal_uInt8)(LEN_2 | (nVal >> 8));
        rStm << (sal_uInt8)nVal;
    }
    else if( nVal < 0x20000000 )
    {
        // highest sal_uInt8
        rStm << (sal_uInt8)(LEN_4 | (nVal >> 24));
        // 2nd highest sal_uInt8
        rStm << (sal_uInt8)(nVal >> 16);
        rStm << (sal_uInt16)(nVal);
    }
    else
#endif
    {
        rStm << (sal_uInt8)LEN_5;
        rStm << nVal;
    }
}

/** This method writes length value of 4 Bytes to the stream and returns the
    stream position.

    Example:
    @code
    sal_uInt32 nObjPos = rStm.WriteDummyLen();
    ...
    // write data
    ...
    rStm.WriteLen( nObjPos );
    @endcode

    @return Position of stream behind length value

    @see SvPersistStream::ReadLen
    @see SvPersistStream::WriteLen
*/
sal_uInt32 SvPersistStream::WriteDummyLen()
{
#ifdef DBG_UTIL
    sal_uInt32 nPos = Tell();
#endif
    sal_uInt32 n0 = 0;
    *this << n0; // wegen Sun sp
    // Don't assert on stream error
    DBG_ASSERT( GetError() != SVSTREAM_OK
                  || (sizeof( sal_uInt32 ) == Tell() -nPos),
                "keine 4-Byte fuer Langenangabe" );
    return Tell();
}

/** Write difference between current position and nObjPos
    as sal_uInt32 to position nObjPos-4 in the stream.

    Afterwards, reset stream to old position.

    Example: Difference does not contain length value
    @code
    sal_uInt32 nObjPos = rStm.WriteDummyLen();
    ...
    // write data
    ...
    rStm.WriteLen( nObjPos );
    // write more data
    @endcode

    @param nObjPos Position+4, on which length is written to

    @see SvPersistStream::ReadLen
    @see SvPersistStream::WriteDummyLen
*/
void SvPersistStream::WriteLen( sal_uInt32 nObjPos )
{
    sal_uInt32 nPos = Tell();
    sal_uInt32 nLen = nPos - nObjPos;
    // Length in stream must be 4 Bytes
    Seek( nObjPos - sizeof( sal_uInt32 ) );
    // write length
    *this << nLen;
    Seek( nPos );
}

/** Read a length value written to stream

    @param pTestPos Position of the stream after reading length. May be NULL.

    @see SvPersistStream::WriteDummyLen
    @see SvPersistStream::WriteLen
*/
sal_uInt32 SvPersistStream::ReadLen( sal_uInt32 * pTestPos )
{
    sal_uInt32 nLen;
    *this >> nLen;
    if( pTestPos )
        *pTestPos = Tell();
    return nLen;
}

// File format backward-compatible
#ifdef STOR_NO_OPTIMIZE
#define P_VER       (sal_uInt8)0x00
#else
#define P_VER       (sal_uInt8)0x01
#endif
#define P_VER_MASK  (sal_uInt8)0x0F
#define P_ID_0      (sal_uInt8)0x80
#define P_OBJ       (sal_uInt8)0x40
#define P_DBGUTIL   (sal_uInt8)0x20
#define P_ID        (sal_uInt8)0x10
#ifdef STOR_NO_OPTIMIZE
#define P_STD   P_DBGUTIL
#else
#define P_STD   0
#endif

static void WriteId
(
    SvStream & rStm,
    sal_uInt8 nHdr,
    sal_uInt32 nId,
    sal_uInt16 nClassId
)
{
#ifdef STOR_NO_OPTIMIZE
    nHdr |= P_ID;
#endif
    nHdr |= P_VER;
    if( nHdr & P_ID )
    {
        if( (nHdr & P_OBJ) || nId != 0 )
        { // Id set only for pointers or DBGUTIL
            rStm << (sal_uInt8)(nHdr);
            SvPersistStream::WriteCompressed( rStm, nId );
        }
        else
        { // NULL Pointer
            rStm << (sal_uInt8)(nHdr | P_ID_0);
            return;
        }
    }
    else
        rStm << nHdr;

    if( (nHdr & P_DBGUTIL) || (nHdr & P_OBJ) )
        // Objects always have a class id
        // Pointers only for DBG_UTIL and != NULL
        SvPersistStream::WriteCompressed( rStm, nClassId );
}

static void ReadId
(
    SvStream & rStm,
    sal_uInt8 & nHdr,
    sal_uInt32 & nId,
    sal_uInt16 & nClassId
)
{
    nClassId = 0;
    rStm >> nHdr;
    if( nHdr & P_ID_0 )
        nId = 0;
    else
    {
        if( (nHdr & P_VER_MASK) == 0 )
        {
            if( (nHdr & P_DBGUTIL) || !(nHdr & P_OBJ) )
                nId = SvPersistStream::ReadCompressed( rStm );
            else
                nId = 0;
        }
        else if( nHdr & P_ID )
            nId = SvPersistStream::ReadCompressed( rStm );

        if( (nHdr & P_DBGUTIL) || (nHdr & P_OBJ) )
            nClassId = (sal_uInt16)SvPersistStream::ReadCompressed( rStm );
    }
}

void SvPersistStream::WriteObj
(
    sal_uInt8 nHdr,
    SvPersistBase * pObj
)
{
#ifdef STOR_NO_OPTIMIZE
    sal_uInt32 nObjPos = 0;
    if( nHdr & P_DBGUTIL )
        // remember position for length value
        nObjPos = WriteDummyLen();
#endif
    pObj->Save( *this );
#ifdef STOR_NO_OPTIMIZE
    if( nHdr & P_DBGUTIL )
        WriteLen( nObjPos );
#endif
}

SvPersistStream& SvPersistStream::WritePointer
(
    SvPersistBase * pObj
)
{
    sal_uInt8 nP = P_STD;

    if( pObj )
    {
        sal_uIntPtr nId = GetIndex( pObj );
        if( nId )
            nP |= P_ID;
        else
        {
            nId = aPUIdx.Insert( pObj );
            aPTable[ pObj ] = nId;
            nP |= P_OBJ;
        }
        WriteId( *this, nP, nId, pObj->GetClassId() );
        if( nP & P_OBJ )
            WriteObj( nP, pObj );
    }
    else
    { // NULL Pointer
        WriteId( *this, nP | P_ID, 0, 0 );
    }
    return *this;
}

sal_uInt32 SvPersistStream::ReadObj
(
    SvPersistBase * &   rpObj,
    sal_Bool                bRegister
)
{
    sal_uInt8   nHdr;
    sal_uInt32  nId = 0;
    sal_uInt16  nClassId;

    rpObj = NULL;   // specification: 0 in case of error
    ReadId( *this, nHdr, nId, nClassId );

    // get version nummer through masking
    if( P_VER < (nHdr & P_VER_MASK) )
    {
        SetError( SVSTREAM_FILEFORMAT_ERROR );
        OSL_FAIL( "false version" );
    }

    if( !(nHdr & P_ID_0) && GetError() == SVSTREAM_OK )
    {
        if( P_OBJ & nHdr )
        { // read object, nId only set for P_DBGUTIL
            DBG_ASSERT( !(nHdr & P_DBGUTIL) || NULL == aPUIdx.Get( nId ),
                        "object already exist" );
            SvCreateInstancePersist pFunc = rClassMgr.Get( nClassId );

            sal_uInt32 nObjLen(0), nObjPos(0);
            if( nHdr & P_DBGUTIL )
                nObjLen = ReadLen( &nObjPos );
            if( !pFunc )
            {
#ifdef DBG_UTIL
                rtl::OStringBuffer aStr(
                    RTL_CONSTASCII_STRINGPARAM("no class with id: " ));
                aStr.append(static_cast<sal_Int32>(nClassId));
                aStr.append(RTL_CONSTASCII_STRINGPARAM(" registered"));
                DBG_WARNING(aStr.getStr());
#else
                (void)nObjLen;
#endif
                SetError( ERRCODE_IO_NOFACTORY );
                return 0;
            }
            pFunc( &rpObj );
            // Save reference
            rpObj->AddRef();

            if( bRegister )
            {
                // insert into table
                sal_uIntPtr nNewId = aPUIdx.Insert( rpObj );
                // in order to restore state after saving
                aPTable[ rpObj ] = nNewId;
                DBG_ASSERT( !(nHdr & P_DBGUTIL) || nId == nNewId,
                            "read write id conflict: not the same" );
            }
            rpObj->Load( *this );
#ifdef DBG_UTIL
            if( nObjLen + nObjPos != Tell() )
            {
                rtl::OStringBuffer aStr(
                    RTL_CONSTASCII_STRINGPARAM("false object len: read = "));
                aStr.append(static_cast<sal_Int64>((long)(Tell() - nObjPos)));
                aStr.append(RTL_CONSTASCII_STRINGPARAM(", should = "));
                aStr.append(static_cast<sal_Int32>(nObjLen));
                OSL_FAIL(aStr.getStr());
            }
#endif
            rpObj->RestoreNoDelete();
            rpObj->ReleaseRef();
        }
        else
        {
            rpObj = GetObject( nId );
            DBG_ASSERT( rpObj != NULL, "object does not exist" );
            DBG_ASSERT( rpObj->GetClassId() == nClassId, "class mismatch" );
        }
    }
    return nId;
}

SvPersistStream& SvPersistStream::ReadPointer
(
    SvPersistBase * & rpObj
)
{
    ReadObj( rpObj, sal_True );
    return *this;
}

SvPersistStream& operator <<
(
    SvPersistStream & rStm,
    SvPersistBase * pObj
)
{
    return rStm.WritePointer( pObj );
}

SvPersistStream& operator >>
(
    SvPersistStream & rStm,
    SvPersistBase * & rpObj
)
{
    return rStm.ReadPointer( rpObj );
}

SvStream& operator <<
(
    SvStream & rStm,
    SvPersistStream & rThis
)
{
    SvStream * pOldStm = rThis.GetStream();
    rThis.SetStream( &rStm );

    sal_uInt8 bTmp = 0;
    rThis << bTmp;    // Version
    sal_uInt32 nCount = (sal_uInt32)rThis.aPUIdx.Count();
    rThis << nCount;
    sal_uIntPtr aIndex = rThis.aPUIdx.FirstIndex();
    for( sal_uInt32 i = 0; i < nCount; i++ )
    {
        SvPersistBase * pEle = rThis.aPUIdx.Get(aIndex);
        sal_uInt8 nP = P_OBJ | P_ID | P_STD;
        WriteId( rThis, nP, aIndex, pEle->GetClassId() );
        rThis.WriteObj( nP, pEle );
        aIndex = rThis.aPUIdx.NextIndex( aIndex );
    }
    rThis.SetStream( pOldStm );
    return rStm;
}

SvStream& operator >>
(
    SvStream & rStm,
    SvPersistStream & rThis
)
{
    SvStream * pOldStm = rThis.GetStream();
    rThis.SetStream( &rStm );

    sal_uInt8 nVers;
    rThis >> nVers;    // Version
    if( 0 == nVers )
    {
        sal_uInt32 nCount = 0;
        rThis >> nCount;
        for( sal_uInt32 i = 0; i < nCount; i++ )
        {
            SvPersistBase * pEle;
            // read, but don't insert into table
            sal_uIntPtr nId = rThis.ReadObj( pEle, sal_False );
            if( rThis.GetError() )
                break;

            // Id of object is never modified
            rThis.aPUIdx.Insert( nId, pEle );
            rThis.aPTable[ pEle ] = nId;
        }
    }
    else
        rThis.SetError( SVSTREAM_FILEFORMAT_ERROR );

    rThis.SetStream( pOldStm );
    return rStm;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

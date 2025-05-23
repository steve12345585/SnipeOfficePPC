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



//___________________________________________________________________

#include <stdio.h>
#include <sot/storage.hxx>
#include "XclExpChangeTrack.hxx"
#include "xeformula.hxx"
#include "cell.hxx"
#include "xcl97rec.hxx"

#include <oox/token/tokens.hxx>
#include <rtl/strbuf.hxx>

using ::rtl::OUString;
using ::rtl::OString;
using ::rtl::OStringBuffer;
using namespace oox;

static OString lcl_GuidToOString( sal_uInt8 aGuid[ 16 ] )
{
    char sBuf[ 40 ];
    snprintf( sBuf, sizeof( sBuf ),
            "{%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
            aGuid[ 0 ], aGuid[ 1 ], aGuid[ 2 ], aGuid[ 3 ], aGuid[ 4 ], aGuid[ 5 ], aGuid[ 6 ], aGuid[ 7 ],
            aGuid[ 8 ], aGuid[ 9 ], aGuid[ 10 ], aGuid[ 11 ], aGuid[ 12 ], aGuid[ 13 ], aGuid[ 14 ], aGuid[ 15 ] );
    return OString( sBuf );
}

static OString lcl_DateTimeToOString( const DateTime& rDateTime )
{
    char sBuf[ 200 ];
    snprintf( sBuf, sizeof( sBuf ),
            "%d-%02d-%02dT%02d:%02d:%02d.%02dZ",
            rDateTime.GetYear(), rDateTime.GetMonth(), rDateTime.GetDay(),
            rDateTime.GetHour(), rDateTime.GetMin(), rDateTime.GetSec(),
            rDateTime.Get100Sec() );
    return OString( sBuf );
}

//___________________________________________________________________
// local functions

static void lcl_WriteDateTime( XclExpStream& rStrm, const DateTime& rDateTime )
{
    rStrm.SetSliceSize( 7 );
    rStrm   << (sal_uInt16) rDateTime.GetYear()
            << (sal_uInt8)  rDateTime.GetMonth()
            << (sal_uInt8)  rDateTime.GetDay()
            << (sal_uInt8)  rDateTime.GetHour()
            << (sal_uInt8)  rDateTime.GetMin()
            << (sal_uInt8)  rDateTime.GetSec();
    rStrm.SetSliceSize( 0 );
}

// write string and fill rest of <nLength> with zero bytes
// <nLength> is without string header
static void lcl_WriteFixedString( XclExpStream& rStrm, const XclExpString& rString, sal_Size nLength )
{
    sal_Size nStrBytes = rString.GetBufferSize();
    OSL_ENSURE( nLength >= nStrBytes, "lcl_WriteFixedString - String too long" );
    if( rString.Len() > 0 )
        rStrm << rString;
    if( nLength > nStrBytes )
        rStrm.WriteZeroBytes( nLength - nStrBytes );
}

static inline void lcl_GenerateGUID( sal_uInt8* pGUID, sal_Bool& rValidGUID )
{
    rtl_createUuid( pGUID, rValidGUID ? pGUID : NULL, false );
    rValidGUID = sal_True;
}

static inline void lcl_WriteGUID( XclExpStream& rStrm, const sal_uInt8* pGUID )
{
    rStrm.SetSliceSize( 16 );
    for( sal_Size nIndex = 0; nIndex < 16; nIndex++ )
        rStrm << pGUID[ nIndex ];
    rStrm.SetSliceSize( 0 );
}

//___________________________________________________________________

XclExpUserBView::XclExpUserBView( const String& rUsername, const sal_uInt8* pGUID ) :
    sUsername( rUsername )
{
    memcpy( aGUID, pGUID, 16 );
}

void XclExpUserBView::SaveCont( XclExpStream& rStrm )
{
    rStrm   << (sal_uInt32) 0xFF078014
            << (sal_uInt32) 0x00000001;
    lcl_WriteGUID( rStrm, aGUID );
    rStrm.WriteZeroBytes( 8 );
    rStrm   << (sal_uInt32) 1200
            << (sal_uInt32) 1000
            << (sal_uInt16) 1000
            << (sal_uInt16) 0x0CF7
            << (sal_uInt16) 0x0000
            << (sal_uInt16) 0x0001
            << (sal_uInt16) 0x0000;
    if( sUsername.Len() > 0 )
        rStrm << sUsername;
}

sal_uInt16 XclExpUserBView::GetNum() const
{
    return 0x01A9;
}

sal_Size XclExpUserBView::GetLen() const
{
    return 50 + ((sUsername.Len() > 0) ? sUsername.GetSize() : 0);
}

//___________________________________________________________________

XclExpUserBViewList::XclExpUserBViewList( const ScChangeTrack& rChangeTrack )
{
    sal_uInt8 aGUID[ 16 ];
    sal_Bool bValidGUID = false;
    const std::set<rtl::OUString>& rStrColl = rChangeTrack.GetUserCollection();
    aViews.reserve(rStrColl.size());
    std::set<rtl::OUString>::const_iterator it = rStrColl.begin(), itEnd = rStrColl.end();
    for (; it != itEnd; ++it)
    {
        lcl_GenerateGUID( aGUID, bValidGUID );
        aViews.push_back( new XclExpUserBView(*it, aGUID) );
    }
}

XclExpUserBViewList::~XclExpUserBViewList()
{
    for( iterator iter = aViews.begin(); iter != aViews.end(); ++iter )
        delete *iter;
}

void XclExpUserBViewList::Save( XclExpStream& rStrm )
{
   for( iterator iter = aViews.begin(); iter != aViews.end(); ++iter )
        (*iter)->Save( rStrm );
}

//___________________________________________________________________

XclExpUsersViewBegin::XclExpUsersViewBegin( const sal_uInt8* pGUID, sal_uInt32 nTab ) :
    nCurrTab( nTab )
{
    memcpy( aGUID, pGUID, 16 );
}

void XclExpUsersViewBegin::SaveCont( XclExpStream& rStrm )
{
    lcl_WriteGUID( rStrm, aGUID );
    rStrm   << nCurrTab
            << (sal_uInt32) 100
            << (sal_uInt32) 64
            << (sal_uInt32) 3
            << (sal_uInt32) 0x0000003C
            << (sal_uInt16) 0
            << (sal_uInt16) 3
            << (sal_uInt16) 0
            << (sal_uInt16) 3
            << (double)     0
            << (double)     0
            << (sal_Int16)  -1
            << (sal_Int16)  -1;
}

sal_uInt16 XclExpUsersViewBegin::GetNum() const
{
    return 0x01AA;
}

sal_Size XclExpUsersViewBegin::GetLen() const
{
    return 64;
}

//___________________________________________________________________

void XclExpUsersViewEnd::SaveCont( XclExpStream& rStrm )
{
    rStrm << (sal_uInt16) 0x0001;
}

sal_uInt16 XclExpUsersViewEnd::GetNum() const
{
    return 0x01AB;
}

sal_Size XclExpUsersViewEnd::GetLen() const
{
    return 2;
}

//___________________________________________________________________

void XclExpChTr0x0191::SaveCont( XclExpStream& rStrm )
{
    rStrm << (sal_uInt16) 0x0000;
}

sal_uInt16 XclExpChTr0x0191::GetNum() const
{
    return 0x0191;
}

sal_Size XclExpChTr0x0191::GetLen() const
{
    return 2;
}

//___________________________________________________________________

void XclExpChTr0x0198::SaveCont( XclExpStream& rStrm )
{
    rStrm   << (sal_uInt16) 0x0006
            << (sal_uInt16) 0x0000;
}

sal_uInt16 XclExpChTr0x0198::GetNum() const
{
    return 0x0198;
}

sal_Size XclExpChTr0x0198::GetLen() const
{
    return 4;
}

//___________________________________________________________________

void XclExpChTr0x0192::SaveCont( XclExpStream& rStrm )
{
    rStrm << sal_uInt16( 0x0022 );
    rStrm.WriteZeroBytes( 510 );
}

sal_uInt16 XclExpChTr0x0192::GetNum() const
{
    return 0x0192;
}

sal_Size XclExpChTr0x0192::GetLen() const
{
    return 512;
}

//___________________________________________________________________

void XclExpChTr0x0197::SaveCont( XclExpStream& rStrm )
{
    rStrm << (sal_uInt16) 0x0000;
}

sal_uInt16 XclExpChTr0x0197::GetNum() const
{
    return 0x0197;
}

sal_Size XclExpChTr0x0197::GetLen() const
{
    return 2;
}

//___________________________________________________________________

XclExpChTrEmpty::~XclExpChTrEmpty()
{
}

sal_uInt16 XclExpChTrEmpty::GetNum() const
{
    return nRecNum;
}

sal_Size XclExpChTrEmpty::GetLen() const
{
    return 0;
}

//___________________________________________________________________

XclExpChTr0x0195::~XclExpChTr0x0195()
{
}

void XclExpChTr0x0195::SaveCont( XclExpStream& rStrm )
{
    rStrm.WriteZeroBytes( 162 );
}

sal_uInt16 XclExpChTr0x0195::GetNum() const
{
    return 0x0195;
}

sal_Size XclExpChTr0x0195::GetLen() const
{
    return 162;
}

//___________________________________________________________________

XclExpChTr0x0194::~XclExpChTr0x0194()
{
}

void XclExpChTr0x0194::SaveCont( XclExpStream& rStrm )
{
    rStrm << (sal_uInt32) 0;
    lcl_WriteDateTime( rStrm, aDateTime );
    rStrm << (sal_uInt8) 0;
    lcl_WriteFixedString( rStrm, sUsername, 147 );
}

sal_uInt16 XclExpChTr0x0194::GetNum() const
{
    return 0x0194;
}

sal_Size XclExpChTr0x0194::GetLen() const
{
    return 162;
}

//___________________________________________________________________

XclExpChTrHeader::~XclExpChTrHeader()
{
}

void XclExpChTrHeader::SaveCont( XclExpStream& rStrm )
{
    rStrm   << (sal_uInt16) 0x0006
            << (sal_uInt16) 0x0000
            << (sal_uInt16) 0x000D;
    lcl_WriteGUID( rStrm, aGUID );
    lcl_WriteGUID( rStrm, aGUID );
    rStrm   << nCount
            << (sal_uInt16) 0x0001
            << (sal_uInt32) 0x00000000
            << (sal_uInt16) 0x001E;
}

sal_uInt16 XclExpChTrHeader::GetNum() const
{
    return 0x0196;
}

sal_Size XclExpChTrHeader::GetLen() const
{
    return 50;
}

void XclExpChTrHeader::SaveXml( XclExpXmlStream& rRevisionHeadersStrm )
{
    sax_fastparser::FSHelperPtr pHeaders = rRevisionHeadersStrm.GetCurrentStream();
    rRevisionHeadersStrm.WriteAttributes(
            XML_guid,               lcl_GuidToOString( aGUID ).getStr(),
            XML_lastGuid,           NULL,   // OOXTODO
            XML_shared,             NULL,   // OOXTODO
            XML_diskRevisions,      NULL,   // OOXTODO
            XML_history,            NULL,   // OOXTODO
            XML_trackRevisions,     NULL,   // OOXTODO
            XML_exclusive,          NULL,   // OOXTODO
            XML_revisionId,         NULL,   // OOXTODO
            XML_version,            NULL,   // OOXTODO
            XML_keepChangeHistory,  NULL,   // OOXTODO
            XML_protected,          NULL,   // OOXTODO
            XML_preserveHistory,    NULL,   // OOXTODO
            FSEND );
    pHeaders->write( ">" );
}

//___________________________________________________________________

XclExpChTrInfo::~XclExpChTrInfo()
{
}

void XclExpChTrInfo::SaveCont( XclExpStream& rStrm )
{
    rStrm   << (sal_uInt32) 0xFFFFFFFF
            << (sal_uInt32) 0x00000000
            << (sal_uInt32) 0x00000020
            << (sal_uInt16) 0xFFFF;
    lcl_WriteGUID( rStrm, aGUID );
    rStrm   << (sal_uInt16) 0x04B0;
    lcl_WriteFixedString( rStrm, sUsername, 113 );
    lcl_WriteDateTime( rStrm, aDateTime );
    rStrm   << (sal_uInt8)  0x0000
            << (sal_uInt16) 0x0002;
}

sal_uInt16 XclExpChTrInfo::GetNum() const
{
    return 0x0138;
}

sal_Size XclExpChTrInfo::GetLen() const
{
    return 158;
}

void XclExpChTrInfo::SaveXml( XclExpXmlStream& rRevisionHeadersStrm )
{
    sax_fastparser::FSHelperPtr pHeader = rRevisionHeadersStrm.GetCurrentStream();

    OUString sRelationshipId;
    sax_fastparser::FSHelperPtr pRevisionLog = rRevisionHeadersStrm.CreateOutputStream(
            XclXmlUtils::GetStreamName( "xl/revisions/", "revisionLog", mnLogNumber ),
            XclXmlUtils::GetStreamName( NULL, "revisionLog", mnLogNumber ),
            rRevisionHeadersStrm.GetCurrentStream()->getOutputStream(),
            "application/vnd.openxmlformats-officedocument.spreadsheetml.revisionLog+xml",
            "http://schemas.openxmlformats.org/officeDocument/2006/relationships/revisionLog",
            &sRelationshipId );

    rRevisionHeadersStrm.WriteAttributes(
            XML_guid,               lcl_GuidToOString( aGUID ).getStr(),
            XML_dateTime,           lcl_DateTimeToOString( aDateTime ).getStr(),
            XML_maxSheetId,         NULL,   // OOXTODO
            XML_userName,           XclXmlUtils::ToOString( sUsername ).getStr(),
            FSNS( XML_r, XML_id ),  XclXmlUtils::ToOString( sRelationshipId ).getStr(),
            XML_minRId,             NULL,   // OOXTODO
            XML_maxRId,             NULL,   // OOXTODO
            FSEND );
    pHeader->write( ">" );

    rRevisionHeadersStrm.PushStream( pRevisionLog );
}

//___________________________________________________________________

XclExpChTrTabIdBuffer::XclExpChTrTabIdBuffer( sal_uInt16 nCount ) :
    nBufSize( nCount ),
    nLastId( nCount )
{
    pBuffer = new sal_uInt16[ nBufSize ];
    memset( pBuffer, 0, sizeof(sal_uInt16) * nBufSize );
    pLast = pBuffer + nBufSize - 1;
}

XclExpChTrTabIdBuffer::XclExpChTrTabIdBuffer( const XclExpChTrTabIdBuffer& rCopy ) :
    nBufSize( rCopy.nBufSize ),
    nLastId( rCopy.nLastId )
{
    pBuffer = new sal_uInt16[ nBufSize ];
    memcpy( pBuffer, rCopy.pBuffer, sizeof(sal_uInt16) * nBufSize );
    pLast = pBuffer + nBufSize - 1;
}

XclExpChTrTabIdBuffer::~XclExpChTrTabIdBuffer()
{
    delete[] pBuffer;
}

void XclExpChTrTabIdBuffer::InitFill( sal_uInt16 nIndex )
{
    OSL_ENSURE( nIndex < nLastId, "XclExpChTrTabIdBuffer::Insert - out of range" );

    sal_uInt16 nFreeCount = 0;
    for( sal_uInt16* pElem = pBuffer; pElem <= pLast; pElem++ )
    {
        if( !*pElem )
            nFreeCount++;
        if( nFreeCount > nIndex )
        {
            *pElem = nLastId--;
            return;
        }
    }
}

void XclExpChTrTabIdBuffer::InitFillup()
{
    sal_uInt16 nFreeCount = 1;
    for( sal_uInt16* pElem = pBuffer; pElem <= pLast; pElem++ )
        if( !*pElem )
            *pElem = nFreeCount++;
    nLastId = nBufSize;
}

sal_uInt16 XclExpChTrTabIdBuffer::GetId( sal_uInt16 nIndex ) const
{
    OSL_ENSURE( nIndex < nBufSize, "XclExpChTrTabIdBuffer::GetId - out of range" );
    return pBuffer[ nIndex ];
}

void XclExpChTrTabIdBuffer::Remove()
{
    OSL_ENSURE( pBuffer <= pLast, "XclExpChTrTabIdBuffer::Remove - buffer empty" );
    sal_uInt16* pElem = pBuffer;
    while( (pElem <= pLast) && (*pElem != nLastId) )
        pElem++;
    while( pElem < pLast )
    {
        *pElem = *(pElem + 1);
        pElem++;
    }
    pLast--;
    nLastId--;
}

//___________________________________________________________________

XclExpChTrTabId::XclExpChTrTabId( const XclExpChTrTabIdBuffer& rBuffer, bool bInRevisionHeaders )
    : nTabCount( rBuffer.GetBufferCount() )
    , mbInRevisionHeaders( bInRevisionHeaders )
{
    pBuffer = new sal_uInt16[ nTabCount ];
    rBuffer.GetBufferCopy( pBuffer );
}

XclExpChTrTabId::~XclExpChTrTabId()
{
    Clear();
}

void XclExpChTrTabId::Copy( const XclExpChTrTabIdBuffer& rBuffer )
{
    Clear();
    nTabCount = rBuffer.GetBufferCount();
    pBuffer = new sal_uInt16[ nTabCount ];
    rBuffer.GetBufferCopy( pBuffer );
}

void XclExpChTrTabId::SaveCont( XclExpStream& rStrm )
{
    rStrm.EnableEncryption();
    if( pBuffer )
        for( sal_uInt16* pElem = pBuffer; pElem < (pBuffer + nTabCount); pElem++ )
            rStrm << *pElem;
    else
        for( sal_uInt16 nIndex = 1; nIndex <= nTabCount; nIndex++ )
            rStrm << nIndex;
}

sal_uInt16 XclExpChTrTabId::GetNum() const
{
    return 0x013D;
}

sal_Size XclExpChTrTabId::GetLen() const
{
    return nTabCount << 1;
}

void XclExpChTrTabId::SaveXml( XclExpXmlStream& rRevisionLogStrm )
{
    if( !mbInRevisionHeaders )
        return;

    sax_fastparser::FSHelperPtr pRevisionLog = rRevisionLogStrm.GetCurrentStream();
    rRevisionLogStrm.PopStream();

    sax_fastparser::FSHelperPtr pHeader = rRevisionLogStrm.GetCurrentStream();
    pHeader->startElement( XML_sheetIdMap,
            XML_count,  OString::valueOf( sal_Int32( nTabCount ) ).getStr(),
            FSEND );
    for( int i = 0; i < nTabCount; ++i )
    {
        pHeader->singleElement( XML_sheetId,
                XML_val,    OString::valueOf( sal_Int32( pBuffer[ i ] ) ).getStr(),
                FSEND );
    }
    pHeader->endElement( XML_sheetIdMap );

    rRevisionLogStrm.PushStream( pRevisionLog );
}

//___________________________________________________________________

// ! does not copy additional actions
XclExpChTrAction::XclExpChTrAction( const XclExpChTrAction& rCopy ) :
    ExcRecord( rCopy ),
    sUsername( rCopy.sUsername ),
    aDateTime( rCopy.aDateTime ),
    nIndex( 0 ),
    pAddAction( 0 ),
    bAccepted( rCopy.bAccepted ),
    rTabInfo( rCopy.rTabInfo ),
    rIdBuffer( rCopy.rIdBuffer ),
    nLength( rCopy.nLength ),
    nOpCode( rCopy.nOpCode ),
    bForceInfo( rCopy.bForceInfo )
{
}

XclExpChTrAction::XclExpChTrAction(
        const ScChangeAction& rAction,
        const XclExpRoot& rRoot,
        const XclExpChTrTabIdBuffer& rTabIdBuffer,
        sal_uInt16 nNewOpCode ) :
    sUsername( rAction.GetUser() ),
    aDateTime( rAction.GetDateTime() ),
    nIndex( 0 ),
    pAddAction( NULL ),
    bAccepted( rAction.IsAccepted() ),
    rTabInfo( rRoot.GetTabInfo() ),
    rIdBuffer( rTabIdBuffer ),
    nLength( 0 ),
    nOpCode( nNewOpCode ),
    bForceInfo( false )
{
    aDateTime.SetSec( 0 );
    aDateTime.Set100Sec( 0 );
}

XclExpChTrAction::~XclExpChTrAction()
{
    if( pAddAction )
        delete pAddAction;
}

void XclExpChTrAction::SetAddAction( XclExpChTrAction* pAction )
{
    if( pAddAction )
        pAddAction->SetAddAction( pAction );
    else
        pAddAction = pAction;
}

void XclExpChTrAction::AddDependentContents(
        const ScChangeAction& rAction,
        const XclExpRoot& rRoot,
        ScChangeTrack& rChangeTrack )
{
    ScChangeActionMap aActionMap;
    ScChangeActionMap::iterator itChangeAction;

    rChangeTrack.GetDependents( (ScChangeAction*)(&rAction), aActionMap );
    for( itChangeAction = aActionMap.begin(); itChangeAction != aActionMap.end(); ++itChangeAction )
        if( itChangeAction->second->GetType() == SC_CAT_CONTENT )
            SetAddAction( new XclExpChTrCellContent(
                *((const ScChangeActionContent*) itChangeAction->second), rRoot, rIdBuffer ) );
}

void XclExpChTrAction::SetIndex( sal_uInt32& rIndex )
{
    nIndex = rIndex++;
}

void XclExpChTrAction::SaveCont( XclExpStream& rStrm )
{
    OSL_ENSURE( nOpCode != EXC_CHTR_OP_UNKNOWN, "XclExpChTrAction::SaveCont - unknown action" );
    rStrm   << nLength
            << nIndex
            << nOpCode
            << (sal_uInt16)(bAccepted ? EXC_CHTR_ACCEPT : EXC_CHTR_NOTHING);
    SaveActionData( rStrm );
}

void XclExpChTrAction::PrepareSaveAction( XclExpStream& /*rStrm*/ ) const
{
}

void XclExpChTrAction::CompleteSaveAction( XclExpStream& /*rStrm*/ ) const
{
}

void XclExpChTrAction::Save( XclExpStream& rStrm )
{
    PrepareSaveAction( rStrm );
    ExcRecord::Save( rStrm );
    if( pAddAction )
        pAddAction->Save( rStrm );
    CompleteSaveAction( rStrm );
}

sal_Size XclExpChTrAction::GetLen() const
{
    return GetHeaderByteCount() + GetActionByteCount();
}

//___________________________________________________________________

XclExpChTrData::XclExpChTrData() :
    pString( NULL ),
    mpFormulaCell( NULL ),
    fValue( 0.0 ),
    nRKValue( 0 ),
    nType( EXC_CHTR_TYPE_EMPTY ),
    nSize( 0 )
{
}

XclExpChTrData::~XclExpChTrData()
{
    Clear();
}

void XclExpChTrData::Clear()
{
    DELETEZ( pString );
    mpFormulaCell = NULL;
    mxTokArr.reset();
    maRefLog.clear();
    fValue = 0.0;
    nRKValue = 0;
    nType = EXC_CHTR_TYPE_EMPTY;
    nSize = 0;
}

void XclExpChTrData::WriteFormula( XclExpStream& rStrm, const XclExpChTrTabIdBuffer& rTabIdBuffer )
{
    OSL_ENSURE( mxTokArr && !mxTokArr->Empty(), "XclExpChTrData::Write - no formula" );
    rStrm << *mxTokArr;

    for( XclExpRefLog::const_iterator aIt = maRefLog.begin(), aEnd = maRefLog.end(); aIt != aEnd; ++aIt )
    {
        if( aIt->mpUrl && aIt->mpFirstTab )
        {
            rStrm << *aIt->mpUrl << (sal_uInt8) 0x01 << *aIt->mpFirstTab << (sal_uInt8) 0x02;
        }
        else
        {
            bool bSingleTab = aIt->mnFirstXclTab == aIt->mnLastXclTab;
            rStrm.SetSliceSize( bSingleTab ? 6 : 8 );
            rStrm << (sal_uInt8) 0x01 << (sal_uInt8) 0x02 << (sal_uInt8) 0x00;
            rStrm << rTabIdBuffer.GetId( aIt->mnFirstXclTab );
            if( bSingleTab )
                rStrm << (sal_uInt8) 0x02;
            else
                rStrm << (sal_uInt8) 0x00 << rTabIdBuffer.GetId( aIt->mnLastXclTab );
        }
    }
    rStrm.SetSliceSize( 0 );
    rStrm << (sal_uInt8) 0x00;
}

void XclExpChTrData::Write( XclExpStream& rStrm, const XclExpChTrTabIdBuffer& rTabIdBuffer )
{
    switch( nType )
    {
        case EXC_CHTR_TYPE_RK:
            rStrm << nRKValue;
        break;
        case EXC_CHTR_TYPE_DOUBLE:
            rStrm << fValue;
        break;
        case EXC_CHTR_TYPE_STRING:
            OSL_ENSURE( pString, "XclExpChTrData::Write - no string" );
            rStrm << *pString;
        break;
        case EXC_CHTR_TYPE_FORMULA:
            WriteFormula( rStrm, rTabIdBuffer );
        break;
    }
}

//___________________________________________________________________

XclExpChTrCellContent::XclExpChTrCellContent(
        const ScChangeActionContent& rAction,
        const XclExpRoot& rRoot,
        const XclExpChTrTabIdBuffer& rTabIdBuffer ) :
    XclExpChTrAction( rAction, rRoot, rTabIdBuffer, EXC_CHTR_OP_CELL ),
    XclExpRoot( rRoot ),
    pOldData( 0 ),
    pNewData( 0 ),
    aPosition( rAction.GetBigRange().MakeRange().aStart )
{
    sal_uInt32 nDummy32;
    sal_uInt16 nDummy16;
    GetCellData( rRoot, rAction.GetOldCell(), pOldData, nDummy32, nOldLength );
    GetCellData( rRoot, rAction.GetNewCell(), pNewData, nLength, nDummy16 );
}

XclExpChTrCellContent::~XclExpChTrCellContent()
{
    if( pOldData )
        delete pOldData;
    if( pNewData )
        delete pNewData;
}

void XclExpChTrCellContent::MakeEmptyChTrData( XclExpChTrData*& rpData )
{
    if( rpData )
        rpData->Clear();
    else
        rpData = new XclExpChTrData;
}

void XclExpChTrCellContent::GetCellData(
        const XclExpRoot& rRoot,
        const ScBaseCell* pScCell,
        XclExpChTrData*& rpData,
        sal_uInt32& rXclLength1,
        sal_uInt16& rXclLength2 )
{
    MakeEmptyChTrData( rpData );
    rXclLength1 = 0x0000003A;
    rXclLength2 = 0x0000;

    if( !pScCell )
    {
        delete rpData;
        rpData = NULL;
        return;
    }

    switch( pScCell->GetCellType() )
    {
        case CELLTYPE_VALUE:
        {
            rpData->fValue = ((const ScValueCell*) pScCell)->GetValue();
            if( XclTools::GetRKFromDouble( rpData->nRKValue, rpData->fValue ) )
            {
                rpData->nType = EXC_CHTR_TYPE_RK;
                rpData->nSize = 4;
                rXclLength1 = 0x0000003E;
                rXclLength2 = 0x0004;
            }
            else
            {
                rpData->nType = EXC_CHTR_TYPE_DOUBLE;
                rpData->nSize = 8;
                rXclLength1 = 0x00000042;
                rXclLength2 = 0x0008;
            }
        }
        break;
        case CELLTYPE_STRING:
        case CELLTYPE_EDIT:
        {
            String sCellStr;
            if( pScCell->GetCellType() == CELLTYPE_STRING )
            {
                const ScStringCell* pStrCell = static_cast< const ScStringCell* >( pScCell );
                sCellStr = pStrCell->GetString();
                rpData->mpFormattedString = XclExpStringHelper::CreateCellString( rRoot,
                        *pStrCell, NULL );
            }
            else
            {
                const ScEditCell* pEditCell = static_cast< const ScEditCell* >( pScCell );
                sCellStr = pEditCell->GetString();
                XclExpHyperlinkHelper aLinkHelper( rRoot, aPosition );
                rpData->mpFormattedString = XclExpStringHelper::CreateCellString( rRoot,
                        *pEditCell, NULL, aLinkHelper );
            }
            rpData->pString = new XclExpString( sCellStr, EXC_STR_DEFAULT, 32766 );
            rpData->nType = EXC_CHTR_TYPE_STRING;
            rpData->nSize = 3 + rpData->pString->GetSize();
            rXclLength1 = 64 + (sCellStr.Len() << 1);
            rXclLength2 = 6 + (sal_uInt16)(sCellStr.Len() << 1);
        }
        break;
        case CELLTYPE_FORMULA:
        {
            const ScFormulaCell* pFmlCell = (const ScFormulaCell*) pScCell;
            rpData->mpFormulaCell = pFmlCell;

            const ScTokenArray* pTokenArray = pFmlCell->GetCode();
            if( pTokenArray )
            {
                XclExpRefLog& rRefLog = rpData->maRefLog;
                rpData->mxTokArr = GetFormulaCompiler().CreateFormula(
                    EXC_FMLATYPE_CELL, *pTokenArray, &pFmlCell->aPos, &rRefLog );
                rpData->nType = EXC_CHTR_TYPE_FORMULA;
                sal_Size nSize = rpData->mxTokArr->GetSize() + 3;

                for( XclExpRefLog::const_iterator aIt = rRefLog.begin(), aEnd = rRefLog.end(); aIt != aEnd; ++aIt )
                {
                    if( aIt->mpUrl && aIt->mpFirstTab )
                        nSize += aIt->mpUrl->GetSize() + aIt->mpFirstTab->GetSize() + 2;
                    else
                        nSize += (aIt->mnFirstXclTab == aIt->mnLastXclTab) ? 6 : 8;
                }
                rpData->nSize = ::std::min< sal_Size >( nSize, 0xFFFF );
                rXclLength1 = 0x00000052;
                rXclLength2 = 0x0018;
            }
        }
        break;
        default:;
    }
}

void XclExpChTrCellContent::SaveActionData( XclExpStream& rStrm ) const
{
    WriteTabId( rStrm, aPosition.Tab() );
    rStrm   << (sal_uInt16)((pOldData ? (pOldData->nType << 3) : 0x0000) | (pNewData ? pNewData->nType : 0x0000))
            << (sal_uInt16) 0x0000;
    Write2DAddress( rStrm, aPosition );
    rStrm   << nOldLength
            << (sal_uInt32) 0x00000000;
    if( pOldData )
        pOldData->Write( rStrm, rIdBuffer );
    if( pNewData )
        pNewData->Write( rStrm, rIdBuffer );
}

sal_uInt16 XclExpChTrCellContent::GetNum() const
{
    return 0x013B;
}

sal_Size XclExpChTrCellContent::GetActionByteCount() const
{
    sal_Size nLen = 16;
    if( pOldData )
        nLen += pOldData->nSize;
    if( pNewData )
        nLen += pNewData->nSize;
    return nLen;
}

static const char* lcl_GetType( XclExpChTrData* pData )
{
    switch( pData->nType )
    {
    case EXC_CHTR_TYPE_RK:
    case EXC_CHTR_TYPE_DOUBLE:
        return "n";
        break;
    case EXC_CHTR_TYPE_FORMULA:
        {
            ScFormulaCell* pFormulaCell = const_cast< ScFormulaCell* >( pData->mpFormulaCell );
            const char* sType;
            OUString sValue;
            XclXmlUtils::GetFormulaTypeAndValue( *pFormulaCell, sType, sValue );
            return sType;
        }
        break;
    case EXC_CHTR_TYPE_STRING:
        return "inlineStr";
        break;
    default:
        break;
    }
    return "*unknown*";
}

static void lcl_WriteCell( XclExpXmlStream& rStrm, sal_Int32 nElement, const ScAddress& rPosition, XclExpChTrData* pData )
{
    sax_fastparser::FSHelperPtr pStream = rStrm.GetCurrentStream();

    pStream->startElement( nElement,
            XML_r,  XclXmlUtils::ToOString( rPosition ).getStr(),
            XML_s,  NULL,   // OOXTODO: not supported
            XML_t,  lcl_GetType( pData ),
            XML_cm, NULL,   // OOXTODO: not supported
            XML_vm, NULL,   // OOXTODO: not supported
            XML_ph, NULL,   // OOXTODO: not supported
            FSEND );
    switch( pData->nType )
    {
        case EXC_CHTR_TYPE_RK:
        case EXC_CHTR_TYPE_DOUBLE:
            pStream->startElement( XML_v, FSEND );
            pStream->write( pData->fValue );
            pStream->endElement( XML_v );
            break;
        case EXC_CHTR_TYPE_FORMULA:
            pStream->startElement( XML_f,
                    // OOXTODO: other attributes?  see XclExpFormulaCell::SaveXml()
                    FSEND );
            pStream->writeEscaped( XclXmlUtils::ToOUString(
                        *pData->mpFormulaCell->GetDocument(),
                        pData->mpFormulaCell->aPos, pData->mpFormulaCell->GetCode(),
                        rStrm.GetRoot().GetOpCodeMap() ) );
            pStream->endElement( XML_f );
            break;
        case EXC_CHTR_TYPE_STRING:
            pStream->startElement( XML_is, FSEND );
            if( pData->mpFormattedString )
                pData->mpFormattedString->WriteXml( rStrm );
            else
                pData->pString->WriteXml( rStrm );
            pStream->endElement( XML_is );
            break;
        default:
            // ignore
            break;
    }
    pStream->endElement( nElement );
}

void XclExpChTrCellContent::SaveXml( XclExpXmlStream& rRevisionLogStrm )
{
    sax_fastparser::FSHelperPtr pStream = rRevisionLogStrm.GetCurrentStream();
    pStream->startElement( XML_rcc,
            XML_rId,                    OString::valueOf( (sal_Int32) GetActionNumber() ).getStr(),
            XML_ua,                     XclXmlUtils::ToPsz( GetAccepted () ),   // OOXTODO? bAccepted == ua or ra; not sure.
            XML_ra,                     NULL,       // OOXTODO: RRD.fUndoAction?  Or RRD.fAccepted?
            XML_sId,                    OString::valueOf( (sal_Int32) GetTabId( aPosition.Tab() ) ).getStr(),
            XML_odxf,                   NULL,       // OOXTODO: not supported
            XML_xfDxf,                  NULL,       // OOXTODO: not supported
            XML_s,                      NULL,       // OOXTODO: not supported
            XML_dxf,                    NULL,       // OOXTODO: not supported
            XML_numFmtId,               NULL,       // OOXTODO: not supported
            XML_quotePrefix,            NULL,       // OOXTODO: not supported
            XML_oldQuotePrefix,         NULL,       // OOXTODO: not supported
            XML_ph,                     NULL,       // OOXTODO: not supported
            XML_oldPh,                  NULL,       // OOXTODO: not supported
            XML_endOfListFormulaUpdate, NULL,       // OOXTODO: not supported
            FSEND );
    if( pOldData )
    {
        lcl_WriteCell( rRevisionLogStrm, XML_oc, aPosition, pOldData );
    }
    if( pNewData )
    {
        lcl_WriteCell( rRevisionLogStrm, XML_nc, aPosition, pNewData );
    }
    // OOXTODO: XML_odxf, XML_ndxf, XML_extLst elements
    pStream->endElement( XML_rcc );
}

//___________________________________________________________________

XclExpChTrInsert::XclExpChTrInsert(
        const ScChangeAction& rAction,
        const XclExpRoot& rRoot,
        const XclExpChTrTabIdBuffer& rTabIdBuffer,
        ScChangeTrack& rChangeTrack ) :
    XclExpChTrAction( rAction, rRoot, rTabIdBuffer ),
    aRange( rAction.GetBigRange().MakeRange() )
{
    nLength = 0x00000030;
    switch( rAction.GetType() )
    {
        case SC_CAT_INSERT_COLS:    nOpCode = EXC_CHTR_OP_INSCOL;   break;
        case SC_CAT_INSERT_ROWS:    nOpCode = EXC_CHTR_OP_INSROW;   break;
        case SC_CAT_DELETE_COLS:    nOpCode = EXC_CHTR_OP_DELCOL;   break;
        case SC_CAT_DELETE_ROWS:    nOpCode = EXC_CHTR_OP_DELROW;   break;
        default:
            OSL_FAIL( "XclExpChTrInsert::XclExpChTrInsert - unknown action" );
    }

    if( nOpCode & EXC_CHTR_OP_COLFLAG )
    {
        aRange.aStart.SetRow( 0 );
        aRange.aEnd.SetRow( rRoot.GetXclMaxPos().Row() );
    }
    else
    {
        aRange.aStart.SetCol( 0 );
        aRange.aEnd.SetCol( rRoot.GetXclMaxPos().Col() );
    }

    if( nOpCode & EXC_CHTR_OP_DELFLAG )
    {
        SetAddAction( new XclExpChTr0x014A( *this ) );
        AddDependentContents( rAction, rRoot, rChangeTrack );
    }
}

XclExpChTrInsert::~XclExpChTrInsert()
{
}

void XclExpChTrInsert::SaveActionData( XclExpStream& rStrm ) const
{
    WriteTabId( rStrm, aRange.aStart.Tab() );
    rStrm   << (sal_uInt16) 0x0000;
    Write2DRange( rStrm, aRange );
    rStrm   << (sal_uInt32) 0x00000000;
}

void XclExpChTrInsert::PrepareSaveAction( XclExpStream& rStrm ) const
{
    if( (nOpCode == EXC_CHTR_OP_DELROW) || (nOpCode == EXC_CHTR_OP_DELCOL) )
        XclExpChTrEmpty( 0x0150 ).Save( rStrm );
}

void XclExpChTrInsert::CompleteSaveAction( XclExpStream& rStrm ) const
{
    if( (nOpCode == EXC_CHTR_OP_DELROW) || (nOpCode == EXC_CHTR_OP_DELCOL) )
        XclExpChTrEmpty( 0x0151 ).Save( rStrm );
}

sal_uInt16 XclExpChTrInsert::GetNum() const
{
    return 0x0137;
}

sal_Size XclExpChTrInsert::GetActionByteCount() const
{
    return 16;
}

static const char* lcl_GetAction( sal_uInt16 nOpCode )
{
    switch( nOpCode )
    {
        case EXC_CHTR_OP_INSCOL:    return "insertCol";
        case EXC_CHTR_OP_INSROW:    return "insertRow";
        case EXC_CHTR_OP_DELCOL:    return "deleteCol";
        case EXC_CHTR_OP_DELROW:    return "deleteRow";
        default:                    return "*unknown*";
    }
}

void XclExpChTrInsert::SaveXml( XclExpXmlStream& rRevisionLogStrm )
{
    sax_fastparser::FSHelperPtr pStream = rRevisionLogStrm.GetCurrentStream();
    pStream->startElement( XML_rrc,
            XML_rId,    OString::valueOf( (sal_Int32) GetActionNumber() ).getStr(),
            XML_ua,     XclXmlUtils::ToPsz( GetAccepted () ),   // OOXTODO? bAccepted == ua or ra; not sure.
            XML_ra,     NULL,       // OOXTODO: RRD.fUndoAction?  Or RRD.fAccepted?
            XML_sId,    OString::valueOf( (sal_Int32) GetTabId( aRange.aStart.Tab() ) ).getStr(),
            XML_eol,    NULL,       // OOXTODO: not supported?
            XML_ref,    XclXmlUtils::ToOString( aRange ).getStr(),
            XML_action, lcl_GetAction( nOpCode ),
            XML_edge,   NULL,       // OOXTODO: ???
            FSEND );
    // OOXTODO: does this handle XML_rfmt, XML_undo?
    XclExpChTrAction* pAction = GetAddAction();
    while( pAction != NULL )
    {
        pAction->SaveXml( rRevisionLogStrm );
        pAction = pAction->GetAddAction();
    }
    pStream->endElement( XML_rrc );
}

//___________________________________________________________________

XclExpChTrInsertTab::XclExpChTrInsertTab(
        const ScChangeAction& rAction,
        const XclExpRoot& rRoot,
        const XclExpChTrTabIdBuffer& rTabIdBuffer ) :
    XclExpChTrAction( rAction, rRoot, rTabIdBuffer, EXC_CHTR_OP_INSTAB ),
    XclExpRoot( rRoot ),
    nTab( (SCTAB) rAction.GetBigRange().aStart.Tab() )
{
    nLength = 0x0000021C;
    bForceInfo = sal_True;
}

XclExpChTrInsertTab::~XclExpChTrInsertTab()
{
}

void XclExpChTrInsertTab::SaveActionData( XclExpStream& rStrm ) const
{
    WriteTabId( rStrm, nTab );
    rStrm << sal_uInt32( 0 );
    lcl_WriteFixedString( rStrm, XclExpString( GetTabInfo().GetScTabName( nTab ) ), 127 );
    lcl_WriteDateTime( rStrm, GetDateTime() );
    rStrm.WriteZeroBytes( 133 );
}

sal_uInt16 XclExpChTrInsertTab::GetNum() const
{
    return 0x014D;
}

sal_Size XclExpChTrInsertTab::GetActionByteCount() const
{
    return 276;
}

void XclExpChTrInsertTab::SaveXml( XclExpXmlStream& rStrm )
{
    sax_fastparser::FSHelperPtr pStream = rStrm.GetCurrentStream();
    pStream->singleElement( XML_ris,
            XML_rId,            OString::valueOf( (sal_Int32) GetActionNumber() ).getStr(),
            XML_ua,             XclXmlUtils::ToPsz( GetAccepted () ),   // OOXTODO? bAccepted == ua or ra; not sure.
            XML_ra,             NULL,       // OOXTODO: RRD.fUndoAction?  Or RRD.fAccepted?
            XML_sheetId,        OString::valueOf( (sal_Int32) GetTabId( nTab ) ).getStr(),
            XML_name,           XclXmlUtils::ToOString( GetTabInfo().GetScTabName( nTab ) ).getStr(),
            XML_sheetPosition,  OString::valueOf( (sal_Int32) nTab ).getStr(),
            FSEND );
}

//___________________________________________________________________

XclExpChTrMoveRange::XclExpChTrMoveRange(
        const ScChangeActionMove& rAction,
        const XclExpRoot& rRoot,
        const XclExpChTrTabIdBuffer& rTabIdBuffer,
        ScChangeTrack& rChangeTrack ) :
    XclExpChTrAction( rAction, rRoot, rTabIdBuffer, EXC_CHTR_OP_MOVE ),
    aDestRange( rAction.GetBigRange().MakeRange() )
{
    nLength = 0x00000042;
    aSourceRange = aDestRange;
    sal_Int32 nDCols, nDRows, nDTabs;
    rAction.GetDelta( nDCols, nDRows, nDTabs );
    aSourceRange.aStart.IncRow( (SCROW) -nDRows );
    aSourceRange.aStart.IncCol( (SCCOL) -nDCols );
    aSourceRange.aStart.IncTab( (SCTAB) -nDTabs );
    aSourceRange.aEnd.IncRow( (SCROW) -nDRows );
    aSourceRange.aEnd.IncCol( (SCCOL) -nDCols );
    aSourceRange.aEnd.IncTab( (SCTAB) -nDTabs );
    AddDependentContents( rAction, rRoot, rChangeTrack );
}

XclExpChTrMoveRange::~XclExpChTrMoveRange()
{
}

void XclExpChTrMoveRange::SaveActionData( XclExpStream& rStrm ) const
{
    WriteTabId( rStrm, aDestRange.aStart.Tab() );
    Write2DRange( rStrm, aSourceRange );
    Write2DRange( rStrm, aDestRange );
    WriteTabId( rStrm, aSourceRange.aStart.Tab() );
    rStrm << (sal_uInt32) 0x00000000;
}

void XclExpChTrMoveRange::PrepareSaveAction( XclExpStream& rStrm ) const
{
    XclExpChTrEmpty( 0x014E ).Save( rStrm );
}

void XclExpChTrMoveRange::CompleteSaveAction( XclExpStream& rStrm ) const
{
    XclExpChTrEmpty( 0x014F ).Save( rStrm );
}

sal_uInt16 XclExpChTrMoveRange::GetNum() const
{
    return 0x0140;
}

sal_Size XclExpChTrMoveRange::GetActionByteCount() const
{
    return 24;
}

void XclExpChTrMoveRange::SaveXml( XclExpXmlStream& rRevisionLogStrm )
{
    sax_fastparser::FSHelperPtr pStream = rRevisionLogStrm.GetCurrentStream();

    pStream->startElement( XML_rm,
            XML_rId,            OString::valueOf( (sal_Int32) GetActionNumber() ).getStr(),
            XML_ua,             XclXmlUtils::ToPsz( GetAccepted () ),   // OOXTODO? bAccepted == ua or ra; not sure.
            XML_ra,             NULL,       // OOXTODO: RRD.fUndoAction?  Or RRD.fAccepted?
            XML_sheetId,        OString::valueOf( (sal_Int32) GetTabId( aDestRange.aStart.Tab() ) ).getStr(),
            XML_source,         XclXmlUtils::ToOString( aSourceRange ).getStr(),
            XML_destination,    XclXmlUtils::ToOString( aDestRange ).getStr(),
            XML_sourceSheetId,  OString::valueOf( (sal_Int32) GetTabId( aSourceRange.aStart.Tab() ) ).getStr(),
            FSEND );
    // OOXTODO: does this handle XML_rfmt, XML_undo?
    XclExpChTrAction* pAction = GetAddAction();
    while( pAction != NULL )
    {
        pAction->SaveXml( rRevisionLogStrm );
        pAction = pAction->GetAddAction();
    }
    pStream->endElement( XML_rm );
}

//___________________________________________________________________

XclExpChTr0x014A::XclExpChTr0x014A( const XclExpChTrInsert& rAction ) :
    XclExpChTrInsert( rAction )
{
    nLength = 0x00000026;
    nOpCode = EXC_CHTR_OP_FORMAT;
}

XclExpChTr0x014A::~XclExpChTr0x014A()
{
}

void XclExpChTr0x014A::SaveActionData( XclExpStream& rStrm ) const
{
    WriteTabId( rStrm, aRange.aStart.Tab() );
    rStrm   << (sal_uInt16) 0x0003
            << (sal_uInt16) 0x0001;
    Write2DRange( rStrm, aRange );
}

sal_uInt16 XclExpChTr0x014A::GetNum() const
{
    return 0x014A;
}

sal_Size XclExpChTr0x014A::GetActionByteCount() const
{
    return 14;
}

void XclExpChTr0x014A::SaveXml( XclExpXmlStream& rStrm )
{
    sax_fastparser::FSHelperPtr pStream = rStrm.GetCurrentStream();

    pStream->startElement( XML_rfmt,
            XML_sheetId,    OString::valueOf( (sal_Int32) GetTabId( aRange.aStart.Tab() ) ).getStr(),
            XML_xfDxf,      NULL,   // OOXTODO: not supported
            XML_s,          NULL,   // OOXTODO: style
            XML_sqref,      XclXmlUtils::ToOString( aRange ).getStr(),
            XML_start,      NULL,   // OOXTODO: for string changes
            XML_length,     NULL,   // OOXTODO: for string changes
            FSEND );
    // OOXTODO: XML_dxf, XML_extLst

    pStream->endElement( XML_rfmt );
}

//___________________________________________________________________

class ExcXmlRecord : public ExcRecord
{
public:
    virtual sal_Size    GetLen() const;
    virtual sal_uInt16  GetNum() const;
    virtual void        Save( XclExpStream& rStrm );
    virtual void        SaveXml( XclExpXmlStream& rStrm ) = 0;
};

sal_Size ExcXmlRecord::GetLen() const
{
    return 0;
}

sal_uInt16 ExcXmlRecord::GetNum() const
{
    return 0;
}

void ExcXmlRecord::Save( XclExpStream& )
{
    // Do nothing; ignored for BIFF output.
}

class StartXmlElement : public ExcXmlRecord
{
public:
    enum Behavior {
        CLOSE_ELEMENT       = 0x1,
        WRITE_NAMESPACES    = 0x2,
    };
                        StartXmlElement( sal_Int32 nElement, sal_Int32 eBehavior )
                            : mnElement( nElement ), meBehavior( (Behavior) eBehavior ) {}
    virtual void        SaveXml( XclExpXmlStream& rStrm );
private:
    sal_Int32           mnElement;
    Behavior            meBehavior;
};

void StartXmlElement::SaveXml( XclExpXmlStream& rStrm )
{
    sax_fastparser::FSHelperPtr pStream = rStrm.GetCurrentStream();
    pStream->write( "<" )
        ->writeId( mnElement );
    if( meBehavior & WRITE_NAMESPACES )
    {
        rStrm.WriteAttributes(
                XML_xmlns,                  "http://schemas.openxmlformats.org/spreadsheetml/2006/main",
                FSNS( XML_xmlns, XML_r ),   "http://schemas.openxmlformats.org/officeDocument/2006/relationships",
                FSEND );
    }
    if( meBehavior & CLOSE_ELEMENT )
    {
        pStream->write( ">" );
    }
}

class EndXmlElement : public ExcXmlRecord
{
    sal_Int32           mnElement;
public:
                        EndXmlElement( sal_Int32 nElement ) : mnElement( nElement) {}
    virtual void        SaveXml( XclExpXmlStream& rStrm );
};

void EndXmlElement::SaveXml( XclExpXmlStream& rStrm )
{
    rStrm.GetCurrentStream()->endElement( mnElement );
}

class EndHeaderElement : public EndXmlElement
{
public:
                        EndHeaderElement() : EndXmlElement( XML_header ) {}
    virtual void        SaveXml( XclExpXmlStream& rStrm );
};

void EndHeaderElement::SaveXml( XclExpXmlStream& rStrm )
{
    // Remove the `xl/revisions/revisionLogX.xml' file from the stack
    rStrm.PopStream();

    EndXmlElement::SaveXml( rStrm );
}

XclExpChangeTrack::XclExpChangeTrack( const XclExpRoot& rRoot ) :
    XclExpRoot( rRoot ),
    aActionStack(),
    pTabIdBuffer( NULL ),
    pTempDoc( NULL ),
    pHeader( NULL ),
    bValidGUID( false )
{
    OSL_ENSURE( GetOldRoot().pTabId, "XclExpChangeTrack::XclExpChangeTrack - root data incomplete" );
    if( !GetOldRoot().pTabId )
        return;

    ScChangeTrack* pTempChangeTrack = CreateTempChangeTrack();
    if (!pTempChangeTrack)
        return;

    pTabIdBuffer = new XclExpChTrTabIdBuffer( GetTabInfo().GetXclTabCount() );
    maBuffers.push_back( pTabIdBuffer );

    // calculate final table order (tab id list)
    const ScChangeAction* pScAction;
    for( pScAction = pTempChangeTrack->GetLast(); pScAction; pScAction = pScAction->GetPrev() )
    {
        if( pScAction->GetType() == SC_CAT_INSERT_TABS )
        {
            SCTAB nScTab = static_cast< SCTAB >( pScAction->GetBigRange().aStart.Tab() );
            pTabIdBuffer->InitFill( GetTabInfo().GetXclTab( nScTab ) );
        }
    }
    pTabIdBuffer->InitFillup();
    GetOldRoot().pTabId->Copy( *pTabIdBuffer );

    // get actions in reverse order
    pScAction = pTempChangeTrack->GetLast();
    while( pScAction )
    {
        PushActionRecord( *pScAction );
        const ScChangeAction* pPrevAction = pScAction->GetPrev();
        pTempChangeTrack->Undo( pScAction->GetActionNumber(), pScAction->GetActionNumber() );
        pScAction = pPrevAction;
    }

    // build record list
    pHeader = new XclExpChTrHeader;
    aRecList.push_back( new StartXmlElement( XML_headers, StartXmlElement::WRITE_NAMESPACES ) );
    aRecList.push_back( pHeader );
    aRecList.push_back( new XclExpChTr0x0195 );
    aRecList.push_back( new XclExpChTr0x0194( *pTempChangeTrack ) );

    String sLastUsername;
    DateTime aLastDateTime( DateTime::EMPTY );
    sal_uInt32 nIndex = 1;
    sal_Int32 nLogNumber = 1;
    while( !aActionStack.empty() )
    {
        XclExpChTrAction* pAction = aActionStack.top();
        aActionStack.pop();

        if( (nIndex == 1) || pAction->ForceInfoRecord() ||
            (pAction->GetUsername() != sLastUsername) ||
            (pAction->GetDateTime() != aLastDateTime) )
        {
            if( nIndex != 1 )
            {
                aRecList.push_back( new EndXmlElement( XML_revisions ) );
                aRecList.push_back( new EndHeaderElement() );
            }

            lcl_GenerateGUID( aGUID, bValidGUID );
            sLastUsername = pAction->GetUsername();
            aLastDateTime = pAction->GetDateTime();

            aRecList.push_back( new StartXmlElement( XML_header, 0 ) );
            aRecList.push_back( new XclExpChTrInfo( sLastUsername, aLastDateTime, aGUID, nLogNumber++ ) );
            aRecList.push_back( new XclExpChTrTabId( pAction->GetTabIdBuffer(), true ) );
            aRecList.push_back( new StartXmlElement( XML_revisions, StartXmlElement::WRITE_NAMESPACES | StartXmlElement::CLOSE_ELEMENT ) );
            pHeader->SetGUID( aGUID );
        }
        pAction->SetIndex( nIndex );
        aRecList.push_back( pAction );
    }

    pHeader->SetGUID( aGUID );
    pHeader->SetCount( nIndex - 1 );
    if( nLogNumber > 1 )
    {
        aRecList.push_back( new EndXmlElement( XML_revisions ) );
        aRecList.push_back( new EndHeaderElement() );
    }
    aRecList.push_back( new EndXmlElement( XML_headers ) );
    aRecList.push_back( new ExcEof );
}

XclExpChangeTrack::~XclExpChangeTrack()
{
    std::vector<ExcRecord*>::iterator prIter;
    for ( prIter = aRecList.begin(); prIter != aRecList.end(); ++prIter )
        delete *prIter;

    std::vector<XclExpChTrTabIdBuffer*>::iterator pIter;
    for ( pIter = maBuffers.begin(); pIter != maBuffers.end(); ++pIter )
        delete *pIter;

    while( !aActionStack.empty() )
    {
        delete aActionStack.top();
        aActionStack.pop();
    }

    if( pTempDoc )
        delete pTempDoc;
}

ScChangeTrack* XclExpChangeTrack::CreateTempChangeTrack()
{
    // get original change track
    ScChangeTrack* pOrigChangeTrack = GetDoc().GetChangeTrack();
    OSL_ENSURE( pOrigChangeTrack, "XclExpChangeTrack::CreateTempChangeTrack - no change track data" );
    if( !pOrigChangeTrack )
        return NULL;

    // create empty document
    pTempDoc = new ScDocument;
    OSL_ENSURE( pTempDoc, "XclExpChangeTrack::CreateTempChangeTrack - no temp document" );
    if( !pTempDoc )
        return NULL;

    // adjust table count
    SCTAB nOrigCount = GetDoc().GetTableCount();
    rtl::OUString sTabName;
    for( sal_Int32 nIndex = 0; nIndex < nOrigCount; nIndex++ )
    {
        pTempDoc->CreateValidTabName( sTabName );
        pTempDoc->InsertTab( SC_TAB_APPEND, sTabName );
    }
    OSL_ENSURE( nOrigCount == pTempDoc->GetTableCount(),
        "XclExpChangeTrack::CreateTempChangeTrack - table count mismatch" );
    if( nOrigCount != pTempDoc->GetTableCount() )
        return NULL;

    return pOrigChangeTrack->Clone(pTempDoc);
}

void XclExpChangeTrack::PushActionRecord( const ScChangeAction& rAction )
{
    XclExpChTrAction* pXclAction = NULL;
    ScChangeTrack* pTempChangeTrack = pTempDoc->GetChangeTrack();
    switch( rAction.GetType() )
    {
        case SC_CAT_CONTENT:
            pXclAction = new XclExpChTrCellContent( (const ScChangeActionContent&) rAction, GetRoot(), *pTabIdBuffer );
        break;
        case SC_CAT_INSERT_ROWS:
        case SC_CAT_INSERT_COLS:
        case SC_CAT_DELETE_ROWS:
        case SC_CAT_DELETE_COLS:
            if (pTempChangeTrack)
                pXclAction = new XclExpChTrInsert( rAction, GetRoot(), *pTabIdBuffer, *pTempChangeTrack );
        break;
        case SC_CAT_INSERT_TABS:
        {
            pXclAction = new XclExpChTrInsertTab( rAction, GetRoot(), *pTabIdBuffer );
            XclExpChTrTabIdBuffer* pNewBuffer = new XclExpChTrTabIdBuffer( *pTabIdBuffer );
            pNewBuffer->Remove();
            maBuffers.push_back( pNewBuffer );
            pTabIdBuffer = pNewBuffer;
        }
        break;
        case SC_CAT_MOVE:
            if (pTempChangeTrack)
                pXclAction = new XclExpChTrMoveRange( (const ScChangeActionMove&) rAction, GetRoot(), *pTabIdBuffer, *pTempChangeTrack );
        break;
        default:;
    }
    if( pXclAction )
        aActionStack.push( pXclAction );
}

sal_Bool XclExpChangeTrack::WriteUserNamesStream()
{
    sal_Bool bRet = false;
    SotStorageStreamRef xSvStrm = OpenStream( EXC_STREAM_USERNAMES );
    OSL_ENSURE( xSvStrm.Is(), "XclExpChangeTrack::WriteUserNamesStream - no stream" );
    if( xSvStrm.Is() )
    {
        XclExpStream aXclStrm( *xSvStrm, GetRoot() );
        XclExpChTr0x0191().Save( aXclStrm );
        XclExpChTr0x0198().Save( aXclStrm );
        XclExpChTr0x0192().Save( aXclStrm );
        XclExpChTr0x0197().Save( aXclStrm );
        xSvStrm->Commit();
        bRet = sal_True;
    }
    return bRet;
}

void XclExpChangeTrack::Write()
{
    if( aRecList.empty() )
        return;

    if( WriteUserNamesStream() )
    {
        SotStorageStreamRef xSvStrm = OpenStream( EXC_STREAM_REVLOG );
        OSL_ENSURE( xSvStrm.Is(), "XclExpChangeTrack::Write - no stream" );
        if( xSvStrm.Is() )
        {
            XclExpStream aXclStrm( *xSvStrm, GetRoot(), EXC_MAXRECSIZE_BIFF8 + 8 );

            std::vector<ExcRecord*>::iterator pIter;
            for ( pIter = aRecList.begin(); pIter != aRecList.end(); ++pIter )
                (*pIter)->Save(aXclStrm);

            xSvStrm->Commit();
        }
    }
}

static void lcl_WriteUserNamesXml( XclExpXmlStream& rWorkbookStrm )
{
    sax_fastparser::FSHelperPtr pUserNames = rWorkbookStrm.CreateOutputStream(
            OUString(RTL_CONSTASCII_USTRINGPARAM( "xl/revisions/userNames.xml" )),
            OUString(RTL_CONSTASCII_USTRINGPARAM( "revisions/userNames.xml" )),
            rWorkbookStrm.GetCurrentStream()->getOutputStream(),
            "application/vnd.openxmlformats-officedocument.spreadsheetml.userNames+xml",
            "http://schemas.openxmlformats.org/officeDocument/2006/relationships/usernames" );
    pUserNames->startElement( XML_users,
            XML_xmlns,                  "http://schemas.openxmlformats.org/spreadsheetml/2006/main",
            FSNS( XML_xmlns, XML_r ),   "http://schemas.openxmlformats.org/officeDocument/2006/relationships",
            XML_count,                  "0",
            FSEND );
    // OOXTODO: XML_userinfo elements for each user editing the file
    //          Doesn't seem to be supported by .xls output either (based on
    //          contents of XclExpChangeTrack::WriteUserNamesStream()).
    pUserNames->endElement( XML_users );
}

void XclExpChangeTrack::WriteXml( XclExpXmlStream& rWorkbookStrm )
{
    if( aRecList.empty() )
        return;

    lcl_WriteUserNamesXml( rWorkbookStrm );

    sax_fastparser::FSHelperPtr pRevisionHeaders = rWorkbookStrm.CreateOutputStream(
            OUString(RTL_CONSTASCII_USTRINGPARAM( "xl/revisions/revisionHeaders.xml" )),
            OUString(RTL_CONSTASCII_USTRINGPARAM( "revisions/revisionHeaders.xml" )),
            rWorkbookStrm.GetCurrentStream()->getOutputStream(),
            "application/vnd.openxmlformats-officedocument.spreadsheetml.revisionHeaders+xml",
            "http://schemas.openxmlformats.org/officeDocument/2006/relationships/revisionHeaders" );
    // OOXTODO: XML_userinfo elements for each user editing the file
    //          Doesn't seem to be supported by .xls output either (based on
    //          contents of XclExpChangeTrack::WriteUserNamesStream()).
    rWorkbookStrm.PushStream( pRevisionHeaders );

    std::vector<ExcRecord*>::iterator pIter;
    for ( pIter = aRecList.begin(); pIter != aRecList.end(); ++pIter )
        (*pIter)->SaveXml(rWorkbookStrm);

    rWorkbookStrm.PopStream();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

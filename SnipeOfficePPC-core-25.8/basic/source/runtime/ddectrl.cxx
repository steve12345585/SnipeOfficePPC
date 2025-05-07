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

#include <tools/errcode.hxx>
#include <svl/svdde.hxx>
#include "ddectrl.hxx"
#include <basic/sberrors.hxx>

#define DDE_FREECHANNEL ((DdeConnection*)0xffffffff)

#define DDE_FIRSTERR    0x4000
#define DDE_LASTERR     0x4011

static const SbError nDdeErrMap[] =
{
    /* DMLERR_ADVACKTIMEOUT       */  0x4000, SbERR_DDE_TIMEOUT,
    /* DMLERR_BUSY                */  0x4001, SbERR_DDE_BUSY,
    /* DMLERR_DATAACKTIMEOUT      */  0x4002, SbERR_DDE_TIMEOUT,
    /* DMLERR_DLL_NOT_INITIALIZED */  0x4003, SbERR_DDE_ERROR,
    /* DMLERR_DLL_USAGE           */  0x4004, SbERR_DDE_ERROR,
    /* DMLERR_EXECACKTIMEOUT      */  0x4005, SbERR_DDE_TIMEOUT,
    /* DMLERR_INVALIDPARAMETER    */  0x4006, SbERR_DDE_ERROR,
    /* DMLERR_LOW_MEMORY          */  0x4007, SbERR_DDE_ERROR,
    /* DMLERR_MEMORY_ERROR        */  0x4008, SbERR_DDE_ERROR,
    /* DMLERR_NOTPROCESSED        */  0x4009, SbERR_DDE_NOTPROCESSED,
    /* DMLERR_NO_CONV_ESTABLISHED */  0x400a, SbERR_DDE_NO_CHANNEL,
    /* DMLERR_POKEACKTIMEOUT      */  0x400b, SbERR_DDE_TIMEOUT,
    /* DMLERR_POSTMSG_FAILED      */  0x400c, SbERR_DDE_QUEUE_OVERFLOW,
    /* DMLERR_REENTRANCY          */  0x400d, SbERR_DDE_ERROR,
    /* DMLERR_SERVER_DIED         */  0x400e, SbERR_DDE_PARTNER_QUIT,
    /* DMLERR_SYS_ERROR           */  0x400f, SbERR_DDE_ERROR,
    /* DMLERR_UNADVACKTIMEOUT     */  0x4010, SbERR_DDE_TIMEOUT,
    /* DMLERR_UNFOUND_QUEUE_ID    */  0x4011, SbERR_DDE_NO_CHANNEL
};

SbError SbiDdeControl::GetLastErr( DdeConnection* pConv )
{
    if( !pConv )
        return 0;
    long nErr = pConv->GetError();
    if( !nErr )
        return 0;
    if( nErr < DDE_FIRSTERR || nErr > DDE_LASTERR )
        return SbERR_DDE_ERROR;
    return nDdeErrMap[ 2*(nErr - DDE_FIRSTERR) + 1 ];
}

IMPL_LINK_INLINE( SbiDdeControl,Data , DdeData*, pData,
{
    aData = String::CreateFromAscii( (char*)(const void*)*pData );
    return 1;
}
)

SbiDdeControl::SbiDdeControl()
{
}

SbiDdeControl::~SbiDdeControl()
{
    TerminateAll();
}

sal_Int16 SbiDdeControl::GetFreeChannel()
{
    sal_Int16 nChannel = 0;
    sal_Int16 nListSize = static_cast<sal_Int16>(aConvList.size());

    for (; nChannel < nListSize; ++nChannel)
    {
        if (aConvList[nChannel] == DDE_FREECHANNEL)
            return nChannel+1;
    }

    aConvList.push_back(DDE_FREECHANNEL);
    return nChannel+1;
}

SbError SbiDdeControl::Initiate( const String& rService, const String& rTopic,
            sal_Int16& rnHandle )
{
    SbError nErr;
    DdeConnection* pConv = new DdeConnection( rService, rTopic );
    nErr = GetLastErr( pConv );
    if( nErr )
    {
        delete pConv;
        rnHandle = 0;
    }
    else
    {
        sal_Int16 nChannel = GetFreeChannel();
        aConvList[nChannel-1] = pConv;
        rnHandle = nChannel;
    }
    return 0;
}

SbError SbiDdeControl::Terminate( sal_uInt16 nChannel )
{
    if (!nChannel || nChannel > aConvList.size())
        return SbERR_DDE_NO_CHANNEL;

    DdeConnection* pConv = aConvList[nChannel-1];

    if( pConv == DDE_FREECHANNEL )
        return SbERR_DDE_NO_CHANNEL;

    delete pConv;
    pConv = DDE_FREECHANNEL;

    return 0L;
}

SbError SbiDdeControl::TerminateAll()
{
    DdeConnection *conv;
    for (sal_uInt16 nChannel = 0; nChannel < aConvList.size(); ++nChannel)
    {
        conv = aConvList[nChannel];

        if (conv != DDE_FREECHANNEL)
            delete conv;
    }

    aConvList.clear();

    return 0;
}

SbError SbiDdeControl::Request( sal_uInt16 nChannel, const String& rItem, String& rResult )
{
    if (!nChannel || nChannel > aConvList.size())
        return SbERR_DDE_NO_CHANNEL;

    DdeConnection* pConv = aConvList[nChannel-1];

    if( pConv == DDE_FREECHANNEL )
        return SbERR_DDE_NO_CHANNEL;

    DdeRequest aRequest( *pConv, rItem, 30000 );
    aRequest.SetDataHdl( LINK( this, SbiDdeControl, Data ) );
    aRequest.Execute();
    rResult = aData;
    return GetLastErr( pConv );
}

SbError SbiDdeControl::Execute( sal_uInt16 nChannel, const String& rCommand )
{
    if (!nChannel || nChannel > aConvList.size())
        return SbERR_DDE_NO_CHANNEL;

    DdeConnection* pConv = aConvList[nChannel-1];

    if( pConv == DDE_FREECHANNEL )
        return SbERR_DDE_NO_CHANNEL;

    DdeExecute aRequest( *pConv, rCommand, 30000 );
    aRequest.Execute();
    return GetLastErr( pConv );
}

SbError SbiDdeControl::Poke( sal_uInt16 nChannel, const String& rItem, const String& rData )
{
    if (!nChannel || nChannel > aConvList.size())
        return SbERR_DDE_NO_CHANNEL;

    DdeConnection* pConv = aConvList[nChannel-1];

    if( pConv == DDE_FREECHANNEL )
        return SbERR_DDE_NO_CHANNEL;

    DdePoke aRequest( *pConv, rItem, DdeData(rData), 30000 );
    aRequest.Execute();
    return GetLastErr( pConv );
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

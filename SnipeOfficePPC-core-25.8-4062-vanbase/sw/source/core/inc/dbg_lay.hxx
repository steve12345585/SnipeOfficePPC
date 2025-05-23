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

#ifndef _DBG_LAY_HXX
#define _DBG_LAY_HXX

#define PROT_FILE_INIT  0x00000000
#define PROT_INIT       0x00000001
#define PROT_MAKEALL    0x00000002
#define PROT_MOVE_FWD   0x00000004
#define PROT_MOVE_BWD   0x00000008
#define PROT_GROW       0x00000010
#define PROT_SHRINK     0x00000020
#define PROT_GROW_TST   0x00000040
#define PROT_SHRINK_TST 0x00000080
#define PROT_SIZE       0x00000100
#define PROT_PRTAREA    0x00000200
#define PROT_POS        0x00000400
#define PROT_ADJUSTN    0x00000800
#define PROT_SECTION    0x00001000
#define PROT_CUT        0x00002000
#define PROT_PASTE      0x00004000
#define PROT_LEAF       0x00008000
#define PROT_TESTFORMAT 0x00010000
#define PROT_FRMCHANGES 0x00020000
#define PROT_SNAPSHOT   0x00040000

#define ACT_START           1
#define ACT_END             2
#define ACT_CREATE_MASTER   3
#define ACT_CREATE_FOLLOW   4
#define ACT_DEL_MASTER      5
#define ACT_DEL_FOLLOW      6
#define ACT_MERGE           7
#define ACT_NEXT_SECT       8
#define ACT_PREV_SECT       9

#ifdef DBG_UTIL

#include "swtypes.hxx"

class SwImplProtocol;
class SwFrm;
class SwImplEnterLeave;

class SwProtocol
{
    static sal_uLong nRecord;
    static SwImplProtocol* pImpl;
    static sal_Bool Start() { return 0 != ( PROT_INIT & nRecord ); }

public:
    static sal_uLong Record() { return nRecord; }
    static void SetRecord( sal_uLong nNew ) { nRecord = nNew; }
    static sal_Bool Record( sal_uLong nFunc ) { return 0 != (( nFunc | PROT_INIT ) & nRecord); }
    static void Record( const SwFrm* pFrm, sal_uLong nFunction, sal_uLong nAction, void* pParam );
    static void Init();
    static void Stop();
};

class SwEnterLeave
{
    SwImplEnterLeave* pImpl;
    void Ctor( const SwFrm* pFrm, sal_uLong nFunc, sal_uLong nAct, void* pPar );
    void Dtor();

public:
    SwEnterLeave( const SwFrm* pFrm, sal_uLong nFunc, sal_uLong nAct, void* pPar )
    {
        if( SwProtocol::Record( nFunc ) )
            Ctor( pFrm, nFunc, nAct, pPar );
        else
            pImpl = NULL;
    }
    ~SwEnterLeave()
    {
        if( pImpl )
            Dtor();
    }
};

#define PROTOCOL( pFrm, nFunc, nAct, pPar ) {   if( SwProtocol::Record( nFunc ) )\
                                                    SwProtocol::Record( pFrm, nFunc, nAct, pPar ); }
#define PROTOCOL_INIT SwProtocol::Init();
#define PROTOCOL_STOP SwProtocol::Stop();
#define PROTOCOL_ENTER( pFrm, nFunc, nAct, pPar ) SwEnterLeave aEnter( pFrm, nFunc, nAct, pPar );

#else

#define PROTOCOL( pFrm, nFunc, nAct, pPar )
#define PROTOCOL_INIT
#define PROTOCOL_STOP
#define PROTOCOL_ENTER( pFrm, nFunc, nAct, pPar )

#endif

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

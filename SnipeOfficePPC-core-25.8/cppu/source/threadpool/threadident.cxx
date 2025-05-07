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

#include <stdio.h>

#include <list>

#include <osl/mutex.hxx>
#include <osl/thread.h>
#include <osl/diagnose.h>

#include <rtl/process.h>
#include <rtl/byteseq.hxx>

#include <uno/threadpool.h>

#include "current.hxx"


using namespace ::std;
using namespace ::osl;
using namespace ::rtl;
using namespace ::cppu;


static inline void createLocalId( sal_Sequence **ppThreadId )
{
    rtl_byte_sequence_constructNoDefault( ppThreadId , 4 + 16 );
    sal_uInt32 id = osl_getThreadIdentifier(0);
    (*ppThreadId)->elements[0] = id & 0xFF;
    (*ppThreadId)->elements[1] = (id >> 8) & 0xFF;
    (*ppThreadId)->elements[2] = (id >> 16) & 0xFF;
    (*ppThreadId)->elements[3] = (id >> 24) & 0xFF;
    rtl_getGlobalProcessId( (sal_uInt8 * ) &( (*ppThreadId)->elements[4]) );
}


extern "C" void SAL_CALL
uno_getIdOfCurrentThread( sal_Sequence **ppThreadId )
    SAL_THROW_EXTERN_C()
{
    IdContainer * p = getIdContainer();
    if( ! p->bInit )
    {
        // first time, that the thread enters the bridge
        createLocalId( ppThreadId );

        // TODO
        // note : this is a leak !
        p->pLocalThreadId = *ppThreadId;
        p->pCurrentId = *ppThreadId;
        p->nRefCountOfCurrentId = 1;
        rtl_byte_sequence_acquire( p->pLocalThreadId );
        rtl_byte_sequence_acquire( p->pCurrentId );
        p->bInit = sal_True;
    }
    else
    {
        p->nRefCountOfCurrentId ++;
        if( *ppThreadId )
        {
            rtl_byte_sequence_release( *ppThreadId );
        }
        *ppThreadId = p->pCurrentId;
        rtl_byte_sequence_acquire( *ppThreadId );
    }
}


extern "C" void SAL_CALL uno_releaseIdFromCurrentThread()
    SAL_THROW_EXTERN_C()
{
    IdContainer *p = getIdContainer();
    OSL_ASSERT( p );
    OSL_ASSERT( p->nRefCountOfCurrentId );

    p->nRefCountOfCurrentId --;
    if( ! p->nRefCountOfCurrentId && (p->pLocalThreadId != p->pCurrentId) )
    {
        rtl_byte_sequence_assign( &(p->pCurrentId) , p->pLocalThreadId );
    }
}

extern "C" sal_Bool SAL_CALL uno_bindIdToCurrentThread( sal_Sequence *pThreadId )
    SAL_THROW_EXTERN_C()
{
    IdContainer *p = getIdContainer();
    if( ! p->bInit )
    {
        p->pLocalThreadId = 0;
        createLocalId( &(p->pLocalThreadId) );
        p->nRefCountOfCurrentId = 1;
        p->pCurrentId = pThreadId;
        rtl_byte_sequence_acquire( p->pCurrentId );
        p->bInit = sal_True;
    }
    else
    {
        OSL_ASSERT( 0 == p->nRefCountOfCurrentId );
        if( 0 == p->nRefCountOfCurrentId )
        {
            rtl_byte_sequence_assign(&( p->pCurrentId ), pThreadId );
            p->nRefCountOfCurrentId ++;
        }
        else
        {
            return sal_False;
        }

    }
    return sal_True;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

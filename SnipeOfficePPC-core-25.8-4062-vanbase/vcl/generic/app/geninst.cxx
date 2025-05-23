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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "osl/module.hxx"
#include "tools/solarmutex.hxx"

#include "generic/geninst.h"

// -------------------------------------------------------------------------
//
// SalYieldMutex
//
// -------------------------------------------------------------------------

SalYieldMutex::SalYieldMutex()
{
    mnCount     = 0;
    mnThreadId  = 0;
    ::tools::SolarMutex::SetSolarMutex( this );
}

SalYieldMutex::~SalYieldMutex()
{
    ::tools::SolarMutex::SetSolarMutex( NULL );
}

void SalYieldMutex::acquire()
{
    SolarMutexObject::acquire();
    mnThreadId = osl::Thread::getCurrentIdentifier();
    mnCount++;
}

void SalYieldMutex::release()
{
    OSL_ENSURE(mnCount > 0, "SalYieldMutex::release() called with zero count");
    if ( mnThreadId == osl::Thread::getCurrentIdentifier() )
    {
        if ( mnCount == 1 )
            mnThreadId = 0;
        mnCount--;
    }
    SolarMutexObject::release();
}

sal_Bool SalYieldMutex::tryToAcquire()
{
    if ( SolarMutexObject::tryToAcquire() )
    {
        mnThreadId = osl::Thread::getCurrentIdentifier();
        mnCount++;
        return sal_True;
    }
    else
        return sal_False;
}

osl::SolarMutex* SalGenericInstance::GetYieldMutex()
{
    return mpSalYieldMutex;
}

sal_uLong SalGenericInstance::ReleaseYieldMutex()
{
    SalYieldMutex* pYieldMutex = mpSalYieldMutex;
    if ( pYieldMutex->GetThreadId() ==
         osl::Thread::getCurrentIdentifier() )
    {
        sal_uLong nCount = pYieldMutex->GetAcquireCount();
        sal_uLong n = nCount;
        while ( n )
        {
            pYieldMutex->release();
            n--;
        }

        return nCount;
    }
    else
        return 0;
}

void SalGenericInstance::AcquireYieldMutex( sal_uLong nCount )
{
    SalYieldMutex* pYieldMutex = mpSalYieldMutex;
    while ( nCount )
    {
        pYieldMutex->acquire();
        nCount--;
    }
}

bool SalGenericInstance::CheckYieldMutex()
{
    bool bRet = true;

    SalYieldMutex* pYieldMutex = mpSalYieldMutex;
    if ( pYieldMutex->GetThreadId() != osl::Thread::getCurrentIdentifier() )
        bRet = false;

    return bRet;
}

SalGenericInstance::~SalGenericInstance()
{
    ::tools::SolarMutex::SetSolarMutex( 0 );
    delete mpSalYieldMutex;
}

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


#include <tools/time.hxx>

#include <vcl/svapp.hxx>
#include <vcl/timer.hxx>

#include <saltimer.hxx>
#include <svdata.hxx>
#include <salinst.hxx>


// =======================================================================

#define MAX_TIMER_PERIOD    ((sal_uLong)0xFFFFFFFF)

// ---------------------
// - TimeManager-Types -
// ---------------------

struct ImplTimerData
{
    ImplTimerData*  mpNext;         // Pointer to the next Instance
    Timer*          mpSVTimer;      // Pointer to SV Timer instance
    sal_uLong           mnUpdateTime;   // Last Update Time
    sal_uLong           mnTimerUpdate;  // TimerCallbackProcs on stack
    sal_Bool            mbDelete;       // Was timer deleted during Update()?
    sal_Bool            mbInTimeout;    // Are we in a timeout handler?
};

// =======================================================================

void Timer::ImplDeInitTimer()
{
    ImplSVData*     pSVData = ImplGetSVData();
    ImplTimerData*  pTimerData = pSVData->mpFirstTimerData;

    if ( pTimerData )
    {
        do
        {
            ImplTimerData* pTempTimerData = pTimerData;
            if ( pTimerData->mpSVTimer )
            {
                pTimerData->mpSVTimer->mbActive = sal_False;
                pTimerData->mpSVTimer->mpTimerData = NULL;
            }
            pTimerData = pTimerData->mpNext;
            delete pTempTimerData;
        }
        while ( pTimerData );

        pSVData->mpFirstTimerData   = NULL;
        pSVData->mnTimerPeriod      = 0;
        delete pSVData->mpSalTimer;
        pSVData->mpSalTimer = NULL;
    }
}

// -----------------------------------------------------------------------

static void ImplStartTimer( ImplSVData* pSVData, sal_uLong nMS )
{
    if ( !nMS )
        nMS = 1;

    if ( nMS != pSVData->mnTimerPeriod )
    {
        pSVData->mnTimerPeriod = nMS;
        pSVData->mpSalTimer->Start( nMS );
    }
}

// -----------------------------------------------------------------------

void Timer::ImplTimerCallbackProc()
{
    ImplSVData*     pSVData = ImplGetSVData();
    ImplTimerData*  pTimerData;
    ImplTimerData*  pPrevTimerData;
    sal_uLong           nMinPeriod = MAX_TIMER_PERIOD;
    sal_uLong           nDeltaTime;
    sal_uLong           nTime = Time::GetSystemTicks();

    if ( pSVData->mbNoCallTimer )
        return;

    pSVData->mnTimerUpdate++;
    pSVData->mbNotAllTimerCalled = sal_True;

    // find timer where the timer handler needs to be called
    pTimerData = pSVData->mpFirstTimerData;
    while ( pTimerData )
    {
        // If the timer is not new, was not deleted, and if it is not in the timeout handler, then
        // call the handler as soon as the time is up.
        if ( (pTimerData->mnTimerUpdate < pSVData->mnTimerUpdate) &&
             !pTimerData->mbDelete && !pTimerData->mbInTimeout )
        {
            // time has expired
            if ( (pTimerData->mnUpdateTime+pTimerData->mpSVTimer->mnTimeout) <= nTime )
            {
                // set new update time
                pTimerData->mnUpdateTime = nTime;

                // if no AutoTimer than stop
                if ( !pTimerData->mpSVTimer->mbAuto )
                {
                    pTimerData->mpSVTimer->mbActive = sal_False;
                    pTimerData->mbDelete = sal_True;
                }

                // call Timeout
                pTimerData->mbInTimeout = sal_True;
                pTimerData->mpSVTimer->Timeout();
                pTimerData->mbInTimeout = sal_False;
            }
        }

        pTimerData = pTimerData->mpNext;
    }

    // determine new time
    sal_uLong nNewTime = Time::GetSystemTicks();
    pPrevTimerData = NULL;
    pTimerData = pSVData->mpFirstTimerData;
    while ( pTimerData )
    {
        // ignore if timer is still in timeout handler
        if ( pTimerData->mbInTimeout )
        {
            pPrevTimerData = pTimerData;
            pTimerData = pTimerData->mpNext;
        }
        // Was timer destroyed in the meantime?
        else if ( pTimerData->mbDelete )
        {
            if ( pPrevTimerData )
                pPrevTimerData->mpNext = pTimerData->mpNext;
            else
                pSVData->mpFirstTimerData = pTimerData->mpNext;
            if ( pTimerData->mpSVTimer )
                pTimerData->mpSVTimer->mpTimerData = NULL;
            ImplTimerData* pTempTimerData = pTimerData;
            pTimerData = pTimerData->mpNext;
            delete pTempTimerData;
        }
        else
        {
            pTimerData->mnTimerUpdate = 0;
            // determine smallest time slot
            if ( pTimerData->mnUpdateTime == nTime )
            {
                nDeltaTime = pTimerData->mpSVTimer->mnTimeout;
                if ( nDeltaTime < nMinPeriod )
                    nMinPeriod = nDeltaTime;
            }
            else
            {
                nDeltaTime = pTimerData->mnUpdateTime + pTimerData->mpSVTimer->mnTimeout;
                if ( nDeltaTime < nNewTime )
                    nMinPeriod = 1;
                else
                {
                    nDeltaTime -= nNewTime;
                    if ( nDeltaTime < nMinPeriod )
                        nMinPeriod = nDeltaTime;
                }
            }
            pPrevTimerData = pTimerData;
            pTimerData = pTimerData->mpNext;
        }
    }

    // delete clock if no more timers available
    if ( !pSVData->mpFirstTimerData )
    {
        pSVData->mpSalTimer->Stop();
        pSVData->mnTimerPeriod = MAX_TIMER_PERIOD;
    }
    else
        ImplStartTimer( pSVData, nMinPeriod );

    pSVData->mnTimerUpdate--;
    pSVData->mbNotAllTimerCalled = sal_False;
}

// =======================================================================

Timer::Timer()
{
    mpTimerData     = NULL;
    mnTimeout       = 1;
    mbAuto          = sal_False;
    mbActive        = sal_False;
}

// -----------------------------------------------------------------------

Timer::Timer( const Timer& rTimer )
{
    mpTimerData     = NULL;
    mnTimeout       = rTimer.mnTimeout;
    mbAuto          = sal_False;
    mbActive        = sal_False;
    maTimeoutHdl    = rTimer.maTimeoutHdl;

    if ( rTimer.IsActive() )
        Start();
}

// -----------------------------------------------------------------------

Timer::~Timer()
{
    if ( mpTimerData )
    {
        mpTimerData->mbDelete = sal_True;
        mpTimerData->mpSVTimer = NULL;
    }
}

// -----------------------------------------------------------------------

void Timer::Timeout()
{
    maTimeoutHdl.Call( this );
}

// -----------------------------------------------------------------------

void Timer::SetTimeout( sal_uLong nNewTimeout )
{
    mnTimeout = nNewTimeout;

    // if timer is active then renew clock
    if ( mbActive )
    {
        ImplSVData* pSVData = ImplGetSVData();
        if ( !pSVData->mnTimerUpdate && (mnTimeout < pSVData->mnTimerPeriod) )
            ImplStartTimer( pSVData, mnTimeout );
    }
}

// -----------------------------------------------------------------------

void Timer::Start()
{
    mbActive = sal_True;

    ImplSVData* pSVData = ImplGetSVData();
    if ( !mpTimerData )
    {
        if ( !pSVData->mpFirstTimerData )
        {
            pSVData->mnTimerPeriod = MAX_TIMER_PERIOD;
            if( ! pSVData->mpSalTimer )
            {
                pSVData->mpSalTimer = pSVData->mpDefInst->CreateSalTimer();
                pSVData->mpSalTimer->SetCallback( ImplTimerCallbackProc );
            }
        }

        // insert timer and start
        mpTimerData                 = new ImplTimerData;
        mpTimerData->mpSVTimer      = this;
        mpTimerData->mnUpdateTime   = Time::GetSystemTicks();
        mpTimerData->mnTimerUpdate  = pSVData->mnTimerUpdate;
        mpTimerData->mbDelete       = sal_False;
        mpTimerData->mbInTimeout    = sal_False;

        // insert last due to SFX!
        ImplTimerData* pPrev = NULL;
        ImplTimerData* pData = pSVData->mpFirstTimerData;
        while ( pData )
        {
            pPrev = pData;
            pData = pData->mpNext;
        }
        mpTimerData->mpNext = NULL;
        if ( pPrev )
            pPrev->mpNext = mpTimerData;
        else
            pSVData->mpFirstTimerData = mpTimerData;

        if ( mnTimeout < pSVData->mnTimerPeriod )
            ImplStartTimer( pSVData, mnTimeout );
    }
    else if( !mpTimerData->mpSVTimer ) // TODO: remove when guilty found
    {
        OSL_FAIL( "Timer::Start() on a destroyed Timer!" );
    }
    else
    {
        mpTimerData->mnUpdateTime    = Time::GetSystemTicks();
        mpTimerData->mnTimerUpdate   = pSVData->mnTimerUpdate;
        mpTimerData->mbDelete        = sal_False;
    }
}

// -----------------------------------------------------------------------

void Timer::Stop()
{
    mbActive = sal_False;

    if ( mpTimerData )
        mpTimerData->mbDelete = sal_True;
}

// -----------------------------------------------------------------------

Timer& Timer::operator=( const Timer& rTimer )
{
    if ( IsActive() )
        Stop();

    mbActive        = sal_False;
    mnTimeout       = rTimer.mnTimeout;
    maTimeoutHdl    = rTimer.maTimeoutHdl;

    if ( rTimer.IsActive() )
        Start();

    return *this;
}

// =======================================================================

AutoTimer::AutoTimer()
{
    mbAuto = sal_True;
}

// -----------------------------------------------------------------------

AutoTimer::AutoTimer( const AutoTimer& rTimer ) : Timer( rTimer )
{
    mbAuto = sal_True;
}

// -----------------------------------------------------------------------

AutoTimer& AutoTimer::operator=( const AutoTimer& rTimer )
{
    Timer::operator=( rTimer );
    return *this;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

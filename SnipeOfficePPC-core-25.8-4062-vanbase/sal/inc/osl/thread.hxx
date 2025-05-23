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

#ifndef _THREAD_HXX_
#define _THREAD_HXX_

#include "sal/config.h"

#include <cassert>

#include <osl/time.h>


#include <osl/diagnose.h>
#include <osl/thread.h>
#include <rtl/alloc.h>

namespace osl
{
/** threadFunc is the function which is executed by the threads
    created by the osl::Thread class. The function's signature
    matches the one of oslWorkerFunction which is declared in
    osl/thread.h .
*/
extern "C" inline void SAL_CALL threadFunc( void* param);

/**
   A thread abstraction.

   @deprecated use ::salhelper::Thread instead.  Only the static member
   functions ::osl::Thread::getCurrentIdentifier, ::osl::Thread::wait, and
   ::osl::Thread::yield are not deprecated.
 */
class Thread
{
    Thread( const Thread& );
    Thread& operator= ( const Thread& );
public:
    // these are here to force memory de/allocation to sal lib.
    inline static void * SAL_CALL operator new( size_t nSize ) SAL_THROW (())
        { return ::rtl_allocateMemory( nSize ); }
    inline static void SAL_CALL operator delete( void * pMem ) SAL_THROW (())
        { ::rtl_freeMemory( pMem ); }
    inline static void * SAL_CALL operator new( size_t, void * pMem ) SAL_THROW (())
        { return pMem; }
    inline static void SAL_CALL operator delete( void *, void * ) SAL_THROW (())
        {}

    Thread(): m_hThread(0){}

    virtual  ~Thread()
    {
        osl_destroyThread( m_hThread);
    }

    sal_Bool SAL_CALL create()
    {
        assert(m_hThread == 0); // only one running thread per instance
        m_hThread = osl_createSuspendedThread( threadFunc, (void*)this);
        if (m_hThread == 0)
        {
            return false;
        }
        osl_resumeThread(m_hThread);
        return true;
    }

    sal_Bool SAL_CALL createSuspended()
    {
        assert(m_hThread == 0); // only one running thread per instance
        if( m_hThread)
            return sal_False;
        m_hThread= osl_createSuspendedThread( threadFunc,
                                             (void*)this);
        return m_hThread != 0;
    }

    virtual void SAL_CALL suspend()
    {
        if( m_hThread )
            osl_suspendThread(m_hThread);
    }

    virtual void SAL_CALL resume()
    {
        if( m_hThread )
            osl_resumeThread(m_hThread);
    }

    virtual void SAL_CALL terminate()
    {
        if( m_hThread )
            osl_terminateThread(m_hThread);
    }

    virtual void SAL_CALL join()
    {
        osl_joinWithThread(m_hThread);
    }

    sal_Bool SAL_CALL isRunning() const
    {
        return osl_isThreadRunning(m_hThread);
    }

    void SAL_CALL setPriority( oslThreadPriority Priority)
    {
        if( m_hThread )
            osl_setThreadPriority(m_hThread, Priority);
    }

    oslThreadPriority SAL_CALL getPriority() const
    {
        return m_hThread ? osl_getThreadPriority(m_hThread) : osl_Thread_PriorityUnknown;
    }

    oslThreadIdentifier SAL_CALL getIdentifier() const
    {
        return osl_getThreadIdentifier(m_hThread);
    }

    static oslThreadIdentifier SAL_CALL getCurrentIdentifier()
    {
        return osl_getThreadIdentifier(0);
    }

    static void SAL_CALL wait(const TimeValue& Delay)
    {
        osl_waitThread(&Delay);
    }

    static void SAL_CALL yield()
    {
        osl_yieldThread();
    }

    static inline void setName(char const * name) throw () {
        osl_setThreadName(name);
    }

    virtual sal_Bool SAL_CALL schedule()
    {
        return m_hThread ? osl_scheduleThread(m_hThread) : sal_False;
    }

    SAL_CALL operator oslThread() const
    {
        return m_hThread;
    }

protected:

    /** The thread functions calls the protected functions
        run and onTerminated.
    */
    friend void SAL_CALL threadFunc( void* param);

    virtual void SAL_CALL run() = 0;

    virtual void SAL_CALL onTerminated()
    {
    }

private:
    oslThread m_hThread;
};

extern "C" inline void SAL_CALL threadFunc( void* param)
{
        Thread* pObj= (Thread*)param;
        pObj->run();
        pObj->onTerminated();
}

class ThreadData
{
    ThreadData( const ThreadData& );
    ThreadData& operator= (const ThreadData& );
public:
     /// Create a thread specific local data key
    ThreadData( oslThreadKeyCallbackFunction pCallback= 0 )
    {
        m_hKey = osl_createThreadKey( pCallback );
    }

    /// Destroy a thread specific local data key
    ~ThreadData()
    {
           osl_destroyThreadKey(m_hKey);
    }

    /** Set the data associated with the data key.
        @returns True if operation was successfull
    */
    sal_Bool SAL_CALL setData(void *pData)
    {
           return (osl_setThreadKeyData(m_hKey, pData));
    }

    /** Get the data associated with the data key.
        @returns The data asscoitaed with the data key or
        NULL if no data was set
    */
    void* SAL_CALL getData()
    {
           return osl_getThreadKeyData(m_hKey);
    }

    operator oslThreadKey() const
    {
        return m_hKey;
    }

private:
    oslThreadKey m_hKey;
};

} // end namespace osl

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

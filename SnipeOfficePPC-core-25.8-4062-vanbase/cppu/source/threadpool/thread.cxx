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

#include <stdio.h>
#include <osl/diagnose.h>
#include <uno/threadpool.h>

#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/XInterface.hpp>
#include <rtl/ustring.h>
#include <rtl/ustring.hxx>

#include "thread.hxx"
#include "jobqueue.hxx"
#include "threadpool.hxx"

using namespace osl;

namespace cppu_threadpool {

// ----------------------------------------------------------------------------------
    ThreadAdmin::ThreadAdmin(): m_disposed(false) {}

    ThreadAdmin::~ThreadAdmin()
    {
#if OSL_DEBUG_LEVEL > 1
        if( m_lst.size() )
        {
            fprintf( stderr, "%lu Threads left\n" , static_cast<unsigned long>(m_lst.size()) );
        }
#endif
    }

    void ThreadAdmin::add( rtl::Reference< ORequestThread > const & p )
    {
        MutexGuard aGuard( m_mutex );
        if( m_disposed )
        {
            throw css::lang::DisposedException(
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM(
                        "cppu_threadpool::ORequestThread created after"
                        " cppu_threadpool::ThreadAdmin has been disposed")),
                css::uno::Reference< css::uno::XInterface >());
        }
        m_lst.push_back( p );
    }

    void ThreadAdmin::remove_locked( rtl::Reference< ORequestThread > const & p )
    {
        ::std::list< rtl::Reference< ORequestThread > >::iterator ii = ::std::find( m_lst.begin(), m_lst.end(), p );
        if( ii != m_lst.end() )
        {
            m_lst.erase( ii );
        }
    }

    void ThreadAdmin::remove( rtl::Reference< ORequestThread > const & p )
    {
        MutexGuard aGuard( m_mutex );
        remove_locked( p );
    }

    void ThreadAdmin::join()
    {
        {
            MutexGuard aGuard( m_mutex );
            m_disposed = true;
        }
        for (;;)
        {
            rtl::Reference< ORequestThread > pCurrent;
            {
                MutexGuard aGuard( m_mutex );
                if( m_lst.empty() )
                {
                    break;
                }
                pCurrent = m_lst.front();
                m_lst.pop_front();
            }
            pCurrent->join();
        }
    }

// ----------------------------------------------------------------------------------
    ORequestThread::ORequestThread( ThreadPoolHolder const &aThreadPool,
                                    JobQueue *pQueue,
                                    const ByteSequence &aThreadId,
                                    sal_Bool bAsynchron )
        : m_aThreadPool( aThreadPool )
        , m_pQueue( pQueue )
        , m_aThreadId( aThreadId )
        , m_bAsynchron( bAsynchron )
    {}

    ORequestThread::~ORequestThread() {}

    void ORequestThread::setTask( JobQueue *pQueue,
                                  const ByteSequence &aThreadId,
                                  sal_Bool bAsynchron )
    {
        m_pQueue = pQueue;
        m_aThreadId = aThreadId;
        m_bAsynchron = bAsynchron;
    }

    void ORequestThread::launch()
    {
        // Assumption is that osl::Thread::create returns normally with a true
        // return value iff it causes osl::Thread::run to start executing:
        acquire();
        ThreadAdmin & rThreadAdmin = m_aThreadPool->getThreadAdmin();
        osl::ClearableMutexGuard g(rThreadAdmin.m_mutex);
        rThreadAdmin.add( this );
        try {
            if (!create()) {
                throw std::runtime_error("osl::Thread::create failed");
            }
        } catch (...) {
            rThreadAdmin.remove_locked( this );
            g.clear();
            release();
            throw;
        }
    }

    void ORequestThread::onTerminated()
    {
        m_aThreadPool->getThreadAdmin().remove( this );
        release();
    }

    void ORequestThread::run()
    {
        try
        {
            while ( m_pQueue )
            {
                if( ! m_bAsynchron )
                {
                    if ( !uno_bindIdToCurrentThread( m_aThreadId.getHandle() ) )
                    {
                        OSL_ASSERT( false );
                    }
                }

                while( ! m_pQueue->isEmpty() )
                {
                    // Note : Oneways should not get a disposable disposeid,
                    //        It does not make sense to dispose a call in this state.
                    //        That's way we put it an disposeid, that can't be used otherwise.
                    m_pQueue->enter(
                        sal::static_int_cast< sal_Int64 >(
                            reinterpret_cast< sal_IntPtr >(this)),
                        sal_True );

                    if( m_pQueue->isEmpty() )
                    {
                        m_aThreadPool->revokeQueue( m_aThreadId , m_bAsynchron );
                        // Note : revokeQueue might have failed because m_pQueue.isEmpty()
                        //        may be false (race).
                    }
                }

                delete m_pQueue;
                m_pQueue = 0;

                if( ! m_bAsynchron )
                {
                    uno_releaseIdFromCurrentThread();
                }

                m_aThreadPool->waitInPool( this );
            }
        }
        catch (...)
        {
            // Work around the problem that onTerminated is not called if run
            // throws an exception:
            onTerminated();
            throw;
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#ifndef INCLUDED_CPPU_SOURCE_THREADPOOL_THREADPOOL_HXX
#define INCLUDED_CPPU_SOURCE_THREADPOOL_THREADPOOL_HXX

#include <list>

#include <boost/unordered_map.hpp>

#include <osl/conditn.h>

#include <rtl/byteseq.hxx>
#include <rtl/ref.hxx>
#include <salhelper/simplereferenceobject.hxx>

#include <boost/shared_ptr.hpp>

#include "jobqueue.hxx"


using namespace ::rtl;
namespace cppu_threadpool {
    class ORequestThread;

    struct EqualThreadId
    {
        sal_Int32 operator () ( const ::rtl::ByteSequence &a , const ::rtl::ByteSequence &b ) const
            {
                return a == b;
            }
    };

    struct HashThreadId
    {
        sal_Int32 operator () ( const ::rtl::ByteSequence &a  )  const
            {
                if( a.getLength() >= 4 )
                {
                    return *(sal_Int32 *)a.getConstArray();
                }
                return 0;
            }
    };

    typedef ::boost::unordered_map
    <
        ByteSequence, // ThreadID
        ::std::pair < JobQueue * , JobQueue * >,
        HashThreadId,
        EqualThreadId
    > ThreadIdHashMap;

    typedef ::std::list < sal_Int64 > DisposedCallerList;


    struct WaitingThread
    {
        oslCondition condition;
        rtl::Reference< ORequestThread > thread;
    };

    typedef ::std::list < struct ::cppu_threadpool::WaitingThread * > WaitingThreadList;

    class DisposedCallerAdmin;
    typedef boost::shared_ptr<DisposedCallerAdmin> DisposedCallerAdminHolder;

    class DisposedCallerAdmin
    {
    public:
        ~DisposedCallerAdmin();

        static DisposedCallerAdminHolder getInstance();

        void dispose( sal_Int64 nDisposeId );
        void destroy( sal_Int64 nDisposeId );
        sal_Bool isDisposed( sal_Int64 nDisposeId );

    private:
        ::osl::Mutex m_mutex;
        DisposedCallerList m_lst;
    };

    class ThreadAdmin
    {
    public:
        ThreadAdmin();
        ~ThreadAdmin ();

        void add( rtl::Reference< ORequestThread > const & );
        void remove( rtl::Reference< ORequestThread > const & );
        void join();

        void remove_locked( rtl::Reference< ORequestThread > const & );
        ::osl::Mutex m_mutex;

    private:
        ::std::list< rtl::Reference< ORequestThread > > m_lst;
        bool m_disposed;
    };

    class ThreadPool;
    typedef rtl::Reference<ThreadPool> ThreadPoolHolder;

    class ThreadPool: public salhelper::SimpleReferenceObject
    {
    public:
        ThreadPool();
        ~ThreadPool();

        void dispose( sal_Int64 nDisposeId );
        void destroy( sal_Int64 nDisposeId );

        void addJob( const ByteSequence &aThreadId,
                     sal_Bool bAsynchron,
                     void *pThreadSpecificData,
                     RequestFun * doRequest );

        void prepare( const ByteSequence &aThreadId );
        void * enter( const ByteSequence &aThreadId, sal_Int64 nDisposeId );

        /********
         * @return true, if queue could be successfully revoked.
         ********/
        sal_Bool revokeQueue( const ByteSequence & aThreadId , sal_Bool bAsynchron );

        void waitInPool( rtl::Reference< ORequestThread > const & pThread );

        void joinWorkers();

        ThreadAdmin & getThreadAdmin() { return m_aThreadAdmin; }

    private:
        void createThread( JobQueue *pQueue, const ByteSequence &aThreadId, sal_Bool bAsynchron);


        ThreadIdHashMap m_mapQueue;
        ::osl::Mutex m_mutex;

        ::osl::Mutex m_mutexWaitingThreadList;
        WaitingThreadList m_lstThreads;

        DisposedCallerAdminHolder m_DisposedCallerAdmin;
        ThreadAdmin m_aThreadAdmin;
    };

} // end namespace cppu_threadpool

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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
#ifndef _CPPU_THREADPOOL_THREAD_HXX
#define _CPPU_THREADPOOL_THREAD_HXX

#include <osl/thread.hxx>
#include <sal/types.h>
#include <salhelper/simplereferenceobject.hxx>

#include "jobqueue.hxx"
#include "threadpool.hxx"

namespace cppu_threadpool {

    class JobQueue;

    //-----------------------------------------
    // private thread class for the threadpool
    // independent from vos
    //-----------------------------------------
    class ORequestThread:
        public salhelper::SimpleReferenceObject, public osl::Thread
    {
    public:
        ORequestThread( ThreadPoolHolder const &aThreadPool,
                        JobQueue * ,
                        const ::rtl::ByteSequence &aThreadId,
                        sal_Bool bAsynchron );
        virtual ~ORequestThread();

        void setTask( JobQueue * , const ::rtl::ByteSequence & aThreadId , sal_Bool bAsynchron );

        void launch();

        static inline void * operator new(std::size_t size)
        { return SimpleReferenceObject::operator new(size); }

        static inline void operator delete(void * pointer)
        { SimpleReferenceObject::operator delete(pointer); }

    private:
        virtual void SAL_CALL run();
        virtual void SAL_CALL onTerminated();

        ThreadPoolHolder m_aThreadPool;
        JobQueue *m_pQueue;
        ::rtl::ByteSequence m_aThreadId;
        sal_Bool m_bAsynchron;
    };

} // end cppu_threadpool


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

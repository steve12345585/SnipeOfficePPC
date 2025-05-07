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

#ifndef _CPPU_THREADPOOL_JOBQUEUE_HXX_
#define _CPPU_THREADPOOL_JOBQUEUE_HXX_

#include <list>
#include <sal/types.h>

#include <osl/conditn.h>
#include <osl/mutex.hxx>

#include <boost/shared_ptr.hpp>

namespace cppu_threadpool
{
    extern "C" typedef void (SAL_CALL RequestFun)(void *);

    struct Job
    {
        void *pThreadSpecificData;
        RequestFun * doRequest;
    };

    typedef ::std::list < struct Job > JobList;

    typedef ::std::list < sal_Int64 > CallStackList;

    class DisposedCallerAdmin;
    typedef boost::shared_ptr<DisposedCallerAdmin> DisposedCallerAdminHolder;

    class JobQueue
    {
    public:
        JobQueue();
        ~JobQueue();

        void add( void *pThreadSpecificData, RequestFun * doRequest );

        void *enter( sal_Int64 nDisposeId , sal_Bool bReturnWhenNoJob = sal_False );
        void dispose( sal_Int64 nDisposeId );

        void suspend();
        void resume();

        sal_Bool isEmpty() const;
        sal_Bool isCallstackEmpty() const;
        sal_Bool isBusy() const;

    private:
        mutable ::osl::Mutex m_mutex;
        JobList      m_lstJob;
        CallStackList m_lstCallstack;
        sal_Int32 m_nToDo;
        sal_Bool m_bSuspended;
        oslCondition m_cndWait;
        DisposedCallerAdminHolder m_DisposedCallerAdmin;
    };
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

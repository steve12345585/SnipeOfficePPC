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

package com.sun.star.lib.uno.environments.remote;

/**
 * This class implements a java thread pool.
 * <p>
 * @version     $Revision: 1.13 $ $ $Date: 2008-04-11 11:20:22 $
 * @author      Kay Ramme
 * @see         com.sun.star.uno.UnoRuntime
 * @see         com.sun.star.lib.uno.environments.remote.ThreadPool
 * @see         com.sun.star.lib.uno.environments.remote.IThreadPool
 * @see         com.sun.star.lib.uno.environments.remote.Job
 * @see         com.sun.star.lib.uno.environments.remote.JobQueue
 * @since       UDK1.0
 */
public class JavaThreadPool implements IThreadPool {
    /**
     * When set to true, enables various debugging output.
     */
    private static final boolean DEBUG = false;

    JavaThreadPoolFactory _javaThreadPoolFactory;

    JavaThreadPool(JavaThreadPoolFactory javaThreadPoolFactory) {
        _javaThreadPoolFactory = javaThreadPoolFactory;
    }

    public ThreadId getThreadId() {
        return JavaThreadPoolFactory.getThreadId();
    }

    public Object attach( ThreadId threadId )
    {
        if(DEBUG) System.err.println("##### " + getClass().getName() + ".attach - id:" + threadId);
        JobQueue jobQueue = _javaThreadPoolFactory.getJobQueue(threadId);
        if(jobQueue == null)
            jobQueue = new JobQueue(_javaThreadPoolFactory, threadId, false);

        // acquiring the jobQueue registers it at the ThreadPoolFactory
        jobQueue.acquire();
        return jobQueue;
    }

    public void attach() {
        attach( getThreadId() );
    }

    public void detach( Object handle, ThreadId id )
    {
        ((JobQueue)handle).release();
    }

    public void detach() {
        ThreadId threadId =  getThreadId();
        detach(_javaThreadPoolFactory.getJobQueue(threadId), threadId );
    }


    public Object enter( ) throws Throwable {
        ThreadId threadId = getThreadId();
        return enter( _javaThreadPoolFactory.getJobQueue( threadId ), threadId  );
    }

    public Object enter( Object handle, ThreadId threadId ) throws Throwable {
        return ((JobQueue)handle).enter(this);
    }

    public void putJob(Job job) {
        if (!job.isRequest() || job.isSynchronous()) {
            JobQueue jobQueue = _javaThreadPoolFactory.getJobQueue(job.getThreadId());

            // this has not be synchronized, cause
            // sync jobs can only come over one bridge
            // (cause the thread blocks on other side)
            if(jobQueue == null)
                jobQueue = new JobQueue(_javaThreadPoolFactory, job.getThreadId(), true);

            // put job acquires the queue and registers it at the ThreadPoolFactory
            jobQueue.putJob(job, this);
        }
        else {
            // this has to be synchronized, cause
            // async jobs of the same thread can come
            // over different bridges
            synchronized(_javaThreadPoolFactory) {
                JobQueue async_jobQueue = _javaThreadPoolFactory.getAsyncJobQueue(job.getThreadId());

                // ensure there is jobQueue
                if(async_jobQueue == null) // so, there is really no async queue
                    async_jobQueue = new JobQueue(_javaThreadPoolFactory, job.getThreadId());

                // put job acquires the queue and registers it at the ThreadPoolFactory
                async_jobQueue.putJob(job, this);
            }
        }
    }

    public void dispose(Throwable throwable) {
        if(DEBUG) System.err.println("##### " + getClass().getName() + ".dispose:" + throwable);

        _javaThreadPoolFactory.dispose(this, throwable);
    }

    public void destroy() {
    }
}

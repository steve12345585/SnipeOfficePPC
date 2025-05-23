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
#ifndef _OBSERVABLETHREAD_HXX
#define _OBSERVABLETHREAD_HXX

#include <osl/thread.hxx>
#include <rtl/ref.hxx>
#include <osl/interlck.h>

#include <boost/weak_ptr.hpp>
#include <ithreadlistenerowner.hxx>

/** class for an observable thread

    OD 2007-01-29 #i73788#
    Note: A thread is ref-counted. Thus, an instance of a derived class has to
    to be hold via a reference. The thread itself assures during its execution,
    that the ref-count is increased. Its execution starts with its <run()> method
    and ends with its <onTerminated()> method.
    Note: A thread can be only observed by one or none thread observer in order
    to notify, that the thread has finished its work.

    @author OD
*/
class ObservableThread : public osl::Thread,
                         public rtl::IReference
{
    public:

        virtual ~ObservableThread();

        void SetListener( boost::weak_ptr< IFinishedThreadListener > pThreadListener,
                          const oslInterlockedCount nThreadID );

        // IReference
        virtual oslInterlockedCount SAL_CALL acquire();
        virtual oslInterlockedCount SAL_CALL release();

    protected:

        ObservableThread();

        /** intrinsic function of the thread

            Important note:
            Do not override this method again. Instead override <threadFunction()>.
            Otherwise, it's not guaranteed, that its ref-count is increased
            during the execution of the thread.

            @author OD
        */
        virtual void SAL_CALL run();

        virtual void threadFunction() = 0;

        /** method called, when thread has finished its work

            Important note:
            Do not override this method again. Instead override <threadFinished()>.
            Otherwise, it's not guaranteed, that the ref-count is decreased at
            the end of its execution and that the observer is notified, that
            the thread has finished its work.

            @author OD
        */
        virtual void SAL_CALL onTerminated();

        virtual void threadFinished();

    private:

        oslInterlockedCount mnRefCount;

        oslInterlockedCount mnThreadID;

        boost::weak_ptr< IFinishedThreadListener > mpThreadListener;

};
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

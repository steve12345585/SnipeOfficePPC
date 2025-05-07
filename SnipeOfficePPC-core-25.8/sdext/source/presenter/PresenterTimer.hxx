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

#ifndef SDEXT_PRESENTER_TIMER_HXX
#define SDEXT_PRESENTER_TIMER_HXX

#include <com/sun/star/awt/XCallback.hpp>
#include <com/sun/star/awt/XRequestCallback.hpp>
#include <cppuhelper/basemutex.hxx>
#include <cppuhelper/compbase1.hxx>
#include <osl/mutex.hxx>
#include <osl/time.h>
#include <rtl/ref.hxx>
#include <sal/types.h>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <vector>

namespace css = ::com::sun::star;

namespace sdext { namespace presenter {

class PresenterClockInternalTimer;

/** The timer allows tasks to be scheduled for execution at a specified time
    in the future.
*/
class PresenterTimer
{
public:
    /** A task is called with the current time.
    */
    typedef ::boost::function<void(const TimeValue&)> Task;

    static const sal_Int32 NotAValidTaskId = 0;

    /** Schedule a task to be executed repeatedly.  The task is executed the
        first time after nFirst nano-seconds (1000000000 corresponds to one
        second).  After that task is executed in intervalls that are
        nIntervall ns long until CancelTask is called.
    */
    static sal_Int32 ScheduleRepeatedTask (
        const Task& rTask,
        const sal_Int64 nFirst,
        const sal_Int64 nIntervall);

    static void CancelTask (const sal_Int32 nTaskId);
};

typedef cppu::WeakComponentImplHelper1<
    css::awt::XCallback
    > PresenterClockTimerInterfaceBase;

/** A timer that calls its listeners, typically clocks, every second to
    update their current time value.
*/
class PresenterClockTimer
    : protected ::cppu::BaseMutex,
      public PresenterClockTimerInterfaceBase
{
public:
    class Listener {
    public:
        virtual void TimeHasChanged (const oslDateTime& rCurrentTime) = 0;

    protected:
        ~Listener() {}
    };
    typedef ::boost::shared_ptr<Listener> SharedListener;

    static ::rtl::Reference<PresenterClockTimer> Instance (
        const css::uno::Reference<css::uno::XComponentContext>& rxContext);

    void AddListener (const SharedListener& rListener);
    void RemoveListener (const SharedListener& rListener);

    static oslDateTime GetCurrentTime (void);

    // XCallback

    virtual void SAL_CALL notify (const css::uno::Any& rUserData)
        throw (css::uno::RuntimeException);

private:
    static ::rtl::Reference<PresenterClockTimer> mpInstance;

    ::osl::Mutex maMutex;
    typedef ::std::vector<SharedListener> ListenerContainer;
    ListenerContainer maListeners;
    oslDateTime maDateTime;
    sal_Int32 mnTimerTaskId;
    bool mbIsCallbackPending;
    css::uno::Reference<css::awt::XRequestCallback> mxRequestCallback;

    PresenterClockTimer (
        const css::uno::Reference<css::uno::XComponentContext>& rxContext);
    ~PresenterClockTimer (void);

    void CheckCurrentTime (const TimeValue& rCurrentTime);
};

} } // end of namespace ::sdext::presenter

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

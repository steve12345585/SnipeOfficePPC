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

#ifndef _UNXCOMMANDTHREAD_HXX_
#define _UNXCOMMANDTHREAD_HXX_

#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>

#include <osl/conditn.hxx>
#include <osl/mutex.hxx>
#include <osl/thread.hxx>
#include <rtl/ustring.hxx>

#include <vcl/svapp.hxx>

#include <list>

class UnxFilePickerNotifyThread;

/** Synchronization for the 'thread-less' version of the fpicker.

    Something like osl::Condition, but calls Application::Yield() while in
    wait().
*/
class YieldingCondition
{
    ::osl::Mutex m_aMutex;
    bool m_bValue;

    bool get()
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        return m_bValue;
    }

public:
    YieldingCondition() { reset(); }

    void reset()
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        m_bValue = false;
    }

    void set()
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        m_bValue = true;
    }

    void wait()
    {
        while ( !get() )
            Application::Yield();
    }
};

class UnxFilePickerCommandThread : public ::osl::Thread
{
protected:
    UnxFilePickerNotifyThread  *m_pNotifyThread;
    int                         m_nReadFD;

    ::osl::Mutex                m_aMutex;

    YieldingCondition           m_aExecCondition;
    sal_Bool                    m_aResult;

    ::osl::Condition            m_aGetCurrentFilterCondition;
    ::rtl::OUString             m_aGetCurrentFilter;

    ::osl::Condition            m_aGetDirectoryCondition;
    ::rtl::OUString             m_aGetDirectory;

    ::osl::Condition            m_aGetFilesCondition;
    ::std::list< ::rtl::OUString > m_aGetFiles;

    ::osl::Condition            m_aGetValueCondition;
    ::com::sun::star::uno::Any  m_aGetValue;

public:
    UnxFilePickerCommandThread( UnxFilePickerNotifyThread *pNotifyThread, int nReadFD );
    ~UnxFilePickerCommandThread();

    YieldingCondition& SAL_CALL execCondition() { return m_aExecCondition; }
    sal_Bool SAL_CALL           result();

    ::osl::Condition& SAL_CALL  getCurrentFilterCondition() { return m_aGetCurrentFilterCondition; }
    ::rtl::OUString SAL_CALL    getCurrentFilter();

    ::osl::Condition& SAL_CALL  getDirectoryCondition() { return m_aGetDirectoryCondition; }
    ::rtl::OUString SAL_CALL    getDirectory();

    ::osl::Condition& SAL_CALL  getFilesCondition() { return m_aGetFilesCondition; }
    ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getFiles();

    ::osl::Condition& SAL_CALL  getValueCondition() { return m_aGetValueCondition; }
    ::com::sun::star::uno::Any SAL_CALL getValue();

protected:
    virtual void SAL_CALL       run();

    virtual void SAL_CALL       handleCommand( const ::rtl::OUString &rCommand/*, sal_Bool &rQuit*/ );
    ::std::list< ::rtl::OUString > SAL_CALL tokenize( const ::rtl::OUString &rCommand );
};

#endif // _UNXCOMMANDTHREAD_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

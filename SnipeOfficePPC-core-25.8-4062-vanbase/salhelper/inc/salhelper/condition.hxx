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


#ifndef _SALHELPER_CONDITION_HXX_
#define _SALHELPER_CONDITION_HXX_


#include <osl/conditn.h>
#include <osl/mutex.hxx>
#include "salhelperdllapi.h"

namespace salhelper
{
    class ConditionModifier;
    class ConditionWaiter;


    class SALHELPER_DLLPUBLIC Condition
    {
        friend class ConditionModifier;
        friend class ConditionWaiter;

    public:

        Condition(osl::Mutex& aMutex);

        virtual ~Condition();


    protected:

        virtual bool applies() const = 0;


    private:
        SALHELPER_DLLPRIVATE Condition(Condition &); // not defined
        SALHELPER_DLLPRIVATE void operator =(Condition &); // not defined

        osl::Mutex&  m_aMutex;
        oslCondition m_aCondition;
    };



    class SALHELPER_DLLPUBLIC ConditionModifier
    {
    public:

        ConditionModifier(Condition& aCond);

        ~ConditionModifier();


    private:
        SALHELPER_DLLPRIVATE ConditionModifier(ConditionModifier &); // not defined
        SALHELPER_DLLPRIVATE void operator =(ConditionModifier &); // not defined

        Condition& m_aCond;
    };



    class SALHELPER_DLLPUBLIC ConditionWaiter
    {
    public:

        ConditionWaiter(Condition& aCond);

        struct SALHELPER_DLLPUBLIC timedout {
            timedout();

            timedout(timedout const &);

            virtual ~timedout();

            timedout & operator =(timedout const &);
        };

        ConditionWaiter(Condition& aCond,sal_uInt32 milliSec)
            throw(
                timedout
            );


        ~ConditionWaiter();


    private:
        SALHELPER_DLLPRIVATE ConditionWaiter(ConditionWaiter &); // not defined
        SALHELPER_DLLPRIVATE void operator =(ConditionWaiter &); // not defined

        Condition& m_aCond;
    };


}   // namespace salhelper


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

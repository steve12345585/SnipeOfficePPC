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

#include <svx/sdr/event/eventhandler.hxx>

// for SOLARIS compiler include of algorithm part of _STL is necesary to
// get access to basic algos like ::std::find
#include <algorithm>
#include <tools/debug.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace event
    {
        BaseEvent::BaseEvent(EventHandler& rEventHandler)
        :   mrEventHandler(rEventHandler)
        {
            mrEventHandler.AddEvent(*this);
        }

        BaseEvent::~BaseEvent()
        {
            mrEventHandler.RemoveEvent(*this);
        }
    } // end of namespace mixer
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace event
    {
        void EventHandler::AddEvent(BaseEvent& rBaseEvent)
        {
            maVector.push_back(&rBaseEvent);
        }

        void EventHandler::RemoveEvent(BaseEvent& rBaseEvent)
        {
            if(maVector.back() == &rBaseEvent)
            {
                // the one to remove is the last, pop
                maVector.pop_back();
            }
            else
            {
                const BaseEventVector::iterator aFindResult = ::std::find(
                    maVector.begin(), maVector.end(), &rBaseEvent);
                DBG_ASSERT(aFindResult != maVector.end(),
                    "EventHandler::RemoveEvent: Event to be removed not found (!)");
                maVector.erase(aFindResult);
            }
        }

        BaseEvent* EventHandler::GetEvent()
        {
            if(!maVector.empty())
            {
                // get the last event, that one is fastest to be removed
                return maVector.back();
            }
            else
            {
                return 0L;
            }
        }

        EventHandler::EventHandler()
        {
        }

        EventHandler::~EventHandler()
        {
            while(!maVector.empty())
            {
                delete GetEvent();
            }
        }

        // Trigger and consume the events
        void EventHandler::ExecuteEvents()
        {
            for(;;)
            {
                BaseEvent* pEvent = GetEvent();
                if(pEvent == NULL)
                    break;
                pEvent->ExecuteEvent();
                delete pEvent;
            }
        }

        // for control
        sal_Bool EventHandler::IsEmpty() const
        {
            return (0L == maVector.size());
        }
    } // end of namespace mixer
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
    namespace event
    {
        TimerEventHandler::TimerEventHandler(sal_uInt32 nTimeout)
        {
            SetTimeout(nTimeout);
            Stop();
        }

        TimerEventHandler::~TimerEventHandler()
        {
            Stop();
        }

        // The timer when it is triggered; from class Timer
        void TimerEventHandler::Timeout()
        {
            ExecuteEvents();
        }

        // reset the timer
        void TimerEventHandler::Restart()
        {
            Start();
        }
    } // end of namespace mixer
} // end of namespace sdr

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

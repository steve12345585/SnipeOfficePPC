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

#ifndef COMPHELPER_ASYNCNOTIFICATION_HXX
#define COMPHELPER_ASYNCNOTIFICATION_HXX

#include "sal/config.h"

#include "boost/scoped_ptr.hpp"
#include "comphelper/comphelperdllapi.h"
#include "rtl/ref.hxx"
#include "sal/types.h"
#include "salhelper/thread.hxx"

//........................................................................
namespace comphelper
{
//........................................................................

    //====================================================================
    //= AnyEvent
    //====================================================================
    /** the very basic instance to hold a description of an event
    */
    class COMPHELPER_DLLPUBLIC AnyEvent : ::rtl::IReference
    {
    private:
        oslInterlockedCount m_refCount;

    public:
        AnyEvent();

        virtual oslInterlockedCount SAL_CALL acquire();
        virtual oslInterlockedCount SAL_CALL release();

    protected:
        virtual ~AnyEvent();

    private:
        AnyEvent( AnyEvent& ); // not defined
        void operator=( AnyEvent& ); // not defined
    };

    //====================================================================
    //= typedefs
    //====================================================================
    typedef ::rtl::Reference< AnyEvent >    AnyEventRef;

    //====================================================================
    //= IEventProcessor
    //====================================================================
    /** an event processor

        @see AsyncEventNotifier
    */
    class SAL_NO_VTABLE IEventProcessor
    {
    public:
        /** process a single event
        */
        virtual void processEvent( const AnyEvent& _rEvent ) = 0;

        virtual void SAL_CALL acquire() = 0;
        virtual void SAL_CALL release() = 0;

    protected:
        ~IEventProcessor() {}
    };

    //====================================================================
    //= AsyncEventNotifier
    //====================================================================
    struct EventNotifierImpl;

    /** a helper class for notifying events asynchronously

        If you need to notify certain events to external components, you usually should
        not do this while you have mutexes locked, to prevent multi-threading issues.

        However, you do not always have complete control over all mutex guards on the stack.
        If, in such a case, the listener notification is one-way, you can decide to do it
        asynchronously.

        The ->AsyncEventNotifier helps you to process such events asynchronously. Every
        event is tied to an ->IEventProcessor which is responsible for processing it.

        The AsyncEventNotifier is implemented as a thread itself, which sleeps as long as there are no
        events in the queue. As soon as you add an event, the thread is woken up, processes the event,
        and sleeps again.
    */
    class COMPHELPER_DLLPUBLIC AsyncEventNotifier: public salhelper::Thread
    {
        friend struct EventNotifierImpl;

    private:
        boost::scoped_ptr< EventNotifierImpl >        m_pImpl;

        SAL_DLLPRIVATE virtual ~AsyncEventNotifier();

        // Thread
        SAL_DLLPRIVATE virtual void execute();

    public:
        /** constructs a notifier thread

            @param name the thread name, see ::osl_setThreadName; must not be
            null
        */
        AsyncEventNotifier(char const * name);

        /** terminates the thread

            Note that this is a cooporative termination - if you call this from a thread different
            from the notification thread itself, then it will block until the notification thread
            finished processing the current event. If you call it from the notification thread
            itself, it will return immediately, and the thread will be terminated as soon as
            the current notification is finished.
        */
        virtual void SAL_CALL terminate();

        /** adds an event to the queue, together with the instance which is responsible for
            processing it

            @param _rEvent
                the event to add to the queue
            @param _xProcessor
                the processor for the event.<br/>
                Beware of life time issues here. If your event processor dies or becomes otherwise
                nonfunctional, you are responsible for removing all respective events from the queue.
                You can do this by calling ->removeEventsForProcessor
        */
        void addEvent( const AnyEventRef& _rEvent, const ::rtl::Reference< IEventProcessor >& _xProcessor );

        /** removes all events for the given event processor from the queue
        */
        void removeEventsForProcessor( const ::rtl::Reference< IEventProcessor >& _xProcessor );
    };

    //====================================================================
    //= EventHolder
    //====================================================================
    /** AnyEvent derivee holding an foreign event instance
    */
    template < typename EVENT_OBJECT >
    class EventHolder : public AnyEvent
    {
    public:
        typedef EVENT_OBJECT    EventObjectType;

    private:
        EventObjectType m_aEvent;

    public:
        inline EventHolder( const EventObjectType& _rEvent )
            :m_aEvent( _rEvent )
        {
        }

        inline const EventObjectType& getEventObject() const { return m_aEvent; }
    };

//........................................................................
} // namespace comphelper
//........................................................................

#endif // COMPHELPER_ASYNCNOTIFICATION_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

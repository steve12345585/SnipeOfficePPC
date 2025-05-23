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

#ifndef SVX_DELAYEDLINK_HXX
#define SVX_DELAYEDLINK_HXX

#include <tools/link.hxx>

//........................................................................
namespace svxform
{
//........................................................................

    //====================================================================
    //= DelayedEvent
    //====================================================================
    /** small class which encapsulates posting a Link instance as ApplicationUserEvent

        No thread safety at all here, just a little saving of code to type multiple times
    */
    class DelayedEvent
    {
    public:
        DelayedEvent( const Link& _rHandler )
            :m_aHandler( _rHandler )
            ,m_nEventId( 0 )
        {
        }

        ~DelayedEvent()
        {
            CancelPendingCall();
        }

        /** calls the handler asynchronously

            If there's already a call pending, this previous call is cancelled.
        */
        void    Call( void* _pArg = NULL );

        /** cancels a call which is currently pending

            If no call is currently pending, then this is ignored.
        */
        void    CancelPendingCall();

    private:
        Link    m_aHandler;
        sal_uLong   m_nEventId;

    private:
        DECL_LINK( OnCall, void* );

    private:
        DelayedEvent();                                  // never implemented
        DelayedEvent( const DelayedEvent& );              // never implemented
        DelayedEvent& operator=( const DelayedEvent& );   // never implemented
    };

//........................................................................
} // namespace svxform
//........................................................................

#endif // SVX_DELAYEDLINK_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

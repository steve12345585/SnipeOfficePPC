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

#include "accessibility/extended/accessibletabbarbase.hxx"
#ifndef ACCESSIBILITY_EXT_ACCESSIBLETABBARPAGELIST
#include "accessibility/extended/accessibletabbarpagelist.hxx"
#endif
#include <toolkit/helper/externallock.hxx>
#include <svtools/tabbar.hxx>

//.........................................................................
namespace accessibility
{
//.........................................................................

AccessibleTabBarBase::AccessibleTabBarBase( TabBar* pTabBar ) :
    AccessibleExtendedComponentHelper_BASE( new VCLExternalSolarLock() ),
    m_pTabBar( 0 )
{
    m_pExternalLock = static_cast< VCLExternalSolarLock* >( getExternalLock() );
    SetTabBarPointer( pTabBar );
}

AccessibleTabBarBase::~AccessibleTabBarBase()
{
    ClearTabBarPointer();
    DELETEZ( m_pExternalLock );
}

IMPL_LINK( AccessibleTabBarBase, WindowEventListener, VclSimpleEvent*, pEvent )
{
    VclWindowEvent* pWinEvent = dynamic_cast< VclWindowEvent* >( pEvent );
    OSL_ENSURE( pWinEvent, "AccessibleTabBarBase::WindowEventListener - unknown window event" );
    if( pWinEvent )
    {
        Window* pEventWindow = pWinEvent->GetWindow();
        OSL_ENSURE( pEventWindow, "AccessibleTabBarBase::WindowEventListener: no window!" );

        if( ( pWinEvent->GetId() == VCLEVENT_TABBAR_PAGEREMOVED ) &&
            ( (sal_uInt16)(sal_IntPtr) pWinEvent->GetData() == TabBar::PAGE_NOT_FOUND ) &&
            ( dynamic_cast< AccessibleTabBarPageList *> (this) != NULL ) )
        {
            return 0;
        }

        if ( !pEventWindow->IsAccessibilityEventsSuppressed() || (pWinEvent->GetId() == VCLEVENT_OBJECT_DYING) )
            ProcessWindowEvent( *pWinEvent );
    }
    return 0;
}

void AccessibleTabBarBase::ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent )
{
    if( rVclWindowEvent.GetId() == VCLEVENT_OBJECT_DYING )
        ClearTabBarPointer();
}

// XComponent

void AccessibleTabBarBase::disposing()
{
    AccessibleExtendedComponentHelper_BASE::disposing();
    ClearTabBarPointer();
}

// private

void AccessibleTabBarBase::SetTabBarPointer( TabBar* pTabBar )
{
    OSL_ENSURE( !m_pTabBar, "AccessibleTabBarBase::SetTabBarPointer - multiple call" );
    m_pTabBar = pTabBar;
    if( m_pTabBar )
        m_pTabBar->AddEventListener( LINK( this, AccessibleTabBarBase, WindowEventListener ) );
}

void AccessibleTabBarBase::ClearTabBarPointer()
{
    if( m_pTabBar )
    {
        m_pTabBar->RemoveEventListener( LINK( this, AccessibleTabBarBase, WindowEventListener ) );
        m_pTabBar = 0;
    }
}

//.........................................................................
}   // namespace accessibility
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

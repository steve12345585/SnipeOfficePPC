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

#include <accessibility/extended/listboxaccessible.hxx>
#include <svtools/svtreebx.hxx>

//........................................................................
namespace accessibility
{
//........................................................................

    //====================================================================
    //= ListBoxAccessibleBase
    //====================================================================
    //--------------------------------------------------------------------
    ListBoxAccessibleBase::ListBoxAccessibleBase( SvTreeListBox& _rWindow )
        :m_pWindow( &_rWindow )
    {
        m_pWindow->AddEventListener( LINK( this, ListBoxAccessibleBase, WindowEventListener ) );
    }

    //--------------------------------------------------------------------
    ListBoxAccessibleBase::~ListBoxAccessibleBase( )
    {
        if ( m_pWindow )
        {
            // cannot call "dispose" here, as it is abstract, so the VTABLE of the derived class
            // is not intact anymore
            // so we call our "disposing" only
            disposing();
        }
    }

    //--------------------------------------------------------------------
    IMPL_LINK( ListBoxAccessibleBase, WindowEventListener, VclSimpleEvent*, pEvent )
    {
        DBG_ASSERT( pEvent && pEvent->ISA( VclWindowEvent ), "ListBoxAccessibleBase::WindowEventListener: unexpected WindowEvent!" );
        if ( pEvent && pEvent->ISA( VclWindowEvent ) )
        {
            DBG_ASSERT( static_cast< VclWindowEvent* >( pEvent )->GetWindow() , "ListBoxAccessibleBase::WindowEventListener: no event window!" );
            DBG_ASSERT( static_cast< VclWindowEvent* >( pEvent )->GetWindow() == m_pWindow, "ListBoxAccessibleBase::WindowEventListener: where did this come from?" );

            ProcessWindowEvent( *static_cast< VclWindowEvent* >( pEvent ) );
        }
        return 0;
    }

    // -----------------------------------------------------------------------------
    void ListBoxAccessibleBase::disposing()
    {
        if ( m_pWindow )
            m_pWindow->RemoveEventListener( LINK( this, ListBoxAccessibleBase, WindowEventListener ) );
        m_pWindow = NULL;
    }

    // -----------------------------------------------------------------------------
    void ListBoxAccessibleBase::ProcessWindowEvent( const VclWindowEvent& _rVclWindowEvent )
    {
        if ( isAlive() )
        {
            switch ( _rVclWindowEvent.GetId() )
            {
                case  VCLEVENT_OBJECT_DYING :
                {
                    if ( m_pWindow )
                        m_pWindow->RemoveEventListener( LINK( this, ListBoxAccessibleBase, WindowEventListener ) );
                    m_pWindow = NULL;
                    dispose();
                    break;
                }
            }
        }
    }

//........................................................................
}   // namespace accessibility
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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


// includes --------------------------------------------------------------
#include <comphelper/accessiblekeybindinghelper.hxx>


//..............................................................................
namespace comphelper
{
//..............................................................................

    using namespace ::com::sun::star; // MT 04/2003: was ::drafts::com::sun::star - otherwise to many changes
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::accessibility;

    //==============================================================================
    // OAccessibleKeyBindingHelper
    //==============================================================================

    OAccessibleKeyBindingHelper::OAccessibleKeyBindingHelper()
    {
    }

    // -----------------------------------------------------------------------------

    OAccessibleKeyBindingHelper::OAccessibleKeyBindingHelper( const OAccessibleKeyBindingHelper& rHelper )
        : cppu::WeakImplHelper1<XAccessibleKeyBinding>( rHelper )
        , m_aKeyBindings( rHelper.m_aKeyBindings )
    {
    }

    // -----------------------------------------------------------------------------

    OAccessibleKeyBindingHelper::~OAccessibleKeyBindingHelper()
    {
    }

    // -----------------------------------------------------------------------------

    void OAccessibleKeyBindingHelper::AddKeyBinding( const Sequence< awt::KeyStroke >& rKeyBinding ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        m_aKeyBindings.push_back( rKeyBinding );
    }

    // -----------------------------------------------------------------------------

    void OAccessibleKeyBindingHelper::AddKeyBinding( const awt::KeyStroke& rKeyStroke ) throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        Sequence< awt::KeyStroke > aSeq(1);
        aSeq[0] = rKeyStroke;
        m_aKeyBindings.push_back( aSeq );
    }

    // -----------------------------------------------------------------------------
    // XAccessibleKeyBinding
    // -----------------------------------------------------------------------------

    sal_Int32 OAccessibleKeyBindingHelper::getAccessibleKeyBindingCount() throw (RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        return m_aKeyBindings.size();
    }

    // -----------------------------------------------------------------------------

    Sequence< awt::KeyStroke > OAccessibleKeyBindingHelper::getAccessibleKeyBinding( sal_Int32 nIndex ) throw (IndexOutOfBoundsException, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if ( nIndex < 0 || nIndex >= (sal_Int32)m_aKeyBindings.size() )
            throw IndexOutOfBoundsException();

        return m_aKeyBindings[nIndex];
    }

    // -----------------------------------------------------------------------------

//..............................................................................
}   // namespace comphelper
//..............................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#include <accessibility/standard/vclxaccessiblemenu.hxx>

#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <vcl/menu.hxx>


using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::accessibility;
using namespace ::comphelper;


// -----------------------------------------------------------------------------
// VCLXAccessibleMenu
// -----------------------------------------------------------------------------

VCLXAccessibleMenu::VCLXAccessibleMenu( Menu* pParent, sal_uInt16 nItemPos, Menu* pMenu )
    :VCLXAccessibleMenuItem( pParent, nItemPos, pMenu )
{
}

// -----------------------------------------------------------------------------

VCLXAccessibleMenu::~VCLXAccessibleMenu()
{
}

// -----------------------------------------------------------------------------

sal_Bool VCLXAccessibleMenu::IsFocused()
{
    sal_Bool bFocused = sal_False;

    if ( IsHighlighted() && !IsChildHighlighted() )
        bFocused = sal_True;

    return bFocused;
}

// -----------------------------------------------------------------------------

sal_Bool VCLXAccessibleMenu::IsPopupMenuOpen()
{
    sal_Bool bPopupMenuOpen = sal_False;

    if ( m_pParent )
    {
        PopupMenu* pPopupMenu = m_pParent->GetPopupMenu( m_pParent->GetItemId( m_nItemPos ) );
        if ( pPopupMenu && pPopupMenu->IsMenuVisible() )
            bPopupMenuOpen = sal_True;
    }

    return bPopupMenuOpen;
}

// -----------------------------------------------------------------------------
// XInterface
// -----------------------------------------------------------------------------

IMPLEMENT_FORWARD_XINTERFACE2( VCLXAccessibleMenu, VCLXAccessibleMenuItem, VCLXAccessibleMenu_BASE )

// -----------------------------------------------------------------------------
// XTypeProvider
// -----------------------------------------------------------------------------

IMPLEMENT_FORWARD_XTYPEPROVIDER2( VCLXAccessibleMenu, VCLXAccessibleMenuItem, VCLXAccessibleMenu_BASE )

// -----------------------------------------------------------------------------
// XServiceInfo
// -----------------------------------------------------------------------------

OUString VCLXAccessibleMenu::getImplementationName() throw (RuntimeException)
{
    return OUString( "com.sun.star.comp.toolkit.AccessibleMenu" );
}

// -----------------------------------------------------------------------------

Sequence< OUString > VCLXAccessibleMenu::getSupportedServiceNames() throw (RuntimeException)
{
    Sequence< OUString > aNames(1);
    aNames[0] = "com.sun.star.awt.AccessibleMenu";
    return aNames;
}

// -----------------------------------------------------------------------------
// XAccessibleContext
// -----------------------------------------------------------------------------

sal_Int32 VCLXAccessibleMenu::getAccessibleChildCount(  ) throw (RuntimeException)
{
    OExternalLockGuard aGuard( this );

    return GetChildCount();
}

// -----------------------------------------------------------------------------

Reference< XAccessible > VCLXAccessibleMenu::getAccessibleChild( sal_Int32 i ) throw (IndexOutOfBoundsException, RuntimeException)
{
    OExternalLockGuard aGuard( this );

    if ( i < 0 || i >= GetChildCount() )
        throw IndexOutOfBoundsException();

    return GetChild( i );
}

// -----------------------------------------------------------------------------

sal_Int16 VCLXAccessibleMenu::getAccessibleRole(  ) throw (RuntimeException)
{
    OExternalLockGuard aGuard( this );

    return AccessibleRole::MENU;
}

// -----------------------------------------------------------------------------
// XAccessibleComponent
// -----------------------------------------------------------------------------

Reference< XAccessible > VCLXAccessibleMenu::getAccessibleAtPoint( const awt::Point& rPoint ) throw (RuntimeException)
{
    OExternalLockGuard aGuard( this );

    return GetChildAt( rPoint );
}

// -----------------------------------------------------------------------------
// XAccessibleSelection
// -----------------------------------------------------------------------------

void VCLXAccessibleMenu::selectAccessibleChild( sal_Int32 nChildIndex ) throw (IndexOutOfBoundsException, RuntimeException)
{
    OExternalLockGuard aGuard( this );

    if ( nChildIndex < 0 || nChildIndex >= GetChildCount() )
        throw IndexOutOfBoundsException();

    SelectChild( nChildIndex );
}

// -----------------------------------------------------------------------------

sal_Bool VCLXAccessibleMenu::isAccessibleChildSelected( sal_Int32 nChildIndex ) throw (IndexOutOfBoundsException, RuntimeException)
{
    OExternalLockGuard aGuard( this );

    if ( nChildIndex < 0 || nChildIndex >= GetChildCount() )
        throw IndexOutOfBoundsException();

    return IsChildSelected( nChildIndex );
}

// -----------------------------------------------------------------------------

void VCLXAccessibleMenu::clearAccessibleSelection(  ) throw (RuntimeException)
{
    OExternalLockGuard aGuard( this );

    DeSelectAll();
}

// -----------------------------------------------------------------------------

void VCLXAccessibleMenu::selectAllAccessibleChildren(  ) throw (RuntimeException)
{
    // This method makes no sense in a menu, and so does nothing.
}

// -----------------------------------------------------------------------------

sal_Int32 VCLXAccessibleMenu::getSelectedAccessibleChildCount(  ) throw (RuntimeException)
{
    OExternalLockGuard aGuard( this );

    sal_Int32 nRet = 0;

    for ( sal_Int32 i = 0, nCount = GetChildCount(); i < nCount; i++ )
    {
        if ( IsChildSelected( i ) )
            ++nRet;
    }

    return nRet;
}

// -----------------------------------------------------------------------------

Reference< XAccessible > VCLXAccessibleMenu::getSelectedAccessibleChild( sal_Int32 nSelectedChildIndex ) throw (IndexOutOfBoundsException, RuntimeException)
{
    OExternalLockGuard aGuard( this );

    if ( nSelectedChildIndex < 0 || nSelectedChildIndex >= getSelectedAccessibleChildCount() )
        throw IndexOutOfBoundsException();

    Reference< XAccessible > xChild;

    for ( sal_Int32 i = 0, j = 0, nCount = GetChildCount(); i < nCount; i++ )
    {
        if ( IsChildSelected( i ) && ( j++ == nSelectedChildIndex ) )
        {
            xChild = GetChild( i );
            break;
        }
    }

    return xChild;
}

// -----------------------------------------------------------------------------

void VCLXAccessibleMenu::deselectAccessibleChild( sal_Int32 nChildIndex ) throw (IndexOutOfBoundsException, RuntimeException)
{
    OExternalLockGuard aGuard( this );

    if ( nChildIndex < 0 || nChildIndex >= GetChildCount() )
        throw IndexOutOfBoundsException();

    DeSelectAll();
}

// -----------------------------------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#include <comphelper/accessiblecomponenthelper.hxx>

//.........................................................................
namespace comphelper
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::awt;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::accessibility;

    //=====================================================================
    //= OCommonAccessibleComponent
    //=====================================================================
    //---------------------------------------------------------------------
    OCommonAccessibleComponent::OCommonAccessibleComponent( )
    {
    }

    //---------------------------------------------------------------------
    OCommonAccessibleComponent::OCommonAccessibleComponent( IMutex* _pExternalLock )
        :OAccessibleContextHelper( _pExternalLock )
    {
    }

    //---------------------------------------------------------------------
    OCommonAccessibleComponent::~OCommonAccessibleComponent( )
    {
        forgetExternalLock();
            // this ensures that the lock, which may be already destroyed as part of the derivee,
            // is not used anymore
    }

    //--------------------------------------------------------------------
    sal_Bool SAL_CALL OCommonAccessibleComponent::containsPoint( const Point& _rPoint ) throw (RuntimeException)
    {
        OExternalLockGuard aGuard( this );
        Rectangle aBounds( implGetBounds() );
        return  ( _rPoint.X >= 0 )
            &&  ( _rPoint.Y >= 0 )
            &&  ( _rPoint.X < aBounds.Width )
            &&  ( _rPoint.Y < aBounds.Height );
    }

    //--------------------------------------------------------------------
    Point SAL_CALL OCommonAccessibleComponent::getLocation(  ) throw (RuntimeException)
    {
        OExternalLockGuard aGuard( this );
        Rectangle aBounds( implGetBounds() );
        return Point( aBounds.X, aBounds.Y );
    }

    //--------------------------------------------------------------------
    Point SAL_CALL OCommonAccessibleComponent::getLocationOnScreen(  ) throw (RuntimeException)
    {
        OExternalLockGuard aGuard( this );

        Point aScreenLoc( 0, 0 );

        Reference< XAccessibleComponent > xParentComponent( implGetParentContext(), UNO_QUERY );
        OSL_ENSURE( xParentComponent.is(), "OCommonAccessibleComponent::getLocationOnScreen: no parent component!" );
        if ( xParentComponent.is() )
        {
            Point aParentScreenLoc( xParentComponent->getLocationOnScreen() );
            Point aOwnRelativeLoc( getLocation() );
            aScreenLoc.X = aParentScreenLoc.X + aOwnRelativeLoc.X;
            aScreenLoc.Y = aParentScreenLoc.Y + aOwnRelativeLoc.Y;
        }

        return aScreenLoc;
    }

    //--------------------------------------------------------------------
    Size SAL_CALL OCommonAccessibleComponent::getSize(  ) throw (RuntimeException)
    {
        OExternalLockGuard aGuard( this );
        Rectangle aBounds( implGetBounds() );
        return Size( aBounds.Width, aBounds.Height );
    }

    //--------------------------------------------------------------------
    Rectangle SAL_CALL OCommonAccessibleComponent::getBounds(  ) throw (RuntimeException)
    {
        OExternalLockGuard aGuard( this );
        return implGetBounds();
    }

    //=====================================================================
    //= OAccessibleComponentHelper
    //=====================================================================
    //---------------------------------------------------------------------
    OAccessibleComponentHelper::OAccessibleComponentHelper( )
    {
    }

    //---------------------------------------------------------------------
    OAccessibleComponentHelper::OAccessibleComponentHelper( IMutex* _pExternalLock )
        :OCommonAccessibleComponent( _pExternalLock )
    {
    }

    //--------------------------------------------------------------------
    IMPLEMENT_FORWARD_XINTERFACE2( OAccessibleComponentHelper, OCommonAccessibleComponent, OAccessibleComponentHelper_Base )
    IMPLEMENT_FORWARD_XTYPEPROVIDER2( OAccessibleComponentHelper, OCommonAccessibleComponent, OAccessibleComponentHelper_Base )
        // (order matters: the first is the class name, the second is the class doing the ref counting)

    //--------------------------------------------------------------------
    sal_Bool SAL_CALL OAccessibleComponentHelper::containsPoint( const Point& _rPoint ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::containsPoint( _rPoint );
    }

    //--------------------------------------------------------------------
    Point SAL_CALL OAccessibleComponentHelper::getLocation(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getLocation( );
    }

    //--------------------------------------------------------------------
    Point SAL_CALL OAccessibleComponentHelper::getLocationOnScreen(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getLocationOnScreen( );
    }

    //--------------------------------------------------------------------
    Size SAL_CALL OAccessibleComponentHelper::getSize(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getSize( );
    }

    //--------------------------------------------------------------------
    Rectangle SAL_CALL OAccessibleComponentHelper::getBounds(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getBounds( );
    }

    //=====================================================================
    //= OAccessibleExtendedComponentHelper
    //=====================================================================
    //---------------------------------------------------------------------
    OAccessibleExtendedComponentHelper::OAccessibleExtendedComponentHelper( )
    {
    }

    //---------------------------------------------------------------------
    OAccessibleExtendedComponentHelper::OAccessibleExtendedComponentHelper( IMutex* _pExternalLock )
        :OCommonAccessibleComponent( _pExternalLock )
    {
    }

    //--------------------------------------------------------------------
    IMPLEMENT_FORWARD_XINTERFACE2( OAccessibleExtendedComponentHelper, OCommonAccessibleComponent, OAccessibleExtendedComponentHelper_Base )
    IMPLEMENT_FORWARD_XTYPEPROVIDER2( OAccessibleExtendedComponentHelper, OCommonAccessibleComponent, OAccessibleExtendedComponentHelper_Base )
        // (order matters: the first is the class name, the second is the class doing the ref counting)

    //--------------------------------------------------------------------
    sal_Bool SAL_CALL OAccessibleExtendedComponentHelper::containsPoint( const Point& _rPoint ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::containsPoint( _rPoint );
    }

    //--------------------------------------------------------------------
    Point SAL_CALL OAccessibleExtendedComponentHelper::getLocation(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getLocation( );
    }

    //--------------------------------------------------------------------
    Point SAL_CALL OAccessibleExtendedComponentHelper::getLocationOnScreen(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getLocationOnScreen( );
    }

    //--------------------------------------------------------------------
    Size SAL_CALL OAccessibleExtendedComponentHelper::getSize(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getSize( );
    }

    //--------------------------------------------------------------------
    Rectangle SAL_CALL OAccessibleExtendedComponentHelper::getBounds(  ) throw (RuntimeException)
    {
        return OCommonAccessibleComponent::getBounds( );
    }

//.........................................................................
}   // namespace comphelper
//.........................................................................


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

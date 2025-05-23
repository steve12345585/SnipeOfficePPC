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
#ifndef _UTL_ACCESSIBLESTATESETHELPER_HXX_
#define _UTL_ACCESSIBLESTATESETHELPER_HXX_

#include "unotools/unotoolsdllapi.h"

#include <com/sun/star/accessibility/XAccessibleStateSet.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include <cppuhelper/weak.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XServiceName.hpp>
#include <osl/mutex.hxx>
#include <cppuhelper/implbase1.hxx>
#include <comphelper/servicehelper.hxx>

class AccessibleStateSetHelperImpl;

//=========================================================================
//= XAccessibleStateSet helper classes
//=========================================================================

//... namespace utl .......................................................
namespace utl
{
//.........................................................................

/** @descr
        This base class provides an implementation of the
        <code>AccessibleStateSet</code> service.
*/
class UNOTOOLS_DLLPUBLIC AccessibleStateSetHelper
    :   public cppu::WeakImplHelper1<
        ::com::sun::star::accessibility::XAccessibleStateSet
        >
{
public:
    //=====  internal  ========================================================

    AccessibleStateSetHelper ();
    /** constructs an object with some states initially set

        <p>This ctor is compatible with
        <method scope="comphelper">OAccessibleImplementationAccess::implGetForeignControlledStates</method></p>

        @param _nInitialStates
            is a bit mask. Every bit 2^n means that the state number n (as got from the
            AccessibleStateType constants) should be set initially.
    */
    AccessibleStateSetHelper ( const sal_Int64 _nInitialStates );

    AccessibleStateSetHelper ( const AccessibleStateSetHelper& rHelper );
protected:
    virtual ~AccessibleStateSetHelper   (void);
public:

    //=====  XAccessibleStateSet  ==============================================

    /** Checks whether the current state set is empty.

        @return
            Returns <TRUE/> if there is no state in this state set and
            <FALSE/> if there is at least one state set in it.
    */
    virtual sal_Bool SAL_CALL isEmpty ()
        throw (::com::sun::star::uno::RuntimeException);

    /** Checks if the given state is a member of the state set of this
        object.

        @param aState
            The state for which to check membership.  This has to be one of
            the constants of <type>AccessibleStateType</type>.

        @return
            Returns <TRUE/> if the given state is a memeber of this object's
            state set and <FALSE/> otherwise.
    */
    virtual sal_Bool SAL_CALL contains (sal_Int16 aState)
        throw (::com::sun::star::uno::RuntimeException);

    /** Checks if all of the given states are in this object's state
        set.

        @param aStateSet
            This sequence of states is interpreted as set and every of its
            members, duplicates are ignored, is checked for membership in
            this object's state set.  Each state has to be one of the
            constants of <type>AccessibleStateType</type>.

        @return
            Returns <TRUE/> if all states of the given state set are members
            of this object's state set.  <FALSE/> is returned if at least
            one of the states in the given state is not a member of this
            object's state set.
    */
    virtual sal_Bool SAL_CALL containsAll (
        const ::com::sun::star::uno::Sequence<sal_Int16>& rStateSet)
        throw (::com::sun::star::uno::RuntimeException);

    /** Returns a sequence of all states.
    */
    virtual com::sun::star::uno::Sequence<sal_Int16> SAL_CALL getStates()
        throw (::com::sun::star::uno::RuntimeException);

    /** Adds a state to the set.
    */
    void    AddState(sal_Int16 aState)
        throw (::com::sun::star::uno::RuntimeException);

    /** Removes a state from the set if the set contains the state, otherwise nothing is done.
    */
    void    RemoveState(sal_Int16 aState)
        throw (::com::sun::star::uno::RuntimeException);

    //=====  XTypeProvider  ===================================================

    /** Returns a sequence of all supported interfaces.
    */
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> SAL_CALL
        getTypes (void)
        throw (::com::sun::star::uno::RuntimeException);

    /** Returns a implementation id.
    */
    virtual ::com::sun::star::uno::Sequence<sal_Int8> SAL_CALL
        getImplementationId (void)
        throw (::com::sun::star::uno::RuntimeException);

protected:
    /// Mutex guarding this object.
    ::osl::Mutex maMutex;

private:
    /// The implementation of this helper interface.
    AccessibleStateSetHelperImpl*   mpHelperImpl;
};

//.........................................................................
}
//... namespace utl .......................................................
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

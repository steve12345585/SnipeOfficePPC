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

#include "unotools/accessiblestatesethelper.hxx"
#include <tools/debug.hxx>
#include <comphelper/servicehelper.hxx>

// defines how many states the bitfield can contain
// it has the size of 64 because I use a uInt64
#define BITFIELDSIZE 64

using namespace ::utl;
using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::accessibility;

class AccessibleStateSetHelperImpl
{
public:
    AccessibleStateSetHelperImpl();
    AccessibleStateSetHelperImpl(const AccessibleStateSetHelperImpl& rImpl);
    ~AccessibleStateSetHelperImpl();

    sal_Bool IsEmpty () const
        throw (uno::RuntimeException);
    sal_Bool Contains (sal_Int16 aState) const
        throw (uno::RuntimeException);
    uno::Sequence<sal_Int16> GetStates() const
        throw (uno::RuntimeException);
    void AddState(sal_Int16 aState)
        throw (uno::RuntimeException);
    void RemoveState(sal_Int16 aState)
        throw (uno::RuntimeException);

    inline void AddStates( const sal_Int64 _nStates ) SAL_THROW( ( ) );

private:
    sal_uInt64 maStates;
};

AccessibleStateSetHelperImpl::AccessibleStateSetHelperImpl()
    : maStates(0)
{
}

AccessibleStateSetHelperImpl::AccessibleStateSetHelperImpl(const AccessibleStateSetHelperImpl& rImpl)
    : maStates(rImpl.maStates)
{
}

AccessibleStateSetHelperImpl::~AccessibleStateSetHelperImpl()
{
}

inline sal_Bool AccessibleStateSetHelperImpl::IsEmpty () const
    throw (uno::RuntimeException)
{
    return maStates == 0;
}

inline sal_Bool AccessibleStateSetHelperImpl::Contains (sal_Int16 aState) const
    throw (uno::RuntimeException)
{
    DBG_ASSERT(aState < BITFIELDSIZE, "the statesset is too small");
    sal_uInt64 aTempBitSet(1);
    aTempBitSet <<= aState;
    return ((aTempBitSet & maStates) != 0);
}

inline uno::Sequence<sal_Int16> AccessibleStateSetHelperImpl::GetStates() const
    throw (uno::RuntimeException)
{
    uno::Sequence<sal_Int16> aRet(BITFIELDSIZE);
    sal_Int16* pSeq = aRet.getArray();
    sal_Int16 nStateCount(0);
    for (sal_Int16 i = 0; i < BITFIELDSIZE; ++i)
        if (Contains(i))
        {
            *pSeq = i;
            ++pSeq;
            ++nStateCount;
        }
    aRet.realloc(nStateCount);
    return aRet;
}

inline void AccessibleStateSetHelperImpl::AddStates( const sal_Int64 _nStates ) SAL_THROW( ( ) )
{
    maStates |= _nStates;
}

inline void AccessibleStateSetHelperImpl::AddState(sal_Int16 aState)
    throw (uno::RuntimeException)
{
    DBG_ASSERT(aState < BITFIELDSIZE, "the statesset is too small");
    sal_uInt64 aTempBitSet(1);
    aTempBitSet <<= aState;
    maStates |= aTempBitSet;
}

inline void AccessibleStateSetHelperImpl::RemoveState(sal_Int16 aState)
    throw (uno::RuntimeException)
{
    DBG_ASSERT(aState < BITFIELDSIZE, "the statesset is too small");
    sal_uInt64 aTempBitSet(1);
    aTempBitSet <<= aState;
    aTempBitSet = ~aTempBitSet;
    maStates &= aTempBitSet;
}

//=====  internal  ============================================================

AccessibleStateSetHelper::AccessibleStateSetHelper ()
    : mpHelperImpl(NULL)
{
    mpHelperImpl = new AccessibleStateSetHelperImpl();
}

AccessibleStateSetHelper::AccessibleStateSetHelper ( const sal_Int64 _nInitialStates )
    : mpHelperImpl(NULL)
{
    mpHelperImpl = new AccessibleStateSetHelperImpl();
    mpHelperImpl->AddStates( _nInitialStates );
}

AccessibleStateSetHelper::AccessibleStateSetHelper (const AccessibleStateSetHelper& rHelper)
    : cppu::WeakImplHelper1<XAccessibleStateSet>()
    , mpHelperImpl(NULL)
{
    if (rHelper.mpHelperImpl)
        mpHelperImpl = new AccessibleStateSetHelperImpl(*rHelper.mpHelperImpl);
    else
        mpHelperImpl = new AccessibleStateSetHelperImpl();
}

AccessibleStateSetHelper::~AccessibleStateSetHelper(void)
{
    delete mpHelperImpl;
}

//=====  XAccessibleStateSet  ==============================================

    /** Checks whether the current state set is empty.

        @return
            Returns <TRUE/> if there is no state in this state set and
            <FALSE/> if there is at least one state set in it.
    */
sal_Bool SAL_CALL AccessibleStateSetHelper::isEmpty ()
    throw (uno::RuntimeException)
{
    osl::MutexGuard aGuard (maMutex);
    return mpHelperImpl->IsEmpty();
}

    /** Checks if the given state is a member of the state set of this
        object.

        @param aState
            The state for which to check membership.  This has to be one of
            the constants of <type>AccessibleStateType</type>.

        @return
            Returns <TRUE/> if the given state is a memeber of this object's
            state set and <FALSE/> otherwise.
    */
sal_Bool SAL_CALL AccessibleStateSetHelper::contains (sal_Int16 aState)
    throw (uno::RuntimeException)
{
    osl::MutexGuard aGuard (maMutex);
    return mpHelperImpl->Contains(aState);
}

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
sal_Bool SAL_CALL AccessibleStateSetHelper::containsAll
    (const uno::Sequence<sal_Int16>& rStateSet)
    throw (uno::RuntimeException)
{
    osl::MutexGuard aGuard (maMutex);
    sal_Int32 nCount(rStateSet.getLength());
    const sal_Int16* pStates = rStateSet.getConstArray();
    sal_Int32 i = 0;
    sal_Bool bFound(sal_True);
    while (i < nCount)
    {
        bFound = mpHelperImpl->Contains(pStates[i]);
        i++;
    }
    return bFound;
}

uno::Sequence<sal_Int16> SAL_CALL AccessibleStateSetHelper::getStates()
    throw (uno::RuntimeException)
{
    osl::MutexGuard aGuard(maMutex);
    return mpHelperImpl->GetStates();
}

void AccessibleStateSetHelper::AddState(sal_Int16 aState)
    throw (uno::RuntimeException)
{
    osl::MutexGuard aGuard (maMutex);
    mpHelperImpl->AddState(aState);
}

void AccessibleStateSetHelper::RemoveState(sal_Int16 aState)
    throw (uno::RuntimeException)
{
    osl::MutexGuard aGuard (maMutex);
    mpHelperImpl->RemoveState(aState);
}

//=====  XTypeProvider  =======================================================

uno::Sequence< ::com::sun::star::uno::Type>
    AccessibleStateSetHelper::getTypes (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    const ::com::sun::star::uno::Type aTypeList[] = {
        ::getCppuType((const uno::Reference<
            XAccessibleStateSet>*)0),
        ::getCppuType((const uno::Reference<
            lang::XTypeProvider>*)0)
        };
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type>
        aTypeSequence (aTypeList, 2);
    return aTypeSequence;
}

namespace
{
    class theAccessibleStateSetHelperUnoTunnelId : public rtl::Static< UnoTunnelIdInit, theAccessibleStateSetHelperUnoTunnelId > {};
}

uno::Sequence<sal_Int8> SAL_CALL
    AccessibleStateSetHelper::getImplementationId (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return theAccessibleStateSetHelperUnoTunnelId::get().getSeq();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

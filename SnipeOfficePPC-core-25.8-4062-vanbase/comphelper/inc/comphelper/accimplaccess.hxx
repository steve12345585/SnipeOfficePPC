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

#ifndef COMPHELPER_ACCIMPLACCESS_HXX
#define COMPHELPER_ACCIMPLACCESS_HXX

#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include "comphelper/comphelperdllapi.h"

// forward declaration
namespace com { namespace sun { namespace star { namespace accessibility {
    class XAccessible;
    class XAccessibleContext;
}}}}

//.........................................................................
namespace comphelper
{
//.........................................................................

    //=====================================================================
    //= OAccessibleImplementationAccess
    //=====================================================================
    typedef ::cppu::ImplHelper1 <   ::com::sun::star::lang::XUnoTunnel
                                >   OAccImpl_Base;
    struct OAccImpl_Impl;

    /** This is a helper class which allows accessing several aspects of the the implementation
        of an AccessibleContext.

        <p>For instance, when you want to implement a context which can be re-parented, you:
            <ul><li>derive your class from <type>OAccessibleImplementationAccess</type></li>
                <li>use <code>setAccessibleParent( <em>component</em>, <em>new_parent</em> )</code>
            </ul>
        </p>

        <p>Another aspect which can be controlled from the outside are states. If you have a class which
        has only partial control over it's states, you may consider deriving from OAccessibleImplementationAccess.<br/>
        For instance, say you have an implementation (say component A) which is <em>unable</em> to know or to
        determine if the represented object is selected, but another component (say B) which uses A (and integrates
        it into a tree of accessibility components) is.<br/>
        In this case, if A is derived from OAccessibleImplementationAccess, B can manipulate this
        foreign-controlled state flag "SELECTED" by using the static helper methods on this class.</p>

        <p>Please note that the support for foreign controlled states is rather restrictive: You can't have states
        which <em>may be</em> controlled by a foreign instances. This is implied by the fact that a derived
        class can ask for states which are <em>set</em> only, not for the ones which are <em>reset</em> currently.
        </p>
    */
    class COMPHELPER_DLLPUBLIC OAccessibleImplementationAccess : public OAccImpl_Base
    {
    private:
        OAccImpl_Impl*  m_pImpl;

    protected:
        /// retrieves the parent previously set via <method>setAccessibleParent</method>
        ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >
                implGetForeignControlledParent( ) const;

        /** retrieves the set of currently set states which are controlled by a foreign instance
        @return
            a bit mask, where a set bit 2^n means that the AccessibleStateType n has been set
        */
        sal_Int64   implGetForeignControlledStates( ) const;

        /// sets the accessible parent component
        virtual void    setAccessibleParent(
            const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& _rxAccParent );

        /// sets or resets a bit of the foreign controlled states
        virtual void    setStateBit( const sal_Int16 _nState, const sal_Bool _bSet );

    protected:
        OAccessibleImplementationAccess( );
        virtual ~OAccessibleImplementationAccess( );

        // XUnoTunnel
        virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& _rIdentifier ) throw (::com::sun::star::uno::RuntimeException);

    public:
        /** tries to access the implementation of an OAccessibleImplementationAccess derivee which is known as
            interface only.

        @param _rxComponent
            is the component which should be examined.
        @return
            the pointer to the implementation, if successfull. The only known error condition so far
            is an invalid context (which means it is <NULL/>, or the implementation is not derived
            from <type>OAccessibleImplementationAccess</type>, or retrieving the implementation failed).
        */
        static OAccessibleImplementationAccess* getImplementation(
            const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext >& _rxComponent
        );


        /** sets the parent for a derived implementation

        @param _rxComponent
            is the component which's new parent should be set
        @param _rxNewParent
            is the new parent of the component
        @return
            <TRUE/> in case of success, <FALSE/> otherwise. For error condition please look at
            <method>getImplementation</method>.
        */
        static sal_Bool setAccessibleParent(
            const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext >& _rxComponent,
            const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& _rxNewParent
        );


    private:
        COMPHELPER_DLLPRIVATE static const ::com::sun::star::uno::Sequence< sal_Int8 > getUnoTunnelImplementationId();
    };

//.........................................................................
}   // namespace comphelper
//.........................................................................


#endif // COMPHELPER_ACCIMPLACCESS_HXX


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

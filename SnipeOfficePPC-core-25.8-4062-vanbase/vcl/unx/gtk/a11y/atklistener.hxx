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

#ifndef _ATK_LISTENER_HXX_
#define _ATK_LISTENER_HXX_

#include <com/sun/star/accessibility/XAccessibleEventListener.hpp>
#include <cppuhelper/implbase1.hxx>

#include <vector>

#include "atkwrapper.hxx"

typedef std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > > AccessibleVector;

class AtkListener : public ::cppu::WeakImplHelper1< ::com::sun::star::accessibility::XAccessibleEventListener >
{
public:
    AtkListener(AtkObjectWrapper * pWrapper);

    // XEventListener
    virtual void disposing( const ::com::sun::star::lang::EventObject& Source )
        throw (::com::sun::star::uno::RuntimeException);

    // XAccessibleEventListener
    virtual void notifyEvent( const ::com::sun::star::accessibility::AccessibleEventObject& aEvent )
        throw( ::com::sun::star::uno::RuntimeException );

    AtkObjectWrapper *mpWrapper;
    AccessibleVector m_aChildList;

private:

    virtual ~AtkListener();

    // Updates the child list held to provide the old IndexInParent on children_changed::remove
    void updateChildList(::com::sun::star::accessibility::XAccessibleContext* pContext);

    // Process CHILD_EVENT notifications with a new child added
    void handleChildAdded(
        const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext >& rxParent,
        const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible>& rxChild);

    // Process CHILD_EVENT notifications with a child removed
    void handleChildRemoved(
        const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext >& rxParent,
        const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible>& rxChild);

    // Process INVALIDATE_ALL_CHILDREN notification
    void handleInvalidateChildren(
        const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext >& rxParent);
};

#endif /* _ATK_LISTENER_HXX_ */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

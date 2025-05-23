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

#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/embed/XEmbeddedObject.hpp>
#include <com/sun/star/awt/XCallback.hpp>
#include <cppuhelper/implbase1.hxx>

#include <rtl/ref.hxx>

class OleEmbeddedObject;

#define OLECOMP_ONVIEWCHANGE    1
#define OLECOMP_ONCLOSE         2

class MainThreadNotificationRequest :  public cppu::WeakImplHelper1< com::sun::star::awt::XCallback >
{
    OleEmbeddedObject* m_pObject;
    ::com::sun::star::uno::WeakReference< ::com::sun::star::embed::XEmbeddedObject > m_xObject;

    sal_uInt16 m_nNotificationType;
    sal_uInt32 m_nAspect;

public:
    virtual void SAL_CALL notify (const com::sun::star::uno::Any& rUserData)
        throw (com::sun::star::uno::RuntimeException);
    MainThreadNotificationRequest( const ::rtl::Reference< OleEmbeddedObject >& xObj, sal_uInt16 nNotificationType, sal_uInt32 nAspect = 0 );
    ~MainThreadNotificationRequest();
};


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

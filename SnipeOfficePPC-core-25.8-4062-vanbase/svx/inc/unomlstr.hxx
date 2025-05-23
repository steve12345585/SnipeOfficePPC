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
#ifndef _SVX_UNOMLSTR_HXX
#define _SVX_UNOMLSTR_HXX

#include <com/sun/star/util/XModifyListener.hpp>

#include <cppuhelper/implbase1.hxx>

class SdrObject;

class SvxUnoShapeModifyListener : public ::cppu::WeakAggImplHelper1< ::com::sun::star::util::XModifyListener >
{
    SdrObject*  mpObj;

public:
    SvxUnoShapeModifyListener( SdrObject* pObj ) throw();
    virtual ~SvxUnoShapeModifyListener() throw();

    // ::com::sun::star::util::XModifyListener
    virtual void SAL_CALL modified(const ::com::sun::star::lang::EventObject& aEvent) throw(  ::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::lang::XEventListener
    virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source) throw(  ::com::sun::star::uno::RuntimeException);

    // internal
    void invalidate() throw();
};


#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

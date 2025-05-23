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

#ifndef _AQUA11YFOCUSLISTENER_HXX_
#define _AQUA11YFOCUSLISTENER_HXX_

#include <salhelper/refobj.hxx>

#include "aqua/keyboardfocuslistener.hxx"
#include "aqua/aquavcltypes.h"

class AquaA11yFocusListener :
    public KeyboardFocusListener,
    public salhelper::ReferenceObject
{
    id m_focusedObject;

    static rtl::Reference< AquaA11yFocusListener > theListener;

    AquaA11yFocusListener();
    virtual ~AquaA11yFocusListener() {};
public:

    static rtl::Reference< AquaA11yFocusListener > get();

    id getFocusedUIElement();

    // KeyboardFocusListener
    virtual void SAL_CALL focusedObjectChanged(const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& xAccessible);

    // rtl::IReference
    virtual oslInterlockedCount SAL_CALL acquire() SAL_THROW(());
    virtual oslInterlockedCount SAL_CALL release() SAL_THROW(());
};

#endif // _AQUA11YFOCUSLISTENER_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

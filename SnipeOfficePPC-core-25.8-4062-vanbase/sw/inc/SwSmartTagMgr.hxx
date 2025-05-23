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

#ifndef _SWSMARTTAGMGR_HXX
#define _SWSMARTTAGMGR_HXX

#include <svx/SmartTagMgr.hxx>


/*************************************************************************
 *                      class SwSmartTagMgr
 *
 * Wrapper for the svx SmartTagMgr
 *************************************************************************/

class SwSmartTagMgr : public SmartTagMgr
{
private:
    static SwSmartTagMgr* mpTheSwSmartTagMgr;

    SwSmartTagMgr( const rtl::OUString& rModuleName );
    virtual ~SwSmartTagMgr();

public:
    static SwSmartTagMgr& Get();

    // ::com::sun::star::util::XModifyListener
    virtual void SAL_CALL modified( const ::com::sun::star::lang::EventObject& aEvent ) throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::util::XChangesListener
      virtual void SAL_CALL changesOccurred( const ::com::sun::star::util::ChangesEvent& Event ) throw(::com::sun::star::uno::RuntimeException);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

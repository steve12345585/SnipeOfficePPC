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

#include "SwSmartTagMgr.hxx"

#include <docsh.hxx>
#include <osl/mutex.hxx>
#include <swmodule.hxx>
#include <vcl/svapp.hxx>

using namespace com::sun::star;
using namespace com::sun::star::uno;

SwSmartTagMgr* SwSmartTagMgr::mpTheSwSmartTagMgr = 0;

SwSmartTagMgr& SwSmartTagMgr::Get()
{
    if ( !mpTheSwSmartTagMgr )
    {
        mpTheSwSmartTagMgr = new SwSmartTagMgr( SwDocShell::Factory().GetModuleName() );
        mpTheSwSmartTagMgr->Init(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Writer")));
    }
    return *mpTheSwSmartTagMgr;
}

SwSmartTagMgr::SwSmartTagMgr( const rtl::OUString& rModuleName ) :
    SmartTagMgr( rModuleName )
{
}

SwSmartTagMgr::~SwSmartTagMgr()
{
}

void SwSmartTagMgr::modified( const lang::EventObject& rEO ) throw( RuntimeException )
{
    SolarMutexGuard aGuard;

    // Installed recognizers have changed. We remove all existing smart tags:
    SW_MOD()->CheckSpellChanges( false, true, true, true );

    SmartTagMgr::modified( rEO );
}

void SwSmartTagMgr::changesOccurred( const util::ChangesEvent& rEvent ) throw( RuntimeException)
{
    SolarMutexGuard aGuard;

    // Configuration has changed. We remove all existing smart tags:
    SW_MOD()->CheckSpellChanges( false, true, true, true );

    SmartTagMgr::changesOccurred( rEvent );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

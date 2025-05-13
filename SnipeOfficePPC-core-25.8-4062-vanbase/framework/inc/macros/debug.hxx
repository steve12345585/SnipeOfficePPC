/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#ifndef __FRAMEWORK_MACROS_DEBUG_HXX_
#define __FRAMEWORK_MACROS_DEBUG_HXX_

//*****************************************************************************************************************
//  Disable all feature of this file in produkt version!
//  But enable normal assertion handling (as messagebox) in normal debug version.
//  User can overwrite these adjustment with his own values! We will do it only if nothing is set.
//*****************************************************************************************************************

//-----------------------------------------------------------------------------------------------------------------
// => "personal debug code"
#if OSL_DEBUG_LEVEL > 1

    // Enable log mechanism for normal assertion and error handling.
    // Look for user decisions before!
    #ifndef ENABLE_LOGMECHANISM
        #define ENABLE_LOGMECHANISM
    #endif
    // Enable assertion handling himself AND additional warnings.
    // The default logtype is MESSAGEBOX.
    // see "assertion.hxx" for further informations
    #ifndef ENABLE_ASSERTIONS
        #define ENABLE_ASSERTIONS
    #endif
    #ifndef ENABLE_WARNINGS
        #define ENABLE_WARNINGS
    #endif

//-----------------------------------------------------------------------------------------------------------------
// => "non product"
#elif OSL_DEBUG_LEVEL > 0

    // Enable log mechanism for normal assertion and error handling.
    // Look for user decisions before!
    #ifndef ENABLE_LOGMECHANISM
        #define ENABLE_LOGMECHANISM
    #endif
    // Enable assertion handling himself.
    // The default logtype is MESSAGEBOX.
    // see "assertion.hxx" for further informations
    #ifndef ENABLE_ASSERTIONS
        #define ENABLE_ASSERTIONS
    #endif

//-----------------------------------------------------------------------------------------------------------------
// => "product" (OSL_DEBUG_LEVEL == 0)
#else

    #undef  ENABLE_LOGMECHANISM
    #undef  ENABLE_ASSERTIONS
    #undef  ENABLE_WARNINGS
    #undef  ENABLE_EVENTDEBUG
    #undef  ENABLE_REGISTRATIONDEBUG
    #undef  ENABLE_TARGETINGDEBUG
    #undef  ENABLE_MUTEXDEBUG
//    #undef  ENABLE_TIMEMEASURE
    #undef  ENABLE_MEMORYMEASURE
    #undef  ENABLE_FILTERDBG

#endif

//*****************************************************************************************************************
//  generic macros for logging
//*****************************************************************************************************************

#include <macros/debug/logmechanism.hxx>

//*****************************************************************************************************************
//  special macros for assertion handling
//*****************************************************************************************************************
#include <macros/debug/assertion.hxx>

//*****************************************************************************************************************
//  special macros for event handling
//*****************************************************************************************************************
#include <macros/debug/event.hxx>

//*****************************************************************************************************************
//  special macros to debug service registration
//*****************************************************************************************************************
#include <macros/debug/registration.hxx>

//*****************************************************************************************************************
//  special macros to debug targeting of frames
//*****************************************************************************************************************
#include <macros/debug/targeting.hxx>

//*****************************************************************************************************************
//  special macros to debug threading mechanism
//*****************************************************************************************************************
#include <macros/debug/mutex.hxx>

//*****************************************************************************************************************
//  special macros to debug our filter cache!
//*****************************************************************************************************************
#include <macros/debug/filterdbg.hxx>

#endif  // #ifndef __FRAMEWORK_MACROS_DEBUG_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#ifndef _OSL_INTERLOCK_H_
#define _OSL_INTERLOCK_H_

#include "sal/config.h"

#include "sal/saldllapi.h"
#include "sal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef sal_Int32 oslInterlockedCount;

/** Increments the count variable addressed by pCount.
    @param pCount Address of count variable
    @return The adjusted value of the count variable.
*/
SAL_DLLPUBLIC oslInterlockedCount SAL_CALL osl_incrementInterlockedCount(oslInterlockedCount* pCount);

/** Decrement the count variable addressed by pCount.
    @param pCount Address of count variable
    @return The adjusted value of the count variable.
*/
SAL_DLLPUBLIC oslInterlockedCount SAL_CALL osl_decrementInterlockedCount(oslInterlockedCount* pCount);


/// @cond INTERNAL

/** Increments the count variable addressed by p.

    @attention This functionality should only be used internally within
    LibreOffice.

    @param p Address of count variable
    @return The adjusted value of the count variable.

    @Since SnipeOffice 4.0
*/
#if defined( HAVE_GCC_BUILTIN_ATOMIC )
#    define osl_atomic_increment(p)  __sync_add_and_fetch((p), 1)
#else
#    define osl_atomic_increment(p) osl_incrementInterlockedCount((p))
#endif


/** Decrement the count variable addressed by p.

    @attention This functionality should only be used internally within
    LibreOffice.

    @param p Address of count variable
    @return The adjusted value of the count variable.

    @Since SnipeOffice 4.0
*/
#if defined( HAVE_GCC_BUILTIN_ATOMIC )
#    define osl_atomic_decrement(p) __sync_sub_and_fetch((p), 1)
#else
#    define osl_atomic_decrement(p) osl_decrementInterlockedCount((p))
#endif

/// @endcond

#ifdef __cplusplus
}
#endif


#endif  /* _OSL_INTERLOCK_H_ */


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

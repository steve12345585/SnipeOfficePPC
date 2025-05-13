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

#ifndef _VCL_IOS_COMMON_H
#define _VCL_IOS_COMMON_H

#include <sal/types.h>
#include <premac.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>
#include <UIKit/UIKit.h>
#include <postmac.h>
#include <tools/debug.hxx>

// CoreFoundation designers, in their wisdom, decided that CFRelease of NULL
// cause a Crash, yet few API can return NULL when asking for the creation
// of an object, which force us to peper the code with egly if construct everywhere
// and open the door to very nasty crash on rare occasion
// this macro hide the mess
#define SafeCFRelease(a) do { if(a) { CFRelease(a); (a)=NULL; } } while(false)


#define round_to_long(a) ((a) >= 0 ? ((long)((a) + 0.5)) : ((long)((a) - 0.5)))

#include "vcl/salgtype.hxx"

#endif/* _VCL_IOS_COMMON_H */

//#define msgs_debug(c,f,...)
//    fprintf(stderr, "%s::%s(%p:%04.4x)\n", this, #c, __func__, 0, __VA_ARGS__ )

#define msgs_debug(c,f,...)                                             \
    fprintf(stderr, "%s::%s(%p:%4.4u)" f "\n", #c, __func__, this, ((unsigned int)pthread_self() & 8191), ##__VA_ARGS__ );

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

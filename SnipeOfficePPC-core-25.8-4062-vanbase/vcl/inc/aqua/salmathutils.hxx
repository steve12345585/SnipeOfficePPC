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

#ifndef _SV_SALMATHUTILS_HXX
#define _SV_SALMATHUTILS_HXX

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------------------------------------------
//
//                            Structures
//
// ------------------------------------------------------------------

// LRectCoor is an abreviation for rectangular coordinates
// represented as long integers

struct LRectCoor
{
    long  x;
    long  y;
    long  z;
};

// ------------------------------------------------------------------
//
//                       Type Definitions
//
// ------------------------------------------------------------------

// LRectCoorVec is an abreviation for vectors in rectangular
// coordinates represented as long integers

typedef struct LRectCoor   LRectCoor;
typedef LRectCoor         *LRectCoorVector;
typedef LRectCoorVector   *LRectCoorTensor;

// ------------------------------------------------------------------
//
//                      Function Headers
//
// ------------------------------------------------------------------

void CSwap  ( char            &rX, char            &rY );
void UCSwap ( unsigned char   &rX, unsigned char   &rY );
void SSwap  ( short           &rX, short           &rY );
void USSwap ( unsigned short  &rX, unsigned short  &rY );
void LSwap  ( long            &rX, long            &rY );
void ULSwap ( unsigned long   &rX, unsigned long   &rY );

// ------------------------------------------------------------------

unsigned long  Euclidian2Norm ( const LRectCoorVector  pVec );

// ------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif  // _SV_SALMATHUTILS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#ifndef __EQNOLEFILEHDR_HXX__
#define __EQNOLEFILEHDR_HXX__

#include <sal/types.h>
#include <sot/storage.hxx>

class SvStorageStream;
class SotStorage;

#define EQNOLEFILEHDR_SIZE 28

class EQNOLEFILEHDR
{
public:
    EQNOLEFILEHDR() {}
    EQNOLEFILEHDR(sal_uInt32 nLenMTEF) : nCBHdr(0x1c),nVersion(0x20000),
        nCf(0xc1c6),nCBObject(nLenMTEF),nReserved1(0),nReserved2(0x0014F690),
        nReserved3(0x0014EBB4), nReserved4(0) {}

    sal_uInt16   nCBHdr;     // length of header, sizeof(EQNOLEFILEHDR) = 28
    sal_uInt32   nVersion;   // hiword = 2, loword = 0
    sal_uInt16   nCf;        // clipboard format ("MathType EF")
    sal_uInt32   nCBObject;  // length of MTEF data following this header
    sal_uInt32   nReserved1; // not used
    sal_uInt32   nReserved2; // not used
    sal_uInt32   nReserved3; // not used
    sal_uInt32   nReserved4; // not used

    inline void Read(SvStorageStream *pS)
    {
        *pS >> nCBHdr;
        *pS >> nVersion;
        *pS >> nCf;
        *pS >> nCBObject;
        *pS >> nReserved1;
        *pS >> nReserved2;
        *pS >> nReserved3;
        *pS >> nReserved4;
    }
    inline void Write(SvStorageStream *pS)
    {
        *pS << nCBHdr;
        *pS << nVersion;
        *pS << nCf;
        *pS << nCBObject;
        *pS << nReserved1;
        *pS << nReserved2;
        *pS << nReserved3;
        *pS << nReserved4;
    }
};

sal_Bool GetMathTypeVersion( SotStorage* pStor, sal_uInt8 &nVersion );

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

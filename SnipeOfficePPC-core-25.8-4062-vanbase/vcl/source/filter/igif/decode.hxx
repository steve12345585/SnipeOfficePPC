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

#ifndef _DECODE_HXX
#define _DECODE_HXX

#include <vcl/bmpacc.hxx>

struct GIFLZWTableEntry;

class GIFLZWDecompressor
{
    GIFLZWTableEntry*   pTable;
    HPBYTE              pOutBuf;
    HPBYTE              pOutBufData;
    HPBYTE              pBlockBuf;
    sal_uLong               nInputBitsBuf;
    sal_uInt16              nTableSize;
    sal_uInt16              nClearCode;
    sal_uInt16              nEOICode;
    sal_uInt16              nCodeSize;
    sal_uInt16              nOldCode;
    sal_uInt16              nOutBufDataLen;
    sal_uInt16              nInputBitsBufSize;
    sal_Bool                bEOIFound;
    sal_uInt8               nDataSize;
    sal_uInt8               nBlockBufSize;
    sal_uInt8               nBlockBufPos;

    void                AddToTable(sal_uInt16 nPrevCode, sal_uInt16 nCodeFirstData);
    sal_Bool                ProcessOneCode();


public:

                        GIFLZWDecompressor( sal_uInt8 cDataSize );
                        ~GIFLZWDecompressor();

    HPBYTE              DecompressBlock( HPBYTE pSrc, sal_uInt8 cBufSize, sal_uLong& rCount, sal_Bool& rEOI );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

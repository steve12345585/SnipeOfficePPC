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

#ifndef _LZWDECOM_HXX
#define _LZWDECOM_HXX

#include <tools/stream.hxx>

struct LZWTableEntry {
    sal_uInt16 nPrevCode;
    sal_uInt16 nDataCount;
    sal_uInt8 nData;
};

class LZWDecompressor {

public:

    LZWDecompressor();
    ~LZWDecompressor();

    void StartDecompression(SvStream & rIStream);

    sal_uLong Decompress(sal_uInt8 * pTarget, sal_uLong nMaxCount);
        // Liefert die Anzahl der geschriebenen Bytes, wenn < nMaxCount,
        // sind keine weiteren Daten zu entpacken, oder es ist ein
        // Fehler aufgetreten.

private:

    sal_uInt16 GetNextCode();
    void AddToTable(sal_uInt16 nPrevCode, sal_uInt16 nCodeFirstData);
    void DecompressSome();

    SvStream * pIStream;

    LZWTableEntry * pTable;
    sal_uInt16 nTableSize;

    sal_Bool bEOIFound, bInvert, bFirst;

    sal_uInt16 nOldCode;

    sal_uInt8 * pOutBuf;
    sal_uInt8 * pOutBufData;
    sal_uInt16 nOutBufDataLen;

    sal_uInt8 nInputBitsBuf;
    sal_uInt16 nInputBitsBufSize;
};


#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

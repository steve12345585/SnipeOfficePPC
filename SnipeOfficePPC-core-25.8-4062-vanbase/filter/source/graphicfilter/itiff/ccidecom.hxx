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

#ifndef _CCIDECOM_HXX
#define _CCIDECOM_HXX

#include <tools/stream.hxx>


#define CCI_OPTION_2D               1       // 2D-Komprimierung (statt 1D)
#define CCI_OPTION_EOL              2       // EOL-Codes am Ende jeder Zeile vorhanden
#define CCI_OPTION_BYTEALIGNEOL     4       // Fuellbits vor jedem EOL-Code, so dass
                                            // Ende von EOL auf Bytes aligend
#define CCI_OPTION_BYTEALIGNROW     8       // Rows beginnen immer auf Byte-Grenze
#define CCI_OPTION_INVERSEBITORDER  16

// Eintrag in eine Huffman-Tabelle:
struct CCIHuffmanTableEntry {
    sal_uInt16 nValue;    // Der Daten-Wert.
    sal_uInt16 nCode;     // Der Code durch den der Daten-Wert repraesentiert wird.
    sal_uInt16 nCodeBits; // Laenge des Codes in Bits.
};


// Eintrag in eine Hash-Tabelle zur schnellen Dekodierung
struct CCILookUpTableEntry {
    sal_uInt16 nValue;
    sal_uInt16 nCodeBits;
};


class CCIDecompressor {

public:

    CCIDecompressor( sal_uLong nOptions, sal_uInt32 nImageWidth );
    ~CCIDecompressor();

    void StartDecompression( SvStream & rIStream );

    sal_Bool DecompressScanline(sal_uInt8 * pTarget, sal_uLong nTargetBits );

private:

    void MakeLookUp(const CCIHuffmanTableEntry * pHufTab,
                    const CCIHuffmanTableEntry * pHufTabSave,
                    CCILookUpTableEntry * pLookUp,
                    sal_uInt16 nHuffmanTableSize,
                    sal_uInt16 nMaxCodeBits);

    sal_Bool ReadEOL( sal_uInt32 nMaxFillBits );

    sal_Bool Read2DTag();

    sal_uInt8 ReadBlackOrWhite();

    sal_uInt16 ReadCodeAndDecode(const CCILookUpTableEntry * pLookUp,
                             sal_uInt16 nMaxCodeBits);

    void FillBits(sal_uInt8 * pTarget, sal_uInt16 nTargetBits,
                  sal_uInt16 nBitPos, sal_uInt16 nNumBits,
                  sal_uInt8 nBlackOrWhite);

    sal_uInt16 CountBits(const sal_uInt8 * pData, sal_uInt16 nDataSizeBits,
                     sal_uInt16 nBitPos, sal_uInt8 nBlackOrWhite);

    void Read1DScanlineData(sal_uInt8 * pTarget, sal_uInt16 nTargetBits);

    void Read2DScanlineData(sal_uInt8 * pTarget, sal_uInt16 nTargetBits);

    sal_Bool bTableBad;

    sal_Bool bStatus;

    sal_uInt8* pByteSwap;

    SvStream * pIStream;

    sal_uInt32 nEOLCount;

    sal_uInt32 nWidth;

    sal_uLong nOptions;

    sal_Bool bFirstEOL;

    CCILookUpTableEntry * pWhiteLookUp;
    CCILookUpTableEntry * pBlackLookUp;
    CCILookUpTableEntry * p2DModeLookUp;
    CCILookUpTableEntry * pUncompLookUp;

    sal_uLong nInputBitsBuf;
    sal_uInt16 nInputBitsBufSize;

    sal_uInt8 * pLastLine;
    sal_uLong nLastLineSize;
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

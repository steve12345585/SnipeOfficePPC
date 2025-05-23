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

#ifndef _PSPRINT_PRINTERUTIL_HXX_
#define _PSPRINT_PRINTERUTIL_HXX_

#include "osl/file.hxx"

#include "rtl/ustring.hxx"
#include "rtl/string.hxx"
#include "rtl/tencinfo.h"
#include "rtl/textcvt.h"

#include <map>

namespace psp {

/*
 *  string convenience routines
 *  sizeof(pBuffer) must be at least 2 Bytes, 0x00 <= nValue <= 0xFF,
 *  effective buffer of get*ValueOf() is NOT NULL-terminated
 */
sal_Int32   getHexValueOf (sal_Int32 nValue, sal_Char* pBuffer);
sal_Int32   getAlignedHexValueOf (sal_Int32 nValue, sal_Char* pBuffer);
sal_Int32   getValueOf    (sal_Int32 nValue, sal_Char* pBuffer);
sal_Int32   appendStr     (const sal_Char* pSrc, sal_Char* pDst);

sal_Bool    WritePS (osl::File* pFile, const sal_Char* pString);
sal_Bool    WritePS (osl::File* pFile, const sal_Char* pString, sal_uInt64 nInLength);
sal_Bool    WritePS (osl::File* pFile, const rtl::OString &rString);
sal_Bool    WritePS (osl::File* pFile, const rtl::OUString &rString);

class ConverterFactory
{

public:
    ConverterFactory();
    ~ConverterFactory();
    rtl_UnicodeToTextConverter  Get (rtl_TextEncoding nEncoding);
    sal_Size                    Convert (const sal_Unicode *pText, int nTextLen,
                                         sal_uChar *pBuffer, sal_Size nBufferSize,
                                         rtl_TextEncoding nEncoding);
private:

    std::map< rtl_TextEncoding, rtl_UnicodeToTextConverter >        m_aConverters;
};

ConverterFactory& GetConverterFactory ();

}  /* namespace psp */

#endif /* _PSPRINT_PRINTERUTIL_HXX_ */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

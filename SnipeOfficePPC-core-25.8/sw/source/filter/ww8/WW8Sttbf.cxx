/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#include <iostream>
#include <dbgoutsw.hxx>
#include "WW8Sttbf.hxx"
#include "ww8scan.hxx"
#include <cstdio>
#include <osl/endian.h>
#include <rtl/ustrbuf.hxx>

#if OSL_DEBUG_LEVEL > 1
#include <stdio.h>
#endif

namespace ww8
{
    WW8Struct::WW8Struct(SvStream& rSt, sal_uInt32 nPos, sal_uInt32 nSize)
        : mn_offset(0), mn_size(0)
    {
        if (checkSeek(rSt, nPos))
        {
            mp_data.reset(new sal_uInt8[nSize]);
            mn_size = rSt.Read(mp_data.get(), nSize);
        }
        OSL_ENSURE(mn_size == nSize, "short read in WW8Struct::WW8Struct");
    }

    WW8Struct::WW8Struct(WW8Struct * pStruct, sal_uInt32 nPos, sal_uInt32 nSize)
        : mp_data(pStruct->mp_data), mn_offset(pStruct->mn_offset + nPos)
        , mn_size(nSize)
    {
    }

    WW8Struct::~WW8Struct()
    {
    }

    sal_uInt8 WW8Struct::getU8(sal_uInt32 nOffset)
    {
        sal_uInt8 nResult = 0;

        if (nOffset < mn_size)
        {
            nResult = mp_data[mn_offset + nOffset];
        }

        return nResult;
    }

    ::rtl::OUString WW8Struct::getUString(sal_uInt32 nOffset,
                                          sal_uInt32 nCount)
    {
        ::rtl::OUString aResult;

        if (nCount > 0)
        {
            //clip to available
            sal_uInt32 nStartOff = mn_offset + nOffset;
            if (nStartOff >= mn_size)
                return aResult;
            sal_uInt32 nAvailable = (mn_size - nStartOff)/sizeof(sal_Unicode);
            if (nCount > nAvailable)
                nCount = nAvailable;
#if defined OSL_LITENDIAN
            aResult = rtl::OUString(reinterpret_cast<const sal_Unicode *>(
                mp_data.get() + nStartOff), nCount);
#else
            rtl::OUStringBuffer aBuf;
            for (sal_uInt32 i = 0; i < nCount; ++i)
                aBuf.append(static_cast<sal_Unicode>(getU16(nStartOff+i*2)));
            aResult = aBuf.makeStringAndClear();
#endif
        }

#if OSL_DEBUG_LEVEL > 1
        char sBuffer[256];
        snprintf(sBuffer, sizeof(sBuffer), "offset=\"%" SAL_PRIuUINT32 "\" count=\"%" SAL_PRIuUINT32 "\"",
                 nOffset, nCount);
        ::std::clog << "<WW8Struct-getUString" << sBuffer << ">"
                    << rtl::OUStringToOString( aResult, RTL_TEXTENCODING_UTF8 ).getStr() << "</WW8Struct-getUString>"
                    << ::std::endl;
#endif

        return aResult;

    }
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

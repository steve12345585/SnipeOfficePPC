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

#ifndef _ADRPARSE_HXX
#define _ADRPARSE_HXX

#include "svl/svldllapi.h"
#include <tools/string.hxx>
#include <vector>

//============================================================================
struct SvAddressEntry_Impl
{
    rtl::OUString m_aAddrSpec;
    rtl::OUString m_aRealName;

    SvAddressEntry_Impl()
    {
    }

    SvAddressEntry_Impl(const rtl::OUString& rTheAddrSpec,
                        const rtl::OUString& rTheRealName)
        : m_aAddrSpec(rTheAddrSpec)
        , m_aRealName(rTheRealName)
    {
    }
};

//============================================================================
typedef ::std::vector< SvAddressEntry_Impl* > SvAddressList_Impl;

//============================================================================
class SVL_DLLPUBLIC SvAddressParser
{
    friend class SvAddressParser_Impl;

    SvAddressEntry_Impl m_aFirst;
    SvAddressList_Impl m_aRest;
    bool m_bHasFirst;

public:
    SvAddressParser(const rtl::OUString& rInput);

    ~SvAddressParser();

    sal_Int32 Count() const { return m_bHasFirst ? m_aRest.size() + 1 : 0; }

    const rtl::OUString& GetEmailAddress(sal_Int32 nIndex) const
    {
        return nIndex == 0 ? m_aFirst.m_aAddrSpec :
                             m_aRest[ nIndex - 1 ]->m_aAddrSpec;
    }

    const rtl::OUString& GetRealName(sal_Int32 nIndex) const
    {
        return nIndex == 0 ? m_aFirst.m_aRealName :
                             m_aRest[ nIndex - 1 ]->m_aRealName;
    }
};

#endif // _ADRPARSE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

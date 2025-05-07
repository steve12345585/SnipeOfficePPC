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

#ifndef _COMPHELPER_PROPERTYINFOHASH_HXX_
#define _COMPHELPER_PROPERTYINFOHASH_HXX_

#include <rtl/ustring.hxx>
#include <comphelper/TypeGeneration.hxx>
#include <boost/unordered_map.hpp>
namespace comphelper
{
    struct PropertyInfo
    {
        const sal_Char* mpName;
        sal_uInt16 mnNameLen;
        sal_Int32 mnHandle;
        CppuTypes meCppuType;
        sal_Int16 mnAttributes;
        sal_uInt8 mnMemberId;
    };
    struct PropertyData
    {
        sal_uInt8 mnMapId;
        PropertyInfo *mpInfo;
        PropertyData ( sal_uInt8 nMapId, PropertyInfo *pInfo )
        : mnMapId ( nMapId )
        , mpInfo ( pInfo ) {}
    };
    struct eqFunc
    {
        sal_Bool operator()( const rtl::OUString &r1,
                             const rtl::OUString &r2) const
        {
            return r1 == r2;
        }
    };
}

typedef boost::unordered_map < ::rtl::OUString,
                        ::comphelper::PropertyInfo*,
                        ::rtl::OUStringHash,
                        ::comphelper::eqFunc > PropertyInfoHash;
typedef boost::unordered_map < ::rtl::OUString,
                        ::comphelper::PropertyData*,
                        ::rtl::OUStringHash,
                        ::comphelper::eqFunc > PropertyDataHash;
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

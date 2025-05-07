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


#include "codemaker/dependencies.hxx"

#include "codemaker/typemanager.hxx"
#include "codemaker/unotype.hxx"

#include "osl/diagnose.h"
#include "registry/reader.hxx"
#include "rtl/string.hxx"
#include "rtl/textcvt.h"
#include "rtl/textenc.h"
#include "rtl/ustring.hxx"
#include "sal/types.h"

#include <vector>

using codemaker::Dependencies;

namespace {

struct Bad {};

}

Dependencies::Dependencies(
    TypeManager const & manager, rtl::OString const & type):
    m_voidDependency(false), m_booleanDependency(false),
    m_byteDependency(false), m_shortDependency(false),
    m_unsignedShortDependency(false), m_longDependency(false),
    m_unsignedLongDependency(false), m_hyperDependency(false),
    m_unsignedHyperDependency(false), m_floatDependency(false),
    m_doubleDependency(false), m_charDependency(false),
    m_stringDependency(false), m_typeDependency(false), m_anyDependency(false),
    m_sequenceDependency(false)
{
    typereg::Reader reader(manager.getTypeReader(type));
    m_valid = reader.isValid();
    if (m_valid) {
        // Not everything is checked for consistency, just things that are cheap
        // to test:
        try {
            RTTypeClass tc = reader.getTypeClass();
            if (tc != RT_TYPE_SERVICE) {
                for (sal_Int16 i = 0; i < reader.getSuperTypeCount(); ++i) {
                    insert(reader.getSuperTypeName(i), true);
                }
            }
            if (tc != RT_TYPE_ENUM) {
                {for (sal_Int16 i = 0; i < reader.getFieldCount(); ++i) {
                    if ((reader.getFieldFlags(i) & RT_ACCESS_PARAMETERIZED_TYPE)
                        == 0)
                    {
                        insert(reader.getFieldTypeName(i), false);
                    }
                }}
            }
            for (sal_Int16 i = 0; i < reader.getMethodCount(); ++i) {
                insert(reader.getMethodReturnTypeName(i), false);
                for (sal_Int16 j = 0; j < reader.getMethodParameterCount(i);
                      ++j)
                {
                    if ((reader.getMethodParameterFlags(i, j) & RT_PARAM_REST)
                        != 0)
                    {
                        m_sequenceDependency = true;
                    }
                    insert(reader.getMethodParameterTypeName(i, j), false);
                }
                for (sal_Int16 j = 0; j < reader.getMethodExceptionCount(i);
                      ++j)
                {
                    insert(reader.getMethodExceptionTypeName(i, j), false);
                }
            }
            for (sal_Int16 i = 0; i < reader.getReferenceCount(); ++i) {
                if (reader.getReferenceSort(i) != RT_REF_TYPE_PARAMETER) {
                    insert(reader.getReferenceTypeName(i), false);
                }
            }
        } catch (Bad &) {
            m_map.clear();
            m_valid = false;
            m_voidDependency = false;
            m_booleanDependency = false;
            m_byteDependency = false;
            m_shortDependency = false;
            m_unsignedShortDependency = false;
            m_longDependency = false;
            m_unsignedLongDependency = false;
            m_hyperDependency = false;
            m_unsignedHyperDependency = false;
            m_floatDependency = false;
            m_doubleDependency = false;
            m_charDependency = false;
            m_stringDependency = false;
            m_typeDependency = false;
            m_anyDependency = false;
            m_sequenceDependency = false;
        }
    }
}

Dependencies::~Dependencies()
{}

void Dependencies::insert(rtl::OUString const & type, bool base) {
    rtl::OString t;
    if (!type.convertToString(
            &t, RTL_TEXTENCODING_UTF8,
            (RTL_UNICODETOTEXT_FLAGS_UNDEFINED_ERROR
             | RTL_UNICODETOTEXT_FLAGS_INVALID_ERROR)))
    {
        throw Bad();
    }
    insert(t, base);
}

void Dependencies::insert(rtl::OString const & type, bool base) {
    sal_Int32 rank;
    std::vector< rtl::OString > args;
    rtl::OString t(UnoType::decompose(type, &rank, &args));
    if (rank > 0) {
        m_sequenceDependency = true;
    }
    switch (UnoType::getSort(t)) {
    case UnoType::SORT_VOID:
        if (rank != 0 || !args.empty()) {
            throw Bad();
        }
        m_voidDependency = true;
        break;

    case UnoType::SORT_BOOLEAN:
        if (!args.empty()) {
            throw Bad();
        }
        m_booleanDependency = true;
        break;

    case UnoType::SORT_BYTE:
        if (!args.empty()) {
            throw Bad();
        }
        m_byteDependency = true;
        break;

    case UnoType::SORT_SHORT:
        if (!args.empty()) {
            throw Bad();
        }
        m_shortDependency = true;
        break;

    case UnoType::SORT_UNSIGNED_SHORT:
        if (!args.empty()) {
            throw Bad();
        }
        m_unsignedShortDependency = true;
        break;

    case UnoType::SORT_LONG:
        if (!args.empty()) {
            throw Bad();
        }
        m_longDependency = true;
        break;

    case UnoType::SORT_UNSIGNED_LONG:
        if (!args.empty()) {
            throw Bad();
        }
        m_unsignedLongDependency = true;
        break;

    case UnoType::SORT_HYPER:
        if (!args.empty()) {
            throw Bad();
        }
        m_hyperDependency = true;
        break;

    case UnoType::SORT_UNSIGNED_HYPER:
        if (!args.empty()) {
            throw Bad();
        }
        m_unsignedHyperDependency = true;
        break;

    case UnoType::SORT_FLOAT:
        if (!args.empty()) {
            throw Bad();
        }
        m_floatDependency = true;
        break;

    case UnoType::SORT_DOUBLE:
        if (!args.empty()) {
            throw Bad();
        }
        m_doubleDependency = true;
        break;

    case UnoType::SORT_CHAR:
        if (!args.empty()) {
            throw Bad();
        }
        m_charDependency = true;
        break;

    case UnoType::SORT_STRING:
        if (!args.empty()) {
            throw Bad();
        }
        m_stringDependency = true;
        break;

    case UnoType::SORT_TYPE:
        if (!args.empty()) {
            throw Bad();
        }
        m_typeDependency = true;
        break;

    case UnoType::SORT_ANY:
        if (!args.empty()) {
            throw Bad();
        }
        m_anyDependency = true;
        break;

    case UnoType::SORT_COMPLEX:
        {
            {for (std::vector< rtl::OString >::iterator i(args.begin());
                  i != args.end(); ++i)
            {
                insert(*i, false);
            }}
            Map::iterator i(m_map.find(t));
            if (i == m_map.end()) {
                m_map.insert(
                    Map::value_type(t, base ? KIND_BASE : KIND_NO_BASE));
            } else if (base) {
                i->second = KIND_BASE;
            }
            break;
        }

    default:
        OSL_ASSERT(false);
        break;
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

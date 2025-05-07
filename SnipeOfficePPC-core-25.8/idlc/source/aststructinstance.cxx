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


#include "idlc/aststructinstance.hxx"

#include "idlc/asttype.hxx"
#include "idlc/idlctypes.hxx"

#include "rtl/strbuf.hxx"
#include "rtl/string.hxx"

namespace {

rtl::OString createName(
    AstType const * typeTemplate, DeclList const * typeArguments)
{
    rtl::OStringBuffer buf(typeTemplate->getScopedName());
    if (typeArguments != 0) {
        buf.append('<');
        for (DeclList::const_iterator i(typeArguments->begin());
             i != typeArguments->end(); ++i)
        {
            if (i != typeArguments->begin()) {
                buf.append(',');
            }
            if (*i != 0) {
                buf.append((*i)->getScopedName());
            }
        }
        buf.append('>');
    }
    return buf.makeStringAndClear();
}

}

AstStructInstance::AstStructInstance(
    AstType const * typeTemplate, DeclList const * typeArguments,
    AstScope * scope):
    AstType(
        NT_instantiated_struct, createName(typeTemplate, typeArguments), scope),
    m_typeTemplate(typeTemplate), m_typeArguments(*typeArguments)
{}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

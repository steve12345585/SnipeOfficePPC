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
#ifndef _IDLC_ASTATTRIBUTE_HXX_
#define _IDLC_ASTATTRIBUTE_HXX_

#include <idlc/astdeclaration.hxx>
#include "idlc/astscope.hxx"

#include "registry/types.h"
#include "rtl/ustring.hxx"

namespace typereg { class Writer; }

class AstAttribute: public AstDeclaration, public AstScope {
public:
    AstAttribute(
        sal_uInt32 flags, AstType const * type, rtl::OString const & name,
        AstScope * scope):
        AstDeclaration(NT_attribute, name, scope),
        AstScope(NT_attribute), m_flags(flags), m_pType(type)
    {}

    AstAttribute(NodeType nodeType, sal_uInt32 flags, AstType const * pType, const ::rtl::OString& name, AstScope* pScope)
        : AstDeclaration(nodeType, name, pScope), AstScope(nodeType)
        , m_flags(flags)
        , m_pType(pType)
        {}
    virtual ~AstAttribute() {}

    void setExceptions(
        rtl::OUString const * getDoc, DeclList const * getExc,
        rtl::OUString const * setDoc, DeclList const * setExc)
    {
        if (getDoc != 0) {
            m_getDocumentation = *getDoc;
        }
        if (getExc != 0) {
            m_getExceptions = *getExc;
        }
        if (setDoc != 0) {
            m_setDocumentation = *setDoc;
        }
        if (setExc != 0) {
            m_setExceptions = *setExc;
        }
    }

    DeclList::size_type getGetExceptionCount() const
    { return m_getExceptions.size(); }

    DeclList::size_type getSetExceptionCount() const
    { return m_setExceptions.size(); }

    AstType const * getType() const
        { return m_pType; }
    sal_Bool isReadonly() const
        { return ((m_flags & AF_READONLY) == AF_READONLY); }
    sal_Bool isOptional() const
        { return ((m_flags & AF_OPTIONAL) == AF_OPTIONAL); }
    sal_Bool isAttribute() const
        { return ((m_flags & AF_ATTRIBUTE) == AF_ATTRIBUTE); }
    sal_Bool isProperty() const
        { return ((m_flags & AF_PROPERTY) == AF_PROPERTY); }
    sal_Bool isBound() const
        { return ((m_flags & AF_BOUND) == AF_BOUND); }
    sal_Bool isMayBeVoid() const
        { return ((m_flags & AF_MAYBEVOID) == AF_MAYBEVOID); }
    sal_Bool isConstrained() const
        { return ((m_flags & AF_CONSTRAINED) == AF_CONSTRAINED); }
    sal_Bool isTransient() const
        { return ((m_flags & AF_TRANSIENT) == AF_TRANSIENT); }
    sal_Bool isMayBeAmbiguous() const
        { return ((m_flags & AF_MAYBEAMBIGUOUS) == AF_MAYBEAMBIGUOUS); }
    sal_Bool isMayBeDefault() const
        { return ((m_flags & AF_MAYBEDEFAULT) == AF_MAYBEDEFAULT); }
    sal_Bool isRemoveable() const
        { return ((m_flags & AF_REMOVEABLE) == AF_REMOVEABLE); }

    sal_Bool dumpBlob(
        typereg::Writer & rBlob, sal_uInt16 index, sal_uInt16 * methodIndex);

private:
    void dumpExceptions(
        typereg::Writer & writer, rtl::OUString const & documentation,
        DeclList const & exceptions, RTMethodMode flags,
        sal_uInt16 * methodIndex);

    const sal_uInt32    m_flags;
    AstType const * m_pType;
    rtl::OUString m_getDocumentation;
    DeclList m_getExceptions;
    rtl::OUString m_setDocumentation;
    DeclList m_setExceptions;
};

#endif // _IDLC_ASTATTRIBUTE_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

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

#ifndef INCLUDED_idlc_inc_idlc_aststructinstance_hxx
#define INCLUDED_idlc_inc_idlc_aststructinstance_hxx

#include "idlc/asttype.hxx"
#include "idlc/idlctypes.hxx"

class AstDeclaration;
class AstScope;

class AstStructInstance: public AstType {
public:
    AstStructInstance(
        AstType const * typeTemplate, DeclList const * typeArguments,
        AstScope * scope);

    AstType const * getTypeTemplate() const { return m_typeTemplate; }

    DeclList::const_iterator getTypeArgumentsBegin() const
    { return m_typeArguments.begin(); }

    DeclList::const_iterator getTypeArgumentsEnd() const
    { return m_typeArguments.end(); }

private:
    AstType const * m_typeTemplate;
    DeclList m_typeArguments;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
